static char sccs_id[] = "@(#)fcall.c	1.9";

#include <stdio.h>
#include <setjmp.h>
#include <sys/errno.h>
#include <stdlib.h>
#include <strings.h>
#include "map.h"
#include "sym.h"
#include "tree.h"
#include "stmt.h"
#include "dex.h"
#define DEBUG_BIT FCALL_C_BIT

long frame_ptr;
long stack_ptr;
extern long global_index;

int setup_pseudos(void)
{
#define XXX_SIZE (64*1024)

    if (!(global_index = (long)malloc(XXX_SIZE)))
	return ENOMEM;
    frame_ptr = stack_ptr = global_index + XXX_SIZE - sizeof(long);
    DEBUG_PRINTF(("global=0x%s frame=0x%s\n",
		 P(global_index), P(frame_ptr)));
    return 0;
}

static void push(v_ptr a, int size)
{
    if ((stack_ptr -= size) < global_index) {
	fprintf(stderr, "push: Out of pseudo variable space\n");
	exit(1);
    }	
    bcopy(a, (void *)stack_ptr, size);
    if (debug_mask & DEBUG_BIT) {
	int i;
	DEBUG_PRINTF(("pushing: %d ", size));
	for (i = 0; i < size; ++i)
	    DEBUG_PRINTF(("%02x", ((char *)a)[i]));
	DEBUG_PRINTF((" from %016lx to %016lx\n", a, stack_ptr));
    }
}

static void mark_stack(void)
{
    push((v_ptr)&frame_ptr, sizeof(frame_ptr));
    frame_ptr = stack_ptr;
}

#ifdef __64BIT__

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

	case long_long_type:
	    result.ll = ll_val(e);
	    v = &result.ll;
	    size = sizeof(result.ll);
	    break ;

	case ulong_long_type:
	    result.ull = ull_val(e);
	    v = &result.ull;
	    size = sizeof(result.ull);
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

	case long_long_type:
	    result.ll = ll_val(e);
	    v = &result.ll;
	    size = sizeof(result.ll);
	    break ;

	case ulong_long_type:
	    result.ull = ull_val(e);
	    v = &result.ull;
	    size = sizeof(result.ull);
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
    frame_ptr = *(long *)frame_ptr;
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
    if (stack_ptr < global_index) {
	fprintf(stderr, "alloc_stack: Out of pseudo variable space\n");
	exit(1);
    }
}

#define MK_FCALL(base_type, prefix) \
base_type prefix ## _fcall(expr *n) \
{ \
    base_type result; \
    expr *e; \
    stmt_index s; \
    long cur_stack = stack_ptr; \
    volatile int had_fault; \
    int local_stmt_index = cur_stmt_index; \
     \
    if (cur_stmt_index < STMT_STACK_SIZE) \
	++cur_stmt_index; \
    s = basic_fcall(n); \
    mark_stack(); \
    BEGIN_PROTECT(&had_fault); \
    if (e = execute_statement(s)) \
	result = prefix ## _val(e); \
    END_PROTECT(); \
    release_stack(); \
    stack_ptr = cur_stack; \
    cur_stmt_index = local_stmt_index; \
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
MK_FCALL(long long, ll);
MK_FCALL(unsigned long long, ull);
MK_FCALL(float, f);
MK_FCALL(double, d);
MK_FCALL(st_t, st);
