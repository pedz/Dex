static char sccs_id[] = "@(#)sym.c	1.1";

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include "map.h"
#include "sym.h"
#include "tree.h"
#include "dex.h"

static ns *ns_head;
static ns *ns_tail;
static void print_name(char *name, typeptr tptr);

static int hash(char *s)
{
    int hash = 0;

    while (*s) {
	hash += *s++;
	hash <<= 1;
    }
    return hash & (HASH_SIZE - 1);
}

typeptr name2typedef(char *name)
{
    ns *ns;
    typeptr ret = 0;
    
    if (name)
	for (ns = ns_head; ns; ns = ns->ns_next)
	    for (ret = ns->ns_typedefs[hash(name)];
		 ret && strcmp(ret->t_name, name);
		 ret = ret->t_hash);
    return ret;
}

typeptr name2namedef(char *name)
{
    ns *ns;
    typeptr ret;
    
    for (ns = ns_head; ns; ns = ns->ns_next)
	for (ret = ns->ns_namedefs[hash(name)];
	     ret && strcmp(ret->t_name, name);
	     ret = ret->t_hash);
    return ret;
}

static do_insert(int typeid, typeptr t, int *limit, typeptr **ptr)
{
    if (typeid >= *limit) {
	*limit = typeid + 100;
	if (*ptr)
	    *ptr = realloc(*ptr, *limit * sizeof(typeptr));
	else
	    *ptr = smalloc(*limit * sizeof(typeptr), __FILE__, __LINE__);
	if (!*ptr) {
	    fprintf(stderr, "Out of memory\n");
	    exit(1);
	}
    }
    (*ptr)[typeid] = t;
}

void insert_type(int typeid, typeptr t)
{
    ns *ns = t->t_ns;
    if (typeid < 0)
	do_insert(-typeid, t, &ns->ns_lower_limit, &ns->ns_type_neg);
    else
	do_insert(typeid, t, &ns->ns_upper_limit, &ns->ns_type_pos);
}

int typedef2typeid(typeptr t)
{
    ns *ns = t->t_ns;
    typeptr *p, *e;

    if (p = ns->ns_type_neg)
	for (e = p + ns->ns_lower_limit; p < e; ++p)
	    if (*p == t)
		return -(p - ns->ns_type_neg);
    if (p = ns->ns_type_pos)
	for (e = p + ns->ns_upper_limit; p < e; ++p)
	    if (*p == t)
		return p - ns->ns_type_pos;
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

typeptr newtype(ns *ns, enum stab_type t, char *s, int tdef)
{
    typeptr ret = new(struct type);

    if (ret->t_name = s) {
	int hash_val = hash(s);
	
	if (tdef) {
	    ret->t_hash = ns->ns_typedefs[hash_val];
	    ns->ns_typedefs[hash_val] = ret;
	} else {
	    ret->t_hash = ns->ns_namedefs[hash_val];
	    ns->ns_namedefs[hash_val] = ret;
	}
    } else
	ret->t_hash = 0;

    ret->t_attrs = 0;
    ret->t_type = t;
    ret->t_typedef = tdef;
    ret->t_ns = ns;
    return ret;
}

typeptr find_type(ns *ns, int typeid)
{
    if (typeid < 0) {
	typeid = -typeid;
	if (typeid < ns->ns_lower_limit)
	    return ns->ns_type_neg[typeid];
    } else if (typeid < ns->ns_upper_limit)
	return ns->ns_type_pos[typeid];
    return 0;
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
    ns *ns = tptr->t_ns;
    double d;

    if (!size)
	size = get_size(tptr);

    switch (tptr->t_type) {
    case UNION_TYPE:
    case STRUCT_TYPE:
	printf("%*s%s", indent, "");
	print_name("", tptr);
	printf("{\n");
	
	for (f = tptr->t_val.val_s.s_fields; f; f = f->f_next)
	    print_out(find_type(ns, f->f_typeid), addr,
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
	item_size = get_size(ltptr = tptr->t_val.val_p);

	getval(&val, addr, offset, size);
	printf("%*s", indent, "");
	print_name(name, tptr);
	if (val && item_size == 8) {
	    char buf[32];
	    int i;

	    v_read(buf, (v_ptr)val, sizeof(buf));
	    for (i = 0; i < sizeof(buf); ++i)
		if (buf[i] < ' ' || buf[i] > 126)
		    break;
	    if (i == sizeof(buf) || buf[i] == '\0') {
		printf(" = 0x%08x => \"%.*s\"\n", val, sizeof(buf), buf);
		break;
	    }
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
	ltptr = tptr->t_val.val_r.r_typeptr;
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

/* hash and store user defined names */
char *store_name(ns *ns, char *name)
{
    int hash_val = hash(name);
    nameptr n;

    for (n = ns->ns_names[hash_val]; n; n = n->n_hash)
	if (!strcmp(name, n->n_name))
	    return n->n_name;

    n = new(struct name);
    n->n_hash = ns->ns_names[hash_val];
    ns->ns_names[hash_val] = n;
    return n->n_name = strcpy(smalloc(strlen(name) + 1, __FILE__, __LINE__),
			      name);
}

/*
 * Return the size in bits
 */
int get_size(typeptr t)
{
    typeptr l;
    attrptr a;
    double d;
    ns *ns = t->t_ns;

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
	    fprintf(stderr, "non-range type for array");
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
    default:
	fprintf(stderr, "bogus type for getsize\n");
	exit(1);
    }
    return 0;
}

ns *ns_create(char *name)
{
    ns *ret = new(ns);

    bzero(ret, sizeof(*ret));
    if (ns_tail)
	ns_tail->ns_next = ret;
    else
	ns_head = ret;
    ns_tail = ret;
    ret->ns_name = name;
    return ret;
}

ns *name2ns(char *name)
{
    ns *ret;

    for (ret = ns_head; ret && strcmp(ret->ns_name, name); ret = ret->ns_next);
    return ret;
}
