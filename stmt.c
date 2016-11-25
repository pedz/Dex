static char sccs_id[] = "@(#)stmt.c	1.8";

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include "map.h"
#include "sym.h"
#include "tree.h"
#include "stmt.h"
#include "dex.h"
#include "scan_pre.h"
#include "fcall.h"
#define DEBUG_BIT STMT_C_BIT

/*
 * Each statement that can have a break (or a continue) calls
 * push_breaks to start the new level with the statement number.  This
 * call creates a nesting structure.  Then calls to mk_add_break_stmt
 * create a link structure which is added to n_list and has the
 * statement number for the goto in the link.  When link_breaks is
 * finally called, the n_list is traversed and the proper statement
 * number is put in.
 *
 * The continues create nesting levels but the goto statements are
 * resolved at the time the mk_cont_stmt is called.  link_conts must
 * still be called to pop the list of link structures.
 */
struct nesting {
    struct nesting *n_next;
    struct link *n_link;
    stmt_index n_stmt;
};
struct link {
    struct link *l_list;		/* list of stmts to fix */
    stmt_index l_stmt;			/* statement to fix */
};
static struct nesting *breaks;
static struct nesting *conts;

struct goto_ref {			/* used for "goto dog;" */
    struct goto_ref *gr_next;
    stmt_index gr_stmt;
    char *gr_id;
};
struct goto_ref *goto_refs;

struct goto_def {			/* used for "dog: i++;" */
    struct goto_def *gd_next;
    stmt_index gd_stmt;
    char *gd_id;
};
struct goto_def *goto_defs;

struct stmt *statements;
static stmt_index current_stmt = 1;
static stmt_index current_max;

static stmt_index next_stmt(enum stmt_type t)
{
    struct stmt *s;

    if (!statements)
	statements = smalloc(sizeof(struct stmt) * (current_max = 100));
    if (current_stmt == current_max) {
	size_t old = current_max * sizeof(struct stmt);
	size_t new = (current_max += 100) * sizeof(struct stmt);
	statements = srealloc(statements, new, old);
    }

    s = statements + current_stmt;
    s->stmt_type = t;
    s->stmt_file = yyfilename;
    s->stmt_line = yylineno;
    return current_stmt++;
}

static struct nesting *new_nesting(void)
{
    return new(struct nesting);
}

static struct link *new_link(void)
{
    return new(struct link);
}

expr *execute_statement(stmt_index s)
{
    struct stmt *sp;
    expr *e;
    all a;

    if (s == NO_STMT)
	return 0;

    sp = statements + s;
    for ( ; ; ) {
	stmt_stack[cur_stmt_index] = sp - statements;
	switch (sp->stmt_type) {
	case NULL_STMT:
	    sp++;
	    break;

	case EXPR_STMT:
	    if (e = sp->stmt_expr)
		(void)i_val(e);
	    sp++;
	    break;

	case SWITCH_STMT:
	    break;

	case GOTO_STMT:
	    sp = statements + sp->stmt_stmt;
	    break;

	case BR_TRUE:
	    if (i_val(sp->stmt_expr))
		sp = statements + sp->stmt_stmt;
	    else
		++sp;
	    break;

	case BR_FALSE:
	    if (i_val(sp->stmt_expr))
		++sp;
	    else
		sp = statements + sp->stmt_stmt;
	    break;

	case RETURN_STMT:
	    return sp->stmt_expr;

	case ALLOC_STMT:
	    alloc_stack(sp->stmt_alloc);
	    ++sp;
	    break;

	case PRINT_STMT:
	    print_expression(&sp->stmt_print);
	    ++sp;
	    break;
	}
    }
}

stmt_index get_current_stmt(void)
{
    return current_stmt;
}

stmt_index mk_expr_stmt(expr *e)
{
    stmt_index ret = next_stmt(EXPR_STMT);

    statements[ret].stmt_expr = e;
    return ret;
}

