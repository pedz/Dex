static char sccs_id[] = "@(#)builtins.c	1.14";

#ifdef __64BIT__
#define __XCOFF64__
#endif

#include <stdio.h>
#include <stdlib.h>
#include <sys/signal.h>
#include <setjmp.h>
#include <string.h>
#include <a.out.h>
#include <stdarg.h>
#include "dex.h"
#include "map.h"
#include "sym.h"
#include "tree.h"
#include "stmt.h"
#include "asgn_expr.h"
#include "base_expr.h"
#include "inter.h"
#include "fcall.h"
#include "disasm.h"
#include "builtins.h"
#include "load.h"
#define DEBUG_BIT BUILTINS_C_BIT

static char *int_fgets(expr *n);
static char *int_instr(expr *n);
static char *int_strcat(expr *n);
static char *int_strchr(expr *n);
static char *int_strcpy(expr *n);
static char *int_strncat(expr *n);
static char *int_strncpy(expr *n);
static char *int_strrchr(expr *n);

static int   int_dis(expr *n);
static int   int_errout(expr *n);
static int   int_fclose(expr *n);
static int   int_fflush(expr *n);
static int   int_find(expr *n);
static int   int_find_name(expr *n);
static int   int_fprintf(expr *n);
static int   int_load(expr *n);
static int   int_pclose(expr *n);
static int   int_printf(expr *n);
static int   int_purge_all_pages(expr *n);
static int   int_purge_user_pages(expr *n);
static int   int_regs_instr(expr *n);
static int   int_return_range(expr *n);
static int   int_rmap_dump(expr *n);
static int   int_sprintf(expr *n);
static int   int_strcmp(expr *n);
static int   int_strlen(expr *n);
static int   int_strncmp(expr *n);

static long  int_f2v(expr *n);
static long  int_name2userdef(expr *n);
static long  int_v2f(expr *n);

static void  int_free(expr *n);

static void *int_fopen(expr *n);
static void *int_malloc(expr *n);
static void *int_popen(expr *n);

/**
 * Currently not used but it points to stdin normally.
 */
FILE *dexin = stdin;
/**
 * printf prints to this FILE.  It starts out pointing to stdout but
 * is redirected when a pipe or file is opened via a command.  Note
 * that it is reset to point to stdout at the end of every interactive
 * command just before the prompt is printed.
 */
FILE *dexout = stdout;
/**
 * Currently not used starts out pointing to stderr.
 */
FILE *dexerr = stderr;

static char *int_fgets(expr *n)
{
    long *f = v2f_type(long *, frame_ptr);

    return fgets((char *)f[1], (int)f[2], (FILE *)f[3]);
}

static char *int_instr(expr *n)
{
    long *f = v2f_type(long *, frame_ptr);

    return instr((long *)f[1]);
}

static char *int_strcat(expr *n)
{
    long *f = v2f_type(long *, frame_ptr);

    return strcat((char *)f[1], (char *)f[2]);
}

static char *int_strchr(expr *n)
{
    long *f = v2f_type(long *, frame_ptr);

    return strchr((char *)f[1], f[2]);
}

static char *int_strcpy(expr *n)
{
    long *f = v2f_type(long *, frame_ptr);

    return strcpy((char *)f[1], (char *)f[2]);
}

static char *int_strncat(expr *n)
{
    long *f = v2f_type(long *, frame_ptr);

    return strncat((char *)f[1], (char *)f[2], f[3]);
}

static char *int_strncpy(expr *n)
{
    long *f = v2f_type(long *, frame_ptr);

    return strncpy((char *)f[1], (char *)f[2], f[3]);
}

static char *int_strrchr(expr *n)
{
    long *f = v2f_type(long *, frame_ptr);

    return strrchr((char *)f[1], f[2]);
}

static int int_dis(expr *n)
{
    long *f = v2f_type(long *, frame_ptr);

    dis((long *)f[1], f[2]);
    return 0;
}

static int int_errout(expr *n)
{
    long *f = v2f_type(long *, frame_ptr);
    volatile int had_fault;
    int ret = -1;

    BEGIN_PROTECT(&had_fault);
    ret = vfprintf(dexerr, (const unsigned char *)(f[1]), (va_list)(f + 2));
    END_PROTECT();
    if (had_fault)
	print_fault("errout");
    return ret;
}

static int int_fclose(expr *n)
{
    long *f = v2f_type(long *, frame_ptr);

    return fclose((FILE *)f[1]);
}

static int int_fflush(expr *n)
{
    long *f = v2f_type(long *, frame_ptr);

    return fflush((FILE *)f[1]);
}

/*
 * called as: find(void *addr, char **func_name_p, void **start_p,
 *                 char **file_name_p, int *lineno_p);
 */
