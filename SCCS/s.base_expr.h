h19859
s 00000/00000/00010
d D 1.7 10/08/23 17:13:02 pedzan 7 6
c Checking in changes made over the years
e
s 00006/00002/00004
d D 1.6 02/03/14 16:12:30 pedz 6 5
e
s 00000/00000/00006
d D 1.5 00/09/18 17:56:58 pedz 5 4
c Checking before V5 conversion
e
s 00002/00002/00004
d D 1.4 98/10/23 12:26:07 pedz 4 3
e
s 00004/00399/00002
d D 1.3 95/02/09 21:35:38 pedz 3 2
e
s 00024/00000/00377
d D 1.2 95/02/01 10:36:45 pedz 2 1
e
s 00377/00000/00000
d D 1.1 94/08/22 17:56:32 pedz 1 0
c date and time created 94/08/22 17:56:32 by pedz
e
u
U
t
T
I 6

E 6
I 1
/* %W% */
D 3
signed char sc_comma(expr *n);
signed char sc_times(expr *n);
signed char sc_divide(expr *n);
signed char sc_mod(expr *n);
signed char sc_plus(expr *n);
signed char sc_minue(expr *n);
signed char sc_rshift(expr *n);
signed char sc_lshift(expr *n);
signed char sc_lt(expr *n);
signed char sc_gt(expr *n);
signed char sc_ge(expr *n);
signed char sc_le(expr *n);
signed char sc_eq(expr *n);
signed char sc_ne(expr *n);
signed char sc_and(expr *n);
signed char sc_xor(expr *n);
signed char sc_or(expr *n);
signed char sc_andand(expr *n);
signed char sc_oror(expr *n);
signed char sc_qc(expr *n);
E 3

I 6
#ifndef __BASE_EXPR_H
#define __BASE_EXPR_H

E 6
D 3
unsigned char uc_comma(expr *n);
unsigned char uc_times(expr *n);
unsigned char uc_divide(expr *n);
unsigned char uc_mod(expr *n);
unsigned char uc_plus(expr *n);
unsigned char uc_minue(expr *n);
unsigned char uc_rshift(expr *n);
unsigned char uc_lshift(expr *n);
unsigned char uc_lt(expr *n);
unsigned char uc_gt(expr *n);
unsigned char uc_ge(expr *n);
unsigned char uc_le(expr *n);
unsigned char uc_eq(expr *n);
unsigned char uc_ne(expr *n);
unsigned char uc_and(expr *n);
unsigned char uc_xor(expr *n);
unsigned char uc_or(expr *n);
unsigned char uc_andand(expr *n);
unsigned char uc_oror(expr *n);
unsigned char uc_qc(expr *n);

int i_comma(expr *n);
int i_times(expr *n);
int i_divide(expr *n);
int i_mod(expr *n);
int i_plus(expr *n);
int i_minue(expr *n);
int i_rshift(expr *n);
int i_lshift(expr *n);
int i_lt(expr *n);
int i_gt(expr *n);
int i_ge(expr *n);
int i_le(expr *n);
int i_eq(expr *n);
int i_ne(expr *n);
int i_and(expr *n);
int i_xor(expr *n);
int i_or(expr *n);
int i_andand(expr *n);
int i_oror(expr *n);
int i_qc(expr *n);

unsigned int ui_comma(expr *n);
unsigned int ui_times(expr *n);
unsigned int ui_divide(expr *n);
unsigned int ui_mod(expr *n);
unsigned int ui_plus(expr *n);
unsigned int ui_minue(expr *n);
unsigned int ui_rshift(expr *n);
unsigned int ui_lshift(expr *n);
unsigned int ui_lt(expr *n);
unsigned int ui_gt(expr *n);
unsigned int ui_ge(expr *n);
unsigned int ui_le(expr *n);
unsigned int ui_eq(expr *n);
unsigned int ui_ne(expr *n);
unsigned int ui_and(expr *n);
unsigned int ui_xor(expr *n);
unsigned int ui_or(expr *n);
unsigned int ui_andand(expr *n);
unsigned int ui_oror(expr *n);
unsigned int ui_qc(expr *n);

short s_comma(expr *n);
short s_times(expr *n);
short s_divide(expr *n);
short s_mod(expr *n);
short s_plus(expr *n);
short s_minue(expr *n);
short s_rshift(expr *n);
short s_lshift(expr *n);
short s_lt(expr *n);
short s_gt(expr *n);
short s_ge(expr *n);
short s_le(expr *n);
short s_eq(expr *n);
short s_ne(expr *n);
short s_and(expr *n);
short s_xor(expr *n);
short s_or(expr *n);
short s_andand(expr *n);
short s_oror(expr *n);
short s_qc(expr *n);

