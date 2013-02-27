h12958
s 00068/00012/00443
d D 1.13 10/08/23 18:01:50 pedzan 13 12
c Checkin in chnages made over the years
e
s 00000/00000/00455
d D 1.12 10/08/23 17:14:09 pedzan 12 11
c Checking in changes made over the years
e
s 00124/00026/00331
d D 1.11 02/03/14 16:12:31 pedz 11 10
e
s 00000/00005/00357
d D 1.10 00/09/18 17:56:58 pedz 10 9
c Checking before V5 conversion
e
s 00002/00000/00360
d D 1.9 00/05/29 15:09:06 pedz 9 8
c Just before IA64 conversion
e
s 00001/00001/00359
d D 1.8 00/02/08 17:36:46 pedz 8 7
e
s 00002/00002/00358
d D 1.7 99/05/26 15:34:05 pedz 7 6
c *** empty log message ***
e
s 00009/00001/00351
d D 1.6 98/10/23 12:26:07 pedz 6 5
e
s 00035/00005/00317
d D 1.5 97/05/13 16:02:55 pedz 5 4
c Check pointer
e
s 00030/00005/00292
d D 1.4 95/05/17 16:39:08 pedz 4 3
c Check point
e
s 00216/00030/00081
d D 1.3 95/04/25 10:09:54 pedz 3 2
e
s 00077/00012/00034
d D 1.2 95/02/09 21:35:40 pedz 2 1
e
s 00046/00000/00000
d D 1.1 95/02/01 10:57:36 pedz 1 0
c date and time created 95/02/01 10:57:36 by pedz
e
u
U
t
T
I 1
static char sccs_id[] = "%W%";

I 13
#ifdef __64BIT__
#define __XCOFF64__
#endif

E 13
#include <stdio.h>
#include <stdlib.h>
#include <sys/signal.h>
I 2
#include <setjmp.h>
I 3
#include <string.h>
D 4
#include <dbxstclass.h>
E 4
I 4
#include <a.out.h>
I 5
#include <stdarg.h>
I 6
#include "dex.h"
E 6
E 5
E 4
E 3
E 2
#include "map.h"
I 3
D 11
#include "dmap.h"
E 11
E 3
#include "sym.h"
#include "tree.h"
#include "stmt.h"
D 2
#include "gram.h"
E 2
#include "asgn_expr.h"
#include "base_expr.h"
#include "inter.h"
D 6
#include "dex.h"
E 6
#include "fcall.h"
I 3
#include "disasm.h"
E 3
I 2
#include "builtins.h"
I 9
#include "load.h"
I 11
#define DEBUG_BIT BUILTINS_C_BIT
E 11
E 9
E 2

I 5
/*
 * Called as: return_range(char *t_name, char *d_name, void **startp, int *lenp)
 */
int int_return_range(expr *n)
{
    long *f = v2f_type(long *, frame_ptr);
    char *t_name = (char *)f[1];
    char *d_name = (char *)f[2];
    void **startp = (void *)f[3];
    int *lenp = (void *)f[4];
    
    return return_range(t_name, d_name, startp, lenp);
}

E 5
I 3
int int_regs_instr(expr *n)
{
    long *f = v2f_type(long *, frame_ptr);

    return regs_instr((long *)f[1], (int)f[2]);
}

char *int_instr(expr *n)
{
    long *f = v2f_type(long *, frame_ptr);

    return instr((long *)f[1]);
}

char *int_strcpy(expr *n)
{
    long *f = v2f_type(long *, frame_ptr);

    return strcpy((char *)f[1], (char *)f[2]);
}

char *int_strncpy(expr *n)
{
    long *f = v2f_type(long *, frame_ptr);

    return strncpy((char *)f[1], (char *)f[2], f[3]);
}

char *int_strcat(expr *n)
{
    long *f = v2f_type(long *, frame_ptr);

    return strcat((char *)f[1], (char *)f[2]);
}

