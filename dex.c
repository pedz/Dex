static char sccs_id[] = "@(#)dex.c	1.11";

#include <stdio.h>
#include <strings.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/access.h>
#include <ctype.h>
#include <stdarg.h>
#include "map.h"
#include "sym.h"
#include "inter.h"
#include "dex.h"
#include "load.h"
#include "tree.h"
#include "builtins.h"
#include "fcall.h"
#include "gram_pre.h"
#include "stmt.h"
#include "scan_pre.h"

#define DEBUG_BIT DEX_C_BIT

char *progname;
static char *dumpname;
static char *unixname;
void *stack_top;
unsigned long debug_mask;
extern int GRAMdebug;
extern int STABdebug;
int interactive;

#include <setjmp.h>
#include "lib/getopt.h"

static jmp_buf GRAMcatch;

void sigint_catch(int sig)
{
    longjmp(GRAMcatch, 1);
}

static int usage(void)
{
    fprintf(stderr, "Usage: %s [options] [src_file_list]\n", progname);
    fprintf(stderr, "        -D, --debug mask     Set debug mask\n");
    fprintf(stderr, "        -d, --dump dumpfile  Specify the dump file\n");
    fprintf(stderr, "        -u, --unix unixfile  Specify the unix file\n");
    fprintf(stderr, "        src_file_list        List of source files\n");
    return 1;
}

extern FILE *yyin;

int main(int argc, char *argv[])
{
    typeptr t;
    char *s;
    int fd;
    int rflag;
    int addr;
    char *argv0 = argv[0];

    if (progname = rindex(argv[0], '/')) /* figure out the program name */
	++progname;
    else
	progname = argv[0];

    while (1) {
	int c;
	int option_index;
	static struct option long_options[] = {
	    { "debug",       required_argument, 0, 'D' },
	    { "dump",        required_argument, 0, 'd' },
	    { "help",        no_argument,       0, '?' },
	    { "interactive", no_argument,       0, 'i' },
	    { "unix",        required_argument, 0, 'u' },
	    { 0, 0, 0, 0}
	};
	
	if ((c = getopt_long(argc, argv, "D:d:u:?", long_options, &option_index)) == -1)
	    break;
	switch (c) {
	case 'i':
	    interactive = 1;
	    break;
	    
	case 'd':
	    dumpname = optarg;
	    break;
	    
	case 'D':
	    debug_mask = strtoul(optarg, (char **)0, 0);
	    GRAMdebug = debug_mask & GRAM_Y_BIT;
	    STABdebug = debug_mask & STAB_Y_BIT;
	    break;
	    
	case 'u':
	    unixname = optarg;
	    break;

	case '?':
	default:
	    return(usage());
	}
    }

    if (debug_mask & DEBUG_BIT) {
	if (dumpname)
	    printf("dumpname = %s\n", dumpname);
	if (unixname)
	    printf("unixname = %s\n", unixname);
	if (debug_mask)
	    printf("debug_mask = %#018lx\n", debug_mask);
	while (optind < argc)
	    printf("source %s\n", argv[optind++]);
    }

    if (unixname && load(unixname, 0, 0))
	return 1;		/* no error message? */
    
    ns_inter = ns_create((ns *)0, progname);
    load_base_types(ns_inter);
    tree_init();

    if (map_init(dumpname))
	return 1;

    DEBUG_PRINTF(("main: map_init done\n"));
    builtin_init();
    if (setup_pseudos())
	return 1;
    
    while (optind < argc) {
	if ((yyin = fopen(argv[optind], "r")) == 0) {
	    fprintf(stderr, "%s: %s not found\n", progname, argv[optind]);
	    exit(1);
	}
	optind++;
	GRAMparse();
    }

    if (interactive)
	yyin = stdin;
	while (1) {
	    signal(SIGINT, sigint_catch);
	    if (setjmp(GRAMcatch) == 0) {
		GRAMparse();
		break;
	    }
	}
    return 0;
}

void *safe_malloc(size_t size, char *file, int lineno)
{
    char *ret;

    /* printf("malloc of %d from %s:%d\n", size, file, lineno); */
    if (!(ret = malloc(size)) && size) {
	fprintf(stderr, "%s: Out of memory in %s: line %d for size %ld\n",
		progname, file, lineno, size);
	exit(1);
    }
#if 0
    fprintf(stderr, "%s:%d allocated %ld\n", file, lineno, size);
#endif
    bzero(ret, size);
    return ret;
}

void *safe_realloc(void *old, size_t size, size_t old_size, char *file, int lineno)
{
    char *ret;

    if (!(ret = realloc(old, size))) {
	fprintf(stderr, "%s: Out of memory in %s: line %d for size %ld\n",
		progname, file, lineno, size);
	exit(1);
    }
#if 0
    fprintf(stderr, "%s:%d reallocated %ld to %ld\n", file, lineno,
	    old_size, size);
#endif
    if (size > old_size)
	bzero(ret + old_size, size - old_size);
    return ret;
}

/*
 * Returns true if the digit string pointed to by s is either null or has
 * nothing but 0's.  We know the string is a valid "integer" string.
 */
int is_zero(char *s)
{
    int c;

    if (!s)
	return 1;
    while ((c = *s++) && (c == '0' || c == '-' || c == '+'));
    if (!c)
	return 1;
    return 0;
}

/*
 * Returns true if s is non-null and the first character is '-'
 */
int is_neg(char *s)
{
    return s && (*s == '-');
}

#ifdef _LONG_LONG
ularge_t atoularge(char *s) { return strtoull(s, (char **)0, 0); }
large_t  atolarge (char *s) { return strtoll (s, (char **)0, 0); }
#else
ularge_t atoularge(char *s) { return strtoul(s, (char **)0, 0); }
large_t  atolarge (char *s) { return strtol (s, (char **)0, 0); }
#endif

char *print_field(int size, ...)
{
    static char print_buf[22*32];
    static char *ptr = print_buf;
    char *ret = ptr;
    va_list Argp;
    /*
     * These are unsigned because I don't want sign extension when
     * they are promoted to int's and passed to sprintf.
     */
    unsigned char ac;
    unsigned short as;
    unsigned int ai;
    unsigned long long al;

    va_start(Argp, size);

    switch (size) {
    case sizeof(char):
	ac = va_arg(Argp, unsigned char);
	sprintf(ptr, "%#0*x", size*2 + 2, ac);
	break;
    case sizeof(short):
	as = va_arg(Argp, unsigned short);
	sprintf(ptr, "%#0*x", size*2 + 2, as);
	break;
    case sizeof(int):
	ai = va_arg(Argp, unsigned int);
	sprintf(ptr, "%#0*x", size*2 + 2, ai);
	break;
    case sizeof(long long):
	al = va_arg(Argp, unsigned long long);
	sprintf(ptr, "%#0*llx", size*2 + 2, al);
	break;
    default:
	fprintf(stderr, "Bad size in print_field %d\n", size);
	exit(1);
    }
    ptr += 22;
    if ((ptr + 22) > (print_buf + sizeof(print_buf)))
	ptr = print_buf;
    return ret;
}
