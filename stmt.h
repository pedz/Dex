
/* @(#)stmt.h	1.2 */

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

struct stmt {
    enum stmt_type stmt_type;
    union {
	struct {			/* non-print statements */
	    expr *np_expr;
	    stmt_index np_stmt;
	} su_non_print;
#define stmt_expr st_un.su_non_print.np_expr
#define stmt_stmt st_un.su_non_print.np_stmt

	cnode su_print;			/* print statements */
#define stmt_print st_un.su_print

	int su_alloc;			/* alloc statements */
#define stmt_alloc st_un.su_alloc
    } st_un;
};

struct stmt *statements;

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
void resolv_gotos(void);
stmt_index mk_alloc_stmt(void);
stmt_index mk_print_stmt(cnode *c);
void push_breaks(stmt_index s);
void push_conts(stmt_index s);
stmt_index link_breaks(void);
stmt_index link_conts(void);
void dump_stmts(void);
void set_alloc(stmt_index s, int alloc);
