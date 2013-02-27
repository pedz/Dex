h28877
s 00005/00005/00113
d D 1.5 10/08/23 17:01:54 pedzan 5 4
e
s 00021/00026/00097
d D 1.4 02/03/14 16:12:05 pedz 4 3
e
s 00000/00000/00123
d D 1.3 00/09/18 17:57:04 pedz 3 2
c Checking before V5 conversion
e
s 00002/00000/00121
d D 1.2 00/05/29 15:09:09 pedz 2 1
c Just before IA64 conversion
e
s 00121/00000/00000
d D 1.1 95/02/09 21:40:28 pedz 1 0
c date and time created 95/02/09 21:40:28 by pedz
e
u
U
t
T
I 1
/* %W% */

I 4
#ifndef __UNARY_EXPR_H
#define __UNARY_EXPR_H

E 4
/* simple constant leaf nodes */

signed char sc_leaf(expr *n);
unsigned char uc_leaf(expr *n);
int i_leaf(expr *n);
unsigned int ui_leaf(expr *n);
short s_leaf(expr *n);
unsigned short us_leaf(expr *n);
long l_leaf(expr *n);
unsigned long ul_leaf(expr *n);
I 4
long long ll_leaf(expr *n);
unsigned long long ull_leaf(expr *n);
E 4
float f_leaf(expr *n);
double d_leaf(expr *n);
D 5
st st_leaf(expr *n);
E 5
I 5
st_t st_leaf(expr *n);
E 5

/* global var address nodes */
signed char *sc_gaddr(expr *n);
unsigned char *uc_gaddr(expr *n);
int *i_gaddr(expr *n);
unsigned int *ui_gaddr(expr *n);
short *s_gaddr(expr *n);
unsigned short *us_gaddr(expr *n);
long *l_gaddr(expr *n);
unsigned long *ul_gaddr(expr *n);
I 4
long long *ll_gaddr(expr *n);
unsigned long long *ull_gaddr(expr *n);
E 4
float *f_gaddr(expr *n);
double *d_gaddr(expr *n);
D 5
st *st_gaddr(expr *n);
E 5
I 5
st_t *st_gaddr(expr *n);
E 5

/* local var address nodes */
signed char *sc_laddr(expr *n);
unsigned char *uc_laddr(expr *n);
int *i_laddr(expr *n);
unsigned int *ui_laddr(expr *n);
short *s_laddr(expr *n);
unsigned short *us_laddr(expr *n);
long *l_laddr(expr *n);
unsigned long *ul_laddr(expr *n);
I 4
long long *ll_laddr(expr *n);
unsigned long long *ull_laddr(expr *n);
E 4
float *f_laddr(expr *n);
double *d_laddr(expr *n);
D 5
st *st_laddr(expr *n);
E 5
I 5
st_t *st_laddr(expr *n);
E 5

/* Routines which take an lvalue into a pvalue */
signed char sc_l2p(expr *n);
unsigned char uc_l2p(expr *n);
int i_l2p(expr *n);
unsigned int ui_l2p(expr *n);
short s_l2p(expr *n);
unsigned short us_l2p(expr *n);
long l_l2p(expr *n);
unsigned long ul_l2p(expr *n);
I 4
long long ll_l2p(expr *n);
unsigned long long ull_l2p(expr *n);
E 4
float f_l2p(expr *n);
double d_l2p(expr *n);
D 5
st st_l2p(expr *n);
E 5
I 5
st_t st_l2p(expr *n);
E 5

/* Routines to call when an improper expression is found */
signed char sc_null(expr *n);
unsigned char uc_null(expr *n);
int i_null(expr *n);
unsigned int ui_null(expr *n);
short s_null(expr *n);
unsigned short us_null(expr *n);
long l_null(expr *n);
unsigned long ul_null(expr *n);
I 4
long long ll_null(expr *n);
unsigned long long ull_null(expr *n);
E 4
float f_null(expr *n);
double d_null(expr *n);
D 5
st st_null(expr *n);
E 5
I 5
st_t st_null(expr *n);
E 5

I 2
D 4
#ifdef Not_Used
E 2
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
I 2
#endif
E 2

E 4
/* Unary math operators */
int sc_lnot(expr *n);
signed char sc_bnot(expr *n);
signed char sc_umin(expr *n);
int uc_lnot(expr *n);
unsigned char uc_bnot(expr *n);
unsigned char uc_umin(expr *n);
int i_lnot(expr *n);
int i_bnot(expr *n);
int i_umin(expr *n);
int ui_lnot(expr *n);
unsigned int ui_bnot(expr *n);
unsigned int ui_umin(expr *n);
int s_lnot(expr *n);
short s_bnot(expr *n);
short s_umin(expr *n);
int us_lnot(expr *n);
unsigned short us_bnot(expr *n);
unsigned short us_umin(expr *n);
int l_lnot(expr *n);
long l_bnot(expr *n);
long l_umin(expr *n);
int ul_lnot(expr *n);
unsigned long ul_bnot(expr *n);
unsigned long ul_umin(expr *n);
I 4
int ll_lnot(expr *n);
long long ll_bnot(expr *n);
long long ll_umin(expr *n);
int ull_lnot(expr *n);
unsigned long long ull_bnot(expr *n);
unsigned long long ull_umin(expr *n);
E 4
int f_lnot(expr *n);
float f_umin(expr *n);
int d_lnot(expr *n);
double d_umin(expr *n);
I 4

#endif /* __UNARY_EXPR_H */
E 4
E 1
