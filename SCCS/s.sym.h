h11605
s 00007/00007/00249
d D 1.11 10/08/23 17:01:54 pedzan 11 10
e
s 00019/00007/00237
d D 1.10 02/03/14 16:12:34 pedz 10 9
e
s 00000/00000/00244
d D 1.9 00/09/18 17:57:03 pedz 9 8
c Checking before V5 conversion
e
s 00004/00003/00240
d D 1.8 00/02/08 17:36:48 pedz 8 7
e
s 00006/00004/00237
d D 1.7 98/10/23 12:26:12 pedz 7 6
e
s 00001/00000/00240
d D 1.6 97/05/13 16:03:00 pedz 6 5
c Check pointer
e
s 00001/00000/00239
d D 1.5 95/05/17 16:39:17 pedz 5 4
c Check point
e
s 00003/00002/00236
d D 1.4 95/04/25 10:10:26 pedz 4 3
e
s 00004/00005/00234
d D 1.3 95/02/09 21:36:03 pedz 3 2
e
s 00138/00023/00101
d D 1.2 95/02/01 10:37:11 pedz 2 1
e
s 00124/00000/00000
d D 1.1 94/08/22 17:56:36 pedz 1 0
c date and time created 94/08/22 17:56:36 by pedz
e
u
U
t
T
I 1

/* %W% */

I 10
#ifndef __SYM_H
#define __SYM_H

E 10
I 2
#define HASH_SIZE 128

E 2
enum stab_type {
    STRUCT_TYPE, UNION_TYPE, PTR_TYPE, ARRAY_TYPE, RANGE_TYPE,
    PROC_TYPE, FLOAT_TYPE, ENUM_TYPE, STRINGPTR_TYPE, COMPLEX_TYPE,
D 2
    ELLIPSES
E 2
I 2
    ELLIPSES, VOLATILE, CONSTANT_TYPE
E 2
};
    
struct type {
    struct type *t_hash;		/* hash list */
    char *t_name;			/* name of type */
    struct attr *t_attrs;		/* attributes */
    enum stab_type t_type;		/* type of the type */
D 4
    char t_typedef;			/* 1 if this is a typedef */
E 4
I 4
    unsigned int t_printed : 1;		/* Used in dump_types */
I 8
    unsigned int t_typedef : 1;		/* true for typedefs */
E 8
E 4
    struct name_space *t_ns;		/* pointer to name space */
    union {
	struct {			/* 's' struct and 'u' union */
D 8
	    int s_size;			/* size in bytes */
E 8
I 8
	    long s_size;		/* size in bytes */
E 8
	    struct field *s_fields;	/* list of fields */
	} val_s;
I 2
	struct type *val_k;		/* contant of this type */
E 2
	struct type *val_p;		/* type this pointer ptrs to */
I 2
	struct type *val_v;		/* base type for volatile */
E 2
	struct {			/* range type */
	    struct type *r_typeptr;	/* type of range */
D 7
	    long r_lower;		/* lower bound */
	    long r_upper;		/* upper bound */
E 7
I 7
	    char *r_lower;		/* lower bound */
	    char *r_upper;		/* upper bound */
E 7
	} val_r;
	struct {			/* float and complex */
	    struct type *g_typeptr;	/* type of float */
D 8
	    int g_size;			/* size in bytes */
E 8
I 8
D 10
	    long g_size;			/* size in bytes */
E 10
I 10
	    long g_size;		/* size in bytes */
E 10
E 8
	} val_g;
	struct e_num *val_e;		/* enumeration list */
	int val_N;			/* pascal stringptr */
	struct {			/* proc type */
	    struct type *f_typeptr;	/* type of procedure */
	    int f_params;		/* number of params */
	    struct param *f_paramlist;	/* list of parameters */
	} val_f;
	struct {			/* array type */
	    struct type *a_typedef;	/* range of array */
	    struct type *a_typeptr;	/* type in array */
	} val_a;
    } t_val;
};
typedef struct type *typeptr;

I 2
struct type_table {
    typeptr tt_hash[HASH_SIZE];
    int tt_count;
};
typedef struct type_table *typetabptr;

struct typeid_table {
    typeptr *tid_type_pos;
    typeptr *tid_type_neg;
    int tid_upper_limit;
    int tid_lower_limit;
};
typedef struct typeid_table *tidtabptr;

E 2
struct field {
    struct field *f_next;		/* next field in list */
    char *f_name;			/* name of field */
D 3
    int f_typeid;			/* typeid of field */
E 3
I 3
    typeptr f_typeptr;			/* typeid of field */
E 3
    int f_offset;			/* offset for field */
    int f_numbits;			/* number of bits for field */
};
typedef struct field *fieldptr;

