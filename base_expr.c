static char sccs_id[] = "@(#)base_expr.c	1.1";

#include "map.h"
#include "sym.h"
#include "tree.h"

/*
 * All the binary operators crossed with all of the base types.  The
 * question/colon ternary operator is also in this file for all the
 * types.
 *
 * This file also contains a complete cross matrix of cast functions
 * e.g. c__i casts an int into a char.  All of these might not be
 * needed but if we have all of them we won't ever need more.
 * 
 * At the current time, the relational operators return the same
 * result type as the argument types.  They should return int instead.
 * I don't know what to do about this at this time.
 */

signed char sc_comma(expr *n)  { return sc_val(n->e_left) ,  sc_val(n->e_right); }
signed char sc_times(expr *n)  { return sc_val(n->e_left) *  sc_val(n->e_right); }
signed char sc_divide(expr *n) { return sc_val(n->e_left) /  sc_val(n->e_right); }
signed char sc_mod(expr *n)    { return sc_val(n->e_left) %  sc_val(n->e_right); }
signed char sc_plus(expr *n)   { return sc_val(n->e_left) +  sc_val(n->e_right); }
signed char sc_minue(expr *n)  { return sc_val(n->e_left) -  sc_val(n->e_right); }
signed char sc_rshift(expr *n) { return sc_val(n->e_left) >> i_val(n->e_right); }
signed char sc_lshift(expr *n) { return sc_val(n->e_left) << i_val(n->e_right); }
signed char sc_lt(expr *n)     { return sc_val(n->e_left) <  sc_val(n->e_right); }
signed char sc_gt(expr *n)     { return sc_val(n->e_left) >  sc_val(n->e_right); }
signed char sc_ge(expr *n)     { return sc_val(n->e_left) >= sc_val(n->e_right); }
signed char sc_le(expr *n)     { return sc_val(n->e_left) <= sc_val(n->e_right); }
signed char sc_eq(expr *n)     { return sc_val(n->e_left) == sc_val(n->e_right); }
signed char sc_ne(expr *n)     { return sc_val(n->e_left) != sc_val(n->e_right); }
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
unsigned char uc_lt(expr *n)     { return uc_val(n->e_left) <  uc_val(n->e_right); }
unsigned char uc_gt(expr *n)     { return uc_val(n->e_left) >  uc_val(n->e_right); }
unsigned char uc_ge(expr *n)     { return uc_val(n->e_left) >= uc_val(n->e_right); }
unsigned char uc_le(expr *n)     { return uc_val(n->e_left) <= uc_val(n->e_right); }
unsigned char uc_eq(expr *n)     { return uc_val(n->e_left) == uc_val(n->e_right); }
unsigned char uc_ne(expr *n)     { return uc_val(n->e_left) != uc_val(n->e_right); }
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
unsigned int ui_lt(expr *n)     { return ui_val(n->e_left) <  ui_val(n->e_right); }
unsigned int ui_gt(expr *n)     { return ui_val(n->e_left) >  ui_val(n->e_right); }
unsigned int ui_ge(expr *n)     { return ui_val(n->e_left) >= ui_val(n->e_right); }
unsigned int ui_le(expr *n)     { return ui_val(n->e_left) <= ui_val(n->e_right); }
unsigned int ui_eq(expr *n)     { return ui_val(n->e_left) == ui_val(n->e_right); }
unsigned int ui_ne(expr *n)     { return ui_val(n->e_left) != ui_val(n->e_right); }
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
short s_lt(expr *n)     { return s_val(n->e_left) <  s_val(n->e_right); }
short s_gt(expr *n)     { return s_val(n->e_left) >  s_val(n->e_right); }
short s_ge(expr *n)     { return s_val(n->e_left) >= s_val(n->e_right); }
short s_le(expr *n)     { return s_val(n->e_left) <= s_val(n->e_right); }
short s_eq(expr *n)     { return s_val(n->e_left) == s_val(n->e_right); }
short s_ne(expr *n)     { return s_val(n->e_left) != s_val(n->e_right); }
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
unsigned short us_lt(expr *n)     { return us_val(n->e_left) <  us_val(n->e_right); }
unsigned short us_gt(expr *n)     { return us_val(n->e_left) >  us_val(n->e_right); }
unsigned short us_ge(expr *n)     { return us_val(n->e_left) >= us_val(n->e_right); }
unsigned short us_le(expr *n)     { return us_val(n->e_left) <= us_val(n->e_right); }
unsigned short us_eq(expr *n)     { return us_val(n->e_left) == us_val(n->e_right); }
unsigned short us_ne(expr *n)     { return us_val(n->e_left) != us_val(n->e_right); }
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
long l_lt(expr *n)     { return l_val(n->e_left) <  l_val(n->e_right); }
long l_gt(expr *n)     { return l_val(n->e_left) >  l_val(n->e_right); }
long l_ge(expr *n)     { return l_val(n->e_left) >= l_val(n->e_right); }
long l_le(expr *n)     { return l_val(n->e_left) <= l_val(n->e_right); }
long l_eq(expr *n)     { return l_val(n->e_left) == l_val(n->e_right); }
long l_ne(expr *n)     { return l_val(n->e_left) != l_val(n->e_right); }
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
unsigned long ul_lt(expr *n)     { return ul_val(n->e_left) <  ul_val(n->e_right); }
unsigned long ul_gt(expr *n)     { return ul_val(n->e_left) >  ul_val(n->e_right); }
unsigned long ul_ge(expr *n)     { return ul_val(n->e_left) >= ul_val(n->e_right); }
unsigned long ul_le(expr *n)     { return ul_val(n->e_left) <= ul_val(n->e_right); }
unsigned long ul_eq(expr *n)     { return ul_val(n->e_left) == ul_val(n->e_right); }
unsigned long ul_ne(expr *n)     { return ul_val(n->e_left) != ul_val(n->e_right); }
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
}

