
/* @(#)dex.h	1.1 */

/* globals */
char *progname;
void *smalloc(int size, char *file, int lineno);
void *srealloc(void *old, int size, char *file, int lineno);
#define new(arg) ((arg *)smalloc(sizeof(arg), __FILE__, __LINE__))
