%{
static char sccs_id[] = "@(#)gram.y	1.2";
/*
 * The yacc grammer for the user interface language.  This should grow
 * into about 90% of C in time.
 */

#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "map.h"
#include "sym.h"
#include "tree.h"
#include "stmt.h"
#include "base_expr.h"
#include "inter.h"
#include "dex.h"

ns *ns_inter;
static anode anode_stack[8];
static int anode_index = -1;
#define current_attributes (anode_stack[anode_index])

#define push_attributes(anode) (anode_stack[++anode_index] = (anode))
#define pop_attributes (--anode_index)

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

#define PROMPT (putchar('>'),putchar(' '),fflush(stdout))
void yyerror(char *s, ...);
%}

%union {
    int val;				/* ints, etc */
    double rval;			/* reals */
    char *str;				/* names, etc */
    typeptr type;
    cnode cnode;			/* expression tree */
    anode anode;			/* class/type pair */
    stmt_index stmt;			/* statement pointer */
    cnode_list *clist;			/* expression list */
    arg_list *alist;			/* argument list */
}

/* keywords used for the commands of the debugger */
%token DUMP
%token LOAD
%token PRINT
%token SOURCE
%token SYM_DUMP

/* keywords used for C -- all of this might be used at the current time */

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

/*
 *  Token from scanner which should not exist but do to make the
 * grammer LR(1).  These symbols are for +=, -=, etc ...
 */

%token <val> PLUSEQUAL MINUSEQUAL TIMESEQUAL DIVEQUAL MODEQUAL LSEQUAL
%token <val> RSEQUAL ANDEQUAL XOREQUAL OREQUAL '=' ',' '*' '/' '%' '+'
%token <val> '-' RSHIFT LSHIFT '<' '>' GTOREQUAL LTOREQUAL EQUALITY
%token <val> NOTEQUAL '&' '^' '|' ANDAND OROR

/*
 * Tokens from the scanner which must and should exist but are not
 * keywords.  These are identifiers, constants, and double operands
 * such as &&, ||, ->, etc.
 */

%token GTOREQUAL LTOREQUAL LSHIFT RSHIFT INCOP DECOP PTROP OROR ANDAND
%token EQUALITY NOTEQUAL

%token <val> ICON
%token <rval>  RCON
%token <str> IDENTIFIER STRING
%token <type> TYPEDEFNAME

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
%type <anode> type_name non_type_def class non_function_declarator
%type <anode> function_declarator ptr_function_declarator null_decl
%type <anode> non_empty_decl

%type <cnode> constant name lvalue pvalue expression func_call
%type <cnode> func_name opt_expression nce

%type <val> opt_constant_expression constant_expression asgn_op
%type <val> compound_statement_preamble function_heading

%type <stmt> compound_statement statement_list statement if_preamble
%type <stmt> else_preamble data_def init_declaration_list
%type <stmt> init_declarator function_body declaration_stat_list

%type <clist> expression_list opt_expression_list

%type <alist> name_list typed_name_list typed_name arg_declaration
%type <alist> arg_declaration_list arg_declarator_list

%start user_input

%%
user_input
    : { PROMPT; } command
	{
	    PROMPT;
	}
    | user_input command
	{
	    PROMPT;
	}
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
    | LOAD STRING ICON ICON ';'
	{
	    load($2, $3, $4);
	}
    | SOURCE STRING ';'
	{
	    add_source($2);
	}
    | data_def 
	{
	    (void) mk_return_stmt((expr *)0);
	    (void) execute_statement($1);
	}
    | statement 
	{
	    (void) mk_return_stmt((expr *)0);
	    (void) execute_statement($1);
	}
    ;

data_def			/* a single global definition */
    : attributes init_declaration_list ';'
	{
	    $$ = $2;
	    pop_attributes;
	}
    | attributes function_heading function_body
	{
	    (void) mk_return_stmt((expr *)0);
	    decrease_nesting($2);
	    end_function();
	    resolv_gotos();
	    $$ = NO_STMT;
	    pop_attributes;
	}
    | attributes error
	{
	    $$ = NO_STMT;
	    pop_attributes;
	}
    | attributes init_declaration_list error
	{
	    $$ = NO_STMT;
	    pop_attributes;
	}
    ;

