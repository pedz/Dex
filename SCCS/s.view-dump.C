h39170
s 00008/00007/00265
d D 1.4 11/08/03 19:14:03 pedzan 4 3
c Fixed some printf's for 64 bit longs.
e
s 00190/00170/00082
d D 1.3 10/08/23 17:49:03 pedzan 3 2
c Massive change to C++ implementation
e
s 00012/00007/00240
d D 1.2 02/11/20 14:40:49 pedz 2 1
c Last check in before class changes
e
s 00247/00000/00000
d D 1.1 02/03/14 16:45:54 pedz 1 0
c date and time created 02/03/14 16:45:54 by pedz
e
u
U
t
T
I 1
/*
 * These routines manage a system dump file.
 */

static char sccsid[] = "%W%";

#include <stdio.h>
#include <sys/dump.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/param.h>
#include <sys/seg.h>
#include <sys/mman.h>
#include <string.h>
I 3
#include "cdt.h"
E 3

char *progname;
D 3
char *cur_pos;
char *dump_file;
char *dump_file_end;
E 3
int total_pages;
D 2
int page_bobs;
E 2
I 2
int page_entries;
E 2

/*
 * This code was developed for version 5.  In order to get it to work
 * on version 4, we had to define these items.
 */
#ifndef VALID_CDT_MAGIC_32_64

#define VALID_CDT_MAGIC_32_64(cp) VALID_CDT_MAGIC(cp)
#define NUM_ENTRIES_32_64(cp)     NUM_ENTRIES(cp)
#define cdt_entry32 cdt_entry
D 3
#define SKIP_64_BIT
E 3

#endif

D 3
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
D 2
	    if (!in_mapped)
		++page_bobs;
	    in_mapped = 1;
E 2
I 2
	    if (!in_mapped || size != PAGESIZE)
		++page_entries;
	    in_mapped = size == PAGESIZE;
E 2
	} else {
	    putc('.', stdout);
	    --col;
	    in_mapped = 0;
	}
	addr += size;
	len -= size;
    }
I 2
    if (in_mapped)
	++page_entries;
E 2
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

