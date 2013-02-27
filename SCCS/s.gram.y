h08253
s 00033/00001/01927
d D 1.11 10/08/23 17:21:11 pedzan 11 10
c Checking in changes made over the years
e
s 00095/00063/01833
d D 1.10 02/03/14 16:12:32 pedz 10 9
e
s 00002/00001/01894
d D 1.9 00/09/18 17:57:01 pedz 9 8
c Checking before V5 conversion
e
s 00016/00004/01879
d D 1.8 00/05/29 15:09:07 pedz 8 7
c Just before IA64 conversion
e
s 00016/00000/01867
d D 1.7 00/02/08 17:36:47 pedz 7 6
e
s 00031/00010/01836
d D 1.6 98/10/23 12:26:09 pedz 6 5
e
s 00057/00054/01789
d D 1.5 97/05/13 16:02:57 pedz 5 4
c Check pointer
e
s 00038/00011/01805
d D 1.4 95/04/25 10:10:08 pedz 4 3
e
s 00385/00180/01431
d D 1.3 95/02/09 21:35:45 pedz 3 2
e
s 00386/00150/01225
d D 1.2 95/02/01 10:36:55 pedz 2 1
e
s 01375/00000/00000
d D 1.1 94/08/22 17:56:33 pedz 1 0
c date and time created 94/08/22 17:56:33 by pedz
e
u
U
t
T
I 1
%{
static char sccs_id[] = "%W%";
/*
 * The yacc grammer for the user interface language.  This should grow
 * into about 90% of C in time.
 */

#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
I 3
#include <dbxstclass.h>
I 10
#include <stdio.h>
E 10
I 6
#include "dex.h"
E 6
E 3
#include "map.h"
#include "sym.h"
#include "tree.h"
#include "stmt.h"
D 3
#include "base_expr.h"
E 3
I 3
#include "unary_expr.h"
E 3
#include "inter.h"
I 8
#include "fcall.h"
I 10
#include "gram_pre.h"
#include "scan_pre.h"
E 10
E 8
D 6
#include "dex.h"
E 6
D 2
#include "dmap.h"
E 2

I 3
#define YYDEBUG 1
I 10
#define DEBUG_BIT GRAM_Y_BIT
E 10

E 3
ns *ns_inter;
D 2
static anode current_attributes;
static char *current_identifier;
E 2
I 2
static anode anode_stack[8];
static int anode_index = -1;
#define current_attributes (anode_stack[anode_index])

#define push_attributes(anode) (anode_stack[++anode_index] = (anode))
#define pop_attributes (--anode_index)

E 2
static int param_count;
static arg_list *current_arg_list;

/*
 * The following defines attempt to redefine everything that yacc
 * craetes as global so that we can have two yacc's in the same
 * program.
 */
#define yyact GRAMact
#define yychar GRAMchar
#define yychk GRAMchk
#define yydebug GRAMdebug
#define yydef GRAMdef
#define yyerrflag GRAMerrflag
#define yyerror GRAMerror
#define yyexca GRAMexca
#define yylex GRAMlex
#define yylval GRAMlval
#define yynerrs GRAMnerrs
#define yypact GRAMpact
#define yyparse GRAMparse
#define yypgo GRAMpgo
#define yyps GRAMps
#define yypv GRAMpv
#define yypvt GRAMpvt
#define yyr1 GRAMr1
#define yyr2 GRAMr2
#define yys GRAMs
#define yystate GRAMstate
#define yytmp GRAMtmp
#define yyv GRAMv
#define yyval GRAMval
#define yytoks GRAMtoks
#define yyreds GRAMreds

I 5
static int nesting_level;
static symptr current_function;
D 9
static long global_index = (long)h_base;
E 9
static int param_index;
static int variable_index;
static int variable_max;
I 9

D 10
long global_index;
E 9

E 5
D 3
#define PROMPT (putchar('>'),putchar(' '),fflush(stdout))
E 3
I 2
void yyerror(char *s, ...);
E 10
I 5
static symptr gram_enter_sym(anode *attr, int line, cnode_list *init);
static int increase_nesting(void);
static void decrease_nesting(int new);
static void start_function(symptr f);
static void end_function(void);
static void do_parameter_allocation(arg_list *old_list, arg_list *args);
static void change_to_int(cnode *c);
I 6
static char *save_as_string(int i);
E 6

E 5
E 2
%}

%union {
D 2
    int val;			/* ints, etc */
    double rval;		/* reals */
    char *str;			/* names, etc */
E 2
I 2
D 8
    int val;				/* ints, etc */
E 8
I 8
D 10
    large_t val;			/* ints, etc */
E 10
I 10
    int ival;				/* int */
    long lval;				/* long */
    large_t llval;			/* large */
E 10
E 8
    double rval;			/* reals */
    char *str;				/* names, etc */
E 2
    typeptr type;
D 2
    cnode cnode;		/* expression tree */
    anode anode;		/* class/type pair */
    stmt stmt;			/* statement pointer */
    cnode_list *clist;	/* expression list */
    arg_list *alist;		/* argument list */
E 2
I 2
    cnode cnode;			/* expression tree */
    anode anode;			/* class/type pair */
    stmt_index stmt;			/* statement pointer */
    cnode_list *clist;			/* expression list */
    arg_list *alist;			/* argument list */
I 3
    fieldptr field;			/* field list */
    enumptr elist;			/* enum list */
E 3
E 2
}

/* keywords used for the commands of the debugger */
D 3
%token DUMP
D 2
%token NM
E 2
I 2
%token LOAD
E 2
%token PRINT
D 2
%token STAB
E 2
I 2
%token SOURCE
E 2
%token SYM_DUMP
E 3
I 3
D 10
%token <val> DUMP
%token <val> PRINT
%token <val> SOURCE
%token <val> SYM_DUMP
I 4
%token <val> STMT_DUMP
%token <val> TYPE_DUMP
E 10
I 10
%token <ival> DUMP
%token <ival> PRINT
%token <ival> SOURCE
%token <ival> SYM_DUMP
%token <ival> STMT_DUMP
%token <ival> TYPE_DUMP
E 10
E 4
E 3

/* keywords used for C -- all of this might be used at the current time */

D 3
%token AUTO
%token BREAK
%token CASE CHAR CONTINUE
%token DEFAULT DO DOUBLE
%token ELSE ENTRY EXTERN
%token ENUM
%token FLOAT FOR
%token GOTO
%token IF INT
%token LONG
%token REGISTER RETURN
%token SHORT SIGNED SIZEOF STATIC STRUCT SWITCH
%token UNION UNSIGNED
%token VOID
%token WHILE
%token TYPEDEF
E 3
I 3
D 10
%token <val> AUTO
%token <val> BREAK
%token <val> CASE CHAR CONTINUE
%token <val> DEFAULT DO DOUBLE
%token <val> ELSE ENTRY EXTERN
%token <val> ENUM
%token <val> FLOAT FOR
%token <val> GOTO
%token <val> IF INT
%token <val> LONG
%token <val> REGISTER RETURN
%token <val> SHORT SIGNED SIZEOF STATIC STRUCT SWITCH
%token <val> UNION UNSIGNED
%token <val> VOID
%token <val> WHILE
%token <val> TYPEDEF
E 10
I 10
%token <ival> AUTO
%token <ival> BREAK
%token <ival> CASE CHAR CONTINUE
%token <ival> DEFAULT DO DOUBLE
D 11
%token <ival> ELSE ENTRY EXTERN
E 11
I 11
%token <ival> ELSE /* ENTRY */ EXTERN
E 11
%token <ival> ENUM
%token <ival> FLOAT FOR
%token <ival> GOTO
%token <ival> IF INT
%token <ival> LONG
%token <ival> REGISTER RETURN
%token <ival> SHORT SIGNED SIZEOF STATIC STRUCT SWITCH
%token <ival> UNION UNSIGNED
%token <ival> VOID
%token <ival> WHILE
%token <ival> TYPEDEF
E 10
E 3

/*
 *  Token from scanner which should not exist but do to make the
 * grammer LR(1).  These symbols are for +=, -=, etc ...
 */

D 10
%token <val> PLUSEQUAL MINUSEQUAL TIMESEQUAL DIVEQUAL MODEQUAL LSEQUAL
%token <val> RSEQUAL ANDEQUAL XOREQUAL OREQUAL '=' ',' '*' '/' '%' '+'
%token <val> '-' RSHIFT LSHIFT '<' '>' GTOREQUAL LTOREQUAL EQUALITY
%token <val> NOTEQUAL '&' '^' '|' ANDAND OROR
E 10
I 10
%token <ival> PLUSEQUAL MINUSEQUAL TIMESEQUAL DIVEQUAL MODEQUAL LSEQUAL
%token <ival> RSEQUAL ANDEQUAL XOREQUAL OREQUAL '=' ',' '*' '/' '%' '+'
%token <ival> '-' RSHIFT LSHIFT '<' '>' GTOREQUAL LTOREQUAL EQUALITY
%token <ival> NOTEQUAL '&' '^' '|' ANDAND OROR
E 10

/*
 * Tokens from the scanner which must and should exist but are not
 * keywords.  These are identifiers, constants, and double operands
 * such as &&, ||, ->, etc.
 */

D 3
%token GTOREQUAL LTOREQUAL LSHIFT RSHIFT INCOP DECOP PTROP OROR ANDAND
%token EQUALITY NOTEQUAL
E 3
I 3
D 10
%token <val> INCOP DECOP PTROP
E 10
I 10
%token <ival> INCOP DECOP PTROP
E 10
E 3

D 10
%token <val> ICON
E 10
I 10
%token <ival>  ICON
%token <lval>  LCON
%token <llval> LLCON
E 10
%token <rval>  RCON
D 10
%token <str> IDENTIFIER STRING
%token <type> TYPEDEFNAME
E 10
I 10
%token <str>   IDENTIFIER STRING
%token <type>  TYPEDEFNAME
E 10

/* Precedence */

%left DOSHIFT
%left ','
%right '=' PLUSEQUAL MINUSEQUAL TIMESEQUAL DIVEQUAL MODEQUAL LSEQUAL
		RSEQUAL ANDEQUAL XOREQUAL OREQUAL
%right '?' ':'
%left OROR
%left ANDAND
%left '|'
%left '^'
%left '&'
%left EQUALITY NOTEQUAL
%left '<' '>' GTOREQUAL LTOREQUAL
%left LSHIFT RSHIFT
%left '+' '-'
%left '*' '/' '%'
%right '~' '!' SIZEOF INCOP DECOP
%left '(' '[' '.' PTROP ']' ')'
%right ELSE

