
/* @(#)dex.h	1.6 */

#ifndef __DEX_H
#define __DEX_H

/* globals */
extern char *progname;
extern char *dumpname;
extern char *unixname;
extern void *stack_top;
extern unsigned long debug_mask;

void *safe_malloc(size_t size, char *file, int lineno);
void *safe_realloc(void *old, size_t size, size_t old_size, char *file, int lineno);
#define smalloc(size) (safe_malloc((size), __FILE__, __LINE__))
#define srealloc(old, size, osize) \
	(safe_realloc((old), (size),(osize),  __FILE__, __LINE__))
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
    QUICK_RMAP_BIT      = 0x00100000
};

char *print_field(int size, ...);

#define P(f) print_field(sizeof(f), (f))
#define DEBUG_PRINTF(b) ((debug_mask & DEBUG_BIT) && printf b)

#endif /* __DEX_H */
