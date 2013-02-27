h42864
s 00082/00049/00359
d D 1.8 02/03/14 16:12:34 pedz 8 7
e
s 00000/00000/00408
d D 1.7 00/09/18 17:57:02 pedz 7 6
c Checking before V5 conversion
e
s 00006/00001/00402
d D 1.6 00/05/29 15:09:08 pedz 6 5
c Just before IA64 conversion
e
s 00002/00004/00401
d D 1.5 97/05/13 16:02:59 pedz 5 4
c Check pointer
e
s 00002/00001/00403
d D 1.4 95/04/25 10:10:21 pedz 4 3
e
s 00004/00006/00400
d D 1.3 95/02/09 21:35:57 pedz 3 2
e
s 00076/00046/00330
d D 1.2 95/02/01 10:37:06 pedz 2 1
e
s 00376/00000/00000
d D 1.1 94/08/22 17:56:35 pedz 1 0
c date and time created 94/08/22 17:56:35 by pedz
e
u
U
t
T
I 1
static char sccs_id[] = "%W%";

D 2
/*
 * TODO's
 * #### switch statements
 */
E 2
#include <stdio.h>
#include <stdlib.h>
I 8
#include <strings.h>
E 8
#include "map.h"
#include "sym.h"
#include "tree.h"
#include "stmt.h"
#include "dex.h"
I 6
D 8
#include "scan.h"
E 8
I 8
#include "scan_pre.h"
#include "fcall.h"
#define DEBUG_BIT STMT_C_BIT
E 8
E 6

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
D 2
    stmt n_stmt;
E 2
I 2
    stmt_index n_stmt;
E 2
};
struct link {
    struct link *l_list;		/* list of stmts to fix */
D 2
    stmt l_stmt;			/* statement to fix */
E 2
I 2
    stmt_index l_stmt;			/* statement to fix */
E 2
};
static struct nesting *breaks;
static struct nesting *conts;

struct goto_ref {			/* used for "goto dog;" */
    struct goto_ref *gr_next;
D 2
    stmt gr_stmt;
E 2
I 2
    stmt_index gr_stmt;
E 2
    char *gr_id;
};
struct goto_ref *goto_refs;

struct goto_def {			/* used for "dog: i++;" */
    struct goto_def *gd_next;
D 2
    stmt gd_stmt;
E 2
I 2
    stmt_index gd_stmt;
E 2
    char *gd_id;
};
struct goto_def *goto_defs;
    
struct stmt *statements;
D 2
static stmt current_stmt = 1;
static stmt current_max;
E 2
I 2
static stmt_index current_stmt = 1;
static stmt_index current_max;
E 2

D 2
static stmt next_stmt(enum stmt_type t)
E 2
I 2
static stmt_index next_stmt(enum stmt_type t)
E 2
{
I 6
    struct stmt *s;
E 6

    if (!statements)
D 5
	statements = smalloc(sizeof(struct stmt) * (current_max = 100),
			     __FILE__, __LINE__);
E 5
I 5
	statements = smalloc(sizeof(struct stmt) * (current_max = 100));
E 5
D 3
    if (current_stmt == current_max &&
	!(statements = realloc(statements,
			       sizeof(struct stmt) * (current_max += 100)))) {
	fprintf(stderr, "Out of memory for realloc in next_stmt\n");
	exit(1);
    }
E 3
I 3
D 8
    if (current_stmt == current_max)
	statements = srealloc(statements,
D 5
			      sizeof(struct stmt) * (current_max += 100),
			      __FILE__, __LINE__);
E 5
I 5
			      sizeof(struct stmt) * (current_max += 100));
E 8
I 8
    if (current_stmt == current_max) {
	size_t old = current_max * sizeof(struct stmt);
	size_t new = (current_max += 100) * sizeof(struct stmt);
	statements = srealloc(statements, new, old);
    }

E 8
E 5
E 3
D 6
    statements[current_stmt].stmt_type = t;
E 6
I 6
    s = statements + current_stmt;
    s->stmt_type = t;
    s->stmt_file = yyfilename;
    s->stmt_line = yylineno;
E 6
    return current_stmt++;
}

static struct nesting *new_nesting(void)
{
D 8
    struct nesting *ret = new(struct nesting);

