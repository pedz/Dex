h18720
s 00015/00003/00058
d D 1.9 10/08/23 17:10:03 pedzan 9 8
e
s 00028/00025/00033
d D 1.8 02/03/14 16:12:30 pedz 8 7
e
s 00000/00000/00058
d D 1.7 00/09/18 17:56:58 pedz 7 6
c Checking before V5 conversion
e
s 00041/00300/00017
d D 1.6 98/10/23 12:26:06 pedz 6 5
e
s 00021/00008/00296
d D 1.5 97/05/13 16:02:55 pedz 5 4
c Check pointer
e
s 00003/00003/00301
d D 1.4 95/04/25 10:09:52 pedz 4 3
e
s 00259/00649/00045
d D 1.3 95/02/09 21:35:36 pedz 3 2
e
s 00187/00056/00507
d D 1.2 95/02/01 10:36:43 pedz 2 1
e
s 00563/00000/00000
d D 1.1 94/08/22 17:56:32 pedz 1 0
c date and time created 94/08/22 17:56:32 by pedz
e
u
U
t
T
I 1
static char sccs_id[] = "%W%";

I 3
#include <stdio.h>
#include <setjmp.h>
I 8
#include <stdlib.h>
E 8
I 6
#include "dex.h"
E 6
E 3
#include "map.h"
#include "sym.h"
#include "tree.h"
I 3
#include "base_expr.h"
E 3

I 8
#define DEBUG_BIT BASE_EXPR_C_BIT

E 8
D 3
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
E 3
I 3
D 6
#define WSIZE (8 * sizeof(unsigned int))
static void print_name(char *name, typeptr tptr);
E 6
I 6
#define WSIZE (8 * sizeof(ularge_t))
E 6
E 3

D 3
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
E 3
I 3
D 6
unsigned int get_field(void *addr, int offset, int size)
E 6
I 6
ularge_t get_field(void *addr, int offset, int size)
E 6
E 3
{
D 3
    return i_val(n->e_left) ?
	sc_val(n->e_right->e_left) :
	    sc_val(n->e_right->e_right);
}
E 3
I 3
D 4
    long laddr = (long)addr;
E 4
I 4
D 6
    unsigned int *laddr = addr;
E 4
    unsigned int temp;
E 6
I 6
  ularge_t *laddr = addr;
  ularge_t temp;
I 9
  int orig_offset = offset;
E 9
E 6
E 3

I 9
  DEBUG_PRINTF(("get field addr: %0*lx, offset: %d, size: %d\n",
		sizeof(addr)*2, addr, offset, size));
  DEBUG_PRINTF(("WSIZE = %d, sizeof(ularge_t) = %d\n", WSIZE, sizeof(ularge_t)));
E 9
D 3
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
E 3
I 3
D 4
    laddr += (offset / 8);
E 4
I 4
D 6
    laddr += (offset / WSIZE);
E 4
    offset %= WSIZE;
    if (offset + size > WSIZE) {
D 5
	fprintf(stderr, "getval spans an integer boundry\n");
E 5
I 5
	fprintf(stderr, "get_field spans an integer boundry\n");
E 5
	exit(1);
    }
D 4
    temp = *(unsigned int *)laddr;
E 4
I 4
    temp = *laddr;
E 4
    temp >>= WSIZE - offset - size;
    temp &= (unsigned)-1 >> (WSIZE - size);
    return temp;
E 6
I 6
  laddr += (offset / WSIZE);
  offset %= WSIZE;
D 9
  if (offset + size > WSIZE) {
    fprintf(stderr, "get_field spans alignment boundry\n");
    exit(1);
E 9
I 9
  DEBUG_PRINTF(("get field new laddr: %0*lx, new offset: %d\n",
	       sizeof(addr)*2, laddr, offset));
  /*
   * We cheat and move laddr by single bytes up until the entire value
   * fits into one fetch.
   */
  while (offset + size > WSIZE) {
    laddr = (ularge_t *)(((char *)laddr) + 1);
    offset -= 8;
E 9
  }
I 9
  DEBUG_PRINTF(("get field new laddr: %0*lx, new offset: %d\n",
	       sizeof(addr)*2, laddr, offset));
E 9
  temp = *laddr;
  temp >>= WSIZE - offset - size;
  temp &= (ularge_t)-1 >> (WSIZE - size);
  return temp;
E 6
E 3
}