unsigned short us_comma(expr *n);
unsigned short us_times(expr *n);
unsigned short us_divide(expr *n);
unsigned short us_mod(expr *n);
unsigned short us_plus(expr *n);
unsigned short us_minue(expr *n);
unsigned short us_rshift(expr *n);
unsigned short us_lshift(expr *n);
unsigned short us_lt(expr *n);
unsigned short us_gt(expr *n);
unsigned short us_ge(expr *n);
unsigned short us_le(expr *n);
unsigned short us_eq(expr *n);
unsigned short us_ne(expr *n);
unsigned short us_and(expr *n);
unsigned short us_xor(expr *n);
unsigned short us_or(expr *n);
unsigned short us_andand(expr *n);
unsigned short us_oror(expr *n);
unsigned short us_qc(expr *n);

long l_comma(expr *n);
long l_times(expr *n);
long l_divide(expr *n);
long l_mod(expr *n);
long l_plus(expr *n);
long l_minue(expr *n);
long l_rshift(expr *n);
long l_lshift(expr *n);
long l_lt(expr *n);
long l_gt(expr *n);
long l_ge(expr *n);
long l_le(expr *n);
long l_eq(expr *n);
long l_ne(expr *n);
long l_and(expr *n);
long l_xor(expr *n);
long l_or(expr *n);
long l_andand(expr *n);
long l_oror(expr *n);
long l_qc(expr *n);

unsigned long ul_comma(expr *n);
unsigned long ul_times(expr *n);
unsigned long ul_divide(expr *n);
unsigned long ul_mod(expr *n);
unsigned long ul_plus(expr *n);
unsigned long ul_minue(expr *n);
unsigned long ul_rshift(expr *n);
unsigned long ul_lshift(expr *n);
unsigned long ul_lt(expr *n);
unsigned long ul_gt(expr *n);
unsigned long ul_ge(expr *n);
unsigned long ul_le(expr *n);
unsigned long ul_eq(expr *n);
unsigned long ul_ne(expr *n);
unsigned long ul_and(expr *n);
unsigned long ul_xor(expr *n);
unsigned long ul_or(expr *n);
unsigned long ul_andand(expr *n);
unsigned long ul_oror(expr *n);
unsigned long ul_qc(expr *n);

float f_comma(expr *n);
float f_times(expr *n);
float f_divide(expr *n);
float f_plus(expr *n);
float f_minue(expr *n);
float f_lt(expr *n);
float f_gt(expr *n);
float f_ge(expr *n);
float f_le(expr *n);
float f_eq(expr *n);
float f_ne(expr *n);
float f_andand(expr *n);
float f_oror(expr *n);
float f_qc(expr *n);

double d_comma(expr *n);
double d_times(expr *n);
double d_divide(expr *n);
double d_plus(expr *n);
double d_minue(expr *n);
double d_lt(expr *n);
double d_gt(expr *n);
double d_ge(expr *n);
double d_le(expr *n);
double d_eq(expr *n);
double d_ne(expr *n);
double d_andand(expr *n);
double d_oror(expr *n);
double d_qc(expr *n);

st st_comma(expr *n);
st st_qc(expr *n);

signed char sc__sc(expr *n);
signed char sc__uc(expr *n);
signed char sc__i(expr *n);
signed char sc__ui(expr *n);
signed char sc__s(expr *n);
signed char sc__us(expr *n);
signed char sc__l(expr *n);
signed char sc__ul(expr *n);
signed char sc__f(expr *n);
signed char sc__d(expr *n);

unsigned char uc__sc(expr *n);
unsigned char uc__uc(expr *n);
unsigned char uc__i(expr *n);
unsigned char uc__ui(expr *n);
unsigned char uc__s(expr *n);
unsigned char uc__us(expr *n);
unsigned char uc__l(expr *n);
unsigned char uc__ul(expr *n);
unsigned char uc__f(expr *n);
unsigned char uc__d(expr *n);

int i__sc(expr *n);
int i__uc(expr *n);
int i__i(expr *n);
int i__ui(expr *n);
int i__s(expr *n);
int i__us(expr *n);
int i__l(expr *n);
int i__ul(expr *n);
int i__f(expr *n);
int i__d(expr *n);

unsigned int ui__sc(expr *n);
unsigned int ui__uc(expr *n);
unsigned int ui__i(expr *n);
unsigned int ui__ui(expr *n);
unsigned int ui__s(expr *n);
unsigned int ui__us(expr *n);
unsigned int ui__l(expr *n);
unsigned int ui__ul(expr *n);
unsigned int ui__f(expr *n);
unsigned int ui__d(expr *n);

short s__sc(expr *n);
short s__uc(expr *n);
short s__i(expr *n);
short s__ui(expr *n);
short s__s(expr *n);
short s__us(expr *n);
short s__l(expr *n);
short s__ul(expr *n);
short s__f(expr *n);
short s__d(expr *n);

unsigned short us__sc(expr *n);
unsigned short us__uc(expr *n);
unsigned short us__i(expr *n);
unsigned short us__ui(expr *n);
unsigned short us__s(expr *n);
unsigned short us__us(expr *n);
unsigned short us__l(expr *n);
unsigned short us__ul(expr *n);
unsigned short us__f(expr *n);
unsigned short us__d(expr *n);

