h33074
s 00038/00038/00712
d D 1.12 10/08/23 17:01:54 pedzan 12 11
e
s 00043/00028/00707
d D 1.11 02/03/14 16:12:34 pedz 11 10
e
s 00000/00000/00735
d D 1.10 00/09/18 17:57:03 pedz 10 9
c Checking before V5 conversion
e
s 00002/00000/00733
d D 1.9 00/02/08 17:36:48 pedz 9 8
e
s 00061/00110/00672
d D 1.8 98/10/23 12:26:12 pedz 8 7
e
s 00024/00005/00758
d D 1.7 97/05/13 16:02:59 pedz 7 6
c Check pointer
e
s 00036/00006/00727
d D 1.6 97/02/24 10:37:51 pedz 6 5
c Checkpoint
e
s 00018/00007/00715
d D 1.5 95/05/17 16:39:15 pedz 5 4
c Check point
e
s 00043/00004/00679
d D 1.4 95/04/25 10:10:23 pedz 4 3
e
s 00048/00276/00635
d D 1.3 95/02/09 21:36:00 pedz 3 2
e
s 00490/00089/00421
d D 1.2 95/02/01 10:37:09 pedz 2 1
e
s 00510/00000/00000
d D 1.1 94/08/22 17:56:35 pedz 1 0
c date and time created 94/08/22 17:56:35 by pedz
e
u
U
t
T
I 1
static char sccs_id[] = "%W%";

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
I 2
D 3
#include <setjmp.h>
E 3
#include <dbxstclass.h>
E 2
#include "map.h"
#include "sym.h"
D 2
#include "tree.h"
E 2
#include "dex.h"
I 11
#include "stab_pre.h"
#include "print.h"
E 11

I 11
#define DEBUG_BIT SYM_C_BIT

E 11
static ns *ns_head;
static ns *ns_tail;
I 2

extern char *type_2_string[];		/* Ick! */
D 8
static char *stab_type_2_string[] = {
    "struct",
    "union",
    "ptr",
    "array",
    "range",
    "proc",
    "float",
    "enum",
    "stringptr",
    "complex",
    "ellipses",
    "volatile",
    "BOGUS"
};
E 8

static int hash(char *s);
D 12
static typeptr do_insert(int typeid, typeptr t, int *limit, typeptr **ptr);
E 12
I 12
static typeptr do_insert(int type_id, typeptr t, int *limit, typeptr **ptr);
E 12
E 2
D 3
static void print_name(char *name, typeptr tptr);
E 3
I 2
D 8
static int sym_compare(void *v1, void *v2);
static void print_sym(symptr s);
static void _dump_symtable(ns *nspace, symtabptr old_sytp);
E 8
E 2

