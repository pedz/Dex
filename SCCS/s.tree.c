h15015
s 00030/00002/01130
d D 1.11 10/08/23 17:25:59 pedzan 11 10
c Checking in changes made over the years
e
s 00178/00066/00954
d D 1.10 02/03/14 16:12:34 pedz 10 9
e
s 00000/00000/01020
d D 1.9 00/09/18 17:57:03 pedz 9 8
c Checking before V5 conversion
e
s 00009/00001/01011
d D 1.8 00/05/29 15:09:08 pedz 8 7
c Just before IA64 conversion
e
s 00014/00001/00998
d D 1.7 00/02/08 17:36:48 pedz 7 6
e
s 00035/00012/00964
d D 1.6 98/10/23 12:26:13 pedz 6 5
e
s 00025/00009/00951
d D 1.5 97/05/13 16:03:00 pedz 5 4
c Check pointer
e
s 00040/00031/00920
d D 1.4 95/04/25 10:10:28 pedz 4 3
e
s 00238/00106/00713
d D 1.3 95/02/09 21:36:04 pedz 3 2
e
s 00139/00435/00680
d D 1.2 95/02/01 10:37:13 pedz 2 1
e
s 01115/00000/00000
d D 1.1 94/08/22 17:56:36 pedz 1 0
c date and time created 94/08/22 17:56:36 by pedz
e
u
U
t
T
I 1
static char sccs_id[] = "%W%";

D 2
/*
 * TODO's
 * #### pointer arithmatic
 * #### redo compare ops (?)
 * #### bit fields
 * #### duplicate symbols
 * #### local symbols
 */

E 2
#include <stdio.h>
#include <stdlib.h>
I 2
#include <sys/signal.h>
I 3
#include <dbxstclass.h>
I 10
#include <strings.h>

E 10
I 6
#include "dex.h"
E 6
E 3
E 2
#include "map.h"
#include "sym.h"
#include "tree.h"
#include "stmt.h"
#include "gram.h"
#include "asgn_expr.h"
#include "base_expr.h"
I 3
#include "binary_expr.h"
#include "cast_expr.h"
#include "unary_expr.h"
E 3
#include "inter.h"
D 2
#include "hmap.h"
E 2
D 6
#include "dex.h"
E 6
#include "fcall.h"
I 10
#include "print.h"
#include "gram_pre.h"
E 10

I 10
#define DEBUG_BIT TREE_C_BIT

E 10
/*
 * promotions is indexed with two expr_types to determine the type to
 * be used in an expression.  A value of bad_type is used for illegal
 * combinations.
 */
static enum expr_type promotions[LAST_TYPE + 1][LAST_TYPE + 1];

/*
 * See bottom of tree.h.  OP_MAX is a somewhat arbitrary number.  If
 * it is set too small, the init code will complain and die.  TOK_MAX
 * is the largest token number in gram.h.  I haven't figured out a way
 * to get the information from that file into this file.  Again, if it
 * is too small, the init code will complain.
 */
D 2
#define OP_MAX 33
E 2
I 2
D 3
#define OP_MAX 40
E 3
I 3
#define OP_MAX 45
E 3
E 2
#define TOK_MAX 316
static int tok_2_op[TOK_MAX];
static char *op_2_string[OP_MAX];
D 2
static char *type_2_string[LAST_TYPE + 1];
E 2
static union all_func op_table[LAST_TYPE + 1][OP_MAX];
static union all_func cast_table[LAST_TYPE + 1][LAST_TYPE + 1];
static union all_func null_func[LAST_TYPE + 1];
I 11
static int base_size_table[LAST_TYPE + 1];
E 11

D 2
static int global_index = h_base;
static symptr current_function;
static int param_index;
static int variable_index;
static int variable_max;
static int type_number;
static int nesting_level;
E 2
I 2
char *type_2_string[LAST_TYPE + 1];
E 2

/* routines to build expression trees */

expr *new_expr(void)
{
D 3
    return new(struct expr);
E 3
I 3
D 10
    expr *ret = new(struct expr);
    bzero(ret, sizeof(*ret));
    return ret;
E 10
I 10
    return new(struct expr);
E 10
E 3
}

D 2
#define set_addrfunc(enode, type, suffix) \
    switch (type) { \
    case schar_type: \
	enode->e_func.sca = sc_ ## suffix ; \
	break ; \
    case uchar_type: \
	enode->e_func.uca = uc_ ## suffix ; \
	break ; \
    case int_type: \
	enode->e_func.ia = i_ ## suffix ; \
	break ; \
    case uint_type: \
	enode->e_func.uia = ui_ ## suffix ; \
	break ; \
    case short_type: \
	enode->e_func.sa = s_ ## suffix ; \
	break ; \
    case ushort_type: \
	enode->e_func.usa = us_ ## suffix ; \
	break ; \
    case long_type: \
	enode->e_func.la = l_ ## suffix ; \
	break ; \
    case ulong_type: \
	enode->e_func.ula = ul_ ## suffix ; \
	break ; \
    case float_type: \
	enode->e_func.fa = f_ ## suffix ; \
	break ; \
    case double_type: \
	enode->e_func.da = d_ ## suffix ; \
	break ; \
    case struct_type: \
	enode->e_func.sta = st_ ## suffix ; \
	break ; \
    default: \
	fprintf(stderr, "bad type %d in set_addrfunc for %s:%d", type, \
		__FILE__, __LINE__); \
	break; \
    }

#define set_func(enode, type, suffix) \
    switch (type) { \
    case schar_type: \
	enode->e_func.sc = sc_ ## suffix ; \
	break ; \
    case uchar_type: \
	enode->e_func.uc = uc_ ## suffix ; \
	break ; \
    case int_type: \
	enode->e_func.i = i_ ## suffix ; \
	break ; \
    case uint_type: \
	enode->e_func.ui = ui_ ## suffix ; \
	break ; \
    case short_type: \
	enode->e_func.s = s_ ## suffix ; \
	break ; \
    case ushort_type: \
	enode->e_func.us = us_ ## suffix ; \
	break ; \
    case long_type: \
	enode->e_func.l = l_ ## suffix ; \
	break ; \
    case ulong_type: \
	enode->e_func.ul = ul_ ## suffix ; \
	break ; \
    case float_type: \
	enode->e_func.f = f_ ## suffix ; \
	break ; \
    case double_type: \
	enode->e_func.d = d_ ## suffix ; \
	break ; \
    case struct_type: \
	enode->e_func.st = st_ ## suffix ; \
	break ; \
    default: \
	fprintf(stderr, "bad type %d in set_func for %s:%d", type, \
		__FILE__, __LINE__); \
	break; \
    }

E 2
D 3
int mkident(cnode *result, char *s)
E 3
I 3
int mk_ident(cnode *result, char *s)
E 3
{
D 2
    symptr sym = name2userdef(s);	/* find user symbol */
E 2
I 2
    symptr sym = name2userdef_all(s);	/* find user symbol */
E 2
    expr *eptr;

    if (!sym)
	return 1;
    result->c_type = sym->s_type;
    result->c_base = sym->s_base;
D 6
    eptr = result->c_expr = new_expr();
E 6
I 6
    /*
     * I think this is always the case.
     */
    result->c_bitfield = 0;
    if (result->c_expr = sym->s_expr)
	return 0;

    eptr = result->c_expr = sym->s_expr = new_expr();
E 6
D 2
    if (sym->s_global) {
	set_addrfunc(eptr, sym->s_base, gaddr);
    } else {
	set_addrfunc(eptr, sym->s_base, laddr);
    }
E 2
I 2
    eptr->e_func = op_table[sym->s_base][tok_2_op[sym->s_global ? 'g' : 'l' ]];
E 2
    eptr->e_addr = sym->s_offset;
    eptr->e_size = sym->s_size;
    return 0;
}

