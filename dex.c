static char sccs_id[] = "@(#)dex.c	1.2";

#include <stdio.h>
#include <strings.h>
#include <stdlib.h>
#include "map.h"
#include "sym.h"
#include "inter.h"
#include "dex.h"

char *progname;
char *dumpname = "/dev/mem";
char *unixname = "/unix";
void *stack_top;
extern int GRAMdebug;

main(int argc, char *argv[])
{
    typeptr t;
    char *s;
    int fd;
    int rflag;
    int addr;

    if (progname = rindex(argv[0], '/')) /* figure out the program name */
	++progname;
    else
	progname = argv[0];
    --argc;
    ++argv;

    if (GRAMdebug = (argc && !strcmp(argv[1], "-d"))) {	/* optional -d flag */
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
	++argv;
	--argc;
    }

    ns_inter = ns_create((ns *)0, progname);
    load_base_types(ns_inter);
    extra_hack();
    printf("go!\n");
    fflush(stdout);
    tree_init();
    map_init();
    GRAMparse();
    return 0;
}

void *smalloc(int size, char *file, int lineno)
{
    char *ret;

    /* printf("malloc of %d from %s:%d\n", size, file, lineno); */
    if (!(ret = malloc(size)) && size) {
	fprintf(stderr, "Out of memory in %s: line %d\n", file, lineno);
	exit(1);
    }
    return ret;
}

void *srealloc(void *old, int size, char *file, int lineno)
{
    char *ret;

    if (!(ret = realloc(old, size))) {
	fprintf(stderr, "Out of memory in %s: line %d\n", file, lineno);
	exit(1);
    }
    return ret;
}
