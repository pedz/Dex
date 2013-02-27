h39010
s 00002/00002/00347
d D 1.4 10/08/23 17:01:52 pedzan 4 3
e
s 00057/00005/00292
d D 1.3 02/03/14 16:12:30 pedz 3 2
e
s 00000/00000/00297
d D 1.2 00/09/18 17:56:58 pedz 2 1
c Checking before V5 conversion
e
s 00297/00000/00000
d D 1.1 95/02/09 21:40:23 pedz 1 0
c date and time created 95/02/09 21:40:23 by pedz
e
u
U
t
T
I 1
static char sccs_id[] = "%W%";

#include <stdio.h>
#include "map.h"
D 3
#include "sym.h"
E 3
#include "tree.h"
#include "binary_expr.h"
I 3
#define DEBUG_BIT BINARY_EXPR_C_BIT
E 3

/*
 * All the binary operators crossed with all of the base types.  The
 * question/colon ternary operator is also in this file for all the
 * types.
 *
 * This file also contains a complete cross matrix of cast functions
 * e.g. c__i casts an int into a char.  All of these might not be
 * needed but if we have all of them we won't ever need more.
D 3
 * 
 * At the current time, the relational operators return the same
 * result type as the argument types.  They should return int instead.
 * I don't know what to do about this at this time.
E 3
 */

signed char sc_comma(expr *n)  { return sc_val(n->e_left) ,  sc_val(n->e_right); }
signed char sc_times(expr *n)  { return sc_val(n->e_left) *  sc_val(n->e_right); }
signed char sc_divide(expr *n) { return sc_val(n->e_left) /  sc_val(n->e_right); }
signed char sc_mod(expr *n)    { return sc_val(n->e_left) %  sc_val(n->e_right); }
signed char sc_plus(expr *n)   { return sc_val(n->e_left) +  sc_val(n->e_right); }
signed char sc_minue(expr *n)  { return sc_val(n->e_left) -  sc_val(n->e_right); }
signed char sc_rshift(expr *n) { return sc_val(n->e_left) >> i_val(n->e_right); }
signed char sc_lshift(expr *n) { return sc_val(n->e_left) << i_val(n->e_right); }

int sc_lt(expr *n)     { return sc_val(n->e_left) <  sc_val(n->e_right); }
int sc_gt(expr *n)     { return sc_val(n->e_left) >  sc_val(n->e_right); }
int sc_ge(expr *n)     { return sc_val(n->e_left) >= sc_val(n->e_right); }
int sc_le(expr *n)     { return sc_val(n->e_left) <= sc_val(n->e_right); }
int sc_eq(expr *n)     { return sc_val(n->e_left) == sc_val(n->e_right); }
int sc_ne(expr *n)     { return sc_val(n->e_left) != sc_val(n->e_right); }

signed char sc_and(expr *n)    { return sc_val(n->e_left) &  sc_val(n->e_right); }
signed char sc_xor(expr *n)    { return sc_val(n->e_left) ^  sc_val(n->e_right); }
signed char sc_or(expr *n)     { return sc_val(n->e_left) |  sc_val(n->e_right); }
signed char sc_andand(expr *n) { return sc_val(n->e_left) && sc_val(n->e_right); }
signed char sc_oror(expr *n)   { return sc_val(n->e_left) || sc_val(n->e_right); }
signed char sc_qc(expr *n)
{
    return i_val(n->e_left) ?
	sc_val(n->e_right->e_left) :
	    sc_val(n->e_right->e_right);
}

unsigned char uc_comma(expr *n)  { return uc_val(n->e_left) ,  uc_val(n->e_right); }
unsigned char uc_times(expr *n)  { return uc_val(n->e_left) *  uc_val(n->e_right); }
unsigned char uc_divide(expr *n) { return uc_val(n->e_left) /  uc_val(n->e_right); }
unsigned char uc_mod(expr *n)    { return uc_val(n->e_left) %  uc_val(n->e_right); }
unsigned char uc_plus(expr *n)   { return uc_val(n->e_left) +  uc_val(n->e_right); }
unsigned char uc_minue(expr *n)  { return uc_val(n->e_left) -  uc_val(n->e_right); }
unsigned char uc_rshift(expr *n) { return uc_val(n->e_left) >> i_val(n->e_right); }
unsigned char uc_lshift(expr *n) { return uc_val(n->e_left) << i_val(n->e_right); }

