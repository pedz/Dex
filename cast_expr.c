static char sccs_id[] = "@(#)cast_expr.c	1.1";

#include <stdio.h>
#include "map.h"
#include "sym.h"
#include "tree.h"
#include "cast_expr.h"

signed char sc__sc(expr *n) { return sc_val(n->e_left); }
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
