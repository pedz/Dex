static char sccs_id[] = "@(#)pre-asgn_expr.c	1.1";

#include "map.h"
#include "sym.h"
#include "tree.h"

/*
 * includes all of the assignment ops
 */

/*
 * Define a single assigned TYPE function name "PREFIX"_"SUFFIX" which
 * uses OP. 
 */
#define asgn_op(type, prefix, suffix, op) \
type prefix ## _ ## suffix (expr *n) \
{ \
    type t = prefix ## _val(n->e_right); \
    type *l = prefix ## _addr(n->e_left); \
    type x; \
 \
    v_read(&x, l, sizeof(type)); \
    x op t; \
    v_write(&x, l, sizeof(type)); \
    return x; \
}

/*
 * Define a function for each type -- inclues ints and doubles
 */
#define all_op(suffix, op) \
asgn_op(signed char, sc, suffix, op) \
asgn_op(unsigned char, uc, suffix, op) \
asgn_op(int, i, suffix, op) \
asgn_op(unsigned int, ui, suffix, op) \
asgn_op(short, s, suffix, op) \
asgn_op(unsigned short, us, suffix, op) \
asgn_op(long, l, suffix, op) \
asgn_op(unsigned long, ul, suffix, op) \
asgn_op(float, f, suffix, op) \
asgn_op(double, d, suffix, op)

all_op(asgn, =)

st st_asgn(expr *n)
{
    st r = st_val(n->e_right);
    st *l = st_addr(n->e_left);
    char buf[PAGESIZE];
    int i = n->e_size;
    char *from = (char *)r;
    char *to = (char *)l;
    int move;
    int temp;

    while (i) {
	move = i;			/* assume we can do the whole thing */
	if (move > sizeof(buf))		/* move must fit buffer */
	    move = sizeof(buf);
	temp = PAGESIZE - ((unsigned long)from & (PAGESIZE - 1));
	if (move > temp)		/* from can't cross page boundry */
	    move = temp;
	temp = PAGESIZE - ((unsigned long)to & (PAGESIZE - 1));
	if (move > temp)		/* to can't cross page boundry */
	    move = temp;
	v_read(buf, from, move);	/* fetch the data */
	v_write(buf, to, move);		/* stuff the data */
	i -= move;			/* move to next chunk */
	from += move;
	to += move;
    }
    return (st)to;
}

all_op(plusasgn, +=)
all_op(minusasgn, -=)
all_op(timesasgn, *=)
all_op(divasgn, /=)

/*
 * define functions for int types only
 */
#define int_op(suffix, op) \
asgn_op(signed char, sc, suffix, op) \
asgn_op(unsigned char, uc, suffix, op) \
asgn_op(int, i, suffix, op) \
asgn_op(unsigned int, ui, suffix, op) \
asgn_op(short, s, suffix, op) \
asgn_op(unsigned short, us, suffix, op) \
asgn_op(long, l, suffix, op) \
asgn_op(unsigned long, ul, suffix, op)

int_op(modasgn, %=)
int_op(andasgn, &=)
int_op(orasgn, |=)


/*
 * Define a single assigned type for a shift operator where the right
 * is an int.
 */
#define shift_asgn_op(type, prefix, suffix, op) \
type prefix ## _ ## suffix (expr *n) \
{ \
    int t = i_val(n->e_right); \
    type *l = prefix ## _addr(n->e_left); \
    return *l op t; \
}

/*
 * define functions for shift ops where right tree is always an int
 */
#define shift_op(suffix, op) \
shift_asgn_op(signed char, sc, suffix, op) \
shift_asgn_op(unsigned char, uc, suffix, op) \
shift_asgn_op(int, i, suffix, op) \
shift_asgn_op(unsigned int, ui, suffix, op) \
shift_asgn_op(short, s, suffix, op) \
shift_asgn_op(unsigned short, us, suffix, op) \
shift_asgn_op(long, l, suffix, op) \
shift_asgn_op(unsigned long, ul, suffix, op)

shift_op(lsasgn, <<=)
shift_op(rsasgn, >>=)

/*
 * Define a single assigned TYPE function name "PREFIX"_"SUFFIX" which
 * uses OP. 
 */
#define post_asgn_op(type, prefix, suffix, op) \
type prefix ## _ ## suffix (expr *n) \
{ \
    type t = prefix ## _val(n->e_right); \
    type *l = prefix ## _addr(n->e_left); \
    type x; \
    type s; \
 \
    v_read(&x, l, sizeof(type)); \
    s = x; \
    x op t; \
    v_write(&x, l, sizeof(type)); \
    return s; \
}

/*
 * Define a function for each type -- inclues ints and doubles
 */
#define post_all_op(suffix, op) \
post_asgn_op(signed char, sc, suffix, op) \
post_asgn_op(unsigned char, uc, suffix, op) \
post_asgn_op(int, i, suffix, op) \
post_asgn_op(unsigned int, ui, suffix, op) \
post_asgn_op(short, s, suffix, op) \
post_asgn_op(unsigned short, us, suffix, op) \
post_asgn_op(long, l, suffix, op) \
post_asgn_op(unsigned long, ul, suffix, op) \
post_asgn_op(float, f, suffix, op) \
post_asgn_op(double, d, suffix, op)

post_all_op(inc, +=)
post_all_op(dec, -=)
