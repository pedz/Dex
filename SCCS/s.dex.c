h44875
s 00020/00002/00199
d D 1.11 10/08/23 17:01:53 pedzan 11 10
e
s 00097/00014/00104
d D 1.10 02/03/14 16:12:31 pedz 10 9
e
s 00001/00009/00117
d D 1.9 00/09/18 17:56:59 pedz 9 8
c Checking before V5 conversion
e
s 00010/00033/00116
d D 1.8 00/05/29 15:09:06 pedz 8 7
c Just before IA64 conversion
e
s 00005/00006/00144
d D 1.7 00/02/08 17:36:47 pedz 7 6
e
s 00033/00000/00117
d D 1.6 98/10/23 12:26:08 pedz 6 5
e
s 00002/00002/00115
d D 1.5 97/05/13 16:02:55 pedz 5 4
c Check pointer
e
s 00012/00004/00105
d D 1.4 95/04/25 10:09:58 pedz 4 3
e
s 00032/00004/00077
d D 1.3 95/02/09 21:35:42 pedz 3 2
e
s 00029/00013/00052
d D 1.2 95/02/01 10:36:46 pedz 2 1
e
s 00065/00000/00000
d D 1.1 94/08/22 17:56:32 pedz 1 0
c date and time created 94/08/22 17:56:32 by pedz
e
u
U
t
T
I 1
static char sccs_id[] = "%W%";

#include <stdio.h>
#include <strings.h>
#include <stdlib.h>
I 3
#include <sys/stat.h>
#include <sys/access.h>
I 6
#include <ctype.h>
I 10
#include <stdarg.h>
E 10
E 6
E 3
#include "map.h"
#include "sym.h"
D 2
#include "tree.h"
E 2
#include "inter.h"
I 2
#include "dex.h"
I 8
#include "load.h"
I 10
#include "tree.h"
#include "builtins.h"
#include "fcall.h"
#include "gram_pre.h"
E 10
E 8
E 2

I 10
#define DEBUG_BIT DEX_C_BIT

E 10
D 2
/*
 * Arguments:
 *  0) name of program
 *  1) symtab file
 *  2) struct to print out
 *  3) address
 *  4) (optional) path to dump
 */

E 2
char *progname;
I 2
char *dumpname = "/dev/mem";
char *unixname = "/unix";
void *stack_top;
I 10
unsigned long debug_mask;
E 10
E 2
extern int GRAMdebug;
I 10
extern int STABdebug;
E 10

I 11
#include <setjmp.h>
static jmp_buf GRAMcatch;

void sigint_catch(int sig)
{
    longjmp(GRAMcatch, 1);
}

E 11
I 3
D 4
static void super_extra_hack(void)
E 4
I 4
D 7
static void super_extra_hack(char *argv0)
E 7
I 7
D 8
static void retrieve_own_stabs(char *argv0)
E 7
E 4
{
    char buf[10240];
    char *path;
    struct stat sbuf;
    
    /*
D 4
     * It seems that mmap can not map the current executable so this
     * code is not used.
E 4
I 4
     * First check whatever argv[0] points to.
E 4
     */
I 4
    if (!(stat(argv0, &sbuf) || !S_ISREG(sbuf.st_mode) || access(argv0, X_OK))) {
	/* We found it */
	load(argv0, -1, -1);
D 7
	printf("Found myself\n");
E 7
	return;
    }

E 4
    for (path = strtok(getenv("PATH"), ":");
	 path;
	 path = strtok((char *)0, ":")) {
	strcpy(buf, path);
	strcat(buf, "/");
	strcat(buf, progname);
	if (stat(buf, &sbuf) || !S_ISREG(sbuf.st_mode) || access(buf, X_OK))
	    continue;

	/* We found it */
	load(buf, -1, -1);
I 4
D 7
	printf("Found myself\n");
E 4
	break;
E 7
I 7
	return;
E 7
    }
I 7
    printf("Did not find myself\n");
E 7
    return;
}

