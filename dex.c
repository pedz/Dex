static char sccs_id[] = "@(#)dex.c	1.5";

#include <stdio.h>
#include <strings.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/access.h>
#include "map.h"
#include "sym.h"
#include "inter.h"
#include "dex.h"

char *progname;
char *dumpname = "/dev/mem";
char *unixname = "/unix";
void *stack_top;
extern int GRAMdebug;

static void super_extra_hack(char *argv0)
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
	printf("Found myself\n");
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
	printf("Found myself\n");
	break;
    }
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
    extra_hack();
    super_extra_hack(argv0);
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