%type <anode> type simple_type struct_declaration enum_declaration
D 2
%type <anode> type_name non_type_def class attributes
%type <anode> non_function_declarator function_declarator
%type <anode> ptr_function_declarator
E 2
I 2
D 5
%type <anode> type_name non_type_def class non_function_declarator
E 5
I 5
%type <anode> type_name class_name class non_function_declarator
E 5
%type <anode> function_declarator ptr_function_declarator null_decl
D 3
%type <anode> non_empty_decl
E 3
I 3
%type <anode> non_empty_decl str_head enum_head
E 3
E 2

D 3
%type <cnode> constant name lvalue pvalue expression func_call
%type <cnode> func_name opt_expression nce
E 3
I 3
%type <cnode> constant name lvalue pvalue expression
%type <cnode> opt_expression nce
E 3

D 10
%type <val> opt_constant_expression constant_expression asgn_op
D 3
%type <val> compound_statement_preamble function_heading
E 3
I 3
%type <val> compound_statement_preamble function_heading struct_union
E 10
I 10
%type <ival> opt_constant_expression constant_expression asgn_op
%type <ival> compound_statement_preamble function_heading struct_union
E 10
E 3

%type <stmt> compound_statement statement_list statement if_preamble
%type <stmt> else_preamble data_def init_declaration_list
%type <stmt> init_declarator function_body declaration_stat_list

D 5
%type <clist> expression_list opt_expression_list
E 5
I 5
%type <clist> expression_list opt_expression_list init_list
%type <clist> initializer
E 5

%type <alist> name_list typed_name_list typed_name arg_declaration
%type <alist> arg_declaration_list arg_declarator_list

I 3
%type <field> type_declaration_list r_type_declaration_list
%type <field> type_declaration declarator_list declarator

%type <elist> member_list

E 3
%start user_input

%%
user_input
D 3
    : { PROMPT; } command
E 3
I 3
    : user_input command
E 3
D 5
	{
D 3
	    PROMPT;
E 3
I 3
	    prompt();
E 3
	}
E 5
D 3
    | user_input command
E 3
I 3
    | /* NULL */
E 3
D 5
	{
D 3
	    PROMPT;
E 3
I 3
	    prompt();
E 3
	}
E 5
    ;

command
    : DUMP STRING ';'
	{
	    open_dump($2);
	}
    | SYM_DUMP ';'
	{
	    dump_symtable();
	}
I 4
    | STMT_DUMP ';'
	{
	    dump_stmts();
	}
    | TYPE_DUMP ';'
	{
	    dump_types();
	}
E 4
D 2
    | STAB STRING ';'
E 2
I 2
D 3
    | LOAD STRING ICON ICON ';'
E 2
	{
D 2
	    parse_stab($2);
E 2
I 2
	    load($2, $3, $4);
E 2
	}
E 3
D 2
    | NM STRING ICON ICON ';'
E 2
I 2
    | SOURCE STRING ';'
E 2
	{
D 2
	    load_namelist($2, $3, $4);
E 2
I 2
	    add_source($2);
E 2
	}
    | data_def 
	{
I 10
	    volatile long had_fault;

E 10
	    (void) mk_return_stmt((expr *)0);
I 10
	    BEGIN_PROTECT(&had_fault);
E 10
	    (void) execute_statement($1);
I 10
	    END_PROTECT();
	    if (had_fault)
		printf("hit a page fault at %s\n", P(had_fault));
E 10
	}
    | statement 
	{
I 10
	    volatile long had_fault;

E 10
	    (void) mk_return_stmt((expr *)0);
I 10
	    BEGIN_PROTECT(&had_fault);
E 10
	    (void) execute_statement($1);
I 10
	    END_PROTECT();
	    if (had_fault)
		printf("hit a page fault at %s\n", P(had_fault));
E 10
	}
    ;

data_def			/* a single global definition */
    : attributes init_declaration_list ';'
	{
	    $$ = $2;
I 2
	    pop_attributes;
E 2
	}
I 3
    | attributes ';'
	{
	    $$ = NO_STMT;
	    pop_attributes;
	}
E 3
    | attributes function_heading function_body
	{
	    (void) mk_return_stmt((expr *)0);
	    decrease_nesting($2);
	    end_function();
	    resolv_gotos();
	    $$ = NO_STMT;
I 2
	    pop_attributes;
E 2
	}
    | attributes error
	{
	    $$ = NO_STMT;
I 2
	    pop_attributes;
E 2
	}
    | attributes init_declaration_list error
	{
	    $$ = NO_STMT;
I 2
	    pop_attributes;
E 2
	}
    ;

function_heading
    : function_declarator arg_declaration_list 
	{
D 2
	    symptr f = enter_sym(&$1, current_identifier);
E 2
I 2
	    symptr f;

	    if ($1.a_class == typedef_class) {
		yyerror("Can not typedef a function");
		YYERROR;
	    }
D 5
	    f = gram_enter_sym(&$1, __LINE__);
E 5
I 5
	    f = gram_enter_sym(&$1, __LINE__, (cnode_list *)0);
E 5
E 2
	    $$ = increase_nesting();
D 6
	    f->s_stmt = mk_alloc_stmt();
D 2
	    f->s_size = sizeof(stmt);
E 2
I 2
	    f->s_size = sizeof(stmt_index);
E 2
	    start_function(f);
E 6
I 6
	    if (f) {
		f->s_stmt = mk_alloc_stmt();
		f->s_size = sizeof(stmt_index);
		if (f->s_expr)
		    f->s_expr->e_addr = f->s_offset;
		start_function(f);
	    }
E 6
	    do_parameter_allocation($2, current_arg_list);
	}
    ;

function_body			/* body of a function */
    : compound_statement
	{
	    $$ = $1;
	}
    ;

arg_declaration_list		/* list of argument declarations for a funcs */
    : arg_declaration_list arg_declaration
	{
	    $$ = $2;
	    $$->a_next = $1;
	}
    | /* NULL */
	{
	    $$ = 0;
	}
    ;

arg_declaration			/* argument declarations (old C style) */
    : attributes arg_declarator_list ';'
	{
	    $$ = $2;
I 2
	    pop_attributes;
E 2
	}
    | attributes ';'
	{
	    $$ = 0;
I 2
	    pop_attributes;
E 2
	}
    ;

arg_declarator_list		/* argument declarations (old C style */
    : non_function_declarator
	{
	    $$ = new(arg_list);
	    $$->a_next = 0;
D 2
	    $$->a_ident = current_identifier;
E 2
	    $$->a_anode = $1;
	}
    | arg_declarator_list ',' non_function_declarator
	{
	    $$ = new(arg_list);
	    $$->a_next = $1;
D 2
	    $$->a_ident = current_identifier;
E 2
	    $$->a_anode = $3;
	}
    ;

statement_list			/* list of statements in compound statement */
    : statement_list statement
	{
	    $$ = ($1 != NO_STMT) ? $1 : $2;
	}
    | /* NULL */
	{
	    $$ = NO_STMT;
	}
    ;

declaration_stat_list		/* list of vars inside a comp statement */
    : declaration_stat_list attributes ';'
	{
	    $$ = $1;
I 2
	    pop_attributes;
E 2
	}
    | declaration_stat_list attributes init_declaration_list ';'
	{
	    $$ = ($1 != NO_STMT) ? $1 : $3;
I 2
	    pop_attributes;
E 2
	}
    | /* NULL */
	{
	    $$ = NO_STMT;
	}
    ;

attributes			/* forced attrs for arguments and local vars */
    : class type
	{
D 2
	    $$.a_class = $1.a_class;
	    $$.a_type = $2.a_type;
	    $$.a_base = $2.a_base;
	    current_attributes = $$;
E 2
I 2
	    current_attributes.a_class = $1.a_class;
	    current_attributes.a_valid_class = 1;
E 2
	}
    | class
	{
D 2
	    $$ = $1;
	    $$.a_type = mkrange("int", 0x80000000, 0x7fffffff);
	    $$.a_base = int_type;
	    current_attributes = $$;
E 2
I 2
	    push_attributes($1);
D 3
	    current_attributes.a_type = mkrange(ns_inter, "int",
						0x80000000, 0x7fffffff);
E 3
I 3
	    current_attributes.a_type = find_type(ns_inter, TP_INT);
E 3
	    current_attributes.a_valid_type = 1;
	    current_attributes.a_base = int_type;
	    current_attributes.a_valid_base = 1;
E 2
	}
    | type
	{
D 2
	    $$ = $1;
	    $$.a_class = auto_class;
	    current_attributes = $$;
E 2
I 2
	    current_attributes.a_class = auto_class;
	    current_attributes.a_valid_class = 1;
E 2
	}
    ;

class				/* storage class of a var or func */
D 5
    : non_type_def
E 5
I 5
    : class_name
E 5
I 2
	{
	    $$.a_class = $1.a_class;
	    $$.a_valid_class = 1;
	    $$.a_valid_type = 0;
	    $$.a_valid_base = 0;
	    $$.a_valid_name = 0;
	}
E 2
D 5
    | TYPEDEF
	{
	    $$.a_class = typedef_class;
I 2
	    $$.a_valid_class = 1;
	    $$.a_valid_type = 0;
	    $$.a_valid_base = 0;
	    $$.a_valid_name = 0;
E 2
	}
E 5
    ;

D 5
non_type_def
E 5
I 5
class_name
E 5
    : AUTO
	{
	    $$.a_class = auto_class;
	}
    | REGISTER
	{
	    $$.a_class = reg_class;
	}
    | EXTERN
	{
	    $$.a_class = extern_class;
	}
    | STATIC
	{
	    $$.a_class = static_class;
	}
I 5
    | TYPEDEF
	{
	    $$.a_class = typedef_class;
	}
E 5
    ;

type				/* data type of a var or func */
    : simple_type
I 2
	{
	    $$ = $1;
	    $$.a_valid_type = 1;
	    $$.a_valid_base = 1;
	    $$.a_valid_class = 0;
	    $$.a_valid_name = 0;
	    push_attributes($$);
	}
E 2
    | struct_declaration
I 2
	{
	    $$ = $1;
	    $$.a_valid_type = 1;
	    $$.a_valid_base = 1;
	    $$.a_valid_class = 0;
	    $$.a_valid_name = 0;
	    push_attributes($$);
	}
E 2
    | enum_declaration
I 2
	{
	    $$ = $1;
	    $$.a_valid_type = 1;
	    $$.a_valid_base = 1;
	    $$.a_valid_class = 0;
	    $$.a_valid_name = 0;
	    push_attributes($$);
	}
