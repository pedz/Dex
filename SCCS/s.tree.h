h02359
s 00004/00004/00208
d D 1.8 10/08/23 17:01:54 pedzan 8 7
e
s 00080/00056/00132
d D 1.7 02/03/14 16:12:35 pedz 7 6
e
s 00000/00000/00188
d D 1.6 00/09/18 17:57:03 pedz 6 5
c Checking before V5 conversion
e
s 00005/00001/00183
d D 1.5 00/05/29 15:09:09 pedz 5 4
c Just before IA64 conversion
e
s 00001/00000/00183
d D 1.4 97/05/13 16:03:01 pedz 4 3
c Check pointer
e
s 00017/00013/00166
d D 1.3 95/02/09 21:36:07 pedz 3 2
e
s 00022/00089/00157
d D 1.2 95/02/01 10:37:16 pedz 2 1
e
s 00246/00000/00000
d D 1.1 94/08/22 17:56:36 pedz 1 0
c date and time created 94/08/22 17:56:36 by pedz
e
u
U
t
T
I 1

/* %W% */

I 7
#ifndef __TREE_H
#define __TREE_H

#include "dex.h"			/* needed for large_t */
#include "sym.h"			/* needed for anode */

E 7
/*
D 2
 * This might be a hugh mistake but the convention is that single or
 * double letters will be used to denote all the types.  c for char,
 * uc for unsigned char, etc.  Functions will be prefixed with sc_ as
 * an example for a function which returns a char.
 *
 * Note that pointer type is never used in an expression tree.
 * Instead an unsigned long is used.  ptr_type is listed in expr_type
 * since it will simplify the parsing but it is purposely left out of
 * "all".
 */

/*
 * Keep FIRST_TYPE equal to the first expr_type and LAST_TYPE equal to
 * the last expr_type.
 */
enum expr_type {
    schar_type, uchar_type, int_type, uint_type, short_type,
    ushort_type, long_type, ulong_type, float_type, double_type,
    struct_type, void_type, bad_type
    };
#define FIRST_TYPE schar_type
#define LAST_TYPE  bad_type

/* Typedef for pointer type */
typedef char *ptr;

/* Pointer type as used in an expression tree */
#define ptr_expr unsigned long

/*
E 2
 * When a struct is evaulated, the address of the structure is
 * returned on the stack.
 */
D 8
typedef unsigned long st;
E 8
I 8
typedef unsigned long st_t;
E 8

/* This union can contain all of the types known to the interpreter */
typedef union all {
D 7
    signed char c;
E 7
I 7
    signed char sc;
E 7
    unsigned char uc;
    int i;
    unsigned int ui;
    long l;
    unsigned long ul;
I 7
    long long ll;
    unsigned long long ull;
E 7
    short s;
    unsigned short us;
    float f;
    double d;
D 8
    st st;
E 8
I 8
    st_t st;
E 8
} all;

I 7
typedef struct expr expr;