static int int_find(expr *n)
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

	if (nspace->ns_parent && file_name_p)
	    *file_name_p = f2v_type(char *, nspace->ns_parent->ns_name);
	if (lineno_p && (lines = nspace->ns_lines)) {
	    for (++lines; lines->l_lnno; ++lines)
		if (lines->l_addr.l_paddr > (ulong)qaddr)
		    break;
	    *lineno_p = nspace->ns_lineoffset + lines[-1].l_lnno - 1;
	}
    }
    if (func_name_p)
	*func_name_p = f2v_type(char *, s->s_name);
    if (start_p)
	*start_p = s->s_offset;
    return 1;
}

/*
 * called as: find_name(char *name, long offset, char **file_name_p,
 *                      int *lineno_p);
 */
static int int_find_name(expr *n)
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

    target_addr = (long)s->s_offset + offset;
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

static int int_fprintf(expr *n)
{
    long *f = v2f_type(long *, frame_ptr);
    volatile int had_fault;
    int ret = -1;

    BEGIN_PROTECT(&had_fault);
    ret = vfprintf((FILE *)(f[1]), (const unsigned char *)(f[2]), (va_list)(f + 3));
    END_PROTECT();
    if (had_fault)
	print_fault("fprintf");
    return ret;
}

static int int_load(expr *n)
{
    long *f;
    char *s;

    f = v2f_type(long *, frame_ptr);
    DEBUG_PRINTF(("frame ptr = %s, f = %s\n", P(frame_ptr), P(f)));
    s = (char *)(f[1]);
    return load(s, f[2], f[3]);
}

static int int_pclose(expr *n)
{
    long *f = v2f_type(long *, frame_ptr);

    return pclose((FILE *)f[1]);
}

static int int_printf(expr *n)
{
    long *f = v2f_type(long *, frame_ptr);
    volatile int had_fault;
    int ret = -1;

    BEGIN_PROTECT(&had_fault);
    ret = vfprintf(dexout, (const unsigned char *)(f[1]), (va_list)(f + 2));
    END_PROTECT();
    if (had_fault)
	print_fault("printf");
    return ret;
}

static int int_purge_all_pages(expr *n)
{
    return purge_all_pages();
}

static int int_purge_user_pages(expr *n)
{
    return purge_user_pages();
}

static int int_regs_instr(expr *n)
{
    long *f = v2f_type(long *, frame_ptr);

    return regs_instr((long *)f[1], (int)f[2]);
}

/*
 * Called as: return_range(char *t_name, char *d_name, void **startp, int *lenp)
 */
static int int_return_range(expr *n)
{
    long *f = v2f_type(long *, frame_ptr);
    char *t_name = (char *)f[1];
    char *d_name = (char *)f[2];
    void **startp = (void *)f[3];
    int *lenp = (void *)f[4];

    return return_range(t_name, d_name, startp, lenp);
}

static int int_rmap_dump(expr *n)
{
    rmap_dump();
    return 1;
}

static int int_sprintf(expr *n)
{
    long *f = v2f_type(long *, frame_ptr);
    volatile int had_fault;
    int ret = -1;

    BEGIN_PROTECT(&had_fault);
    ret = vsprintf((char *)(f[1]),
		   (const unsigned char *)(f[2]),
		   (va_list)(f + 3));
    END_PROTECT();
    if (had_fault)
	print_fault("sprintf");
    return ret;
}

static int int_strcmp(expr *n)
{
    long *f = v2f_type(long *, frame_ptr);

    return strcmp((char *)f[1], (char *)f[2]);
}

static int int_strlen(expr *n)
{
    long *f = v2f_type(long *, frame_ptr);

    return strlen((char *)f[1]);
}

static int int_strncmp(expr *n)
{
    long *f = v2f_type(long *, frame_ptr);

    return strncmp((char *)f[1], (char *)f[2], f[3]);
}

static long int_f2v(expr *n)
{
    long *f = v2f_type(long *, frame_ptr);

    return (long)f2v((void *)(f[1]));
}

static long int_name2userdef(expr *n)
{
    long *f = v2f_type(long *, frame_ptr);
    char *name = (char *)f[1];
    return (long)name2userdef_all(name);
}

static long int_v2f(expr *n)
{
    long *f = v2f_type(long *, frame_ptr);

    return (long)v2f((void *)(f[1]));
}

static void int_free(expr *n)
{
    long *f = v2f_type(long *, frame_ptr);

    free((void *)f[1]);
}

static void *int_fopen(expr *n)
{
    long *f = v2f_type(long *, frame_ptr);

    return fopen((char *)f[1], (char *)f[2]);
}

static void *int_malloc(expr *n)
{
    long *f = v2f_type(long *, frame_ptr);

    return malloc((size_t)f[1]);
}

static void *int_popen(expr *n)
{
    long *f = v2f_type(long *, frame_ptr);

    return popen((char *)f[1], (char *)f[2]);
}

static void do_char_ptr_funcs(void)
{
    struct table {
	char *st_name;
	char *(*st_func)(expr *n);
    };
    struct table tab[] = {
	{ "fgets",   int_fgets },
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
	DEBUG_PRINTF(("do_char_ptr_funcs: %s at %d\n", s->s_name,
		      s->s_stmt));
    }
}