D 3
void mkl2p(cnode *result, cnode *c)
E 3
I 3
void mk_l2p(cnode *result, cnode *c)
E 3
{
    expr *eptr;

I 2
D 3
    if (c->c_type->t_type == ARRAY_TYPE) {
	mk_v2f(result, c);
	return;
    }
E 3
E 2
    *result = *c;
D 2
    if (c->c_type->t_type != ARRAY_TYPE &&
	c->c_type->t_type != PROC_TYPE) {
E 2
I 2
D 3
    if (c->c_type->t_type != PROC_TYPE) {
E 3
I 3
    if (c->c_type->t_type != PROC_TYPE &&
	c->c_type->t_type != ARRAY_TYPE) {
E 3
E 2
	eptr = result->c_expr = new_expr();
	eptr->e_func = op_table[c->c_base][tok_2_op['.']];
	eptr->e_left = c->c_expr;
	eptr->e_size = c->c_expr->e_size;
I 7
D 10
	PRINTF("mk_l2p %d %08x\n", eptr->e_size, eptr);
E 10
I 10
D 11
	DEBUG_PRINTF(("mk_l2p %d %s\n", eptr->e_size, P(eptr)));
E 11
I 11
	DEBUG_PRINTF(("mk_l2p %d %d %s\n", eptr->e_size, c->c_bitfield, P(eptr)));
E 11
E 10
E 7
I 3
	if (c->c_bitfield) {
	    eptr->e_bsize = c->c_size;
	    eptr->e_boffset = c->c_offset;
	} else {
	    eptr->e_bsize = 0;
	    eptr->e_boffset = 0;
	}
E 3
    }
}

I 8
#ifdef Not_Used
E 8
I 2
void mk_f2v(cnode *result, cnode *c)
{
    typeptr t;
    expr *eptr;

    t = newtype(c->c_type->t_ns, PTR_TYPE);
    t->t_val.val_p = c->c_type;
    result->c_type = t;
    result->c_base = base_type(result->c_type);
    result->c_const = 1;
I 3
    result->c_bitfield = 0;
E 3
    eptr = result->c_expr = new_expr();
    eptr->e_func = op_table[c->c_base][tok_2_op['f']];
    eptr->e_left = c->c_expr;
    eptr->e_size = get_size(t) / 8;
}

int mk_v2f(cnode *result, cnode *c)
{
    expr *eptr;

    if (c->c_type->t_type == PTR_TYPE)
	result->c_type = c->c_type->t_val.val_p;
    else if (c->c_type->t_type == ARRAY_TYPE)
	result->c_type = c->c_type->t_val.val_a.a_typeptr;
    else
	return 1;
    result->c_base = base_type(result->c_type);
    result->c_const = 0;
I 3
    result->c_bitfield = 0;
E 3
    eptr = result->c_expr = new_expr();
    eptr->e_func = op_table[c->c_base][tok_2_op['v']];
    eptr->e_left = c->c_expr;
    eptr->e_size = get_size(result->c_type) / 8;
    return 0;
}
I 8
#endif
E 8

E 2
D 3
int mkdot(cnode *result, cnode *c, char *s)
E 3
I 3
int mk_dot(cnode *result, cnode *c, char *s)
E 3
{
    typeptr t = c->c_type;
    fieldptr f;
D 2
    ns *ns;
E 2
I 2
    ns *nspace;
E 2
D 3
    expr *c_expr, *r_expr;
E 3
I 3
    int byte_offset;
    int byte_size;
    int bit_offset;
    int bit_size;
E 3

    if (t->t_type != STRUCT_TYPE && t->t_type != UNION_TYPE) {
D 4
	fprintf(stderr, "struct or union expected\n");
E 4
I 4
	GRAMerror("struct or union expected");
E 4
	return 1;
    }
D 2
    ns = t->t_ns;
E 2
I 2
    nspace = t->t_ns;
E 2
    for (f = t->t_val.val_s.s_fields;
	 f && strcmp(f->f_name, s);
	 f = f->f_next);
    if (!f) {
D 4
	fprintf(stderr, "field %s not part of structure %s\n", s,
		t->t_name);
E 4
I 4
	GRAMerror("field %s not part of structure %s", s, t->t_name);
E 4
	return 1;
    }

D 3
    c_expr = new_expr();
    c_expr->e_func.ul = ul_leaf;
    c_expr->e_i = f->f_offset / 8;
    c_expr->e_size = f->f_numbits / 8;
E 3
I 3
    /*
     * If the offset or the number of bits is not divisible by 8, then
     * we simply set the cnode properly and continue.  This will
     * eventually get changed into either a pvalue or used as an
     * lvalue and we will deal with the problem at that time.
I 11
     *
     * Bug fix: If we have a bit field, the traditional type is
     * unsigned int but if the bit field is a multiple of 8 bits yet
     * smaller than the size denoted by c_base, we still need to use
     * the get_field operations otherwise, we end up picking up the
     * entire int instead of just the bytes we are suppose to pick
     * up.  We added base_size_table and the second or test in the if
     * below to catch this.
E 11
     */
    result->c_type = f->f_typeptr;
    result->c_base = base_type(result->c_type);
I 11
    DEBUG_PRINTF(("mk_dot: numbits: %d, type: %d, true size: %d \n",
		  f->f_numbits, result->c_base, base_size_table[result->c_base]));
E 11
E 3

D 3
    /* Make the plus node */
    r_expr = new_expr();
    r_expr->e_func.ul = ul_plus;
    r_expr->e_left = c->c_expr;
    r_expr->e_right = c_expr;
    r_expr->e_size = f->f_numbits / 8;
E 3
I 3
D 5
    /* If we need to use bit ops */
E 5
I 5
    /*
     * The bit field routines are designed to work with an address
     * that is word aligned.  The other routines, like to read and
     * write a short or a byte, can operate on any byte boundry.  The
     * test for the need of a bit field is based upon either the
     * offset not being on a byte boundry or the size of the field not
     * being a multiple of bytes.
     *
     * So in the case of a bit field, byte_offset will be forced to be
     * a multiple of sizeof(int).
     */
    byte_offset = f->f_offset / 8;
E 5
D 11
    if ((f->f_offset | f->f_numbits) & 7) {
E 11
I 11
    if (((f->f_offset | f->f_numbits) & 7) ||
	(base_size_table[result->c_base] * 8 != f->f_numbits)) {
E 11
D 5
	byte_offset = f->f_offset / (sizeof(int) * 8);
E 5
I 5
	byte_offset &= ~(sizeof(int) - 1); /* make it word aligned */
	bit_offset = f->f_offset - (byte_offset * 8);
E 5
	byte_size = sizeof(int);
D 5
	bit_offset = f->f_offset - (byte_offset * sizeof(int) * 8);
E 5
	bit_size = f->f_numbits;
    } else {
	bit_offset = 0;
	bit_size = 0;
D 5
	byte_offset = f->f_offset / 8;
E 5
	byte_size = f->f_numbits / 8;
    }
E 3

I 3
    if (byte_offset) {
	expr *c_expr, *r_expr;

	c_expr = new_expr();
	c_expr->e_func.ul = ul_leaf;
D 5
	c_expr->e_i = f->f_offset / 8;
E 5
I 5
D 8
	c_expr->e_i = byte_offset;
E 8
I 8
	c_expr->e_ul = byte_offset;
E 8
E 5
	c_expr->e_size = f->f_numbits / 8;

	/* Make the plus node */
	r_expr = new_expr();
	r_expr->e_func.ul = ul_plus;
	r_expr->e_left = c->c_expr;
	r_expr->e_right = c_expr;
	r_expr->e_size = f->f_numbits / 8;
	result->c_expr = r_expr;
D 6
    } else
E 6
I 6
    } else {
E 6
	result->c_expr = c->c_expr;
I 6
	/*
	 * The result is not the size of the structure but the size of
	 * this field -- except if it is an array in which case the
	 * size is the size of an address.
	 */
	result->c_expr->e_size = 
D 10
	  (f->f_typeptr->t_type == ARRAY_TYPE) ?
	  sizeof(char *) :
	  f->f_numbits / 8;
E 10
I 10
	    (f->f_typeptr->t_type == ARRAY_TYPE) ?
	    sizeof(char *) :
	    f->f_numbits / 8;
E 10
    }
E 6

E 3
    /* set the result */
D 2
    result->c_type = find_type(ns, f->f_typeid);
E 2
I 2
D 3
    result->c_type = find_type(nspace, f->f_typeid);
E 2
    result->c_base = base_type(result->c_type);
E 3
    result->c_const = c->c_const;
D 3
    result->c_expr = r_expr;
E 3
I 3
    if (result->c_bitfield = (bit_size != 0))
	result->c_const = 0;
    result->c_offset = bit_offset;
    result->c_size = bit_size;
E 3
    return 0;
}

D 3
int mkptr(cnode *result, cnode *c, char *s)
E 3
I 3
int mk_ptr(cnode *result, cnode *c, char *s)
E 3
{
    typeptr t = c->c_type;
    cnode x;

D 5
    if (t->t_type != PTR_TYPE) {
D 4
	fprintf(stderr, "not a pointer type\n");
E 4
I 4
	GRAMerror("not a pointer type");
E 5
I 5
    if (t->t_type == PTR_TYPE) {
	mk_l2p(&x, c);
	x.c_type = t->t_val.val_p;
    } else if (t->t_type != ARRAY_TYPE) {
	mk_l2p(&x, c);
	x.c_type = t->t_val.val_a.a_typeptr;
    } else {
	GRAMerror("not a pointer or array type");
E 5
E 4
	return 1;
    }
D 3
    mkl2p(&x, c);
E 3
I 3
D 5
    mk_l2p(&x, c);
E 3
    x.c_type = t->t_val.val_p;
E 5
D 3
    return mkdot(result, &x, s);
E 3
I 3
    return mk_dot(result, &x, s);
E 3
}

D 3
int mkarray(cnode *result, cnode *array, cnode *index)
E 3
I 3
int mk_array(cnode *result, cnode *array, cnode *index)
E 3
{
    int rc;
I 3
    cnode temp;
E 3

D 3
    if (array->c_type->t_type != ARRAY_TYPE) {
	fprintf(stderr, "array type needed for subscripting\n");
E 3
I 3
    if (array->c_type->t_type != ARRAY_TYPE &&
	array->c_type->t_type != PTR_TYPE) {
D 4
	fprintf(stderr, "array or pointer type needed for subscripting\n");
E 4
I 4
	GRAMerror("array or pointer type needed for subscripting");
E 4
E 3
	return 1;
    }
D 3
    if (rc = mkbinary(result, array, '+', index))
E 3
I 3
    /*
     * Currently in the case of an array, this does nothing but it may
     * need to convert address space in the future.
     */
    mk_l2p(&temp, array);
    if (rc = mk_binary(result, &temp, '+', index))
E 3
	return rc;
D 3
    result->c_type = array->c_type->t_val.val_a.a_typeptr;
E 3
I 3
    if (array->c_type->t_type == ARRAY_TYPE)
	result->c_type = array->c_type->t_val.val_a.a_typeptr;
    else
	result->c_type = array->c_type->t_val.val_p;
E 3
    result->c_base = base_type(result->c_type);
    result->c_const = 0;
I 3
    result->c_bitfield = 0;
E 3
    return 0;
}

D 3
int mkasgn(cnode *result, cnode *lvalue, int opcode, cnode *rvalue)
E 3
I 3
int mk_asgn(cnode *result, cnode *lvalue, int opcode, cnode *rvalue)
E 3
{
    typeptr t = lvalue->c_type;
    expr *eptr = new_expr();
    int rc = 0;

    if ((opcode == PLUSEQUAL || opcode == MINUSEQUAL ||
	 opcode == INCOP || opcode == DECOP) && t->t_type == PTR_TYPE) {
	expr *newr = new_expr();
	expr *size = new_expr();

	/* Create expression for sizeof(*lvalue) */
	size->e_func.i = i_leaf;
	size->e_i = get_size(t->t_val.val_p) / 8;
	size->e_size = sizeof(int);

	/* Create multiply node of rvalue times sizeof(*lvalue) */
	newr->e_left = size;
	newr->e_right = cast_to_int(rvalue);
	newr->e_func = op_table[lvalue->c_base][tok_2_op['*']];
	newr->e_size = sizeof(int);

	/* Create addition node of base plus offset */
D 3
	eptr->e_left = lvalue->c_expr;
E 3
	eptr->e_right = newr;
D 3
	eptr->e_func = op_table[lvalue->c_base][tok_2_op[opcode]];
	eptr->e_size = lvalue->c_expr->e_size;
E 3
    } else {
	cnode right = *rvalue;

D 2
	cast_to(&right, t);
E 2
I 2
	cast_to(&right, t, lvalue->c_base);
E 2
D 3
	eptr->e_left = lvalue->c_expr;
E 3
	eptr->e_right = right.c_expr;
D 3
	eptr->e_func = op_table[lvalue->c_base][tok_2_op[opcode]];
	if (!eptr->e_func.sc) {
	    fprintf(stderr, "`%s' op is illegal for %s's\n",
		    op_2_string[tok_2_op[opcode]],
		    type_2_string[lvalue->c_base]);
	    eptr->e_func = null_func[lvalue->c_base];
	    rc = 1;
	}
	eptr->e_size = lvalue->c_expr->e_size;
E 3
    }
I 3

    eptr->e_left = lvalue->c_expr;
    eptr->e_func = op_table[lvalue->c_base][tok_2_op[opcode]];
    if (!eptr->e_func.sc) {
D 4
	fprintf(stderr, "`%s' op is illegal for %s's\n",
		op_2_string[tok_2_op[opcode]], type_2_string[lvalue->c_base]);
E 4
I 4
	GRAMerror("`%s' op is illegal for %s's",
D 10
		  op_2_string[tok_2_op[opcode]], type_2_string[lvalue->c_base]);
E 10
I 10
		  op_2_string[tok_2_op[opcode]],
		  type_2_string[lvalue->c_base]);
E 10
E 4
	eptr->e_func = null_func[lvalue->c_base];
	rc = 1;
    }
    eptr->e_size = lvalue->c_expr->e_size;
    if (lvalue->c_bitfield) {
	eptr->e_boffset = lvalue->c_offset;
	eptr->e_bsize = lvalue->c_size;
    } else {
	eptr->e_boffset = 0;
	eptr->e_bsize = 0;
    }
E 3
    result->c_type = t;
    result->c_base = lvalue->c_base;
D 3
    result->c_const = 0;
E 3
    result->c_expr = eptr;
I 3
    result->c_const = 0;
    result->c_bitfield = 0;
E 3
    return rc;
}

D 3
int mkbinary(cnode *result, cnode *lvalue, int opcode, cnode *rvalue)
E 3
I 3
int mk_unary(cnode *result, cnode *lvalue, int opcode)
E 3
{
I 3
    expr *eptr;

    *result = *lvalue;
    result->c_expr = eptr = new_expr();
    eptr->e_left = lvalue->c_expr;
    eptr->e_size = lvalue->c_expr->e_size;
    eptr->e_func = op_table[lvalue->c_base][tok_2_op[opcode]];
    if (!eptr->e_func.sc) {
	eptr->e_func = null_func[lvalue->c_base];
	return 1;
    }
    return 0;
}

int mk_binary(cnode *result, cnode *lvalue, int opcode, cnode *rvalue)
{
E 3
    int lptr = (lvalue->c_type->t_type == PTR_TYPE ||
		lvalue->c_type->t_type == ARRAY_TYPE);
    int rptr = (rvalue->c_type->t_type == PTR_TYPE ||
		rvalue->c_type->t_type == ARRAY_TYPE);
    cnode right = *rvalue;
    cnode left = *lvalue;
    enum expr_type htype;
    typeptr totype;
    expr *eptr;
I 4
    int ptr_is_ok = !(opcode == ',' || opcode == '>' || opcode == '<' || 
		      opcode == GTOREQUAL || opcode == LTOREQUAL ||
		      opcode == EQUALITY || opcode == NOTEQUAL ||
		      opcode == ANDAND || opcode == OROR);
E 4

I 3
    /*
D 4
     * Check for pointer subtraction first.
E 4
I 4
     * Check for pointer subtraction first.  The list of opcodes below
     * are those which are legal to do but do not require any special
     * code.
E 4
     */
D 4
    if (lptr && rptr) {
E 4
I 4
    if (lptr && rptr && ptr_is_ok) {

E 4
	expr *newr;
	expr *size;
	typeptr ltype;
	typeptr rtype;

	if (opcode != '-') {
D 4
	    fprintf(stderr, "ptr %s ptr is illegal\n",
		    op_2_string[tok_2_op[opcode]]);
E 4
I 4
	    GRAMerror("ptr %s ptr is illegal", op_2_string[tok_2_op[opcode]]);
E 4
	    return 1;
	}
I 4

E 4
	ltype = ((left.c_type->t_type == PTR_TYPE) ?
		 left.c_type->t_val.val_p :
		 left.c_type->t_val.val_a.a_typeptr);
	rtype = ((right.c_type->t_type == PTR_TYPE) ?
		 right.c_type->t_val.val_p :
		 right.c_type->t_val.val_a.a_typeptr);
D 6
	if (rtype != ltype) {
E 6
I 6
	if (rtype != ltype && get_size(ltype) != get_size(rtype)) {
E 6
D 4
	    fprintf(stderr, "pointer subtraction of different types not legal");
E 4
I 4
	    GRAMerror("pointer subtraction of different types not legal");
E 4
	    return 1;
	}

	/*
	 * Result will be a long
	 */
I 8
#ifdef __64BIT__
	result->c_type = find_type(left.c_type->t_ns, TP_LONG_64);
#else
E 8
	result->c_type = find_type(left.c_type->t_ns, TP_LONG);
I 8
#endif
E 8
	result->c_base = long_type;
	result->c_expr = eptr = new_expr();
	result->c_const = left.c_const && right.c_const;
	result->c_bitfield = 0;

	/* Create expression for sizeof(*lvalue) */
D 4
	size->e_func.i = i_leaf;
	size->e_i = get_size(ltype) / 8;
	size->e_size = sizeof(int);
E 4
I 4
	size = new_expr();
	size->e_func.l = l_leaf;
	size->e_l = get_size(ltype) / 8;
	size->e_size = sizeof(long);
E 4
	
	/* Create subtraction node of two pointers */
D 4
	newr->e_func = op_table[left.c_base][tok_2_op['-']];
E 4
I 4
	newr = new_expr();
	newr->e_func = op_table[long_type][tok_2_op['-']];
E 4
	newr->e_left = left.c_expr;
	newr->e_right = right.c_expr;
D 4
	newr->e_size = sizeof(int);
E 4
I 4
	newr->e_size = sizeof(long);
E 4

	/* Create divide node of new rvalue by sizeof(*lvalue) */
D 4
	eptr->e_func = op_table[left.c_base][tok_2_op['/']];
E 4
I 4
	eptr->e_func = op_table[long_type][tok_2_op['/']];
E 4
	eptr->e_left = newr;
	eptr->e_right = size;
D 4
	eptr->e_size = sizeof(int);
E 4
I 4
	eptr->e_size = sizeof(long);
	return 0;
E 4
    }

E 3
D 4
    if (lptr || rptr) {
E 4
I 4
    if ((lptr || rptr) && ptr_is_ok) {
E 4
D 3
	expr *newr = new_expr();
	expr *size = new_expr();
E 3
I 3
	expr *newr;
	expr *size;
E 3

	if (rptr) {
	    /* Now we have ptr + blah or ptr - blah */
	    left = *rvalue;
	    right = *lvalue;
	}

I 3
	newr = new_expr();
	size = new_expr();

E 3
	result->c_type = left.c_type;
	result->c_base = left.c_base;
D 3
	result->c_const = left.c_const && right.c_const;
E 3
	result->c_expr = eptr = new_expr();
I 3
	result->c_const = left.c_const && right.c_const;
	result->c_bitfield = 0;
E 3
	eptr->e_func = null_func[left.c_base];

	if (opcode != '+' && opcode != '-') {
D 4
	    fprintf(stderr, "'%s' op is illegal with pointers\n",
		    op_2_string[tok_2_op[opcode]]);
E 4
I 4
	    GRAMerror("'%s' op is illegal with pointers",
		      op_2_string[tok_2_op[opcode]]);
E 4
	    return 1;
	}

D 3
	if (rptr && lptr) {
	    fprintf(stderr, "ptr %s ptr is illegal\n",
		    op_2_string[tok_2_op[opcode]]);
	    return 1;
	}
	
E 3
	/* Create expression for sizeof(*lvalue) */
	size->e_func.i = i_leaf;
	size->e_i = get_size((left.c_type->t_type == PTR_TYPE) ?
			     left.c_type->t_val.val_p :
			     left.c_type->t_val.val_a.a_typeptr) / 8;
	size->e_size = sizeof(int);

	/* Create multiply node of rvalue times sizeof(*lvalue) */
	newr->e_func = op_table[left.c_base][tok_2_op['*']];
	newr->e_left = size;
	newr->e_right = cast_to_int(&right);
	newr->e_size = sizeof(int);

	/* Create addition node of base plus offset */
	eptr->e_func = op_table[left.c_base][tok_2_op[opcode]];
	eptr->e_left = left.c_expr;
	eptr->e_right = newr;
	eptr->e_size = left.c_expr->e_size;
	return 0;
    }

    htype = promotions[lvalue->c_base][rvalue->c_base];
    totype = ((htype == lvalue->c_base) ? lvalue->c_type : rvalue->c_type);
D 2
    cast_to(&right, totype);
    cast_to(&left, totype);
E 2
I 2
    cast_to(&right, totype, htype);
    cast_to(&left, totype, htype);
E 2

    result->c_type = totype;
    result->c_base = htype;
D 3
    result->c_const = right.c_const && left.c_const;
E 3
    result->c_expr = eptr = new_expr();
I 3
    result->c_const = right.c_const && left.c_const;
    result->c_bitfield = 0;
E 3
    eptr->e_left = left.c_expr;
    eptr->e_right = right.c_expr;
    eptr->e_func = op_table[htype][tok_2_op[opcode]];
    eptr->e_size = get_size(result->c_type) / 8;
    if (!eptr->e_func.sc) {
D 4
	fprintf(stderr, "`%s' op is illegal for %s's\n",
		op_2_string[tok_2_op[opcode]], type_2_string[htype]);
E 4
I 4
	GRAMerror("`%s' op is illegal for %s's",
		  op_2_string[tok_2_op[opcode]], type_2_string[htype]);
E 4
	eptr->e_func = null_func[htype];
	return 1;
    }
    return 0;
}

void mk_fcall(cnode *result, cnode *name, cnode_list *args)
{
    expr *eptr;

    *result = *name;
    eptr = result->c_expr = new_expr();
D 2
    set_func(eptr, name->c_base, fcall);
E 2
I 2
    eptr->e_func = op_table[name->c_base][tok_2_op['x']];
E 2
    eptr->e_call = name->c_expr;
    eptr->e_args = args;
    eptr->e_size = get_size(result->c_type) / 8;
}

int mk_qc_op(cnode *result, cnode *qvalue, cnode *tvalue, cnode *fvalue)
{
    cnode true = *tvalue;
    cnode false = *fvalue;
    enum expr_type htype;
    typeptr totype;
    expr *eptr;

    htype = promotions[tvalue->c_base][fvalue->c_base];
    totype = ((htype == tvalue->c_base) ? tvalue->c_type : fvalue->c_type);
D 2
    cast_to(&false, totype);
    cast_to(&true, totype);
E 2
I 2
    cast_to(&false, totype, htype);
    cast_to(&true, totype, htype);
E 2

    result->c_type = totype;
    result->c_base = htype;
D 3
    result->c_const = 0;
E 3
    result->c_expr = eptr = new_expr();
I 3
    result->c_const = 0;
    result->c_bitfield = 0;
E 3
    eptr->e_left = cast_to_int(qvalue);
    eptr->e_right = new_expr();
    eptr->e_right->e_left = true.c_expr;
    eptr->e_right->e_right = false.c_expr;
    eptr->e_func = op_table[htype][tok_2_op['?']];
    eptr->e_size = true.c_expr->e_size;
    if (!eptr->e_func.sc) {
D 4
	fprintf(stderr, "`%s' op is illegal for %s's\n",
		op_2_string[tok_2_op['?']], type_2_string[htype]);
E 4
I 4
	GRAMerror("`%s' op is illegal for %s's",
		  op_2_string[tok_2_op['?']], type_2_string[htype]);
E 4
	eptr->e_func = null_func[htype];
	return 1;
    }
    return 0;
}

/*
 * create an expr tree to cast the RVALUE to type BASE.  Note that
 * c_type is not filled in.
 */
D 2
void cast_to(cnode *rvalue, typeptr t)
E 2
I 2
void cast_to(cnode *rvalue, typeptr t, enum expr_type base)
E 2
{
I 2
    union all_func f;
E 2
    expr *eptr;
D 2
    enum expr_type base = base_type(t);
E 2

    rvalue->c_type = t;
    if (base == rvalue->c_base)
	return;
D 2
    
    eptr = new_expr();
    eptr->e_left = rvalue->c_expr;
    eptr->e_func = cast_table[base][rvalue->c_base];
    eptr->e_size = get_size(t) / 8;
E 2
I 2
    if (!(f = cast_table[base][rvalue->c_base]).sc) {
D 4
	fprintf(stderr, "Illegal cast from %s to %s\n",
		type_2_string[base], type_2_string[rvalue->c_base]);
E 4
I 4
	GRAMerror("Illegal cast from %s to %s",
		  type_2_string[base], type_2_string[rvalue->c_base]);
E 4
	eptr = rvalue->c_expr;
    } else {
	eptr = new_expr();
	eptr->e_left = rvalue->c_expr;
	eptr->e_func = f;
	eptr->e_size = get_size(t) / 8;
    }
E 2
    rvalue->c_base = base;
    rvalue->c_expr = eptr;
}

expr *cast_to_int(cnode *r)
{
D 2
    enum expr_type base = base_type(r->c_type);
E 2
I 2
    union all_func f;
E 2
    expr *ret;

D 2
    if (base == int_type)
E 2
I 2
    if (r->c_base == int_type)
E 2
	return r->c_expr;
I 2
    if (!(f = cast_table[int_type][r->c_base]).sc) {
D 4
	fprintf(stderr, "Illegal cast from %s to %s\n",
		type_2_string[int_type], type_2_string[r->c_base]);
E 4
I 4
	GRAMerror("Illegal cast from %s to %s",
		  type_2_string[int_type], type_2_string[r->c_base]);
E 4
	return r->c_expr;
    }
E 2
    ret = new_expr();
I 2
    ret->e_func = f;
E 2
    ret->e_left = r->c_expr;
D 2
    ret->e_func = cast_table[int_type][base];
E 2
    ret->e_size = sizeof(int);
    return ret;
}

I 2
static void sigill_handler(int sig)
{
    dump_symtable();
    exit(1);
}

E 2
/*
 * We initialize the promotions table.  We do this in somewhat a funny
 * way but it should always work out.  We basically fill in from the
 * lowest to the highest.
 *
 * Note: I've been through this before and created these tables
 * statically.  But when new types are added (and they will be),
 * updating these tables is a real pain.  The init code is big and
 * ugly but it is more self maintaining.
 */
void tree_init(void)
{
    enum expr_type in;
    int next_op = -1;
I 2
    struct sigaction s;
E 2

I 2
    s.sa_handler = (void (*)())sigill_handler;
    SIGINITSET(s.sa_mask);
    s.sa_flags = 0;
I 5
/*
E 5
D 10
    if (sigaction(SIGILL, &s, (struct sigaction *)0) < 0) {
	perror("sigaction");
	exit(1);
    }
E 10
I 10
  if (sigaction(SIGILL, &s, (struct sigaction *)0) < 0) {
  perror("sigaction");
  exit(1);
  }
E 10
I 5
*/
E 5

E 2
#define do_xy(from_type, to_type) \
    for (in = FIRST_TYPE; in <= LAST_TYPE; ++in) \
	promotions[from_type][in] = to_type; \
    for (in = FIRST_TYPE; in <= LAST_TYPE; ++in) \
	promotions[in][from_type] = to_type;


    do_xy(schar_type, schar_type);
    do_xy(uchar_type, uchar_type);
    do_xy(short_type, short_type);
    do_xy(ushort_type, ushort_type);
    do_xy(int_type, int_type);
    do_xy(uint_type, uint_type);
    do_xy(long_type, long_type);
    do_xy(ulong_type, ulong_type);
I 10
    do_xy(long_long_type, long_long_type);
    do_xy(ulong_long_type, ulong_long_type);
E 10
    do_xy(float_type, float_type);
    do_xy(double_type, double_type);

    /*
     * Now do all of the illegal things
     */
    do_xy(struct_type, bad_type);
    do_xy(void_type, bad_type);
    do_xy(bad_type, bad_type);

    /*
     * Now make promotions[type][type] equal to type for all types.
     */
    for (in = FIRST_TYPE; in <= LAST_TYPE; ++in)
	promotions[in][in] = in;

    /*
     * Now we set up the lvalue functions and the pvalue functions.
     */

#define SET_OP(operator) \
    if (++next_op >= OP_MAX) { \
	fprintf(stderr, "OP_MAX is too small\n"); \
	exit(1); \
    } \
    if (operator >= TOK_MAX) { \
	fprintf(stderr, "TOK_MAX needs to be at least %d\n", operator); \
	exit(1); \
    } \
D 10
    tok_2_op[operator] = next_op;
E 10
I 10
    tok_2_op[operator] = next_op
E 10

/*
 * macro to set the function pointer of an expression ENODE based upon
 * TYPE to a set of functions with a common SUFFIX
 */
#define setalltable(table, operator, suffix, str) \
    SET_OP(operator); \
    op_2_string[next_op] = str; \
    table[schar_type][next_op].sc = sc_ ## suffix ; \
    table[uchar_type][next_op].uc = uc_ ## suffix ; \
    table[int_type][next_op].i = i_ ## suffix ; \
    table[uint_type][next_op].ui = ui_ ## suffix ; \
    table[short_type][next_op].s = s_ ## suffix ; \
    table[ushort_type][next_op].us = us_ ## suffix ; \
    table[long_type][next_op].l = l_ ## suffix ; \
    table[ulong_type][next_op].ul = ul_ ## suffix ; \
I 10
    table[long_long_type][next_op].ll = ll_ ## suffix ; \
    table[ulong_long_type][next_op].ull = ull_ ## suffix ; \
E 10
    table[float_type][next_op].f = f_ ## suffix ; \
    table[double_type][next_op].d = d_ ## suffix ; \
D 10
    table[struct_type][next_op].st = st_ ## suffix ;
E 10
I 10
    table[struct_type][next_op].st = st_ ## suffix
E 10

/*
I 2
 * macro to set the function pointer of an expression ENODE based upon
 * TYPE to a set of functions with a common SUFFIX
 */
#define setalladdr(table, operator, suffix, str) \
    SET_OP(operator); \
    op_2_string[next_op] = str; \
    table[schar_type][next_op].sca = sc_ ## suffix ; \
    table[uchar_type][next_op].uca = uc_ ## suffix ; \
    table[int_type][next_op].ia = i_ ## suffix ; \
    table[uint_type][next_op].uia = ui_ ## suffix ; \
    table[short_type][next_op].sa = s_ ## suffix ; \
    table[ushort_type][next_op].usa = us_ ## suffix ; \
    table[long_type][next_op].la = l_ ## suffix ; \
    table[ulong_type][next_op].ula = ul_ ## suffix ; \
I 10
    table[long_long_type][next_op].lla = ll_ ## suffix ; \
    table[ulong_long_type][next_op].ulla = ull_ ## suffix ; \
E 10
    table[float_type][next_op].fa = f_ ## suffix ; \
    table[double_type][next_op].da = d_ ## suffix ; \
D 10
    table[struct_type][next_op].sta = st_ ## suffix ;
E 10
I 10
    table[struct_type][next_op].sta = st_ ## suffix
E 10

/*
E 2
 * Many operators work only on numeric (int and float) types and not
 * structures.
 */
#define setnumtable(table, operator, suffix, str) \
    SET_OP(operator); \
    op_2_string[next_op] = str; \
    table[schar_type][next_op].sc = sc_ ## suffix ; \
    table[uchar_type][next_op].uc = uc_ ## suffix ; \
    table[int_type][next_op].i = i_ ## suffix ; \
    table[uint_type][next_op].ui = ui_ ## suffix ; \
    table[short_type][next_op].s = s_ ## suffix ; \
    table[ushort_type][next_op].us = us_ ## suffix ; \
    table[long_type][next_op].l = l_ ## suffix ; \
    table[ulong_type][next_op].ul = ul_ ## suffix ; \
I 10
    table[long_long_type][next_op].ll = ll_ ## suffix ; \
    table[ulong_long_type][next_op].ull = ull_ ## suffix ; \
E 10
    table[float_type][next_op].f = f_ ## suffix ; \
D 10
    table[double_type][next_op].d = d_ ## suffix ;
E 10
I 10
    table[double_type][next_op].d = d_ ## suffix
E 10

/*
I 3
D 10
 * Compare and logical not work on numeric types but produce an
 * integer result.
E 10
I 10
 * Compare and logical work on numeric types but produce an integer
 * result.
E 10
 */
#define setintrtable(table, operator, suffix, str) \
    SET_OP(operator); \
    op_2_string[next_op] = str; \
    table[schar_type][next_op].i = sc_ ## suffix ; \
    table[uchar_type][next_op].i = uc_ ## suffix ; \
    table[int_type][next_op].i = i_ ## suffix ; \
    table[uint_type][next_op].i = ui_ ## suffix ; \
    table[short_type][next_op].i = s_ ## suffix ; \
    table[ushort_type][next_op].i = us_ ## suffix ; \
    table[long_type][next_op].i = l_ ## suffix ; \
    table[ulong_type][next_op].i = ul_ ## suffix ; \
I 10
    table[long_long_type][next_op].i = ll_ ## suffix ; \
    table[ulong_long_type][next_op].i = ull_ ## suffix ; \
E 10
    table[float_type][next_op].i = f_ ## suffix ; \
D 10
    table[double_type][next_op].i = d_ ## suffix ;
E 10
I 10
    table[double_type][next_op].i = d_ ## suffix
E 10

/*
E 3
 * Some operators work only on integral types
 */
#define setinttable(table, operator, suffix, str) \
    SET_OP(operator); \
    op_2_string[next_op] = str; \
    table[schar_type][next_op].sc = sc_ ## suffix ; \
    table[uchar_type][next_op].uc = uc_ ## suffix ; \
    table[int_type][next_op].i = i_ ## suffix ; \
    table[uint_type][next_op].ui = ui_ ## suffix ; \
    table[short_type][next_op].s = s_ ## suffix ; \
    table[ushort_type][next_op].us = us_ ## suffix ; \
    table[long_type][next_op].l = l_ ## suffix ; \
D 10
    table[ulong_type][next_op].ul = ul_ ## suffix ;
E 10
I 10
    table[ulong_type][next_op].ul = ul_ ## suffix ; \
    table[long_long_type][next_op].ll = ll_ ## suffix ; \
    table[ulong_long_type][next_op].ull = ull_ ## suffix
E 10

    setalltable(op_table, '=', asgn, "=");
    setnumtable(op_table, PLUSEQUAL, plusasgn, "+=");
    setnumtable(op_table, MINUSEQUAL, minusasgn, "-=");
    setnumtable(op_table, TIMESEQUAL, timesasgn, "*=");
    setnumtable(op_table, DIVEQUAL, divasgn, "/=");
    setinttable(op_table, MODEQUAL, modasgn, "%=");
    setinttable(op_table, LSEQUAL, lsasgn, "<<=");
    setinttable(op_table, RSEQUAL, rsasgn, ">>=");
    setinttable(op_table, ANDEQUAL, andasgn, "&=");
    setinttable(op_table, OREQUAL, orasgn, "|=");
I 6
    setinttable(op_table, XOREQUAL, xorasgn, "^=");
E 6
    setalltable(op_table, ',', comma, ",");
    setnumtable(op_table, '*', times, "*");
    setnumtable(op_table, '/', divide, "/");
    setinttable(op_table, '%', mod, "%");
    setnumtable(op_table, '+', plus, "+");
    setnumtable(op_table, '-', minue, "-");
    setinttable(op_table, RSHIFT, rshift, ">>");
    setinttable(op_table, LSHIFT, lshift, "<<");
D 3
    setnumtable(op_table, '<', lt, "<");
    setnumtable(op_table, '>', gt, ">");
    setnumtable(op_table, GTOREQUAL, ge, ">=");
    setnumtable(op_table, LTOREQUAL, le, "<=");
    setnumtable(op_table, EQUALITY, eq, "==");
    setnumtable(op_table, NOTEQUAL, ne, "!=");
E 3
I 3

    setintrtable(op_table, '<', lt, "<");
    setintrtable(op_table, '>', gt, ">");
    setintrtable(op_table, GTOREQUAL, ge, ">=");
    setintrtable(op_table, LTOREQUAL, le, "<=");
    setintrtable(op_table, EQUALITY, eq, "==");
    setintrtable(op_table, NOTEQUAL, ne, "!=");

E 3
    setinttable(op_table, '&', and, "&");
    setinttable(op_table, '^', xor, "^");
    setinttable(op_table, '|', or, "|");
    setnumtable(op_table, ANDAND, andand, "&&");
    setnumtable(op_table, OROR, oror, "||");
    setalltable(op_table, '.', l2p, ".");
    setnumtable(op_table, INCOP, inc, "++");
    setnumtable(op_table, DECOP, dec, "--");
    setalltable(op_table, '?', qc, "?:");
I 8
#ifdef Not_Used
E 8
I 2
    setalladdr(op_table, 'v', v2f, "*(indirection)");
    setalladdr(op_table, 'f', f2v, "&(address of)");
I 8
#endif
E 8
    setalladdr(op_table, 'g', gaddr, "global address");
    setalladdr(op_table, 'l', laddr, "local address");
    setalltable(op_table, 'x', fcall, "function");
I 3
    setintrtable(op_table, '!', lnot, "!");
    setinttable(op_table, '~', bnot, "~");
    setnumtable(op_table, 'u', umin, "-");
E 3
E 2

#define setcasttable(table, from_base, suffix, str) \
    type_2_string[from_base] = str; \
    table[schar_type][from_base].sc = sc_ ## suffix ; \
    table[uchar_type][from_base].uc = uc_ ## suffix ; \
    table[int_type][from_base].i = i_ ## suffix ; \
    table[uint_type][from_base].ui = ui_ ## suffix ; \
    table[short_type][from_base].s = s_ ## suffix ; \
    table[ushort_type][from_base].us = us_ ## suffix ; \
    table[long_type][from_base].l = l_ ## suffix ; \
    table[ulong_type][from_base].ul = ul_ ## suffix ; \
I 10
    table[long_long_type][from_base].ll = ll_ ## suffix ; \
    table[ulong_long_type][from_base].ull = ull_ ## suffix ; \
E 10
    table[float_type][from_base].f = f_ ## suffix ; \
D 10
    table[double_type][from_base].d = d_ ## suffix ;
E 10
I 10
    table[double_type][from_base].d = d_ ## suffix
E 10

    setcasttable(cast_table, schar_type, _sc, "signed char");
    setcasttable(cast_table, uchar_type, _uc, "unsigned char");
    setcasttable(cast_table, int_type, _i, "int");
    setcasttable(cast_table, uint_type, _ui, "unsigned int");
    setcasttable(cast_table, short_type, _s, "short");
    setcasttable(cast_table, ushort_type, _us, "unsigned short");
    setcasttable(cast_table, long_type, _l, "long");
    setcasttable(cast_table, ulong_type, _ul, "unsigned long");
I 10
    setcasttable(cast_table, long_long_type, _ll, "long long");
    setcasttable(cast_table, ulong_long_type, _ull, "unsigned long long");
E 10
    setcasttable(cast_table, float_type, _f, "float");
    setcasttable(cast_table, double_type, _d, "double");
    type_2_string[struct_type] = "structure";
    type_2_string[void_type] = "void";
    type_2_string[bad_type] = "bad";

D 10
    null_func[schar_type].sc = sc_null; \
    null_func[uchar_type].uc = uc_null; \
    null_func[int_type].i = i_null; \
    null_func[uint_type].ui = ui_null; \
    null_func[short_type].s = s_null; \
    null_func[ushort_type].us = us_null; \
    null_func[long_type].l = l_null; \
    null_func[ulong_type].ul = ul_null; \
    null_func[float_type].f = f_null; \
    null_func[double_type].d = d_null; \
E 10
I 10
    null_func[schar_type].sc = sc_null;
    null_func[uchar_type].uc = uc_null;
    null_func[int_type].i = i_null;
    null_func[uint_type].ui = ui_null;
    null_func[short_type].s = s_null;
    null_func[ushort_type].us = us_null;
    null_func[long_type].l = l_null;
    null_func[ulong_type].ul = ul_null;
    null_func[long_long_type].ll = ll_null;
    null_func[ulong_long_type].ull = ull_null;
    null_func[float_type].f = f_null;
    null_func[double_type].d = d_null;
E 10
    null_func[struct_type].st = st_null;
I 11

    base_size_table[schar_type]	     = sizeof(signed char);
    base_size_table[uchar_type]	     = sizeof(unsigned char);
    base_size_table[int_type]	     = sizeof(int);
    base_size_table[uint_type]	     = sizeof(unsigned int);
    base_size_table[short_type]	     = sizeof(short);
    base_size_table[ushort_type]     = sizeof(unsigned short);
    base_size_table[long_type]	     = sizeof(long);
    base_size_table[ulong_type]	     = sizeof(unsigned long);
    base_size_table[long_long_type]  = sizeof(long long);
    base_size_table[ulong_long_type] = sizeof(unsigned long long);
    base_size_table[float_type]	     = sizeof(float);
    base_size_table[double_type]     = sizeof(double);
    base_size_table[struct_type]     = -1;
    base_size_table[void_type]	     = -1;
    base_size_table[bad_type]	     = -1;
E 11
}

D 2
#define A_SIZE(arg) (sizeof(arg) / sizeof((arg)[0]))
symptr ns_syms[HASH_SIZE];
static ns_count;			/* used only by debug routines */

static int hash(char *s)
{
    int hash = 0;

    while (*s) {
	hash += *s++;
	hash <<= 1;
    }
    return hash % A_SIZE(ns_syms);
}

symptr name2userdef(char *name)
{
    symptr ret;
    
    for (ret = ns_syms[hash(name)];
	 ret && strcmp(ret->s_name, name);
	 ret = ret->s_hash);
    return ret;
}

/* after leaving a nesting level, we delete the old symbols */
void clean_symtable()
{
    symptr *sym, *end, temp;

    for (end = (sym = ns_syms) + HASH_SIZE; sym < end; ++sym)
	while ((temp = *sym) && temp->s_nesting > nesting_level) {
	    *sym = temp->s_hash;
	    free(temp);
	    --ns_count;
	}
}

symptr enter_sym(anode *attr, char *name)
{
    symptr ret;
    int hash_val = hash(name);
    
    for (ret = ns_syms[hash_val]; ret; ret = ret->s_hash) {
	if (strcmp(ret->s_name, name))
	    continue;
	/* ### put all the logic here for duplicate symbol stuff */
    }
    ++ns_count;
    ret = new(struct sym);
    ret->s_hash = ns_syms[hash_val];
    ns_syms[hash_val] = ret;
    ret->s_name = name;
    ret->s_base = attr->a_base;
    ret->s_type = attr->a_type;
    ret->s_nesting = nesting_level;
    /*
     * No storage type specified and we are not in a function so we assume it
     * is an extern.
     */
    if (attr->a_class == auto_class && !current_function)
	attr->a_class = extern_class;
    if (attr->a_class == extern_class || attr->a_class == static_class) {
	ret->s_global = 1;
	if (ret->s_type->t_type == PROC_TYPE) {
	    ret->s_stmt = NO_STMT;
	    ret->s_size = 0;
	} else {
	    int size = get_size(ret->s_type) / 8;

	    /*
	     * If we cross a page boundry, we move up to the next page.
	     */
	    if ((global_index & PAGE_MASK) !=
		((global_index + size) & PAGE_MASK))
		global_index = (global_index + PAGE_SIZE - 1) & PAGE_MASK;

	    /*
	     * If a new page, do the virtual memory stuff
	     */
	    if ((global_index & (PAGESIZE - 1)) == 0)
		h_mkpage((v_ptr)(global_index + size));

	    ret->s_offset = (v_ptr)global_index;
	    ret->s_size = size;
	    global_index += size;
	}
    } else {
	int size = ret->s_size = get_size(ret->s_type) / 8;
	
	if (attr->a_class == param_class) {
	    ret->s_offset = (v_ptr)param_index;
	    param_index += size;
	} else {
	    variable_index += size;
	    ret->s_offset = (v_ptr)-variable_index;
	    if (variable_index > variable_max)
		variable_max = variable_index;
	}
	ret->s_global = 0;
    }
    return ret;
}

E 2
enum expr_type base_type(typeptr t)
{
I 6
D 10
    large_t upper;
E 10
I 10
    ularge_t upper;
    ularge_t max;
E 10

E 6
    switch (t->t_type) {
    case STRUCT_TYPE:
    case UNION_TYPE:
	return struct_type;

    case PTR_TYPE:
    case ARRAY_TYPE:
	return ulong_type;

    case RANGE_TYPE:
D 6
	if (t->t_val.val_r.r_lower) {	/* non-zero lower bound => signed */
	    if (t->t_val.val_r.r_upper < 128 )
E 6
I 6
	upper = atolarge(t->t_val.val_r.r_upper);
D 10
	if (!is_zero(t->t_val.val_r.r_lower)) {	/* non-zero lower bound => signed */
	    if (upper < 128 )
E 10
I 10

	if (!is_zero(t->t_val.val_r.r_lower)) {/* signed */
	    max = 128;
	    if (upper < max)
E 10
E 6
		return schar_type;
D 6
	    else if (t->t_val.val_r.r_upper < (128 * 256))
E 6
I 6
D 10
	    else if (upper < (128 * 256))
E 10
I 10
	    max <<= 8 * (sizeof(short) - sizeof(char));
	    if (upper < max)
E 10
E 6
		return short_type;
D 10
	    else
E 10
I 10
	    max <<= 8 * (sizeof(int) - sizeof(short));
	    if (upper < max)
		return int_type;
	    max <<= 8 * (sizeof(long) - sizeof(int));
	    if (upper < max)
E 10
		return long_type;
D 10
	} else {
D 6
	    if (t->t_val.val_r.r_upper < 256)
E 6
I 6
	    if (upper < 256)
E 10
I 10
	    return long_long_type;
	} else {			/* unsigned */
	    max = 256;
	    if (upper < max)
E 10
E 6
		return uchar_type;
D 6
	    else if (t->t_val.val_r.r_upper < (256 * 256))
E 6
I 6
D 10
	    else if (upper < (256 * 256))
E 10
I 10
	    max <<= 8 * (sizeof(short) - sizeof(char));
	    if (upper < max)
E 10
E 6
		return ushort_type;
D 10
	    else
E 10
I 10
	    max <<= 8 * (sizeof(int) - sizeof(short));
	    if (upper < max)
		return uint_type;
	    max <<= 8 * (sizeof(long) - sizeof(int));
	    if (upper < max)
E 10
		return ulong_type;
I 10
	    return ulong_long_type;
E 10
	}
	
    case FLOAT_TYPE:
	return (t->t_val.val_g.g_size == sizeof(float)) ?
	    float_type : double_type;

    case ENUM_TYPE:
	return int_type;

    case PROC_TYPE:
	return ulong_type;

D 2
    case STRINGPTR_TYPE:
    case COMPLEX_TYPE:
E 2
I 2
    case CONSTANT_TYPE:
	return base_type(t->t_val.val_k);

I 3
    case VOLATILE:
	return base_type(t->t_val.val_v);

E 3
    default:
E 2
	fprintf(stderr, "Unimplemented type %d\n", t->t_type);
	exit(1);
    }
}

void eval_all(all *result, cnode *c)
{
I 7
D 10
    PRINTF("eval_all called with %s and evaled to ", type_2_string[c->c_base]);
E 10
I 10
    DEBUG_PRINTF(("eval_all called with %s and evaled to ",
		  type_2_string[c->c_base]));
E 10
E 7
    switch (c->c_base) {
    case schar_type:
D 10
	result->c = sc_val(c->c_expr);
I 7
	PRINTF("%08x\n", result->c);
E 10
I 10
	result->sc = sc_val(c->c_expr);
	DEBUG_PRINTF(("%s\n", P(result->sc)));
E 10
E 7
	break ;
    case uchar_type:
	result->uc = uc_val(c->c_expr);
I 7
D 10
	PRINTF("%08x\n", result->uc);
E 10
I 10
	DEBUG_PRINTF(("%s\n", P(result->uc)));
E 10
E 7
	break ;
    case int_type:
	result->i = i_val(c->c_expr);
I 7
D 10
	PRINTF("%08x\n", result->i);
E 10
I 10
	DEBUG_PRINTF(("%s\n", P(result->i)));
E 10
E 7
	break;
    case uint_type:
	result->ui = ui_val(c->c_expr);
I 7
D 10
	PRINTF("%08x\n", result->ui);
E 10
I 10
	DEBUG_PRINTF(("%s\n", P(result->ui)));
E 10
E 7
	break ;
    case short_type:
	result->s = s_val(c->c_expr);
I 7
D 10
	PRINTF("%08x\n", result->s);
E 10
I 10
	DEBUG_PRINTF(("%s\n", P(result->s)));
E 10
E 7
	break ;
    case ushort_type:
	result->us = us_val(c->c_expr);
I 7
D 10
	PRINTF("%08x\n", result->us);
E 10
I 10
	DEBUG_PRINTF(("%s\n", P(result->us)));
E 10
E 7
	break ;
    case long_type:
	result->l = l_val(c->c_expr);
I 7
D 10
	PRINTF("%08x\n", result->l);
E 10
I 10
	DEBUG_PRINTF(("%s\n", P(result->l)));
E 10
E 7
	break ;
    case ulong_type:
	result->ul = ul_val(c->c_expr);
I 7
D 10
	PRINTF("%08x\n", result->ul);
E 10
I 10
	DEBUG_PRINTF(("%s\n", P(result->ul)));
E 10
E 7
	break ;
I 10
    case long_long_type:
	result->ll = ll_val(c->c_expr);
	DEBUG_PRINTF(("%s\n", P(result->ll)));
	break ;
    case ulong_long_type:
	result->ull = ull_val(c->c_expr);
	DEBUG_PRINTF(("%s\n", P(result->ull)));
	break ;
E 10
    case float_type:
	result->f = f_val(c->c_expr);
I 7
D 10
	PRINTF("%f\n", result->f);
E 10
I 10
	DEBUG_PRINTF(("%f\n", result->f));
E 10
E 7
	break ;
    case double_type:
	result->d = d_val(c->c_expr);
I 7
D 10
	PRINTF("%f\n", result->d);
E 10
I 10
	DEBUG_PRINTF(("%f\n", result->d));
E 10
E 7
	break ;
    case struct_type:
	result->st = st_val(c->c_expr);
I 7
D 10
	PRINTF("%08x\n", result->st);
E 10
I 10
	DEBUG_PRINTF(("%s\n", P(result->st)));
E 10
E 7
	break;
    case void_type:
    default:
D 4
	printf("bogus type dude!!!\n");
E 4
I 4
	fprintf(stderr, "bogus type dude!!!\n");
E 4
	break;
    }
}

D 2
typeptr mkstrtype(int len)
E 2
I 2
D 3
typeptr mkstrtype(ns *nspace, int len)
E 3
I 3
typeptr mk_strtype(ns *nspace, int len)
E 3
E 2
{
    typeptr ret;
D 3
    typeptr char_type;
E 3
I 3
    typeptr c_type;
E 3
    typeptr r_type;
I 6
    char buf[32];
E 6

D 2
    if (ret = name2typedef("char *"))
E 2
I 2
    if (ret = name2typedef(nspace, "char *"))
E 2
	return ret;
D 2
    char_type = mkrange("char", 0, 255);
    r_type = mkrange(0, 0, len);
    ret = newtype(char_type->t_ns, ARRAY_TYPE, 0, 0);
E 2
I 2
D 3
    char_type = mkrange(nspace, "char", 0, 255);
    r_type = mkrange(nspace, 0, 0, len);
    ret = newtype(char_type->t_ns, ARRAY_TYPE);
E 2
    ret->t_val.val_a.a_typedef = r_type;
    ret->t_val.val_a.a_typeptr = char_type;
D 2
    insert_type(++type_number, ret);
E 2
    return ret;
}
E 3
I 3
    c_type = find_type(nspace, TP_CHAR);
E 3

D 2
typeptr mkrange(char *n, long lower, long upper)
E 2
I 2
D 3
typeptr mkrange(ns *nspace, char *n, long lower, long upper)
E 2
{
    typeptr ret;
E 3
I 3
    r_type = newtype(nspace, RANGE_TYPE);
    r_type->t_val.val_r.r_typeptr = 0;
    r_type->t_val.val_r.r_lower = 0;
D 6
    r_type->t_val.val_r.r_upper = len;
E 6
I 6
    sprintf(buf, "%d", len);
    r_type->t_val.val_r.r_upper = store_string(nspace, buf, 0, (char *)0);
E 6
E 3

D 2
    if (!(ret = name2typedef(n))) {
	ret = newtype(ns_inter, RANGE_TYPE, n, 1);
E 2
I 2
D 3
    if (!(ret = name2typedef(nspace, n))) {
	ret = newtype(nspace, RANGE_TYPE);
	if (n)
	    add_typedef(ret, n);
E 2
	ret->t_val.val_r.r_typeptr = 0;
	ret->t_val.val_r.r_lower = lower;
	ret->t_val.val_r.r_upper = upper;
D 2
	insert_type(++type_number, ret);
E 2
    }
E 3
I 3
    ret = newtype(nspace, ARRAY_TYPE);
    ret->t_val.val_a.a_typedef = r_type;
    ret->t_val.val_a.a_typeptr = c_type;
E 3
    return ret;
}

D 2
typeptr mkfloat(char *n, int bytes)
E 2
I 2
D 3
typeptr mkfloat(ns *nspace, char *n, int bytes)
E 3
I 3
D 10
void mk_const(ns *nspace, cnode *c, int value)
E 10
I 10
void mk_const(ns *nspace, cnode *c, large_t value, int tval)
E 10
E 3
E 2
{
D 3
    typeptr ret;

D 2
    if (!(ret = name2typedef(n))) {
	ret = newtype(ns_inter, FLOAT_TYPE, n, 1);
E 2
I 2
    if (!(ret = name2typedef(nspace, n))) {
	ret = newtype(nspace, FLOAT_TYPE);
	if (n)
	    add_typedef(ret, n);
E 2
	ret->t_val.val_g.g_typeptr = 0;
	ret->t_val.val_g.g_size = bytes;
D 2
	insert_type(++type_number, ret);
E 2
    }
    return ret;
}

D 2
void mkconst(cnode *c, int value)
E 2
I 2
void mkconst(ns *nspace, cnode *c, int value)
E 2
{
D 2
    c->c_type = mkrange("int", 0x80000000, 0x7fffffff);
E 2
I 2
    c->c_type = mkrange(nspace, "int", 0x80000000, 0x7fffffff);
E 3
I 3
D 10
    c->c_type = find_type(nspace, TP_INT);
E 3
E 2
    c->c_base = int_type;
E 10
I 10
    c->c_type = find_type(nspace, tval);
    c->c_base = base_type(c->c_type);
E 10
D 3
    c->c_const = 1;
E 3
    c->c_expr = new_expr();
D 10
    c->c_expr->e_func.i = i_leaf;
    c->c_expr->e_i = value;
    c->c_expr->e_size = sizeof(int);
E 10
I 10

    switch (c->c_base) {
    case schar_type:
	c->c_expr->e_sc = value;
	c->c_expr->e_func.sc = sc_leaf;
	break;

    case uchar_type:
	c->c_expr->e_uc = value;
	c->c_expr->e_func.uc = uc_leaf;
	break;

    case int_type:
	c->c_expr->e_i = value;
	c->c_expr->e_func.i = i_leaf;
	break;

    case uint_type:
	c->c_expr->e_ui = value;
	c->c_expr->e_func.ui = ui_leaf;
	break;

    case short_type:
	c->c_expr->e_s = value;
	c->c_expr->e_func.s = s_leaf;
	break;

    case ushort_type:
	c->c_expr->e_us = value;
	c->c_expr->e_func.us = us_leaf;
	break;

    case long_type:
	c->c_expr->e_l = value;
	c->c_expr->e_func.l = l_leaf;
	break;

    case ulong_type:
	c->c_expr->e_ul = value;
	c->c_expr->e_func.ul = ul_leaf;
	break;

    case long_long_type:
	c->c_expr->e_ll = value;
	c->c_expr->e_func.ll = ll_leaf;
	break;

    case ulong_long_type:
	c->c_expr->e_ull = value;
	c->c_expr->e_func.ull = ull_leaf;
	break;
    }

    c->c_expr->e_size = get_size(c->c_type);
E 10
I 3
    c->c_const = 1;
    c->c_bitfield = 0;
E 3
}

D 2
int mk_incdec(cnode *result, cnode *lvalue, int op)
E 2
I 2
int mk_incdec(ns *nspace, cnode *result, cnode *lvalue, int op)
E 2
{
    cnode one;

D 2
    mkconst(&one, 1);
E 2
I 2
D 3
    mkconst(nspace, &one, 1);
E 2
    return mkasgn(result, lvalue, op, &one);
E 3
I 3
D 10
    mk_const(nspace, &one, 1);
E 10
I 10
    mk_const(nspace, &one, 1, TP_INT);
E 10
    return mk_asgn(result, lvalue, op, &one);
E 3
}

void print_expression(cnode *c)
{
    all value;
D 2
    int size;
E 2
    char *p;
D 10
    char numbuf[16];
E 10
I 10
    char numbuf[32];
    typeptr tptr = c->c_type;
E 10
D 2
    char smallbuf[128];
E 2

    eval_all(&value, c);
D 2

    if ((size = c->c_expr->e_size) > sizeof(smallbuf)) {
	if (!(p = malloc(size))) {
	    fprintf(stderr, "Out of memory in print_expressin\n");
	    exit(1);
	}
    } else
	p = smallbuf;

E 2
    numbuf[0] = 0;
    if (c->c_base == struct_type) {
D 2
	v_read(p, (v_ptr)value.st, size);
E 2
I 2
D 6
	p = v2f(value.st);
E 6
I 6
	p = v2f_type(char *, value.st);
E 6
E 2
D 10
	sprintf(numbuf, "0x%08x", value.st);
D 3
    } else if (c->c_type->t_type == ARRAY_TYPE)
E 3
I 3
    } else if (c->c_type->t_type == ARRAY_TYPE ||
	       c->c_type->t_type == PROC_TYPE)
E 3
D 2
	v_read(p, (v_ptr)value.ul, size);
E 2
I 2
D 6
	p = v2f(value.ul);
E 6
I 6
	p = v2f_type(char *, value.ul);
E 10
I 10
	sprintf(numbuf, "0x%s", P(value.st));
    } else if (c->c_type->t_type == ARRAY_TYPE) {
	typeptr rtptr = tptr->t_val.val_a.a_typedef;

	if (rtptr->t_type != RANGE_TYPE) {
	    printf("bogus typedef for array\n");
	    return;
	}

	/* Check if this is actually an & operator */
	if (!rtptr->t_val.val_r.r_lower && !rtptr->t_val.val_r.r_upper)
	    p = (char *)&value;
	else
	    p = v2f_type(char *, value.ul);
    } else if (c->c_type->t_type == PROC_TYPE)
	p = (char *)value.ul;
E 10
E 6
E 2
    else
	p = (char *)&value;

D 10
    print_out(c->c_type, p, 0, 0, 0, numbuf);
E 10
I 10
    print_out(tptr, p, 0, 0, 0, numbuf);
E 10
D 2
    if (p != smallbuf && p != (char *)&value)
	free(p);
E 2
}

void *get_user_sym_addr(char *name)
{
D 2
    symptr sym = name2userdef(name);
E 2
I 2
    symptr sym = name2userdef_all(name);
E 2
    cnode c;
    all a;

    if (!sym)
	return 0;
D 3
    mkident(&c, name);
E 3
I 3
    mk_ident(&c, name);
E 3
    eval_all(&a, &c);
D 7
    return (void *)a.i;
E 7
I 7
    return (void *)a.l;
E 7
D 2
}

static char *expr_type_2_string[] = {
    "schar",
    "uchar",
    "int",
    "uint",
    "short",
    "ushort",
    "long",
    "ulong",
    "float",
    "double",
    "struct",
    "void",
    "bad",
    "BOGUS"
};

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
    "BOGUS"
};
	
static void print_sym(symptr s)
{
    typeptr t = s->s_type;

    printf("%s: ", s->s_name);
    while (t) {
	printf("%s%s", (t == s->s_type) ? "" : "->",
	   stab_type_2_string[(t->t_type > A_SIZE(stab_type_2_string) ||
			       t->t_type < 0) ?
			      (A_SIZE(stab_type_2_string) - 1) :
			      t->t_type]);
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
	default:
	    t = 0;
	}
    }
    printf("(%s), addr=%08x, size=%d, nesting=%d\n",
	   expr_type_2_string[(s->s_base > A_SIZE(expr_type_2_string) ||
			       s->s_base < 0) ?
			      (A_SIZE(expr_type_2_string) - 1) :
			      s->s_base],
	   s->s_offset, s->s_size, s->s_nesting);
}

static int sym_compare(void *v1, void *v2)
{
    symptr *p1 = v1;
    symptr *p2 = v2;
    return strcmp((*p1)->s_name, (*p2)->s_name);
}

void dump_symtable(void)
{
    symptr *all;
    symptr *a, *a_end;
    symptr s;
    int i;

    a = all = (symptr *)smalloc(sizeof(symptr) * ns_count, __FILE__, __LINE__);
    a_end = a + ns_count;
    for (i = 0; i < A_SIZE(ns_syms); i++)
	for (s = ns_syms[i]; s; s = s->s_hash)
	    if (a == a_end)
		printf("ns_count is off\n");
	    else
		*a++ = s;
    qsort(all, ns_count, sizeof(symptr), sym_compare);
    for (a = all; a < a_end; ++a)
	print_sym(*a);
    free(all);
    dump_stmts();
}

void do_parameter_allocation(arg_list *old_list, arg_list *args)
{
    arg_list *a1, *a2;

    /*
     * We run throught the list of old style C declarations for the
     * arguments and match them up with those specified in the
     * argument list.
     */
    for (a1 = old_list; a2 = a1; a1 = a1->a_next, free(a2)) {
	arg_list *p;

	for (p = args; p; p = p->a_next)
	    if (!strcmp(p->a_ident, a1->a_ident)) {
		if (p->a_anode.a_type)
		    fprintf(stderr, "Double declaration for argument %s\n",
			    a1->a_ident);
		else
		    p->a_anode = a1->a_anode;
		break;
	    }
	if (!p)				/* not found */
	    fprintf(stderr,
		    "Arg %s declarated but not in argument list\n",
		    a1->a_ident);
    }

    /*
     * Now actually enter the symbols
     */
    for (a1 = args; a1; a1 = a1->a_next) {
	a1->a_anode.a_class = param_class;
	(void) enter_sym(&a1->a_anode, a1->a_ident);
    }
}

void start_function(symptr f)
{
    current_function = f;
    param_index = sizeof(int);
    variable_max = variable_index = 0;
}

void end_function(void)
{
    statements[current_function->s_stmt].stmt_alloc = variable_max;
    current_function = 0;
}

void cast_to_current_function(cnode *rvalue)
{
    if (current_function)
	cast_to(rvalue, current_function->s_type);
}

int increase_nesting(void)
{
    nesting_level++;
    return variable_index;
}

void decrease_nesting(int new)
{
    --nesting_level;
    clean_symtable();
    variable_index = new;
E 2
}
E 1
