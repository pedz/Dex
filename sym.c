static char sccs_id[] = "@(#)sym.c	1.6";

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

static int hash(char *s);
static typeptr do_insert(int typeid, typeptr t, int *limit, typeptr **ptr);
static int sym_compare(void *v1, void *v2);
static void print_sym(symptr s);
static void _dump_symtable(ns *nspace, symtabptr old_sytp);

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

typeptr name2namedef(ns *nspace, char *name)
{
    typeptr ret;
    typetabptr ttp;

    if (ttp = nspace->ns_namedefs)
	for (ret = ttp->tt_hash[hash(name)]; ret; ret = ret->t_hash)
	    if (!strcmp(ret->t_name, name))
		return ret;

    for (nspace = nspace->ns_lower; nspace; nspace = nspace->ns_next)
	if (ret = name2namedef(nspace, name))
	    return ret;

    return 0;
}

typeptr name2namedef_all(char *name)
{
    ns *nspace;
    typeptr ret;
    
    for (nspace = ns_head; nspace; nspace = nspace->ns_next)
	if (ret = name2namedef(nspace, name))
	    return ret;

    return 0;
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
    double d;
    ns *nspace = t->t_ns;

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
	    (l->t_val.val_r.r_upper - l->t_val.val_r.r_lower + 1);
    case RANGE_TYPE:			/* no size for a range? */
	d = (double)t->t_val.val_r.r_upper - t->t_val.val_r.r_lower + 1;
	if (d <= 256)
	    return 8;
	else if (d <= (256 * 256))
	    return 16;
	else
	    return 32;
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
    
    if (!strcmp(name, "mstsave"))
	printf("name = %s(%08x), nspace = %s\n", name, name, nspace->ns_name);

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
    if (s->s_base > LAST_TYPE || s->s_base < 0)
	printf("(%s:%d), addr=%08x, size=%d, nesting=%d\n",
	       "bad", s->s_base, s->s_offset, s->s_size, s->s_nesting);
    else
	printf("(%s), addr=%08x, size=%d, nesting=%d\n",
	       type_2_string[s->s_base], s->s_offset, s->s_size, s->s_nesting);
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
	    a = all = (symptr *)smalloc(sizeof(symptr) * sytp->sy_count);
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

void dump_symtable(void)
{
    ns *nspace;

    for (nspace = ns_head; nspace; nspace = nspace->ns_next)
	_dump_symtable(nspace, (symtabptr)0);
    dump_stmts();
}

void dump_types(void)
{
}
