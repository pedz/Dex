
/* @(#)tree.h	1.1 */

/*
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
 * When a struct is evaulated, the address of the structure is
 * returned on the stack.
 */
typedef unsigned long st;

/* This union can contain all of the types known to the interpreter */
typedef union all {
    signed char c;
    unsigned char uc;
    int i;
    unsigned int ui;
    long l;
    unsigned long ul;
    short s;
    unsigned short us;
    float f;
    double d;
    st st;
} all;

/* A node in the expression tree */
typedef struct expr {
    union all_func {
	/* set of function pointers for pvalues */
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

	/* set of function pointers for lvalues */
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
#define e_c     e_all.c
#define e_uc    e_all.uc
#define e_i     e_all.i
#define e_ui    e_all.ui
#define e_l     e_all.l
#define e_ul    e_all.ul
#define e_s     e_all.s
#define e_us    e_all.us
#define e_f     e_all.f
#define e_d     e_all.d
#define e_st    e_all.st

	char *_e_addr;
#define e_addr e_arg._e_addr

    } e_arg;
    int e_size;
} expr;

/* Macros to compute the pvalue of a single node */
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

/* Macros to compute the lvalue value of a single node */
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

/*
 * During the compile phase, this struct is used to keep track of the
 * expression node, its type, and other information.
 */

typedef struct cnode {
    typeptr c_type;			/* pointer to typenode */
    enum expr_type c_base;		/* base type of cnode */
    unsigned int c_const : 1;		/* true if a constant */
    expr *c_expr;			/* current expression tree */
} cnode;

typedef struct cnode_list {
    struct cnode_list *cl_next;
    cnode cl_cnode;
} cnode_list;

enum classes {
    auto_class, reg_class, extern_class, static_class, typedef_class,
    param_class
    };

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

expr *new_expr(void);
int mkident(cnode *result, char *s);
void mkl2p(cnode *result, cnode *c);
int mkdot(cnode *result, cnode *c, char *s);
int mkptr(cnode *result, cnode *c, char *s);
int mkarray(cnode *result, cnode *c, cnode *index);
int mkasgn(cnode *result, cnode *lvalue, int opcode, cnode *rvalue);
int mkbinary(cnode *result, cnode *lvalue, int opcode, cnode *rvalue);
void mk_fcall(cnode *result, cnode *name, cnode_list *args);
int mk_qc_op(cnode *result, cnode *qvalue, cnode *tvalue, cnode *fvalue);
void cast_to(cnode *rvalue, typeptr t);
expr *cast_to_int(cnode *r);
void tree_init(void);
symptr name2userdef(char *name);
void clean_symtable();
symptr enter_sym(anode * attr, char *name);
enum expr_type base_type(typeptr t);
void eval_all(all *result, cnode *c);
typeptr mkstrtype(int len);
typeptr mkrange(char *n, long lower, long upper);
typeptr mkfloat(char *n, int bytes);
void mkconst(cnode *c, int value);
int mk_incdec(cnode *result, cnode *lvalue, int op);
void print_expression(cnode *c);
void do_parameter_allocation(arg_list *old_list, arg_list *args);
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
