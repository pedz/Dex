static char sccs_id[] = "@(#)tree.c	1.1";

/*
 * TODO's
 * #### pointer arithmatic
 * #### redo compare ops (?)
 * #### bit fields
 * #### duplicate symbols
 * #### local symbols
 */

#include <stdio.h>
#include <stdlib.h>
#include "map.h"
#include "sym.h"
#include "tree.h"
#include "stmt.h"
#include "gram.h"
#include "asgn_expr.h"
#include "base_expr.h"
#include "inter.h"
#include "hmap.h"
#include "dex.h"
#include "fcall.h"

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
#define OP_MAX 33
#define TOK_MAX 316
static int tok_2_op[TOK_MAX];
static char *op_2_string[OP_MAX];
static char *type_2_string[LAST_TYPE + 1];
static union all_func op_table[LAST_TYPE + 1][OP_MAX];
static union all_func cast_table[LAST_TYPE + 1][LAST_TYPE + 1];
static union all_func null_func[LAST_TYPE + 1];

static int global_index = h_base;
static symptr current_function;
static int param_index;
static int variable_index;
static int variable_max;
static int type_number;
static int nesting_level;

/* routines to build expression trees */

expr *new_expr(void)
{
    return new(struct expr);
}

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

int mkident(cnode *result, char *s)
{
    symptr sym = name2userdef(s);	/* find user symbol */
    expr *eptr;

    if (!sym)
	return 1;
    result->c_type = sym->s_type;
    result->c_base = sym->s_base;
    eptr = result->c_expr = new_expr();
    if (sym->s_global) {
	set_addrfunc(eptr, sym->s_base, gaddr);
    } else {
	set_addrfunc(eptr, sym->s_base, laddr);
    }
    eptr->e_addr = sym->s_offset;
    eptr->e_size = sym->s_size;
    return 0;
}

void mkl2p(cnode *result, cnode *c)
{
    expr *eptr;

    *result = *c;
    if (c->c_type->t_type != ARRAY_TYPE &&
	c->c_type->t_type != PROC_TYPE) {
	eptr = result->c_expr = new_expr();
	eptr->e_func = op_table[c->c_base][tok_2_op['.']];
	eptr->e_left = c->c_expr;
	eptr->e_size = c->c_expr->e_size;
    }
}

int mkdot(cnode *result, cnode *c, char *s)
{
    typeptr t = c->c_type;
    fieldptr f;
    ns *ns;
    expr *c_expr, *r_expr;

    if (t->t_type != STRUCT_TYPE && t->t_type != UNION_TYPE) {
	fprintf(stderr, "struct or union expected\n");
	return 1;
    }
    ns = t->t_ns;
    for (f = t->t_val.val_s.s_fields;
	 f && strcmp(f->f_name, s);
	 f = f->f_next);
    if (!f) {
	fprintf(stderr, "field %s not part of structure %s\n", s,
		t->t_name);
	return 1;
    }

    c_expr = new_expr();
    c_expr->e_func.ul = ul_leaf;
    c_expr->e_i = f->f_offset / 8;
    c_expr->e_size = f->f_numbits / 8;

    /* Make the plus node */
    r_expr = new_expr();
    r_expr->e_func.ul = ul_plus;
    r_expr->e_left = c->c_expr;
    r_expr->e_right = c_expr;
    r_expr->e_size = f->f_numbits / 8;

    /* set the result */
    result->c_type = find_type(ns, f->f_typeid);
    result->c_base = base_type(result->c_type);
    result->c_const = c->c_const;
    result->c_expr = r_expr;
    return 0;
}

int mkptr(cnode *result, cnode *c, char *s)
{
    typeptr t = c->c_type;
    cnode x;

    if (t->t_type != PTR_TYPE) {
	fprintf(stderr, "not a pointer type\n");
	return 1;
    }
    mkl2p(&x, c);
    x.c_type = t->t_val.val_p;
    return mkdot(result, &x, s);
}