int uc_lt(expr *n)     { return uc_val(n->e_left) <  uc_val(n->e_right); }
int uc_gt(expr *n)     { return uc_val(n->e_left) >  uc_val(n->e_right); }
int uc_ge(expr *n)     { return uc_val(n->e_left) >= uc_val(n->e_right); }
int uc_le(expr *n)     { return uc_val(n->e_left) <= uc_val(n->e_right); }
int uc_eq(expr *n)     { return uc_val(n->e_left) == uc_val(n->e_right); }
int uc_ne(expr *n)     { return uc_val(n->e_left) != uc_val(n->e_right); }

unsigned char uc_and(expr *n)    { return uc_val(n->e_left) &  uc_val(n->e_right); }
unsigned char uc_xor(expr *n)    { return uc_val(n->e_left) ^  uc_val(n->e_right); }
unsigned char uc_or(expr *n)     { return uc_val(n->e_left) |  uc_val(n->e_right); }
unsigned char uc_andand(expr *n) { return uc_val(n->e_left) && uc_val(n->e_right); }
unsigned char uc_oror(expr *n)   { return uc_val(n->e_left) || uc_val(n->e_right); }
unsigned char uc_qc(expr *n)
{
    return i_val(n->e_left) ?
	uc_val(n->e_right->e_left) :
	    uc_val(n->e_right->e_right);
}

int i_comma(expr *n)  { return i_val(n->e_left) ,  i_val(n->e_right); }
int i_times(expr *n)  { return i_val(n->e_left) *  i_val(n->e_right); }
int i_divide(expr *n) { return i_val(n->e_left) /  i_val(n->e_right); }
int i_mod(expr *n)    { return i_val(n->e_left) %  i_val(n->e_right); }
int i_plus(expr *n)   { return i_val(n->e_left) +  i_val(n->e_right); }
int i_minue(expr *n)  { return i_val(n->e_left) -  i_val(n->e_right); }
int i_rshift(expr *n) { return i_val(n->e_left) >> i_val(n->e_right); }
int i_lshift(expr *n) { return i_val(n->e_left) << i_val(n->e_right); }

int i_lt(expr *n)     { return i_val(n->e_left) <  i_val(n->e_right); }
int i_gt(expr *n)     { return i_val(n->e_left) >  i_val(n->e_right); }
int i_ge(expr *n)     { return i_val(n->e_left) >= i_val(n->e_right); }
int i_le(expr *n)     { return i_val(n->e_left) <= i_val(n->e_right); }
int i_eq(expr *n)     { return i_val(n->e_left) == i_val(n->e_right); }
int i_ne(expr *n)     { return i_val(n->e_left) != i_val(n->e_right); }

int i_and(expr *n)    { return i_val(n->e_left) &  i_val(n->e_right); }
int i_xor(expr *n)    { return i_val(n->e_left) ^  i_val(n->e_right); }
int i_or(expr *n)     { return i_val(n->e_left) |  i_val(n->e_right); }
int i_andand(expr *n) { return i_val(n->e_left) && i_val(n->e_right); }
int i_oror(expr *n)   { return i_val(n->e_left) || i_val(n->e_right); }
int i_qc(expr *n)
{
    return i_val(n->e_left) ?
	i_val(n->e_right->e_left) :
	    i_val(n->e_right->e_right);
}

unsigned int ui_comma(expr *n)  { return ui_val(n->e_left) ,  ui_val(n->e_right); }
unsigned int ui_times(expr *n)  { return ui_val(n->e_left) *  ui_val(n->e_right); }
unsigned int ui_divide(expr *n) { return ui_val(n->e_left) /  ui_val(n->e_right); }
unsigned int ui_mod(expr *n)    { return ui_val(n->e_left) %  ui_val(n->e_right); }
unsigned int ui_plus(expr *n)   { return ui_val(n->e_left) +  ui_val(n->e_right); }
unsigned int ui_minue(expr *n)  { return ui_val(n->e_left) -  ui_val(n->e_right); }
unsigned int ui_rshift(expr *n) { return ui_val(n->e_left) >> i_val(n->e_right); }
unsigned int ui_lshift(expr *n) { return ui_val(n->e_left) << i_val(n->e_right); }

