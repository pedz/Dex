static char sccs_id[] = "@(#)sym.c	1.10";

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <dbxstclass.h>
#include "map.h"
#include "sym.h"
#include "dex.h"

static ns *ns_head;
static ns *ns_tail;

extern char *type_2_string[];		/* Ick! */

static int hash(char *s);
static typeptr do_insert(int typeid, typeptr t, int *limit, typeptr **ptr);

static int hash(char *s)
{
    int hash = 0;
    int c;

    while (c = *s++) {
	hash <<= 1;
	hash += c;
    }
    return hash & (HASH_SIZE - 1);
}

typeptr name2typedef(ns *nspace, char *name)
{
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
    
    if (name)
	for (nspace = ns_head; nspace; nspace = nspace->ns_next)
	    if (ret = name2typedef(nspace, name))
		return ret;
    return 0;
}

/*
 * A special patch was added in the case of structures.  If the first
 * thing we find is a structure but it has zero size, we keep
 * searching.  If we do not find anything else, we return what we
 * found at first.  But if we find something more interesting, like a
 * structure of non-zero size, we return that.  We have to do this for
 * name2namedef as well as name2namedef_all.
 */
typeptr name2namedef(ns *nspace, char *name)
{
    typeptr ret;
    typetabptr ttp;
    typeptr struct_temp = 0;

    if (ttp = nspace->ns_namedefs)
	for (ret = ttp->tt_hash[hash(name)]; ret; ret = ret->t_hash)
	    if (!strcmp(ret->t_name, name))
		if (ret->t_type == STRUCT_TYPE && ret->t_val.val_s.s_size == 0)
		    struct_temp = ret;
		else
		    return ret;

    for (nspace = nspace->ns_lower; nspace; nspace = nspace->ns_next)
	if (ret = name2namedef(nspace, name))
	    if (ret->t_type == STRUCT_TYPE && ret->t_val.val_s.s_size == 0)
		struct_temp = ret;
	    else
		return ret;

    return struct_temp;
}

typeptr name2namedef_all(char *name)
{
    ns *nspace;
    typeptr ret;
    typeptr struct_temp = 0;
    
    for (nspace = ns_head; nspace; nspace = nspace->ns_next)
	if (ret = name2namedef(nspace, name))
	    if (ret->t_type == STRUCT_TYPE && ret->t_val.val_s.s_size == 0)
		struct_temp = ret;
	    else
		return ret;

    return struct_temp;
}

static typeptr do_insert(int typeid, typeptr t, int *limit, typeptr **ptr)
{
    int old_limit;
    typeptr old_type;

    if (typeid >= (old_limit = *limit)) {
	*limit = typeid + 100;
	if (*ptr)
	    *ptr = srealloc(*ptr, *limit * sizeof(typeptr));
	else
	    *ptr = smalloc(*limit * sizeof(typeptr));
	bzero(*ptr + old_limit, sizeof(typeptr) * (*limit - old_limit));
    }

    if (old_type = (*ptr)[typeid]) {
	char *old_name = old_type->t_name;
	struct type *old_hash = old_type->t_hash;

	if (t->t_name) {		/* if this has a name then we freak */
	    printf("Replacing %s name of %s:%d\n", t->t_name, t->t_ns->ns_name,
		   typeid);
	    exit(1);
	}

	*old_type = *t;			/* copy new into old */
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
#endif
	return old_type;
    } else
	(*ptr)[typeid] = t;
    return t;
}

typeptr insert_type(int typeid, typeptr t)
{
    ns *nspace = t->t_ns;
    tidtabptr tid;

    /*
     * A typeid of 0 seems to be for temp ranges.
     */
    if (typeid == 0)
	return t;
    if (!(tid = nspace->ns_tids)) {
	tid = nspace->ns_tids = new(struct typeid_table);
	bzero(nspace->ns_tids, sizeof(*tid));
    }
    if (typeid < 0)
	return do_insert(-typeid, t, &tid->tid_lower_limit, &tid->tid_type_neg);
    else
	return do_insert(typeid, t, &tid->tid_upper_limit, &tid->tid_type_pos);
}

int typedef2typeid(typeptr t)
{
    ns *nspace = t->t_ns;
    typeptr *p, *e;
    tidtabptr tid;

    if (!(tid = nspace->ns_tids))
	return 0;
    if (p = tid->tid_type_neg)
	for (e = p + tid->tid_lower_limit; p < e; ++p)
	    if (*p == t)
		return -(p - tid->tid_type_neg);
    if (p = tid->tid_type_pos)
	for (e = p + tid->tid_upper_limit; p < e; ++p)
	    if (*p == t)
		return p - tid->tid_type_pos;
    return 0;
}

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