char *int_strncat(expr *n)
{
    long *f = v2f_type(long *, frame_ptr);

    return strncat((char *)f[1], (char *)f[2], f[3]);
}

int int_strcmp(expr *n)
{
    long *f = v2f_type(long *, frame_ptr);

    return strcmp((char *)f[1], (char *)f[2]);
}

I 5
int int_strlen(expr *n)
{
    long *f = v2f_type(long *, frame_ptr);

    return strlen((char *)f[1]);
}

E 5
int int_strncmp(expr *n)
{
    long *f = v2f_type(long *, frame_ptr);

    return strncmp((char *)f[1], (char *)f[2], f[3]);
}

char *int_strchr(expr *n)
{
    long *f = v2f_type(long *, frame_ptr);

    return strchr((char *)f[1], f[2]);
}

char *int_strrchr(expr *n)
{
    long *f = v2f_type(long *, frame_ptr);

    return strrchr((char *)f[1], f[2]);
}

int int_purge_user_pages(expr *n)
{
    return purge_user_pages();
}

int int_purge_all_pages(expr *n)
{
    return purge_all_pages();
}

int int_sprintf(expr *n)
{
    long *f = v2f_type(long *, frame_ptr);
D 11
    volatile int had_fault;
E 11
I 11
    volatile long had_fault;
    int ret = -1;
E 11

    BEGIN_PROTECT(&had_fault);
D 11
    vsprintf((char *)(f[1]), (const unsigned char *)(f[2]), (va_list)(f + 3));
E 11
I 11
    ret = vsprintf((char *)(f[1]),
		   (const unsigned char *)(f[2]),
		   (va_list)(f + 3));
E 11
    END_PROTECT();
    if (had_fault)
D 11
	printf("sprintf hit a page fault at %08x\n", had_fault);
    return 0;
E 11
I 11
	printf("sprintf hit a page fault at %s\n", P(had_fault));
    return ret;
E 11
}

E 3
D 2
static int int_printf(expr *n)
E 2
I 2
int int_printf(expr *n)
E 2
{
    long *f = v2f_type(long *, frame_ptr);
I 2
D 11
    volatile int had_fault;
E 11
I 11
    volatile long had_fault;
    int ret = -1;
E 11
E 2

I 2
    BEGIN_PROTECT(&had_fault);
E 2
D 11
    vprintf((const unsigned char *)(f[1]), (va_list)(f + 2));
E 11
I 11
    ret = vprintf((const unsigned char *)(f[1]), (va_list)(f + 2));
E 11
D 2
    return 4;
E 2
I 2
    END_PROTECT();
    if (had_fault)
D 3
	printf("printf hit a page fault\n");
E 3
I 3
D 11
	printf("printf hit a page fault at %08x\n", had_fault);
E 3
    return 0;
E 11
I 11
	printf("printf hit a page fault at %s\n", P(had_fault));
    return ret;
E 11
E 2
}

D 2
void extra_hack()
E 2
I 2
D 11
int int_v2f(expr *n)
E 11
I 11
long int_v2f(expr *n)
E 11
E 2
{
D 2
    char *name = store_string(ns_inter, "printf", 0, (char *)0);
E 2
I 2
    long *f = v2f_type(long *, frame_ptr);

D 7
    return (int)v2f(f[1]);
E 7
I 7
D 11
    return (int)v2f((void *)(f[1]));
E 11
I 11
    return (long)v2f((void *)(f[1]));
E 11
E 7
}

I 6
D 11
int int_f2v(expr *n)
E 11
I 11
long int_f2v(expr *n)
E 11
{
    long *f = v2f_type(long *, frame_ptr);

D 7
    return (int)f2v(f[1]);
E 7
I 7
D 11
    return (int)f2v((void *)(f[1]));
E 11
I 11
    return (long)f2v((void *)(f[1]));
E 11
E 7
}