function_heading
    : function_declarator arg_declaration_list 
	{
	    symptr f;

	    if ($1.a_class == typedef_class) {
		yyerror("Can not typedef a function");
		YYERROR;
	    }
	    f = gram_enter_sym(&$1, __LINE__);
	    $$ = increase_nesting();
	    f->s_stmt = mk_alloc_stmt();
	    f->s_size = sizeof(stmt_index);
	    start_function(f);
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
	    pop_attributes;
	}
    | attributes ';'
	{
	    $$ = 0;
	    pop_attributes;
	}
    ;

arg_declarator_list		/* argument declarations (old C style */
    : non_function_declarator
	{
	    $$ = new(arg_list);
	    $$->a_next = 0;
	    $$->a_anode = $1;
	}
    | arg_declarator_list ',' non_function_declarator
	{
	    $$ = new(arg_list);
	    $$->a_next = $1;
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
	    pop_attributes;
	}
    | declaration_stat_list attributes init_declaration_list ';'
	{
	    $$ = ($1 != NO_STMT) ? $1 : $3;
	    pop_attributes;
	}
    | /* NULL */
	{
	    $$ = NO_STMT;
	}
    ;

attributes			/* forced attrs for arguments and local vars */
    : class type
	{
	    current_attributes.a_class = $1.a_class;
	    current_attributes.a_valid_class = 1;
	}
    | class
	{
	    push_attributes($1);
	    current_attributes.a_type = mkrange(ns_inter, "int",
						0x80000000, 0x7fffffff);
	    current_attributes.a_valid_type = 1;
	    current_attributes.a_base = int_type;
	    current_attributes.a_valid_base = 1;
	}
    | type
	{
	    current_attributes.a_class = auto_class;
	    current_attributes.a_valid_class = 1;
	}
    ;

class				/* storage class of a var or func */
    : non_type_def
	{
	    $$.a_class = $1.a_class;
	    $$.a_valid_class = 1;
	    $$.a_valid_type = 0;
	    $$.a_valid_base = 0;
	    $$.a_valid_name = 0;
	}
    | TYPEDEF
	{
	    $$.a_class = typedef_class;
	    $$.a_valid_class = 1;
	    $$.a_valid_type = 0;
	    $$.a_valid_base = 0;
	    $$.a_valid_name = 0;
	}
    ;

non_type_def
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
    ;

type				/* data type of a var or func */
    : simple_type
	{
	    $$ = $1;
	    $$.a_valid_type = 1;
	    $$.a_valid_base = 1;
	    $$.a_valid_class = 0;
	    $$.a_valid_name = 0;
	    push_attributes($$);
	}
    | struct_declaration
	{
	    $$ = $1;
	    $$.a_valid_type = 1;
	    $$.a_valid_base = 1;
	    $$.a_valid_class = 0;
	    $$.a_valid_name = 0;
	    push_attributes($$);
	}
    | enum_declaration
	{
	    $$ = $1;
	    $$.a_valid_type = 1;
	    $$.a_valid_base = 1;
	    $$.a_valid_class = 0;
	    $$.a_valid_name = 0;
	    push_attributes($$);
	}
    | TYPEDEFNAME
	{
	    $$.a_type = $1;
	    $$.a_valid_type = 1;
	    $$.a_base = base_type($1);
	    $$.a_valid_base = 1;
	    $$.a_valid_class = 0;
	    $$.a_valid_name = 0;
	    push_attributes($$);
	}
    ;

