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

#define DEBUG_BIT DEX_C_BIT

char *progname;
char *dumpname = "/dev/mem";
char *unixname = "/unix";
void *stack_top;
unsigned long debug_mask;
extern int GRAMdebug;
extern int STABdebug;

#include <setjmp.h>
static jmp_buf GRAMcatch;

void sigint_catch(int sig)
{
    longjmp(GRAMcatch, 1);
}

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
    --argc;
    ++argv;

    /* optional -d flag */
    if (argc && !strncmp(argv[0], "-d", 2)) {
	if (strlen(argv[0]) > 2) {
	    debug_mask = strtoul(argv[0]+2, (char **)0, 0);
	    --argc;
	    ++argv;
	} else {
	    --argc;
	    ++argv;
	    if (debug_mask = strtoul(argv[0], (char **)0, 0)) {
		--argc;
		++argv;
	    } else {
		debug_mask = -1;
	    }
	}
	GRAMdebug = debug_mask & GRAM_Y_BIT;
	STABdebug = debug_mask & STAB_Y_BIT;
    }

    if (argc) {				/* First arg: dump */
	dumpname = argv[0];
	++argv;
	--argc;
    } else {
	fprintf(stderr, "Usage: %s [-d] dump [unix]\n", progname);
	return 1;
    }

    if (argc) {				/* Optional second arg: unix */
	unixname = argv[0];
	if (load(unixname, 0, 0))
	    return 1;
	++argv;
	--argc;
    }

    ns_inter = ns_create((ns *)0, progname);
    load_base_types(ns_inter);
    tree_init();
    if (map_init())
	return 1;
    DEBUG_PRINTF(("main: map_init done\n"));
    builtin_init();
    if (setup_pseudos())
	return 1;
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