int mkarray(cnode *result, cnode *array, cnode *index)
{
    int rc;

    if (array->c_type->t_type != ARRAY_TYPE) {
	fprintf(stderr, "array type needed for subscripting\n");
	return 1;
    }
    if (rc = mkbinary(result, array, '+', index))
	return rc;
    result->c_type = array->c_type->t_val.val_a.a_typeptr;
    result->c_base = base_type(result->c_type);
    result->c_const = 0;
    return 0;
}

int mkasgn(cnode *result, cnode *lvalue, int opcode, cnode *rvalue)
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
	eptr->e_left = lvalue->c_expr;
	eptr->e_right = newr;
	eptr->e_func = op_table[lvalue->c_base][tok_2_op[opcode]];
	eptr->e_size = lvalue->c_expr->e_size;
    } else {
	cnode right = *rvalue;

	cast_to(&right, t);
	eptr->e_left = lvalue->c_expr;
	eptr->e_right = right.c_expr;
	eptr->e_func = op_table[lvalue->c_base][tok_2_op[opcode]];
	if (!eptr->e_func.sc) {
	    fprintf(stderr, "`%s' op is illegal for %s's\n",
		    op_2_string[tok_2_op[opcode]],
		    type_2_string[lvalue->c_base]);
	    eptr->e_func = null_func[lvalue->c_base];
	    rc = 1;
	}
	eptr->e_size = lvalue->c_expr->e_size;
    }
    result->c_type = t;
    result->c_base = lvalue->c_base;
    result->c_const = 0;
    result->c_expr = eptr;
    return rc;
}

