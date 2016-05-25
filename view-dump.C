/*
 * These routines manage a system dump file.
 */

static char sccsid[] = "@(#)view-dump.C	1.4";

#include <stdio.h>
#include <sys/dump.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/param.h>
#include <sys/seg.h>
#include <sys/mman.h>
#include <string.h>
#include "cdt.h"

char *progname;
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

#endif

int open_dump(char *path)
{
    struct stat64 stat_buf;
    int dump_fd;
    class CDT *cdt;
    class CDT_32   cdt_32;
    class CDT_64   cdt_64;
    class CDT_vr   cdt_vr;
    class CDT_u32  cdt_u32;
    class CDT_u64  cdt_u64;
    class CDT_ras  cdt_ras;
    class CDT_uras cdt_uras;
    char *cur_pos;
    char *dump_file;
    char *dump_file_end;

    if ((dump_fd = open64(path, O_RDONLY)) < 0) {
	perror(path);
	return 1;
    }
    if (fstat64(dump_fd, &stat_buf) < 0) {
	perror("stat");
	return 1;
    }
    dump_file = (char *)mmap64((void *)0, stat_buf.st_size, PROT_READ,
			       MAP_VARIABLE|MAP_FILE, dump_fd, 0);
    if ((long)dump_file == -1L) {
	perror("mmap");
	return 1;
    }
    dump_file_end = dump_file + stat_buf.st_size;
    cur_pos = dump_file;

    if (!VALID_CDT_MAGIC_32_64(((struct cdt *)cur_pos)))
	cur_pos += 512;

    while (cur_pos < dump_file_end) {
	switch (((struct cdt *)cur_pos)->cdt_magic) {
	case DMP_MAGIC_32:
	    cdt_32.header_setup(cur_pos);
	    cdt = & cdt_32;
	    break;

	case DMP_MAGIC_VR:
	    cdt_vr.header_setup(cur_pos);
	    cdt = & cdt_vr;
	    break;

	case DMP_MAGIC_64:
	    cdt_64.header_setup(cur_pos);
	    cdt = & cdt_64;
	    break;

	case DMP_MAGIC_U32:
	    cdt_u32.header_setup(cur_pos);
	    cdt = & cdt_u32;
	    break;

	case DMP_MAGIC_U64:
	    cdt_u64.header_setup(cur_pos);
	    cdt = & cdt_u64;
	    break;

	case DMP_MAGIC_RAS:
	    cdt_ras.header_setup(cur_pos);
	    cdt = & cdt_ras;
	    break;
	    
	case DMP_MAGIC_RAS_U:
	    cdt_uras.header_setup(cur_pos);
	    cdt = & cdt_uras;
	    break;

	case DMP_MAGIC_UD32:
	case DMP_MAGIC_UD64:
	    fprintf(stderr, "Out of sync with magic equal to %0*lx\n",
		    sizeof(long)*2, ((struct cdt *)cur_pos)->cdt_magic);
	    goto loop_end;
	case DMP_MAGIC_END:
	    goto loop_end;

	default:
	    fprintf(stderr, "Exiting with magic equal to %0*x\n",
		    sizeof(int)*2, ((struct cdt *)cur_pos)->cdt_magic);
	    {
		int *ip = (int *)cur_pos;

		printf("cur_pos = %0*lx %0*lx\n",
		       sizeof(long)*2, cur_pos, sizeof(long)*2, (cur_pos - dump_file));
		for (int i = -8*4; i < 8*4; i += 4) {
		    for (int j = 0; j < 4; ++j)
			printf("%c%0*x", i == 0 && j == 0 ? '*' : ' ',
			       sizeof(int)*2, ip[i+j]);
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
	    }
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
#endif
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
	}

    }
loop_end:
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