D 3
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
E 3
I 3
D 6
void set_field(void *addr, int offset, int size, unsigned int val)
E 6
I 6
void set_field(void *addr, int offset, int size, ularge_t val)
E 6
E 3
{
D 3
    return i_val(n->e_left) ?
	i_val(n->e_right->e_left) :
	    i_val(n->e_right->e_right);
}
E 3
I 3
D 6
    long laddr = (long)addr;
    unsigned int temp;
E 6
I 6
D 8
  ularge_t *laddr = addr;
  ularge_t temp;
  ularge_t mask;
E 8
I 8
    ularge_t *laddr = addr;
    ularge_t temp;
    ularge_t mask;
E 8
E 6
E 3

D 3
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
E 3
I 3
D 6
    laddr += (offset / 8);
    offset %= WSIZE;
    if (offset + size > WSIZE) {
D 5
	fprintf(stderr, "getval spans an integer boundry\n");
E 5
I 5
	fprintf(stderr, "set_field spans an integer boundry\n");
E 5
	exit(1);
    }
    temp = *(unsigned int *)laddr;
    temp &= (((unsigned int)-1 >> (WSIZE - size)) << (WSIZE - offset - size));
    temp |= ((val & ((unsigned int)-1 >> (WSIZE - size))) <<
	     (WSIZE - offset - size));
    *(unsigned int *)laddr = temp;
    return;
E 3
}

D 3
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
E 3
I 3
/*
 * Print out the thing that t points to.  The address is addr, offset is
 * a bit offset from addr (which may be greater than 32), size is the
 * size of the object, indent is the indent level for printouts, name
 * is the name of the field or variable, sname is the possible name of
 * the structure (e.g. struct toad { ...)
 */
void print_out(typeptr tptr, char *addr, int offset, int size, int indent,
	       char *name)
E 3
{
D 3
    return i_val(n->e_left) ?
	s_val(n->e_right->e_left) :
	    s_val(n->e_right->e_right);
}
E 3
I 3
    fieldptr f;
    typeptr ltptr, rtptr;
    attrptr lattr;
    int val;
D 5
    int index, lower, upper, item_size, range;
E 5
I 5
    int index, lower, upper, item_size, range, width;
E 5
    char *e_val;
    enumptr eptr;
    ns *nspace = tptr->t_ns;
    double d;
E 3

D 3
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
E 3
I 3
    if (!size)
	size = get_size(tptr);
E 3

