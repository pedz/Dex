
/* @(#)dex.h	1.4 */

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

#ifdef _LONG_LONG
typedef unsigned long long ularge_t;	/* Largest unsigned thing on system */
typedef long long large_t;		/* Largest signed thing on system */
#else
typedef unsigned long ularge_t;		/* Largest unsigned thing on system */
typedef long large_t;			/* Largest signed thing on system */
#endif

int is_zero(char *s);
int is_neg(char *s);
ularge_t atoularge(char *s);
large_t atolarge(char *);
