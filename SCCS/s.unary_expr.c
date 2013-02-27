h60657
s 00012/00011/00312
d D 1.7 10/08/23 17:01:54 pedzan 7 6
e
s 00068/00129/00255
d D 1.6 02/03/14 16:12:35 pedz 6 5
e
s 00000/00000/00384
d D 1.5 00/09/18 17:57:04 pedz 5 4
c Checking before V5 conversion
e
s 00003/00001/00381
d D 1.4 00/05/29 15:09:09 pedz 4 3
c Just before IA64 conversion
e
s 00005/00004/00377
d D 1.3 00/02/08 17:36:49 pedz 3 2
e
s 00006/00000/00375
d D 1.2 98/10/23 12:26:14 pedz 2 1
e
s 00375/00000/00000
d D 1.1 95/02/09 21:40:28 pedz 1 0
c date and time created 95/02/09 21:40:28 by pedz
e
u
U
t
T
I 1
static char sccs_id[] = "%W%";

#include <stdio.h>
I 2
#include "dex.h"
E 2
#include "map.h"
#include "sym.h"
#include "tree.h"
#include "unary_expr.h"
I 2
#include "base_expr.h"
I 4
#include "fcall.h"
I 6
#define DEBUG_BIT UNARY_EXPR_C_BIT
E 6
E 4
E 2

/* simple constant leaf nodes */

D 6
signed char sc_leaf(expr *n) { return n->e_c; }
E 6
I 6
signed char sc_leaf(expr *n) { return n->e_sc; }
E 6
unsigned char uc_leaf(expr *n) { return n->e_uc; }
int i_leaf(expr *n) { return n->e_i; }
unsigned int ui_leaf(expr *n) { return n->e_ui; }
short s_leaf(expr *n) { return n->e_s; }
unsigned short us_leaf(expr *n) { return n->e_us; }
long l_leaf(expr *n) { return n->e_l; }
unsigned long ul_leaf(expr *n) { return n->e_ul; }
I 6
long long ll_leaf(expr *n) { return n->e_ll; }
unsigned long long ull_leaf(expr *n) { return n->e_ull; }
E 6
float f_leaf(expr *n) { return n->e_f; }
double d_leaf(expr *n) { return n->e_d; }
D 7
st st_leaf(expr *n) { return n->e_st; }
E 7
I 7
st_t st_leaf(expr *n) { return n->e_st; }
E 7

/* global var address nodes */
signed char *sc_gaddr(expr *n)
{
    return (signed char *)n->e_addr;
}

unsigned char *uc_gaddr(expr *n)
{
    return (unsigned char *)n->e_addr;
}

int *i_gaddr(expr *n)
{
    return (int *)n->e_addr;
}

unsigned int *ui_gaddr(expr *n)
{
    return (unsigned int *)n->e_addr;
}

short *s_gaddr(expr *n)
{
    return (short *)n->e_addr;
}

unsigned short *us_gaddr(expr *n)
{
    return (unsigned short *)n->e_addr;
}

long *l_gaddr(expr *n)
{
    return (long *)n->e_addr;
}

unsigned long *ul_gaddr(expr *n)
{
    return (unsigned long *)n->e_addr;
}

I 6
long long *ll_gaddr(expr *n)
{
    return (long long *)n->e_addr;
}

unsigned long long *ull_gaddr(expr *n)
{
    return (unsigned long long *)n->e_addr;
}

E 6
float *f_gaddr(expr *n)
{
    return (float *)n->e_addr;
}

double *d_gaddr(expr *n)
{
    return (double *)n->e_addr;
}

D 7
st *st_gaddr(expr *n)
E 7
I 7
st_t *st_gaddr(expr *n)
E 7
{
D 7
    return (st *)n->e_addr;
E 7
I 7
    return (st_t *)n->e_addr;
E 7
}

/* local var address nodes */
D 4
extern int frame_ptr;
E 4

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

I 6
long long *ll_laddr(expr *n)
{
    return (long long *)(n->e_addr + frame_ptr);
}

unsigned long long *ull_laddr(expr *n)
{
    return (unsigned long long *)(n->e_addr + frame_ptr);
}