E 2
    | TYPEDEFNAME
	{
	    $$.a_type = $1;
I 2
	    $$.a_valid_type = 1;
E 2
	    $$.a_base = base_type($1);
I 2
	    $$.a_valid_base = 1;
	    $$.a_valid_class = 0;
	    $$.a_valid_name = 0;
	    push_attributes($$);
E 2
	}
    ;

simple_type			/* predefined types */
    : CHAR
	{
D 2
	    $$.a_type = mkrange("char", 0, 255);
E 2
I 2
D 3
	    $$.a_type = mkrange(ns_inter, "char", 0, 255);
E 3
I 3
	    $$.a_type = find_type(ns_inter, TP_CHAR);
E 3
E 2
	    $$.a_base = uchar_type; /* should be an option */
	}
    | SHORT
	{
D 2
	    $$.a_type = mkrange("short", 0xffff8000, 0x00007fff);
E 2
I 2
D 3
	    $$.a_type = mkrange(ns_inter, "short", 0xffff8000, 0x00007fff);
E 3
I 3
	    $$.a_type = find_type(ns_inter, TP_SHORT);
E 3
E 2
	    $$.a_base = short_type;
	}
    | INT
	{
D 2
	    $$.a_type = mkrange("int", 0x80000000, 0x7fffffff);
E 2
I 2
D 3
	    $$.a_type = mkrange(ns_inter, "int", 0x80000000, 0x7fffffff);
E 3
I 3
	    $$.a_type = find_type(ns_inter, TP_INT);
E 3
E 2
	    $$.a_base = int_type;
	}
    | LONG
	{
I 8
D 10
#ifdef __64BIT__
	    $$.a_type = find_type(ns_inter, TP_LONG_64);
#else
E 8
D 2
	    $$.a_type = mkrange("long", 0x80000000, 0x7fffffff);
E 2
I 2
D 3
	    $$.a_type = mkrange(ns_inter, "long", 0x80000000, 0x7fffffff);
E 3
I 3
	    $$.a_type = find_type(ns_inter, TP_LONG);
I 8
#endif
E 10
I 10
	    if (sizeof(long) == 8)
		$$.a_type = find_type(ns_inter, TP_LONG_64);
	    else
		$$.a_type = find_type(ns_inter, TP_LONG);
E 10
E 8
E 3
E 2
	    $$.a_base = long_type;
	}
I 10
    | LONG LONG
	{
	    $$.a_type = find_type(ns_inter, TP_LLONG);
	    $$.a_base = long_long_type;
	}
E 10
    | UNSIGNED
	{
D 2
	    $$.a_type = mkrange("unsigned", 0, 0xffffffff);
E 2
I 2
D 3
	    $$.a_type = mkrange(ns_inter, "unsigned", 0, 0xffffffff);
E 3
I 3
	    $$.a_type = find_type(ns_inter, TP_UNSIGNED);
E 3
E 2
	    $$.a_base = uint_type;
	}
    | FLOAT
	{
D 2
	    $$.a_type = mkfloat("float", sizeof(float));
E 2
I 2
D 3
	    $$.a_type = mkfloat(ns_inter, "float", sizeof(float));
E 3
I 3
	    $$.a_type = find_type(ns_inter, TP_FLOAT);
E 3
E 2
	    $$.a_base = float_type;
	}
    | DOUBLE
	{
D 2
	    $$.a_type = mkfloat("double", sizeof(double));
E 2
I 2
D 3
	    $$.a_type = mkfloat(ns_inter, "double", sizeof(double));
E 3
I 3
	    $$.a_type = find_type(ns_inter, TP_DOUBLE);
E 3
E 2
	    $$.a_base = double_type;
	}
    | VOID
	{
D 2
	    $$.a_type = mkrange("void", 0, 0);
E 2
I 2
D 3
	    $$.a_type = mkrange(ns_inter, "void", 0, 0);
E 3
I 3
	    $$.a_type = find_type(ns_inter, TP_VOID);
E 3
E 2
	    $$.a_base = void_type;
	}
    | UNSIGNED CHAR
	{
D 2
	    $$.a_type = mkrange("unsigned char", 0, 255);
E 2
I 2
D 3
	    $$.a_type = mkrange(ns_inter, "unsigned char", 0, 255);
E 3
I 3
	    $$.a_type = find_type(ns_inter, TP_UCHAR);
E 3
E 2
	    $$.a_base = uchar_type;
	}
    | SIGNED CHAR
	{
D 2
	    $$.a_type = mkrange("signed char", -128, 127);
E 2
I 2
D 3
	    $$.a_type = mkrange(ns_inter, "signed char", -128, 127);
E 3
I 3
	    $$.a_type = find_type(ns_inter, TP_SCHAR);
E 3
E 2
	    $$.a_base = schar_type;
	}
    | UNSIGNED SHORT
	{
D 2
	    $$.a_type = mkrange("unsigned short", 0, 0xffff);
E 2
I 2
D 3
	    $$.a_type = mkrange(ns_inter, "unsigned short", 0, 0xffff);
E 3
I 3
	    $$.a_type = find_type(ns_inter, TP_USHORT);
E 3
E 2
	    $$.a_base = ushort_type;
	}
    | UNSIGNED INT
	{
D 2
	    $$.a_type = mkrange("unsigned int", 0, 0xffffffff);
E 2
I 2
D 3
	    $$.a_type = mkrange(ns_inter, "unsigned int", 0, 0xffffffff);
E 3
I 3
	    $$.a_type = find_type(ns_inter, TP_UINT);
E 3
E 2
	    $$.a_base = uint_type;
	}
    | UNSIGNED LONG
	{
I 8
D 10
#ifdef __64BIT__
	    $$.a_type = find_type(ns_inter, TP_ULONG_64);
#else
E 8
D 2
	    $$.a_type = mkrange("unsigned long", 0, 0xffffffff);
E 2
I 2
D 3
	    $$.a_type = mkrange(ns_inter, "unsigned long", 0, 0xffffffff);
E 3
I 3
	    $$.a_type = find_type(ns_inter, TP_ULONG);
I 8
#endif
E 10
I 10
	    if (sizeof(long) == 8)
		$$.a_type = find_type(ns_inter, TP_ULONG_64);
	    else
		$$.a_type = find_type(ns_inter, TP_ULONG);
E 10
E 8
E 3
E 2
	    $$.a_base = ulong_type;
	}
I 10
    | UNSIGNED LONG LONG
	{
	    $$.a_type = find_type(ns_inter, TP_ULLONG);
	    $$.a_base = ulong_long_type;
	}
E 10
    ;