float f_comma(expr *n)  { return f_val(n->e_left) ,  f_val(n->e_right); }
float f_times(expr *n)  { return f_val(n->e_left) *  f_val(n->e_right); }
float f_divide(expr *n) { return f_val(n->e_left) /  f_val(n->e_right); }
float f_plus(expr *n)   { return f_val(n->e_left) +  f_val(n->e_right); }
float f_minue(expr *n)  { return f_val(n->e_left) -  f_val(n->e_right); }

float f_lt(expr *n)     { return f_val(n->e_left) <  f_val(n->e_right); }
float f_gt(expr *n)     { return f_val(n->e_left) >  f_val(n->e_right); }
float f_ge(expr *n)     { return f_val(n->e_left) >= f_val(n->e_right); }
float f_le(expr *n)     { return f_val(n->e_left) <= f_val(n->e_right); }
float f_eq(expr *n)     { return f_val(n->e_left) == f_val(n->e_right); }
float f_ne(expr *n)     { return f_val(n->e_left) != f_val(n->e_right); }
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

double d_lt(expr *n)     { return d_val(n->e_left) <  d_val(n->e_right); }
double d_gt(expr *n)     { return d_val(n->e_left) >  d_val(n->e_right); }
double d_ge(expr *n)     { return d_val(n->e_left) >= d_val(n->e_right); }
double d_le(expr *n)     { return d_val(n->e_left) <= d_val(n->e_right); }
double d_eq(expr *n)     { return d_val(n->e_left) == d_val(n->e_right); }
double d_ne(expr *n)     { return d_val(n->e_left) != d_val(n->e_right); }
double d_andand(expr *n) { return d_val(n->e_left) && d_val(n->e_right); }
double d_oror(expr *n)   { return d_val(n->e_left) || d_val(n->e_right); }
double d_qc(expr *n)
{
    return i_val(n->e_left) ?
	d_val(n->e_right->e_left) :
	    d_val(n->e_right->e_right);
}

st st_comma(expr *n)  { return st_val(n->e_left) ,  st_val(n->e_right); }
st st_qc(expr *n)
{
    return i_val(n->e_left) ?
	st_val(n->e_right->e_left) :
	    st_val(n->e_right->e_right);
}

