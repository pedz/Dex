/* @(#)unary_expr.h	1.1 */

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
int f_lnot(expr *n);
float f_umin(expr *n);
int d_lnot(expr *n);
double d_umin(expr *n);