static char sccs_id[] = "@(#)print.c	1.2";

#include <stdio.h>
#include <setjmp.h>
#include "map.h"
#include "sym.h"
#include "dex.h"
#include "tree.h"
#include "base_expr.h"

static int sym_compare(void *v1, void *v2);

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

void print_name(char *name, typeptr tptr)
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
    int val;
    large_t rval;
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
	if (size != 32) {
	    fprintf(stderr, "size of pointer not 32 bits for %s\n", name);
	    exit(1);
	}
	item_size = get_size(tptr->t_val.val_p);

	val = get_field(addr, offset, size);
	printf("%*s", indent, "");
	print_name(name, tptr);
	/*
	 * Special case for non-null pointers to char
	 */
	if (val && item_size == 8) {
	    char buf[32];
	    int i;
	    volatile int had_fault;

	    BEGIN_PROTECT(&had_fault);
	    bcopy(v2f((void *)val), buf, sizeof(buf));
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
	lower_index = atolarge(lower = rtptr->t_val.val_r.r_lower);
	upper_index = atolarge(upper = rtptr->t_val.val_r.r_upper);
	if (!lower_index && !upper_index) { /* 0 size array probably from & op */
	    printf("%*s", indent, "");
	    print_name(name, tptr);
	    printf(" = 0x%08x\n", f2v(addr));
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
	    printf("%lld(0x%08llx), ", rval, rval);
#else
	    printf("%d(0x%08x), ", rval, rval);
#endif
	    offset += 8 * sizeof(large_t);
	    size -= 8 * sizeof(large_t);
	}

	rval = get_field(addr, offset, size);
	if (tptr->t_val.val_r.r_lower < 0) /* signed */
	    if (rval & (1 << (size - 1))) /* test sign big */
		rval |= (-1 << size);	/* smash in sign extension */

#ifdef _LONG_LONG
	if (size == 8)
	    printf("%lld(0x%0*llx)'%c'\n", rval, (size + 3) / 4, rval, (int)rval);
	else
	    printf("%lld(0x%0*llx)\n", rval, (size + 3) / 4, rval);
#else
	if (size == 8)
	    printf("%d(0x%0*x)'%c'\n", rval, (size + 3) / 4, rval, rval);
	else
	    printf("%d(0x%0*x)\n", rval, (size + 3) / 4, rval);
#endif
	break;

    case PROC_TYPE:
	printf("%*s", indent, "");
	print_name(name, tptr);
	printf(" = 0x%08x\n", f2v(addr));
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
	val = get_field(addr, offset, size);
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
    printf("\n");
}

void _dump_types(ns *nspace)
{
    typetabptr ttp;
    typeptr t;
    int i;

    if (ttp = nspace->ns_typedefs) {
	printf("Typedefs\n");
	for (i = 0; i < HASH_SIZE; ++i) {
	    t = ttp->tt_hash[i];
	    while (t) {
		_dump_type(t);
		t = t->t_hash;
	    }
	}
    }

    if (ttp = nspace->ns_namedefs) {
	printf("Structure names\n");
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