E 7
/* A node in the expression tree */
D 7
typedef struct expr {
E 7
I 7
struct expr {
E 7
    union all_func {
	/* set of function pointers for pvalues */
D 7
	signed char (*sc)();
	unsigned char (*uc)();
	int (*i)();
	unsigned int (*ui)();
	short (*s)();
	unsigned short (*us)();
	long (*l)();
	unsigned long (*ul)();
	float (*f)();
	double (*d)();
	st (*st)();
	void (*v)();
E 7
I 7
	signed char (*sc)(expr *);
	unsigned char (*uc)(expr *);
	int (*i)(expr *);
	unsigned int (*ui)(expr *);
	short (*s)(expr *);
	unsigned short (*us)(expr *);
	long (*l)(expr *);
	unsigned long (*ul)(expr *);
	long long (*ll)(expr *);
	unsigned long long (*ull)(expr *);
	float (*f)(expr *);
	double (*d)(expr *);
D 8
	st (*st)(expr *);
E 8
I 8
	st_t (*st)(expr *);
E 8
	void (*v)(expr *);
E 7

	/* set of function pointers for lvalues */
D 7
	signed char *(*sca)();
	unsigned char *(*uca)();
	int *(*ia)();
	unsigned int *(*uia)();
	short *(*sa)();
	unsigned short *(*usa)();
	long *(*la)();
	unsigned long *(*ula)();
	float *(*fa)();
	double *(*da)();
	st *(*sta)();
	void *(*va)();
E 7
I 7
	signed char *(*sca)(expr *);
	unsigned char *(*uca)(expr *);
	int *(*ia)(expr *);
	unsigned int *(*uia)(expr *);
	short *(*sa)(expr *);
	unsigned short *(*usa)(expr *);
	long *(*la)(expr *);
	unsigned long *(*ula)(expr *);
	long long *(*lla)(expr *);
	unsigned long long *(*ulla)(expr *);
	float *(*fa)(expr *);
	double *(*da)(expr *);
D 8
	st *(*sta)(expr *);
E 8
I 8
	st_t *(*sta)(expr *);
E 8
	void *(*va)(expr *);
E 7
    } e_func;
    union {
	struct {			/* binary and unary op nodes */
	    struct expr *_e_left;
	    struct expr *_e_right;
	} _e_op;
#define e_op    e_arg._e_op
#define e_left  e_op._e_left
#define e_right e_op._e_right

	struct {			/* function call nodes */
	    struct expr *_e_call;
	    struct cnode_list *_e_args;
	} _e_fcall;
#define e_fcall e_arg._e_fcall
#define e_args  e_fcall._e_args
#define e_call  e_fcall._e_call

	all _e_all;			/* constant leaf nodes */
#define e_all   e_arg._e_all
D 7
#define e_c     e_all.c
E 7
I 7
#define e_sc    e_all.sc
E 7
#define e_uc    e_all.uc
#define e_i     e_all.i
#define e_ui    e_all.ui
#define e_l     e_all.l
#define e_ul    e_all.ul
I 7
#define e_ll    e_all.ll
#define e_ull   e_all.ull
E 7
#define e_s     e_all.s
#define e_us    e_all.us
#define e_f     e_all.f
#define e_d     e_all.d
#define e_st    e_all.st

	char *_e_addr;
#define e_addr e_arg._e_addr

    } e_arg;
D 3
    int e_size;
E 3
I 3
    int e_size : 22;
    int e_boffset : 5;
    int e_bsize : 5;
E 3
D 7
} expr;
E 7
I 7
};
E 7

/* Macros to compute the pvalue of a single node */
D 7
#define sc_val(node) ((*(node)->e_func.sc)(node))
#define uc_val(node) ((*(node)->e_func.uc)(node))
#define i_val(node)  ((*(node)->e_func.i)(node))
#define ui_val(node) ((*(node)->e_func.ui)(node))
#define s_val(node)  ((*(node)->e_func.s)(node))
#define us_val(node) ((*(node)->e_func.us)(node))
#define l_val(node)  ((*(node)->e_func.l)(node))
#define ul_val(node) ((*(node)->e_func.ul)(node))
#define p_val(node)  ((*(node)->e_func.p)(node))
#define d_val(node)  ((*(node)->e_func.d)(node))
#define f_val(node)  ((*(node)->e_func.f)(node))
#define st_val(node) ((*(node)->e_func.st)(node))
#define v_val(node)  ((*(node)->e_func.v)(node))
E 7
I 7
#define sc_val(node)  ((*(node)->e_func.sc)(node))
#define uc_val(node)  ((*(node)->e_func.uc)(node))
#define i_val(node)   ((*(node)->e_func.i)(node))
#define ui_val(node)  ((*(node)->e_func.ui)(node))
#define s_val(node)   ((*(node)->e_func.s)(node))
#define us_val(node)  ((*(node)->e_func.us)(node))
#define l_val(node)   ((*(node)->e_func.l)(node))
#define ul_val(node)  ((*(node)->e_func.ul)(node))
#define ll_val(node)  ((*(node)->e_func.ll)(node))
#define ull_val(node) ((*(node)->e_func.ull)(node))
#define p_val(node)   ((*(node)->e_func.p)(node))
#define d_val(node)   ((*(node)->e_func.d)(node))
#define f_val(node)   ((*(node)->e_func.f)(node))
#define st_val(node)  ((*(node)->e_func.st)(node))
#define v_val(node)   ((*(node)->e_func.v)(node))
E 7