D 3
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
E 3
I 3
    switch (tptr->t_type) {
    case UNION_TYPE:
    case STRUCT_TYPE:
	printf("%*s", indent, "");
	print_name("", tptr);
	printf("{\n");
	
	for (f = tptr->t_val.val_s.s_fields; f; f = f->f_next)
	    print_out(f->f_typeptr, addr, offset + f->f_offset,
		      f->f_numbits, indent + 2, f->f_name);
E 3

D 3
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
E 3
I 3
	if (name)
	    printf("%*s} %s;\n", indent, "", name);
	else
	    printf("%*s};", indent, "");
	break;
E 3

D 3
float f_comma(expr *n)  { return f_val(n->e_left) ,  f_val(n->e_right); }
float f_times(expr *n)  { return f_val(n->e_left) *  f_val(n->e_right); }
float f_divide(expr *n) { return f_val(n->e_left) /  f_val(n->e_right); }
float f_plus(expr *n)   { return f_val(n->e_left) +  f_val(n->e_right); }
float f_minue(expr *n)  { return f_val(n->e_left) -  f_val(n->e_right); }
E 3
I 3
    case PTR_TYPE: /* pointer to some type but who cares what type it is */
	if (size != 32) {
	    fprintf(stderr, "size of pointer not 32 bits for %s\n", name);
	    exit(1);
	}
	item_size = get_size(tptr->t_val.val_p);
E 3

D 3
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
E 3
I 3
	val = get_field(addr, offset, size);
	printf("%*s", indent, "");
	print_name(name, tptr);
	/*
	 * Special case for non-null pointers to char
	 */
	if (val && item_size == 8) {
	    char buf[32];
	    int i;
	    volatile int had_fault;
E 3

D 3
double d_comma(expr *n)  { return d_val(n->e_left) ,  d_val(n->e_right); }
double d_times(expr *n)  { return d_val(n->e_left) *  d_val(n->e_right); }
double d_divide(expr *n) { return d_val(n->e_left) /  d_val(n->e_right); }
double d_plus(expr *n)   { return d_val(n->e_left) +  d_val(n->e_right); }
double d_minue(expr *n)  { return d_val(n->e_left) -  d_val(n->e_right); }
E 3
I 3
	    BEGIN_PROTECT(&had_fault);
	    bcopy(v2f(val), buf, sizeof(buf));
	    for (i = 0; i < sizeof(buf); ++i)
		if (buf[i] < ' ' || buf[i] > 126)
		    break;
	    if (i == sizeof(buf) || buf[i] == '\0') {
		printf(" = 0x%08x => \"%.*s\"\n", val, sizeof(buf), buf);
		EXIT_PROTECT(break);
	    }
	    END_PROTECT();
	}
	printf(" = 0x%08x\n", val);
	break;
E 3

D 3
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
E 3
I 3
    case ARRAY_TYPE:
	ltptr = tptr->t_val.val_a.a_typeptr;
	if ((rtptr = tptr->t_val.val_a.a_typedef)->t_type != RANGE_TYPE) {
	    printf("bogus typedef for array for %s\n", name);
	    return;
	}
	lower = rtptr->t_val.val_r.r_lower;
	upper = rtptr->t_val.val_r.r_upper;
	if (!lower && !upper) {		/* 0 size array probably from & op */
	    printf("%*s", indent, "");
	    print_name(name, tptr);
	    printf(" = 0x%08x\n", f2v(addr));
	    break;
	}
	range = upper - lower + 1;
	item_size = size / range;	/* first guess */
E 3

D 3
st st_comma(expr *n)  { return st_val(n->e_left) ,  st_val(n->e_right); }
st st_qc(expr *n)
{
    return i_val(n->e_left) ?
	st_val(n->e_right->e_left) :
	    st_val(n->e_right->e_right);
}
E 3
I 3
	for (lattr = ltptr->t_attrs; lattr; lattr = lattr->a_next)
	    if (lattr->a_type == SIZE_ATTR) {
		item_size = lattr->a_val;
		break;
	    }
	if (item_size * range != size) {
	    printf("bogus sizes, item_size = %d, count = %d, size = %d\n",
		   item_size, range, size);
	    return;
	}
E 3

D 2
signed char sc__sc(expr *n) { return sc_val(n->e_left);  }
E 2
I 2
D 3
signed char sc__sc(expr *n) { return sc_val(n->e_left); }
E 2
signed char sc__uc(expr *n) { return uc_val(n->e_left); }
signed char sc__i(expr *n)  { return i_val(n->e_left);  }
signed char sc__ui(expr *n) { return ui_val(n->e_left); }
signed char sc__s(expr *n)  { return s_val(n->e_left);  }
signed char sc__us(expr *n) { return us_val(n->e_left); }
signed char sc__l(expr *n)  { return l_val(n->e_left);  }
signed char sc__ul(expr *n) { return ul_val(n->e_left); }
signed char sc__f(expr *n)  { return f_val(n->e_left);  }
signed char sc__d(expr *n)  { return d_val(n->e_left);  }
E 3
I 3
	if (item_size == 8) {		/* assume array of char => string */
	    if (offset % 8) {
		printf("bogus offset for string of characters for %s\n", name);
		return;
	    }
	    printf("%*s", indent, "");
	    print_name(name, tptr);
	    printf(" = \"%.*s\"\n", range, addr + (offset / 8));
	    break;
	}
E 3

I 5
	for (index = upper, width = 0; index; ++width, index /= 10);
	if (!width)
	    width = 1;

E 5
D 3
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
E 3
I 3
	for (index = lower; index <= upper; ++index) {
	    char buf[64];
E 3

D 3
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
E 3
I 3
D 5
	    sprintf(buf, "%s[%3i]", name, index);
E 5
I 5
	    sprintf(buf, "%s[%*i]", name, width, index);
E 5
	    print_out(ltptr, addr, offset, item_size, indent, buf);
	    offset += item_size;
	}
	break;
E 3

D 3
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
E 3
I 3
    case RANGE_TYPE:
I 5
	printf("%*s", indent, "");
	print_name(name, tptr);
	printf(" = ");

	while (size > 8 * sizeof(long)) {
	    val = get_field(addr, offset, 8 * sizeof(long));
	    printf("%d(0x%08x), ", val);
	    offset += 8 * sizeof(long);
	    size -= 8 * sizeof(long);
	}

E 5
	val = get_field(addr, offset, size);
	if (tptr->t_val.val_r.r_lower < 0) /* signed */
	    if (val & (1 << (size - 1))) /* test sign big */
		val |= (-1 << size);	/* smash in sign extension */
D 5
	printf("%*s", indent, "");
	print_name(name, tptr);
E 5
	if (size == 8)
D 5
	    printf(" = %d(0x%0*x)'%c'\n", val, (size +3) / 4,val, val);
E 5
I 5
	    printf("%d(0x%0*x)'%c'\n", val, (size +3) / 4,val, val);
E 5
	else
D 5
	    printf(" = %d(0x%0*x)\n", val, (size + 3) / 4, val);
E 5
I 5
	    printf("%d(0x%0*x)\n", val, (size + 3) / 4, val);
E 5
	break;
E 3

D 3
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
E 3
I 3
    case PROC_TYPE:
	printf("%*s", indent, "");
	print_name(name, tptr);
	printf(" = 0x%08x\n", f2v(addr));
	break;
E 3

D 3
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
E 3
I 3
    case FLOAT_TYPE:
	if (size == sizeof(float) * 8)
	    d = *(float *)addr;
	else
	    d = *(double *)addr;
	
	printf("%*s", indent, "");
	print_name(name, tptr);
	printf(" = %f\n", d);
	break;
E 3

D 3
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
E 3
I 3
    case ENUM_TYPE:
	val = get_field(addr, offset, size);
	e_val = "UNKOWN";
	for (eptr = tptr->t_val.val_e; eptr; eptr = eptr->e_next)
	    if (eptr->e_val == val) {
		e_val = eptr->e_name;
		break;
	    }
	printf("%*s", indent, "");
	print_name(name, tptr);
	printf(" = %s(0x%08x)\n", e_val, val);
	break;
E 3

D 3
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
D 2
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
E 2
I 2
signed char *sc_gaddr(expr *n)
{
    return (signed char *)n->e_addr;
E 3
I 3
    case STRINGPTR_TYPE:
	printf("%s bogus stringptr\n", name);
	break;
    case COMPLEX_TYPE:
	printf("%s bogus complex\n", name);
	break;
    case CONSTANT_TYPE:
	printf("%*s", indent, "");
	printf("const ");
	print_out(tptr->t_val.val_k, addr, offset, size, 0, name);
	break;
    default:
	printf("%s bogus default\n", name);
	break;
    }
E 3
}
E 2