I 13
long int_name2userdef(expr *n)
{
    long *f = v2f_type(long *, frame_ptr);
    char *name = (char *)f[1];
    return name2userdef_all(name);
}

E 13
E 6
I 4
/*
I 13
 * called as: find_name(char *name, long offset, char **file_name_p,
 *                      int *lineno_p);
 */
 
int int_find_name(expr *n)
{
    long *f = v2f_type(long *, frame_ptr);
    char *name = (char *)f[1];
    long offset = (long)f[2];
    char **file_name_p = v2f_type(char **, f[3]);
    int *lineno_p = v2f_type(int *, f[4]);
    symptr s = name2userdef_all(name);
    ns *nspace;
    long target_addr;
    
    if (!s)
	return 0;

    target_addr = s->s_offset + offset;
    for (nspace = s->s_ns->ns_lower; nspace; nspace = nspace->ns_next)
	if (!strcmp(s->s_name, nspace->ns_name))
	    break;

    /*
     * It may be that we get back more than one symbol and we need to
     * loop through them.  We'll deal with that when we get there.
     */
    if (nspace) {
	struct lineno *lines;

	if (nspace->ns_parent && file_name_p)
	    *file_name_p = f2v_type(char *, nspace->ns_parent->ns_name);
	if (lineno_p && (lines = nspace->ns_lines)) {
	    for (++lines; lines->l_lnno; ++lines)
		if (lines->l_addr.l_paddr > target_addr)
		    break;
	    *lineno_p = nspace->ns_lineoffset + lines[-1].l_lnno - 1;
	}
    }
    return 1;
}

/*
E 13
 * called as: find(void *addr, char **func_name_p, void **start_p,
 *                 char **file_name_p, int *lineno_p);
 */
E 4
int int_find(expr *n)
{
    long *f = v2f_type(long *, frame_ptr);
    void *qaddr = (void *)(f[1]);
D 4
    char **name = v2f_type(char **, f[2]);
    void **raddr = v2f_type(void **, f[3]);
E 4
I 4
    char **func_name_p = v2f_type(char **, f[2]);
    void **start_p = v2f_type(void **, f[3]);
    char **file_name_p = v2f_type(char **, f[4]);
    int *lineno_p = v2f_type(int *, f[5]);
E 4
    symptr s = addr2userdef_all(qaddr);
I 4
    ns *nspace;
E 4

    if (!s)
	return 0;
D 4
    *name = f2v_type(char *, s->s_name);
    *raddr = s->s_offset;
E 4
I 4
    
    nspace = s->s_ns;
    for (nspace = nspace->ns_lower; nspace; nspace = nspace->ns_next)
	if (!strcmp(s->s_name, nspace->ns_name))
	    break;
    if (nspace) {
	struct lineno *lines;
D 5
	int diff = (ulong)qaddr - (ulong)s->s_offset;
E 5

	if (nspace->ns_parent && file_name_p)
	    *file_name_p = f2v_type(char *, nspace->ns_parent->ns_name);
	if (lineno_p && (lines = nspace->ns_lines)) {
	    for (++lines; lines->l_lnno; ++lines)
D 5
		if (lines->l_addr.l_paddr > diff)
E 5
I 5
		if (lines->l_addr.l_paddr > (ulong)qaddr)
E 5
		    break;
	    *lineno_p = nspace->ns_lineoffset + lines[-1].l_lnno - 1;
	}
    }
D 5
    *func_name_p = f2v_type(char *, s->s_name);
    *start_p = s->s_offset;
E 5
I 5
    if (func_name_p)
	*func_name_p = f2v_type(char *, s->s_name);
    if (start_p)
	*start_p = s->s_offset;
E 5
E 4
    return 1;
}

D 3
extern void dis(long *addr, int count);
E 3
int int_dis(expr *n)
{
    long *f = v2f_type(long *, frame_ptr);

    dis((long *)f[1], f[2]);
    return 0;
}