fieldptr newfield(char *name, typeptr tptr, int offset, int numbits)
{
    fieldptr ret = new(struct field);

    ret->f_next = 0;
    ret->f_name = name;
    ret->f_typeptr = tptr;
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

void add_typedef(typeptr t, char *name)
{
    ns *nspace;
    typetabptr ttp;
    int hash_val = hash(name);

    if (!t) {
	fprintf(stderr, "Null typedef pointer %s:%d\n", __FILE__, __LINE__);
	return;
    }

    if (t->t_name) {			/* already entered */
	/* printf("type %s %s\n", name, t->t_ns->ns_name); */
	return;
    }

    nspace = t->t_ns;
    if (!(ttp = nspace->ns_typedefs)) {
	ttp = nspace->ns_typedefs = new(struct type_table);
	bzero(nspace->ns_typedefs, sizeof(*ttp));
    }
    t->t_hash = ttp->tt_hash[hash_val];
    ttp->tt_hash[hash_val] = t;
    t->t_name = name;
    t->t_typedef = 1;
    ++ttp->tt_count;
}

void add_namedef(typeptr t, char *name)
{
    ns *nspace;
    typetabptr ttp;
    int hash_val = hash(name);
	
    if (!t) {
	fprintf(stderr, "Null namedef pointer %s:%d\n", __FILE__, __LINE__);
	return;
    }

    if (t->t_name) {			/* already entered */
	/* printf("name %s %s\n", name, t->t_ns->ns_name); */
	return;
    }

    nspace = t->t_ns;
    if (!(ttp = nspace->ns_namedefs)) {
	ttp = nspace->ns_namedefs = new(struct type_table);
	bzero(nspace->ns_namedefs, sizeof(*ttp));
    }
    t->t_hash = ttp->tt_hash[hash_val];
    ttp->tt_hash[hash_val] = t;
    t->t_name = name;
    t->t_typedef = 0;
    ++ttp->tt_count;
}

typeptr newtype(ns *nspace, enum stab_type t)
{
    typeptr ret = new(struct type);

    bzero(ret, sizeof(*ret));
    ret->t_type = t;
    ret->t_ns = nspace;
    return ret;
}

typeptr find_type(ns *nspace, int typeid)
{
    typeptr ret = 0;
    tidtabptr tid;

    if (typeid == 0)
	return 0;

    if (tid = nspace->ns_tids) {
	if (typeid < 0) {
	    int temp_typeid = -typeid;

	    if (temp_typeid < tid->tid_lower_limit)
		ret = tid->tid_type_neg[temp_typeid];
	} else if (typeid < tid->tid_upper_limit)
	    ret = tid->tid_type_pos[typeid];
    }
    /*
     * If this is a forward reference, we create and insert an empty
     * type node and then fill it in whenever it gets defined.
     */
    if (!ret) {
	ret = new(struct type);
	bzero(ret, sizeof(*ret));
	ret->t_ns = nspace;
	insert_type(typeid, ret);
    }
    return ret;
}

paramptr newparam(int typeid, int passby)
{
    paramptr ret = new(struct param);

    ret->p_next = 0;
    ret->p_typeid = typeid;
    ret->p_passby = passby;
    return ret;
}

struct copy_rec {
    struct copy_rec *cr_next;
    typeptr cr_new;
    typeptr cr_old;
};

struct copy_rec *record_list;

void copy_type_and_record(typeptr new, typeptr old)
{
    struct copy_rec *cr = new(struct copy_rec);
    
    cr->cr_new = new;
    cr->cr_old = old;
    cr->cr_next = record_list;
    record_list = cr;
    copy_type(new, old);
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

void copy_type(typeptr new, typeptr old)
{
    new->t_type = old->t_type;
    new->t_val = old->t_val;
}

/*
 * hash and store strings.  If len is non-zero, then it is the maximum
 * length of the string.
 */
char *store_string(ns *nspace, char *name, int len, char *suffix)
{
    int hash_val = hash(name);
    strtabptr stp = nspace->ns_strings;
    int name_len = strlen(name);
    int suffix_len = suffix ? strlen(suffix) : 0;
    stringptr st;
    char *result;

    if (len && name_len > len)
	name_len = len;

    if (!stp) {
	stp = nspace->ns_strings = new(struct string_table);
	bzero(nspace->ns_strings, sizeof(*stp));
    }
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
    result = smalloc(name_len + suffix_len + 1);
    bcopy(name, result, name_len);
    result[name_len] = 0;
    if (suffix)
	strcat(result, suffix);
#if 0					/* Screws up filenames */
    if (result[0] == '.')
	result[0] = '$';
#endif
    ++stp->st_count;
    return st->str_name = result;
}

/*
 * Return the size in bits
 */
int get_size(typeptr t)
{
    typeptr l;
    attrptr a;
    ularge_t r;
    ularge_t e;
    ns *nspace = t->t_ns;
    int size;

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
	    fprintf(stderr, "non-range type for array\n");
	    exit(1);
	}
	return get_size(t->t_val.val_a.a_typeptr) *
	    (atolarge(l->t_val.val_r.r_upper) -
	     atolarge(l->t_val.val_r.r_lower) +
	     1);
    case RANGE_TYPE:			/* no size for a range? */
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
	    return 32;
	r = atolarge(t->t_val.val_r.r_upper) -
	    atolarge(t->t_val.val_r.r_lower) + 1;
	for (e = 256, size = 8; e && e < r; ) {
	    e = e * e;
	    size <<= 1;
	}
	return size;
    case FLOAT_TYPE:
	return t->t_val.val_g.g_size * 8;
    case ENUM_TYPE:
	return sizeof(unsigned int) * 8;
    case PROC_TYPE:
	return 0;
    case CONSTANT_TYPE:
	return get_size(t->t_val.val_k);
    case VOLATILE:
	return get_size(t->t_val.val_v);
    default:
	fprintf(stderr, "bogus type for get_size\n");
	exit(1);
    }
    return 0;
}