/* Macros to compute the lvalue value of a single node */
D 7
#define sc_addr(node) ((*(node)->e_func.sca)(node))
#define uc_addr(node) ((*(node)->e_func.uca)(node))
#define i_addr(node)  ((*(node)->e_func.ia)(node))
#define ui_addr(node) ((*(node)->e_func.uia)(node))
#define s_addr(node)  ((*(node)->e_func.sa)(node))
#define us_addr(node) ((*(node)->e_func.usa)(node))
#define l_addr(node)  ((*(node)->e_func.la)(node))
#define ul_addr(node) ((*(node)->e_func.ula)(node))
#define p_addr(node)  ((*(node)->e_func.pa)(node))
#define d_addr(node)  ((*(node)->e_func.da)(node))
#define f_addr(node)  ((*(node)->e_func.fa)(node))
#define st_addr(node) ((*(node)->e_func.sta)(node))
#define v_addr(node)  ((*(node)->e_func.va)(node))
E 7
I 7
#define sc_addr(node)  ((*(node)->e_func.sca)(node))
#define uc_addr(node)  ((*(node)->e_func.uca)(node))
#define i_addr(node)   ((*(node)->e_func.ia)(node))
#define ui_addr(node)  ((*(node)->e_func.uia)(node))
#define s_addr(node)   ((*(node)->e_func.sa)(node))
#define us_addr(node)  ((*(node)->e_func.usa)(node))
#define l_addr(node)   ((*(node)->e_func.la)(node))
#define ul_addr(node)  ((*(node)->e_func.ula)(node))
#define ll_addr(node)  ((*(node)->e_func.lla)(node))
#define ull_addr(node) ((*(node)->e_func.ulla)(node))
#define p_addr(node)   ((*(node)->e_func.pa)(node))
#define d_addr(node)   ((*(node)->e_func.da)(node))
#define f_addr(node)   ((*(node)->e_func.fa)(node))
#define st_addr(node)  ((*(node)->e_func.sta)(node))
#define v_addr(node)   ((*(node)->e_func.va)(node))
E 7

I 2
typedef struct arg_list {
    struct arg_list *a_next;
    anode a_anode;
} arg_list;

E 2
/*
 * During the compile phase, this struct is used to keep track of the
 * expression node, its type, and other information.
I 5
 *
 * History: The original concept was that only expression trees would
 * be kept af the compile phase.  But the print statement violates
 * this idea and the final cnode for the expression to be printed is
 * kept as well.
E 5
 */
D 5

E 5
typedef struct cnode {
    typeptr c_type;			/* pointer to typenode */
    enum expr_type c_base;		/* base type of cnode */
D 3
    unsigned int c_const : 1;		/* true if a constant */
E 3
    expr *c_expr;			/* current expression tree */
I 3
    int c_offset;			/* bit field offset */
    int c_size;				/* bit field size */
    unsigned int c_const : 1;		/* true if a constant */
    unsigned int c_bitfield : 1;	/* true for bit fields */
E 3
} cnode;

typedef struct cnode_list {
    struct cnode_list *cl_next;
I 4
    struct cnode_list *cl_down;
E 4
    cnode cl_cnode;
} cnode_list;

D 2
enum classes {
    auto_class, reg_class, extern_class, static_class, typedef_class,
    param_class
    };
E 2
I 2
extern char *type_2_string[];
E 2

D 2
typedef struct anode {
    typeptr a_type;			/* pointer to typenode */
    enum expr_type a_base;		/* base type of cnode */
    enum classes a_class;		/* storage class */
} anode;
    
/* user defined symbols */
struct sym {
    struct sym *s_hash;			/* hash chain */
    char *s_name;			/* name of symbol */
    enum expr_type s_base;		/* base type of symbol */
    typeptr s_type;			/* type */
    union {
	v_ptr _s_offset;		/* offset */
	int   _s_stmt;			/* statement */
    } _s_u;
#define s_offset _s_u._s_offset
#define s_stmt   _s_u._s_stmt
E 2
I 2
/*
 * To add a new type you need to add it to expr_type and make up a
 * "prefix" for the type, e.g. sc_ for signed char.  In tree.c, a
 * table of operators needs to be created, e.g. sc_table.  Then the
 * various macros like setalltable need to be modified.  All of this
 * could be done with a m4 style preprocessor but that gets pretty
 * gross (I've been that path as well).
 */
E 2

D 2
    int s_size;				/* size of symbol */
    int s_nesting;			/* nesting level */
    unsigned int s_global : 1;		/* true if a global */
};
typedef struct sym *symptr;