E 6
float *f_laddr(expr *n)
{
    return (float *)(n->e_addr + frame_ptr);
}

double *d_laddr(expr *n)
{
    return (double *)(n->e_addr + frame_ptr);
}

D 7
st *st_laddr(expr *n)
E 7
I 7
st_t *st_laddr(expr *n)
E 7
{
D 7
    return (st *)(n->e_addr + frame_ptr);
E 7
I 7
    return (st_t *)(n->e_addr + frame_ptr);
E 7
}

/* Routines which take an lvalue into a pvalue */
signed char sc_l2p(expr *n)
{
    if (n->e_bsize)
D 6
	return (signed char)get_field(v2f(sc_addr(n->e_left)),
			 n->e_boffset, n->e_bsize);
E 6
I 6
	return (signed char)get_field(V2F(sc_addr(n->e_left)),
				      n->e_boffset, n->e_bsize);
E 6
    else
	return *v2f_type(signed char *, sc_addr(n->e_left));
}

unsigned char uc_l2p(expr *n)
{
    if (n->e_bsize)
D 6
	return (unsigned char)get_field(v2f(uc_addr(n->e_left)),
			 n->e_boffset, n->e_bsize);
E 6
I 6
	return (unsigned char)get_field(V2F(uc_addr(n->e_left)),
					n->e_boffset, n->e_bsize);
E 6
    else
	return *v2f_type(unsigned char *, uc_addr(n->e_left));
}

int i_l2p(expr *n)
{
    if (n->e_bsize)
D 6
	return (int)get_field(v2f(i_addr(n->e_left)),
			 n->e_boffset, n->e_bsize);
E 6
I 6
	return (int)get_field(V2F(i_addr(n->e_left)),
			      n->e_boffset, n->e_bsize);
E 6
    else
	return *v2f_type(int *, i_addr(n->e_left));
}

unsigned int ui_l2p(expr *n)
{
    if (n->e_bsize)
D 6
	return (unsigned int)get_field(v2f(ui_addr(n->e_left)),
			 n->e_boffset, n->e_bsize);
E 6
I 6
	return (unsigned int)get_field(V2F(ui_addr(n->e_left)),
				       n->e_boffset, n->e_bsize);
E 6
    else
	return *v2f_type(unsigned int *, ui_addr(n->e_left));
}

short s_l2p(expr *n)
{
    if (n->e_bsize)
D 6
	return (short)get_field(v2f(s_addr(n->e_left)),
			 n->e_boffset, n->e_bsize);
E 6
I 6
	return (short)get_field(V2F(s_addr(n->e_left)),
				n->e_boffset, n->e_bsize);
E 6
    else
	return *v2f_type(short *, s_addr(n->e_left));
}

unsigned short us_l2p(expr *n)
{
    if (n->e_bsize)
D 6
	return (unsigned short)get_field(v2f(us_addr(n->e_left)),
			 n->e_boffset, n->e_bsize);
E 6
I 6
	return (unsigned short)get_field(V2F(us_addr(n->e_left)),
					 n->e_boffset, n->e_bsize);
E 6
    else
	return *v2f_type(unsigned short *, us_addr(n->e_left));
}

long l_l2p(expr *n)
{
    if (n->e_bsize)
D 6
	return (long)get_field(v2f(l_addr(n->e_left)),
			 n->e_boffset, n->e_bsize);
E 6
I 6
	return (long)get_field(V2F(l_addr(n->e_left)),
			       n->e_boffset, n->e_bsize);
E 6
I 2
    else if (n->e_size == sizeof(ularge_t))
	return *v2f_type(large_t *, ul_addr(n->e_left));
E 2
    else
	return *v2f_type(long *, l_addr(n->e_left));
}