int ui_lt(expr *n)     { return ui_val(n->e_left) <  ui_val(n->e_right); }
int ui_gt(expr *n)     { return ui_val(n->e_left) >  ui_val(n->e_right); }
int ui_ge(expr *n)     { return ui_val(n->e_left) >= ui_val(n->e_right); }
int ui_le(expr *n)     { return ui_val(n->e_left) <= ui_val(n->e_right); }
int ui_eq(expr *n)     { return ui_val(n->e_left) == ui_val(n->e_right); }
int ui_ne(expr *n)     { return ui_val(n->e_left) != ui_val(n->e_right); }

unsigned int ui_and(expr *n)    { return ui_val(n->e_left) &  ui_val(n->e_right); }
unsigned int ui_xor(expr *n)    { return ui_val(n->e_left) ^  ui_val(n->e_right); }
unsigned int ui_or(expr *n)     { return ui_val(n->e_left) |  ui_val(n->e_right); }
unsigned int ui_andand(expr *n) { return ui_val(n->e_left) && ui_val(n->e_right); }
unsigned int ui_oror(expr *n)   { return ui_val(n->e_left) || ui_val(n->e_right); }
unsigned int ui_qc(expr *n)
{
    return i_val(n->e_left) ?
	ui_val(n->e_right->e_left) :
	    ui_val(n->e_right->e_right);
}

short s_comma(expr *n)  { return s_val(n->e_left) ,  s_val(n->e_right); }
short s_times(expr *n)  { return s_val(n->e_left) *  s_val(n->e_right); }
short s_divide(expr *n) { return s_val(n->e_left) /  s_val(n->e_right); }
short s_mod(expr *n)    { return s_val(n->e_left) %  s_val(n->e_right); }
short s_plus(expr *n)   { return s_val(n->e_left) +  s_val(n->e_right); }
short s_minue(expr *n)  { return s_val(n->e_left) -  s_val(n->e_right); }
short s_rshift(expr *n) { return s_val(n->e_left) >> i_val(n->e_right); }
short s_lshift(expr *n) { return s_val(n->e_left) << i_val(n->e_right); }

int s_lt(expr *n)     { return s_val(n->e_left) <  s_val(n->e_right); }
int s_gt(expr *n)     { return s_val(n->e_left) >  s_val(n->e_right); }
int s_ge(expr *n)     { return s_val(n->e_left) >= s_val(n->e_right); }
int s_le(expr *n)     { return s_val(n->e_left) <= s_val(n->e_right); }
int s_eq(expr *n)     { return s_val(n->e_left) == s_val(n->e_right); }
int s_ne(expr *n)     { return s_val(n->e_left) != s_val(n->e_right); }

short s_and(expr *n)    { return s_val(n->e_left) &  s_val(n->e_right); }
short s_xor(expr *n)    { return s_val(n->e_left) ^  s_val(n->e_right); }
short s_or(expr *n)     { return s_val(n->e_left) |  s_val(n->e_right); }
short s_andand(expr *n) { return s_val(n->e_left) && s_val(n->e_right); }
short s_oror(expr *n)   { return s_val(n->e_left) || s_val(n->e_right); }
short s_qc(expr *n)
{
    return i_val(n->e_left) ?
	s_val(n->e_right->e_left) :
	    s_val(n->e_right->e_right);
}

unsigned short us_comma(expr *n)  { return us_val(n->e_left) ,  us_val(n->e_right); }
unsigned short us_times(expr *n)  { return us_val(n->e_left) *  us_val(n->e_right); }
unsigned short us_divide(expr *n) { return us_val(n->e_left) /  us_val(n->e_right); }
unsigned short us_mod(expr *n)    { return us_val(n->e_left) %  us_val(n->e_right); }
unsigned short us_plus(expr *n)   { return us_val(n->e_left) +  us_val(n->e_right); }
unsigned short us_minue(expr *n)  { return us_val(n->e_left) -  us_val(n->e_right); }
unsigned short us_rshift(expr *n) { return us_val(n->e_left) >> i_val(n->e_right); }
unsigned short us_lshift(expr *n) { return us_val(n->e_left) << i_val(n->e_right); }