static void do_int_funcs(void)
{
    struct table {
	char *st_name;
	int (*st_func)(expr *n);
    };
    struct table tab[] = {
	{ "dis", int_dis },
	{ "errout", int_errout },
	{ "fclose", int_fclose },
	{ "fflush", int_fflush },
	{ "find", int_find },
	{ "find_name", int_find_name },
	{ "fprintf", int_fprintf },
	{ "load", int_load },
	{ "pclose", int_pclose },
	{ "printf", int_printf },
	{ "purge_all_pages", int_purge_all_pages },
	{ "purge_user_pages", int_purge_user_pages },
	{ "regs_instr", int_regs_instr },
	{ "return_range", int_return_range },
	{ "rmap_dump", int_rmap_dump },
	{ "sprintf", int_sprintf },
	{ "strcmp", int_strcmp },
	{ "strlen", int_strlen },
	{ "strncmp", int_strncmp },
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
	DEBUG_PRINTF(("do_int_funcs: %s at %d\n", s->s_name,
		      s->s_stmt));
    }
}

static void do_long_funcs(void)
{
    struct table {
	char *st_name;
	long (*st_func)(expr *n);
    };
    struct table tab[] = {
	{ "f2v",          int_f2v },
	{ "name2userdef", int_name2userdef },
	{ "v2f",          int_v2f }
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
	DEBUG_PRINTF(("do_long_funcs: %s at %d\n", s->s_name,
		      s->s_stmt));
    }
}

static void do_void_funcs(void)
{
    struct table {
	char *st_name;
	void (*st_func)(expr *n);
    };
    struct table tab[] = {
	{ "free", int_free }
    };
    struct table *tp, *tp_end;
    typeptr t = newtype(ns_inter, PROC_TYPE);

    t->t_val.val_f.f_typeptr = find_type(ns_inter, TP_VOID);
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
	e->e_func.v = tp->st_func;
	DEBUG_PRINTF(("do_void_funcs: %s at %d\n", s->s_name,
		      s->s_stmt));
    }
}

static void do_void_ptr_funcs(void)
{
    struct table {
	void *st_name;
	void *(*st_func)(expr *n);
    };
    struct table tab[] = {
	{ "fopen",  int_fopen },
	{ "malloc", int_malloc },
	{ "popen",  int_popen }
    };
    struct table *tp, *tp_end;
    typeptr t = newtype(ns_inter, PROC_TYPE);

    t->t_val.val_f.f_typeptr = newtype(ns_inter, PTR_TYPE);
    t->t_val.val_f.f_params = -1;
    t->t_val.val_f.f_paramlist = 0;
    t->t_val.val_f.f_typeptr->t_val.val_p = find_type(ns_inter, TP_VOID);

    for (tp_end = (tp = tab) + A_SIZE(tab); tp < tp_end; ++tp) {
	void *name = store_string(ns_inter, tp->st_name, 0, (void *)0);
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
	DEBUG_PRINTF(("do_void_ptr_funcs: %s at %d\n", s->s_name,
		      s->s_stmt));
    }
}

static void do_int_vars(void)
{
    struct table {
	char *st_name;
	int *st_func;
    };
    struct table tab[] = {
	{ "fromdump",     &fromdump },
	{ "real_mode",    &real_mode },
	{ "thread_slot",  &thread_slot },
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
	DEBUG_PRINTF(("do_int_vars: %s at 0x%0*lx\n", s->s_name,
		      sizeof(s->s_offset)*2, s->s_offset));
    }
}

static void do_ulong_vars(void)
{
    struct table {
	char *st_name;
	unsigned long *st_func;
    };
    struct table tab[] = {
	{ "bos_segval", &bos_segval },
	{ "debug_mask", &debug_mask },
	{ "inv_segval", &inv_segval }
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
	s->s_size = sizeof(unsigned long);
	s->s_offset = f2v(tp->st_func);
	DEBUG_PRINTF(("do_ulong_vars: %s at 0x%0*lx\n", s->s_name,
		      sizeof(s->s_offset)*2, s->s_offset));
    }
}

static void do_void_ptr_vars(void)
{
    struct table {
	char *st_name;
	void *st_func;
    };
    struct table tab[] = {
	{ "dexin",  &dexin  },
	{ "dexout", &dexout },
	{ "dexerr", &dexerr }
    };

    struct table *tp, *tp_end;
    typeptr t = newtype(ns_inter, PTR_TYPE);

    t->t_val.val_f.f_typeptr = find_type(ns_inter, TP_VOID);


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
	s->s_size = sizeof(void *);
	s->s_offset = f2v(tp->st_func);
	DEBUG_PRINTF(("do_void_ptr_vars: %s at 0x%0*lx\n", s->s_name,
		      sizeof(s->s_offset)*2, s->s_offset));
    }
}

void builtin_init(void)
{
    do_char_ptr_funcs();
    do_int_funcs();
    do_long_funcs();
    do_void_funcs();
    do_void_ptr_funcs();
    do_int_vars();
    do_ulong_vars();
    do_void_ptr_vars();
}