int int_load(expr *n)
{
D 11
    long *f = v2f_type(long *, frame_ptr);
    char *s = (char *)(f[1]);
E 11
I 11
    long *f;
    char *s;
E 11

D 11
    load(s, f[2], f[3]);
    return 0;
E 11
I 11
    f = v2f_type(long *, frame_ptr);
    DEBUG_PRINTF(("frame ptr = %s, f = %s\n", P(frame_ptr), P(f)));
    s = (char *)(f[1]);
    return load(s, f[2], f[3]);
E 11
}

D 11
static add_entry(char *ss, int (*func)(expr *n), typeptr t)
{
D 3
    char *name = store_string(ns_inter, ss, 0, (char *)0);
E 2
    symptr s = enter_sym(ns_inter, name);
D 2
    typeptr t = newtype(ns_inter, PROC_TYPE);
E 2
    expr *e = new_expr();
    
D 2
    t->t_val.val_f.f_typeptr = name2typedef(ns_inter, "int");
    t->t_val.val_f.f_params = -1;
    t->t_val.val_f.f_paramlist = 0;
E 2
    s->s_defined = 1;
    s->s_base = base_type(t);
    s->s_type = t;
    s->s_nesting = 0;
    s->s_typed = 1;
    s->s_global = 1;
    s->s_size = 0;
    s->s_stmt = mk_return_stmt(e);
D 2
    e->e_func.i = int_printf;
    e->e_i = 55;
    e->e_size = 0;
E 2
I 2
    e->e_func.i = func;
E 3
}

E 11
D 3
struct table {
    char *st_name;
    int (*st_func)(expr *n);
};
E 3
I 3
static void do_int_funcs(void)
{
    struct table {
	char *st_name;
	int (*st_func)(expr *n);
    };
    struct table tab[] = {
D 13
	{ "dis",     int_dis },
I 6
D 11
	{ "f2v",     int_f2v },
E 11
E 6
	{ "find",    int_find },
	{ "load",    int_load },
	{ "printf",  int_printf },
	{ "purge_all_pages", int_purge_all_pages },
E 13
I 13
	{ "dis",              int_dis },
	{ "find",             int_find },
	{ "find_name",        int_find_name },
	{ "load",             int_load },
	{ "printf",           int_printf },
	{ "purge_all_pages",  int_purge_all_pages },
E 13
	{ "purge_user_pages", int_purge_user_pages },
D 13
	{ "regs_instr", int_regs_instr },
I 5
	{ "return_range", int_return_range },
E 5
	{ "sprintf", int_sprintf },
	{ "strcmp",  int_strcmp },
I 5
	{ "strlen",  int_strlen },
E 5
	{ "strncmp", int_strncmp },
E 13
I 13
	{ "regs_instr",       int_regs_instr },
	{ "return_range",     int_return_range },
	{ "sprintf",          int_sprintf },
	{ "strcmp",           int_strcmp },
	{ "strlen",           int_strlen },
	{ "strncmp",          int_strncmp },
E 13
D 11
	{ "v2f",     int_v2f },
E 11
    };
    struct table *tp, *tp_end;
    typeptr t = newtype(ns_inter, PROC_TYPE);
E 3

D 3
static struct table tab[] = {
    { "dis", int_dis },
    { "find", int_find },
    { "load", int_load },
    { "printf", int_printf },
    { "v2f", int_v2f },
};
E 3
I 3
    t->t_val.val_f.f_typeptr = find_type(ns_inter, TP_INT);
    t->t_val.val_f.f_params = -1;
    t->t_val.val_f.f_paramlist = 0;
    for (tp_end = (tp = tab) + A_SIZE(tab); tp < tp_end; ++tp) {
	char *name = store_string(ns_inter, tp->st_name, 0, (char *)0);
	symptr s = enter_sym(ns_inter, name, 0);
	expr *e = new_expr();
    
	/*
	 * We just blindly assume that this is not a duplicate entry.
	 */
	s->s_defined = 1;
	s->s_base = base_type(t);
	s->s_type = t;
	s->s_nesting = 0;
	s->s_typed = 1;
	s->s_global = 1;
	s->s_size = 0;
	s->s_stmt = mk_return_stmt(e);
	e->e_func.i = tp->st_func;
I 11
	DEBUG_PRINTF(("do_int_vars: %s at %d\n", s->s_name,
		      s->s_stmt));
E 11
    }
}
E 3