typedef struct arg_list {
    struct arg_list *a_next;
    char *a_ident;
    anode a_anode;
} arg_list;

E 2
expr *new_expr(void);
D 3
int mkident(cnode *result, char *s);
void mkl2p(cnode *result, cnode *c);
int mkdot(cnode *result, cnode *c, char *s);
int mkptr(cnode *result, cnode *c, char *s);
D 2
int mkarray(cnode *result, cnode *c, cnode *index);
E 2
I 2
int mkarray(cnode *result, cnode *array, cnode *index);
E 2
int mkasgn(cnode *result, cnode *lvalue, int opcode, cnode *rvalue);
int mkbinary(cnode *result, cnode *lvalue, int opcode, cnode *rvalue);
E 3
I 3
int mk_ident(cnode *result, char *s);
void mk_l2p(cnode *result, cnode *c);
int mk_dot(cnode *result, cnode *c, char *s);
int mk_ptr(cnode *result, cnode *c, char *s);
int mk_array(cnode *result, cnode *array, cnode *index);
int mk_asgn(cnode *result, cnode *lvalue, int opcode, cnode *rvalue);
int mk_unary(cnode *result, cnode *lvalue, int opcode);
int mk_binary(cnode *result, cnode *lvalue, int opcode, cnode *rvalue);
E 3
void mk_fcall(cnode *result, cnode *name, cnode_list *args);
int mk_qc_op(cnode *result, cnode *qvalue, cnode *tvalue, cnode *fvalue);
D 2
void cast_to(cnode *rvalue, typeptr t);
E 2
I 2
void cast_to(cnode *rvalue, typeptr t, enum expr_type base);
E 2
expr *cast_to_int(cnode *r);
void tree_init(void);
D 2
symptr name2userdef(char *name);
void clean_symtable();
symptr enter_sym(anode * attr, char *name);
E 2
enum expr_type base_type(typeptr t);
void eval_all(all *result, cnode *c);
D 2
typeptr mkstrtype(int len);
typeptr mkrange(char *n, long lower, long upper);
typeptr mkfloat(char *n, int bytes);
void mkconst(cnode *c, int value);
int mk_incdec(cnode *result, cnode *lvalue, int op);
E 2
I 2
D 3
typeptr mkstrtype(ns *nspace, int len);
typeptr mkrange(ns *nspace, char *n, long lower, long upper);
typeptr mkfloat(ns *nspace, char *n, int bytes);
void mkconst(ns *nspace, cnode *c, int value);
E 3
I 3
typeptr mk_strtype(ns *nspace, int len);
D 7
void mk_const(ns *nspace, cnode *c, int value);
E 7
I 7
void mk_const(ns *nspace, cnode *c, large_t value, int tval);
E 7
E 3
int mk_incdec(ns *nspace, cnode *result, cnode *lvalue, int op);
E 2
void print_expression(cnode *c);
D 7
void do_parameter_allocation(arg_list *old_list, arg_list *args);
E 7
D 2
void start_function(symptr f);
void end_function(void);
void cast_to_current_function(cnode *rvalue);
int increase_nesting(void);
void decrease_nesting(int new);


/*
 * To add a new type you need to add it to expr_type and make up a
 * "prefix" for the type, e.g. sc_ for signed char.  In tree.c, a
 * table of operators needs to be created, e.g. sc_table.  Then the
 * various macros like setalltable need to be modified.  All of this
 * could be done with a m4 style preprocessor but that gets pretty
 * gross (I've been that path as well).
 */
E 2
I 2
void *get_user_sym_addr(char *name);
I 7

/* From tree_dump.h */
void tree_dump(FILE *f, expr *e);

#endif /* __TREE_H */
E 7
E 2
E 1
