static char sccs_id[] = "@(#)fcall.c	1.7";

#include <stdio.h>
#include <setjmp.h>
#include "map.h"
#include "sym.h"
#include "tree.h"
#include "stmt.h"

long frame_ptr;
long stack_ptr;

int setup_pseudos(void)
{
    extern long global_index;

    frame_ptr = h_high;
    stack_ptr = h_high;
    global_index = h_base;
}

static void push(v_ptr a, int size)
{
    stack_ptr -= size;
    bcopy(a, v2f((void *)stack_ptr), size);
}

static void mark_stack(void)
{
    push((v_ptr)&frame_ptr, sizeof(frame_ptr));
    frame_ptr = stack_ptr;
}

#if __64BIT__

static void process_args(cnode_list *l)
{
    all result;
    cnode *c;
    expr *e;
    v_ptr v;
    int size;

    while (l) {
	c = &l->cl_cnode;
	e = c->c_expr;
	switch (c->c_base) {
	case schar_type:
	    result.l = sc_val(e);
	    v = &result.l;
	    size = sizeof(result.l);
	    break ;

	case uchar_type:
	    result.ul = uc_val(e);
	    v = &result.ul;
	    size = sizeof(result.ul);
	    break ;

	case int_type:
	    result.l = i_val(e);
	    v = &result.l;
	    size = sizeof(result.l);
	    break;

	case uint_type:
	    result.ul = ui_val(e);
	    v = &result.ul;
	    size = sizeof(result.ul);
	    break ;

	case short_type:
	    result.l = s_val(e);
	    v = &result.l;
	    size = sizeof(result.l);
	    break ;

	case ushort_type:
	    result.ul = us_val(e);
	    v = &result.ul;
	    size = sizeof(result.ul);
	    break ;

	case long_type:
	    result.l = l_val(e);
	    v = &result.l;
	    size = sizeof(result.l);
	    break ;

	case ulong_type:
	    result.ul = ul_val(e);
	    v = &result.ul;
	    size = sizeof(result.ul);
	    break ;

	case float_type:
	    result.d = f_val(e);
	    v = &result.d;
	    size = sizeof(result.d);
	    break ;

	case double_type:
	    result.d = d_val(e);
	    v = &result.d;
	    size = sizeof(result.d);
	    break ;

	case struct_type:
	    result.st = st_val(e);
	    v = (v_ptr)result.st;
	    size = e->e_size;
	    break;

	case void_type:
	default:
	    fprintf(stderr, "bogus type dude!!!\n");
	    break;
	}
	push(v, size);
	l = l->cl_next;
    }
}

#else

static void process_args(cnode_list *l)
{
    all result;
    cnode *c;
    expr *e;
    v_ptr v;
    int size;

    while (l) {
	c = &l->cl_cnode;
	e = c->c_expr;
	switch (c->c_base) {
	case schar_type:
	    result.i = sc_val(e);
	    v = &result.i;
	    size = sizeof(result.i);
	    break ;

	case uchar_type:
	    result.ui = uc_val(e);
	    v = &result.ui;
	    size = sizeof(result.ui);
	    break ;

	case int_type:
	    result.i = i_val(e);
	    v = &result.i;
	    size = sizeof(result.i);
	    break;

	case uint_type:
	    result.ui = ui_val(e);
	    v = &result.ui;
	    size = sizeof(result.ui);
	    break ;

	case short_type:
	    result.i = s_val(e);
	    v = &result.i;
	    size = sizeof(result.i);
	    break ;

	case ushort_type:
	    result.ui = us_val(e);
	    v = &result.ui;
	    size = sizeof(result.ui);
	    break ;

	case long_type:
	    result.l = l_val(e);
	    v = &result.l;
	    size = sizeof(result.l);
	    break ;

	case ulong_type:
	    result.ul = ul_val(e);
	    v = &result.ul;
	    size = sizeof(result.ul);
	    break ;

	case float_type:
	    result.d = f_val(e);
	    v = &result.d;
	    size = sizeof(result.d);
	    break ;

	case double_type:
	    result.d = d_val(e);
	    v = &result.d;
	    size = sizeof(result.d);
	    break ;

	case struct_type:
	    result.st = st_val(e);
	    v = (v_ptr)result.st;
	    size = e->e_size;
	    break;

	case void_type:
	default:
	    fprintf(stderr, "bogus type dude!!!\n");
	    break;
	}
	push(v, size);
	l = l->cl_next;
    }
}
#endif

static void release_stack(void)
{
    frame_ptr = *v2f_type(long *, frame_ptr);
}

static stmt_index basic_fcall(expr *n)
{
    stmt_index s;

    s = i_val(n->e_call);
    process_args(n->e_args);
    if (s == NO_STMT) {
	fprintf(stderr, "Function called but not defined\n");
	exit(1);
    }
    return s;
}

void alloc_stack(int n)
{
    stack_ptr -= (n + sizeof(long) - 1) & (~(sizeof(long) - 1));
}

#define MK_FCALL(base_type, prefix) \
base_type prefix ## _fcall(expr *n) \
{ \
    base_type result; \
    expr *e; \
    stmt_index s; \
    int cur_stack = stack_ptr; \
    volatile int had_fault; \
     \
    s = basic_fcall(n); \
    mark_stack(); \
    BEGIN_PROTECT(&had_fault); \
    if (e = execute_statement(s)) \
	result = prefix ## _val(e); \
    END_PROTECT(); \
    release_stack(); \
    stack_ptr = cur_stack; \
    if (had_fault) \
	return 0; \
    return result; \
}

MK_FCALL(signed char, sc);
MK_FCALL(unsigned char, uc);
MK_FCALL(int, i);
MK_FCALL(unsigned int, ui);
MK_FCALL(short, s);
MK_FCALL(unsigned short, us);
MK_FCALL(long, l);
MK_FCALL(unsigned long, ul);
MK_FCALL(float, f);
MK_FCALL(double, d);
MK_FCALL(st, st);
