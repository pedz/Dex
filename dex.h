
/* @(#)dex.h	1.3 */

/* globals */
extern char *progname;
extern char *dumpname;
extern char *unixname;
extern void *stack_top;
void *safe_malloc(int size, char *file, int lineno);
void *safe_realloc(void *old, int size, char *file, int lineno);
#define smalloc(size) (safe_malloc((size), __FILE__, __LINE__))
#define srealloc(old, size) (safe_realloc((old), (size), __FILE__, __LINE__))
#define new(arg) ((arg *)smalloc(sizeof(arg)))
#define A_SIZE(arg) (sizeof(arg) / sizeof((arg)[0]))
