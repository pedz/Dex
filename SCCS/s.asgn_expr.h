h37026
s 00001/00001/00164
d D 1.6 10/08/23 17:01:52 pedzan 6 5
e
s 00018/00013/00147
d D 1.5 02/03/14 16:12:30 pedz 5 4
e
s 00026/00000/00134
d D 1.4 01/03/09 21:45:22 pedz 4 3
c Added long long and unsigned long long prototypes
e
s 00000/00000/00134
d D 1.3 00/09/18 17:56:57 pedz 3 2
c Checking before V5 conversion
e
s 00009/00000/00125
d D 1.2 98/10/23 12:26:06 pedz 2 1
e
s 00125/00000/00000
d D 1.1 95/02/01 10:57:35 pedz 1 0
c date and time created 95/02/01 10:57:35 by pedz
e
u
U
t
T
I 1

/* %W% */

I 5
#ifndef __ASGN_EXPR_H
#define __ASGN_EXPR_H

E 5
signed char sc_asgn(expr *n);
unsigned char uc_asgn(expr *n);
int i_asgn(expr *n);
unsigned int ui_asgn(expr *n);
short s_asgn(expr *n);
unsigned short us_asgn(expr *n);
long l_asgn(expr *n);
unsigned long ul_asgn(expr *n);
I 4
long long ll_asgn(expr *n);
D 5
unsigned long ull_asgn(expr *n);
E 5
I 5
unsigned long long ull_asgn(expr *n);
E 5
E 4
float f_asgn(expr *n);
double d_asgn(expr *n);
D 6
st st_asgn(expr *n);
E 6
I 6
st_t st_asgn(expr *n);
E 6

signed char sc_plusasgn(expr *n);
unsigned char uc_plusasgn(expr *n);
int i_plusasgn(expr *n);
unsigned int ui_plusasgn(expr *n);
short s_plusasgn(expr *n);
unsigned short us_plusasgn(expr *n);
long l_plusasgn(expr *n);
unsigned long ul_plusasgn(expr *n);
I 4
long long ll_plusasgn(expr *n);
D 5
unsigned long ull_plusasgn(expr *n);
E 5
I 5
unsigned long long ull_plusasgn(expr *n);
E 5
E 4
float f_plusasgn(expr *n);
double d_plusasgn(expr *n);

signed char sc_minusasgn(expr *n);
unsigned char uc_minusasgn(expr *n);
int i_minusasgn(expr *n);
unsigned int ui_minusasgn(expr *n);
short s_minusasgn(expr *n);
unsigned short us_minusasgn(expr *n);
long l_minusasgn(expr *n);
unsigned long ul_minusasgn(expr *n);
I 4
long long ll_minusasgn(expr *n);
D 5
unsigned long ull_minusasgn(expr *n);
E 5
I 5
unsigned long long ull_minusasgn(expr *n);
E 5
E 4
float f_minusasgn(expr *n);
double d_minusasgn(expr *n);

signed char sc_timesasgn(expr *n);
unsigned char uc_timesasgn(expr *n);
int i_timesasgn(expr *n);
unsigned int ui_timesasgn(expr *n);
short s_timesasgn(expr *n);
unsigned short us_timesasgn(expr *n);
long l_timesasgn(expr *n);
unsigned long ul_timesasgn(expr *n);
I 4
long long ll_timesasgn(expr *n);
D 5
unsigned long ull_timesasgn(expr *n);
E 5
I 5
unsigned long long ull_timesasgn(expr *n);
E 5
E 4
float f_timesasgn(expr *n);
double d_timesasgn(expr *n);

signed char sc_divasgn(expr *n);
unsigned char uc_divasgn(expr *n);
int i_divasgn(expr *n);
unsigned int ui_divasgn(expr *n);
short s_divasgn(expr *n);
unsigned short us_divasgn(expr *n);
long l_divasgn(expr *n);
unsigned long ul_divasgn(expr *n);
I 4
long long ll_divasgn(expr *n);
D 5
unsigned long ull_divasgn(expr *n);
E 5
I 5
unsigned long long ull_divasgn(expr *n);
E 5
E 4
float f_divasgn(expr *n);
double d_divasgn(expr *n);