E 8
E 3
D 10
main(int argc, char *argv[])
E 10
I 10
int main(int argc, char *argv[])
E 10
{
    typeptr t;
    char *s;
    int fd;
    int rflag;
    int addr;
I 4
    char *argv0 = argv[0];
E 4

D 2
    if (progname = rindex(argv[0], '/'))
E 2
I 2
    if (progname = rindex(argv[0], '/')) /* figure out the program name */
E 2
	++progname;
    else
	progname = argv[0];
I 2
    --argc;
    ++argv;
E 2

D 2
    GRAMdebug = (argc > 1 && !strcmp(argv[1], "-d"));
    ns_inter = ns_create(progname);
E 2
I 2
D 3
    if (GRAMdebug = (argc && !strcmp(argv[1], "-d"))) {	/* optional -d flag */
E 3
I 3
D 10
    if (GRAMdebug = (argc && !strcmp(argv[0], "-d"))) {	/* optional -d flag */
E 3
	--argc;
	++argv;
E 10
I 10
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
E 10
    }

I 8
D 9
    printf("Line %d\n", __LINE__);
E 9
E 8
D 10
    if (argc) {				/* Optional first arg is the dump name */
E 10
I 10
D 11
    if (argc) {				/* Optional first arg: dump */
E 11
I 11
    if (argc) {				/* First arg: dump */
E 11
E 10
	dumpname = argv[0];
	++argv;
	--argc;
I 10
    } else {
	fprintf(stderr, "Usage: %s [-d] dump [unix]\n", progname);
	return 1;
E 10
    }

I 8
D 9
    printf("Line %d\n", __LINE__);
E 9
E 8
D 10
    if (argc) {				/* Optional second arg is /unix name */
E 10
I 10
    if (argc) {				/* Optional second arg: unix */
E 10
	unixname = argv[0];
I 3
D 10
	load(unixname, 0, 0);
E 10
I 10
	if (load(unixname, 0, 0))
	    return 1;
E 10
E 3
	++argv;
	--argc;
    }

I 8
D 9
    printf("Line %d\n", __LINE__);
E 9
E 8
    ns_inter = ns_create((ns *)0, progname);
I 8
D 9
    printf("Line %d\n", __LINE__);
E 9
E 8
    load_base_types(ns_inter);
I 8
D 9
    printf("Line %d\n", __LINE__);
E 9
E 8
D 3
    extra_hack();
E 2
    printf("go!\n");
    fflush(stdout);
E 3
    tree_init();
I 8
D 9
    printf("Line %d\n", __LINE__);
E 9
E 8
I 2
D 10
    map_init();
E 10
I 10
    if (map_init())
	return 1;
    DEBUG_PRINTF(("main: map_init done\n"));
E 10
I 8
D 9
    printf("Line %d\n", __LINE__);
E 9
E 8
I 3
D 7
    extra_hack();
D 4
    /* super_extra_hack(); */
E 4
I 4
    super_extra_hack(argv0);
E 7
I 7
    builtin_init();
D 8
    retrieve_own_stabs(argv0);
E 8
I 8
D 9
    printf("Line %d\n", __LINE__);
E 9
I 9
D 10
    setup_pseudos();
E 10
I 10
    if (setup_pseudos())
	return 1;
E 10
E 9
E 8
E 7
E 4
E 3
E 2
D 11
    GRAMparse();
E 11
I 11
    while (1) {
	signal(SIGINT, sigint_catch);
	if (setjmp(GRAMcatch) == 0) {
	    GRAMparse();
	    break;
	}
    }
E 11
I 8
D 9
    printf("Line %d\n", __LINE__);
E 9
E 8
    return 0;
}

D 5
void *smalloc(int size, char *file, int lineno)
E 5
I 5
D 10
void *safe_malloc(int size, char *file, int lineno)
E 10
I 10
void *safe_malloc(size_t size, char *file, int lineno)
E 10
E 5
{
    char *ret;

I 2
    /* printf("malloc of %d from %s:%d\n", size, file, lineno); */
E 2
    if (!(ret = malloc(size)) && size) {
D 10
	fprintf(stderr, "Out of memory in %s: line %d\n", file, lineno);
E 10
I 10
	fprintf(stderr, "%s: Out of memory in %s: line %d for size %ld\n",
		progname, file, lineno, size);
E 10
	exit(1);
    }
I 11
#if 0
E 11
I 10
    fprintf(stderr, "%s:%d allocated %ld\n", file, lineno, size);
I 11
#endif
E 11
    bzero(ret, size);
E 10
    return ret;
}

D 5
void *srealloc(void *old, int size, char *file, int lineno)
E 5
I 5
D 10
void *safe_realloc(void *old, int size, char *file, int lineno)
E 10
I 10
void *safe_realloc(void *old, size_t size, size_t old_size, char *file, int lineno)
E 10
E 5
{
    char *ret;

    if (!(ret = realloc(old, size))) {
D 10
	fprintf(stderr, "Out of memory in %s: line %d\n", file, lineno);
E 10
I 10
	fprintf(stderr, "%s: Out of memory in %s: line %d for size %ld\n",
		progname, file, lineno, size);
E 10
	exit(1);
    }
I 11
#if 0
E 11
I 10
    fprintf(stderr, "%s:%d reallocated %ld to %ld\n", file, lineno,
	    old_size, size);
I 11
#endif
E 11
    if (size > old_size)
	bzero(ret + old_size, size - old_size);
E 10
    return ret;
}
I 6

/*
 * Returns true if the digit string pointed to by s is either null or has
 * nothing but 0's.  We know the string is a valid "integer" string.
 */
int is_zero(char *s)
{
    int c;

    if (!s)
	return 1;
D 10
    while ((c = *s++) && (isdigit(c) || c == '-' || c == '+'));
E 10
I 10
    while ((c = *s++) && (c == '0' || c == '-' || c == '+'));
E 10
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
I 10

char *print_field(int size, ...)
{
    static char print_buf[20*32];
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
	sprintf(ptr, "%0*x", size*2, ac);
	break;
    case sizeof(short):
	as = va_arg(Argp, unsigned short);
	sprintf(ptr, "%0*x", size*2, as);
	break;
    case sizeof(int):
	ai = va_arg(Argp, unsigned int);
	sprintf(ptr, "%0*x", size*2, ai);
	break;
    case sizeof(long long):
	al = va_arg(Argp, unsigned long long);
	sprintf(ptr, "%0*llx", size*2, al);
	break;
    default:
	fprintf(stderr, "Bad size in print_field %d\n", size);
	exit(1);
    }
    ptr += 20;
    if ((ptr + 20) > (print_buf + sizeof(print_buf)))
	ptr = print_buf;
    return ret;
}
E 10
E 6
E 1