long l__sc(expr *n);
long l__uc(expr *n);
long l__i(expr *n);
long l__ui(expr *n);
long l__s(expr *n);
long l__us(expr *n);
long l__l(expr *n);
long l__ul(expr *n);
long l__f(expr *n);
long l__d(expr *n);

unsigned long ul__sc(expr *n);
unsigned long ul__uc(expr *n);
unsigned long ul__i(expr *n);
unsigned long ul__ui(expr *n);
unsigned long ul__s(expr *n);
unsigned long ul__us(expr *n);
unsigned long ul__l(expr *n);
unsigned long ul__ul(expr *n);
unsigned long ul__f(expr *n);
unsigned long ul__d(expr *n);

float f__sc(expr *n);
float f__uc(expr *n);
float f__i(expr *n);
float f__ui(expr *n);
float f__s(expr *n);
float f__us(expr *n);
float f__l(expr *n);
float f__ul(expr *n);
float f__f(expr *n);
float f__d(expr *n);

double d__sc(expr *n);
double d__uc(expr *n);
double d__i(expr *n);
double d__ui(expr *n);
double d__s(expr *n);
double d__us(expr *n);
double d__l(expr *n);
double d__ul(expr *n);
double d__f(expr *n);
double d__d(expr *n);

/* simple constant leaf nodes */

signed char sc_leaf(expr *n);
unsigned char uc_leaf(expr *n);
int i_leaf(expr *n);
unsigned int ui_leaf(expr *n);
short s_leaf(expr *n);
unsigned short us_leaf(expr *n);
long l_leaf(expr *n);
unsigned long ul_leaf(expr *n);
float f_leaf(expr *n);
double d_leaf(expr *n);
st st_leaf(expr *n);

/* global var address nodes */
signed char *sc_gaddr(expr *n);
unsigned char *uc_gaddr(expr *n);
int *i_gaddr(expr *n);
unsigned int *ui_gaddr(expr *n);
short *s_gaddr(expr *n);
unsigned short *us_gaddr(expr *n);
long *l_gaddr(expr *n);
unsigned long *ul_gaddr(expr *n);
float *f_gaddr(expr *n);
double *d_gaddr(expr *n);
st *st_gaddr(expr *n);

/* local var address nodes */
signed char *sc_laddr(expr *n);
unsigned char *uc_laddr(expr *n);
int *i_laddr(expr *n);
unsigned int *ui_laddr(expr *n);
short *s_laddr(expr *n);
unsigned short *us_laddr(expr *n);
long *l_laddr(expr *n);
unsigned long *ul_laddr(expr *n);
float *f_laddr(expr *n);
double *d_laddr(expr *n);
st *st_laddr(expr *n);

/* Routines which take an lvalue into a pvalue */
signed char sc_l2p(expr *n);
unsigned char uc_l2p(expr *n);
int i_l2p(expr *n);
unsigned int ui_l2p(expr *n);
short s_l2p(expr *n);
unsigned short us_l2p(expr *n);
long l_l2p(expr *n);
unsigned long ul_l2p(expr *n);
float f_l2p(expr *n);
double d_l2p(expr *n);
st st_l2p(expr *n);

/* Routines to call when an improper expression is found */
signed char sc_null(expr *n);
unsigned char uc_null(expr *n);
int i_null(expr *n);
unsigned int ui_null(expr *n);
short s_null(expr *n);
unsigned short us_null(expr *n);
long l_null(expr *n);
unsigned long ul_null(expr *n);
float f_null(expr *n);
double d_null(expr *n);
st st_null(expr *n);
I 2

signed char *sc_v2f(expr *n);
unsigned char *uc_v2f(expr *n);
int *i_v2f(expr *n);
unsigned int *ui_v2f(expr *n);
short *s_v2f(expr *n);
unsigned short *us_v2f(expr *n);
long *l_v2f(expr *n);
unsigned long *ul_v2f(expr *n);
float *f_v2f(expr *n);
double *d_v2f(expr *n);
st *st_v2f(expr *n);

signed char *sc_f2v(expr *n);
unsigned char *uc_f2v(expr *n);
int *i_f2v(expr *n);
unsigned int *ui_f2v(expr *n);
short *s_f2v(expr *n);
unsigned short *us_f2v(expr *n);
long *l_f2v(expr *n);
unsigned long *ul_f2v(expr *n);
float *f_f2v(expr *n);
double *d_f2v(expr *n);
st *st_f2v(expr *n);
E 3
I 3
D 4
unsigned int get_field(void *p, int offset, int size);
void set_field(void *p, int offset, int size, unsigned int val);
E 4
I 4
ularge_t get_field(void *p, int offset, int size);
void set_field(void *p, int offset, int size, ularge_t val);
E 4
D 6
void print_out(typeptr tptr, char *addr, int offset, int size, int indent,
	       char *name);
E 6
I 6

#endif /* __BASE_EXPR_H */
E 6
E 3
E 2
E 1