I 2
D 3
unsigned char *uc_gaddr(expr *n)
E 3
I 3
static void print_name(char *name, typeptr tptr)
E 3
{
D 3
    return (unsigned char *)n->e_addr;
}
E 3
I 3
    char buf[32];
    typeptr ltptr;
    char *lname;
    char *u_or_s;
E 3

D 3
int *i_gaddr(expr *n)
{
    return (int *)n->e_addr;
}
E 3
I 3
    if (!tptr)
	printf("%s", name);
E 3

D 3
unsigned int *ui_gaddr(expr *n)
{
    return (unsigned int *)n->e_addr;
}
E 3
I 3
    switch (tptr->t_type) {
    case STRUCT_TYPE:
    case UNION_TYPE:
	u_or_s = tptr->t_type == STRUCT_TYPE ? "struct" : "union";
	if (tptr->t_name)
	    printf("%s %s %s", u_or_s, tptr->t_name, name);
	else
	    printf("%s %s", u_or_s, name);
	break;
E 3

D 3
short *s_gaddr(expr *n)
{
    return (short *)n->e_addr;
}
E 3
I 3
    case PTR_TYPE:
	if ((ltptr = tptr->t_val.val_p) &&
	    (ltptr->t_type == ARRAY_TYPE || ltptr->t_type == PROC_TYPE))
	    sprintf(buf, "(*%s)", name);
	else
	    sprintf(buf, "*%s", name);
	print_name(buf, ltptr);
	break;
E 3

D 3
unsigned short *us_gaddr(expr *n)
{
    return (unsigned short *)n->e_addr;
}
E 3
I 3
    case ARRAY_TYPE:
	if (!(lname = tptr->t_name))
	    lname = "";
	ltptr = tptr->t_val.val_a.a_typeptr;
	sprintf(buf, "%s[]", name);
	print_name(buf, ltptr);
	break;
E 3

D 3
long *l_gaddr(expr *n)
{
    return (long *)n->e_addr;
}
E 3
I 3
    case RANGE_TYPE:
	if ((!(ltptr = tptr->t_val.val_r.r_typeptr) ||
	     !(lname = ltptr->t_name)) &&
	    !(lname = tptr->t_name))
	    lname = "";
	printf("%s %s", lname, name);
	break;
E 3

D 3
unsigned long *ul_gaddr(expr *n)
{
    return (unsigned long *)n->e_addr;
}
E 3
I 3
    case PROC_TYPE:
	ltptr = tptr->t_val.val_f.f_typeptr;
	sprintf(buf, "%s()", name);
	print_name(buf, ltptr);
	break;
E 3

D 3
float *f_gaddr(expr *n)
{
    return (float *)n->e_addr;
}
E 3
I 3
    case FLOAT_TYPE:
	printf("%s %s", tptr->t_name, name);
	break;
E 3

D 3
double *d_gaddr(expr *n)
{
    return (double *)n->e_addr;
}
E 3
I 3
    case ENUM_TYPE:
	if (tptr->t_name)
	    printf("enum %s %s", tptr->t_name, name);
	else
	    printf("enum %s", name);
	break;
E 3

D 3
st *st_gaddr(expr *n)
{
    return (st *)n->e_addr;
}

E 2
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
D 2
    signed char v, *a;
    a = sc_addr(n->e_left);
    v_read(&v, a, sizeof(v));
    return v;
E 2
I 2
    return *v2f_type(signed char *, sc_addr(n->e_left));
E 2
}
I 2

