/*
 * These routines manage a system dump file.
 */

static char sccsid[] = "@(#)view-dump.C	1.2";

#include <stdio.h>
#include <sys/dump.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/param.h>
#include <sys/seg.h>
#include <sys/mman.h>
#include <string.h>

char *progname;
char *cur_pos;
char *dump_file;
char *dump_file_end;
int total_pages;
int page_entries;

/*
 * This code was developed for version 5.  In order to get it to work
 * on version 4, we had to define these items.
 */
#ifndef VALID_CDT_MAGIC_32_64

#define VALID_CDT_MAGIC_32_64(cp) VALID_CDT_MAGIC(cp)
#define NUM_ENTRIES_32_64(cp)     NUM_ENTRIES(cp)
#define cdt_entry32 cdt_entry
#define SKIP_64_BIT

#endif

void showbits(char *name, size_t sizeof_name,
	      u_longlong_t addr, size_t sizeof_addr,
	      u_longlong_t segval, size_t sizeof_segval,
	      size_t len, int isreal)
{
    unsigned long bit_entries = BITMAPSIZE(addr, len);
    unsigned long pages = NPAGES(addr, len);
    bitmap_t *bitmap = (bitmap_t *)cur_pos;
    char *new_line_format;
    int pages_per_line;
    int col = 79;
    int bit;
    int in_mapped;

    col -=  printf("%*.*s", sizeof_name, sizeof_name, name);
    col -= printf(" %0*llx-%0*llx", sizeof_addr*2, addr,
		  sizeof_addr*2, addr+len);
    if (isreal)
	col -= printf(" r");
    else
	col -= printf(" %0*llx", sizeof_segval*2, segval);

    if (pages > col) {
	col = 0;			/* force a new line */
	if (isreal || sizeof_addr == 8) {
	    new_line_format = "\n%016llx%*s";
	    pages_per_line = 32;
	} else {
	    new_line_format = "\n%08llx%*s";
	    pages_per_line = 64;
	}
    } else {
	col -= printf(" ");
    }

    cur_pos += (bit_entries * sizeof(bitmap_t));
    if (cur_pos > dump_file_end) {
	fprintf(stderr, "%s is empty for short dump\n", name);
	exit(1);
    }

    in_mapped = 0;
    for (bit = 0; bit < pages; ++bit) {
	int size = PAGESIZE - (addr % PAGESIZE);

	if (size > len)
	    size = len;

	if (col == 0) {
	    col = pages_per_line - ((addr / PAGESIZE) % pages_per_line);
	    printf(new_line_format, addr,
		   (int)(1 + ((addr / PAGESIZE) % pages_per_line)),
		   "");
	}

	/*
	 * Page is in the dump
	 */
	if (ISBITMAP(bitmap, bit)) {
	    if ((cur_pos += size) > dump_file_end) {
		fprintf(stderr, "%s is not complete\n", name);
		exit(1);
	    }
	    putc('x', stdout);
	    ++total_pages;
	    --col;
	    if (!in_mapped || size != PAGESIZE)
		++page_entries;
	    in_mapped = size == PAGESIZE;
	} else {
	    putc('.', stdout);
	    --col;
	    in_mapped = 0;
	}
	addr += size;
	len -= size;
    }
    if (in_mapped)
	++page_entries;
    putc('\n', stdout);
}

/*
 * I don't remember this but it appears as if the general purpose code
 * that is able to read 32 and 64 bit dumps can only be built in 32
 * bit mode.
 */
#ifdef __64BIT__
#define MY_NUM_ENTRIES(cp) NUM_ENTRIES(cp)
#else
#define MY_NUM_ENTRIES(cp) NUM_ENTRIES_32_64(cp)
#endif

