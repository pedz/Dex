static char sccs_id[] = "@(#)builtins.c	1.4";

#include <stdio.h>
#include <stdlib.h>
#include <sys/signal.h>
#include <setjmp.h>
#include <string.h>
#include <a.out.h>
#include "map.h"
#include "dmap.h"
#include "sym.h"
#include "tree.h"
#include "stmt.h"
#include "asgn_expr.h"
#include "base_expr.h"
#include "inter.h"
#include "dex.h"
#include "fcall.h"
#include "disasm.h"
#include "builtins.h"

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
    volatile int had_fault;

    BEGIN_PROTECT(&had_fault);
    vsprintf((char *)(f[1]), (const unsigned char *)(f[2]), (va_list)(f + 3));
    END_PROTECT();
    if (had_fault)
	printf("sprintf hit a page fault at %08x\n", had_fault);
    return 0;
}

int int_printf(expr *n)
{
    long *f = v2f_type(long *, frame_ptr);
    volatile int had_fault;

    BEGIN_PROTECT(&had_fault);
    vprintf((const unsigned char *)(f[1]), (va_list)(f + 2));
    END_PROTECT();
    if (had_fault)
	printf("printf hit a page fault at %08x\n", had_fault);
    return 0;
}

int int_v2f(expr *n)
{
    long *f = v2f_type(long *, frame_ptr);

    return (int)v2f(f[1]);
}

/*
 * called as: find(void *addr, char **func_name_p, void **start_p,
 *                 char **file_name_p, int *lineno_p);
 */
int int_find(expr *n)
{
    long *f = v2f_type(long *, frame_ptr);
    void *qaddr = (void *)(f[1]);
    char **func_name_p = v2f_type(char **, f[2]);
    void **start_p = v2f_type(void **, f[3]);
    char **file_name_p = v2f_type(char **, f[4]);
    int *lineno_p = v2f_type(int *, f[5]);
    symptr s = addr2userdef_all(qaddr);
    ns *nspace;

    if (!s)
	return 0;
    
    nspace = s->s_ns;
    for (nspace = nspace->ns_lower; nspace; nspace = nspace->ns_next)
	if (!strcmp(s->s_name, nspace->ns_name))
	    break;
    if (nspace) {
	struct lineno *lines;
	int diff = (ulong)qaddr - (ulong)s->s_offset;

	if (nspace->ns_parent && file_name_p)
	    *file_name_p = f2v_type(char *, nspace->ns_parent->ns_name);
	if (lineno_p && (lines = nspace->ns_lines)) {
	    for (++lines; lines->l_lnno; ++lines)
		if (lines->l_addr.l_paddr > diff)
		    break;
	    *lineno_p = nspace->ns_lineoffset + lines[-1].l_lnno - 1;
	}
    }
    *func_name_p = f2v_type(char *, s->s_name);
    *start_p = s->s_offset;
    return 1;
}

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
}

static void do_int_funcs(void)
{
    struct table {
	char *st_name;
	int (*st_func)(expr *n);
    };
    struct table tab[] = {
	{ "dis",     int_dis },
	{ "find",    int_find },
	{ "load",    int_load },
	{ "printf",  int_printf },
	{ "purge_all_pages", int_purge_all_pages },
	{ "purge_user_pages", int_purge_user_pages },
	{ "regs_instr", int_regs_instr },
	{ "sprintf", int_sprintf },
	{ "strcmp",  int_strcmp },
	{ "strncmp", int_strncmp },
	{ "v2f",     int_v2f },
    };
    struct table *tp, *tp_end;
    typeptr t = newtype(ns_inter, PROC_TYPE);

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
    }
}

static void do_char_ptr_funcs(void)
{
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
    struct table *tp, *tp_end;
    typeptr t = newtype(ns_inter, PROC_TYPE);

    t->t_val.val_f.f_typeptr = newtype(ns_inter, PTR_TYPE);
    t->t_val.val_f.f_params = -1;
    t->t_val.val_f.f_paramlist = 0;
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
    }
}

static void do_int_vars(void)
{
    struct table {
	char *st_name;
	int *st_func;
    };
    struct table tab[] = {
	{ "thread_slot",  &thread_slot },
	{ "selected_cpu", &selected_cpu }
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
    }
}

void extra_hack()
{
    do_int_funcs();
    do_char_ptr_funcs();
    do_int_vars();
}
