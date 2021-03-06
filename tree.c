static char sccs_id[] = "@(#)tree.c	1.11";

#include <stdio.h>
#include <stdlib.h>
#include <sys/signal.h>
#include <dbxstclass.h>
#include <strings.h>

#include "dex.h"
#include "map.h"
#include "sym.h"
#include "tree.h"
#include "stmt.h"
#include "gram.h"
#include "asgn_expr.h"
#include "base_expr.h"
#include "binary_expr.h"
#include "cast_expr.h"
#include "unary_expr.h"
#include "inter.h"
#include "fcall.h"
#include "print.h"
#include "gram_pre.h"

#define DEBUG_BIT TREE_C_BIT

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
#define OP_MAX 45
#define TOK_MAX 316
static int tok_2_op[TOK_MAX];
static char *op_2_string[OP_MAX];
static union all_func op_table[LAST_TYPE + 1][OP_MAX];
static union all_func cast_table[LAST_TYPE + 1][LAST_TYPE + 1];
static union all_func null_func[LAST_TYPE + 1];
static int base_size_table[LAST_TYPE + 1];

char *type_2_string[LAST_TYPE + 1];

/* routines to build expression trees */

expr *new_expr(void)
{
    return new(struct expr);
}

int mk_ident(cnode *result, char *s)
{
    symptr sym = name2userdef_all(s);	/* find user symbol */
    expr *eptr;

    if (!sym)
	return 1;
    result->c_type = sym->s_type;
    result->c_base = sym->s_base;
    /*
     * I think this is always the case.
     */
    result->c_bitfield = 0;
    if (result->c_expr = sym->s_expr)
	return 0;

    eptr = result->c_expr = sym->s_expr = new_expr();
    eptr->e_func = op_table[sym->s_base][tok_2_op[sym->s_global ? 'g' : 'l' ]];
    eptr->e_addr = sym->s_offset;
    eptr->e_size = sym->s_size;
    return 0;
}

void mk_l2p(cnode *result, cnode *c)
{
    expr *eptr;

    *result = *c;
    if (c->c_type->t_type != PROC_TYPE &&
	c->c_type->t_type != ARRAY_TYPE) {
	eptr = result->c_expr = new_expr();
	eptr->e_func = op_table[c->c_base][tok_2_op['.']];
	eptr->e_left = c->c_expr;
	eptr->e_size = c->c_expr->e_size;
	DEBUG_PRINTF(("mk_l2p %d %d %s\n", eptr->e_size, c->c_bitfield, P(eptr)));
	if (c->c_bitfield) {
	    eptr->e_bsize = c->c_size;
	    eptr->e_boffset = c->c_offset;
	} else {
	    eptr->e_bsize = 0;
	    eptr->e_boffset = 0;
	}
    }
}

#ifdef Not_Used
void mk_f2v(cnode *result, cnode *c)
{
    typeptr t;
    expr *eptr;

    t = newtype(c->c_type->t_ns, PTR_TYPE);
    t->t_val.val_p = c->c_type;
    result->c_type = t;
    result->c_base = base_type(result->c_type);
    result->c_const = 1;
    result->c_bitfield = 0;
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
    result->c_bitfield = 0;
    eptr = result->c_expr = new_expr();
    eptr->e_func = op_table[c->c_base][tok_2_op['v']];
    eptr->e_left = c->c_expr;
    eptr->e_size = get_size(result->c_type) / 8;
    return 0;
}
#endif