int us_lt(expr *n)     { return us_val(n->e_left) <  us_val(n->e_right); }
int us_gt(expr *n)     { return us_val(n->e_left) >  us_val(n->e_right); }
int us_ge(expr *n)     { return us_val(n->e_left) >= us_val(n->e_right); }
int us_le(expr *n)     { return us_val(n->e_left) <= us_val(n->e_right); }
int us_eq(expr *n)     { return us_val(n->e_left) == us_val(n->e_right); }
int us_ne(expr *n)     { return us_val(n->e_left) != us_val(n->e_right); }

unsigned short us_and(expr *n)    { return us_val(n->e_left) &  us_val(n->e_right); }
unsigned short us_xor(expr *n)    { return us_val(n->e_left) ^  us_val(n->e_right); }
unsigned short us_or(expr *n)     { return us_val(n->e_left) |  us_val(n->e_right); }
unsigned short us_andand(expr *n) { return us_val(n->e_left) && us_val(n->e_right); }
unsigned short us_oror(expr *n)   { return us_val(n->e_left) || us_val(n->e_right); }
unsigned short us_qc(expr *n)
{
    return i_val(n->e_left) ?
	us_val(n->e_right->e_left) :
	    us_val(n->e_right->e_right);
}

long l_comma(expr *n)  { return l_val(n->e_left) ,  l_val(n->e_right); }
long l_times(expr *n)  { return l_val(n->e_left) *  l_val(n->e_right); }
long l_divide(expr *n) { return l_val(n->e_left) /  l_val(n->e_right); }
long l_mod(expr *n)    { return l_val(n->e_left) %  l_val(n->e_right); }
long l_plus(expr *n)   { return l_val(n->e_left) +  l_val(n->e_right); }
long l_minue(expr *n)  { return l_val(n->e_left) -  l_val(n->e_right); }
long l_rshift(expr *n) { return l_val(n->e_left) >> i_val(n->e_right); }
long l_lshift(expr *n) { return l_val(n->e_left) << i_val(n->e_right); }

int l_lt(expr *n)     { return l_val(n->e_left) <  l_val(n->e_right); }
int l_gt(expr *n)     { return l_val(n->e_left) >  l_val(n->e_right); }
int l_ge(expr *n)     { return l_val(n->e_left) >= l_val(n->e_right); }
int l_le(expr *n)     { return l_val(n->e_left) <= l_val(n->e_right); }
int l_eq(expr *n)     { return l_val(n->e_left) == l_val(n->e_right); }
int l_ne(expr *n)     { return l_val(n->e_left) != l_val(n->e_right); }

long l_and(expr *n)    { return l_val(n->e_left) &  l_val(n->e_right); }
long l_xor(expr *n)    { return l_val(n->e_left) ^  l_val(n->e_right); }
long l_or(expr *n)     { return l_val(n->e_left) |  l_val(n->e_right); }
long l_andand(expr *n) { return l_val(n->e_left) && l_val(n->e_right); }
long l_oror(expr *n)   { return l_val(n->e_left) || l_val(n->e_right); }
long l_qc(expr *n)
{
    return i_val(n->e_left) ?
	l_val(n->e_right->e_left) :
	    l_val(n->e_right->e_right);
}

unsigned long ul_comma(expr *n)  { return ul_val(n->e_left) ,  ul_val(n->e_right); }
unsigned long ul_times(expr *n)  { return ul_val(n->e_left) *  ul_val(n->e_right); }
unsigned long ul_divide(expr *n) { return ul_val(n->e_left) /  ul_val(n->e_right); }
unsigned long ul_mod(expr *n)    { return ul_val(n->e_left) %  ul_val(n->e_right); }
unsigned long ul_plus(expr *n)   { return ul_val(n->e_left) +  ul_val(n->e_right); }
unsigned long ul_minue(expr *n)  { return ul_val(n->e_left) -  ul_val(n->e_right); }
unsigned long ul_rshift(expr *n) { return ul_val(n->e_left) >> i_val(n->e_right); }
unsigned long ul_lshift(expr *n) { return ul_val(n->e_left) << i_val(n->e_right); }

int ul_lt(expr *n)     { return ul_val(n->e_left) <  ul_val(n->e_right); }
int ul_gt(expr *n)     { return ul_val(n->e_left) >  ul_val(n->e_right); }
int ul_ge(expr *n)     { return ul_val(n->e_left) >= ul_val(n->e_right); }
int ul_le(expr *n)     { return ul_val(n->e_left) <= ul_val(n->e_right); }
int ul_eq(expr *n)     { return ul_val(n->e_left) == ul_val(n->e_right); }
int ul_ne(expr *n)     { return ul_val(n->e_left) != ul_val(n->e_right); }