signed char sc__sc(expr *n) { return sc_val(n->e_left);  }
signed char sc__uc(expr *n) { return uc_val(n->e_left); }
signed char sc__i(expr *n)  { return i_val(n->e_left);  }
signed char sc__ui(expr *n) { return ui_val(n->e_left); }
signed char sc__s(expr *n)  { return s_val(n->e_left);  }
signed char sc__us(expr *n) { return us_val(n->e_left); }
signed char sc__l(expr *n)  { return l_val(n->e_left);  }
signed char sc__ul(expr *n) { return ul_val(n->e_left); }
signed char sc__f(expr *n)  { return f_val(n->e_left);  }
signed char sc__d(expr *n)  { return d_val(n->e_left);  }

unsigned char uc__sc(expr *n) { return sc_val(n->e_left);  }
unsigned char uc__uc(expr *n) { return uc_val(n->e_left); }
unsigned char uc__i(expr *n)  { return i_val(n->e_left);  }
unsigned char uc__ui(expr *n) { return ui_val(n->e_left); }
unsigned char uc__s(expr *n)  { return s_val(n->e_left);  }
unsigned char uc__us(expr *n) { return us_val(n->e_left); }
unsigned char uc__l(expr *n)  { return l_val(n->e_left);  }
unsigned char uc__ul(expr *n) { return ul_val(n->e_left); }
unsigned char uc__f(expr *n)  { return f_val(n->e_left);  }
unsigned char uc__d(expr *n)  { return d_val(n->e_left);  }

int i__sc(expr *n) { return sc_val(n->e_left);  }
int i__uc(expr *n) { return uc_val(n->e_left); }
int i__i(expr *n)  { return i_val(n->e_left);  }
int i__ui(expr *n) { return ui_val(n->e_left); }
int i__s(expr *n)  { return s_val(n->e_left);  }
int i__us(expr *n) { return us_val(n->e_left); }
int i__l(expr *n)  { return l_val(n->e_left);  }
int i__ul(expr *n) { return ul_val(n->e_left); }
int i__f(expr *n)  { return f_val(n->e_left);  }
int i__d(expr *n)  { return d_val(n->e_left);  }

unsigned int ui__sc(expr *n) { return sc_val(n->e_left);  }
unsigned int ui__uc(expr *n) { return uc_val(n->e_left); }
unsigned int ui__i(expr *n)  { return i_val(n->e_left);  }
unsigned int ui__ui(expr *n) { return ui_val(n->e_left); }
unsigned int ui__s(expr *n)  { return s_val(n->e_left);  }
unsigned int ui__us(expr *n) { return us_val(n->e_left); }
unsigned int ui__l(expr *n)  { return l_val(n->e_left);  }
unsigned int ui__ul(expr *n) { return ul_val(n->e_left); }
unsigned int ui__f(expr *n)  { return f_val(n->e_left);  }
unsigned int ui__d(expr *n)  { return d_val(n->e_left);  }

short s__sc(expr *n) { return sc_val(n->e_left);  }
short s__uc(expr *n) { return uc_val(n->e_left); }
short s__i(expr *n)  { return i_val(n->e_left);  }
short s__ui(expr *n) { return ui_val(n->e_left); }
short s__s(expr *n)  { return s_val(n->e_left);  }
short s__us(expr *n) { return us_val(n->e_left); }
short s__l(expr *n)  { return l_val(n->e_left);  }
short s__ul(expr *n) { return ul_val(n->e_left); }
short s__f(expr *n)  { return f_val(n->e_left);  }
short s__d(expr *n)  { return d_val(n->e_left);  }

unsigned short us__sc(expr *n) { return sc_val(n->e_left);  }
unsigned short us__uc(expr *n) { return uc_val(n->e_left); }
unsigned short us__i(expr *n)  { return i_val(n->e_left);  }
unsigned short us__ui(expr *n) { return ui_val(n->e_left); }
unsigned short us__s(expr *n)  { return s_val(n->e_left);  }
unsigned short us__us(expr *n) { return us_val(n->e_left); }
unsigned short us__l(expr *n)  { return l_val(n->e_left);  }
unsigned short us__ul(expr *n) { return ul_val(n->e_left); }
unsigned short us__f(expr *n)  { return f_val(n->e_left);  }
unsigned short us__d(expr *n)  { return d_val(n->e_left);  }