I 11
long int_addr2seg(expr *n)
{
    return 0;
}

long int_addr2segtest(expr *n)
{
    long *f = v2f_type(long *, frame_ptr);

    return get_addr2seg(f[1]);
}

static void do_long_funcs(void)
{
    struct table {
	char *st_name;
	long (*st_func)(expr *n);
    };
    struct table tab[] = {
	{ "addr2seg",     int_addr2seg },
	{ "addr2segtest", int_addr2segtest },
	{ "f2v",          int_f2v },
D 13
	{ "v2f",          int_v2f }
E 13
I 13
	{ "v2f",          int_v2f },
	{ "name2userdef", int_name2userdef }
E 13
    };
    struct table *tp, *tp_end;
    typeptr t = newtype(ns_inter, PROC_TYPE);

    if (sizeof(long) == 4)
	t->t_val.val_f.f_typeptr = find_type(ns_inter, TP_LONG);
    else
	t->t_val.val_f.f_typeptr = find_type(ns_inter, TP_LONG_64);
    t->t_val.val_f.f_params = -1;
    t->t_val.val_f.f_paramlist = 0;
    for (tp_end = (tp = tab) + A_SIZE(tab); tp < tp_end; ++tp) {
	char *name = store_string(ns_inter, tp->st_name, 0, (char *)0);
	symptr s = enter_sym(ns_inter, name, 0);
	expr *e = new_expr();
    
	/*
	 * We just blindly assume that this is not a duplicate entry.
	 */
	s->s_defined = 1;
	s->s_base = base_type(t);
	s->s_type = t;
	s->s_nesting = 0;
	s->s_typed = 1;
	s->s_global = 1;
	s->s_size = 0;
	s->s_stmt = mk_return_stmt(e);
	e->e_func.l = tp->st_func;
	DEBUG_PRINTF(("do_long funcs: %s at %d\n", s->s_name,
		      s->s_stmt));
    }
}

E 11
D 3
void extra_hack()
E 3
I 3
static void do_char_ptr_funcs(void)
E 3
{
I 3
    struct table {
	char *st_name;
	char *(*st_func)(expr *n);
    };
    struct table tab[] = {
	{ "index",   int_strchr },
	{ "instr",   int_instr },
	{ "rindex",  int_strrchr },
	{ "strcat",  int_strcat },
	{ "strchr",  int_strchr },
	{ "strcpy",  int_strcpy },
	{ "strncat", int_strncat },
	{ "strncpy", int_strncpy },
	{ "strrchr", int_strrchr },
    };
E 3
    struct table *tp, *tp_end;
    typeptr t = newtype(ns_inter, PROC_TYPE);

D 3
    t->t_val.val_f.f_typeptr = name2typedef(ns_inter, "int");
E 3
I 3
    t->t_val.val_f.f_typeptr = newtype(ns_inter, PTR_TYPE);
E 3
    t->t_val.val_f.f_params = -1;
    t->t_val.val_f.f_paramlist = 0;
D 3
    for (tp_end = (tp = tab) + A_SIZE(tab); tp < tp_end; ++tp)
	add_entry(tp->st_name, tp->st_func, t);
E 3
I 3
    t->t_val.val_f.f_typeptr->t_val.val_p = find_type(ns_inter, TP_CHAR);

    for (tp_end = (tp = tab) + A_SIZE(tab); tp < tp_end; ++tp) {
	char *name = store_string(ns_inter, tp->st_name, 0, (char *)0);
	symptr s = enter_sym(ns_inter, name, 0);
	expr *e = new_expr();
    
	/*
	 * We just blindly assume that this is not a duplicate entry.
	 */
	s->s_defined = 1;
	s->s_base = base_type(t);
	s->s_type = t;
	s->s_nesting = 0;
	s->s_typed = 1;
	s->s_global = 1;
	s->s_size = 0;
	s->s_stmt = mk_return_stmt(e);
	e->e_func.l = (long (*)())tp->st_func;
I 11
	DEBUG_PRINTF(("do_char_ptr_vars: %s at %d\n", s->s_name,
		      s->s_stmt));
E 11
    }
}