unsigned long ul_l2p(expr *n)
{
    if (n->e_bsize)
D 6
	return (unsigned long)get_field(v2f(ul_addr(n->e_left)),
E 6
I 6
	return (unsigned long)get_field(V2F(ul_addr(n->e_left)),
E 6
D 3
			 n->e_boffset, n->e_bsize);
I 2
    else if (n->e_size == sizeof(ularge_t))
E 3
I 3
					n->e_boffset, n->e_bsize);
    if (n->e_size == sizeof(ularge_t)) {
D 6
	PRINTF("hey %08x\n", n);
E 6
I 6
D 7
	DEBUG_PRINTF(("hey %s\n", P(n)));
E 6
E 3
	return *v2f_type(ularge_t *, ul_addr(n->e_left));
E 7
I 7
	ularge_t *temp = v2f_type(ularge_t *, ul_addr(n->e_left));
	DEBUG_PRINTF(("hey1 %s %lx %lx %lx\n", P(n), ul_addr(n->e_left), temp, *temp));
	return *temp;
E 7
E 2
D 3
    else
	return *v2f_type(unsigned long *, ul_addr(n->e_left));
E 3
I 3
    }
    return *v2f_type(unsigned long *, ul_addr(n->e_left));
E 3
}

I 6
long long ll_l2p(expr *n)
{
    if (n->e_bsize)
	return (long)get_field(V2F(ll_addr(n->e_left)),
			       n->e_boffset, n->e_bsize);
    return *v2f_type(long *, ll_addr(n->e_left));
}

unsigned long long ull_l2p(expr *n)
{
    if (n->e_bsize)
	return (unsigned long)get_field(V2F(ull_addr(n->e_left)),
					n->e_boffset, n->e_bsize);
    if (n->e_size == sizeof(ularge_t)) {
D 7
	DEBUG_PRINTF(("hey %s\n", P(n)));
E 7
I 7
	DEBUG_PRINTF(("hey2 %s\n", P(n)));
E 7
	return *v2f_type(ularge_t *, ull_addr(n->e_left));
    }
    return *v2f_type(unsigned long *, ull_addr(n->e_left));
}

E 6
float f_l2p(expr *n)
{
    return *v2f_type(float *, f_addr(n->e_left));
}

double d_l2p(expr *n)
{
    return *v2f_type(double *, d_addr(n->e_left));
}

D 7
st st_l2p(expr *n)
E 7
I 7
st_t st_l2p(expr *n)
E 7
{
D 7
    return (st)st_addr(n->e_left);
E 7
I 7
    return (st_t)st_addr(n->e_left);
E 7
}


signed char sc_null(expr *n) { return 0; }
unsigned char uc_null(expr *n) { return 0; }
int i_null(expr *n) { return 0; }
unsigned int ui_null(expr *n) { return 0; }
short s_null(expr *n) { return 0; }
unsigned short us_null(expr *n) { return 0; }
long l_null(expr *n) { return 0; }
unsigned long ul_null(expr *n) { return 0; }
I 6
long long ll_null(expr *n) { return 0; }
unsigned long long ull_null(expr *n) { return 0; }
E 6
float f_null(expr *n) { return 0; }
double d_null(expr *n) { return 0; }
D 7
st st_null(expr *n) { static st s; return s; }
E 7
I 7
st_t st_null(expr *n) { static st_t s; return s; }
E 7

I 4
D 6
#ifdef Not_Used
E 4
signed char *sc_v2f(expr *n)
{
    return v2f_type(signed char *, sc_addr(n->e_left));
}

unsigned char *uc_v2f(expr *n)
{
    return v2f_type(unsigned char *, uc_addr(n->e_left));
}

int *i_v2f(expr *n)
{
    return v2f_type(int *, i_addr(n->e_left));
}

unsigned int *ui_v2f(expr *n)
{
    return v2f_type(unsigned int *, ui_addr(n->e_left));
}

short *s_v2f(expr *n)
{
    return v2f_type(short *, s_addr(n->e_left));
}

unsigned short *us_v2f(expr *n)
{
    return v2f_type(unsigned short *, us_addr(n->e_left));
}

long *l_v2f(expr *n)
{
    return v2f_type(long *, l_addr(n->e_left));
}

unsigned long *ul_v2f(expr *n)
{
    return v2f_type(unsigned long *, ul_addr(n->e_left));
}

float *f_v2f(expr *n)
{
    return v2f_type(float *, f_addr(n->e_left));
}