simple_type			/* predefined types */
    : CHAR
	{
	    $$.a_type = mkrange(ns_inter, "char", 0, 255);
	    $$.a_base = uchar_type; /* should be an option */
	}
    | SHORT
	{
	    $$.a_type = mkrange(ns_inter, "short", 0xffff8000, 0x00007fff);
	    $$.a_base = short_type;
	}
    | INT
	{
	    $$.a_type = mkrange(ns_inter, "int", 0x80000000, 0x7fffffff);
	    $$.a_base = int_type;
	}
    | LONG
	{
	    $$.a_type = mkrange(ns_inter, "long", 0x80000000, 0x7fffffff);
	    $$.a_base = long_type;
	}
    | UNSIGNED
	{
	    $$.a_type = mkrange(ns_inter, "unsigned", 0, 0xffffffff);
	    $$.a_base = uint_type;
	}
    | FLOAT
	{
	    $$.a_type = mkfloat(ns_inter, "float", sizeof(float));
	    $$.a_base = float_type;
	}
    | DOUBLE
	{
	    $$.a_type = mkfloat(ns_inter, "double", sizeof(double));
	    $$.a_base = double_type;
	}
    | VOID
	{
	    $$.a_type = mkrange(ns_inter, "void", 0, 0);
	    $$.a_base = void_type;
	}
    | UNSIGNED CHAR
	{
	    $$.a_type = mkrange(ns_inter, "unsigned char", 0, 255);
	    $$.a_base = uchar_type;
	}
    | SIGNED CHAR
	{
	    $$.a_type = mkrange(ns_inter, "signed char", -128, 127);
	    $$.a_base = schar_type;
	}
    | UNSIGNED SHORT
	{
	    $$.a_type = mkrange(ns_inter, "unsigned short", 0, 0xffff);
	    $$.a_base = ushort_type;
	}
    | UNSIGNED INT
	{
	    $$.a_type = mkrange(ns_inter, "unsigned int", 0, 0xffffffff);
	    $$.a_base = uint_type;
	}
    | UNSIGNED LONG
	{
	    $$.a_type = mkrange(ns_inter, "unsigned long", 0, 0xffffffff);
	    $$.a_base = ulong_type;
	}
    ;

enum_declaration
    : enum_head '{' member_list opt_comma '}'
	{
	    /* #### enumeration definitions */
	    yyerror("can't declare enumeration types yet");
	    YYERROR;
	}
    | ENUM IDENTIFIER
	{
	    if (!($$.a_type = name2namedef_all($2)) ||
		$$.a_type->t_type != ENUM_TYPE) {
		yyerror("enum %s not found", $2);
		YYERROR;
	    }
	    $$.a_base = int_type;
	}
    | ENUM error
	{
	    $$.a_type = 0;
	}
    ;

enum_head
    : ENUM
    | ENUM IDENTIFIER
    ;

member_list
    : member
    | member_list ',' member
    ;

member
    : IDENTIFIER
    | IDENTIFIER '=' constant_expression
    ;

struct_declaration
    : str_head '{' type_declaration_list opt_semi '}'
	{
	    /* #### structure definitions */
	    yyerror("can't declare structs and unions yet");
	    YYERROR;
	}
    | struct_union IDENTIFIER
	{
	    if (!($$.a_type = name2namedef_all($2))) {
		yyerror("struct or union %s not found", $2);
		YYERROR;
	    }
	    $$.a_base = struct_type;
	}
    | struct_union error
	{
	    $$.a_type = 0;
	}
    ;

str_head
    : struct_union
    | struct_union IDENTIFIER
    ;

struct_union
    : STRUCT
    | UNION
    ;

type_declaration_list		/* list of struct fields w/ type */
    : type_declaration
    | type_declaration_list ';' type_declaration
    ;

type_declaration		/* a struct field w/ type */
    : type declarator_list	/* #### */
	{
	    pop_attributes;
	}
    | type
	{
	    pop_attributes;
	}
    ;

declarator_list			/* fields after TYPE for a struct */
    : declarator
    | declarator_list ',' declarator
    ;

declarator			/* a single field in a struct */
    : non_function_declarator
    | non_function_declarator ':' constant_expression
    | ':' constant_expression
    ;

non_function_declarator		/* argument, var, or field */
    : '*' non_function_declarator
	{
	    typeptr t = newtype($2.a_type->t_ns, PTR_TYPE);

	    $$ = $2;
	    $$.a_type = t;
	    $$.a_base = ulong_type;
	    t->t_val.val_p = $2.a_type;
	}
    | '(' '*' non_function_declarator ')' arg_list
	{
	    typeptr t1 = newtype($3.a_type->t_ns, PTR_TYPE);
	    typeptr t2 = newtype($3.a_type->t_ns, PROC_TYPE);

	    $$ = $3;
	    $$.a_type = t1;
	    $$.a_base = ulong_type;
	    t1->t_val.val_p = t2;
	    t2->t_val.val_f.f_typeptr = $3.a_type;
	    t2->t_val.val_f.f_params = -1;
	    t2->t_val.val_f.f_paramlist = 0;
	}
    | non_function_declarator '[' opt_constant_expression ']'
	{
	    typeptr t1 = newtype($1.a_type->t_ns, RANGE_TYPE);
	    typeptr t2 = newtype($1.a_type->t_ns, ARRAY_TYPE);

	    $$ = $1;
	    t1->t_val.val_r.r_typeptr = 0;
	    t1->t_val.val_r.r_lower = 0;
	    t1->t_val.val_r.r_upper = $3 - 1;
	    t2->t_val.val_a.a_typedef = t1;
	    t2->t_val.val_a.a_typeptr = $1.a_type;
	    $$.a_type = t2;
	    $$.a_base = ulong_type;
	}
    | '(' non_function_declarator ')'
	{
	    $$ = $2;
	}
    | IDENTIFIER
	{
	    $$ = current_attributes;
	    $$.a_name = $1;
	    $$.a_valid_name = 1;
	}
    ;