int mk_dot(cnode *result, cnode *c, char *s)
{
    typeptr t = c->c_type;
    fieldptr f;
    ns *nspace;
    int byte_offset;
    int byte_size;
    int bit_offset;
    int bit_size;

    if (t->t_type == VOLATILE)
	t = t->t_val.val_v;
    
    if (t->t_type != STRUCT_TYPE && t->t_type != UNION_TYPE) {
	GRAMerror("struct or union expected: %d", t->t_type);
	return 1;
    }
    nspace = t->t_ns;
    for (f = t->t_val.val_s.s_fields;
	 f && strcmp(f->f_name, s);
	 f = f->f_next);
    if (!f) {
	GRAMerror("field %s not part of structure %s", s, t->t_name);
	return 1;
    }

    /*
     * If the offset or the number of bits is not divisible by 8, then
     * we simply set the cnode properly and continue.  This will
     * eventually get changed into either a pvalue or used as an
     * lvalue and we will deal with the problem at that time.
     *
     * Bug fix: If we have a bit field, the traditional type is
     * unsigned int but if the bit field is a multiple of 8 bits yet
     * smaller than the size denoted by c_base, we still need to use
     * the get_field operations otherwise, we end up picking up the
     * entire int instead of just the bytes we are suppose to pick
     * up.  We added base_size_table and the second or test in the if
     * below to catch this.
     *
     * The bit field routines are designed to work with an address
     * that is word aligned.  The other routines, like to read and
     * write a short or a byte, can operate on any byte boundry.  The
     * test for the need of a bit field is based upon either the
     * offset not being on a byte boundry or the size of the field not
     * being a multiple of bytes.
     *
     * So in the case of a bit field, byte_offset will be forced to be
     * a multiple of sizeof(int).
     *
     * Mon Jul 11 11:43:30 CDT 2016 - I discovered that sa_data in a
     * sockaddr was given an offset of 0 when it should have an offset
     * of 2.  sa_data is an array of 14 characters.  My first attempt
     * didn't work.  My second attempt was to add the t_type != ARRAY
     * as a condition for the second part of the if statement below.
     * Removing the second part results in print_csa not working.  I
     * didn 't figure out exactly.
     */
    result->c_type = f->f_typeptr;
    result->c_base = base_type(result->c_type);
    DEBUG_PRINTF(("mk_dot: numbits: %d, type: %d, true size: %d \n",
		  f->f_numbits, result->c_base, base_size_table[result->c_base]));
    byte_offset = f->f_offset / 8;
    if (((f->f_offset | f->f_numbits) & 7) ||
	(result->c_type->t_type != ARRAY_TYPE &&
	 (base_size_table[result->c_base] * 8 != f->f_numbits))) {
	byte_offset &= ~(sizeof(int) - 1); /* make it word aligned */
	bit_offset = f->f_offset - (byte_offset * 8);
	byte_size = sizeof(int);
	bit_size = f->f_numbits;
    } else {
	bit_offset = 0;
	bit_size = 0;
	byte_size = f->f_numbits / 8;
    }

    if (byte_offset) {
	expr *c_expr, *r_expr;

	c_expr = new_expr();
	c_expr->e_func.ul = ul_leaf;
	c_expr->e_ul = byte_offset;
	c_expr->e_size = f->f_numbits / 8;

	/* Make the plus node */
	r_expr = new_expr();
	r_expr->e_func.ul = ul_plus;
	r_expr->e_left = c->c_expr;
	r_expr->e_right = c_expr;
	r_expr->e_size = f->f_numbits / 8;
	result->c_expr = r_expr;
    } else {
	result->c_expr = c->c_expr;
	/*
	 * The result is not the size of the structure but the size of
	 * this field -- except if it is an array in which case the
	 * size is the size of an address.
	 */
	result->c_expr->e_size =
	    (f->f_typeptr->t_type == ARRAY_TYPE) ?
	    sizeof(char *) :
	    f->f_numbits / 8;
    }

    /* set the result */
    result->c_const = c->c_const;
    if (result->c_bitfield = (bit_size != 0))
	result->c_const = 0;
    result->c_offset = bit_offset;
    result->c_size = bit_size;
    return 0;
}

int mk_ptr(cnode *result, cnode *c, char *s)
{
    typeptr t = c->c_type;
    cnode x;

    if (t->t_type == PTR_TYPE) {
	mk_l2p(&x, c);
	x.c_type = t->t_val.val_p;
    } else if (t->t_type != ARRAY_TYPE) {
	mk_l2p(&x, c);
	x.c_type = t->t_val.val_a.a_typeptr;
    } else {
	GRAMerror("not a pointer or array type");
	return 1;
    }
    return mk_dot(result, &x, s);
}

