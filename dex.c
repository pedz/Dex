static char sccs_id[] = "@(#)dex.c	1.1";

#include <stdio.h>
#include <strings.h>
#include <stdlib.h>
#include "map.h"
#include "sym.h"
#include "tree.h"
#include "inter.h"

/*
 * Arguments:
 *  0) name of program
 *  1) symtab file
 *  2) struct to print out
 *  3) address
 *  4) (optional) path to dump
 */

char *progname;
extern int GRAMdebug;

main(int argc, char *argv[])
{
    typeptr t;
    char *s;
    int fd;
    int rflag;
    int addr;

    if (progname = rindex(argv[0], '/'))
	++progname;
    else
	progname = argv[0];

    GRAMdebug = (argc > 1 && !strcmp(argv[1], "-d"));
    ns_inter = ns_create(progname);
    printf("go!\n");
    fflush(stdout);
    tree_init();
    GRAMparse();
    return 0;
}

void *smalloc(int size, char *file, int lineno)
{
    char *ret;

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