function_declarator		/* function name */
    : '*' function_declarator
	{
	    typeptr t = newtype($2.a_type->t_ns, PTR_TYPE);

	    $$ = $2;
	    $$.a_type = t;
	    $$.a_base = ulong_type;
	    t->t_val.val_p = $2.a_type;
	}
    | ptr_function_declarator arg_list
	{
	    typeptr t2 = newtype($1.a_type->t_ns, PROC_TYPE);

	    $$ = $1;
	    $$.a_type = t2;
	    $$.a_base = ulong_type;
	    t2->t_val.val_f.f_typeptr = $1.a_type;
	    t2->t_val.val_f.f_params = -1;
	    t2->t_val.val_f.f_paramlist = 0;
	}
    | ptr_function_declarator '[' opt_constant_expression ']'
	{
	    typeptr t1 = newtype($1.a_type->t_ns, RANGE_TYPE);
	    typeptr t2 = newtype($1.a_type->t_ns, ARRAY_TYPE);

	    $$ = $1;
	    t1->t_val.val_r.r_typeptr = 0;
	    t1->t_val.val_r.r_lower = 0;
	    t1->t_val.val_r.r_upper = $3 - 1;
	    t2->t_val.val_a.a_typedef = t1;
	    t2->t_val.val_a.a_typeptr = $1.a_type;
	    $$.a_type = t2;
	    $$.a_base = ulong_type;
	}
    | '(' function_declarator ')'
	{
	    $$ = $2;
	}
    | IDENTIFIER arg_list
	{
	    typeptr t2;

	    $$ = current_attributes;
	    $$.a_name = $1;
	    $$.a_valid_name = 1;
	    $$.a_base = ulong_type;
	    t2 = newtype($$.a_type->t_ns, PROC_TYPE);
	    t2->t_val.val_f.f_typeptr = $$.a_type;
	    t2->t_val.val_f.f_params = -1;
	    t2->t_val.val_f.f_paramlist = 0;
	    $$.a_type = t2;
	}
    ;

ptr_function_declarator		/* function returning pointer to ... */
    : '(' '*' function_declarator ')'
	{
	    typeptr t = newtype($3.a_type->t_ns, PTR_TYPE);

	    $$ = $3;
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
	    bzero(&$$, sizeof($$));
	    $$->a_anode.a_name = $1;
	    $$->a_anode.a_valid_name = 1;
	    param_count = 1;
	}
    | IDENTIFIER ',' name_list
	{
	    $$ = new(arg_list);
	    bzero(&$$, sizeof($$));
	    $$->a_next = $3;
	    $$->a_anode.a_name = $1;
	    $$->a_anode.a_valid_name = 1;
	    ++param_count;
	}
    ;

typed_name_list			/* simple list of formal arguments w/ types */
    : typed_name
	{
	    $$ = $1;
	    param_count = 1;
	}
    | typed_name ',' typed_name_list
	{
	    $$ = $1;
	    $$->a_next = $3;
	    ++param_count;
	}
    ;

