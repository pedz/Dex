static char sccs_id[] = "@(#)builtins.c	1.2";

#include <stdio.h>
#include <stdlib.h>
#include <sys/signal.h>
#include <setjmp.h>
#include "map.h"
#include "sym.h"
#include "tree.h"
#include "stmt.h"
#include "asgn_expr.h"
#include "base_expr.h"
#include "inter.h"
#include "dex.h"
#include "fcall.h"
#include "builtins.h"

int int_printf(expr *n)
{
    long *f = v2f_type(long *, frame_ptr);
    volatile int had_fault;

    BEGIN_PROTECT(&had_fault);
    vprintf((const unsigned char *)(f[1]), (va_list)(f + 2));
    END_PROTECT();
    if (had_fault)
	printf("printf hit a page fault\n");
    return 0;
}

int int_v2f(expr *n)
{
    long *f = v2f_type(long *, frame_ptr);

    return (int)v2f(f[1]);
}

int int_find(expr *n)
{
    long *f = v2f_type(long *, frame_ptr);
    void *qaddr = (void *)(f[1]);
    char **name = v2f_type(char **, f[2]);
    void **raddr = v2f_type(void **, f[3]);
    symptr s = addr2userdef_all(qaddr);

    if (!s)
	return 0;
    *name = f2v_type(char *, s->s_name);
    *raddr = s->s_offset;
    return 1;
}

extern void dis(long *addr, int count);
int int_dis(expr *n)
{
    long *f = v2f_type(long *, frame_ptr);

    dis((long *)f[1], f[2]);
    return 0;
}

int int_load(expr *n)
{
    long *f = v2f_type(long *, frame_ptr);
    char *s = (char *)(f[1]);

    load(s, f[2], f[3]);
    return 0;
}

static add_entry(char *ss, int (*func)(expr *n), typeptr t)
{
    char *name = store_string(ns_inter, ss, 0, (char *)0);
    symptr s = enter_sym(ns_inter, name);
    expr *e = new_expr();
    
    s->s_defined = 1;
    s->s_base = base_type(t);
    s->s_type = t;
    s->s_nesting = 0;
    s->s_typed = 1;
    s->s_global = 1;
    s->s_size = 0;
    s->s_stmt = mk_return_stmt(e);
    e->e_func.i = func;
}

struct table {
    char *st_name;
    int (*st_func)(expr *n);
};

static struct table tab[] = {
    { "dis", int_dis },
    { "find", int_find },
    { "load", int_load },
    { "printf", int_printf },
    { "v2f", int_v2f },
};

void extra_hack()
{
    struct table *tp, *tp_end;
    typeptr t = newtype(ns_inter, PROC_TYPE);

    t->t_val.val_f.f_typeptr = name2typedef(ns_inter, "int");
    t->t_val.val_f.f_params = -1;
    t->t_val.val_f.f_paramlist = 0;
    for (tp_end = (tp = tab) + A_SIZE(tab); tp < tp_end; ++tp)
	add_entry(tp->st_name, tp->st_func, t);
}