int mk_array(cnode *result, cnode *array, cnode *index)
{
    int rc;
    cnode temp;

    if (array->c_type->t_type != ARRAY_TYPE &&
	array->c_type->t_type != PTR_TYPE) {
	GRAMerror("array or pointer type needed for subscripting");
	return 1;
    }
    /*
     * Currently in the case of an array, this does nothing but it may
     * need to convert address space in the future.
     */
    mk_l2p(&temp, array);
    if (rc = mk_binary(result, &temp, '+', index))
	return rc;
    if (array->c_type->t_type == ARRAY_TYPE)
	result->c_type = array->c_type->t_val.val_a.a_typeptr;
    else
	result->c_type = array->c_type->t_val.val_p;
    result->c_base = base_type(result->c_type);
    result->c_const = 0;
    result->c_bitfield = 0;
    return 0;
}

int mk_asgn(cnode *result, cnode *lvalue, int opcode, cnode *rvalue)
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
	eptr->e_right = newr;
    } else {
	cnode right = *rvalue;

	cast_to(&right, t, lvalue->c_base);
	eptr->e_right = right.c_expr;
    }

    eptr->e_left = lvalue->c_expr;
    eptr->e_func = op_table[lvalue->c_base][tok_2_op[opcode]];
    if (!eptr->e_func.sc) {
	GRAMerror("`%s' op is illegal for %s's",
		  op_2_string[tok_2_op[opcode]],
		  type_2_string[lvalue->c_base]);
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
    result->c_type = t;
    result->c_base = lvalue->c_base;
    result->c_expr = eptr;
    result->c_const = 0;
    result->c_bitfield = 0;
    return rc;
}