    bzero(ret, sizeof(*ret));
    return ret;
E 8
I 8
    return new(struct nesting);
E 8
}

static struct link *new_link(void)
{
D 8
    struct link *ret = new(struct link);

    bzero(ret, sizeof(*ret));
    return ret;
E 8
I 8
    return new(struct link);
E 8
}

D 2
expr *execute_statement(stmt s)
E 2
I 2
expr *execute_statement(stmt_index s)
E 2
{
    struct stmt *sp;
    expr *e;
    all a;

    if (s == NO_STMT)
	return 0;

    sp = statements + s;
D 4
    for ( ; ; )
E 4
I 4
    for ( ; ; ) {
I 8
	stmt_stack[cur_stmt_index] = sp - statements;
E 8
E 4
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
I 4
    }
E 4
}

D 2
stmt get_current_stmt(void)
E 2
I 2
stmt_index get_current_stmt(void)
E 2
{
    return current_stmt;
}

D 2
stmt mk_expr_stmt(expr *e)
E 2
I 2
stmt_index mk_expr_stmt(expr *e)
E 2
{
D 2
    stmt ret = next_stmt(EXPR_STMT);
E 2
I 2
    stmt_index ret = next_stmt(EXPR_STMT);
E 2

    statements[ret].stmt_expr = e;
    return ret;
}

D 2
stmt mk_switch_stmt(expr *e)
E 2
I 2
stmt_index mk_switch_stmt(expr *e)
E 2
{
D 2
    stmt ret = next_stmt(SWITCH_STMT);
E 2
I 2
    stmt_index ret = next_stmt(SWITCH_STMT);
E 2

    statements[ret].stmt_expr = e;
    return ret;
}

D 2
stmt mk_goto_stmt(stmt s)
E 2
I 2
stmt_index mk_goto_stmt(stmt_index s)
E 2
{
D 2
    stmt ret = next_stmt(GOTO_STMT);
E 2
I 2
    stmt_index ret = next_stmt(GOTO_STMT);
E 2

    statements[ret].stmt_stmt = (s == NO_STMT) ? current_stmt : s;
    return ret;
}

D 2
stmt mk_br_true(expr *e, stmt s)
E 2
I 2
stmt_index mk_br_true(expr *e, stmt_index s)
E 2
{
D 2
    stmt ret = next_stmt(BR_TRUE);
E 2
I 2
    stmt_index ret = next_stmt(BR_TRUE);
E 2
    struct stmt *sp = statements + ret;

    sp->stmt_expr = e;
    sp->stmt_stmt = (s == NO_STMT) ? current_stmt : s;
    return ret;
}

D 2
stmt mk_br_false(expr *e, stmt s)
E 2
I 2
stmt_index mk_br_false(expr *e, stmt_index s)
E 2
{
D 2
    stmt ret = next_stmt(BR_FALSE);
E 2
I 2
    stmt_index ret = next_stmt(BR_FALSE);
E 2
    struct stmt *sp = statements + ret;

    sp->stmt_expr = e;
    sp->stmt_stmt = (s == NO_STMT) ? current_stmt : s;
    return ret;
}

D 2
stmt mk_return_stmt(expr *e)
E 2
I 2
stmt_index mk_return_stmt(expr *e)
E 2
{
D 2
    stmt ret = next_stmt(RETURN_STMT);
E 2
I 2
    stmt_index ret = next_stmt(RETURN_STMT);
E 2

    statements[ret].stmt_expr = e;
    return ret;
}

D 2
void link_stmt(stmt from_stmt, stmt to_stmt)
E 2
I 2
void link_stmt(stmt_index from_stmt, stmt_index to_stmt)
E 2
{
    statements[from_stmt].stmt_stmt =
	(to_stmt == NO_STMT) ? current_stmt : to_stmt;
}

D 2
stmt mk_add_break_stmt(void)
E 2
I 2
stmt_index mk_add_break_stmt(void)
E 2
{
D 2
    stmt ret = mk_goto_stmt(NO_STMT);
E 2
I 2
    stmt_index ret = mk_goto_stmt(NO_STMT);
E 2
    struct link *l = new_link();

    l->l_stmt = ret;
    l->l_list = breaks->n_link;
    breaks->n_link = l;
    return ret;
}