stmt_index mk_switch_stmt(expr *e)
{
    stmt_index ret = next_stmt(SWITCH_STMT);

    statements[ret].stmt_expr = e;
    return ret;
}

stmt_index mk_goto_stmt(stmt_index s)
{
    stmt_index ret = next_stmt(GOTO_STMT);

    statements[ret].stmt_stmt = (s == NO_STMT) ? current_stmt : s;
    return ret;
}

stmt_index mk_br_true(expr *e, stmt_index s)
{
    stmt_index ret = next_stmt(BR_TRUE);
    struct stmt *sp = statements + ret;

    sp->stmt_expr = e;
    sp->stmt_stmt = (s == NO_STMT) ? current_stmt : s;
    return ret;
}

stmt_index mk_br_false(expr *e, stmt_index s)
{
    stmt_index ret = next_stmt(BR_FALSE);
    struct stmt *sp = statements + ret;

    sp->stmt_expr = e;
    sp->stmt_stmt = (s == NO_STMT) ? current_stmt : s;
    return ret;
}

stmt_index mk_return_stmt(expr *e)
{
    stmt_index ret = next_stmt(RETURN_STMT);

    statements[ret].stmt_expr = e;
    return ret;
}

void link_stmt(stmt_index from_stmt, stmt_index to_stmt)
{
    statements[from_stmt].stmt_stmt =
	(to_stmt == NO_STMT) ? current_stmt : to_stmt;
}

stmt_index mk_add_break_stmt(void)
{
    stmt_index ret = mk_goto_stmt(NO_STMT);
    struct link *l = new_link();

    l->l_stmt = ret;
    l->l_list = breaks->n_link;
    breaks->n_link = l;
    return ret;
}

stmt_index mk_add_cont_stmt(void)
{
    stmt_index ret = mk_goto_stmt(NO_STMT);
    struct link *l = new_link();

    l->l_stmt = ret;
    l->l_list = conts->n_link;
    conts->n_link = l;
    return ret;
}

stmt_index mk_add_goto_stmt(char *id)
{
    stmt_index ret = mk_goto_stmt(NO_STMT);
    struct goto_ref *ref = new(struct goto_ref);

    ref->gr_next = goto_refs;
    goto_refs = ref;
    ref->gr_stmt = ret;
    ref->gr_id = id;
    return ret;
}

void mk_goto_def(stmt_index s, char *id)
{
    struct goto_def *def = new(struct goto_def);

    def->gd_next = goto_defs;
    goto_defs = def;
    def->gd_stmt = s;
    def->gd_id = id;
}

void resolv_gotos(void)
{
    struct goto_def *def;
    struct goto_ref *ref;

    /* Link up all references */
    while (ref = goto_refs) {
	for (def = goto_defs;
	     def && strcmp(def->gd_id, ref->gr_id);
	     def = def->gd_next);
	if (def)
	    statements[ref->gr_stmt].stmt_stmt = def->gd_stmt;
	else
	    fprintf(stderr, "Label %s referenced but never defined\n",
		    ref->gr_id);

	goto_refs = ref->gr_next;
	free(ref);
    }
    /* Free all definitions */
    while (def = goto_defs) {
	goto_defs = def->gd_next;
	free(def);
    }
}

stmt_index mk_alloc_stmt(void)
{
    return next_stmt(ALLOC_STMT);
}

stmt_index mk_print_stmt(cnode *c)
{
    stmt_index ret = next_stmt(PRINT_STMT);

    statements[ret].stmt_print = *c;
    return ret;
}

void push_breaks(stmt_index s)
{
    struct nesting *n = new_nesting();

    n->n_next = breaks;
    n->n_stmt = s;
    breaks = n;
}

void push_conts(stmt_index s)
{
    struct nesting *n = new_nesting();

    n->n_next = conts;
    n->n_stmt = s;
    conts = n;
}