int mk_unary(cnode *result, cnode *lvalue, int opcode)
{
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
    int lptr = (lvalue->c_type->t_type == PTR_TYPE ||
		lvalue->c_type->t_type == ARRAY_TYPE);
    int rptr = (rvalue->c_type->t_type == PTR_TYPE ||
		rvalue->c_type->t_type == ARRAY_TYPE);
    cnode right = *rvalue;
    cnode left = *lvalue;
    enum expr_type htype;
    typeptr totype;
    expr *eptr;
    int ptr_is_ok = !(opcode == ',' || opcode == '>' || opcode == '<' ||
		      opcode == GTOREQUAL || opcode == LTOREQUAL ||
		      opcode == EQUALITY || opcode == NOTEQUAL ||
		      opcode == ANDAND || opcode == OROR);

    /*
     * Check for pointer subtraction first.  The list of opcodes below
     * are those which are legal to do but do not require any special
     * code.
     */
    if (lptr && rptr && ptr_is_ok) {

	expr *newr;
	expr *size;
	typeptr ltype;
	typeptr rtype;

	if (opcode != '-') {
	    GRAMerror("ptr %s ptr is illegal", op_2_string[tok_2_op[opcode]]);
	    return 1;
	}

	ltype = ((left.c_type->t_type == PTR_TYPE) ?
		 left.c_type->t_val.val_p :
		 left.c_type->t_val.val_a.a_typeptr);
	rtype = ((right.c_type->t_type == PTR_TYPE) ?
		 right.c_type->t_val.val_p :
		 right.c_type->t_val.val_a.a_typeptr);
	if (rtype != ltype && get_size(ltype) != get_size(rtype)) {
	    GRAMerror("pointer subtraction of different types not legal");
	    return 1;
	}

	/*
	 * Result will be a long
	 */
#ifdef __64BIT__
	result->c_type = find_type(left.c_type->t_ns, TP_LONG_64);
#else
	result->c_type = find_type(left.c_type->t_ns, TP_LONG);
#endif
	result->c_base = long_type;
	result->c_expr = eptr = new_expr();
	result->c_const = left.c_const && right.c_const;
	result->c_bitfield = 0;

	/* Create expression for sizeof(*lvalue) */
	size = new_expr();
	size->e_func.l = l_leaf;
	size->e_l = get_size(ltype) / 8;
	size->e_size = sizeof(long);

	/* Create subtraction node of two pointers */
	newr = new_expr();
	newr->e_func = op_table[long_type][tok_2_op['-']];
	newr->e_left = left.c_expr;
	newr->e_right = right.c_expr;
	newr->e_size = sizeof(long);

	/* Create divide node of new rvalue by sizeof(*lvalue) */
	eptr->e_func = op_table[long_type][tok_2_op['/']];
	eptr->e_left = newr;
	eptr->e_right = size;
	eptr->e_size = sizeof(long);
	return 0;
    }

    if ((lptr || rptr) && ptr_is_ok) {
	expr *newr;
	expr *size;

	if (rptr) {
	    /* Now we have ptr + blah or ptr - blah */
	    left = *rvalue;
	    right = *lvalue;
	}

	newr = new_expr();
	size = new_expr();

	result->c_type = left.c_type;
	result->c_base = left.c_base;
	result->c_expr = eptr = new_expr();
	result->c_const = left.c_const && right.c_const;
	result->c_bitfield = 0;
	eptr->e_func = null_func[left.c_base];

	if (opcode != '+' && opcode != '-') {
	    GRAMerror("'%s' op is illegal with pointers",
		      op_2_string[tok_2_op[opcode]]);
	    return 1;
	}

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
    cast_to(&right, totype, htype);
    cast_to(&left, totype, htype);

    result->c_type = totype;
    result->c_base = htype;
    result->c_expr = eptr = new_expr();
    result->c_const = right.c_const && left.c_const;
    result->c_bitfield = 0;
    eptr->e_left = left.c_expr;
    eptr->e_right = right.c_expr;
    eptr->e_func = op_table[htype][tok_2_op[opcode]];
    eptr->e_size = get_size(result->c_type) / 8;
    if (!eptr->e_func.sc) {
	GRAMerror("`%s' op is illegal for %s's",
		  op_2_string[tok_2_op[opcode]], type_2_string[htype]);
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
    eptr->e_func = op_table[name->c_base][tok_2_op['x']];
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
    cast_to(&false, totype, htype);
    cast_to(&true, totype, htype);

    result->c_type = totype;
    result->c_base = htype;
    result->c_expr = eptr = new_expr();
    result->c_const = 0;
    result->c_bitfield = 0;
    eptr->e_left = cast_to_int(qvalue);
    eptr->e_right = new_expr();
    eptr->e_right->e_left = true.c_expr;
    eptr->e_right->e_right = false.c_expr;
    eptr->e_func = op_table[htype][tok_2_op['?']];
    eptr->e_size = true.c_expr->e_size;
    if (!eptr->e_func.sc) {
	GRAMerror("`%s' op is illegal for %s's",
		  op_2_string[tok_2_op['?']], type_2_string[htype]);
	eptr->e_func = null_func[htype];
	return 1;
    }
    return 0;
}

/*
 * create an expr tree to cast the RVALUE to type BASE.  Note that
 * c_type is not filled in.
 */
void cast_to(cnode *rvalue, typeptr t, enum expr_type base)
{
    union all_func f;
    expr *eptr;

    rvalue->c_type = t;
    if (base == rvalue->c_base)
	return;
    if (!(f = cast_table[base][rvalue->c_base]).sc) {
	GRAMerror("Illegal cast from %s to %s",
		  type_2_string[base], type_2_string[rvalue->c_base]);
	eptr = rvalue->c_expr;
    } else {
	eptr = new_expr();
	eptr->e_left = rvalue->c_expr;
	eptr->e_func = f;
	eptr->e_size = get_size(t) / 8;
    }
    rvalue->c_base = base;
    rvalue->c_expr = eptr;
}

expr *cast_to_int(cnode *r)
{
    union all_func f;
    expr *ret;

    if (r->c_base == int_type)
	return r->c_expr;
    if (!(f = cast_table[int_type][r->c_base]).sc) {
	GRAMerror("Illegal cast from %s to %s",
		  type_2_string[int_type], type_2_string[r->c_base]);
	return r->c_expr;
    }
    ret = new_expr();
    ret->e_func = f;
    ret->e_left = r->c_expr;
    ret->e_size = sizeof(int);
    return ret;
}

static void sigill_handler(int sig)
{
    dump_symtable();
    exit(1);
}

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
    struct sigaction s;

    s.sa_handler = (void (*)())sigill_handler;
    SIGINITSET(s.sa_mask);
    s.sa_flags = 0;
/*
  if (sigaction(SIGILL, &s, (struct sigaction *)0) < 0) {
  perror("sigaction");
  exit(1);
  }
*/

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
    do_xy(long_long_type, long_long_type);
    do_xy(ulong_long_type, ulong_long_type);
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
    tok_2_op[operator] = next_op

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
    table[long_long_type][next_op].ll = ll_ ## suffix ; \
    table[ulong_long_type][next_op].ull = ull_ ## suffix ; \
    table[float_type][next_op].f = f_ ## suffix ; \
    table[double_type][next_op].d = d_ ## suffix ; \
    table[struct_type][next_op].st = st_ ## suffix

/*
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
    table[long_long_type][next_op].lla = ll_ ## suffix ; \
    table[ulong_long_type][next_op].ulla = ull_ ## suffix ; \
    table[float_type][next_op].fa = f_ ## suffix ; \
    table[double_type][next_op].da = d_ ## suffix ; \
    table[struct_type][next_op].sta = st_ ## suffix

/*
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
    table[long_long_type][next_op].ll = ll_ ## suffix ; \
    table[ulong_long_type][next_op].ull = ull_ ## suffix ; \
    table[float_type][next_op].f = f_ ## suffix ; \
    table[double_type][next_op].d = d_ ## suffix

/*
 * Compare and logical work on numeric types but produce an integer
 * result.
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
    table[long_long_type][next_op].i = ll_ ## suffix ; \
    table[ulong_long_type][next_op].i = ull_ ## suffix ; \
    table[float_type][next_op].i = f_ ## suffix ; \
    table[double_type][next_op].i = d_ ## suffix

/*
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
    table[ulong_type][next_op].ul = ul_ ## suffix ; \
    table[long_long_type][next_op].ll = ll_ ## suffix ; \
    table[ulong_long_type][next_op].ull = ull_ ## suffix

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
    setinttable(op_table, XOREQUAL, xorasgn, "^=");
    setalltable(op_table, ',', comma, ",");
    setnumtable(op_table, '*', times, "*");
    setnumtable(op_table, '/', divide, "/");
    setinttable(op_table, '%', mod, "%");
    setnumtable(op_table, '+', plus, "+");
    setnumtable(op_table, '-', minue, "-");
    setinttable(op_table, RSHIFT, rshift, ">>");
    setinttable(op_table, LSHIFT, lshift, "<<");

    setintrtable(op_table, '<', lt, "<");
    setintrtable(op_table, '>', gt, ">");
    setintrtable(op_table, GTOREQUAL, ge, ">=");
    setintrtable(op_table, LTOREQUAL, le, "<=");
    setintrtable(op_table, EQUALITY, eq, "==");
    setintrtable(op_table, NOTEQUAL, ne, "!=");

    setinttable(op_table, '&', and, "&");
    setinttable(op_table, '^', xor, "^");
    setinttable(op_table, '|', or, "|");
    setnumtable(op_table, ANDAND, andand, "&&");
    setnumtable(op_table, OROR, oror, "||");
    setalltable(op_table, '.', l2p, ".");
    setnumtable(op_table, INCOP, inc, "++");
    setnumtable(op_table, DECOP, dec, "--");
    setalltable(op_table, '?', qc, "?:");
#ifdef Not_Used
    setalladdr(op_table, 'v', v2f, "*(indirection)");
    setalladdr(op_table, 'f', f2v, "&(address of)");
#endif
    setalladdr(op_table, 'g', gaddr, "global address");
    setalladdr(op_table, 'l', laddr, "local address");
    setalltable(op_table, 'x', fcall, "function");
    setintrtable(op_table, '!', lnot, "!");
    setinttable(op_table, '~', bnot, "~");
    setnumtable(op_table, 'u', umin, "-");

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
    table[long_long_type][from_base].ll = ll_ ## suffix ; \
    table[ulong_long_type][from_base].ull = ull_ ## suffix ; \
    table[float_type][from_base].f = f_ ## suffix ; \
    table[double_type][from_base].d = d_ ## suffix

    setcasttable(cast_table, schar_type, _sc, "signed char");
    setcasttable(cast_table, uchar_type, _uc, "unsigned char");
    setcasttable(cast_table, int_type, _i, "int");
    setcasttable(cast_table, uint_type, _ui, "unsigned int");
    setcasttable(cast_table, short_type, _s, "short");
    setcasttable(cast_table, ushort_type, _us, "unsigned short");
    setcasttable(cast_table, long_type, _l, "long");
    setcasttable(cast_table, ulong_type, _ul, "unsigned long");
    setcasttable(cast_table, long_long_type, _ll, "long long");
    setcasttable(cast_table, ulong_long_type, _ull, "unsigned long long");
    setcasttable(cast_table, float_type, _f, "float");
    setcasttable(cast_table, double_type, _d, "double");
    type_2_string[struct_type] = "structure";
    type_2_string[void_type] = "void";
    type_2_string[bad_type] = "bad";

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
    null_func[struct_type].st = st_null;

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
}

enum expr_type base_type(typeptr t)
{
    ularge_t upper;
    ularge_t max;

    switch (t->t_type) {
    case STRUCT_TYPE:
    case UNION_TYPE:
	return struct_type;

    case PTR_TYPE:
    case ARRAY_TYPE:
	return ulong_type;

    case RANGE_TYPE:
	upper = atolarge(t->t_val.val_r.r_upper);

	if (!is_zero(t->t_val.val_r.r_lower)) {/* signed */
	    max = 128;
	    if (upper < max)
		return schar_type;
	    max <<= 8 * (sizeof(short) - sizeof(char));
	    if (upper < max)
		return short_type;
	    max <<= 8 * (sizeof(int) - sizeof(short));
	    if (upper < max)
		return int_type;
	    max <<= 8 * (sizeof(long) - sizeof(int));
	    if (upper < max)
		return long_type;
	    return long_long_type;
	} else {			/* unsigned */
	    max = 256;
	    if (upper < max)
		return uchar_type;
	    max <<= 8 * (sizeof(short) - sizeof(char));
	    if (upper < max)
		return ushort_type;
	    max <<= 8 * (sizeof(int) - sizeof(short));
	    if (upper < max)
		return uint_type;
	    max <<= 8 * (sizeof(long) - sizeof(int));
	    if (upper < max)
		return ulong_type;
	    return ulong_long_type;
	}

    case FLOAT_TYPE:
	return (t->t_val.val_g.g_size == sizeof(float)) ?
	    float_type : double_type;

    case ENUM_TYPE:
	return int_type;

    case PROC_TYPE:
	return ulong_type;

    case CONSTANT_TYPE:
	return base_type(t->t_val.val_k);

    case VOLATILE:
	return base_type(t->t_val.val_v);

    default:
	fprintf(stderr, "Unimplemented type %d\n", t->t_type);
	exit(1);
    }
}