D 2
stmt mk_add_cont_stmt(void)
E 2
I 2
stmt_index mk_add_cont_stmt(void)
E 2
{
D 2
    stmt ret = mk_goto_stmt(NO_STMT);
E 2
I 2
    stmt_index ret = mk_goto_stmt(NO_STMT);
E 2
    struct link *l = new_link();

    l->l_stmt = ret;
    l->l_list = conts->n_link;
    conts->n_link = l;
    return ret;
}

D 2
stmt mk_add_goto_stmt(char *id)
E 2
I 2
stmt_index mk_add_goto_stmt(char *id)
E 2
{
D 2
    stmt ret = mk_goto_stmt(NO_STMT);
E 2
I 2
    stmt_index ret = mk_goto_stmt(NO_STMT);
E 2
    struct goto_ref *ref = new(struct goto_ref);

    ref->gr_next = goto_refs;
    goto_refs = ref;
    ref->gr_stmt = ret;
    ref->gr_id = id;
    return ret;
}

D 2
void mk_goto_def(stmt s, char *id)
E 2
I 2
void mk_goto_def(stmt_index s, char *id)
E 2
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

D 2
stmt mk_alloc_stmt(void)
E 2
I 2
stmt_index mk_alloc_stmt(void)
E 2
{
    return next_stmt(ALLOC_STMT);
}

D 2
stmt mk_print_stmt(cnode *c)
E 2
I 2
stmt_index mk_print_stmt(cnode *c)
E 2
{
D 2
    stmt ret = next_stmt(PRINT_STMT);
E 2
I 2
    stmt_index ret = next_stmt(PRINT_STMT);
E 2

    statements[ret].stmt_print = *c;
    return ret;
}

D 2
void push_breaks(stmt s)
E 2
I 2
void push_breaks(stmt_index s)
E 2
{
    struct nesting *n = new_nesting();
    
    n->n_next = breaks;
    n->n_stmt = s;
    breaks = n;
}

D 2
void push_conts(stmt s)
E 2
I 2
void push_conts(stmt_index s)
E 2
{
    struct nesting *n = new_nesting();
    
    n->n_next = conts;
    n->n_stmt = s;
    conts = n;
}

