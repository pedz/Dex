
/* @(#)dex.h	1.2 */

/* globals */
extern char *progname;
extern char *dumpname;
extern char *unixname;
extern void *stack_top;
void *smalloc(int size, char *file, int lineno);
void *srealloc(void *old, int size, char *file, int lineno);
#define new(arg) ((arg *)smalloc(sizeof(arg), __FILE__, __LINE__))
#define A_SIZE(arg) (sizeof(arg) / sizeof((arg)[0]))
