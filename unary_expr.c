static char sccs_id[] = "@(#)unary_expr.c	1.5";

#include <stdio.h>
#include "dex.h"
#include "map.h"
#include "sym.h"
#include "tree.h"
#include "unary_expr.h"
#include "base_expr.h"
#include "fcall.h"

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

float *f_gaddr(expr *n)
{
    return (float *)n->e_addr;
}

double *d_gaddr(expr *n)
{
    return (double *)n->e_addr;
}

st *st_gaddr(expr *n)
{
    return (st *)n->e_addr;
}

/* local var address nodes */

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
    if (n->e_bsize)
	return (signed char)get_field(v2f(sc_addr(n->e_left)),
			 n->e_boffset, n->e_bsize);
    else
	return *v2f_type(signed char *, sc_addr(n->e_left));
}

unsigned char uc_l2p(expr *n)
{
    if (n->e_bsize)
	return (unsigned char)get_field(v2f(uc_addr(n->e_left)),
			 n->e_boffset, n->e_bsize);
    else
	return *v2f_type(unsigned char *, uc_addr(n->e_left));
}

int i_l2p(expr *n)
{
    if (n->e_bsize)
	return (int)get_field(v2f(i_addr(n->e_left)),
			 n->e_boffset, n->e_bsize);
    else
	return *v2f_type(int *, i_addr(n->e_left));
}

unsigned int ui_l2p(expr *n)
{
    if (n->e_bsize)
	return (unsigned int)get_field(v2f(ui_addr(n->e_left)),
			 n->e_boffset, n->e_bsize);
    else
	return *v2f_type(unsigned int *, ui_addr(n->e_left));
}

short s_l2p(expr *n)
{
    if (n->e_bsize)
	return (short)get_field(v2f(s_addr(n->e_left)),
			 n->e_boffset, n->e_bsize);
    else
	return *v2f_type(short *, s_addr(n->e_left));
}

unsigned short us_l2p(expr *n)
{
    if (n->e_bsize)
	return (unsigned short)get_field(v2f(us_addr(n->e_left)),
			 n->e_boffset, n->e_bsize);
    else
	return *v2f_type(unsigned short *, us_addr(n->e_left));
}

long l_l2p(expr *n)
{
    if (n->e_bsize)
	return (long)get_field(v2f(l_addr(n->e_left)),
			 n->e_boffset, n->e_bsize);
    else if (n->e_size == sizeof(ularge_t))
	return *v2f_type(large_t *, ul_addr(n->e_left));
    else
	return *v2f_type(long *, l_addr(n->e_left));
}

unsigned long ul_l2p(expr *n)
{
    if (n->e_bsize)
	return (unsigned long)get_field(v2f(ul_addr(n->e_left)),
					n->e_boffset, n->e_bsize);
    if (n->e_size == sizeof(ularge_t)) {
	PRINTF("hey %08x\n", n);
	return *v2f_type(ularge_t *, ul_addr(n->e_left));
    }
    return *v2f_type(unsigned long *, ul_addr(n->e_left));
}

float f_l2p(expr *n)
{
    return *v2f_type(float *, f_addr(n->e_left));
}

double d_l2p(expr *n)
{
    return *v2f_type(double *, d_addr(n->e_left));
}

st st_l2p(expr *n)
{
    return (st)st_addr(n->e_left);
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

#ifdef Not_Used
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
#endif

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
int f_lnot(expr *n) { return !f_val(n->e_left); }
float f_umin(expr *n) { return -f_val(n->e_left); }
int d_lnot(expr *n) { return !d_val(n->e_left); }
double d_umin(expr *n) { return -d_val(n->e_left); }
