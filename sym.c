static char sccs_id[] = "@(#)sym.c	1.2";

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <setjmp.h>
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
static void print_name(char *name, typeptr tptr);
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
	if (*ptr) {
	    if (!(*ptr = realloc(*ptr, *limit * sizeof(typeptr)))) {
		fprintf(stderr, "Out of memory\n");
		exit(1);
	    }
	} else
	    *ptr = smalloc(*limit * sizeof(typeptr), __FILE__, __LINE__);
	bzero(*ptr + old_limit, sizeof(typeptr) * (*limit - old_limit));
    }

    if (old_type = (*ptr)[typeid]) {
	*old_type = *t;			/* copy new into old */
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

fieldptr newfield(char *name, int typeid, int offset, int numbits)
{
    fieldptr ret = new(struct field);

    ret->f_next = 0;
    ret->f_name = name;
    ret->f_typeid = typeid;
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

    nspace = t->t_ns;
    if (!(ttp = nspace->ns_typedefs)) {
	ttp = nspace->ns_typedefs = new(struct type_table);
	bzero(nspace->ns_typedefs, sizeof(*ttp));
    }
    t->t_hash = ttp->tt_hash[hash_val];
    ttp->tt_hash[hash_val] = t;
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

    nspace = t->t_ns;
    if (!(ttp = nspace->ns_namedefs)) {
	ttp = nspace->ns_namedefs = new(struct type_table);
	bzero(nspace->ns_namedefs, sizeof(*ttp));
    }
    t->t_hash = ttp->tt_hash[hash_val];
    ttp->tt_hash[hash_val] = t;
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
    ns *nspace = tptr->t_ns;
    double d;

    if (!size)
	size = get_size(tptr);

    switch (tptr->t_type) {
    case UNION_TYPE:
    case STRUCT_TYPE:
	printf("%*s", indent, "");
	print_name("", tptr);
	printf("{\n");
	
	for (f = tptr->t_val.val_s.s_fields; f; f = f->f_next)
	    print_out(find_type(nspace, f->f_typeid), addr,
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
	item_size = get_size(tptr->t_val.val_p);

	getval(&val, addr, offset, size);
	printf("%*s", indent, "");
	print_name(name, tptr);
	/*
	 * Special case for non-null pointers to char
	 */
	if (val && item_size == 8) {
	    char buf[32];
	    int i;

	    BEGIN_PROTECT();
	    bcopy(v2f(val), buf, sizeof(buf));
	    for (i = 0; i < sizeof(buf); ++i)
		if (buf[i] < ' ' || buf[i] > 126)
		    break;
	    if (i == sizeof(buf) || buf[i] == '\0') {
		printf(" = 0x%08x => \"%.*s\"\n", val, sizeof(buf), buf);
		EXIT_PROTECT(break);
	    }
	    END_PROTECT();
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
    case CONSTANT_TYPE:
	printf("%*s", indent, "");
	printf("const ");
	print_out(tptr->t_val.val_k, addr, offset, size, 0, name);
	break;
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
    case CONSTANT_TYPE:
	printf("const %s", name);
	break;
    default:
	printf("unknown type %s", name);
	break;
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
    result = smalloc(name_len + suffix_len + 1, __FILE__, __LINE__);
    bcopy(name, result, name_len);
    result[name_len] = 0;
    if (suffix)
	strcat(result, suffix);
    if (result[0] == '.')
	result[0] = '$';
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
    default:
	fprintf(stderr, "bogus type for getsize\n");
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
	if (ns_tail)
	    ns_tail->ns_next = ret;
	else
	    ns_head = ret;
	ns_tail = ret;
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

symptr enter_sym(ns *nspace, char *name)
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
    for (ret = sytp->sy_hash[hash_val]; ret; ret = ret->s_hash)
	if (ret->s_name == name)
	    return ret;

    ++sytp->sy_count;
    ret = new(struct sym);
    bzero(ret, sizeof(*ret));
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
    printf("(%s), addr=%08x, size=%d, nesting=%d\n",
	   type_2_string[(s->s_base > LAST_TYPE || s->s_base < 0) ?
			 LAST_TYPE : s->s_base],
	   s->s_offset, s->s_size, s->s_nesting);
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
	    a = all = (symptr *)smalloc(sizeof(symptr) * sytp->sy_count,
					__FILE__, __LINE__);
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