long l__sc(expr *n) { return sc_val(n->e_left);  }
long l__uc(expr *n) { return uc_val(n->e_left); }
long l__i(expr *n)  { return i_val(n->e_left);  }
long l__ui(expr *n) { return ui_val(n->e_left); }
long l__s(expr *n)  { return s_val(n->e_left);  }
long l__us(expr *n) { return us_val(n->e_left); }
long l__l(expr *n)  { return l_val(n->e_left);  }
long l__ul(expr *n) { return ul_val(n->e_left); }
long l__f(expr *n)  { return f_val(n->e_left);  }
long l__d(expr *n)  { return d_val(n->e_left);  }

unsigned long ul__sc(expr *n) { return sc_val(n->e_left);  }
unsigned long ul__uc(expr *n) { return uc_val(n->e_left); }
unsigned long ul__i(expr *n)  { return i_val(n->e_left);  }
unsigned long ul__ui(expr *n) { return ui_val(n->e_left); }
unsigned long ul__s(expr *n)  { return s_val(n->e_left);  }
unsigned long ul__us(expr *n) { return us_val(n->e_left); }
unsigned long ul__l(expr *n)  { return l_val(n->e_left);  }
unsigned long ul__ul(expr *n) { return ul_val(n->e_left); }
unsigned long ul__f(expr *n)  { return f_val(n->e_left);  }
unsigned long ul__d(expr *n)  { return d_val(n->e_left);  }

float f__sc(expr *n) { return sc_val(n->e_left);  }
float f__uc(expr *n) { return uc_val(n->e_left); }
float f__i(expr *n)  { return i_val(n->e_left);  }
float f__ui(expr *n) { return ui_val(n->e_left); }
float f__s(expr *n)  { return s_val(n->e_left);  }
float f__us(expr *n) { return us_val(n->e_left); }
float f__l(expr *n)  { return l_val(n->e_left);  }
float f__ul(expr *n) { return ul_val(n->e_left); }
float f__f(expr *n)  { return f_val(n->e_left);  }
float f__d(expr *n)  { return d_val(n->e_left);  }

double d__sc(expr *n) { return sc_val(n->e_left);  }
double d__uc(expr *n) { return uc_val(n->e_left); }
double d__i(expr *n)  { return i_val(n->e_left);  }
double d__ui(expr *n) { return ui_val(n->e_left); }
double d__s(expr *n)  { return s_val(n->e_left);  }
double d__us(expr *n) { return us_val(n->e_left); }
double d__l(expr *n)  { return l_val(n->e_left);  }
double d__ul(expr *n) { return ul_val(n->e_left); }
double d__f(expr *n)  { return f_val(n->e_left);  }
double d__d(expr *n)  { return d_val(n->e_left);  }

/* simple constant leaf nodes */

signed char sc_leaf(expr *n) { return n->e_c; }
unsigned char uc_leaf(expr *n) { return n->e_uc; }
int i_leaf(expr *n) { return n->e_i; }
unsigned int ui_leaf(expr *n) { return n->e_ui; }
short s_leaf(expr *n) { return n->e_s; }
unsigned short us_leaf(expr *n) { return n->e_us; }
long l_leaf(expr *n) { return n->e_l; }
unsigned long ul_leaf(expr *n) { return n->e_ul; }
float f_leaf(expr *n) { return n->e_f; }
double d_leaf(expr *n) { return n->e_d; }
st st_leaf(expr *n) { return n->e_st; }

/* global var address nodes */
signed char *sc_gaddr(expr *n) { return (signed char *)n->e_addr; }
unsigned char *uc_gaddr(expr *n) { return (unsigned char *)n->e_addr; }
int *i_gaddr(expr *n) { return (int *)n->e_addr; }
unsigned int *ui_gaddr(expr *n) { return (unsigned int *)n->e_addr; }
short *s_gaddr(expr *n) { return (short *)n->e_addr; }
unsigned short *us_gaddr(expr *n) { return (unsigned short *)n->e_addr; }
long *l_gaddr(expr *n) { return (long *)n->e_addr; }
unsigned long *ul_gaddr(expr *n) { return (unsigned long *)n->e_addr; }
float *f_gaddr(expr *n) { return (float *)n->e_addr; }
double *d_gaddr(expr *n) { return (double *)n->e_addr; }
st *st_gaddr(expr *n) { return (st *)n->e_addr; }