enum attr_type { ALIGN_ATTR, SIZE_ATTR, PTR_ATTR, PACKED_ATTR };
struct attr {
    struct attr *a_next;
    enum attr_type a_type;
    int a_val;
};
typedef struct attr *attrptr;

struct e_num {
    struct e_num *e_next;
    char *e_name;
    int e_val;
};
typedef struct e_num *enumptr;

struct param {
    struct param *p_next;
    int p_typeid;
    int p_passby;
};
typedef struct param *paramptr;

D 2
struct name {
    struct name *n_hash;
    char *n_name;
E 2
I 2
struct string {
    struct string *str_hash;
    char *str_name;
E 2
};
D 2
typedef struct name *nameptr;
E 2
I 2
typedef struct string *stringptr;

struct string_table {
    stringptr st_hash[HASH_SIZE];
    int st_count;
};
typedef struct string_table *strtabptr;

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
D 10
    ushort_type, long_type, ulong_type, float_type, double_type,
    struct_type, void_type, bad_type
    };
E 10
I 10
    ushort_type, long_type, ulong_type, long_long_type, ulong_long_type,
    float_type, double_type, struct_type, void_type, bad_type
};
E 10
#define FIRST_TYPE schar_type
#define LAST_TYPE  bad_type

/* Typedef for pointer type */
typedef char *ptr;

enum classes {
    auto_class, reg_class, extern_class, static_class, typedef_class,
    param_class
    };

typedef struct anode {
    typeptr a_type;			/* pointer to typenode */
    enum expr_type a_base;		/* base type of cnode */
    enum classes a_class;		/* storage class */
    char *a_name;			/* identifier name */
    unsigned int a_valid_type : 1;
    unsigned int a_valid_base : 1;
    unsigned int a_valid_class : 1;
    unsigned int a_valid_name : 1;
} anode;
E 2
    
I 2
D 8
typedef unsigned int stmt_index;
E 8
I 8
typedef unsigned long stmt_index;
E 8
#define NO_STMT ((stmt_index)0)

/* user defined symbols */
struct sym {
    struct sym *s_hash;			/* hash chain */
    char *s_name;			/* name of symbol */
I 5
D 7
    struct name_space *s_ns;		/* pointer back to name space */
E 7
I 7
    struct name_space *s_ns;		/* pointer to name space */
E 7
E 5
    enum expr_type s_base;		/* base type of symbol */
    typeptr s_type;			/* type */
    union {
D 11
	v_ptr _s_offset;		/* offset */
E 11
I 11
	void *_s_offset;		/* offset */
E 11
	stmt_index _s_stmt;		/* statement */
    } _s_u;
#define s_offset _s_u._s_offset
#define s_stmt   _s_u._s_stmt
D 7

E 7
I 7
    struct expr *s_expr;		/* one expr per symbol */
E 7
    int s_size;				/* size of symbol */
    int s_nesting;			/* nesting level */
    unsigned int s_global : 1;		/* true if a global */
    unsigned int s_defined : 1;		/* true if already defined */
    unsigned int s_haddot : 1;		/* actually was .foo not foo */
    unsigned int s_typed : 1;		/* has real typing */
    unsigned int s_internal : 1;	/* internal function */
};
typedef struct sym *symptr;
    
struct sym_table {
I 10
    unsigned int sy_addr_sorted : 1;	/* true sy_addr2sym is valid */
E 10
    symptr sy_hash[HASH_SIZE];
    int sy_count;
I 10
    symptr **sy_addr2sym;
E 10
};
typedef struct sym_table *symtabptr;

E 2
/*
 * The interpreter creates its own name space with identifiers,
 * typedefs, etc.  Each stab file that is loaded creates another name
 * space which is seperate.  Currently I have not thought too much how
 * to resolve conflicts except that when a user types in a name, a
 * sequential search will be made and the first name found will be the
 * name that is used.  Later, a syntax may be created to force a
 * particular name space.
 */