ns *ns_create(ns *nspace, char *name)
{
    ns *ret = new(ns);

    bzero(ret, sizeof(*ret));
    ret->ns_name = name;
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
#if 0
	if (ns_tail)
	    ns_tail->ns_next = ret;
	else
	    ns_head = ret;
	ns_tail = ret;
#else
	ret->ns_next = ns_head;
	ns_head = ret;
	if (!ns_tail)
	    ns_tail = ret;
#endif
    }
    return ret;
}

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

ns *name2ns(char *name)
{
    ns *ret;

    for (ret = ns_head; ret && strcmp(ret->ns_name, name); ret = ret->ns_next);
    return ret;
}

symptr name2userdef(ns *nspace, char *name)
{
    symptr ret;
    symptr ret_not_typed = 0;
    symtabptr sytp;
    int h = hash(name);

    if (!(sytp = nspace->ns_syms))
	return 0;

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
}

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
		(!closest || (closest->s_offset < temp->s_offset ||
			      (closest->s_offset == temp->s_offset &&
			       closest->s_name[0] == '$'))))
		closest = temp;
    for (nspace = nspace->ns_lower; nspace; nspace = nspace->ns_next)
	if ((temp = addr2userdef(nspace, addr)) &&
	    (!closest || (closest->s_offset < temp->s_offset ||
			  (closest->s_offset == temp->s_offset &&
			   closest->s_name[0] == '$'))))
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
	    (!closest || (closest->s_offset < temp->s_offset ||
			  (closest->s_offset == temp->s_offset &&
			   closest->s_name[0] == '$'))))
	    closest = temp;
    return closest;
}

/* after leaving a nesting level, we delete the old symbols */
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

symptr enter_sym(ns *nspace, char *name, int force)
{
    int hash_val = hash(name);
    symtabptr sytp;
    symptr ret;
    
    if (!(sytp = nspace->ns_syms)) {
	sytp = nspace->ns_syms = new(struct sym_table);
	bzero(nspace->ns_syms, sizeof(*sytp));
    }
    
    /*
     * In theory, we can just compare the name pointers because of the
     * string table.
     */
    if (!force)
	for (ret = sytp->sy_hash[hash_val]; ret; ret = ret->s_hash)
	    if (ret->s_name == name)
		return ret;

    ++sytp->sy_count;
    ret = new(struct sym);
    bzero(ret, sizeof(*ret));
    ret->s_ns = nspace;
    ret->s_hash = sytp->sy_hash[hash_val];
    sytp->sy_hash[hash_val] = ret;
    ret->s_name = name;
    return ret;
}

void dump_symtable(void)
{
    ns *nspace;

    for (nspace = ns_head; nspace; nspace = nspace->ns_next)
	_dump_symtable(nspace, (symtabptr)0);
}

void dump_types(void)
{
    ns *nspace;

    for (nspace = ns_head; nspace; nspace = nspace->ns_next)
	_dump_types(nspace);
}