E 2
unsigned char uc_l2p(expr *n)
{
D 2
    unsigned char v, *a;
    a = uc_addr(n->e_left);
    v_read(&v, a, sizeof(v));
    return v;
E 2
I 2
    return *v2f_type(unsigned char *, uc_addr(n->e_left));
E 2
}
I 2

E 2
int i_l2p(expr *n)
{
D 2
    int v, *a;
    a = i_addr(n->e_left);
    v_read(&v, a, sizeof(v));
    return v;
E 2
I 2
    return *v2f_type(int *, i_addr(n->e_left));
E 2
}
I 2

E 2
unsigned int ui_l2p(expr *n)
{
D 2
    unsigned int v, *a;
    a = ui_addr(n->e_left);
    v_read(&v, a, sizeof(v));
    return v;
E 2
I 2
    return *v2f_type(unsigned int *, ui_addr(n->e_left));
E 2
}
I 2

E 2
short s_l2p(expr *n)
{
D 2
    short v, *a;
    a = s_addr(n->e_left);
    v_read(&v, a, sizeof(v));
    return v;
E 2
I 2
    return *v2f_type(short *, s_addr(n->e_left));
E 2
}
I 2

E 2
unsigned short us_l2p(expr *n)
{
D 2
    unsigned short v, *a;
    a = us_addr(n->e_left);
    v_read(&v, a, sizeof(v));
    return v;
E 2
I 2
    return *v2f_type(unsigned short *, us_addr(n->e_left));
E 2
}
I 2

