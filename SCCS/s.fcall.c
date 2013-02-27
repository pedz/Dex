h16793
s 00001/00001/00322
d D 1.9 10/08/23 17:01:53 pedzan 9 8
e
s 00062/00009/00261
d D 1.8 02/03/14 16:12:32 pedz 8 7
e
s 00011/00002/00259
d D 1.7 00/09/18 17:57:00 pedz 7 6
c Checking before V5 conversion
e
s 00093/00000/00168
d D 1.6 00/05/29 15:09:06 pedz 6 5
c Just before IA64 conversion
e
s 00001/00001/00167
d D 1.5 99/05/26 15:34:00 pedz 5 4
c *** empty log message ***
e
s 00001/00001/00167
d D 1.4 98/10/23 12:26:09 pedz 4 3
e
s 00014/00007/00154
d D 1.3 95/04/25 10:10:07 pedz 3 2
e
s 00005/00014/00156
d D 1.2 95/02/01 10:36:52 pedz 2 1
e
s 00170/00000/00000
d D 1.1 94/08/22 17:56:33 pedz 1 0
c date and time created 94/08/22 17:56:33 by pedz
e
u
U
t
T
I 1
static char sccs_id[] = "%W%";

#include <stdio.h>
I 3
#include <setjmp.h>
I 8
#include <sys/errno.h>
#include <stdlib.h>
#include <strings.h>
E 8
E 3
#include "map.h"
#include "sym.h"
#include "tree.h"
#include "stmt.h"
I 8
#include "dex.h"
#define DEBUG_BIT FCALL_C_BIT
E 8
D 2
#include "hmap.h"
E 2

D 2
int frame_ptr = h_high;
int stack_ptr = h_high;
E 2
I 2
D 7
long frame_ptr = (long)h_high;
long stack_ptr = (long)h_high;
E 7
I 7
long frame_ptr;
long stack_ptr;
I 8
extern long global_index;
E 8

int setup_pseudos(void)
{
D 8
    extern long global_index;
E 8
I 8
#define XXX_SIZE (64*1024)
E 8

D 8
    frame_ptr = h_high;
    stack_ptr = h_high;
    global_index = h_base;
E 8
I 8
    if (!(global_index = (long)malloc(XXX_SIZE)))
	return ENOMEM;
    frame_ptr = stack_ptr = global_index + XXX_SIZE - sizeof(long);
    DEBUG_PRINTF(("global=0x%s frame=0x%s\n",
		 P(global_index), P(frame_ptr)));
    return 0;
E 8
}
E 7
E 2

static void push(v_ptr a, int size)
{
D 2
    int old_stack = stack_ptr;

E 2
D 8
    stack_ptr -= size;
D 2
    if ((old_stack ^ stack_ptr) & ~(PAGESIZE - 1))
	h_mkpage((v_ptr)stack_ptr);
    v_write(a, (v_ptr)stack_ptr, size);
E 2
I 2
D 5
    bcopy(a, v2f(stack_ptr), size);
E 5
I 5
    bcopy(a, v2f((void *)stack_ptr), size);
E 8
I 8
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
E 8
E 5
E 2
}

static void mark_stack(void)
{
    push((v_ptr)&frame_ptr, sizeof(frame_ptr));
    frame_ptr = stack_ptr;
}

I 6
D 8
#if __64BIT__
E 8
I 8
#ifdef __64BIT__
E 8

E 6
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
I 6
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

I 8
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

E 8
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
E 6
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

I 8
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

E 8
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
D 3
	    printf("bogus type dude!!!\n");
E 3
I 3
	    fprintf(stderr, "bogus type dude!!!\n");
E 3
	    break;
	}
	push(v, size);
	l = l->cl_next;
    }
}
I 6
#endif
E 6

static void release_stack(void)
{
D 2
    v_read(&frame_ptr, (v_ptr)frame_ptr, sizeof(frame_ptr));
E 2
I 2
D 8
    frame_ptr = *v2f_type(long *, frame_ptr);
E 8
I 8
    frame_ptr = *(long *)frame_ptr;
E 8
E 2
}

D 3
static expr *basic_fcall(expr *n)
E 3
I 3
static stmt_index basic_fcall(expr *n)
E 3
{
D 2
    stmt s;
E 2
I 2
    stmt_index s;
E 2
D 3
    expr *e;
E 3

    s = i_val(n->e_call);
    process_args(n->e_args);
    if (s == NO_STMT) {
	fprintf(stderr, "Function called but not defined\n");
	exit(1);
    }
D 3
    mark_stack();
    return execute_statement(s);
E 3
I 3
    return s;
E 3
}

void alloc_stack(int n)
{
D 2
    int old_stack = stack_ptr;

E 2
D 4
    stack_ptr -= n;
E 4
I 4
    stack_ptr -= (n + sizeof(long) - 1) & (~(sizeof(long) - 1));
I 8
    if (stack_ptr < global_index) {
	fprintf(stderr, "alloc_stack: Out of pseudo variable space\n");
	exit(1);
    }
E 8
E 4
D 2
    if ((old_stack ^ stack_ptr) & ~(PAGESIZE - 1))
	h_mkpage((v_ptr)stack_ptr);
E 2
}

#define MK_FCALL(base_type, prefix) \
base_type prefix ## _fcall(expr *n) \
{ \
    base_type result; \
    expr *e; \
I 3
    stmt_index s; \
E 3
D 8
    int cur_stack = stack_ptr; \
E 8
I 8
    long cur_stack = stack_ptr; \
E 8
D 3
 \
    if (e = basic_fcall(n)) \
E 3
I 3
    volatile int had_fault; \
I 8
    int local_stmt_index = cur_stmt_index; \
E 8
     \
I 8
    if (cur_stmt_index < STMT_STACK_SIZE) \
	++cur_stmt_index; \
E 8
    s = basic_fcall(n); \
    mark_stack(); \
    BEGIN_PROTECT(&had_fault); \
    if (e = execute_statement(s)) \
E 3
	result = prefix ## _val(e); \
I 3
    END_PROTECT(); \
E 3
    release_stack(); \
    stack_ptr = cur_stack; \
I 8
    cur_stmt_index = local_stmt_index; \
E 8
I 3
    if (had_fault) \
	return 0; \
E 3
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
I 8
MK_FCALL(long long, ll);
MK_FCALL(unsigned long long, ull);
E 8
MK_FCALL(float, f);
MK_FCALL(double, d);
D 9
MK_FCALL(st, st);
E 9
I 9
MK_FCALL(st_t, st);
E 9
E 1