stmt_index link_breaks(void)
{
    struct nesting *t = breaks;
    stmt_index ret = t->n_stmt;
    struct link *l = t->n_link;

    while (l) {
	struct link *lt = l->l_list;

	statements[l->l_stmt].stmt_stmt = current_stmt;
	free(l);
	l = lt;
    }

    breaks = t->n_next;
    free(t);
    return ret;
}

stmt_index link_conts(void)
{
    struct nesting *t = conts;
    stmt_index ret = t->n_stmt;
    struct link *l = t->n_link;

    while (l) {
	struct link *lt = l->l_list;

	statements[l->l_stmt].stmt_stmt = current_stmt;
	free(l);
	l = lt;
    }

    conts = t->n_next;
    free(t);
    return ret;
}

static char *stmt_names[LAST_STMT + 1];

static void dump_init(void)
{
    stmt_names[NULL_STMT] = "%3d: null\n";
    stmt_names[EXPR_STMT] = "%3d: expr 0x%s\n";
    stmt_names[SWITCH_STMT] = "%3d: switch (0x%s) <%3d>\n";
    stmt_names[GOTO_STMT] = "%3d: goto <%3d>\n";
    stmt_names[BR_TRUE] = "%3d: if (0x%s) <%3d>\n";
    stmt_names[BR_FALSE] = "%3d: if NOT (0x%s) <%3d>\n";
    stmt_names[RETURN_STMT] = "%3d: return (0x%s)\n";
    stmt_names[ALLOC_STMT] = "%3d: alloc %d\n";
    stmt_names[PRINT_STMT] = "%3d: print %s (0x%s)\n";
}

void print_stmt(FILE *f, struct stmt *s)
{
    int i = s - statements;

    if (!stmt_names[PRINT_STMT])
	dump_init();

    switch (s->stmt_type) {
    case NULL_STMT:
	break;

    case ALLOC_STMT:
	fprintf(f, stmt_names[s->stmt_type], i, s->stmt_alloc);
	break;

    case GOTO_STMT:
	fprintf(f, stmt_names[s->stmt_type], i, s->stmt_stmt);
	break;

    case EXPR_STMT:
    case SWITCH_STMT:
    case BR_TRUE:
    case BR_FALSE:
    case RETURN_STMT:
	fprintf(f, stmt_names[s->stmt_type], i, P(s->stmt_expr), s->stmt_stmt);
	if (s->stmt_expr)
	    tree_dump(f, s->stmt_expr);
	break;

    case PRINT_STMT:
	fprintf(f, stmt_names[s->stmt_type], i,
	       type_2_string[s->stmt_print.c_base], P(s->stmt_print.c_expr));
	if (s->stmt_print.c_expr)
	    tree_dump(f, s->stmt_print.c_expr);
	break;
    }
}

void dump_stmts(void)
{
    struct stmt *s = statements + 1;
    struct stmt *send = statements + current_stmt;

    for ( ; s < send; ++s)
	print_stmt(stdout, s);
}

void set_alloc(stmt_index s, int alloc)
{
    statements[s].stmt_alloc = alloc;
}

void dump_stmt_stack(FILE *f)
{
    struct stmt *s;

    fprintf(f, "Failure trace back\n");
    while (cur_stmt_index >= 0) {
	s = statements + stmt_stack[cur_stmt_index];
	--cur_stmt_index;
	fprintf(f, "Line %d of %s: ", s->stmt_line, s->stmt_file);
	print_stmt(f, s);
    }
}

extern long frame_ptr;
extern long stack_ptr;
extern long last_v;

void fail(int err)
{
    static int in_fail;

    if (in_fail)
	exit(err);

    in_fail = 1;
    dump_stmt_stack(stderr);
    fprintf(stderr, "frame=%s stack=%s\n", P(frame_ptr), P(stack_ptr));
    fprintf(stderr, "map_top = %s thread_slot = %d\n", P(map_top),
	    thread_slot);
    trace_mappings();
    exit(err);
}