typed_name			/* a single formal arguemnt w/ type */
    : type non_function_declarator
	{
	    $$ = new(arg_list);
	    $$->a_next = 0;
	    $$->a_anode = $2;
	    pop_attributes;
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
	    if ($1.a_class == typedef_class) {
		typeptr t = newtype(ns_inter, $1.a_type->t_type);
		add_typedef(t, $1.a_name);
		copy_type(t, $1.a_type);
	    } else {
		(void) gram_enter_sym(&$1, __LINE__);
	    }
	    $$ = NO_STMT;
	}
    | function_declarator
	{
	    if ($1.a_class == typedef_class) {
		typeptr t = newtype(ns_inter, $1.a_type->t_type);
		add_typedef(t, $1.a_name);
		copy_type(t, $1.a_type);
	    } else {
		(void) gram_enter_sym(&$1, __LINE__);
	    }
	    $$ = NO_STMT;
	}
    | non_function_declarator '=' nce
	{
	    cnode id;
	    cnode asgn;

	    if ($1.a_class == typedef_class) {
		yyerror("Cannot initialize a typedef");
		YYERROR;
	    }
	    (void) gram_enter_sym(&$1, __LINE__);
	    if (mkident(&id, $1.a_name)) {
		yyerror("identifier %s not defined", $1.a_name);
		YYERROR;
	    }
	    if (mkasgn(&asgn, &id, $2, &$3))
		YYERROR;
	    $$ = mk_expr_stmt(asgn.c_expr);
	}
    | non_function_declarator '=' '{' init_list opt_comma '}'
	{
	    if ($1.a_class == typedef_class) {
		yyerror("Cannot initialize a typedef");
		YYERROR;
	    }
	    (void) gram_enter_sym(&$1, __LINE__);
	}
    ;

init_list
    : initializer
    | init_list ',' initializer
    ;

initializer
    : nce
    | '{' init_list opt_comma '}'
    ;

opt_comma
    : /* NULL */
    | ','
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
	    $$ = mk_expr_stmt($1.c_expr);
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
	    /* breaks will save address of goto stmt */
	    push_breaks(mk_goto_stmt(NO_STMT));
	    push_conts(NO_STMT);
	}
	statement
	{
	    stmt_index br;

	    (void)link_conts();
	    br = mk_br_true(cast_to_int(&$3), $6);
	    link_stmt(link_breaks(), br);
	    $$ = br;
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
	    (void) link_breaks();
	}
    | FOR '(' opt_expression ';' opt_expression ';' opt_expression ')'
	{
	    stmt_index t;

	    t = mk_expr_stmt($3.c_expr);
	    push_breaks(t);
	    t = mk_br_false(cast_to_int(&$5), NO_STMT);
	    push_conts(t);
	}
	statement
	{
	    stmt_index t;

	    t = link_conts();
	    (void) mk_expr_stmt($7.c_expr);
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
	    if (current_function)
		cast_to(&$2, current_function->s_type, current_function->s_base);
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
    | DEFAULT ':'
    ;

/* expression */

opt_expression
    : /* NULL */
	{
	    $$.c_type = 0;
	    $$.c_base = int_type;
	    $$.c_const = 1;
	    $$.c_expr = 0;
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
	    } else if (!$1.c_const) {
		yyerror("expression is not a constant");
		YYERROR;
	    } else
		$$ = i_val($1.c_expr);
	}
    ;

expression
    : nce
    | expression ',' nce
	{
	    if (mkbinary(&$$, &$1, $2, &$3))
		YYERROR;
	}
    ;

