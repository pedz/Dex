static char sccs_id[] = "@(#)print.c	1.7";

#include <stdio.h>
#include <setjmp.h>
#include <stdlib.h>
#include <strings.h>
#include "map.h"
#include "sym.h"
#include "dex.h"
#include "tree.h"
#include "base_expr.h"
#define DEBUG_BIT PRINT_C_BIT

static int sym_compare(void *v1, void *v2);

static char *stab_type_names[] = {
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

static char *attr_type_names[] = {
    "align",
    "size",
    "ptr",
    "packed",
    "BOGUS",
};

static char *stab_name(enum stab_type s_type)
{
    const int max = A_SIZE(stab_type_names);
    return stab_type_names[(s_type >= max || s_type < 0) ? (max-1) : s_type];
}

static char *attr_name(enum attr_type a_type)
{
    const int max = A_SIZE(attr_type_names);
    return attr_type_names[(a_type >= max || a_type < 0) ? (max-1) : a_type];
}

void print_name(char *name, typeptr tptr)
{
    char buf[32];
    typeptr ltptr;
    char *lname;

    if (!tptr)
	printf("%s", name);

    switch (tptr->t_type) {
    case STRUCT_TYPE:
    case UNION_TYPE:
	if (tptr->t_typedef) {
	    printf("%s %s", tptr->t_name, name);
	} else {
	    char *u_or_s = tptr->t_type == STRUCT_TYPE ? "struct" : "union";
	    if (tptr->t_name)
		printf("%s %s %s", u_or_s, tptr->t_name, name);
	    else
		printf("%s %s", u_or_s, name);
	}
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
	if (!(lname = tptr->t_name) &&
	    (!(ltptr = tptr->t_val.val_r.r_typeptr) ||
	     !(lname = ltptr->t_name)))
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

/*
 * Print out the thing that t points to.  The address is addr, offset is
 * a bit offset from addr (which may be greater than 32), size is the
 * size of the object, indent is the indent level for printouts, name
 * is the name of the field or variable, sname is the possible name of
 * the structure (e.g. struct toad { ...)
 *
 * In the print routines, we assume that an array range can easily fit
 * within a large_t.
 */
void print_out(typeptr tptr, char *addr, int offset, int size, int indent,
	       char *name)
{
    fieldptr f;
    typeptr ltptr, rtptr;
    attrptr lattr;
    int eval;				/* enum's are int's */
    void *pval;				/* for pointers */
    large_t rval;
    large_t rval_signed;
    char *lower;
    char *upper;
    ularge_t range;
    large_t upper_index;
    large_t lower_index;
    int item_size, width;
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
	    print_out(f->f_typeptr, addr, offset + f->f_offset,
		      f->f_numbits, indent + 2, f->f_name);

	if (name)
	    printf("%*s} %s;\n", indent, "", name);
	else
	    printf("%*s};", indent, "");
	break;

    case PTR_TYPE: /* pointer to some type but who cares what type it is */
	if (size != sizeof(void *) * 8) {
	    fprintf(stderr, "size of pointer not %d bits for %s\n",
		    sizeof(void *) * 8, name);
	    exit(1);
	}
	item_size = get_size(tptr->t_val.val_p);

	pval = (void *)get_field(addr, offset, size);
	printf("%*s", indent, "");
	print_name(name, tptr);
	/*
	 * Special case for non-null pointers to char
	 */
	if (pval && item_size == 8) {
	    char buf[32];
	    int i;
	    volatile int had_fault;

	    BEGIN_PROTECT(&had_fault);
	    bcopy(v2f((void *)pval), buf, sizeof(buf));
	    for (i = 0; i < sizeof(buf); ++i)
		if (buf[i] < ' ' || buf[i] > 126)
		    break;
	    if (i == sizeof(buf) || buf[i] == '\0') {
		printf(" = 0x%s => \"%.*s\"\n", P(pval), sizeof(buf), buf);
		EXIT_PROTECT(break);
	    }
	    END_PROTECT();
	}
	printf(" = 0x%s\n", P(pval));
	break;

    case ARRAY_TYPE:
	ltptr = tptr->t_val.val_a.a_typeptr;
	if ((rtptr = tptr->t_val.val_a.a_typedef)->t_type != RANGE_TYPE) {
	    printf("bogus typedef for array for %s\n", name);
	    return;
	}
	lower_index = atolarge(lower = rtptr->t_val.val_r.r_lower);
	upper_index = atolarge(upper = rtptr->t_val.val_r.r_upper);
	if (!lower_index && !upper_index) { /* 0 size array from & op */
	    printf("%*s", indent, "");
	    print_name(name, tptr);
	    printf(" = 0x%s\n", P(addr));
	    break;
	}
	range = upper_index - lower_index + 1;
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

	if (lower_index < 0 && ((-lower_index) * 10) > upper_index)
	    upper_index = (-lower_index) * 10;
	for (width = 0; upper_index; ++width, upper_index /= 10);
	if (!width)
	    width = 1;

	upper_index = atolarge(upper);

	while (lower_index <= upper_index) {
	    char buf[64];

	    sprintf(buf, "%s[%*lld]", name, width, lower_index++);
	    print_out(ltptr, addr, offset, item_size, indent, buf);
	    offset += item_size;
	}
	break;

    case RANGE_TYPE:
	printf("%*s", indent, "");
	print_name(name, tptr);
	printf(" = ");

	while (size > 8 * sizeof(large_t)) {
	    rval = get_field(addr, offset, 8 * sizeof(large_t));
#ifdef _LONG_LONG
	    printf("%lld(0x%s), ", rval, P(rval));
#else
	    printf("%d(0x%s), ", rval, P(rval));
#endif
	    offset += 8 * sizeof(large_t);
	    size -= 8 * sizeof(large_t);
	}

	rval = get_field(addr, offset, size);
	rval_signed = rval;
	lower_index = atolarge(lower = tptr->t_val.val_r.r_lower);
	if (lower_index < 0) {		/* signed */
	    if (rval & (1 << (size - 1))) { /* test sign big */
		rval_signed |= ((large_t)-1 << size); /* smash in sign extension */
	    }
	}

#ifdef _LONG_LONG
	/*
	 * For the decimal value, we want the sign extended value.
	 * For the hex value, we want the original -- otherwise, we
	 * get tons of leadings 0xf's for negative numbers.
	 */
	if (size == 8)
	    printf("%lld(0x%0*llx)'%c'\n", rval_signed, (size + 3) / 4, rval,
		   (int)rval);
	else
	    printf("%lld(0x%0*llx)\n", rval_signed, (size + 3) / 4, rval);
#else
	/*
	 * I don't have a platform to test this on so I'll won't
	 * change it -- but it appears broken to me.
	 */
	if (size == 8)
	    printf("%d(0x%0*x)'%c'\n", rval, (size + 3) / 4, rval, rval);
	else
	    printf("%d(0x%0*x)\n", rval, (size + 3) / 4, rval);
#endif
	break;

    case PROC_TYPE:
	printf("%*s", indent, "");
	print_name(name, tptr);
	printf(" = 0x%s\n", P(addr));
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
	eval = get_field(addr, offset, size);
	e_val = "UNKOWN";
	for (eptr = tptr->t_val.val_e; eptr; eptr = eptr->e_next)
	    if (eptr->e_val == eval) {
		e_val = eptr->e_name;
		break;
	    }
	printf("%*s", indent, "");
	print_name(name, tptr);
	printf(" = %s(0x%s)\n", e_val, P(eval));
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
    case VOLATILE:
	printf("%*s", indent, "");
	printf("volatile ");
	print_out(tptr->t_val.val_v, addr, offset, size, 0, name);
	break;
    default:
	printf("%s bogus default %d\n", name, tptr->t_type);
	break;
    }
}

void print_sym(symptr s)
{
    typeptr t = s->s_type;
    typeptr oldt;

    printf("%s: ", s->s_name);
    while (t) {
	printf("%s%s", (t == s->s_type) ? "" : "->", stab_name(t->t_type));
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
	printf("(%s:%d), addr=%s, size=%d, nesting=%d\n",
	       "bad", s->s_base, P(s->s_offset), s->s_size, s->s_nesting);
    else
	printf("(%s), addr=%s, size=%d, nesting=%d\n",
	       type_2_string[s->s_base], P(s->s_offset), s->s_size,
	       s->s_nesting);
}

void _dump_symtable(ns *nspace, symtabptr old_sytp)
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

void _dump_type(typeptr t)
{
    print_name("", t);
    if (t->t_attrs) {
	struct attr *a;

	printf("attrs:");
	for (a = t->t_attrs; a; a = a->a_next)
	    printf(" %s:%d", attr_name(a->a_type), a->a_val);
    }
    if (t->t_type == RANGE_TYPE)
	printf("%s->%s", t->t_val.val_r.r_lower, t->t_val.val_r.r_upper);
    printf("\n");
}

void _dump_types(ns *nspace)
{
    typetabptr ttp;
    typeptr t;
    int i;

    if (ttp = nspace->ns_typedefs) {
	printf("Typedefs from %s\n", nspace->ns_name);
	for (i = 0; i < HASH_SIZE; ++i) {
	    t = ttp->tt_hash[i];
	    while (t) {
		_dump_type(t);
		t = t->t_hash;
	    }
	}
    }

    if (ttp = nspace->ns_namedefs) {
	printf("Structure names from %s\n", nspace->ns_name);
	for (i = 0; i < HASH_SIZE; ++i) {
	    t = ttp->tt_hash[i];
	    while (t) {
		_dump_type(t);
		t = t->t_hash;
	    }
	}
    }

    for (nspace = nspace->ns_lower; nspace; nspace = nspace->ns_next)
	_dump_types(nspace);
}

static int sym_compare(void *v1, void *v2)
{
    symptr *p1 = v1;
    symptr *p2 = v2;
    return strcmp((*p1)->s_name, (*p2)->s_name);
}