static int hash(char *s)
{
    int hash = 0;
I 2
    int c;
E 2

D 2
    while (*s) {
	hash += *s++;
E 2
I 2
    while (c = *s++) {
E 2
	hash <<= 1;
I 2
	hash += c;
E 2
    }
    return hash & (HASH_SIZE - 1);
}

D 2
typeptr name2typedef(char *name)
E 2
I 2
typeptr name2typedef(ns *nspace, char *name)
E 2
{
D 2
    ns *ns;
    typeptr ret = 0;
E 2
I 2
    typeptr ret;
    typetabptr ttp;

    if (ttp = nspace->ns_typedefs)
	for (ret = ttp->tt_hash[hash(name)]; ret; ret = ret->t_hash)
	    if (!strcmp(ret->t_name, name))
		return ret;

    for (nspace = nspace->ns_lower; nspace; nspace = nspace->ns_next)
	if (ret = name2typedef(nspace, name))
	    return ret;

    return 0;
}

typeptr name2typedef_all(char *name)
{
    ns *nspace;
    typeptr ret;
E 2
    
    if (name)
D 2
	for (ns = ns_head; ns; ns = ns->ns_next)
	    for (ret = ns->ns_typedefs[hash(name)];
		 ret && strcmp(ret->t_name, name);
		 ret = ret->t_hash);
    return ret;
E 2
I 2
	for (nspace = ns_head; nspace; nspace = nspace->ns_next)
	    if (ret = name2typedef(nspace, name))
		return ret;
    return 0;
E 2
}

I 7
/*
 * A special patch was added in the case of structures.  If the first
 * thing we find is a structure but it has zero size, we keep
 * searching.  If we do not find anything else, we return what we
 * found at first.  But if we find something more interesting, like a
 * structure of non-zero size, we return that.  We have to do this for
 * name2namedef as well as name2namedef_all.
 */
E 7
D 2
typeptr name2namedef(char *name)
E 2
I 2
typeptr name2namedef(ns *nspace, char *name)
E 2
{
D 2
    ns *ns;
E 2
    typeptr ret;
I 2
    typetabptr ttp;
I 7
    typeptr struct_temp = 0;
E 7

    if (ttp = nspace->ns_namedefs)
	for (ret = ttp->tt_hash[hash(name)]; ret; ret = ret->t_hash)
	    if (!strcmp(ret->t_name, name))
D 7
		return ret;
E 7
I 7
		if (ret->t_type == STRUCT_TYPE && ret->t_val.val_s.s_size == 0)
		    struct_temp = ret;
		else
		    return ret;
E 7

    for (nspace = nspace->ns_lower; nspace; nspace = nspace->ns_next)
	if (ret = name2namedef(nspace, name))
D 7
	    return ret;
E 7
I 7
	    if (ret->t_type == STRUCT_TYPE && ret->t_val.val_s.s_size == 0)
		struct_temp = ret;
	    else
		return ret;
E 7

D 7
    return 0;
E 7
I 7
    return struct_temp;
E 7
}

typeptr name2namedef_all(char *name)
{
    ns *nspace;
    typeptr ret;
I 7
    typeptr struct_temp = 0;
E 7
E 2
    
D 2
    for (ns = ns_head; ns; ns = ns->ns_next)
	for (ret = ns->ns_namedefs[hash(name)];
	     ret && strcmp(ret->t_name, name);
	     ret = ret->t_hash);
    return ret;
E 2
I 2
    for (nspace = ns_head; nspace; nspace = nspace->ns_next)
	if (ret = name2namedef(nspace, name))
D 7
	    return ret;
E 7
I 7
	    if (ret->t_type == STRUCT_TYPE && ret->t_val.val_s.s_size == 0)
		struct_temp = ret;
	    else
		return ret;
E 7

D 7
    return 0;
E 7
I 7
    return struct_temp;
E 7
E 2
}

D 2
static do_insert(int typeid, typeptr t, int *limit, typeptr **ptr)
E 2
I 2
D 12
static typeptr do_insert(int typeid, typeptr t, int *limit, typeptr **ptr)
E 12
I 12
static typeptr do_insert(int type_id, typeptr t, int *limit, typeptr **ptr)
E 12
E 2
{
D 2
    if (typeid >= *limit) {
E 2
I 2
    int old_limit;
    typeptr old_type;

D 12
    if (typeid >= (old_limit = *limit)) {
E 2
D 11
	*limit = typeid + 100;
E 11
I 11
	size_t old = old_limit * sizeof(typeptr);
	size_t new = (*limit = typeid + 100) * sizeof(typeptr);
E 12
I 12
    if (type_id >= (old_limit = *limit)) {
	size_t oldp = old_limit * sizeof(typeptr);
	size_t newp = (*limit = type_id + 100) * sizeof(typeptr);
E 12

E 11
D 2
	if (*ptr)
	    *ptr = realloc(*ptr, *limit * sizeof(typeptr));
	else
E 2
I 2
D 3
	if (*ptr) {
	    if (!(*ptr = realloc(*ptr, *limit * sizeof(typeptr)))) {
		fprintf(stderr, "Out of memory\n");
		exit(1);
	    }
	} else
E 3
I 3
	if (*ptr)
D 6
	    *ptr = srealloc(*ptr, *limit * sizeof(typeptr), __FILE__, __LINE__);
E 6
I 6
D 11
	    *ptr = srealloc(*ptr, *limit * sizeof(typeptr));
E 11
I 11
D 12
	    *ptr = srealloc(*ptr, new, old);
E 12
I 12
	    *ptr = srealloc(*ptr, newp, oldp);
E 12
E 11
E 6
	else
E 3
E 2
D 6
	    *ptr = smalloc(*limit * sizeof(typeptr), __FILE__, __LINE__);
E 6
I 6
D 11
	    *ptr = smalloc(*limit * sizeof(typeptr));
E 6
D 2
	if (!*ptr) {
	    fprintf(stderr, "Out of memory\n");
	    exit(1);
	}
E 2
I 2
	bzero(*ptr + old_limit, sizeof(typeptr) * (*limit - old_limit));
E 11
I 11
D 12
	    *ptr = smalloc(new);
E 12
I 12
	    *ptr = smalloc(newp);
E 12
E 11
E 2
    }
D 2
    (*ptr)[typeid] = t;
E 2
I 2

D 12
    if (old_type = (*ptr)[typeid]) {
E 12
I 12
    if (old_type = (*ptr)[type_id]) {
E 12
I 4
	char *old_name = old_type->t_name;
	struct type *old_hash = old_type->t_hash;

	if (t->t_name) {		/* if this has a name then we freak */
D 11
	    printf("Replacing %s name of %s:%d\n", t->t_name, t->t_ns->ns_name,
		   typeid);
E 11
I 11
	    fprintf(stderr, "Replacing %s name of %s:%d\n",
D 12
		    t->t_name, t->t_ns->ns_name, typeid);
E 12
I 12
		    t->t_name, t->t_ns->ns_name, type_id);
E 12
E 11
	    exit(1);
	}

E 4
D 12
	*old_type = *t;			/* copy new into old */
E 12
I 12
	*old_type = *t;			/* copy newp into oldp */
E 12
I 4
	old_type->t_name = old_name;
	old_type->t_hash = old_hash;
#if 0
	/*
	 * We can copy with the case of one named entry replacing
	 * another entry that is not named and a non-named entry
	 * replacing a named entry.  However, I think the first case
	 * will never happen. i.e. t->t_name will always be null.  The
	 * code below is a starting point for the second case.  If
	 * both of them are named, then we have problems for sure.
	 */
E 4
	if (old_type->t_name) {		/* need to fix the hash link */
	    int hash_val = hash(t->t_name);
	    typetabptr ttp = t->t_ns->ns_typedefs;
	    typeptr *tp = ttp->tt_hash + hash_val;

	    while (*tp && *tp != t)
		tp = &(*tp)->t_hash;

	    if (!*tp) {
		/* Not in typedefs so try namedefs */
		ttp = t->t_ns->ns_namedefs;
		tp = ttp->tt_hash + hash_val;
		while (*tp && *tp != t)
		    tp = &(*tp)->t_hash;
		if (!*tp) {
		    fprintf(stderr, "Didn't find hash link %s:%d\n",
			    __FILE__, __LINE__);
		    exit(1);
		}
	    }
	    *tp = old_type;
	}
I 4
#endif
E 4
	return old_type;
    } else
D 12
	(*ptr)[typeid] = t;
E 12
I 12
	(*ptr)[type_id] = t;
E 12
    return t;
E 2
}

D 2
void insert_type(int typeid, typeptr t)
E 2
I 2
D 12
typeptr insert_type(int typeid, typeptr t)
E 12
I 12
typeptr insert_type(int type_id, typeptr t)
E 12
E 2
{
D 2
    ns *ns = t->t_ns;
E 2
I 2
    ns *nspace = t->t_ns;
    tidtabptr tid;

    /*
D 12
     * A typeid of 0 seems to be for temp ranges.
E 12
I 12
     * A type_id of 0 seems to be for temp ranges.
E 12
     */
D 12
    if (typeid == 0)
E 12
I 12
    if (type_id == 0)
E 12
	return t;
D 11
    if (!(tid = nspace->ns_tids)) {
E 11
I 11
    if (!(tid = nspace->ns_tids))
E 11
	tid = nspace->ns_tids = new(struct typeid_table);
D 11
	bzero(nspace->ns_tids, sizeof(*tid));
    }
E 11
E 2
D 12
    if (typeid < 0)
D 2
	do_insert(-typeid, t, &ns->ns_lower_limit, &ns->ns_type_neg);
E 2
I 2
D 11
	return do_insert(-typeid, t, &tid->tid_lower_limit, &tid->tid_type_neg);
E 11
I 11
	return do_insert(-typeid, t, &tid->tid_lower_limit,&tid->tid_type_neg);
E 12
I 12
    if (type_id < 0)
	return do_insert(-type_id, t, &tid->tid_lower_limit,&tid->tid_type_neg);
E 12
E 11
E 2
    else
D 2
	do_insert(typeid, t, &ns->ns_upper_limit, &ns->ns_type_pos);
E 2
I 2
D 12
	return do_insert(typeid, t, &tid->tid_upper_limit, &tid->tid_type_pos);
E 12
I 12
	return do_insert(type_id, t, &tid->tid_upper_limit, &tid->tid_type_pos);
E 12
E 2
}

D 12
int typedef2typeid(typeptr t)
E 12
I 12
int typedef2type_id(typeptr t)
E 12
{
D 2
    ns *ns = t->t_ns;
E 2
I 2
    ns *nspace = t->t_ns;
E 2
    typeptr *p, *e;
I 2
    tidtabptr tid;
E 2

D 2
    if (p = ns->ns_type_neg)
	for (e = p + ns->ns_lower_limit; p < e; ++p)
E 2
I 2
    if (!(tid = nspace->ns_tids))
	return 0;
    if (p = tid->tid_type_neg)
	for (e = p + tid->tid_lower_limit; p < e; ++p)
E 2
	    if (*p == t)
D 2
		return -(p - ns->ns_type_neg);
    if (p = ns->ns_type_pos)
	for (e = p + ns->ns_upper_limit; p < e; ++p)
E 2
I 2
		return -(p - tid->tid_type_neg);
    if (p = tid->tid_type_pos)
	for (e = p + tid->tid_upper_limit; p < e; ++p)
E 2
	    if (*p == t)
D 2
		return p - ns->ns_type_pos;
E 2
I 2
		return p - tid->tid_type_pos;
E 2
    return 0;
}

I 6
int allocate_fields(fieldptr f)
{
    int pos = 0;

    for ( ; f; f = f->f_next) {
	/* if foo : size was specified */
	if (f->f_numbits < 0)
	    f->f_numbits = -f->f_numbits;
	else {
	    /* No size specified so move to proper boundry */
	    int bytes = (f->f_numbits + 7) / 8;
	    
	    if (bytes > 4)
		bytes = 4;
	    pos += (bytes * 8 - 1);
	    pos &= ~(bytes * 8 - 1);
	}
	f->f_offset = pos;
	pos += f->f_numbits;
    }
    return pos;
}

E 6
D 3
fieldptr newfield(char *name, int typeid, int offset, int numbits)
E 3
I 3
fieldptr newfield(char *name, typeptr tptr, int offset, int numbits)
E 3
{
    fieldptr ret = new(struct field);

    ret->f_next = 0;
    ret->f_name = name;
D 3
    ret->f_typeid = typeid;
E 3
I 3
    ret->f_typeptr = tptr;
E 3
    ret->f_offset = offset;
    ret->f_numbits = numbits;
    return ret;
}

attrptr newattr(enum attr_type t, int val)
{
    attrptr ret = new(struct attr);

    ret->a_next = 0;
    ret->a_type = t;
    ret->a_val = val;
    return ret;
}

enumptr newenum(char *name, int val)
{
    enumptr ret = new(struct e_num);

    ret->e_next = 0;
    ret->e_name = name;
    ret->e_val = val;
    return ret;
}

D 2
typeptr newtype(ns *ns, enum stab_type t, char *s, int tdef)
E 2
I 2
void add_typedef(typeptr t, char *name)
E 2
{
D 2
    typeptr ret = new(struct type);
E 2
I 2
    ns *nspace;
    typetabptr ttp;
    int hash_val = hash(name);
E 2

D 2
    if (ret->t_name = s) {
	int hash_val = hash(s);
E 2
I 2
    if (!t) {
	fprintf(stderr, "Null typedef pointer %s:%d\n", __FILE__, __LINE__);
	return;
    }

I 4
    if (t->t_name) {			/* already entered */
	/* printf("type %s %s\n", name, t->t_ns->ns_name); */
	return;
    }

E 4
    nspace = t->t_ns;
D 11
    if (!(ttp = nspace->ns_typedefs)) {
E 11
I 11
    if (!(ttp = nspace->ns_typedefs))
E 11
	ttp = nspace->ns_typedefs = new(struct type_table);
D 11
	bzero(nspace->ns_typedefs, sizeof(*ttp));
    }
E 11
    t->t_hash = ttp->tt_hash[hash_val];
    ttp->tt_hash[hash_val] = t;
I 3
    t->t_name = name;
I 9
    t->t_typedef = 1;
E 9
E 3
    ++ttp->tt_count;
}

void add_namedef(typeptr t, char *name)
{
    ns *nspace;
    typetabptr ttp;
    int hash_val = hash(name);
E 2
	
D 2
	if (tdef) {
	    ret->t_hash = ns->ns_typedefs[hash_val];
	    ns->ns_typedefs[hash_val] = ret;
	} else {
	    ret->t_hash = ns->ns_namedefs[hash_val];
	    ns->ns_namedefs[hash_val] = ret;
	}
    } else
	ret->t_hash = 0;
E 2
I 2
    if (!t) {
	fprintf(stderr, "Null namedef pointer %s:%d\n", __FILE__, __LINE__);
	return;
    }
E 2

I 4
    if (t->t_name) {			/* already entered */
	/* printf("name %s %s\n", name, t->t_ns->ns_name); */
	return;
    }

E 4
D 2
    ret->t_attrs = 0;
E 2
I 2
    nspace = t->t_ns;
D 11
    if (!(ttp = nspace->ns_namedefs)) {
E 11
I 11
    if (!(ttp = nspace->ns_namedefs))
E 11
	ttp = nspace->ns_namedefs = new(struct type_table);
D 11
	bzero(nspace->ns_namedefs, sizeof(*ttp));
    }
E 11
    t->t_hash = ttp->tt_hash[hash_val];
    ttp->tt_hash[hash_val] = t;
I 3
    t->t_name = name;
I 9
    t->t_typedef = 0;
E 9
E 3
    ++ttp->tt_count;
}

typeptr newtype(ns *nspace, enum stab_type t)
{
    typeptr ret = new(struct type);

D 11
    bzero(ret, sizeof(*ret));
E 11
E 2
    ret->t_type = t;
D 2
    ret->t_typedef = tdef;
    ret->t_ns = ns;
E 2
I 2
    ret->t_ns = nspace;
E 2
    return ret;
}

D 2
typeptr find_type(ns *ns, int typeid)
E 2
I 2
D 12
typeptr find_type(ns *nspace, int typeid)
E 12
I 12
typeptr find_type(ns *nspace, int type_id)
E 12
E 2
{
D 2
    if (typeid < 0) {
	typeid = -typeid;
	if (typeid < ns->ns_lower_limit)
	    return ns->ns_type_neg[typeid];
    } else if (typeid < ns->ns_upper_limit)
	return ns->ns_type_pos[typeid];
    return 0;
E 2
I 2
    typeptr ret = 0;
    tidtabptr tid;

D 12
    if (typeid == 0)
E 12
I 12
    if (type_id == 0)
E 12
	return 0;

    if (tid = nspace->ns_tids) {
D 12
	if (typeid < 0) {
	    int temp_typeid = -typeid;
E 12
I 12
	if (type_id < 0) {
	    int temp_type_id = -type_id;
E 12

D 12
	    if (temp_typeid < tid->tid_lower_limit)
		ret = tid->tid_type_neg[temp_typeid];
	} else if (typeid < tid->tid_upper_limit)
	    ret = tid->tid_type_pos[typeid];
E 12
I 12
	    if (temp_type_id < tid->tid_lower_limit)
		ret = tid->tid_type_neg[temp_type_id];
	} else if (type_id < tid->tid_upper_limit)
	    ret = tid->tid_type_pos[type_id];
E 12
    }
    /*
     * If this is a forward reference, we create and insert an empty
     * type node and then fill it in whenever it gets defined.
     */
    if (!ret) {
	ret = new(struct type);
D 11
	bzero(ret, sizeof(*ret));
E 11
	ret->t_ns = nspace;
D 12
	insert_type(typeid, ret);
E 12
I 12
	insert_type(type_id, ret);
E 12
    }
    return ret;
E 2
}

D 12
paramptr newparam(int typeid, int passby)
E 12
I 12
paramptr newparam(int type_id, int passby)
E 12
{
    paramptr ret = new(struct param);

    ret->p_next = 0;
D 12
    ret->p_typeid = typeid;
E 12
I 12
    ret->p_typeid = type_id;
E 12
    ret->p_passby = passby;
    return ret;
}

I 8
struct copy_rec {
    struct copy_rec *cr_next;
    typeptr cr_new;
    typeptr cr_old;
};

struct copy_rec *record_list;

D 12
void copy_type_and_record(typeptr new, typeptr old)
E 12
I 12
void copy_type_and_record(typeptr newp, typeptr oldp)
E 12
{
    struct copy_rec *cr = new(struct copy_rec);
    
D 12
    cr->cr_new = new;
    cr->cr_old = old;
E 12
I 12
    cr->cr_new = newp;
    cr->cr_old = oldp;
E 12
    cr->cr_next = record_list;
    record_list = cr;
D 12
    copy_type(new, old);
E 12
I 12
    copy_type(newp, oldp);
E 12
}

void finish_copies(void)
{
    struct copy_rec *cr;

    for (cr = record_list; cr; cr = record_list) {
	record_list = cr->cr_next;
	copy_type(cr->cr_new, cr->cr_old);
	free(cr);
    }
}

E 8
D 3
/*
 * Print out the thing that t points to.  The address is addr, offset is
 * a bit offset from addr (which may be greater than 32), size is the
 * size of the object, indent is the indent level for printouts, name
 * is the name of the field or variable, sname is the possible name of
 * the structure (e.g. struct toad { ...)
 */
void print_out(typeptr tptr, char *addr, int offset, int size, int indent,
	       char *name)
{
    fieldptr f;
    typeptr ltptr, rtptr;
    attrptr lattr;
    int val;
    int index, lower, upper, item_size, range;
    char *e_val;
    enumptr eptr;
D 2
    ns *ns = tptr->t_ns;
E 2
I 2
    ns *nspace = tptr->t_ns;
E 2
    double d;

    if (!size)
	size = get_size(tptr);

    switch (tptr->t_type) {
    case UNION_TYPE:
    case STRUCT_TYPE:
D 2
	printf("%*s%s", indent, "");
E 2
I 2
	printf("%*s", indent, "");
E 2
	print_name("", tptr);
	printf("{\n");
	
	for (f = tptr->t_val.val_s.s_fields; f; f = f->f_next)
D 2
	    print_out(find_type(ns, f->f_typeid), addr,
E 2
I 2
	    print_out(find_type(nspace, f->f_typeid), addr,
E 2
		      offset + f->f_offset, f->f_numbits, indent + 2,
		      f->f_name);

	if (name)
	    printf("%*s} %s;\n", indent, "", name);
	else
	    printf("%*s};", indent, "");
	break;

    case PTR_TYPE: /* pointer to some type but who cares what type it is */
	if (size != 32) {
	    fprintf(stderr, "size of pointer not 32 bits for %s\n", name);
	    exit(1);
	}
D 2
	item_size = get_size(ltptr = tptr->t_val.val_p);
E 2
I 2
	item_size = get_size(tptr->t_val.val_p);
E 2

	getval(&val, addr, offset, size);
	printf("%*s", indent, "");
	print_name(name, tptr);
I 2
	/*
	 * Special case for non-null pointers to char
	 */
E 2
	if (val && item_size == 8) {
	    char buf[32];
	    int i;

D 2
	    v_read(buf, (v_ptr)val, sizeof(buf));
E 2
I 2
	    BEGIN_PROTECT();
	    bcopy(v2f(val), buf, sizeof(buf));
E 2
	    for (i = 0; i < sizeof(buf); ++i)
		if (buf[i] < ' ' || buf[i] > 126)
		    break;
	    if (i == sizeof(buf) || buf[i] == '\0') {
		printf(" = 0x%08x => \"%.*s\"\n", val, sizeof(buf), buf);
D 2
		break;
E 2
I 2
		EXIT_PROTECT(break);
E 2
	    }
I 2
	    END_PROTECT();
E 2
	}
	printf(" = 0x%08x\n", val);
	break;

    case ARRAY_TYPE:
	ltptr = tptr->t_val.val_a.a_typeptr;
	if ((rtptr = tptr->t_val.val_a.a_typedef)->t_type != RANGE_TYPE) {
	    printf("bogus typedef for array for %s\n", name);
	    return;
	}
	lower = rtptr->t_val.val_r.r_lower;
	upper = rtptr->t_val.val_r.r_upper;
	range = upper - lower + 1;
	item_size = size / range;	/* first guess */

	for (lattr = ltptr->t_attrs; lattr; lattr = lattr->a_next)
	    if (lattr->a_type == SIZE_ATTR) {
		item_size = lattr->a_val;
		break;
	    }
	if (item_size * range != size) {
	    printf("bogus sizes, item_size = %d, count = %d, size = %d\n",
		   item_size, range, size);
	    return;
	}

	if (item_size == 8) {		/* assume array of char => string */
	    if (offset % 8) {
		printf("bogus offset for string of characters for %s\n", name);
		return;
	    }
	    printf("%*s", indent, "");
	    print_name(name, tptr);
	    printf(" = \"%.*s\"\n", range, addr + (offset / 8));
	    break;
	}

	for (index = lower; index <= upper; ++index) {
	    char buf[64];

	    sprintf(buf, "%s[%3i]", name, index);
	    print_out(ltptr, addr, offset, item_size, indent, buf);
	    offset += item_size;
	}
	break;

    case RANGE_TYPE:
D 2
	ltptr = tptr->t_val.val_r.r_typeptr;
E 2
	getval(&val, addr, offset, size);
	if (tptr->t_val.val_r.r_lower < 0) /* signed */
	    if (val & (1 << (size - 1))) /* test sign big */
		val |= (-1 << size);	/* smash in sign extension */
	printf("%*s", indent, "");
	print_name(name, tptr);
	if (size == 8)
	    printf(" = %d(0x%0*x)'%c'\n", val, (size +3) / 4,val, val);
	else
	    printf(" = %d(0x%0*x)\n", val, (size + 3) / 4, val);
	break;

    case PROC_TYPE:
	printf("%s bogus proc\n", name);
	break;

    case FLOAT_TYPE:
	if (size == sizeof(float) * 8)
	    d = *(float *)addr;
	else
	    d = *(double *)addr;
	
	printf("%*s", indent, "");
	print_name(name, tptr);
	printf(" = %f\n", d);
	break;

    case ENUM_TYPE:
	getval(&val, addr, offset, size);
	e_val = "UNKOWN";
	for (eptr = tptr->t_val.val_e; eptr; eptr = eptr->e_next)
	    if (eptr->e_val == val) {
		e_val = eptr->e_name;
		break;
	    }
	printf("%*s", indent, "");
	print_name(name, tptr);
	printf(" = %s(0x%08x)\n", e_val, val);
	break;

    case STRINGPTR_TYPE:
	printf("%s bogus stringptr\n", name);
	break;
    case COMPLEX_TYPE:
	printf("%s bogus complex\n", name);
	break;
I 2
    case CONSTANT_TYPE:
	printf("%*s", indent, "");
	printf("const ");
	print_out(tptr->t_val.val_k, addr, offset, size, 0, name);
	break;
E 2
    default:
	printf("%s bogus default\n", name);
	break;
    }
}

static void print_name(char *name, typeptr tptr)
{
    char buf[32];
    typeptr ltptr;
    char *lname;
    char *u_or_s;

    if (!tptr)
	printf("%s", name);

    switch (tptr->t_type) {
    case STRUCT_TYPE:
    case UNION_TYPE:
	u_or_s = tptr->t_type == STRUCT_TYPE ? "struct" : "union";
	if (tptr->t_name)
	    printf("%s %s %s", u_or_s, tptr->t_name, name);
	else
	    printf("%s %s", u_or_s, name);
	break;

    case PTR_TYPE:
	if ((ltptr = tptr->t_val.val_p) &&
	    (ltptr->t_type == ARRAY_TYPE || ltptr->t_type == PROC_TYPE))
	    sprintf(buf, "(*%s)", name);
	else
	    sprintf(buf, "*%s", name);
	print_name(buf, ltptr);
	break;

    case ARRAY_TYPE:
	if (!(lname = tptr->t_name))
	    lname = "";
	ltptr = tptr->t_val.val_a.a_typeptr;
	sprintf(buf, "%s[]", name);
	print_name(buf, ltptr);
	break;

    case RANGE_TYPE:
	if ((!(ltptr = tptr->t_val.val_r.r_typeptr) ||
	     !(lname = ltptr->t_name)) &&
	    !(lname = tptr->t_name))
	    lname = "";
	printf("%s %s", lname, name);
	break;

    case PROC_TYPE:
	ltptr = tptr->t_val.val_f.f_typeptr;
	sprintf(buf, "%s()", name);
	print_name(buf, ltptr);
	break;

    case FLOAT_TYPE:
	printf("%s %s", tptr->t_name, name);
	break;

    case ENUM_TYPE:
	if (tptr->t_name)
	    printf("enum %s %s", tptr->t_name, name);
	else
	    printf("enum %s", name);
	break;

    case STRINGPTR_TYPE:
	printf("strptr %s", name);
	break;
    case COMPLEX_TYPE:
	printf("complex %s", name);
	break;
I 2
    case CONSTANT_TYPE:
	printf("const %s", name);
	break;
    default:
	printf("unknown type %s", name);
	break;
E 2
    }
}

void getval(int *i, char *addr, int offset, int size)
{
    unsigned int temp;

    addr += offset / 8;
    offset %= 8;
    if (offset + size > 32) {
	fprintf(stderr, "getval spans an integer boundry\n");
	exit(1);
    }
    temp = *(int *)addr;
    temp >>= 32 - offset - size;
    temp &= (unsigned)-1 >> (32 - size);
    *i = temp;
}

E 3
D 12
void copy_type(typeptr new, typeptr old)
E 12
I 12
void copy_type(typeptr newp, typeptr oldp)
E 12
{
D 12
    new->t_type = old->t_type;
    new->t_val = old->t_val;
E 12
I 12
    newp->t_type = oldp->t_type;
    newp->t_val = oldp->t_val;
E 12
}

D 2
/* hash and store user defined names */
char *store_name(ns *ns, char *name)
E 2
I 2
/*
 * hash and store strings.  If len is non-zero, then it is the maximum
 * length of the string.
 */
char *store_string(ns *nspace, char *name, int len, char *suffix)
E 2
{
    int hash_val = hash(name);
D 2
    nameptr n;
E 2
I 2
    strtabptr stp = nspace->ns_strings;
    int name_len = strlen(name);
    int suffix_len = suffix ? strlen(suffix) : 0;
    stringptr st;
    char *result;
E 2

D 2
    for (n = ns->ns_names[hash_val]; n; n = n->n_hash)
	if (!strcmp(name, n->n_name))
	    return n->n_name;
E 2
I 2
    if (len && name_len > len)
	name_len = len;
E 2

D 2
    n = new(struct name);
    n->n_hash = ns->ns_names[hash_val];
    ns->ns_names[hash_val] = n;
    return n->n_name = strcpy(smalloc(strlen(name) + 1, __FILE__, __LINE__),
			      name);
E 2
I 2
D 11
    if (!stp) {
E 11
I 11
    if (!stp)
E 11
	stp = nspace->ns_strings = new(struct string_table);
D 11
	bzero(nspace->ns_strings, sizeof(*stp));
    }
E 11
    for (st = stp->st_hash[hash_val]; st; st = st->str_hash)
	if (!strncmp(name, st->str_name, name_len) &&
	    (suffix ?
	     !strcmp(suffix, st->str_name + name_len) :
	     st->str_name[name_len] == 0))
	    return st->str_name;

    st = new(struct string);
    st->str_hash = stp->st_hash[hash_val];
    stp->st_hash[hash_val] = st;
    /*
     * if len is set, we copy only up to len bytes and we put the null
     * terminater in by hand.
     */
D 6
    result = smalloc(name_len + suffix_len + 1, __FILE__, __LINE__);
E 6
I 6
    result = smalloc(name_len + suffix_len + 1);
E 6
    bcopy(name, result, name_len);
    result[name_len] = 0;
    if (suffix)
	strcat(result, suffix);
I 5
#if 0					/* Screws up filenames */
E 5
    if (result[0] == '.')
	result[0] = '$';
I 5
#endif
E 5
    ++stp->st_count;
    return st->str_name = result;
E 2
}

/*
 * Return the size in bits
 */
int get_size(typeptr t)
{
    typeptr l;
    attrptr a;
D 8
    double d;
E 8
I 8
    ularge_t r;
    ularge_t e;
E 8
D 2
    ns *ns = t->t_ns;
E 2
I 2
    ns *nspace = t->t_ns;
I 8
    int size;
E 8
E 2

    for (a = t->t_attrs; a; a = a->a_next)
	if (a->a_type == SIZE_ATTR)
	    return a->a_val;
    switch (t->t_type) {
    case STRUCT_TYPE:
    case UNION_TYPE:
	return t->t_val.val_s.s_size * 8;
    case PTR_TYPE:
	return sizeof(char *) * 8;
    case ARRAY_TYPE:
	l = t->t_val.val_a.a_typedef;
	if (l->t_type != RANGE_TYPE) {
D 2
	    fprintf(stderr, "non-range type for array");
E 2
I 2
	    fprintf(stderr, "non-range type for array\n");
E 2
	    exit(1);
	}
	return get_size(t->t_val.val_a.a_typeptr) *
D 8
	    (l->t_val.val_r.r_upper - l->t_val.val_r.r_lower + 1);
E 8
I 8
	    (atolarge(l->t_val.val_r.r_upper) -
	     atolarge(l->t_val.val_r.r_lower) +
	     1);
E 8
    case RANGE_TYPE:			/* no size for a range? */
D 8
	d = (double)t->t_val.val_r.r_upper - t->t_val.val_r.r_lower + 1;
	if (d <= 256)
	    return 8;
	else if (d <= (256 * 256))
	    return 16;
	else
E 8
I 8
	/*
	 * We special case the 32 and 64 bit cases to be safer
	 */
	if (is_zero(t->t_val.val_r.r_lower)) {
	    if (!strcmp(t->t_val.val_r.r_upper, "18446744073709551615"))
		return 64;
	    if (!strcmp(t->t_val.val_r.r_upper, "4294967295"))
		return 32;
	}
	if (!strcmp(t->t_val.val_r.r_lower, "-9223372036854775808") &&
	    !strcmp(t->t_val.val_r.r_upper, "9223372036854775807"))
	    return 64;
	if (!strcmp(t->t_val.val_r.r_lower, "-2147483648") &&
	    !strcmp(t->t_val.val_r.r_upper, "2147483647"))
E 8
	    return 32;
I 8
	r = atolarge(t->t_val.val_r.r_upper) -
	    atolarge(t->t_val.val_r.r_lower) + 1;
	for (e = 256, size = 8; e && e < r; ) {
	    e = e * e;
	    size <<= 1;
	}
	return size;
E 8
    case FLOAT_TYPE:
	return t->t_val.val_g.g_size * 8;
    case ENUM_TYPE:
	return sizeof(unsigned int) * 8;
    case PROC_TYPE:
	return 0;
I 2
    case CONSTANT_TYPE:
	return get_size(t->t_val.val_k);
I 3
    case VOLATILE:
	return get_size(t->t_val.val_v);
E 3
E 2
    default:
D 3
	fprintf(stderr, "bogus type for getsize\n");
E 3
I 3
	fprintf(stderr, "bogus type for get_size\n");
E 3
	exit(1);
    }
    return 0;
}

D 2
ns *ns_create(char *name)
E 2
I 2
ns *ns_create(ns *nspace, char *name)
E 2
{
    ns *ret = new(ns);

D 11
    bzero(ret, sizeof(*ret));
E 11
D 2
    if (ns_tail)
	ns_tail->ns_next = ret;
    else
	ns_head = ret;
    ns_tail = ret;
E 2
    ret->ns_name = name;
I 2
    if (ret->ns_parent = nspace) {
	if (nspace->ns_ltail)
	    nspace->ns_ltail->ns_next = ret;
	else
	    nspace->ns_lower = ret;
	nspace->ns_ltail = ret;

	ret->ns_strings = nspace->ns_strings;
	ret->ns_syms = nspace->ns_syms;
	ret->ns_typedefs = nspace->ns_typedefs;
	ret->ns_namedefs = nspace->ns_namedefs;
	ret->ns_tids = nspace->ns_tids;
    } else {
I 6
#if 0
E 6
	if (ns_tail)
	    ns_tail->ns_next = ret;
	else
	    ns_head = ret;
	ns_tail = ret;
I 6
#else
	ret->ns_next = ns_head;
	ns_head = ret;
	if (!ns_tail)
	    ns_tail = ret;
#endif
E 6
    }
E 2
    return ret;
}

I 2
void load_base_types(ns *nspace)
{
    static char *tp_array[] = TP_ARRAY;

    char **tpp;
    char **tpp_end;

    nspace->ns_typedefs = 0;
    nspace->ns_namedefs = 0;
    nspace->ns_tids = 0;
    for (tpp_end = (tpp = tp_array) + A_SIZE(tp_array); tpp < tpp_end; ++tpp)
	parse_stab(nspace, *tpp, 0, (symptr *)0);
}

void new_symbols(ns *nspace)
{
    nspace->ns_syms = 0;
}

E 2
ns *name2ns(char *name)
{
    ns *ret;

    for (ret = ns_head; ret && strcmp(ret->ns_name, name); ret = ret->ns_next);
    return ret;
I 2
}

symptr name2userdef(ns *nspace, char *name)
{
    symptr ret;
    symptr ret_not_typed = 0;
    symtabptr sytp;
    int h = hash(name);

    if (!(sytp = nspace->ns_syms))
	return 0;
I 6

E 6
    for (ret = sytp->sy_hash[h]; ret; ret = ret->s_hash)
	if (!strcmp(ret->s_name, name))
	    if (ret->s_typed)
		return ret;
	    else if (!ret_not_typed)
		ret_not_typed = ret;

    for (nspace = nspace->ns_lower; nspace; nspace = nspace->ns_next)
	if (ret = name2userdef(nspace, name))
	    if (ret->s_typed)
		return ret;
	    else if (!ret_not_typed)
		ret_not_typed = ret;
    return ret_not_typed;
}

symptr name2userdef_all(char *name)
{
    ns *nspace;
    symptr ret;
    symptr ret_not_typed = 0;
    
    for (nspace = ns_head; nspace; nspace = nspace->ns_next)
	if (ret = name2userdef(nspace, name))
	    if (ret->s_typed)
		return ret;
	    else if (!ret_not_typed)
		ret_not_typed = ret;
    return ret_not_typed;
I 3
}

I 11
void xxx(ns *nspace, int indent)
{
    printf("%*s%s\n", indent, "", nspace->ns_name);
    for (nspace = nspace->ns_lower; nspace; nspace = nspace->ns_next)
	xxx(nspace, indent + 2);
}

void view_namespaces(void)
{
    ns *nspace;

    for (nspace = ns_head; nspace; nspace = nspace->ns_next)
	xxx(nspace, 0);
}

E 11
/*
 * Find the user defined symbol with the highest address which is less
 * than or equal to addr.
 */
symptr addr2userdef(ns *nspace, void *addr)
{
    symptr *sym, *end, temp;
    symptr closest = 0;
    symtabptr sytp;

    if (!(sytp = nspace->ns_syms))
	return 0;
    
    for (end = (sym = sytp->sy_hash) + HASH_SIZE; sym < end; ++sym)
	for (temp = *sym; temp; temp = temp->s_hash)
	    if (temp->s_global && (temp->s_offset <= addr) &&
D 5
		(!closest || (closest->s_offset < temp->s_offset)))
E 5
I 5
		(!closest || (closest->s_offset < temp->s_offset ||
D 6
			      (closest->s_offset -= temp->s_offset &&
E 6
I 6
			      (closest->s_offset == temp->s_offset &&
E 6
			       closest->s_name[0] == '$'))))
E 5
		closest = temp;
    for (nspace = nspace->ns_lower; nspace; nspace = nspace->ns_next)
	if ((temp = addr2userdef(nspace, addr)) &&
D 5
	    (!closest || (closest->s_offset < temp->s_offset)))
E 5
I 5
	    (!closest || (closest->s_offset < temp->s_offset ||
			  (closest->s_offset == temp->s_offset &&
			   closest->s_name[0] == '$'))))
E 5
	    closest = temp;
    return closest;
}

symptr addr2userdef_all(void *addr)
{
    ns *nspace;
    symptr temp;
    symptr closest = 0;

    for (nspace = ns_head; nspace; nspace = nspace->ns_next)
	if ((temp = addr2userdef(nspace, addr)) &&
D 5
	    (!closest || (closest->s_offset < temp->s_offset)))
E 5
I 5
	    (!closest || (closest->s_offset < temp->s_offset ||
			  (closest->s_offset == temp->s_offset &&
			   closest->s_name[0] == '$'))))
E 5
	    closest = temp;
    return closest;
E 3
}

D 12
/* after leaving a nesting level, we delete the old symbols */
E 12
I 12
/* after leaving a nesting level, we delete the oldp symbols */
E 12
void clean_symtable(ns *nspace, int nesting_level)
{
    symptr *sym, *end, temp;
    symtabptr sytp;

    if (!(sytp = nspace->ns_syms))
	return;

    for (end = (sym = sytp->sy_hash) + HASH_SIZE; sym < end; ++sym)
	while ((temp = *sym) && temp->s_nesting > nesting_level) {
	    *sym = temp->s_hash;
	    free(temp);
	    --sytp->sy_count;
	}
}

D 4
symptr enter_sym(ns *nspace, char *name)
E 4
I 4
symptr enter_sym(ns *nspace, char *name, int force)
E 4
{
    int hash_val = hash(name);
    symtabptr sytp;
    symptr ret;
    
I 4
D 8
    if (!strcmp(name, "mstsave"))
	printf("name = %s(%08x), nspace = %s\n", name, name, nspace->ns_name);

E 8
E 4
D 11
    if (!(sytp = nspace->ns_syms)) {
E 11
I 11
    if (!(sytp = nspace->ns_syms))
E 11
	sytp = nspace->ns_syms = new(struct sym_table);
D 11
	bzero(nspace->ns_syms, sizeof(*sytp));
    }
E 11
    
    /*
     * In theory, we can just compare the name pointers because of the
     * string table.
     */
D 4
    for (ret = sytp->sy_hash[hash_val]; ret; ret = ret->s_hash)
	if (ret->s_name == name)
	    return ret;
E 4
I 4
    if (!force)
	for (ret = sytp->sy_hash[hash_val]; ret; ret = ret->s_hash)
D 11
	    if (ret->s_name == name)
E 11
I 11
	    if (ret->s_name == name) {
		DEBUG_PRINTF(("enter_sym: found %s\n", name));
E 11
		return ret;
I 11
	    }
E 11
E 4

I 11
D 12
    DEBUG_PRINTF(("enter_sym: new %s\n", name));
E 12
I 12
    DEBUG_PRINTF(("enter_sym: newp %s\n", name));
E 12
E 11
    ++sytp->sy_count;
I 11
    sytp->sy_addr_sorted = 0;
E 11
    ret = new(struct sym);
D 11
    bzero(ret, sizeof(*ret));
E 11
I 5
    ret->s_ns = nspace;
E 5
    ret->s_hash = sytp->sy_hash[hash_val];
    sytp->sy_hash[hash_val] = ret;
    ret->s_name = name;
    return ret;
}

D 8
static int sym_compare(void *v1, void *v2)
{
    symptr *p1 = v1;
    symptr *p2 = v2;
    return strcmp((*p1)->s_name, (*p2)->s_name);
}
	
static void print_sym(symptr s)
{
    typeptr t = s->s_type;
    typeptr oldt;

    printf("%s: ", s->s_name);
    while (t) {
	printf("%s%s", (t == s->s_type) ? "" : "->",
	       stab_type_2_string[(t->t_type > A_SIZE(stab_type_2_string) ||
				   t->t_type < 0) ?
				  (A_SIZE(stab_type_2_string) - 1) :
				  t->t_type]);
	oldt = t;
	switch (t->t_type) {
	case PTR_TYPE:
	    t = t->t_val.val_p;
	    break;
	case ARRAY_TYPE:
	    t = t->t_val.val_a.a_typeptr;
	    break;
	case RANGE_TYPE:
	    t = t->t_val.val_r.r_typeptr;
	    break;
	case PROC_TYPE:
	    t = t->t_val.val_f.f_typeptr;
	    break;
	case FLOAT_TYPE:
	    t = t->t_val.val_g.g_typeptr;
	    break;
	}
	if (oldt == t)
	    break;
    }
D 5
    printf("(%s), addr=%08x, size=%d, nesting=%d\n",
	   type_2_string[(s->s_base > LAST_TYPE || s->s_base < 0) ?
			 LAST_TYPE : s->s_base],
	   s->s_offset, s->s_size, s->s_nesting);
E 5
I 5
    if (s->s_base > LAST_TYPE || s->s_base < 0)
	printf("(%s:%d), addr=%08x, size=%d, nesting=%d\n",
	       "bad", s->s_base, s->s_offset, s->s_size, s->s_nesting);
    else
	printf("(%s), addr=%08x, size=%d, nesting=%d\n",
	       type_2_string[s->s_base], s->s_offset, s->s_size, s->s_nesting);
E 5
}

static void _dump_symtable(ns *nspace, symtabptr old_sytp)
{
    symtabptr sytp = nspace->ns_syms;
    symptr *all;
    symptr *a, *a_end;
    symptr s;
    int i;

    if (sytp) {
	if (sytp == old_sytp)
	    printf("Symbols for %s point to the parent\n", nspace->ns_name);
	else {
	    printf("Symbols for %s\n", nspace->ns_name);
D 6
	    a = all = (symptr *)smalloc(sizeof(symptr) * sytp->sy_count,
					__FILE__, __LINE__);
E 6
I 6
	    a = all = (symptr *)smalloc(sizeof(symptr) * sytp->sy_count);
E 6
	    a_end = a + sytp->sy_count;
	    for (i = 0; i < HASH_SIZE; i++)
		for (s = sytp->sy_hash[i]; s; s = s->s_hash)
		    if (a == a_end)
			printf("nspace->ns_symcount is off\n");
		    else
			*a++ = s;
	    qsort(all, sytp->sy_count, sizeof(symptr), sym_compare);
	    for (a = all; a < a_end; ++a)
		print_sym(*a);
	    free(all);
	}
    }

    for (nspace = nspace->ns_lower; nspace; nspace = nspace->ns_next)
	_dump_symtable(nspace, sytp);
}

E 8
void dump_symtable(void)
{
    ns *nspace;

D 11
    for (nspace = ns_head; nspace; nspace = nspace->ns_next)
E 11
I 11
    for (nspace = ns_head; nspace; nspace = nspace->ns_next) {
#if 1
E 11
	_dump_symtable(nspace, (symtabptr)0);
I 11
#else
	xxx(nspace, 0);
#endif
    }
E 11
D 8
    dump_stmts();
E 8
I 4
}

void dump_types(void)
{
I 8
    ns *nspace;

    for (nspace = ns_head; nspace; nspace = nspace->ns_next)
	_dump_types(nspace);
E 8
E 4
E 2
}
E 1
