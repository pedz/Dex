
/* @(#)sym.h	1.9 */

#define HASH_SIZE 128

enum stab_type {
    STRUCT_TYPE, UNION_TYPE, PTR_TYPE, ARRAY_TYPE, RANGE_TYPE,
    PROC_TYPE, FLOAT_TYPE, ENUM_TYPE, STRINGPTR_TYPE, COMPLEX_TYPE,
    ELLIPSES, VOLATILE, CONSTANT_TYPE
};
    
struct type {
    struct type *t_hash;		/* hash list */
    char *t_name;			/* name of type */
    struct attr *t_attrs;		/* attributes */
    enum stab_type t_type;		/* type of the type */
    unsigned int t_printed : 1;		/* Used in dump_types */
    unsigned int t_typedef : 1;		/* true for typedefs */
    struct name_space *t_ns;		/* pointer to name space */
    union {
	struct {			/* 's' struct and 'u' union */
	    long s_size;		/* size in bytes */
	    struct field *s_fields;	/* list of fields */
	} val_s;
	struct type *val_k;		/* contant of this type */
	struct type *val_p;		/* type this pointer ptrs to */
	struct type *val_v;		/* base type for volatile */
	struct {			/* range type */
	    struct type *r_typeptr;	/* type of range */
	    char *r_lower;		/* lower bound */
	    char *r_upper;		/* upper bound */
	} val_r;
	struct {			/* float and complex */
	    struct type *g_typeptr;	/* type of float */
	    long g_size;			/* size in bytes */
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

struct field {
    struct field *f_next;		/* next field in list */
    char *f_name;			/* name of field */
    typeptr f_typeptr;			/* typeid of field */
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

struct string {
    struct string *str_hash;
    char *str_name;
};
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
    ushort_type, long_type, ulong_type, float_type, double_type,
    struct_type, void_type, bad_type
    };
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
    
typedef unsigned long stmt_index;
#define NO_STMT ((stmt_index)0)

/* user defined symbols */
struct sym {
    struct sym *s_hash;			/* hash chain */
    char *s_name;			/* name of symbol */
    struct name_space *s_ns;		/* pointer to name space */
    enum expr_type s_base;		/* base type of symbol */
    typeptr s_type;			/* type */
    union {
	v_ptr _s_offset;		/* offset */
	stmt_index _s_stmt;		/* statement */
    } _s_u;
#define s_offset _s_u._s_offset
#define s_stmt   _s_u._s_stmt
    struct expr *s_expr;		/* one expr per symbol */
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
    symptr sy_hash[HASH_SIZE];
    int sy_count;
};
typedef struct sym_table *symtabptr;

/*
 * The interpreter creates its own name space with identifiers,
 * typedefs, etc.  Each stab file that is loaded creates another name
 * space which is seperate.  Currently I have not thought too much how
 * to resolve conflicts except that when a user types in a name, a
 * sequential search will be made and the first name found will be the
 * name that is used.  Later, a syntax may be created to force a
 * particular name space.
 */
struct name_space {
    struct name_space *ns_next;		/* sequential on same level */
    struct name_space *ns_parent;	/* next level up */
    struct name_space *ns_lower;	/* next level down */
    struct name_space *ns_ltail;	/* next level down */
    char *ns_name;			/* name of name space */
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
};
typedef struct name_space ns;

typeptr name2typedef(ns *nspace, char *name);
typeptr name2typedef_all(char *name);
typeptr name2namedef(ns *nspace, char *name);
typeptr name2namedef_all(char *name);
typeptr insert_type(int typeid, typeptr t);
int typedef2typeid(typeptr t);
fieldptr newfield(char *name, typeptr tptr, int offset, int numbits);
attrptr newattr(enum attr_type t, int val);
enumptr newenum(char *name, int val);
void add_typedef(typeptr t, char *name);
void add_namedef(typeptr t, char *name);
typeptr newtype(ns *nspace, enum stab_type t);
typeptr find_type(ns *nspace, int typeid);
paramptr newparam(int typeid, int passby);
void copy_type(typeptr new, typeptr old);
void copy_type_and_record(typeptr new, typeptr old);
char *store_string(ns *nspace, char *name, int len, char *suffix);
int get_size(typeptr t);
ns *ns_create(ns *nspace, char *name);
ns *name2ns(char *name);
symptr name2userdef(ns *nspace, char *name);
symptr name2userdef_all(char *name);
symptr addr2userdef(ns *nspace, void *addr);
symptr addr2userdef_all(void *addr);
void clean_symtable(ns *nspace, int nesting_level);
symptr enter_sym(ns *nspace, char *name, int force);
void dump_symtable(void);
void dump_types(void);
int allocate_fields(fieldptr f);
void finish_copies(void);