enum_declaration
    : enum_head '{' member_list opt_comma '}'
	{
D 3
	    /* #### enumeration definitions */
	    yyerror("can't declare enumeration types yet");
	    YYERROR;
E 3
I 3
	    typeptr t = $1.a_type;

	    $$ = $1;
	    t->t_val.val_e = $3;
	    if ($1.a_name)
		add_namedef(t, $1.a_name);
E 3
	}
    | ENUM IDENTIFIER
	{
D 2
	    if (!($$.a_type = name2namedef($2)) ||
E 2
I 2
	    if (!($$.a_type = name2namedef_all($2)) ||
E 2
		$$.a_type->t_type != ENUM_TYPE) {
D 2
		yyerror2("enum %s not found", $2);
E 2
I 2
		yyerror("enum %s not found", $2);
E 2
		YYERROR;
	    }
	    $$.a_base = int_type;
I 3
	    $$.a_valid_type = 1;
	    $$.a_valid_base = 1;
E 3
	}
    | ENUM error
	{
	    $$.a_type = 0;
	}
    ;

enum_head
    : ENUM
I 3
	{
	    $$.a_type = newtype(ns_inter, ENUM_TYPE);
	    $$.a_valid_type = 1;
	    $$.a_name = 0;
	    $$.a_valid_name = 1;
	    $$.a_base = int_type;
	    $$.a_valid_base = 1;
	    $$.a_valid_class = 0;
	}
E 3
    | ENUM IDENTIFIER
I 3
	{
	    $$.a_type = newtype(ns_inter, ENUM_TYPE);
	    $$.a_valid_type = 1;
	    $$.a_name = $2;
	    $$.a_valid_name = 1;
	    $$.a_base = int_type;
	    $$.a_valid_base = 1;
	    $$.a_valid_class = 0;
	}
E 3
    ;

member_list
D 3
    : member
    | member_list ',' member
    ;

member
E 3
    : IDENTIFIER
I 3
	{
	    $$ = newenum($1, 0);
	}
E 3
    | IDENTIFIER '=' constant_expression
I 3
	{
	    $$ = newenum($1, $3);
	}
    | member_list ',' IDENTIFIER
	{
	    $$ = newenum($3, $1->e_val + 1);
	    $$->e_next = $1;
	}
    | member_list ',' IDENTIFIER '=' constant_expression
	{
	    $$ = newenum($3, $5);
	    $$->e_next = $1;
	}
E 3
    ;

struct_declaration
D 3
    : str_head '{' type_declaration_list opt_semi '}'
E 3
I 3
    : str_head '{' type_declaration_list '}'
E 3
	{
D 3
	    /* #### structure definitions */
	    yyerror("can't declare structs and unions yet");
	    YYERROR;
E 3
I 3
	    typeptr t = $1.a_type;
	    int pos = 0;
	    fieldptr f = $3;

I 4
	    if (t->t_val.val_s.s_fields) {
		yyerror("struct or unions %s can not be redeclared",
			$1.a_name);
		YYERROR;
	    }
E 4
	    t->t_val.val_s.s_fields = f;
	    if (t->t_type == STRUCT_TYPE) {
		pos = allocate_fields(f);
	    } else {
		for ( ; f; f = f->f_next) {
		    if (f->f_numbits < 0)
			f->f_numbits = -f->f_numbits;
		    if (f->f_numbits > pos)
			pos = f->f_numbits;
		}
	    }
	    pos += (sizeof(int) * 8) - 1;
I 5
	    pos &= ~((sizeof(int) * 8) - 1);
E 5
	    pos /= 8;
	    t->t_val.val_s.s_size = pos;
D 4
	    if ($1.a_name)
		add_namedef(t, $1.a_name);
E 4
	    $$ = $1;
E 3
	}
    | struct_union IDENTIFIER
	{
D 2
	    if (!($$.a_type = name2namedef($2))) {
		yyerror2("struct or union %s not found", $2);
E 2
I 2
	    if (!($$.a_type = name2namedef_all($2))) {
D 4
		yyerror("struct or union %s not found", $2);
E 2
		YYERROR;
E 4
I 4
		$$.a_type = newtype(ns_inter, (($1 == STRUCT) ?
					       STRUCT_TYPE : UNION_TYPE));
		add_namedef($$.a_type, $2);
E 4
	    }
D 4
	    $$.a_base = struct_type;
E 4
I 3
	    $$.a_valid_type = 1;
I 4
	    $$.a_name = $2;
	    $$.a_valid_name = 1;
	    $$.a_base = struct_type;
E 4
	    $$.a_valid_base = 1;
E 3
	}
    | struct_union error
	{
	    $$.a_type = 0;
	}
    ;

str_head
    : struct_union
I 3
	{
	    $$.a_type = newtype(ns_inter, (($1 == STRUCT) ?
					   STRUCT_TYPE : UNION_TYPE));
	    $$.a_valid_type = 1;
	    $$.a_name = 0;
	    $$.a_valid_name = 1;
	    $$.a_base = struct_type;
	    $$.a_valid_base = 1;
	    $$.a_valid_class = 0;
	}
E 3
    | struct_union IDENTIFIER
I 3
	{
D 4
	    $$.a_type = newtype(ns_inter, (($1 == STRUCT) ?
					   STRUCT_TYPE : UNION_TYPE));
E 4
I 4
	    if (!($$.a_type = name2namedef(ns_inter, $2))) {
		$$.a_type = newtype(ns_inter, (($1 == STRUCT) ?
					       STRUCT_TYPE : UNION_TYPE));
		add_namedef($$.a_type, $2);
	    }
E 4
	    $$.a_valid_type = 1;
	    $$.a_name = $2;
	    $$.a_valid_name = 1;
	    $$.a_base = struct_type;
	    $$.a_valid_base = 1;
	    $$.a_valid_class = 0;
	}
E 3
    ;

struct_union
    : STRUCT
    | UNION
    ;

I 3
/*
 * The fields are flipped into their proper order here.
 */
E 3
type_declaration_list		/* list of struct fields w/ type */
I 3
    : r_type_declaration_list opt_semi
	{
	    register fieldptr last, this, next;

	    last = 0;
	    this = $1;
	    do {
		next = this->f_next;
		this->f_next = last;
		last = this;
		this = next;
	    } while (this);
	    $$ = last;
	}
    ;

/*
 * This constructs the list of fields in reverse order
 */
r_type_declaration_list
E 3
    : type_declaration
D 3
    | type_declaration_list ';' type_declaration
E 3
I 3
	{
	    $$ = $1;
	}
    | r_type_declaration_list ';' type_declaration
	{
	    $3->f_next = $1;
	    $$ = $3;
	}
E 3
    ;

type_declaration		/* a struct field w/ type */
D 2
    : type declarator_list
E 2
I 2
D 3
    : type declarator_list	/* #### */
E 3
I 3
    : type declarator_list
E 3
	{
I 3
	    $$ = $2;
E 3
	    pop_attributes;
	}
E 2
    | type
I 2
	{
I 3
	    $$ = newfield((char *)0, $1.a_type, 0, get_size($1.a_type));
E 3
	    pop_attributes;
	}
E 2
    ;

D 3
declarator_list			/* fields after TYPE for a struct */
E 3
I 3
declarator_list		/* fields after TYPE for a struct */
E 3
    : declarator
I 3
	{
	    $$ = $1;
	}
E 3
    | declarator_list ',' declarator
I 3
	{
	    $3->f_next = $1;
	    $$ = $3;
	}
E 3
    ;

declarator			/* a single field in a struct */
    : non_function_declarator
I 3
	{
	    $$ = newfield($1.a_name, $1.a_type, 0, get_size($1.a_type));
	}
E 3
    | non_function_declarator ':' constant_expression
I 3
	{
	    $$ = newfield($1.a_name, $1.a_type, 0, -$3);
	}
E 3
    | ':' constant_expression
I 3
	{
	    $$ = newfield((char *)0, (typeptr)0, 0, -$2);
	}
E 3
    ;

non_function_declarator		/* argument, var, or field */
    : '*' non_function_declarator
	{
D 2
	    typeptr t = newtype($2.a_type->t_ns, PTR_TYPE, (char *)0, 0);
E 2
I 2
	    typeptr t = newtype($2.a_type->t_ns, PTR_TYPE);
E 2

D 2
	    $$.a_class = $2.a_class;
E 2
I 2
	    $$ = $2;
E 2
	    $$.a_type = t;
	    $$.a_base = ulong_type;
	    t->t_val.val_p = $2.a_type;
	}
    | '(' '*' non_function_declarator ')' arg_list
	{
D 2
	    typeptr t1 = newtype($3.a_type->t_ns, PTR_TYPE, (char *)0, 0);
	    typeptr t2 = newtype($3.a_type->t_ns, PROC_TYPE, (char *)0, 0);
E 2
I 2
	    typeptr t1 = newtype($3.a_type->t_ns, PTR_TYPE);
	    typeptr t2 = newtype($3.a_type->t_ns, PROC_TYPE);
E 2

D 2
	    $$.a_class = $3.a_class;
E 2
I 2
	    $$ = $3;
E 2
	    $$.a_type = t1;
	    $$.a_base = ulong_type;
	    t1->t_val.val_p = t2;
	    t2->t_val.val_f.f_typeptr = $3.a_type;
	    t2->t_val.val_f.f_params = -1;
	    t2->t_val.val_f.f_paramlist = 0;
	}
    | non_function_declarator '[' opt_constant_expression ']'
	{
D 2
	    typeptr t1 = newtype($1.a_type->t_ns, RANGE_TYPE, (char *)0, 0);
	    typeptr t2 = newtype($1.a_type->t_ns, ARRAY_TYPE, (char *)0, 0);
E 2
I 2
	    typeptr t1 = newtype($1.a_type->t_ns, RANGE_TYPE);
	    typeptr t2 = newtype($1.a_type->t_ns, ARRAY_TYPE);
E 2

I 2
	    $$ = $1;
E 2
	    t1->t_val.val_r.r_typeptr = 0;
	    t1->t_val.val_r.r_lower = 0;
D 6
	    t1->t_val.val_r.r_upper = $3 - 1;
E 6
I 6
	    t1->t_val.val_r.r_upper = save_as_string($3 - 1);
E 6
	    t2->t_val.val_a.a_typedef = t1;
	    t2->t_val.val_a.a_typeptr = $1.a_type;
D 2
	    $$.a_class = $1.a_class;
E 2
	    $$.a_type = t2;
	    $$.a_base = ulong_type;
	}
    | '(' non_function_declarator ')'
	{
	    $$ = $2;
	}
    | IDENTIFIER
	{
D 2
	    current_identifier = $1;
E 2
	    $$ = current_attributes;
I 2
	    $$.a_name = $1;
	    $$.a_valid_name = 1;
E 2
	}
    ;

function_declarator		/* function name */
    : '*' function_declarator
	{
D 2
	    typeptr t = newtype($2.a_type->t_ns, PTR_TYPE, (char *)0, 0);
E 2
I 2
	    typeptr t = newtype($2.a_type->t_ns, PTR_TYPE);
E 2

I 4
	    t->t_val.val_p = $2.a_type->t_val.val_f.f_typeptr;
E 4
D 2
	    $$.a_class = $2.a_class;
E 2
I 2
	    $$ = $2;
E 2
D 4
	    $$.a_type = t;
E 4
I 4
	    $$.a_type->t_val.val_f.f_typeptr = t;
E 4
	    $$.a_base = ulong_type;
D 4
	    t->t_val.val_p = $2.a_type;
E 4
	}
    | ptr_function_declarator arg_list
	{
D 2
	    typeptr t2 = newtype($1.a_type->t_ns, PROC_TYPE, (char *)0, 0);
E 2
I 2
	    typeptr t2 = newtype($1.a_type->t_ns, PROC_TYPE);
E 2

D 2
	    $$.a_class = $1.a_class;
E 2
I 2
	    $$ = $1;
E 2
	    $$.a_type = t2;
	    $$.a_base = ulong_type;
	    t2->t_val.val_f.f_typeptr = $1.a_type;
	    t2->t_val.val_f.f_params = -1;
	    t2->t_val.val_f.f_paramlist = 0;
	}
    | ptr_function_declarator '[' opt_constant_expression ']'
	{
D 2
	    typeptr t1 = newtype($1.a_type->t_ns, RANGE_TYPE, (char *)0, 0);
	    typeptr t2 = newtype($1.a_type->t_ns, ARRAY_TYPE, (char *)0, 0);
E 2
I 2
	    typeptr t1 = newtype($1.a_type->t_ns, RANGE_TYPE);
	    typeptr t2 = newtype($1.a_type->t_ns, ARRAY_TYPE);
E 2

I 2
	    $$ = $1;
E 2
	    t1->t_val.val_r.r_typeptr = 0;
	    t1->t_val.val_r.r_lower = 0;
D 6
	    t1->t_val.val_r.r_upper = $3 - 1;
E 6
I 6
	    t1->t_val.val_r.r_upper = save_as_string($3 - 1);
E 6
	    t2->t_val.val_a.a_typedef = t1;
	    t2->t_val.val_a.a_typeptr = $1.a_type;
D 2
	    $$.a_class = $1.a_class;
E 2
	    $$.a_type = t2;
	    $$.a_base = ulong_type;
	}
    | '(' function_declarator ')'
	{
	    $$ = $2;
	}
    | IDENTIFIER arg_list
	{
D 2
	    typeptr t2 = newtype(current_attributes.a_type->t_ns,
				 PROC_TYPE, (char *)0, 0);
E 2
I 2
	    typeptr t2;
E 2

D 2
	    current_identifier = $1;
	    $$.a_class = current_attributes.a_class;
	    $$.a_type = t2;
E 2
I 2
	    $$ = current_attributes;
	    $$.a_name = $1;
	    $$.a_valid_name = 1;
E 2
	    $$.a_base = ulong_type;
D 2
	    t2->t_val.val_f.f_typeptr = current_attributes.a_type;
E 2
I 2
	    t2 = newtype($$.a_type->t_ns, PROC_TYPE);
	    t2->t_val.val_f.f_typeptr = $$.a_type;
E 2
	    t2->t_val.val_f.f_params = -1;
	    t2->t_val.val_f.f_paramlist = 0;
I 2
	    $$.a_type = t2;
E 2
	}
    ;

ptr_function_declarator		/* function returning pointer to ... */
    : '(' '*' function_declarator ')'
	{
D 2
	    typeptr t = newtype($3.a_type->t_ns, PTR_TYPE, (char *)0, 0);
E 2
I 2
	    typeptr t = newtype($3.a_type->t_ns, PTR_TYPE);
E 2

D 2
	    $$.a_class = $3.a_class;
E 2
I 2
	    $$ = $3;
E 2
	    $$.a_type = t;
	    $$.a_base = ulong_type;
	    t->t_val.val_p = $3.a_type;
	}
    ;

arg_list			/* Formal argument list */
    : arg_list_prefix '(' ')'
	{
	    param_count = 0;
	    current_arg_list = 0;
	}
    | arg_list_prefix '(' name_list ')'
	{
	    current_arg_list = $3;
	}
    | arg_list_prefix '(' typed_name_list ')'
	{
	    current_arg_list = $3;
	}
    ;

arg_list_prefix
    : /* NULL */
	{
	    arg_list *a, *b;

	    for (a = current_arg_list; b = a; free(b))
		a = b->a_next;
	    current_arg_list = 0;
	}
    ;

name_list			/* simple list of formal arguments */
    : IDENTIFIER
	{
	    $$ = new(arg_list);
D 2
	    $$->a_next = 0;
	    $$->a_ident = $1;
	    $$->a_anode.a_type = 0;
E 2
I 2
D 10
	    bzero(&$$, sizeof($$));
E 10
	    $$->a_anode.a_name = $1;
	    $$->a_anode.a_valid_name = 1;
E 2
	    param_count = 1;
	}
D 2
    | name_list ',' IDENTIFIER
E 2
I 2
    | IDENTIFIER ',' name_list
E 2
	{
	    $$ = new(arg_list);
D 2
	    $$->a_next = $1;
	    $$->a_ident = $3;
	    $$->a_anode.a_type = 0;
E 2
I 2
D 10
	    bzero(&$$, sizeof($$));
E 10
	    $$->a_next = $3;
	    $$->a_anode.a_name = $1;
	    $$->a_anode.a_valid_name = 1;
E 2
	    ++param_count;
	}
    ;

typed_name_list			/* simple list of formal arguments w/ types */
    : typed_name
	{
	    $$ = $1;
	    param_count = 1;
	}
D 2
    | typed_name_list ',' typed_name
E 2
I 2
    | typed_name ',' typed_name_list
E 2
	{
D 2
	    $$ = $3;
	    $$->a_next = $1;
E 2
I 2
	    $$ = $1;
	    $$->a_next = $3;
E 2
	    ++param_count;
	}
    ;

typed_name			/* a single formal arguemnt w/ type */
    : type non_function_declarator
	{
	    $$ = new(arg_list);
	    $$->a_next = 0;
D 2
	    $$->a_ident = current_identifier;
E 2
	    $$->a_anode = $2;
I 2
	    pop_attributes;
E 2
	}
    ;

init_declaration_list		/* local or global vars after attributes */
    : init_declarator
    | init_declaration_list ',' init_declarator
	{
	    $$ = ($1 != NO_STMT) ? $1 : $3;
	}
    ;

init_declarator			/* single local or global var */
    : non_function_declarator
	{
D 2
	    (void)enter_sym(&$1, current_identifier);
E 2
I 2
	    if ($1.a_class == typedef_class) {
		typeptr t = newtype(ns_inter, $1.a_type->t_type);
		add_typedef(t, $1.a_name);
		copy_type(t, $1.a_type);
	    } else {
D 5
		(void) gram_enter_sym(&$1, __LINE__);
E 5
I 5
		(void) gram_enter_sym(&$1, __LINE__, (cnode_list *)0);
E 5
	    }
E 2
	    $$ = NO_STMT;
	}
    | function_declarator
	{
D 2
	    (void)enter_sym(&$1, current_identifier);
E 2
I 2
	    if ($1.a_class == typedef_class) {
		typeptr t = newtype(ns_inter, $1.a_type->t_type);
		add_typedef(t, $1.a_name);
		copy_type(t, $1.a_type);
	    } else {
D 5
		(void) gram_enter_sym(&$1, __LINE__);
E 5
I 5
		(void) gram_enter_sym(&$1, __LINE__, (cnode_list *)0);
E 5
	    }
E 2
	    $$ = NO_STMT;
	}
    | non_function_declarator '=' nce
	{
	    cnode id;
	    cnode asgn;

D 2
	    (void)enter_sym(&$1, current_identifier);
	    if (mkident(&id, current_identifier)) {
		yyerror2("identifier %s not defined", current_identifier);
E 2
I 2
	    if ($1.a_class == typedef_class) {
		yyerror("Cannot initialize a typedef");
E 2
		YYERROR;
	    }
I 2
D 5
	    (void) gram_enter_sym(&$1, __LINE__);
E 5
I 5
	    (void) gram_enter_sym(&$1, __LINE__, (cnode_list *)0);
E 5
D 3
	    if (mkident(&id, $1.a_name)) {
E 3
I 3
	    if (mk_ident(&id, $1.a_name)) {
E 3
		yyerror("identifier %s not defined", $1.a_name);
		YYERROR;
	    }
E 2
D 3
	    if (mkasgn(&asgn, &id, $2, &$3))
E 3
I 3
	    if (mk_asgn(&asgn, &id, $2, &$3))
E 3
		YYERROR;
D 2
	    $$ = mk_expr_stmt(cast_to_int(&asgn));
E 2
I 2
	    $$ = mk_expr_stmt(asgn.c_expr);
E 2
	}
    | non_function_declarator '=' '{' init_list opt_comma '}'
	{
D 2
	    (void)enter_sym(&$1, current_identifier);
E 2
I 2
	    if ($1.a_class == typedef_class) {
		yyerror("Cannot initialize a typedef");
		YYERROR;
	    }
D 5
	    (void) gram_enter_sym(&$1, __LINE__);
E 5
I 5
	    (void) gram_enter_sym(&$1, __LINE__, $4);
	    $$ = NO_STMT;
E 5
E 2
	}
    ;

init_list
    : initializer
I 5
	{
	    $$ = $1;
	}
E 5
    | init_list ',' initializer
I 5
	{
	    $$ = $3;
	    $$->cl_next = $1;
	    $$->cl_down = 0;
	}
E 5
    ;

initializer
    : nce
I 3
	{
I 5
	    cnode_list *e = new(cnode_list);
	    e->cl_next = 0;
	    e->cl_down = 0;
	    e->cl_cnode = $1;
	    $$ = e;
E 5
	}
E 3
    | '{' init_list opt_comma '}'
I 5
	{
	    cnode_list *e = new(cnode_list);
	    e->cl_next = 0;
	    e->cl_down = $2;
	    $$ = e;
	}
E 5
    ;

opt_comma
    : /* NULL */
    | ','
I 3
D 5
	{
	}
E 5
E 3
    ;

opt_semi
    : /* NULL */
    | ';'
    ;

/* Statements */

compound_statement
    : compound_statement_preamble declaration_stat_list statement_list '}'
	{
	    decrease_nesting($1);
	    $$ = ($2 != NO_STMT) ? $2 : $3;
	}
    ;

compound_statement_preamble
    : '{'
	{
	    $$ = increase_nesting();
	}
    ;

statement
    : expression ';'
	{
D 2
	    $$ = mk_expr_stmt(cast_to_int(&$1));
E 2
I 2
	    $$ = mk_expr_stmt($1.c_expr);
E 2
	}
    | compound_statement
	{
	    $$ = $1;
	}
    | if_preamble statement	%prec ELSE
	{
	    link_stmt($1, NO_STMT);
	    $$ = $1;
	}
    | if_preamble statement ELSE else_preamble statement
	{
	    link_stmt($1, $5);
	    link_stmt($4, NO_STMT);
	    $$ = $1;
	}
    | WHILE '(' expression ')'
	{
D 2
	    stmt t = mk_goto_stmt(NO_STMT);
	    
	    push_breaks(t);
	    push_conts(t);
E 2
I 2
	    /* breaks will save address of goto stmt */
	    push_breaks(mk_goto_stmt(NO_STMT));
	    push_conts(NO_STMT);
E 2
	}
	statement
	{
D 2
	    stmt t = link_conts();
E 2
I 2
	    stmt_index br;
E 2

D 2
	    (void) mk_br_true(cast_to_int(&$3), t);
	    (void) link_breaks();
	    $$ = t;
E 2
I 2
	    (void)link_conts();
	    br = mk_br_true(cast_to_int(&$3), $6);
I 3
	    if ($6 == NO_STMT)
		link_stmt(br, br);
E 3
	    link_stmt(link_breaks(), br);
	    $$ = br;
E 2
	}
    | DO
	{
	    push_breaks(NO_STMT);
	    push_conts(NO_STMT);
	}
	statement WHILE '(' expression ')' ';'
	{
	    (void) link_conts();
	    $$ = mk_br_true(cast_to_int(&$6), $3);
I 3
	    if ($3 == NO_STMT)
		link_stmt($$, $$);
E 3
	    (void) link_breaks();
	}
    | FOR '(' opt_expression ';' opt_expression ';' opt_expression ')'
	{
D 2
	    stmt t;
E 2
I 2
	    stmt_index t;
E 2

D 2
	    t = mk_expr_stmt(cast_to_int(&$3));
E 2
I 2
	    t = mk_expr_stmt($3.c_expr);
E 2
	    push_breaks(t);
	    t = mk_br_false(cast_to_int(&$5), NO_STMT);
	    push_conts(t);
	}
	statement
	{
D 2
	    stmt t;
E 2
I 2
	    stmt_index t;
E 2

	    t = link_conts();
D 2
	    (void) mk_expr_stmt(cast_to_int(&$7));
E 2
I 2
	    (void) mk_expr_stmt($7.c_expr);
E 2
	    (void) mk_goto_stmt(t);
	    link_stmt(t, NO_STMT);
	    $$ = link_breaks();
	}
    | SWITCH '(' expression ')'
	{
	    $$ = mk_switch_stmt(cast_to_int(&$3));
	    push_breaks($$);
	}
	statement
	{
	    $$ = link_breaks();
	}
    | BREAK ';'
	{
	    $$ = mk_add_break_stmt();
	}
    | CONTINUE ';'
	{
	    $$ = mk_add_cont_stmt();
	}
    | RETURN opt_expression ';'
	{
D 2
	    cast_to_current_function(&$2);
E 2
I 2
	    if (current_function)
D 3
		cast_to(&$2, current_function->s_type, current_function->s_base);
E 3
I 3
		cast_to(&$2, current_function->s_type,
			current_function->s_base);
E 3
E 2
	    $$ = mk_return_stmt($2.c_expr);
	}
    | GOTO IDENTIFIER ';'
	{
	    $$ = mk_add_goto_stmt($2);
	}
    | ';'
	{
	    $$ = NO_STMT;
	}
    | IDENTIFIER ':' statement
	{
	    mk_goto_def($3, $1);
	    $$ = $3;
	}
    | label statement
	{
	    /* #### make switch label */
	    $$ = $2;
	}
    | PRINT expression ';'
	{
	    $$ = mk_print_stmt(&$2);
I 7
D 10
	    PRINTF("did statement\n");
E 10
I 10
	    DEBUG_PRINTF(("did statement\n"));
E 10
E 7
	}
    | error
	{
	    $$ = NO_STMT;
	}
    ;

if_preamble
    : IF '(' expression ')'
	{
	    $$ = mk_br_false(cast_to_int(&$3), NO_STMT);
	}
    ;

else_preamble
    : /* NULL */
	{
	    $$ = mk_goto_stmt(NO_STMT);
	}
    ;

label
    : CASE constant_expression ':'
I 3
	{
	}
E 3
    | DEFAULT ':'
I 3
	{
	}
E 3
    ;

/* expression */

opt_expression
    : /* NULL */
	{
	    $$.c_type = 0;
	    $$.c_base = int_type;
D 3
	    $$.c_const = 1;
E 3
	    $$.c_expr = 0;
I 3
	    $$.c_const = 1;
	    $$.c_bitfield = 0;
E 3
	}
    | expression
    ;

opt_constant_expression
    : /* NULL */
	{
	    $$ = 0;
	}
    | constant_expression
    ;

constant_expression
    : expression			%prec DOSHIFT
	{
	    if ($1.c_base != int_type) {
		yyerror("constant expressions must be integer");
		YYERROR;
D 3
	    } else if (!$1.c_const) {
E 3
I 3
	    }
	    if (!$1.c_const) {
E 3
		yyerror("expression is not a constant");
		YYERROR;
D 3
	    } else
		$$ = i_val($1.c_expr);
E 3
I 3
	    }
	    $$ = i_val($1.c_expr);
E 3
	}
    ;

expression
    : nce
    | expression ',' nce
	{
D 3
	    if (mkbinary(&$$, &$1, $2, &$3))
E 3
I 3
	    if (mk_binary(&$$, &$1, $2, &$3))
E 3
		YYERROR;
	}
    ;

/* nce is for non-comma-expression */
nce
    : nce '*' nce
	{
D 3
	    if (mkbinary(&$$, &$1, $2, &$3))
E 3
I 3
	    if (mk_binary(&$$, &$1, $2, &$3))
E 3
		YYERROR;
	}
    | nce '/' nce
	{
D 3
	    if (mkbinary(&$$, &$1, $2, &$3))
E 3
I 3
	    if (mk_binary(&$$, &$1, $2, &$3))
E 3
		YYERROR;
	}
    | nce '%' nce
	{
D 3
	    if (mkbinary(&$$, &$1, $2, &$3))
E 3
I 3
	    if (mk_binary(&$$, &$1, $2, &$3))
E 3
		YYERROR;
	}
    | nce '+' nce
	{
D 3
	    if (mkbinary(&$$, &$1, $2, &$3))
E 3
I 3
	    if (mk_binary(&$$, &$1, $2, &$3))
E 3
		YYERROR;
	}
    | nce '-' nce
	{
D 3
	    if (mkbinary(&$$, &$1, $2, &$3))
E 3
I 3
	    if (mk_binary(&$$, &$1, $2, &$3))
E 3
		YYERROR;
	}
    | nce RSHIFT nce
	{
D 3
	    if (mkbinary(&$$, &$1, $2, &$3))
E 3
I 3
	    if (mk_binary(&$$, &$1, $2, &$3))
E 3
		YYERROR;
	}
    | nce LSHIFT nce
	{
D 3
	    if (mkbinary(&$$, &$1, $2, &$3))
E 3
I 3
	    if (mk_binary(&$$, &$1, $2, &$3))
E 3
		YYERROR;
	}
    | nce '<' nce
	{
D 3
	    if (mkbinary(&$$, &$1, $2, &$3))
E 3
I 3
	    if (mk_binary(&$$, &$1, $2, &$3))
E 3
		YYERROR;
I 3
	    change_to_int(&$$);
E 3
	}
    | nce '>' nce
	{
D 3
	    if (mkbinary(&$$, &$1, $2, &$3))
E 3
I 3
	    if (mk_binary(&$$, &$1, $2, &$3))
E 3
		YYERROR;
I 3
	    change_to_int(&$$);
E 3
	}
    | nce GTOREQUAL nce
	{
D 3
	    if (mkbinary(&$$, &$1, $2, &$3))
E 3
I 3
	    if (mk_binary(&$$, &$1, $2, &$3))
E 3
		YYERROR;
I 3
	    change_to_int(&$$);
E 3
	}
    | nce LTOREQUAL nce
	{
D 3
	    if (mkbinary(&$$, &$1, $2, &$3))
E 3
I 3
	    if (mk_binary(&$$, &$1, $2, &$3))
E 3
		YYERROR;
I 3
	    change_to_int(&$$);
E 3
	}
    | nce EQUALITY nce
	{
D 3
	    if (mkbinary(&$$, &$1, $2, &$3))
E 3
I 3
	    if (mk_binary(&$$, &$1, $2, &$3))
E 3
		YYERROR;
I 3
	    change_to_int(&$$);
E 3
	}
    | nce NOTEQUAL nce
	{
D 3
	    if (mkbinary(&$$, &$1, $2, &$3))
E 3
I 3
	    if (mk_binary(&$$, &$1, $2, &$3))
E 3
		YYERROR;
I 3
	    change_to_int(&$$);
E 3
	}
    | nce '&' nce
	{
D 3
	    if (mkbinary(&$$, &$1, $2, &$3))
E 3
I 3
	    if (mk_binary(&$$, &$1, $2, &$3))
E 3
		YYERROR;
	}
    | nce '^' nce
	{
D 3
	    if (mkbinary(&$$, &$1, $2, &$3))
E 3
I 3
	    if (mk_binary(&$$, &$1, $2, &$3))
E 3
		YYERROR;
	}
    | nce '|' nce
	{
D 3
	    if (mkbinary(&$$, &$1, $2, &$3))
E 3
I 3
	    if (mk_binary(&$$, &$1, $2, &$3))
E 3
		YYERROR;
	}
    | nce ANDAND nce
	{
D 3
	    if (mkbinary(&$$, &$1, $2, &$3))
E 3
I 3
	    if (mk_binary(&$$, &$1, $2, &$3))
E 3
		YYERROR;
	}
    | nce OROR nce
	{
D 3
	    if (mkbinary(&$$, &$1, $2, &$3))
E 3
I 3
	    if (mk_binary(&$$, &$1, $2, &$3))
E 3
		YYERROR;
	}
    | nce '?' nce ':' nce
	{
	    if (mk_qc_op(&$$, &$1, &$3, &$5))
		YYERROR;
	}
    | lvalue asgn_op nce		%prec '='
	{
D 3
	    if (mkasgn(&$$, &$1, $2, &$3))
E 3
I 3
	    if (mk_asgn(&$$, &$1, $2, &$3))
E 3
		YYERROR;
	}
D 3
    | '(' expression ')'
	{
	    $$ = $2;
	}
E 3
    | pvalue				%prec ','
D 2
    | '&' lvalue '=' nce
	{
	    $$ = $2;
	}
E 2
    ;


asgn_op
    : '='
    | PLUSEQUAL
    | MINUSEQUAL
    | TIMESEQUAL
    | DIVEQUAL
    | MODEQUAL
    | LSEQUAL
    | RSEQUAL
    | ANDEQUAL
    | OREQUAL
I 6
    | XOREQUAL
E 6
    ;

pvalue
D 3
    : lvalue				%prec ','
E 3
I 3
    : lvalue				%prec DOSHIFT
E 3
	{
I 7
D 10
	    PRINTF("hit this %d\n", $1.c_expr->e_size);
E 10
I 10
	    DEBUG_PRINTF(("hit this %d\n", $1.c_expr->e_size));
E 10
E 7
D 3
	    mkl2p(&$$, &$1);
E 3
I 3
	    mk_l2p(&$$, &$1);
E 3
	}
D 3
    | '(' pvalue ')'
E 3
I 3
    | '(' expression ')'
E 3
	{
	    $$ = $2;
	}
I 3
D 5
    | '(' type_name ')' nce
E 5
I 5
    | '(' type_name ')' pvalue
E 5
	{
	    $$ = $4;
	    cast_to(&$$, $2.a_type, $2.a_base);
	}
    | '-' pvalue
	{
	    if (mk_unary(&$$, &$2, 'u')) {
		yyerror("Unary minus must be numeric");
	    }
	}
    | '!' pvalue
	{
	    if (mk_unary(&$$, &$2, '!')) {
		yyerror("Logical-not must be numeric");
	    }
	    change_to_int(&$$);
	}
    | '~' pvalue
	{
	    if (mk_unary(&$$, &$2, '~')) {
		yyerror("Bitwise-not must be integral");
	    }
	    change_to_int(&$$);
	}
    | constant
	/* $$ = $1; */
    | name '(' opt_expression_list ')'
	{
	    if ($1.c_type->t_type != PROC_TYPE) {
		yyerror("function name expected");
		YYERROR;
	    }
	    mk_fcall(&$$, &$1, $3);
	}
E 3
    | '&' lvalue
	{
D 2
	    typeptr t = newtype($2.c_type->t_ns, PTR_TYPE, (char *)0, 0);

	    t->t_val.val_p = $2.c_type;
	    $$.c_type = t;
	    $$.c_base = base_type($$.c_type);
	    $$.c_const = 1;
	    $$.c_expr = $2.c_expr;
E 2
I 2
D 3
	    mk_f2v(&$$, &$2);
E 3
I 3
	    typeptr t1 = newtype($2.c_type->t_ns, RANGE_TYPE);
	    typeptr t2 = newtype($2.c_type->t_ns, ARRAY_TYPE);

	    t1->t_val.val_r.r_typeptr = 0;
	    t1->t_val.val_r.r_lower = 0;
	    t1->t_val.val_r.r_upper = 0;
	    t2->t_val.val_a.a_typedef = t1;
	    t2->t_val.val_a.a_typeptr = $2.c_type;

	    $$.c_type = t2;
	    $$.c_base = base_type($$.c_type);
	    $$.c_expr = $2.c_expr;
	    $$.c_const = 1;
	    $$.c_bitfield = 0;
E 3
E 2
	}
    | INCOP lvalue
	{
D 2
	    if (mk_incdec(&$$, &$2, PLUSEQUAL))
E 2
I 2
	    if (mk_incdec(ns_inter, &$$, &$2, PLUSEQUAL))
E 2
		YYERROR;
	}
    | DECOP lvalue
	{
D 2
	    if (mk_incdec(&$$, &$2, MINUSEQUAL))
E 2
I 2
	    if (mk_incdec(ns_inter, &$$, &$2, MINUSEQUAL))
E 2
		YYERROR;
	}
    | lvalue INCOP
	{
D 2
	    if (mk_incdec(&$$, &$1, INCOP))
E 2
I 2
	    if (mk_incdec(ns_inter, &$$, &$1, INCOP))
E 2
		YYERROR;
	}
    | lvalue DECOP
	{
D 2
	    if (mk_incdec(&$$, &$1, DECOP))
E 2
I 2
	    if (mk_incdec(ns_inter, &$$, &$1, DECOP))
E 2
		YYERROR;
	}
D 3
    | '(' type_name ')' nce
	{
	    $$ = $4;
D 2
	    cast_to(&$$, $2.a_type);
E 2
I 2
	    cast_to(&$$, $2.a_type, $2.a_base);
E 2
	}
    | '-' pvalue
	{
	    $$ = $2;
	}
    | '!' pvalue
	{
	    $$ = $2;
	}
    | '~' pvalue
	{
	    $$ = $2;
	}
    | constant
	/* $$ = $1; */
    | func_call
	/* $$ = $1; */
E 3
    ;

lvalue
D 3
    : name				%prec ','
E 3
I 3
    /*
     * The %prec clause below was changed from DOSHIFT to ELSE to fix
     * the problem that sizeof(*foo) was not parsing.  It is not clear
     * if this is a true fix. 950203 - pedz
     */
    : name				%prec ELSE
E 3
	{
	    $$ = $1;
	}
D 3
    | '(' lvalue ')'
	{
	    $$ = $2;
	}
E 3
    | '*' pvalue
	{
D 2
	    if ($2.c_type->t_type == PTR_TYPE) {
		$$.c_type = $2.c_type->t_val.val_p;
	    } else if ($2.c_type->t_type == ARRAY_TYPE) {
		$$.c_type = $2.c_type->t_val.val_a.a_typeptr;
	    } else {
E 2
I 2
D 3
	    if (mk_v2f(&$$, &$2)) {
E 3
I 3
	    if ($2.c_type->t_type == PTR_TYPE) {
		$$.c_type = $2.c_type->t_val.val_p;
	    } else if ($2.c_type->t_type == ARRAY_TYPE) {
		$$.c_type = $2.c_type->t_val.val_a.a_typeptr;
	    } else {
E 3
E 2
		yyerror("pointer type expected");
		YYERROR;
	    }
I 3
	    $$.c_base = base_type($$.c_type);
	    $$.c_expr = new_expr();
	    *$$.c_expr = *$2.c_expr;
I 7
	    /*
	     * O.k.  The change below took a while to figure out and
	     * it may not be right.  The logic to set the size to the
	     * size of the type divided by 8 may seem right but
	     * actually the size of the expression is the size of an
	     * address since the result is going to be an lvalue (an
	     * address).  So I changed it to be the size of a
	     * pointer.  Lets see what that breaks now...
	     */
#if 0
E 7
	    $$.c_expr->e_size = get_size($$.c_type) / 8;
I 7
#else
	    $$.c_expr->e_size = sizeof(void *);
#endif
D 10
	    PRINTF("* %08x %d\n", $$.c_expr, $$.c_expr->e_size);
E 10
I 10
	    DEBUG_PRINTF(("* %s %d\n", P($$.c_expr),
			 $$.c_expr->e_size));
E 10
E 7
	    $$.c_const = 0;
	    $$.c_bitfield = 0;
E 3
D 2
	    $$.c_base = base_type($$.c_type);
	    $$.c_const = 0;
	    $$.c_expr = new_expr();
	    *$$.c_expr = *$2.c_expr;
	    $$.c_expr->e_size = get_size($$.c_type) / 8;
E 2
	}
    ;

name
    : IDENTIFIER
	{
D 3
	    if (mkident(&$$, $1)) {
E 3
I 3
	    if (mk_ident(&$$, $1)) {
E 3
D 2
		yyerror2("identifier %s not defined", $1);
E 2
I 2
		yyerror("identifier %s not defined", $1);
E 2
		YYERROR;
	    }
	}
D 3
    | '(' name ')'
E 3
I 3
    | '(' lvalue ')'
E 3
	{
	    $$ = $2;
	}
    | name '.' IDENTIFIER
	{
D 3
	    if (mkdot(&$$, &$1, $3))
E 3
I 3
	    if (mk_dot(&$$, &$1, $3))
E 3
		YYERROR;
		
	}
I 11
    /* kludge / hack since the scanner returns a typedef name if it
       finds it.  But that can be a legal field name it turns out. */
    | name '.' TYPEDEFNAME
	{
	    if (mk_dot(&$$, &$1, $3->t_name))
		YYERROR;
		
	}
E 11
D 3
    | func_call PTROP IDENTIFIER
E 3
I 3
    | name '(' opt_expression_list ')' PTROP IDENTIFIER
E 3
	{
D 3
	    if (mkptr(&$$, &$1, $3))
E 3
I 3
	    cnode ctemp;

	    if ($1.c_type->t_type != PROC_TYPE) {
		yyerror("function name expected");
E 3
		YYERROR;
I 3
	    }
	    mk_fcall(&ctemp, &$1, $3);

	    if (mk_ptr(&$$, &ctemp, $6))
		YYERROR;
E 3
	}
    | name PTROP IDENTIFIER
	{
D 3
	    if (mkptr(&$$, &$1, $3))
E 3
I 3
	    if (mk_ptr(&$$, &$1, $3))
E 3
		YYERROR;
	}
I 3
    | '(' expression ')' PTROP IDENTIFIER
	{
	    if (mk_ptr(&$$, &$2, $5))
I 11
		YYERROR;
	}
    /* ditto of above */
    | name '(' opt_expression_list ')' PTROP TYPEDEFNAME
	{
	    cnode ctemp;

	    if ($1.c_type->t_type != PROC_TYPE) {
		yyerror("function name expected");
		YYERROR;
	    }
	    mk_fcall(&ctemp, &$1, $3);

	    if (mk_ptr(&$$, &ctemp, $6->t_name))
		YYERROR;
	}
    | name PTROP TYPEDEFNAME
	{
	    if (mk_ptr(&$$, &$1, $3->t_name))
		YYERROR;
	}
    | '(' expression ')' PTROP TYPEDEFNAME
	{
	    if (mk_ptr(&$$, &$2, $5->t_name))
E 11
		YYERROR;
	}
E 3
    | name '[' expression ']'
	{
D 3
	    if (mkarray(&$$, &$1, &$3))
E 3
I 3
	    if (mk_array(&$$, &$1, &$3))
E 3
		YYERROR;
	}
I 3
    | '(' expression ')' '[' expression ']'
	{
	    if (mk_array(&$$, &$2, &$5))
		YYERROR;
	}
E 3
    | name '.' error
	{
	    yyerror("field name expected");
D 3
	    YYERROR;
E 3
	}
    | name PTROP error
	{
	    yyerror("field name expected");
D 3
	    YYERROR;
E 3
	}
D 3
    | func_call PTROP error
	{
	    yyerror("field name expected");
	    YYERROR;
	}
E 3
    | name '[' expression error
	{
	    yyerror("']' expected");
D 3
	    YYERROR;
E 3
	}
    | '(' lvalue error
	{
	    yyerror("')' expected");
D 3
	    YYERROR;
E 3
	}
    ;

I 3

E 3
opt_expression_list
    : /* NULL */
	{
	    $$ = 0;
	}
    | expression_list
    ;

expression_list
    : nce
	{
	    cnode_list *e = new(cnode_list);
	    e->cl_next = 0;
I 5
	    e->cl_down = 0;
E 5
	    e->cl_cnode = $1;
	    $$ = e;
	}
D 2
    | nce ',' expression_list
E 2
I 2
    | expression_list ',' nce
E 2
	{
	    cnode_list *e = new(cnode_list);

D 2
	    e->cl_next = $3;
	    e->cl_cnode = $1;
E 2
I 2
	    e->cl_next = $1;
I 5
	    e->cl_down = 0;
E 5
	    e->cl_cnode = $3;
E 2
	    $$ = e;
	}
    ;

D 3
func_name
    : name
    | '(' '*' name ')'
	{
	    if ($3.c_type->t_type == PTR_TYPE) {
		$$ = $3;
		$$.c_type = $3.c_type->t_val.val_p;
	    } else {
		yyerror("pointer type expected");
		YYERROR;
	    }
	}
    ;

func_call
    : func_name '(' opt_expression_list ')'
	{
	    if ($1.c_type->t_type != PROC_TYPE) {
		yyerror("function name expected");
		YYERROR;
	    } else
		mk_fcall(&$$, &$1, $3);
	}
    | func_name '(' opt_expression_list error
    ;

E 3
constant
    : RCON
	{
D 2
	    $$.c_type = mkfloat("double", sizeof(double));
E 2
I 2
D 3
	    $$.c_type = mkfloat(ns_inter, "double", sizeof(double));
E 3
I 3
	    $$.c_type = find_type(ns_inter, TP_DOUBLE);
E 3
E 2
	    $$.c_base = double_type;
D 3
	    $$.c_const = 1;
E 3
	    $$.c_expr = new_expr();
	    $$.c_expr->e_func.d = d_leaf;
	    $$.c_expr->e_d = $1;
	    $$.c_expr->e_size = sizeof(double);
I 3
	    $$.c_const = 1;
	    $$.c_bitfield = 0;
E 3
	}
    | ICON
	{
D 2
	    mkconst(&$$, $1);
E 2
I 2
D 3
	    mkconst(ns_inter, &$$, $1);
E 3
I 3
D 10
	    mk_const(ns_inter, &$$, $1);
E 10
I 10
	    mk_const(ns_inter, &$$, $1, TP_INT);
E 10
E 3
E 2
	}
I 10
    | LCON
	{
	    if (sizeof(long) == 4)
		mk_const(ns_inter, &$$, $1, TP_LONG);
	    else
		mk_const(ns_inter, &$$, $1, TP_LONG_64);
	}
    | LLCON
	{
	    mk_const(ns_inter, &$$, $1, TP_LLONG);
	}
E 10
    | STRING
	{
D 2
	    $$.c_type = mkstrtype(strlen($1));
E 2
I 2
D 3
	    $$.c_type = mkstrtype(ns_inter, strlen($1));
E 3
I 3
	    $$.c_type = mk_strtype(ns_inter, strlen($1));
E 3
E 2
	    $$.c_base = base_type($$.c_type);
D 3
	    $$.c_const = 1;
E 3
	    $$.c_expr = new_expr();
	    $$.c_expr->e_func.ul = ul_leaf;
	    $$.c_expr->e_ul = (unsigned long)$1;
	    $$.c_expr->e_size = get_size($$.c_type) / 8;
I 3
	    $$.c_const = 1;
	    $$.c_bitfield = 0;
E 3
	}
    | SIZEOF nce
	{
D 2
	    mkconst(&$$, get_size($2.c_type) / 8);
E 2
I 2
D 3
	    mkconst(ns_inter, &$$, get_size($2.c_type) / 8);
E 3
I 3
D 10
	    mk_const(ns_inter, &$$, get_size($2.c_type) / 8);
E 10
I 10
	    mk_const(ns_inter, &$$, get_size($2.c_type) / 8, TP_INT);
E 10
E 3
E 2
	}
    | SIZEOF '(' type_name ')'
	{
D 2
	    mkconst(&$$, get_size($3.a_type) / 8);
E 2
I 2
D 3
	    mkconst(ns_inter, &$$, get_size($3.a_type) / 8);
E 3
I 3
D 10
	    mk_const(ns_inter, &$$, get_size($3.a_type) / 8);
E 10
I 10
	    mk_const(ns_inter, &$$, get_size($3.a_type) / 8, TP_INT);
E 10
E 3
E 2
	}
    | SIZEOF '(' type_name error
	{
	    yyerror("')' expected");
D 3
	    YYERROR;
E 3
	}
    ;

type_name
    : type null_decl
I 2
	{
	    $$ = $2;
	    pop_attributes;
	}
E 2
    | error
	{
	    yyerror("bad type name");
D 3
	    YYERROR;
E 3
	}
    ;

null_decl
    : /* NULL */
I 2
	{
	    $$ = current_attributes;
	}
E 2
    | non_empty_decl
    ;

non_empty_decl
    : '(' '*' null_decl ')' arg_list
I 2
	{
	    typeptr t1 = newtype($3.a_type->t_ns, PTR_TYPE);
	    typeptr t2 = newtype($3.a_type->t_ns, PROC_TYPE);
	    
	    t1->t_val.val_p = $3.a_type;
	    t2->t_val.val_f.f_typeptr = t1;
	    t2->t_val.val_f.f_params = -1;
	    t2->t_val.val_f.f_paramlist = 0;
	    $$.a_type = t1;
	    $$.a_base = base_type(t1);
	}
E 2
    | '*' null_decl
I 2
	{
	    typeptr t1 = newtype($2.a_type->t_ns, PTR_TYPE);
	    
	    $$ = $2;
	    t1->t_val.val_p = $2.a_type;
	    $$.a_type = t1;
	    $$.a_base = base_type(t1);
	}
E 2
    | null_decl '[' opt_constant_expression ']'
I 2
	{
	    typeptr t1 = newtype($1.a_type->t_ns, RANGE_TYPE);
	    typeptr t2 = newtype($1.a_type->t_ns, ARRAY_TYPE);

	    t1->t_val.val_r.r_typeptr = 0;
	    t1->t_val.val_r.r_lower = 0;
D 6
	    t1->t_val.val_r.r_upper = $3 - 1;
E 6
I 6
	    t1->t_val.val_r.r_upper = save_as_string($3 - 1);
E 6
	    t2->t_val.val_a.a_typedef = t1;
	    t2->t_val.val_a.a_typeptr = $1.a_type;
	    $$.a_class = $1.a_class;
	    $$.a_type = t2;
	    $$.a_base = ulong_type;
	}
E 2
    | '(' non_empty_decl ')'
I 2
	{
	    $$ = $2;
	}
E 2
    ;

%%

D 10
#include <stdio.h>
I 8
#include "scan.h"
E 8

E 10
D 2
yyerror2(char *s, char *n)
E 2
I 2
D 5
static int nesting_level;
static symptr current_function;
static long global_index = (long)h_base;
static int param_index;
static int variable_index;
static int variable_max;

E 5
void yyerror(char *s, ...)
E 2
{
D 2
    char smallbuf[128];
E 2
I 2
D 8
    extern int yylineno;
    extern char *yyfilename;
E 8
    va_list Argp;
    char smallbuf[256];
    char *f = yyfilename ? yyfilename : "<stdin>";
E 2

D 2
    sprintf(smallbuf, s, n);
    yyerror(smallbuf);
E 2
I 2
    va_start(Argp, s);
    vsprintf(smallbuf, s, Argp);
    va_end(Argp);
    fprintf(stderr, "%s:%d %s\n", f, yylineno, smallbuf);
E 2
}

D 2
yyerror(char *s)
E 2
I 2
D 5
static symptr gram_enter_sym(anode *attr, int line)
E 5
I 5
static symptr gram_enter_sym(anode *attr, int line, cnode_list *init)
E 5
E 2
{
D 2
    fprintf(stderr, "%s\n", s);
E 2
I 2
    symptr ret;

    if (!attr->a_valid_name)
	yyerror("No valid name from line %d", line);
    if (!attr->a_valid_type)
	yyerror("No valid type from line %d", line);
    if (!attr->a_valid_base)
	yyerror("No valid base from line %d", line);
    if (!attr->a_valid_class)
	yyerror("No valid class from line %d", line);
D 4
    ret = enter_sym(ns_inter, attr->a_name);
E 4
I 4
    ret = enter_sym(ns_inter, attr->a_name, 0);

    /*
     * There are many cases where a symbol is not a duplicate but this solves
     * most of them.
     */
E 4
    if (ret->s_defined)
D 4
	yyerror("Duplicate symbol");
E 4
I 4
D 6
	if (ret->s_nesting == nesting_level)
	    yyerror("Duplicate symbol");
	else
E 6
I 6
	if (ret->s_nesting == nesting_level) {
	    if (0) {
		yyerror("Duplicate symbol");
		return 0;
	    } else {
		return ret;
	    }
	} else {
E 6
	    ret = enter_sym(ns_inter, attr->a_name, 1);
I 6
	}

E 6
E 4
    ret->s_defined = 1;
    ret->s_base = attr->a_base;
    ret->s_type = attr->a_type;
    ret->s_nesting = nesting_level;
    ret->s_typed = 1;

    /*
     * No storage type specified and we are not in a function so we assume it
     * is an extern.
     */
    if (attr->a_class == auto_class && !current_function)
	attr->a_class = extern_class;
    if (attr->a_class == extern_class || attr->a_class == static_class) {
	ret->s_global = 1;
	if (ret->s_type->t_type == PROC_TYPE) {
	    ret->s_stmt = NO_STMT;
	    ret->s_size = 0;
	} else {
	    int size = get_size(ret->s_type) / 8;

	    ret->s_offset = (v_ptr)global_index;
	    ret->s_size = size;
	    global_index += size;
	}
    } else {
	int size = ret->s_size = get_size(ret->s_type) / 8;
	
	if (attr->a_class == param_class) {
D 10
	    ret->s_offset = (v_ptr)param_index;
I 8
#if 0
E 8
	    param_index += size;
I 8
#else
	    param_index += (((size + sizeof(long)-1)/sizeof(long))*sizeof(long));
#endif
E 10
I 10
	    if (size < sizeof(long))
		ret->s_offset = (v_ptr)(param_index + (sizeof(long) - size));
	    else
		ret->s_offset = (v_ptr)param_index;
	    param_index += (((size + sizeof(long)-1) / sizeof(long)) *
			    sizeof(long));
E 10
E 8
	} else {
	    variable_index += size;
	    ret->s_offset = (v_ptr)-variable_index;
	    if (variable_index > variable_max)
		variable_max = variable_index;
	}
	ret->s_global = 0;
    }
    return ret;
}

static int increase_nesting(void)
{
    nesting_level++;
    return variable_index;
}

static void decrease_nesting(int new)
{
    --nesting_level;
    clean_symtable(ns_inter, nesting_level);
    variable_index = new;
}

static void start_function(symptr f)
{
    current_function = f;
D 8
    param_index = sizeof(int);
E 8
I 8
    param_index = sizeof(frame_ptr);
E 8
    variable_max = variable_index = 0;
}

static void end_function(void)
{
    set_alloc(current_function->s_stmt, variable_max);
    current_function = 0;
}

static void do_parameter_allocation(arg_list *old_list, arg_list *args)
{
    arg_list *a1, *a2;

    /*
     * We run throught the list of old style C declarations for the
     * arguments and match them up with those specified in the
     * argument list.
     */
    for (a1 = old_list; a2 = a1; a1 = a1->a_next, free(a2)) {
	arg_list *p;

	for (p = args; p; p = p->a_next)
	    if (!strcmp(p->a_anode.a_name, a1->a_anode.a_name)) {
		if (p->a_anode.a_type)
		    fprintf(stderr, "Double declaration for argument %s\n",
			    a1->a_anode.a_name);
		else
		    p->a_anode = a1->a_anode;
		break;
	    }
	if (!p)				/* not found */
	    fprintf(stderr,
		    "Arg %s declarated but not in argument list\n",
		    a1->a_anode.a_name);
    }

    /*
     * Now actually enter the symbols
     */
    for (a1 = args; a1; a1 = a1->a_next) {
	a1->a_anode.a_class = param_class;
	a1->a_anode.a_valid_class = 1;
D 5
	(void) gram_enter_sym(&a1->a_anode, __LINE__);
E 5
I 5
	(void) gram_enter_sym(&a1->a_anode, __LINE__, (cnode_list *)0);
E 5
    }
I 3
}

I 5
#if 0
E 5
static void prompt(void)
{
    extern FILE *yyin;

    if (isatty(fileno(yyin))) {
	putchar('>');
	putchar(' ');
	fflush(stdout);
    }
}
D 5

static int allocate_fields(fieldptr f)
{
    int pos = 0;

    for ( ; f; f = f->f_next) {
	/* if foo : size was specified */
	if (f->f_numbits < 0)
	    f->f_numbits = -f->f_numbits;
	else {
	    /* No size specified so move to proper boundry */
	    pos += (f->f_numbits - 1);
	    pos &= ~(f->f_numbits - 1);
	}
	f->f_offset = pos;
	pos += f->f_numbits;
    }
    return pos;
}
E 5
I 5
#endif
E 5

static void change_to_int(cnode *c)
{
    c->c_type = find_type(ns_inter, TP_INT);
    c->c_base = int_type;
    c->c_expr->e_size = sizeof(int);
I 6
}

static char *save_as_string(int i)
{
    char buf[32];

    sprintf(buf, "%d", i);
    return store_string(ns_inter, buf, 0, (char *)0);
E 6
E 3
E 2
}
E 1