static void do_int_vars(void)
{
    struct table {
	char *st_name;
	int *st_func;
    };
    struct table tab[] = {
I 5
D 10
#ifdef _AIX41
E 10
E 5
	{ "thread_slot",  &thread_slot },
D 5
	{ "selected_cpu", &selected_cpu }
E 5
I 5
D 10
#else
	{ "proc_slot",  &proc_slot },
#endif
E 10
D 11
	{ "selected_cpu", &selected_cpu },
	{ "fromdump",     &fromdump }
E 11
I 11
	{ "fromdump",     &fromdump },
E 11
E 5
    };
    struct table *tp, *tp_end;
    typeptr t = find_type(ns_inter, TP_INT);

    for (tp_end = (tp = tab) + A_SIZE(tab); tp < tp_end; ++tp) {
	char *name = store_string(ns_inter, tp->st_name, 0, (char *)0);
	symptr s = enter_sym(ns_inter, name, 0);
    
	/*
	 * We just blindly assume that this is not a duplicate entry.
	 */
	s->s_defined = 1;
	s->s_base = base_type(t);
	s->s_type = t;
	s->s_nesting = 0;
	s->s_typed = 1;
	s->s_global = 1;
	s->s_size = sizeof(int);
	s->s_offset = f2v(tp->st_func);
I 11
	DEBUG_PRINTF(("do_int_vars: %s at 0x%0*lx\n", s->s_name,
		      sizeof(s->s_offset)*2, s->s_offset));
E 11
I 9
D 10
	printf("%s %16lx\n", tp->st_name, tp->st_func);
E 10
E 9
    }
}

D 8
void extra_hack()
E 8
I 8
D 11
void builtin_init()
E 11
I 11
static void do_ulong_vars(void)
E 11
E 8
{
I 11
    struct table {
	char *st_name;
	unsigned long *st_func;
    };
    struct table tab[] = {
	{ "debug_mask",   &debug_mask }
    };
    struct table *tp, *tp_end;
#ifdef __64BIT__
    typeptr t = find_type(ns_inter, TP_ULONG_64);
#else
    typeptr t = find_type(ns_inter, TP_ULONG);
#endif

    for (tp_end = (tp = tab) + A_SIZE(tab); tp < tp_end; ++tp) {
	char *name = store_string(ns_inter, tp->st_name, 0, (char *)0);
	symptr s = enter_sym(ns_inter, name, 0);
    
	/*
	 * We just blindly assume that this is not a duplicate entry.
	 */
	s->s_defined = 1;
	s->s_base = base_type(t);
	s->s_type = t;
	s->s_nesting = 0;
	s->s_typed = 1;
	s->s_global = 1;
	s->s_size = sizeof(int);
	s->s_offset = f2v(tp->st_func);
	DEBUG_PRINTF(("do_ulong_vars: %s at 0x%0*lx\n", s->s_name,
		      sizeof(s->s_offset)*2, s->s_offset));
    }
}

void builtin_init(void)
{
E 11
    do_int_funcs();
I 11
    do_long_funcs();
E 11
    do_char_ptr_funcs();
    do_int_vars();
I 11
    do_ulong_vars();
E 11
E 3
E 2
}
E 1