void eval_all(all *result, cnode *c)
{
    DEBUG_PRINTF(("eval_all called with %s and evaled to ",
		  type_2_string[c->c_base]));
    switch (c->c_base) {
    case schar_type:
	result->sc = sc_val(c->c_expr);
	DEBUG_PRINTF(("%s\n", P(result->sc)));
	break ;
    case uchar_type:
	result->uc = uc_val(c->c_expr);
	DEBUG_PRINTF(("%s\n", P(result->uc)));
	break ;
    case int_type:
	result->i = i_val(c->c_expr);
	DEBUG_PRINTF(("%s\n", P(result->i)));
	break;
    case uint_type:
	result->ui = ui_val(c->c_expr);
	DEBUG_PRINTF(("%s\n", P(result->ui)));
	break ;
    case short_type:
	result->s = s_val(c->c_expr);
	DEBUG_PRINTF(("%s\n", P(result->s)));
	break ;
    case ushort_type:
	result->us = us_val(c->c_expr);
	DEBUG_PRINTF(("%s\n", P(result->us)));
	break ;
    case long_type:
	result->l = l_val(c->c_expr);
	DEBUG_PRINTF(("%s\n", P(result->l)));
	break ;
    case ulong_type:
	result->ul = ul_val(c->c_expr);
	DEBUG_PRINTF(("%s\n", P(result->ul)));
	break ;
    case long_long_type:
	result->ll = ll_val(c->c_expr);
	DEBUG_PRINTF(("%s\n", P(result->ll)));
	break ;
    case ulong_long_type:
	result->ull = ull_val(c->c_expr);
	DEBUG_PRINTF(("%s\n", P(result->ull)));
	break ;
    case float_type:
	result->f = f_val(c->c_expr);
	DEBUG_PRINTF(("%f\n", result->f));
	break ;
    case double_type:
	result->d = d_val(c->c_expr);
	DEBUG_PRINTF(("%f\n", result->d));
	break ;
    case struct_type:
	result->st = st_val(c->c_expr);
	DEBUG_PRINTF(("%s\n", P(result->st)));
	break;
    case void_type:
    default:
	fprintf(stderr, "bogus type dude!!!\n");
	break;
    }
}

