static char sccs_id[] = "@(#)dex.c	1.7";

#include <stdio.h>
#include <strings.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/access.h>
#include <ctype.h>
#include "map.h"
#include "sym.h"
#include "inter.h"
#include "dex.h"

char *progname;
char *dumpname = "/dev/mem";
char *unixname = "/unix";
void *stack_top;
extern int GRAMdebug;

static void retrieve_own_stabs(char *argv0)
{
    char buf[10240];
    char *path;
    struct stat sbuf;
    
    /*
     * First check whatever argv[0] points to.
     */
    if (!(stat(argv0, &sbuf) || !S_ISREG(sbuf.st_mode) || access(argv0, X_OK))) {
	/* We found it */
	load(argv0, -1, -1);
	return;
    }

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
	return;
    }
    printf("Did not find myself\n");
    return;
}

main(int argc, char *argv[])
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

    if (GRAMdebug = (argc && !strcmp(argv[0], "-d"))) {	/* optional -d flag */
	--argc;
	++argv;
    }

    if (argc) {				/* Optional first arg is the dump name */
	dumpname = argv[0];
	++argv;
	--argc;
    }

    if (argc) {				/* Optional second arg is /unix name */
	unixname = argv[0];
	load(unixname, 0, 0);
	++argv;
	--argc;
    }

    ns_inter = ns_create((ns *)0, progname);
    load_base_types(ns_inter);
    tree_init();
    map_init();
    builtin_init();
    retrieve_own_stabs(argv0);
    GRAMparse();
    return 0;
}

void *safe_malloc(int size, char *file, int lineno)
{
    char *ret;

    /* printf("malloc of %d from %s:%d\n", size, file, lineno); */
    if (!(ret = malloc(size)) && size) {
	fprintf(stderr, "Out of memory in %s: line %d\n", file, lineno);
	exit(1);
    }
    return ret;
}

void *safe_realloc(void *old, int size, char *file, int lineno)
{
    char *ret;

    if (!(ret = realloc(old, size))) {
	fprintf(stderr, "Out of memory in %s: line %d\n", file, lineno);
	exit(1);
    }
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
    while ((c = *s++) && (isdigit(c) || c == '-' || c == '+'));
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