E 3
int open_dump(char *path)
{
    struct stat64 stat_buf;
    int dump_fd;
I 3
    class CDT *cdt;
    class CDT_32 cdt_32;
    class CDT_64 cdt_64;
    class CDT_vr cdt_vr;
    class CDT_u32 cdt_u32;
    class CDT_u64 cdt_u64;
    char *cur_pos;
    char *dump_file;
    char *dump_file_end;
E 3

D 2
    if ((dump_fd = open64(path, O_RDWR)) < 0) {
E 2
I 2
    if ((dump_fd = open64(path, O_RDONLY)) < 0) {
E 2
	perror(path);
	return 1;
    }
    if (fstat64(dump_fd, &stat_buf) < 0) {
	perror("stat");
	return 1;
    }
D 2
    dump_file = mmap64((void *)0, stat_buf.st_size, PROT_READ|PROT_WRITE,
E 2
I 2
D 3
    dump_file = mmap64((void *)0, stat_buf.st_size, PROT_READ,
E 2
		       MAP_VARIABLE|MAP_FILE, dump_fd, 0);
E 3
I 3
    dump_file = (char *)mmap64((void *)0, stat_buf.st_size, PROT_READ,
			       MAP_VARIABLE|MAP_FILE, dump_fd, 0);
E 3
    if ((long)dump_file == -1L) {
	perror("mmap");
	return 1;
    }
    dump_file_end = dump_file + stat_buf.st_size;
D 3

E 3
    cur_pos = dump_file;
I 3

E 3
    if (!VALID_CDT_MAGIC_32_64(((struct cdt *)cur_pos)))
	cur_pos += 512;
D 3
    while (cur_pos < dump_file_end &&
	   VALID_CDT_MAGIC_32_64(((struct cdt *)cur_pos))) {
	int num_entries = MY_NUM_ENTRIES((struct cdt *)cur_pos);
	struct cdt *cdt = (struct cdt *)cur_pos;
	size_t len;
	int index;
E 3

D 3
	printf("%.*s %s %d\n", sizeof(cdt->cdt_name), cdt->cdt_name,
	       ((cdt->cdt_magic == DMP_MAGIC_32) ? "32" :
		((cdt->cdt_magic == DMP_MAGIC_VR) ? "vr" :
		 ((cdt->cdt_magic == DMP_MAGIC_64) ? "64" :
		  "??"))), num_entries);
E 3
I 3
    while (cur_pos < dump_file_end) {
	switch (((struct cdt *)cur_pos)->cdt_magic) {
	case DMP_MAGIC_32:
	    cdt_32.header_setup(cur_pos);
	    cdt = & cdt_32;
	    break;
E 3

D 3
	if ((cur_pos += cdt->cdt_len) > dump_file_end) {
	    fprintf(stderr, "%s has incomplete cdt's\n", cdt->cdt_name);
	    return 1;
	}

	switch (cdt->cdt_magic) {
#ifndef __64BIT__
E 3
	case DMP_MAGIC_VR:
D 3
	    for (index = 0; index < num_entries; ++index) {
		struct cdt_entry_vr *e;
		unsigned long long addr;
E 3
I 3
	    cdt_vr.header_setup(cur_pos);
	    cdt = & cdt_vr;
	    break;
E 3

D 3
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
E 3
I 3
	case DMP_MAGIC_64:
	    cdt_64.header_setup(cur_pos);
	    cdt = & cdt_64;
E 3
	    break;

D 3
	case DMP_MAGIC_32:
	    for (index = 0; index < num_entries; ++index) {
		struct cdt_entry32 *e;
		unsigned long addr;
E 3
I 3
	case DMP_MAGIC_U32:
	    cdt_u32.header_setup(cur_pos);
	    cdt = & cdt_u32;
	    break;
E 3

D 3
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
E 3
I 3
	case DMP_MAGIC_U64:
	    cdt_u64.header_setup(cur_pos);
	    cdt = & cdt_u64;
E 3
	    break;
D 3
#endif
#ifndef SKIP_64_BIT
	case DMP_MAGIC_64:
	    for (index = 0; index < num_entries; ++index) {
		struct cdt_entry64 *e;
		unsigned long long addr;
E 3

D 3
		e = (struct cdt_entry64 *)cdt->cdt_entry + index;
		addr = (unsigned long)e->d_ptr;
		len = e->d_len;
		showbits(e->d_name, sizeof(e->d_name),
			 (u_longlong_t)e->d_ptr,
			 sizeof(e->d_ptr),
			 (u_longlong_t)e->__d_segval,
			 sizeof(e->__d_segval),
			 e->d_len, e->__d_segval == DUMP_REAL_SEGVAL);
E 3
I 3
	case DMP_MAGIC_UD32:
	case DMP_MAGIC_UD64:
D 4
	    fprintf(stderr, "Out of sync with magic equal to %08x\n",
		    ((struct cdt *)cur_pos)->cdt_magic);
E 4
I 4
	    fprintf(stderr, "Out of sync with magic equal to %0*lx\n",
		    sizeof(long)*2, ((struct cdt *)cur_pos)->cdt_magic);
E 4
	    goto loop_end;
	case DMP_MAGIC_END:
	    goto loop_end;

	default:
D 4
	    fprintf(stderr, "Exiting with magic equal to %08x\n",
		    ((struct cdt *)cur_pos)->cdt_magic);
E 4
I 4
	    fprintf(stderr, "Exiting with magic equal to %0*lx\n",
		    sizeof(long)*2, ((struct cdt *)cur_pos)->cdt_magic);
E 4
	    {
		int *ip = (int *)cur_pos;

D 4
		printf("cur_pos = %08x %08x\n", cur_pos, cur_pos - dump_file);
E 4
I 4
		printf("cur_pos = %0*lx %0*lx\n",
		       sizeof(long)*2, cur_pos, sizeof(long)*2, (cur_pos - dump_file));
E 4
		for (int i = -8*4; i < 8*4; i += 4) {
		    for (int j = 0; j < 4; ++j)
D 4
			printf("%c%08x", i == 0 && j == 0 ? '*' : ' ',
			       ip[i+j]);
E 4
I 4
			printf("%c%0*lx", i == 0 && j == 0 ? '*' : ' ',
			       sizeof(long)*2, ip[i+j]);
E 4
		    printf("    ");
		    for (int j = 0; j < 4; ++j) {
			unsigned int v = ip[i+j];
			for (int k = 4; --k >= 0; v <<= 8) {
			    unsigned char c = ((v >> 24) & 0xff);
			    if ((c & 0x80) || (c < ' ') || (c == 0x7f))
				c = '.';
			    printf("%c", c);
			}
		    }
		    printf("\n");
		}
E 3
	    }
D 3
	    break;
E 3
I 3
	    goto loop_end;
	}
	
	printf("%.*s %s %d\n", cdt->header_name_size(), cdt->header_name(),
	       cdt->header_type_string(), cdt->header_num_entries());

	/*
	 * A "group" is a sequence of entries followed by the matching
	 * sequence of bit and data pairs.  There are an unknown
	 * number of groups per header.  This loop will effectively
	 * walk through the entries of the new "unlimited" tables.
	 * The old style will have one group per header.  After
	 * first_group and next_group, cur_pos will point to the first
	 * bitmap for the first entry of that group.
	 */
	for (cur_pos = cdt->first_group(cur_pos);
	     cdt->more_groups();
	     cur_pos = cdt->next_group(cur_pos)) {

	    /*
	     * This loop will go through the entries in a group.  The
	     * old dump style entries have N entries per group
	     * (computed from d_len).  The new unlimited style always
	     * has one entry per group.  But rather than use an index
	     * method, we use the first, more, next style since it is
	     * more versatile.  We also pass in and set cur_pos for
	     * each of these but currently, they simply return what
	     * was passed in since there is nothing between the page
	     * data of one entry and the bitmap of the next entry.
	     */
	    for (cur_pos = cdt->first_entry(cur_pos);
		 cdt->more_entrys();
		 cur_pos = cdt->next_entry(cur_pos)) {

		char *name = cdt->entry_name();
		size_t sizeof_name = cdt->entry_name_size();
		u_longlong_t addr = (u_longlong_t) cdt->entry_addr();
		size_t sizeof_addr = cdt->entry_addr_size();
		u_longlong_t segval = cdt->entry_segval();
		size_t sizeof_segval = cdt->entry_segval_size();
		size_t len = cdt->entry_len();
		int isreal = cdt->entry_isreal();

#if 0
		unsigned long bit_entries = cdt->entry_bitmapsize(addr, len);
		unsigned long pages = cdt->entry_npages(addr, len);
#else
		unsigned long bit_entries = BITMAPSIZE(addr, len);
		unsigned long pages = NPAGES(addr, len);
E 3
#endif
I 2
D 3
	default:
	    fprintf(stderr, "Unknown cdt_magic of %08x\n", cdt->cdt_magic);
	    exit(1);
E 3
I 3
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
			col = pages_per_line -
			    ((addr / PAGESIZE) % pages_per_line);
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
E 3
E 2
	}
I 3

E 3
    }
I 3
loop_end:
E 3
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
D 2
    printf("Total pages: %d\nPage bobs: %d\n", total_pages, page_bobs);
E 2
I 2
    printf("Total pages: %d\nPage entries: %d\n", total_pages, page_entries);
E 2
    return rc;
}
E 1
