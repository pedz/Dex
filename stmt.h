
/* @(#)stmt.h	1.1 */

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

typedef unsigned int stmt;

struct stmt {
    enum stmt_type stmt_type;
    union {
	struct {			/* non-print statements */
	    expr *np_expr;
	    stmt np_stmt;
	} su_non_print;
#define stmt_expr st_un.su_non_print.np_expr
#define stmt_stmt st_un.su_non_print.np_stmt

	cnode su_print;			/* print statements */
#define stmt_print st_un.su_print

	int su_alloc;			/* alloc statements */
#define stmt_alloc st_un.su_alloc
    } st_un;
};

#define NO_STMT ((stmt)0)

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
void resolv_gotos(void);
stmt mk_print_stmt(cnode *c);
void push_breaks(stmt s);
void push_conts(stmt s);
stmt link_breaks(void);
stmt link_conts(void);
void dump_stmts(void);

struct stmt *statements;