D 2
stmt link_breaks(void)
E 2
I 2
stmt_index link_breaks(void)
E 2
{
    struct nesting *t = breaks;
D 2
    stmt ret = t->n_stmt;
E 2
I 2
    stmt_index ret = t->n_stmt;
E 2
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

D 2
stmt link_conts(void)
E 2
I 2
stmt_index link_conts(void)
E 2
{
    struct nesting *t = conts;
D 2
    stmt ret = t->n_stmt;
E 2
I 2
    stmt_index ret = t->n_stmt;
E 2
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
D 8
    stmt_names[EXPR_STMT] = "%3d: expr 0x%08x\n";
    stmt_names[SWITCH_STMT] = "%3d: switch (0x%08x) <%3d>\n";
E 8
I 8
    stmt_names[EXPR_STMT] = "%3d: expr 0x%s\n";
    stmt_names[SWITCH_STMT] = "%3d: switch (0x%s) <%3d>\n";
E 8
D 2
    stmt_names[GOTO_STMT] = "%3d: %.0sgoto <%3d>\n";
E 2
I 2
    stmt_names[GOTO_STMT] = "%3d: goto <%3d>\n";
E 2
D 8
    stmt_names[BR_TRUE] = "%3d: if (0x%08x) <%3d>\n";
    stmt_names[BR_FALSE] = "%3d: if NOT (0x%08x) <%3d>\n";
    stmt_names[RETURN_STMT] = "%3d: return (0x%08x)\n";
E 8
I 8
    stmt_names[BR_TRUE] = "%3d: if (0x%s) <%3d>\n";
    stmt_names[BR_FALSE] = "%3d: if NOT (0x%s) <%3d>\n";
    stmt_names[RETURN_STMT] = "%3d: return (0x%s)\n";
E 8
    stmt_names[ALLOC_STMT] = "%3d: alloc %d\n";
D 2
    stmt_names[PRINT_STMT] = "%3d: print (0x%08x)\n";
E 2
I 2
D 8
    stmt_names[PRINT_STMT] = "%3d: print %s (0x%08x)\n";
E 8
I 8
    stmt_names[PRINT_STMT] = "%3d: print %s (0x%s)\n";
E 8
E 2
}

D 8
void dump_stmts(void)
E 8
I 8
void print_stmt(FILE *f, struct stmt *s)
E 8
{
D 8
    int i = 1;
    struct stmt *s = statements + i;
    struct stmt *send = statements + current_stmt;
E 8
I 8
    int i = s - statements;
E 8

D 8
    if (!stmt_names[0])
E 8
I 8
    if (!stmt_names[PRINT_STMT])
E 8
	dump_init();

D 2
    for ( ; s < send; ++s, ++i)
	printf(stmt_names[s->stmt_type], i, s->stmt_expr, s->stmt_stmt);
E 2
I 2
D 8
    for ( ; s < send; ++s, ++i) {
	switch (s->stmt_type) {
	case NULL_STMT:
	    break;
E 8
I 8
    switch (s->stmt_type) {
    case NULL_STMT:
	break;
E 8

D 8
	case ALLOC_STMT:
	    printf(stmt_names[s->stmt_type], i, s->stmt_alloc);
	    break;
E 8
I 8
    case ALLOC_STMT:
	fprintf(f, stmt_names[s->stmt_type], i, s->stmt_alloc);
	break;
E 8

D 8
	case GOTO_STMT:
	    printf(stmt_names[s->stmt_type], i, s->stmt_stmt);
	    break;
E 8
I 8
    case GOTO_STMT:
	fprintf(f, stmt_names[s->stmt_type], i, s->stmt_stmt);
	break;
E 8

D 8
	case EXPR_STMT:
	case SWITCH_STMT:
	case BR_TRUE:
	case BR_FALSE:
	case RETURN_STMT:
	    printf(stmt_names[s->stmt_type], i, s->stmt_expr, s->stmt_stmt);
	    if (s->stmt_expr)
		tree_dump(s->stmt_expr);
	    break;
E 8
I 8
    case EXPR_STMT:
    case SWITCH_STMT:
    case BR_TRUE:
    case BR_FALSE:
    case RETURN_STMT:
	fprintf(f, stmt_names[s->stmt_type], i, P(s->stmt_expr), s->stmt_stmt);
	if (s->stmt_expr)
	    tree_dump(f, s->stmt_expr);
	break;
E 8

D 8
	case PRINT_STMT:
	    printf(stmt_names[s->stmt_type], i,
		   type_2_string[s->stmt_print.c_base], s->stmt_print.c_expr);
	    if (s->stmt_print.c_expr)
		tree_dump(s->stmt_print.c_expr);
	    break;
	}
E 8
I 8
    case PRINT_STMT:
	fprintf(f, stmt_names[s->stmt_type], i,
	       type_2_string[s->stmt_print.c_base], P(s->stmt_print.c_expr));
	if (s->stmt_print.c_expr)
	    tree_dump(f, s->stmt_print.c_expr);
	break;
E 8
    }
}

I 8
void dump_stmts(void)
{
    struct stmt *s = statements + 1;
    struct stmt *send = statements + current_stmt;

    for ( ; s < send; ++s)
	print_stmt(stdout, s);
}

E 8
void set_alloc(stmt_index s, int alloc)
{
    statements[s].stmt_alloc = alloc;
I 8
}

extern long frame_ptr;
extern long stack_ptr;
extern long last_v;

void fail(int err)
{
    static int in_fail;
    struct stmt *s;

    if (in_fail)
	exit(err);

    in_fail = 1;
    fprintf(stderr, "Failure trace back\n");
    while (cur_stmt_index >= 0) {
	s = statements + stmt_stack[cur_stmt_index];
	--cur_stmt_index;
	fprintf(stderr, "Line %d of %s: ", s->stmt_line, s->stmt_file);
	print_stmt(stderr, s);
    }
    fprintf(stderr, "frame=%s stack=%s\n", P(frame_ptr), P(stack_ptr));
    fprintf(stderr, "map_top = %s thread_slot = %d\n", P(map_top),
	    thread_slot);
    trace_mappings();
    exit(err);
E 8
E 2
}
E 1
