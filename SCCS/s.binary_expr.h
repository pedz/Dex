h35615
s 00002/00002/00272
d D 1.5 10/08/23 17:01:52 pedzan 5 4
e
s 00006/00000/00268
d D 1.4 02/03/14 16:12:31 pedz 4 3
e
s 00046/00000/00222
d D 1.3 01/03/09 21:45:06 pedz 3 2
c Added long long and unsigned long long prototypes
e
s 00000/00000/00222
d D 1.2 00/09/18 17:56:58 pedz 2 1
c Checking before V5 conversion
e
s 00222/00000/00000
d D 1.1 95/02/09 21:40:24 pedz 1 0
c date and time created 95/02/09 21:40:24 by pedz
e
u
U
t
T
I 4

E 4
I 1
/* %W% */

I 4
#ifndef __BINARY_EXPR_H
#define __BINARY_EXPR_H

E 4
signed char sc_comma(expr *n);
signed char sc_times(expr *n);
signed char sc_divide(expr *n);
signed char sc_mod(expr *n);
signed char sc_plus(expr *n);
signed char sc_minue(expr *n);
signed char sc_rshift(expr *n);
signed char sc_lshift(expr *n);

int sc_lt(expr *n);
int sc_gt(expr *n);
int sc_ge(expr *n);
int sc_le(expr *n);
int sc_eq(expr *n);
int sc_ne(expr *n);

signed char sc_and(expr *n);
signed char sc_xor(expr *n);
signed char sc_or(expr *n);
signed char sc_andand(expr *n);
signed char sc_oror(expr *n);
signed char sc_qc(expr *n);

unsigned char uc_comma(expr *n);
unsigned char uc_times(expr *n);
unsigned char uc_divide(expr *n);
unsigned char uc_mod(expr *n);
unsigned char uc_plus(expr *n);
unsigned char uc_minue(expr *n);
unsigned char uc_rshift(expr *n);
unsigned char uc_lshift(expr *n);

int uc_lt(expr *n);
int uc_gt(expr *n);
int uc_ge(expr *n);
int uc_le(expr *n);
int uc_eq(expr *n);
int uc_ne(expr *n);

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

int ui_lt(expr *n);
int ui_gt(expr *n);
int ui_ge(expr *n);
int ui_le(expr *n);
int ui_eq(expr *n);
int ui_ne(expr *n);

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

int s_lt(expr *n);
int s_gt(expr *n);
int s_ge(expr *n);
int s_le(expr *n);
int s_eq(expr *n);
int s_ne(expr *n);

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

int us_lt(expr *n);
int us_gt(expr *n);
int us_ge(expr *n);
int us_le(expr *n);
int us_eq(expr *n);
int us_ne(expr *n);

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

int l_lt(expr *n);
int l_gt(expr *n);
int l_ge(expr *n);
int l_le(expr *n);
int l_eq(expr *n);
int l_ne(expr *n);

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

int ul_lt(expr *n);
int ul_gt(expr *n);
int ul_ge(expr *n);
int ul_le(expr *n);
int ul_eq(expr *n);
int ul_ne(expr *n);

unsigned long ul_and(expr *n);
unsigned long ul_xor(expr *n);
unsigned long ul_or(expr *n);
unsigned long ul_andand(expr *n);
unsigned long ul_oror(expr *n);
unsigned long ul_qc(expr *n);

I 3
long long ll_comma(expr *n);
long long ll_times(expr *n);
long long ll_divide(expr *n);
long long ll_mod(expr *n);
long long ll_plus(expr *n);
long long ll_minue(expr *n);
long long ll_rshift(expr *n);
long long ll_lshift(expr *n);

int ll_lt(expr *n);
int ll_gt(expr *n);
int ll_ge(expr *n);
int ll_le(expr *n);
int ll_eq(expr *n);
int ll_ne(expr *n);

long long ll_and(expr *n);
long long ll_xor(expr *n);
long long ll_or(expr *n);
long long ll_andand(expr *n);
long long ll_oror(expr *n);
long long ll_qc(expr *n);

unsigned long long ull_comma(expr *n);
unsigned long long ull_times(expr *n);
unsigned long long ull_divide(expr *n);
unsigned long long ull_mod(expr *n);
unsigned long long ull_plus(expr *n);
unsigned long long ull_minue(expr *n);
unsigned long long ull_rshift(expr *n);
unsigned long long ull_lshift(expr *n);

int ull_lt(expr *n);
int ull_gt(expr *n);
int ull_ge(expr *n);
int ull_le(expr *n);
int ull_eq(expr *n);
int ull_ne(expr *n);

unsigned long long ull_and(expr *n);
unsigned long long ull_xor(expr *n);
unsigned long long ull_or(expr *n);
unsigned long long ull_andand(expr *n);
unsigned long long ull_oror(expr *n);
unsigned long long ull_qc(expr *n);

E 3
float f_comma(expr *n);
float f_times(expr *n);
float f_divide(expr *n);
float f_plus(expr *n);
float f_minue(expr *n);

int f_lt(expr *n);
int f_gt(expr *n);
int f_ge(expr *n);
int f_le(expr *n);
int f_eq(expr *n);
int f_ne(expr *n);

float f_andand(expr *n);
float f_oror(expr *n);
float f_qc(expr *n);

double d_comma(expr *n);
double d_times(expr *n);
double d_divide(expr *n);
double d_plus(expr *n);
double d_minue(expr *n);

int d_lt(expr *n);
int d_gt(expr *n);
int d_ge(expr *n);
int d_le(expr *n);
int d_eq(expr *n);
int d_ne(expr *n);

double d_andand(expr *n);
double d_oror(expr *n);
double d_qc(expr *n);

D 5
st st_comma(expr *n);
st st_qc(expr *n);
E 5
I 5
st_t st_comma(expr *n);
st_t st_qc(expr *n);
E 5
I 4

#endif /* __BINARY_EXPR_H */
E 4
E 1