int open_dump(char *path)
{
    struct stat64 stat_buf;
    int dump_fd;

    if ((dump_fd = open64(path, O_RDONLY)) < 0) {
	perror(path);
	return 1;
    }
    if (fstat64(dump_fd, &stat_buf) < 0) {
	perror("stat");
	return 1;
    }
    dump_file = mmap64((void *)0, stat_buf.st_size, PROT_READ,
		       MAP_VARIABLE|MAP_FILE, dump_fd, 0);
    if ((long)dump_file == -1L) {
	perror("mmap");
	return 1;
    }
    dump_file_end = dump_file + stat_buf.st_size;

    cur_pos = dump_file;
    if (!VALID_CDT_MAGIC_32_64(((struct cdt *)cur_pos)))
	cur_pos += 512;
    while (cur_pos < dump_file_end &&
	   VALID_CDT_MAGIC_32_64(((struct cdt *)cur_pos))) {
	int num_entries = MY_NUM_ENTRIES((struct cdt *)cur_pos);
	struct cdt *cdt = (struct cdt *)cur_pos;
	size_t len;
	int index;

	printf("%.*s %s %d\n", sizeof(cdt->cdt_name), cdt->cdt_name,
	       ((cdt->cdt_magic == DMP_MAGIC_32) ? "32" :
		((cdt->cdt_magic == DMP_MAGIC_VR) ? "vr" :
		 ((cdt->cdt_magic == DMP_MAGIC_64) ? "64" :
		  "??"))), num_entries);

	if ((cur_pos += cdt->cdt_len) > dump_file_end) {
	    fprintf(stderr, "%s has incomplete cdt's\n", cdt->cdt_name);
	    return 1;
	}

	switch (cdt->cdt_magic) {
#ifndef __64BIT__
	case DMP_MAGIC_VR:
	    for (index = 0; index < num_entries; ++index) {
		struct cdt_entry_vr *e;
		unsigned long long addr;

		e = ((struct cdt_entry_vr *)cdt->cdt_entry) + index;
		if (e->d_addrflags & D_REAL)
		    showbits(e->d_name, sizeof(e->d_name),
			     (u_longlong_t)e->d_realaddr,
			     sizeof(e->d_realaddr),
			     (u_longlong_t)0,
			     0,
			     e->d_len, 1);
		else
		    showbits(e->d_name, sizeof(e->d_name),
			     (u_longlong_t)e->d_ptr_v,
			     sizeof(e->d_ptr_v),
			     (u_longlong_t)e->d_segval_v,
			     sizeof(e->d_segval_v),
			     e->d_len, 0);
	    }
	    break;

	case DMP_MAGIC_32:
	    for (index = 0; index < num_entries; ++index) {
		struct cdt_entry32 *e;
		unsigned long addr;

		e = (struct cdt_entry32 *)cdt->cdt_entry + index;
		addr = (unsigned long)e->d_ptr;
		len = e->d_len;
		showbits(e->d_name, sizeof(e->d_name),
			 (u_longlong_t)e->d_ptr,
			 sizeof(e->d_ptr),
			 (u_longlong_t)e->d_segval,
			 sizeof(e->d_segval),
			 e->d_len, 0);
	    }
	    break;
#endif
#ifndef SKIP_64_BIT
	case DMP_MAGIC_64:
	    for (index = 0; index < num_entries; ++index) {
		struct cdt_entry64 *e;
		unsigned long long addr;

		e = (struct cdt_entry64 *)cdt->cdt_entry + index;
		addr = (unsigned long)e->d_ptr;
		len = e->d_len;
		showbits(e->d_name, sizeof(e->d_name),
			 (u_longlong_t)e->d_ptr,
			 sizeof(e->d_ptr),
			 (u_longlong_t)e->__d_segval,
			 sizeof(e->__d_segval),
			 e->d_len, e->__d_segval == DUMP_REAL_SEGVAL);
	    }
	    break;
#endif
	default:
	    fprintf(stderr, "Unknown cdt_magic of %08x\n", cdt->cdt_magic);
	    exit(1);
	}
    }
    return 0;
}

int main(int argc, char *argv[])
{
    int rc;

    if (progname = rindex(argv[0], '/'))
	++progname;
    else
	progname = argv[0];

    rc = open_dump(argv[1]);
    printf("Total pages: %d\nPage entries: %d\n", total_pages, page_entries);
    return rc;
}
