
/* @(#)sym.h	1.1 */

enum stab_type {
    STRUCT_TYPE, UNION_TYPE, PTR_TYPE, ARRAY_TYPE, RANGE_TYPE,
    PROC_TYPE, FLOAT_TYPE, ENUM_TYPE, STRINGPTR_TYPE, COMPLEX_TYPE,
    ELLIPSES
};
    
struct type {
    struct type *t_hash;		/* hash list */
    char *t_name;			/* name of type */
    struct attr *t_attrs;		/* attributes */
    enum stab_type t_type;		/* type of the type */
    char t_typedef;			/* 1 if this is a typedef */
    struct name_space *t_ns;		/* pointer to name space */
    union {
	struct {			/* 's' struct and 'u' union */
	    int s_size;			/* size in bytes */
	    struct field *s_fields;	/* list of fields */
	} val_s;
	struct type *val_p;		/* type this pointer ptrs to */
	struct {			/* range type */
	    struct type *r_typeptr;	/* type of range */
	    long r_lower;		/* lower bound */
	    long r_upper;		/* upper bound */
	} val_r;
	struct {			/* float and complex */
	    struct type *g_typeptr;	/* type of float */
	    int g_size;			/* size in bytes */
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

struct field {
    struct field *f_next;		/* next field in list */
    char *f_name;			/* name of field */
    int f_typeid;			/* typeid of field */
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

struct name {
    struct name *n_hash;
    char *n_name;
};
typedef struct name *nameptr;
    
/*
 * The interpreter creates its own name space with identifiers,
 * typedefs, etc.  Each stab file that is loaded creates another name
 * space which is seperate.  Currently I have not thought too much how
 * to resolve conflicts except that when a user types in a name, a
 * sequential search will be made and the first name found will be the
 * name that is used.  Later, a syntax may be created to force a
 * particular name space.
 */
#define HASH_SIZE 128
struct name_space {
    struct name_space *ns_next;
    char *ns_name;			/* name of name space */
    nameptr ns_names[HASH_SIZE];	/* table for names */
    typeptr ns_typedefs[HASH_SIZE];	/* typedefs */
    typeptr ns_namedefs[HASH_SIZE];	/* struct, union, and enum's */
    typeptr *ns_type_pos;		/* vector of positive type nums */
    typeptr *ns_type_neg;		/* vector of negative type nums */
    int ns_upper_limit;			/* size of type_pos */
    int ns_lower_limit;			/* size of type_neg */
};
typedef struct name_space ns;

typeptr name2typedef(char *name);
typeptr name2namedef(char *name);
void insert_type(int typeid, typeptr t);
int typedef2typeid(typeptr t);
fieldptr newfield(char *name, int typeid, int offset, int numbits);
attrptr newattr(enum attr_type t, int val);
enumptr newenum(char *name, int val);
typeptr newtype(ns *ns, enum stab_type t, char *s, int tdef);
typeptr find_type(ns *ns, int typeid);
paramptr newparam(int typeid, int passby);
void print_out(typeptr tptr, char *addr, int offset, int size,
	       int indent, char *name);
void getval(int *i, char *addr, int offset, int size);
void copy_type(typeptr new, typeptr old);
char *store_name(ns *ns, char *name);
int get_size(typeptr t);
ns *ns_create(char *name);
ns *name2ns(char *name);
