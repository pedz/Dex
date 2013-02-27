h34031
s 00000/00000/00013
d D 1.2 00/09/18 17:57:01 pedz 2 1
c Checking before V5 conversion
e
s 00013/00000/00000
d D 1.1 95/02/01 10:57:37 pedz 1 0
c date and time created 95/02/01 10:57:37 by pedz
e
u
U
t
T
I 1

/* %W% */

/*
 * This is the set of globals needed between the scanner in scan.l and
 * the parser in gram.y.
 */

/*
 * The name of the name space will be the same as progname which is
 * created in dex.c (and declared in dex.h)
 */
extern ns *ns_inter;
E 1