unsigned long ul_and(expr *n)    { return ul_val(n->e_left) &  ul_val(n->e_right); }
unsigned long ul_xor(expr *n)    { return ul_val(n->e_left) ^  ul_val(n->e_right); }
unsigned long ul_or(expr *n)     { return ul_val(n->e_left) |  ul_val(n->e_right); }
unsigned long ul_andand(expr *n) { return ul_val(n->e_left) && ul_val(n->e_right); }
unsigned long ul_oror(expr *n)   { return ul_val(n->e_left) || ul_val(n->e_right); }
unsigned long ul_qc(expr *n)
{
    return i_val(n->e_left) ?
	ul_val(n->e_right->e_left) :
	    ul_val(n->e_right->e_right);
I 3
}

long long ll_comma(expr *n)  { return ll_val(n->e_left) ,  ll_val(n->e_right); }
long long ll_times(expr *n)  { return ll_val(n->e_left) *  ll_val(n->e_right); }
long long ll_divide(expr *n) { return ll_val(n->e_left) /  ll_val(n->e_right); }
long long ll_mod(expr *n)    { return ll_val(n->e_left) %  ll_val(n->e_right); }
long long ll_plus(expr *n)   { return ll_val(n->e_left) +  ll_val(n->e_right); }
long long ll_minue(expr *n)  { return ll_val(n->e_left) -  ll_val(n->e_right); }
long long ll_rshift(expr *n) { return ll_val(n->e_left) >> i_val(n->e_right); }
long long ll_lshift(expr *n) { return ll_val(n->e_left) << i_val(n->e_right); }

int ll_lt(expr *n)     { return ll_val(n->e_left) <  ll_val(n->e_right); }
int ll_gt(expr *n)     { return ll_val(n->e_left) >  ll_val(n->e_right); }
int ll_ge(expr *n)     { return ll_val(n->e_left) >= ll_val(n->e_right); }
int ll_le(expr *n)     { return ll_val(n->e_left) <= ll_val(n->e_right); }
int ll_eq(expr *n)     { return ll_val(n->e_left) == ll_val(n->e_right); }
int ll_ne(expr *n)     { return ll_val(n->e_left) != ll_val(n->e_right); }

long long ll_and(expr *n)    { return ll_val(n->e_left) &  ll_val(n->e_right); }
long long ll_xor(expr *n)    { return ll_val(n->e_left) ^  ll_val(n->e_right); }
long long ll_or(expr *n)     { return ll_val(n->e_left) |  ll_val(n->e_right); }
long long ll_andand(expr *n) { return ll_val(n->e_left) && ll_val(n->e_right); }
long long ll_oror(expr *n)   { return ll_val(n->e_left) || ll_val(n->e_right); }
long long ll_qc(expr *n)
{
    return i_val(n->e_left) ?
	ll_val(n->e_right->e_left) :
	    ll_val(n->e_right->e_right);
}

unsigned long long ull_comma(expr *n)  { return ull_val(n->e_left) ,  ull_val(n->e_right); }
unsigned long long ull_times(expr *n)  { return ull_val(n->e_left) *  ull_val(n->e_right); }
unsigned long long ull_divide(expr *n) { return ull_val(n->e_left) /  ull_val(n->e_right); }
unsigned long long ull_mod(expr *n)    { return ull_val(n->e_left) %  ull_val(n->e_right); }
unsigned long long ull_plus(expr *n)   { return ull_val(n->e_left) +  ull_val(n->e_right); }
unsigned long long ull_minue(expr *n)  { return ull_val(n->e_left) -  ull_val(n->e_right); }
unsigned long long ull_rshift(expr *n) { return ull_val(n->e_left) >> i_val(n->e_right); }
unsigned long long ull_lshift(expr *n) { return ull_val(n->e_left) << i_val(n->e_right); }

