static char sccs_id[] = "@(#)base_expr.c	1.4";

#include <stdio.h>
#include <setjmp.h>
#include "map.h"
#include "sym.h"
#include "tree.h"
#include "base_expr.h"

#define WSIZE (8 * sizeof(unsigned int))
static void print_name(char *name, typeptr tptr);

unsigned int get_field(void *addr, int offset, int size)
{
    unsigned int *laddr = addr;
    unsigned int temp;

    laddr += (offset / WSIZE);
    offset %= WSIZE;
    if (offset + size > WSIZE) {
	fprintf(stderr, "getval spans an integer boundry\n");
	exit(1);
    }
    temp = *laddr;
    temp >>= WSIZE - offset - size;
    temp &= (unsigned)-1 >> (WSIZE - size);
    return temp;
}

void set_field(void *addr, int offset, int size, unsigned int val)
{
    long laddr = (long)addr;
    unsigned int temp;

    laddr += (offset / 8);
    offset %= WSIZE;
    if (offset + size > WSIZE) {
	fprintf(stderr, "getval spans an integer boundry\n");
	exit(1);
    }
    temp = *(unsigned int *)laddr;
    temp &= (((unsigned int)-1 >> (WSIZE - size)) << (WSIZE - offset - size));
    temp |= ((val & ((unsigned int)-1 >> (WSIZE - size))) <<
	     (WSIZE - offset - size));
    *(unsigned int *)laddr = temp;
    return;
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
	if (!lower && !upper) {		/* 0 size array probably from & op */
	    printf("%*s", indent, "");
	    print_name(name, tptr);
	    printf(" = 0x%08x\n", f2v(addr));
	    break;
	}
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
	val = get_field(addr, offset, size);
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