signed char sc_modasgn(expr *n);
unsigned char uc_modasgn(expr *n);
int i_modasgn(expr *n);
unsigned int ui_modasgn(expr *n);
short s_modasgn(expr *n);
unsigned short us_modasgn(expr *n);
long l_modasgn(expr *n);
unsigned long ul_modasgn(expr *n);
I 4
long long ll_modasgn(expr *n);
D 5
unsigned long ull_modasgn(expr *n);
E 5
I 5
unsigned long long ull_modasgn(expr *n);
E 5
E 4

signed char sc_andasgn(expr *n);
unsigned char uc_andasgn(expr *n);
int i_andasgn(expr *n);
unsigned int ui_andasgn(expr *n);
short s_andasgn(expr *n);
unsigned short us_andasgn(expr *n);
long l_andasgn(expr *n);
unsigned long ul_andasgn(expr *n);
I 4
long long ll_andasgn(expr *n);
D 5
unsigned long ull_andasgn(expr *n);
E 5
I 5
unsigned long long ull_andasgn(expr *n);
E 5
E 4

signed char sc_orasgn(expr *n);
unsigned char uc_orasgn(expr *n);
int i_orasgn(expr *n);
unsigned int ui_orasgn(expr *n);
short s_orasgn(expr *n);
unsigned short us_orasgn(expr *n);
long l_orasgn(expr *n);
unsigned long ul_orasgn(expr *n);
I 4
long long ll_orasgn(expr *n);
D 5
unsigned long ull_orasgn(expr *n);
E 5
I 5
unsigned long long ull_orasgn(expr *n);
E 5
E 4

I 2
signed char sc_xorasgn(expr *n);
unsigned char uc_xorasgn(expr *n);
int i_xorasgn(expr *n);
unsigned int ui_xorasgn(expr *n);
short s_xorasgn(expr *n);
unsigned short us_xorasgn(expr *n);
long l_xorasgn(expr *n);
unsigned long ul_xorasgn(expr *n);
I 4
long long ll_xorasgn(expr *n);
D 5
unsigned long ull_xorasgn(expr *n);
E 5
I 5
unsigned long long ull_xorasgn(expr *n);
E 5
E 4

E 2
signed char sc_lsasgn(expr *n);
unsigned char uc_lsasgn(expr *n);
int i_lsasgn(expr *n);
unsigned int ui_lsasgn(expr *n);
short s_lsasgn(expr *n);
unsigned short us_lsasgn(expr *n);
long l_lsasgn(expr *n);
unsigned long ul_lsasgn(expr *n);
I 4
long long ll_lsasgn(expr *n);
D 5
unsigned long ull_lsasgn(expr *n);
E 5
I 5
unsigned long long ull_lsasgn(expr *n);
E 5
E 4

signed char sc_rsasgn(expr *n);
unsigned char uc_rsasgn(expr *n);
int i_rsasgn(expr *n);
unsigned int ui_rsasgn(expr *n);
short s_rsasgn(expr *n);
unsigned short us_rsasgn(expr *n);
long l_rsasgn(expr *n);
unsigned long ul_rsasgn(expr *n);
I 4
long long ll_rsasgn(expr *n);
D 5
unsigned long ull_rsasgn(expr *n);
E 5
I 5
unsigned long long ull_rsasgn(expr *n);
E 5
E 4

signed char sc_inc(expr *n);
unsigned char uc_inc(expr *n);
int i_inc(expr *n);
unsigned int ui_inc(expr *n);
short s_inc(expr *n);
unsigned short us_inc(expr *n);
long l_inc(expr *n);
unsigned long ul_inc(expr *n);
I 4
long long ll_inc(expr *n);
D 5
unsigned long ull_inc(expr *n);
E 5
I 5
unsigned long long ull_inc(expr *n);
E 5
E 4
float f_inc(expr *n);
double d_inc(expr *n);

signed char sc_dec(expr *n);
unsigned char uc_dec(expr *n);
int i_dec(expr *n);
unsigned int ui_dec(expr *n);
short s_dec(expr *n);
unsigned short us_dec(expr *n);
long l_dec(expr *n);
unsigned long ul_dec(expr *n);
I 4
long long ll_dec(expr *n);
D 5
unsigned long ull_dec(expr *n);
E 5
I 5
unsigned long long ull_dec(expr *n);
E 5
E 4
float f_dec(expr *n);
double d_dec(expr *n);
I 5

#endif /* __ASGN_EXPR_H */
E 5
E 1