E 2
long l_l2p(expr *n)
{
D 2
    long v, *a;
    a = l_addr(n->e_left);
    v_read(&v, a, sizeof(v));
    return v;
E 2
I 2
    return *v2f_type(long *, l_addr(n->e_left));
E 2
}
I 2

E 2
unsigned long ul_l2p(expr *n)
{
D 2
    unsigned long v, *a;
    a = ul_addr(n->e_left);
    v_read(&v, a, sizeof(v));
    return v;
E 2
I 2
    return *v2f_type(unsigned long *, ul_addr(n->e_left));
E 2
}
I 2

E 2
float f_l2p(expr *n)
{
D 2
    float v, *a;
    a = f_addr(n->e_left);
    v_read(&v, a, sizeof(v));
    return v;
E 2
I 2
    return *v2f_type(float *, f_addr(n->e_left));
E 2
}
I 2

E 2
double d_l2p(expr *n)
{
D 2
    double v, *a;
    a = d_addr(n->e_left);
    v_read(&v, a, sizeof(v));
    return v;
E 2
I 2
    return *v2f_type(double *, d_addr(n->e_left));
E 2
}
I 2

E 2
st st_l2p(expr *n)
{
D 2
    st v, *a;
    a = st_addr(n->e_left);
    v = (st)a;
    return v;
E 2
I 2
    return (st)st_addr(n->e_left);
E 2
}

I 2

E 2
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
I 2

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
E 3
I 3
    case STRINGPTR_TYPE:
	printf("strptr %s", name);
	break;
    case COMPLEX_TYPE:
	printf("complex %s", name);
	break;
    case CONSTANT_TYPE:
	printf("const %s", name);
	break;
    default:
	printf("unknown type %s", name);
	break;
    }
E 6
I 6
D 8
  printf("set laddr:%08x offset %d size %d val %016llx\n",
	 laddr, offset, size, val);
  laddr += (offset / WSIZE);
  offset %= WSIZE;
  if (offset + size > WSIZE) {
    fprintf(stderr, "set_field spans alignment boundry\n");
    exit(1);
  }
  printf("set new laddr:%08x offset %d\n", laddr, offset);
  temp = *laddr;
  printf("set *laddr:%016llx\n", temp);
  mask = (((ularge_t)-1 >> (WSIZE - size)) << (WSIZE - offset - size));
  printf("set mask:%016llx\n", mask);
  temp &= ~mask;
  printf("set *laddr after mask:%016llx\n", temp);
  val <<= (WSIZE - offset - size);
  val &= mask;
  printf("set val after mask:%016llx\n", val);
  temp |= val;
  printf("set new val:%016llx\n", temp);
  *(ularge_t *)laddr = temp;
  return;
E 8
I 8
    DEBUG_PRINTF(("set laddr:%s offset %d size %d val %s\n",
		  P(laddr), offset, size, P(val)));
    laddr += (offset / WSIZE);
    offset %= WSIZE;
    if (offset + size > WSIZE) {
	fprintf(stderr, "set_field spans alignment boundry\n");
	exit(1);
    }
    DEBUG_PRINTF(("set new laddr:%s offset %d\n", P(laddr), offset));
    temp = *laddr;
    DEBUG_PRINTF(("set *laddr:%s\n", P(temp)));
    mask = (((ularge_t)-1 >> (WSIZE - size)) << (WSIZE - offset - size));
    DEBUG_PRINTF(("set mask:%s\n", P(mask)));
    temp &= ~mask;
    DEBUG_PRINTF(("set *laddr after mask:%s\n", P(temp)));
    val <<= (WSIZE - offset - size);
    val &= mask;
    DEBUG_PRINTF(("set val after mask:%s\n", P(val)));
    temp |= val;
    DEBUG_PRINTF(("set new val:%s\n", P(temp)));
    *(ularge_t *)laddr = temp;
    return;
E 8
E 6
E 3
}
E 2
E 1