double *d_v2f(expr *n)
{
    return v2f_type(double *, d_addr(n->e_left));
}

st *st_v2f(expr *n)
{
    return v2f_type(st *, st_addr(n->e_left));
}

signed char *sc_f2v(expr *n)
{
    return f2v_type(signed char *, sc_addr(n->e_left));
}

unsigned char *uc_f2v(expr *n)
{
    return f2v_type(unsigned char *, uc_addr(n->e_left));
}

int *i_f2v(expr *n)
{
    return f2v_type(int *, i_addr(n->e_left));
}

unsigned int *ui_f2v(expr *n)
{
    return f2v_type(unsigned int *, ui_addr(n->e_left));
}

short *s_f2v(expr *n)
{
    return f2v_type(short *, s_addr(n->e_left));
}

unsigned short *us_f2v(expr *n)
{
    return f2v_type(unsigned short *, us_addr(n->e_left));
}

long *l_f2v(expr *n)
{
    return f2v_type(long *, l_addr(n->e_left));
}

unsigned long *ul_f2v(expr *n)
{
    return f2v_type(unsigned long *, ul_addr(n->e_left));
}

float *f_f2v(expr *n)
{
    return f2v_type(float *, f_addr(n->e_left));
}

double *d_f2v(expr *n)
{
    return f2v_type(double *, d_addr(n->e_left));
}

st *st_f2v(expr *n)
{
    return f2v_type(st *, st_addr(n->e_left));
}
I 4
#endif
E 4

E 6
int sc_lnot(expr *n) { return !sc_val(n->e_left); }
signed char sc_bnot(expr *n) { return ~sc_val(n->e_left); }
signed char sc_umin(expr *n) { return -sc_val(n->e_left); }
int uc_lnot(expr *n) { return !uc_val(n->e_left); }
unsigned char uc_bnot(expr *n) { return ~uc_val(n->e_left); }
unsigned char uc_umin(expr *n) { return -uc_val(n->e_left); }
int i_lnot(expr *n) { return !i_val(n->e_left); }
int i_bnot(expr *n) { return ~i_val(n->e_left); }
int i_umin(expr *n) { return -i_val(n->e_left); }
int ui_lnot(expr *n) { return !ui_val(n->e_left); }
unsigned int ui_bnot(expr *n) { return ~ui_val(n->e_left); }
unsigned int ui_umin(expr *n) { return -ui_val(n->e_left); }
int s_lnot(expr *n) { return !s_val(n->e_left); }
short s_bnot(expr *n) { return ~s_val(n->e_left); }
short s_umin(expr *n) { return -s_val(n->e_left); }
int us_lnot(expr *n) { return !us_val(n->e_left); }
unsigned short us_bnot(expr *n) { return ~us_val(n->e_left); }
unsigned short us_umin(expr *n) { return -us_val(n->e_left); }
int l_lnot(expr *n) { return !l_val(n->e_left); }
long l_bnot(expr *n) { return ~l_val(n->e_left); }
long l_umin(expr *n) { return -l_val(n->e_left); }
int ul_lnot(expr *n) { return !ul_val(n->e_left); }
unsigned long ul_bnot(expr *n) { return ~ul_val(n->e_left); }
unsigned long ul_umin(expr *n) { return -ul_val(n->e_left); }
I 6
int ll_lnot(expr *n) { return !ll_val(n->e_left); }
long long ll_bnot(expr *n) { return ~ll_val(n->e_left); }
long long ll_umin(expr *n) { return -ll_val(n->e_left); }
int ull_lnot(expr *n) { return !ull_val(n->e_left); }
unsigned long long ull_bnot(expr *n) { return ~ull_val(n->e_left); }
unsigned long long ull_umin(expr *n) { return -ull_val(n->e_left); }
E 6
int f_lnot(expr *n) { return !f_val(n->e_left); }
float f_umin(expr *n) { return -f_val(n->e_left); }
int d_lnot(expr *n) { return !d_val(n->e_left); }
double d_umin(expr *n) { return -d_val(n->e_left); }
E 1