D 2
#define HASH_SIZE 128
E 2
struct name_space {
D 2
    struct name_space *ns_next;
E 2
I 2
    struct name_space *ns_next;		/* sequential on same level */
    struct name_space *ns_parent;	/* next level up */
    struct name_space *ns_lower;	/* next level down */
    struct name_space *ns_ltail;	/* next level down */
E 2
    char *ns_name;			/* name of name space */
D 2
    nameptr ns_names[HASH_SIZE];	/* table for names */
    typeptr ns_typedefs[HASH_SIZE];	/* typedefs */
    typeptr ns_namedefs[HASH_SIZE];	/* struct, union, and enum's */
    typeptr *ns_type_pos;		/* vector of positive type nums */
    typeptr *ns_type_neg;		/* vector of negative type nums */
    int ns_upper_limit;			/* size of type_pos */
    int ns_lower_limit;			/* size of type_neg */
E 2
I 2
    strtabptr ns_strings;		/* pointer to string table */
    symtabptr ns_syms;			/* pointer to symbol table */
    typetabptr ns_typedefs;		/* pointer to typedef name table */
    typetabptr ns_namedefs;		/* pointer to other name table */
    tidtabptr ns_tids;			/* pointer to typeid table */

    int ns_text_start;			/* Starting Address */
    int ns_text_size;			/* Size */
    int ns_data_start;
    int ns_data_size;
    struct lineno *ns_lines;
    int ns_lineoffset;
I 10
    int ns_lastdup;			/* used to make unique names */
E 10
E 2
};
typedef struct name_space ns;

D 2
typeptr name2typedef(char *name);
typeptr name2namedef(char *name);
void insert_type(int typeid, typeptr t);
E 2
I 2
typeptr name2typedef(ns *nspace, char *name);
typeptr name2typedef_all(char *name);
typeptr name2namedef(ns *nspace, char *name);
typeptr name2namedef_all(char *name);
D 11
typeptr insert_type(int typeid, typeptr t);
E 2
int typedef2typeid(typeptr t);
E 11
I 11
typeptr insert_type(int type_id, typeptr t);
int typedef2type_id(typeptr t);
E 11
I 10
int allocate_fields(fieldptr f);
E 10
D 3
fieldptr newfield(char *name, int typeid, int offset, int numbits);
E 3
I 3
fieldptr newfield(char *name, typeptr tptr, int offset, int numbits);
E 3
attrptr newattr(enum attr_type t, int val);
enumptr newenum(char *name, int val);
D 2
typeptr newtype(ns *ns, enum stab_type t, char *s, int tdef);
typeptr find_type(ns *ns, int typeid);
E 2
I 2
void add_typedef(typeptr t, char *name);
void add_namedef(typeptr t, char *name);
typeptr newtype(ns *nspace, enum stab_type t);
D 11
typeptr find_type(ns *nspace, int typeid);
E 2
paramptr newparam(int typeid, int passby);
D 2
void print_out(typeptr tptr, char *addr, int offset, int size,
	       int indent, char *name);
E 2
I 2
D 3
void print_out(typeptr tptr, char *addr, int offset, int size, int indent,
	       char *name);
E 2
void getval(int *i, char *addr, int offset, int size);
E 3
D 10
void copy_type(typeptr new, typeptr old);
E 10
I 7
void copy_type_and_record(typeptr new, typeptr old);
E 11
I 11
typeptr find_type(ns *nspace, int type_id);
paramptr newparam(int type_id, int passby);
void copy_type_and_record(typeptr newp, typeptr oldp);
E 11
I 10
void finish_copies(void);
D 11
void copy_type(typeptr new, typeptr old);
E 11
I 11
void copy_type(typeptr newp, typeptr oldp);
E 11
E 10
E 7
D 2
char *store_name(ns *ns, char *name);
E 2
I 2
char *store_string(ns *nspace, char *name, int len, char *suffix);
E 2
int get_size(typeptr t);
D 2
ns *ns_create(char *name);
E 2
I 2
ns *ns_create(ns *nspace, char *name);
I 10
void load_base_types(ns *nspace);
void new_symbols(ns *nspace);
E 10
E 2
ns *name2ns(char *name);
I 2
symptr name2userdef(ns *nspace, char *name);
symptr name2userdef_all(char *name);
I 10
void xxx(ns *nspace, int indent);
void view_namespaces(void);
E 10
I 3
symptr addr2userdef(ns *nspace, void *addr);
symptr addr2userdef_all(void *addr);
E 3
void clean_symtable(ns *nspace, int nesting_level);
D 4
symptr enter_sym(ns *nspace, char *name);
E 4
I 4
symptr enter_sym(ns *nspace, char *name, int force);
E 4
void dump_symtable(void);
I 4
void dump_types(void);
I 6
D 10
int allocate_fields(fieldptr f);
I 7
void finish_copies(void);
E 10
I 10

#endif /* __SYM_H */
E 10
E 7
E 6
E 4
E 2
E 1