/* nce is for non-comma-expression */
nce
    : nce '*' nce
	{
	    if (mkbinary(&$$, &$1, $2, &$3))
		YYERROR;
	}
    | nce '/' nce
	{
	    if (mkbinary(&$$, &$1, $2, &$3))
		YYERROR;
	}
    | nce '%' nce
	{
	    if (mkbinary(&$$, &$1, $2, &$3))
		YYERROR;
	}
    | nce '+' nce
	{
	    if (mkbinary(&$$, &$1, $2, &$3))
		YYERROR;
	}
    | nce '-' nce
	{
	    if (mkbinary(&$$, &$1, $2, &$3))
		YYERROR;
	}
    | nce RSHIFT nce
	{
	    if (mkbinary(&$$, &$1, $2, &$3))
		YYERROR;
	}
    | nce LSHIFT nce
	{
	    if (mkbinary(&$$, &$1, $2, &$3))
		YYERROR;
	}
    | nce '<' nce
	{
	    if (mkbinary(&$$, &$1, $2, &$3))
		YYERROR;
	}
    | nce '>' nce
	{
	    if (mkbinary(&$$, &$1, $2, &$3))
		YYERROR;
	}
    | nce GTOREQUAL nce
	{
	    if (mkbinary(&$$, &$1, $2, &$3))
		YYERROR;
	}
    | nce LTOREQUAL nce
	{
	    if (mkbinary(&$$, &$1, $2, &$3))
		YYERROR;
	}
    | nce EQUALITY nce
	{
	    if (mkbinary(&$$, &$1, $2, &$3))
		YYERROR;
	}
    | nce NOTEQUAL nce
	{
	    if (mkbinary(&$$, &$1, $2, &$3))
		YYERROR;
	}
    | nce '&' nce
	{
	    if (mkbinary(&$$, &$1, $2, &$3))
		YYERROR;
	}
    | nce '^' nce
	{
	    if (mkbinary(&$$, &$1, $2, &$3))
		YYERROR;
	}
    | nce '|' nce
	{
	    if (mkbinary(&$$, &$1, $2, &$3))
		YYERROR;
	}
    | nce ANDAND nce
	{
	    if (mkbinary(&$$, &$1, $2, &$3))
		YYERROR;
	}
    | nce OROR nce
	{
	    if (mkbinary(&$$, &$1, $2, &$3))
		YYERROR;
	}
    | nce '?' nce ':' nce
	{
	    if (mk_qc_op(&$$, &$1, &$3, &$5))
		YYERROR;
	}
    | lvalue asgn_op nce		%prec '='
	{
	    if (mkasgn(&$$, &$1, $2, &$3))
		YYERROR;
	}
    | '(' expression ')'
	{
	    $$ = $2;
	}
    | pvalue				%prec ','
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
    ;

pvalue
    : lvalue				%prec ','
	{
	    mkl2p(&$$, &$1);
	}
    | '(' pvalue ')'
	{
	    $$ = $2;
	}
    | '&' lvalue
	{
	    mk_f2v(&$$, &$2);
	}
    | INCOP lvalue
	{
	    if (mk_incdec(ns_inter, &$$, &$2, PLUSEQUAL))
		YYERROR;
	}
    | DECOP lvalue
	{
	    if (mk_incdec(ns_inter, &$$, &$2, MINUSEQUAL))
		YYERROR;
	}
    | lvalue INCOP
	{
	    if (mk_incdec(ns_inter, &$$, &$1, INCOP))
		YYERROR;
	}
    | lvalue DECOP
	{
	    if (mk_incdec(ns_inter, &$$, &$1, DECOP))
		YYERROR;
	}
    | '(' type_name ')' nce
	{
	    $$ = $4;
	    cast_to(&$$, $2.a_type, $2.a_base);
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
    ;

lvalue
    : name				%prec ','
	{
	    $$ = $1;
	}
    | '(' lvalue ')'
	{
	    $$ = $2;
	}
    | '*' pvalue
	{
	    if (mk_v2f(&$$, &$2)) {
		yyerror("pointer type expected");
		YYERROR;
	    }
	}
    ;

name
    : IDENTIFIER
	{
	    if (mkident(&$$, $1)) {
		yyerror("identifier %s not defined", $1);
		YYERROR;
	    }
	}
    | '(' name ')'
	{
	    $$ = $2;
	}
    | name '.' IDENTIFIER
	{
	    if (mkdot(&$$, &$1, $3))
		YYERROR;
		
	}
    | func_call PTROP IDENTIFIER
	{
	    if (mkptr(&$$, &$1, $3))
		YYERROR;
	}
    | name PTROP IDENTIFIER
	{
	    if (mkptr(&$$, &$1, $3))
		YYERROR;
	}
    | name '[' expression ']'
	{
	    if (mkarray(&$$, &$1, &$3))
		YYERROR;
	}
    | name '.' error
	{
	    yyerror("field name expected");
	    YYERROR;
	}
    | name PTROP error
	{
	    yyerror("field name expected");
	    YYERROR;
	}
    | func_call PTROP error
	{
	    yyerror("field name expected");
	    YYERROR;
	}
    | name '[' expression error
	{
	    yyerror("']' expected");
	    YYERROR;
	}
    | '(' lvalue error
	{
	    yyerror("')' expected");
	    YYERROR;
	}
    ;

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
	    e->cl_cnode = $1;
	    $$ = e;
	}
    | expression_list ',' nce
	{
	    cnode_list *e = new(cnode_list);

	    e->cl_next = $1;
	    e->cl_cnode = $3;
	    $$ = e;
	}
    ;

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