int ull_lt(expr *n)     { return ull_val(n->e_left) <  ull_val(n->e_right); }
int ull_gt(expr *n)     { return ull_val(n->e_left) >  ull_val(n->e_right); }
int ull_ge(expr *n)     { return ull_val(n->e_left) >= ull_val(n->e_right); }
int ull_le(expr *n)     { return ull_val(n->e_left) <= ull_val(n->e_right); }
int ull_eq(expr *n)     { return ull_val(n->e_left) == ull_val(n->e_right); }
int ull_ne(expr *n)     { return ull_val(n->e_left) != ull_val(n->e_right); }

unsigned long long ull_and(expr *n)    { return ull_val(n->e_left) &  ull_val(n->e_right); }
unsigned long long ull_xor(expr *n)    { return ull_val(n->e_left) ^  ull_val(n->e_right); }
unsigned long long ull_or(expr *n)     { return ull_val(n->e_left) |  ull_val(n->e_right); }
unsigned long long ull_andand(expr *n) { return ull_val(n->e_left) && ull_val(n->e_right); }
unsigned long long ull_oror(expr *n)   { return ull_val(n->e_left) || ull_val(n->e_right); }
unsigned long long ull_qc(expr *n)
{
    return i_val(n->e_left) ?
	ull_val(n->e_right->e_left) :
	    ull_val(n->e_right->e_right);
E 3
}

float f_comma(expr *n)  { return f_val(n->e_left) ,  f_val(n->e_right); }
float f_times(expr *n)  { return f_val(n->e_left) *  f_val(n->e_right); }
float f_divide(expr *n) { return f_val(n->e_left) /  f_val(n->e_right); }
float f_plus(expr *n)   { return f_val(n->e_left) +  f_val(n->e_right); }
float f_minue(expr *n)  { return f_val(n->e_left) -  f_val(n->e_right); }

int f_lt(expr *n)     { return f_val(n->e_left) <  f_val(n->e_right); }
int f_gt(expr *n)     { return f_val(n->e_left) >  f_val(n->e_right); }
int f_ge(expr *n)     { return f_val(n->e_left) >= f_val(n->e_right); }
int f_le(expr *n)     { return f_val(n->e_left) <= f_val(n->e_right); }
int f_eq(expr *n)     { return f_val(n->e_left) == f_val(n->e_right); }
int f_ne(expr *n)     { return f_val(n->e_left) != f_val(n->e_right); }

float f_andand(expr *n) { return f_val(n->e_left) && f_val(n->e_right); }
float f_oror(expr *n)   { return f_val(n->e_left) || f_val(n->e_right); }
float f_qc(expr *n)
{
    return i_val(n->e_left) ?
	f_val(n->e_right->e_left) :
	    f_val(n->e_right->e_right);
}

double d_comma(expr *n)  { return d_val(n->e_left) ,  d_val(n->e_right); }
double d_times(expr *n)  { return d_val(n->e_left) *  d_val(n->e_right); }
double d_divide(expr *n) { return d_val(n->e_left) /  d_val(n->e_right); }
double d_plus(expr *n)   { return d_val(n->e_left) +  d_val(n->e_right); }
double d_minue(expr *n)  { return d_val(n->e_left) -  d_val(n->e_right); }

int d_lt(expr *n)     { return d_val(n->e_left) <  d_val(n->e_right); }
int d_gt(expr *n)     { return d_val(n->e_left) >  d_val(n->e_right); }
int d_ge(expr *n)     { return d_val(n->e_left) >= d_val(n->e_right); }
int d_le(expr *n)     { return d_val(n->e_left) <= d_val(n->e_right); }
int d_eq(expr *n)     { return d_val(n->e_left) == d_val(n->e_right); }
int d_ne(expr *n)     { return d_val(n->e_left) != d_val(n->e_right); }

double d_andand(expr *n) { return d_val(n->e_left) && d_val(n->e_right); }
double d_oror(expr *n)   { return d_val(n->e_left) || d_val(n->e_right); }
double d_qc(expr *n)
{
    return i_val(n->e_left) ?
	d_val(n->e_right->e_left) :
	    d_val(n->e_right->e_right);
}

D 4
st st_comma(expr *n)  { return st_val(n->e_left) ,  st_val(n->e_right); }
st st_qc(expr *n)
E 4
I 4
st_t st_comma(expr *n)  { return st_val(n->e_left) ,  st_val(n->e_right); }
st_t st_qc(expr *n)
E 4
{
    return i_val(n->e_left) ?
	st_val(n->e_right->e_left) :
	    st_val(n->e_right->e_right);
}
E 1
