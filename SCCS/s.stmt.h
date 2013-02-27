h11132
s 00009/00000/00063
d D 1.6 02/03/14 16:12:34 pedz 6 5
e
s 00000/00000/00063
d D 1.5 00/09/18 17:57:03 pedz 5 4
c Checking before V5 conversion
e
s 00002/00000/00061
d D 1.4 00/05/29 15:09:08 pedz 4 3
c Just before IA64 conversion
e
s 00001/00000/00060
d D 1.3 95/02/09 21:35:59 pedz 3 2
e
s 00022/00024/00038
d D 1.2 95/02/01 10:37:08 pedz 2 1
e
s 00062/00000/00000
d D 1.1 94/08/22 17:56:35 pedz 1 0
c date and time created 94/08/22 17:56:35 by pedz
e
u
U
t
T
I 1

/* %W% */

I 6
#ifndef __STMT_H
#define __STMT_H

E 6
/*
 * Rather than trying to minimize space, the structure for statements
 * is a straight forward union for each statement type.  Statements
 * are somewhat pseudo code in that they are executed sequentially
 * unless there is an explicit goto.  A break and continue statement
 * are replaced with goto's to the proper place.  Note also that a
 * "compound statement" disappears in the structure of statements.
 */

enum stmt_type {
    NULL_STMT, EXPR_STMT, SWITCH_STMT, GOTO_STMT, BR_TRUE, BR_FALSE,
    RETURN_STMT, ALLOC_STMT, PRINT_STMT
    };
#define LAST_STMT PRINT_STMT

D 2
typedef unsigned int stmt;

E 2
struct stmt {
    enum stmt_type stmt_type;
    union {
	struct {			/* non-print statements */
	    expr *np_expr;
D 2
	    stmt np_stmt;
E 2
I 2
	    stmt_index np_stmt;
E 2
	} su_non_print;
#define stmt_expr st_un.su_non_print.np_expr
#define stmt_stmt st_un.su_non_print.np_stmt

	cnode su_print;			/* print statements */
#define stmt_print st_un.su_print

	int su_alloc;			/* alloc statements */
#define stmt_alloc st_un.su_alloc
I 3

E 3
    } st_un;
I 4
    char *stmt_file;
    int stmt_line;
E 4
};

D 2
#define NO_STMT ((stmt)0)
E 2
I 2
struct stmt *statements;
I 6
#define STMT_STACK_SIZE 1024
int stmt_stack[STMT_STACK_SIZE];
int cur_stmt_index;
E 6
E 2

D 2
expr *execute_statement(stmt s);
stmt get_current_stmt(void);
stmt mk_expr_stmt(expr *e);
stmt mk_switch_stmt(expr *e);
stmt mk_goto_stmt(stmt s);
stmt mk_br_true(expr *e, stmt s);
stmt mk_br_false(expr *e, stmt s);
stmt mk_return_stmt(expr *e);
void link_stmt(stmt from_stmt, stmt to_stmt);
stmt mk_add_break_stmt(void);
stmt mk_add_cont_stmt(void);
stmt mk_add_goto_stmt(char *id);
void mk_goto_def(stmt s, char *id);
E 2
I 2
expr *execute_statement(stmt_index s);
stmt_index get_current_stmt(void);
stmt_index mk_expr_stmt(expr *e);
stmt_index mk_switch_stmt(expr *e);
stmt_index mk_goto_stmt(stmt_index s);
stmt_index mk_br_true(expr *e, stmt_index s);
stmt_index mk_br_false(expr *e, stmt_index s);
stmt_index mk_return_stmt(expr *e);
void link_stmt(stmt_index from_stmt, stmt_index to_stmt);
stmt_index mk_add_break_stmt(void);
stmt_index mk_add_cont_stmt(void);
stmt_index mk_add_goto_stmt(char *id);
void mk_goto_def(stmt_index s, char *id);
E 2
void resolv_gotos(void);
D 2
stmt mk_print_stmt(cnode *c);
void push_breaks(stmt s);
void push_conts(stmt s);
stmt link_breaks(void);
stmt link_conts(void);
E 2
I 2
stmt_index mk_alloc_stmt(void);
stmt_index mk_print_stmt(cnode *c);
void push_breaks(stmt_index s);
void push_conts(stmt_index s);
stmt_index link_breaks(void);
stmt_index link_conts(void);
E 2
void dump_stmts(void);
D 2

struct stmt *statements;
E 2
I 2
void set_alloc(stmt_index s, int alloc);
I 6
void fail(int err);

#endif /* __STMT_H */
E 6
E 2
E 1