constant
    : RCON
	{
	    $$.c_type = mkfloat(ns_inter, "double", sizeof(double));
	    $$.c_base = double_type;
	    $$.c_const = 1;
	    $$.c_expr = new_expr();
	    $$.c_expr->e_func.d = d_leaf;
	    $$.c_expr->e_d = $1;
	    $$.c_expr->e_size = sizeof(double);
	}
    | ICON
	{
	    mkconst(ns_inter, &$$, $1);
	}
    | STRING
	{
	    $$.c_type = mkstrtype(ns_inter, strlen($1));
	    $$.c_base = base_type($$.c_type);
	    $$.c_const = 1;
	    $$.c_expr = new_expr();
	    $$.c_expr->e_func.ul = ul_leaf;
	    $$.c_expr->e_ul = (unsigned long)$1;
	    $$.c_expr->e_size = get_size($$.c_type) / 8;
	}
    | SIZEOF nce
	{
	    mkconst(ns_inter, &$$, get_size($2.c_type) / 8);
	}
    | SIZEOF '(' type_name ')'
	{
	    mkconst(ns_inter, &$$, get_size($3.a_type) / 8);
	}
    | SIZEOF '(' type_name error
	{
	    yyerror("')' expected");
	    YYERROR;
	}
    ;

type_name
    : type null_decl
	{
	    $$ = $2;
	    pop_attributes;
	}
    | error
	{
	    yyerror("bad type name");
	    YYERROR;
	}
    ;

null_decl
    : /* NULL */
	{
	    $$ = current_attributes;
	}
    | non_empty_decl
    ;

non_empty_decl
    : '(' '*' null_decl ')' arg_list
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
    | '*' null_decl
	{
	    typeptr t1 = newtype($2.a_type->t_ns, PTR_TYPE);
	    
	    $$ = $2;
	    t1->t_val.val_p = $2.a_type;
	    $$.a_type = t1;
	    $$.a_base = base_type(t1);
	}
    | null_decl '[' opt_constant_expression ']'
	{
	    typeptr t1 = newtype($1.a_type->t_ns, RANGE_TYPE);
	    typeptr t2 = newtype($1.a_type->t_ns, ARRAY_TYPE);

	    t1->t_val.val_r.r_typeptr = 0;
	    t1->t_val.val_r.r_lower = 0;
	    t1->t_val.val_r.r_upper = $3 - 1;
	    t2->t_val.val_a.a_typedef = t1;
	    t2->t_val.val_a.a_typeptr = $1.a_type;
	    $$.a_class = $1.a_class;
	    $$.a_type = t2;
	    $$.a_base = ulong_type;
	}
    | '(' non_empty_decl ')'
	{
	    $$ = $2;
	}
    ;

%%

#include <stdio.h>

static int nesting_level;
static symptr current_function;
static long global_index = (long)h_base;
static int param_index;
static int variable_index;
static int variable_max;

void yyerror(char *s, ...)
{
    extern int yylineno;
    extern char *yyfilename;
    va_list Argp;
    char smallbuf[256];
    char *f = yyfilename ? yyfilename : "<stdin>";

    va_start(Argp, s);
    vsprintf(smallbuf, s, Argp);
    va_end(Argp);
    fprintf(stderr, "%s:%d %s\n", f, yylineno, smallbuf);
}

static symptr gram_enter_sym(anode *attr, int line)
{
    symptr ret;

    if (!attr->a_valid_name)
	yyerror("No valid name from line %d", line);
    if (!attr->a_valid_type)
	yyerror("No valid type from line %d", line);
    if (!attr->a_valid_base)
	yyerror("No valid base from line %d", line);
    if (!attr->a_valid_class)
	yyerror("No valid class from line %d", line);
    ret = enter_sym(ns_inter, attr->a_name);
    if (ret->s_defined)
	yyerror("Duplicate symbol");
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
	    ret->s_offset = (v_ptr)param_index;
	    param_index += size;
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
    param_index = sizeof(int);
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
	(void) gram_enter_sym(&a1->a_anode, __LINE__);
    }
}
