
/* @(#)inter.h	1.1 */

/*
 * This is the set of globals needed between the scanner in scan.l and
 * the parser in gram.y.
 */

/*
 * The name of the name space will be the same as progname which is
 * created in dex.c (and declared in dex.h)
 */
extern ns *ns_inter;