/* local var address nodes */
extern int frame_ptr;

signed char *sc_laddr(expr *n)
{
    return (signed char *)(n->e_addr + frame_ptr);
}

unsigned char *uc_laddr(expr *n)
{
    return (unsigned char *)(n->e_addr + frame_ptr);
}

int *i_laddr(expr *n)
{
    return (int *)(n->e_addr + frame_ptr);
}

unsigned int *ui_laddr(expr *n)
{
    return (unsigned int *)(n->e_addr + frame_ptr);
}

short *s_laddr(expr *n)
{
    return (short *)(n->e_addr + frame_ptr);
}

unsigned short *us_laddr(expr *n)
{
    return (unsigned short *)(n->e_addr + frame_ptr);
}

long *l_laddr(expr *n)
{
    return (long *)(n->e_addr + frame_ptr);
}

unsigned long *ul_laddr(expr *n)
{
    return (unsigned long *)(n->e_addr + frame_ptr);
}

float *f_laddr(expr *n)
{
    return (float *)(n->e_addr + frame_ptr);
}

double *d_laddr(expr *n)
{
    return (double *)(n->e_addr + frame_ptr);
}

st *st_laddr(expr *n)
{
    return (st *)(n->e_addr + frame_ptr);
}

/* Routines which take an lvalue into a pvalue */
signed char sc_l2p(expr *n)
{
    signed char v, *a;
    a = sc_addr(n->e_left);
    v_read(&v, a, sizeof(v));
    return v;
}
unsigned char uc_l2p(expr *n)
{
    unsigned char v, *a;
    a = uc_addr(n->e_left);
    v_read(&v, a, sizeof(v));
    return v;
}
int i_l2p(expr *n)
{
    int v, *a;
    a = i_addr(n->e_left);
    v_read(&v, a, sizeof(v));
    return v;
}
unsigned int ui_l2p(expr *n)
{
    unsigned int v, *a;
    a = ui_addr(n->e_left);
    v_read(&v, a, sizeof(v));
    return v;
}
short s_l2p(expr *n)
{
    short v, *a;
    a = s_addr(n->e_left);
    v_read(&v, a, sizeof(v));
    return v;
}
unsigned short us_l2p(expr *n)
{
    unsigned short v, *a;
    a = us_addr(n->e_left);
    v_read(&v, a, sizeof(v));
    return v;
}
long l_l2p(expr *n)
{
    long v, *a;
    a = l_addr(n->e_left);
    v_read(&v, a, sizeof(v));
    return v;
}
unsigned long ul_l2p(expr *n)
{
    unsigned long v, *a;
    a = ul_addr(n->e_left);
    v_read(&v, a, sizeof(v));
    return v;
}
float f_l2p(expr *n)
{
    float v, *a;
    a = f_addr(n->e_left);
    v_read(&v, a, sizeof(v));
    return v;
}
double d_l2p(expr *n)
{
    double v, *a;
    a = d_addr(n->e_left);
    v_read(&v, a, sizeof(v));
    return v;
}
st st_l2p(expr *n)
{
    st v, *a;
    a = st_addr(n->e_left);
    v = (st)a;
    return v;
}

signed char sc_null(expr *n) { return 0; }
unsigned char uc_null(expr *n) { return 0; }
int i_null(expr *n) { return 0; }
unsigned int ui_null(expr *n) { return 0; }
short s_null(expr *n) { return 0; }
unsigned short us_null(expr *n) { return 0; }
long l_null(expr *n) { return 0; }
unsigned long ul_null(expr *n) { return 0; }
float f_null(expr *n) { return 0; }
double d_null(expr *n) { return 0; }
st st_null(expr *n) { static st s; return s; }