typeptr mk_strtype(ns *nspace, int len)
{
    typeptr ret;
    typeptr c_type;
    typeptr r_type;
    char buf[32];

    if (ret = name2typedef(nspace, "char *"))
	return ret;
    c_type = find_type(nspace, TP_CHAR);

    r_type = newtype(nspace, RANGE_TYPE);
    r_type->t_val.val_r.r_typeptr = 0;
    r_type->t_val.val_r.r_lower = 0;
    sprintf(buf, "%d", len);
    r_type->t_val.val_r.r_upper = store_string(nspace, buf, 0, (char *)0);

    ret = newtype(nspace, ARRAY_TYPE);
    ret->t_val.val_a.a_typedef = r_type;
    ret->t_val.val_a.a_typeptr = c_type;
    return ret;
}

void mk_const(ns *nspace, cnode *c, large_t value, int tval)
{
    c->c_type = find_type(nspace, tval);
    c->c_base = base_type(c->c_type);
    c->c_expr = new_expr();

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
    c->c_const = 1;
    c->c_bitfield = 0;
}

int mk_incdec(ns *nspace, cnode *result, cnode *lvalue, int op)
{
    cnode one;

    mk_const(nspace, &one, 1, TP_INT);
    return mk_asgn(result, lvalue, op, &one);
}

void print_expression(cnode *c)
{
    all value;
    char *p;
    char numbuf[32];
    typeptr tptr = c->c_type;

    eval_all(&value, c);
    numbuf[0] = 0;
    if (c->c_base == struct_type) {
	p = v2f_type(char *, value.st);
	sprintf(numbuf, "%s", P(value.st));
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
    else
	p = (char *)&value;

    print_out(tptr, p, 0, 0, 0, numbuf);
}

void *get_user_sym_addr(char *name)
{
    symptr sym = name2userdef_all(name);
    cnode c;
    all a;

    if (!sym)
	return 0;
    mk_ident(&c, name);
    eval_all(&a, &c);
    return (void *)a.l;
}
