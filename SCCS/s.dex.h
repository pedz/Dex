h21272
s 00003/00002/00062
d D 1.7 10/08/23 17:01:53 pedzan 7 6
e
s 00040/00003/00024
d D 1.6 02/03/14 16:12:31 pedz 6 5
e
s 00000/00000/00027
d D 1.5 00/09/18 17:56:59 pedz 5 4
c Checking before V5 conversion
e
s 00013/00000/00014
d D 1.4 98/10/23 12:26:08 pedz 4 3
e
s 00005/00003/00009
d D 1.3 97/05/13 16:02:56 pedz 3 2
c Check pointer
e
s 00005/00001/00007
d D 1.2 95/02/01 10:36:48 pedz 2 1
e
s 00008/00000/00000
d D 1.1 94/08/22 17:56:32 pedz 1 0
c date and time created 94/08/22 17:56:32 by pedz
e
u
U
t
T
I 1

/* %W% */

I 6
#ifndef __DEX_H
#define __DEX_H

E 6
/* globals */
D 2
char *progname;
E 2
I 2
extern char *progname;
extern char *dumpname;
extern char *unixname;
extern void *stack_top;
E 2
D 3
void *smalloc(int size, char *file, int lineno);
void *srealloc(void *old, int size, char *file, int lineno);
#define new(arg) ((arg *)smalloc(sizeof(arg), __FILE__, __LINE__))
E 3
I 3
D 6
void *safe_malloc(int size, char *file, int lineno);
void *safe_realloc(void *old, int size, char *file, int lineno);
E 6
I 6
extern unsigned long debug_mask;

void *safe_malloc(size_t size, char *file, int lineno);
void *safe_realloc(void *old, size_t size, size_t old_size, char *file, int lineno);
E 6
#define smalloc(size) (safe_malloc((size), __FILE__, __LINE__))
D 6
#define srealloc(old, size) (safe_realloc((old), (size), __FILE__, __LINE__))
E 6
I 6
#define srealloc(old, size, osize) \
	(safe_realloc((old), (size),(osize),  __FILE__, __LINE__))
E 6
#define new(arg) ((arg *)smalloc(sizeof(arg)))
E 3
I 2
#define A_SIZE(arg) (sizeof(arg) / sizeof((arg)[0]))
I 4

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
I 6

enum debug_bits {
    BASE_C_BIT          = 0x00000001,
    BASE_EXPR_C_BIT     = 0x00000002,
    BINARY_EXPR_C_BIT   = 0x00000004,
    BUILTINS_C_BIT      = 0x00000008,
    CAST_EXPR_C_BIT     = 0x00000010,
    DEX_C_BIT           = 0x00000020,
    DISASM_C_BIT        = 0x00000040,
    FCALL_C_BIT         = 0x00000080,
    GRAM_Y_BIT          = 0x00000100,
    LOAD_C_BIT          = 0x00000200,
    MAP_C_BIT           = 0x00000400,
    PRE_ASGN_EXPR_C_BIT = 0x00000800,
    PRINT_C_BIT         = 0x00001000,
    SCAN_L_BIT          = 0x00002000,
    STAB_Y_BIT          = 0x00004000,
    STMT_C_BIT          = 0x00008000,
    SYM_C_BIT           = 0x00010000,
    TREE_C_BIT          = 0x00020000,
    TREE_DUMP_C_BIT     = 0x00040000,
    UNARY_EXPR_C_BIT    = 0x00080000,
D 7
    QUICK_RMAP_BIT      = 0x00100000
E 7
I 7
    QUICK_RMAP_BIT      = 0x00100000,
    MMAP_BIT            = 0x00200000
E 7
};

D 7
char *print_field(int size, ...);
E 7
I 7
extern char *print_field(int size, ...);
E 7

#define P(f) print_field(sizeof(f), (f))
#define DEBUG_PRINTF(b) ((debug_mask & DEBUG_BIT) && printf b)

#endif /* __DEX_H */
E 6
E 4
E 2
E 1
