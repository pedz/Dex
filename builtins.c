static char sccs_id[] = "@(#)builtins.c	1.1";

#include <stdio.h>
#include <stdlib.h>
#include <sys/signal.h>
#include "map.h"
#include "sym.h"
#include "tree.h"
#include "stmt.h"
#include "gram.h"
#include "asgn_expr.h"
#include "base_expr.h"
#include "inter.h"
#include "dex.h"
#include "fcall.h"

static int int_printf(expr *n)
{
    long *f = v2f_type(long *, frame_ptr);

    vprintf((const unsigned char *)(f[1]), (va_list)(f + 2));
    return 4;
}

void extra_hack()
{
    char *name = store_string(ns_inter, "printf", 0, (char *)0);
    symptr s = enter_sym(ns_inter, name);
    typeptr t = newtype(ns_inter, PROC_TYPE);
    expr *e = new_expr();
    
    t->t_val.val_f.f_typeptr = name2typedef(ns_inter, "int");
    t->t_val.val_f.f_params = -1;
    t->t_val.val_f.f_paramlist = 0;
    s->s_defined = 1;
    s->s_base = base_type(t);
    s->s_type = t;
    s->s_nesting = 0;
    s->s_typed = 1;
    s->s_global = 1;
    s->s_size = 0;
    s->s_stmt = mk_return_stmt(e);
    e->e_func.i = int_printf;
    e->e_i = 55;
    e->e_size = 0;
}