int mkbinary(cnode *result, cnode *lvalue, int opcode, cnode *rvalue)
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

    if (lptr || rptr) {
	expr *newr = new_expr();
	expr *size = new_expr();

	if (rptr) {
	    /* Now we have ptr + blah or ptr - blah */
	    left = *rvalue;
	    right = *lvalue;
	}

	result->c_type = left.c_type;
	result->c_base = left.c_base;
	result->c_const = left.c_const && right.c_const;
	result->c_expr = eptr = new_expr();
	eptr->e_func = null_func[left.c_base];

	if (opcode != '+' && opcode != '-') {
	    fprintf(stderr, "'%s' op is illegal with pointers\n",
		    op_2_string[tok_2_op[opcode]]);
	    return 1;
	}

	if (rptr && lptr) {
	    fprintf(stderr, "ptr %s ptr is illegal\n",
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
    cast_to(&right, totype);
    cast_to(&left, totype);

    result->c_type = totype;
    result->c_base = htype;
    result->c_const = right.c_const && left.c_const;
    result->c_expr = eptr = new_expr();
    eptr->e_left = left.c_expr;
    eptr->e_right = right.c_expr;
    eptr->e_func = op_table[htype][tok_2_op[opcode]];
    eptr->e_size = get_size(result->c_type) / 8;
    if (!eptr->e_func.sc) {
	fprintf(stderr, "`%s' op is illegal for %s's\n",
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
    set_func(eptr, name->c_base, fcall);
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
    cast_to(&false, totype);
    cast_to(&true, totype);

    result->c_type = totype;
    result->c_base = htype;
    result->c_const = 0;
    result->c_expr = eptr = new_expr();
    eptr->e_left = cast_to_int(qvalue);
    eptr->e_right = new_expr();
    eptr->e_right->e_left = true.c_expr;
    eptr->e_right->e_right = false.c_expr;
    eptr->e_func = op_table[htype][tok_2_op['?']];
    eptr->e_size = true.c_expr->e_size;
    if (!eptr->e_func.sc) {
	fprintf(stderr, "`%s' op is illegal for %s's\n",
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
void cast_to(cnode *rvalue, typeptr t)
{
    expr *eptr;
    enum expr_type base = base_type(t);

    rvalue->c_type = t;
    if (base == rvalue->c_base)
	return;
    
    eptr = new_expr();
    eptr->e_left = rvalue->c_expr;
    eptr->e_func = cast_table[base][rvalue->c_base];
    eptr->e_size = get_size(t) / 8;
    rvalue->c_base = base;
    rvalue->c_expr = eptr;
}

expr *cast_to_int(cnode *r)
{
    enum expr_type base = base_type(r->c_type);
    expr *ret;

    if (base == int_type)
	return r->c_expr;
    ret = new_expr();
    ret->e_left = r->c_expr;
    ret->e_func = cast_table[int_type][base];
    ret->e_size = sizeof(int);
    return ret;
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
    tok_2_op[operator] = next_op;

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
    table[float_type][next_op].f = f_ ## suffix ; \
    table[double_type][next_op].d = d_ ## suffix ; \
    table[struct_type][next_op].st = st_ ## suffix ;

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
    table[float_type][next_op].f = f_ ## suffix ; \
    table[double_type][next_op].d = d_ ## suffix ;

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
    table[ulong_type][next_op].ul = ul_ ## suffix ;

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
    setalltable(op_table, ',', comma, ",");
    setnumtable(op_table, '*', times, "*");
    setnumtable(op_table, '/', divide, "/");
    setinttable(op_table, '%', mod, "%");
    setnumtable(op_table, '+', plus, "+");
    setnumtable(op_table, '-', minue, "-");
    setinttable(op_table, RSHIFT, rshift, ">>");
    setinttable(op_table, LSHIFT, lshift, "<<");
    setnumtable(op_table, '<', lt, "<");
    setnumtable(op_table, '>', gt, ">");
    setnumtable(op_table, GTOREQUAL, ge, ">=");
    setnumtable(op_table, LTOREQUAL, le, "<=");
    setnumtable(op_table, EQUALITY, eq, "==");
    setnumtable(op_table, NOTEQUAL, ne, "!=");
    setinttable(op_table, '&', and, "&");
    setinttable(op_table, '^', xor, "^");
    setinttable(op_table, '|', or, "|");
    setnumtable(op_table, ANDAND, andand, "&&");
    setnumtable(op_table, OROR, oror, "||");
    setalltable(op_table, '.', l2p, ".");
    setnumtable(op_table, INCOP, inc, "++");
    setnumtable(op_table, DECOP, dec, "--");
    setalltable(op_table, '?', qc, "?:");

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
    table[float_type][from_base].f = f_ ## suffix ; \
    table[double_type][from_base].d = d_ ## suffix ;

    setcasttable(cast_table, schar_type, _sc, "signed char");
    setcasttable(cast_table, uchar_type, _uc, "unsigned char");
    setcasttable(cast_table, int_type, _i, "int");
    setcasttable(cast_table, uint_type, _ui, "unsigned int");
    setcasttable(cast_table, short_type, _s, "short");
    setcasttable(cast_table, ushort_type, _us, "unsigned short");
    setcasttable(cast_table, long_type, _l, "long");
    setcasttable(cast_table, ulong_type, _ul, "unsigned long");
    setcasttable(cast_table, float_type, _f, "float");
    setcasttable(cast_table, double_type, _d, "double");
    type_2_string[struct_type] = "structure";
    type_2_string[void_type] = "void";
    type_2_string[bad_type] = "bad";

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
    null_func[struct_type].st = st_null;
}

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

enum expr_type base_type(typeptr t)
{
    switch (t->t_type) {
    case STRUCT_TYPE:
    case UNION_TYPE:
	return struct_type;

    case PTR_TYPE:
    case ARRAY_TYPE:
	return ulong_type;

    case RANGE_TYPE:
	if (t->t_val.val_r.r_lower) {	/* non-zero lower bound => signed */
	    if (t->t_val.val_r.r_upper < 128 )
		return schar_type;
	    else if (t->t_val.val_r.r_upper < (128 * 256))
		return short_type;
	    else
		return long_type;
	} else {
	    if (t->t_val.val_r.r_upper < 256)
		return uchar_type;
	    else if (t->t_val.val_r.r_upper < (256 * 256))
		return ushort_type;
	    else
		return ulong_type;
	}
	
    case FLOAT_TYPE:
	return (t->t_val.val_g.g_size == sizeof(float)) ?
	    float_type : double_type;

    case ENUM_TYPE:
	return int_type;

    case PROC_TYPE:
	return ulong_type;

    case STRINGPTR_TYPE:
    case COMPLEX_TYPE:
	fprintf(stderr, "Unimplemented type %d\n", t->t_type);
	exit(1);
    }
}

void eval_all(all *result, cnode *c)
{
    switch (c->c_base) {
    case schar_type:
	result->c = sc_val(c->c_expr);
	break ;
    case uchar_type:
	result->uc = uc_val(c->c_expr);
	break ;
    case int_type:
	result->i = i_val(c->c_expr);
	break;
    case uint_type:
	result->ui = ui_val(c->c_expr);
	break ;
    case short_type:
	result->s = s_val(c->c_expr);
	break ;
    case ushort_type:
	result->us = us_val(c->c_expr);
	break ;
    case long_type:
	result->l = l_val(c->c_expr);
	break ;
    case ulong_type:
	result->ul = ul_val(c->c_expr);
	break ;
    case float_type:
	result->f = f_val(c->c_expr);
	break ;
    case double_type:
	result->d = d_val(c->c_expr);
	break ;
    case struct_type:
	result->st = st_val(c->c_expr);
	break;
    case void_type:
    default:
	printf("bogus type dude!!!\n");
	break;
    }
}

typeptr mkstrtype(int len)
{
    typeptr ret;
    typeptr char_type;
    typeptr r_type;

    if (ret = name2typedef("char *"))
	return ret;
    char_type = mkrange("char", 0, 255);
    r_type = mkrange(0, 0, len);
    ret = newtype(char_type->t_ns, ARRAY_TYPE, 0, 0);
    ret->t_val.val_a.a_typedef = r_type;
    ret->t_val.val_a.a_typeptr = char_type;
    insert_type(++type_number, ret);
    return ret;
}

typeptr mkrange(char *n, long lower, long upper)
{
    typeptr ret;

    if (!(ret = name2typedef(n))) {
	ret = newtype(ns_inter, RANGE_TYPE, n, 1);
	ret->t_val.val_r.r_typeptr = 0;
	ret->t_val.val_r.r_lower = lower;
	ret->t_val.val_r.r_upper = upper;
	insert_type(++type_number, ret);
    }
    return ret;
}

typeptr mkfloat(char *n, int bytes)
{
    typeptr ret;

    if (!(ret = name2typedef(n))) {
	ret = newtype(ns_inter, FLOAT_TYPE, n, 1);
	ret->t_val.val_g.g_typeptr = 0;
	ret->t_val.val_g.g_size = bytes;
	insert_type(++type_number, ret);
    }
    return ret;
}

void mkconst(cnode *c, int value)
{
    c->c_type = mkrange("int", 0x80000000, 0x7fffffff);
    c->c_base = int_type;
    c->c_const = 1;
    c->c_expr = new_expr();
    c->c_expr->e_func.i = i_leaf;
    c->c_expr->e_i = value;
    c->c_expr->e_size = sizeof(int);
}

int mk_incdec(cnode *result, cnode *lvalue, int op)
{
    cnode one;

    mkconst(&one, 1);
    return mkasgn(result, lvalue, op, &one);
}

void print_expression(cnode *c)
{
    all value;
    int size;
    char *p;
    char numbuf[16];
    char smallbuf[128];

    eval_all(&value, c);

    if ((size = c->c_expr->e_size) > sizeof(smallbuf)) {
	if (!(p = malloc(size))) {
	    fprintf(stderr, "Out of memory in print_expressin\n");
	    exit(1);
	}
    } else
	p = smallbuf;

    numbuf[0] = 0;
    if (c->c_base == struct_type) {
	v_read(p, (v_ptr)value.st, size);
	sprintf(numbuf, "0x%08x", value.st);
    } else if (c->c_type->t_type == ARRAY_TYPE)
	v_read(p, (v_ptr)value.ul, size);
    else
	p = (char *)&value;

    print_out(c->c_type, p, 0, 0, 0, numbuf);
    if (p != smallbuf && p != (char *)&value)
	free(p);
}

void *get_user_sym_addr(char *name)
{
    symptr sym = name2userdef(name);
    cnode c;
    all a;

    if (!sym)
	return 0;
    mkident(&c, name);
    eval_all(&a, &c);
    return (void *)a.i;
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
}
