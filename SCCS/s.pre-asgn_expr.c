h53980
s 00004/00004/00168
d D 1.7 10/08/23 17:01:54 pedzan 7 6
e
s 00017/00003/00155
d D 1.6 02/03/14 16:12:33 pedz 6 5
e
s 00000/00000/00158
d D 1.5 00/09/18 17:57:02 pedz 5 4
c Checking before V5 conversion
e
s 00003/00000/00155
d D 1.4 98/10/23 12:26:11 pedz 4 3
e
s 00023/00003/00132
d D 1.3 95/02/09 21:35:51 pedz 3 2
e
s 00012/00038/00123
d D 1.2 95/02/01 10:37:01 pedz 2 1
e
s 00161/00000/00000
d D 1.1 94/08/22 17:56:34 pedz 1 0
c date and time created 94/08/22 17:56:34 by pedz
e
u
U
t
T
I 1
static char sccs_id[] = "%W%";

I 6
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
E 6
I 4
#include "dex.h"
E 4
#include "map.h"
#include "sym.h"
#include "tree.h"
I 4
#include "base_expr.h"
I 6
#include "asgn_expr.h"
E 6
E 4

I 6
#define DEBUG_BIT PRE_ASGN_EXPR_C_BIT

E 6
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
D 2
    type *l = prefix ## _addr(n->e_left); \
E 2
I 2
    type *p = v2f_type(type *, prefix ## _addr(n->e_left)); \
E 2
    type x; \
 \
D 2
    v_read(&x, l, sizeof(type)); \
    x op t; \
    v_write(&x, l, sizeof(type)); \
E 2
I 2
D 3
    x = ((*p) op t); \
E 3
I 3
    if (n->e_bsize) { \
	x = get_field(p, n->e_boffset, n->e_bsize); \
	x = x op t; \
	set_field(p, n->e_boffset, n->e_bsize, x); \
    } else \
	x = ((*p) op t); \
E 3
E 2
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
I 6
asgn_op(long long, ll, suffix, op) \
asgn_op(unsigned long long, ull, suffix, op) \
E 6
asgn_op(float, f, suffix, op) \
asgn_op(double, d, suffix, op)

all_op(asgn, =)

D 7
st st_asgn(expr *n)
E 7
I 7
st_t st_asgn(expr *n)
E 7
{
D 2
    st r = st_val(n->e_right);
    st *l = st_addr(n->e_left);
    char buf[PAGESIZE];
    int i = n->e_size;
    char *from = (char *)r;
    char *to = (char *)l;
    int move;
    int temp;
E 2
I 2
D 7
    st r = v2f_type(st, st_val(n->e_right));
    st *l = v2f_type(st *, st_addr(n->e_left));
E 7
I 7
    st_t r = v2f_type(st_t, st_val(n->e_right));
    st_t *l = v2f_type(st_t *, st_addr(n->e_left));
E 7
    int size = n->e_size;
E 2

D 2
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
E 2
I 2
D 6
    bcopy(r, l, size);
E 6
I 6
    bcopy((void *)r, (void *)l, size);
E 6
D 7
    return (st)l;
E 7
I 7
    return (st_t)l;
E 7
E 2
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
D 6
asgn_op(unsigned long, ul, suffix, op)
E 6
I 6
asgn_op(unsigned long, ul, suffix, op) \
asgn_op(long long, ll, suffix, op) \
asgn_op(unsigned long long, ull, suffix, op)
E 6

int_op(modasgn, %=)
int_op(andasgn, &=)
int_op(orasgn, |=)
I 4
int_op(xorasgn, ^=)
E 4


/*
 * Define a single assigned type for a shift operator where the right
 * is an int.
 */
#define shift_asgn_op(type, prefix, suffix, op) \
type prefix ## _ ## suffix (expr *n) \
{ \
    int t = i_val(n->e_right); \
D 2
    type *l = prefix ## _addr(n->e_left); \
E 2
I 2
    type *l = v2f_type(type *, prefix ## _addr(n->e_left)); \
I 3
 \
    if (n->e_bsize) { \
	type x = get_field(l, n->e_boffset, n->e_bsize); \
	 \
	x op t; \
	set_field(l, n->e_boffset, n->e_bsize, x); \
	return x; \
    } \
E 3
E 2
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
D 6
shift_asgn_op(unsigned long, ul, suffix, op)
E 6
I 6
shift_asgn_op(unsigned long, ul, suffix, op) \
shift_asgn_op(long long, ll, suffix, op) \
shift_asgn_op(unsigned long long, ull, suffix, op)
E 6

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
D 2
    type *l = prefix ## _addr(n->e_left); \
    type x; \
    type s; \
E 2
I 2
    type *p = v2f_type(type *, prefix ## _addr(n->e_left)); \
D 3
    type x = *p; \
E 3
I 3
    type x; \
E 3
E 2
 \
D 2
    v_read(&x, l, sizeof(type)); \
    s = x; \
    x op t; \
    v_write(&x, l, sizeof(type)); \
    return s; \
E 2
I 2
D 3
    (*p) op t; \
E 3
I 3
    if (n->e_bsize) { \
	type y = x = get_field(p, n->e_boffset, n->e_bsize); \
	y = y op t; \
	set_field(p, n->e_boffset, n->e_bsize, y); \
    } else { \
	x = *p; \
	(*p) op t; \
    } \
E 3
    return x; \
E 2
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
I 6
post_asgn_op(long long, ll, suffix, op) \
post_asgn_op(unsigned long long, ull, suffix, op) \
E 6
post_asgn_op(float, f, suffix, op) \
post_asgn_op(double, d, suffix, op)

post_all_op(inc, +=)
post_all_op(dec, -=)
E 1
