%{
static char sccs_id[] = "@(#)gram.y	1.9";
/*
 * The yacc grammer for the user interface language.  This should grow
 * into about 90% of C in time.
 */

#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <dbxstclass.h>
#include "dex.h"
#include "map.h"
#include "sym.h"
#include "tree.h"
#include "stmt.h"
#include "unary_expr.h"
#include "inter.h"
#include "fcall.h"

#define YYDEBUG 1

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

static int nesting_level;
static symptr current_function;
static int param_index;
static int variable_index;
static int variable_max;

long global_index;

void yyerror(char *s, ...);
static symptr gram_enter_sym(anode *attr, int line, cnode_list *init);
static int increase_nesting(void);
static void decrease_nesting(int new);
static void start_function(symptr f);
static void end_function(void);
static void do_parameter_allocation(arg_list *old_list, arg_list *args);
static void change_to_int(cnode *c);
static char *save_as_string(int i);

%}

%union {
    large_t val;			/* ints, etc */
    double rval;			/* reals */
    char *str;				/* names, etc */
    typeptr type;
    cnode cnode;			/* expression tree */
    anode anode;			/* class/type pair */
    stmt_index stmt;			/* statement pointer */
    cnode_list *clist;			/* expression list */
    arg_list *alist;			/* argument list */
    fieldptr field;			/* field list */
    enumptr elist;			/* enum list */
}

/* keywords used for the commands of the debugger */
%token <val> DUMP
%token <val> PRINT
%token <val> SOURCE
%token <val> SYM_DUMP
%token <val> STMT_DUMP
%token <val> TYPE_DUMP

/* keywords used for C -- all of this might be used at the current time */

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

%token <val> INCOP DECOP PTROP

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
%type <anode> type_name class_name class non_function_declarator
%type <anode> function_declarator ptr_function_declarator null_decl
%type <anode> non_empty_decl str_head enum_head

%type <cnode> constant name lvalue pvalue expression
%type <cnode> opt_expression nce

%type <val> opt_constant_expression constant_expression asgn_op
%type <val> compound_statement_preamble function_heading struct_union

%type <stmt> compound_statement statement_list statement if_preamble
%type <stmt> else_preamble data_def init_declaration_list
%type <stmt> init_declarator function_body declaration_stat_list

%type <clist> expression_list opt_expression_list init_list
%type <clist> initializer

%type <alist> name_list typed_name_list typed_name arg_declaration
%type <alist> arg_declaration_list arg_declarator_list

%type <field> type_declaration_list r_type_declaration_list
%type <field> type_declaration declarator_list declarator

%type <elist> member_list

%start user_input

%%
user_input
    : user_input command
    | /* NULL */
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
    | STMT_DUMP ';'
	{
	    dump_stmts();
	}
    | TYPE_DUMP ';'
	{
	    dump_types();
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
    | attributes ';'
	{
	    $$ = NO_STMT;
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
	    f = gram_enter_sym(&$1, __LINE__, (cnode_list *)0);
	    $$ = increase_nesting();
	    if (f) {
		f->s_stmt = mk_alloc_stmt();
		f->s_size = sizeof(stmt_index);
		if (f->s_expr)
		    f->s_expr->e_addr = f->s_offset;
		start_function(f);
	    }
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
	    current_attributes.a_type = find_type(ns_inter, TP_INT);
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
    : class_name
	{
	    $$.a_class = $1.a_class;
	    $$.a_valid_class = 1;
	    $$.a_valid_type = 0;
	    $$.a_valid_base = 0;
	    $$.a_valid_name = 0;
	}
    ;

class_name
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
    | TYPEDEF
	{
	    $$.a_class = typedef_class;
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
	    $$.a_type = find_type(ns_inter, TP_CHAR);
	    $$.a_base = uchar_type; /* should be an option */
	}
    | SHORT
	{
	    $$.a_type = find_type(ns_inter, TP_SHORT);
	    $$.a_base = short_type;
	}
    | INT
	{
	    $$.a_type = find_type(ns_inter, TP_INT);
	    $$.a_base = int_type;
	}
    | LONG
	{
#ifdef __64BIT__
	    $$.a_type = find_type(ns_inter, TP_LONG_64);
#else
	    $$.a_type = find_type(ns_inter, TP_LONG);
#endif
	    $$.a_base = long_type;
	}
    | UNSIGNED
	{
	    $$.a_type = find_type(ns_inter, TP_UNSIGNED);
	    $$.a_base = uint_type;
	}
    | FLOAT
	{
	    $$.a_type = find_type(ns_inter, TP_FLOAT);
	    $$.a_base = float_type;
	}
    | DOUBLE
	{
	    $$.a_type = find_type(ns_inter, TP_DOUBLE);
	    $$.a_base = double_type;
	}
    | VOID
	{
	    $$.a_type = find_type(ns_inter, TP_VOID);
	    $$.a_base = void_type;
	}
    | UNSIGNED CHAR
	{
	    $$.a_type = find_type(ns_inter, TP_UCHAR);
	    $$.a_base = uchar_type;
	}
    | SIGNED CHAR
	{
	    $$.a_type = find_type(ns_inter, TP_SCHAR);
	    $$.a_base = schar_type;
	}
    | UNSIGNED SHORT
	{
	    $$.a_type = find_type(ns_inter, TP_USHORT);
	    $$.a_base = ushort_type;
	}
    | UNSIGNED INT
	{
	    $$.a_type = find_type(ns_inter, TP_UINT);
	    $$.a_base = uint_type;
	}
    | UNSIGNED LONG
	{
#ifdef __64BIT__
	    $$.a_type = find_type(ns_inter, TP_ULONG_64);
#else
	    $$.a_type = find_type(ns_inter, TP_ULONG);
#endif
	    $$.a_base = ulong_type;
	}
    ;

enum_declaration
    : enum_head '{' member_list opt_comma '}'
	{
	    typeptr t = $1.a_type;

	    $$ = $1;
	    t->t_val.val_e = $3;
	    if ($1.a_name)
		add_namedef(t, $1.a_name);
	}
    | ENUM IDENTIFIER
	{
	    if (!($$.a_type = name2namedef_all($2)) ||
		$$.a_type->t_type != ENUM_TYPE) {
		yyerror("enum %s not found", $2);
		YYERROR;
	    }
	    $$.a_base = int_type;
	    $$.a_valid_type = 1;
	    $$.a_valid_base = 1;
	}
    | ENUM error
	{
	    $$.a_type = 0;
	}
    ;

enum_head
    : ENUM
	{
	    $$.a_type = newtype(ns_inter, ENUM_TYPE);
	    $$.a_valid_type = 1;
	    $$.a_name = 0;
	    $$.a_valid_name = 1;
	    $$.a_base = int_type;
	    $$.a_valid_base = 1;
	    $$.a_valid_class = 0;
	}
    | ENUM IDENTIFIER
	{
	    $$.a_type = newtype(ns_inter, ENUM_TYPE);
	    $$.a_valid_type = 1;
	    $$.a_name = $2;
	    $$.a_valid_name = 1;
	    $$.a_base = int_type;
	    $$.a_valid_base = 1;
	    $$.a_valid_class = 0;
	}
    ;

member_list
    : IDENTIFIER
	{
	    $$ = newenum($1, 0);
	}
    | IDENTIFIER '=' constant_expression
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
    ;

struct_declaration
    : str_head '{' type_declaration_list '}'
	{
	    typeptr t = $1.a_type;
	    int pos = 0;
	    fieldptr f = $3;

	    if (t->t_val.val_s.s_fields) {
		yyerror("struct or unions %s can not be redeclared",
			$1.a_name);
		YYERROR;
	    }
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
	    pos &= ~((sizeof(int) * 8) - 1);
	    pos /= 8;
	    t->t_val.val_s.s_size = pos;
	    $$ = $1;
	}
    | struct_union IDENTIFIER
	{
	    if (!($$.a_type = name2namedef_all($2))) {
		$$.a_type = newtype(ns_inter, (($1 == STRUCT) ?
					       STRUCT_TYPE : UNION_TYPE));
		add_namedef($$.a_type, $2);
	    }
	    $$.a_valid_type = 1;
	    $$.a_name = $2;
	    $$.a_valid_name = 1;
	    $$.a_base = struct_type;
	    $$.a_valid_base = 1;
	}
    | struct_union error
	{
	    $$.a_type = 0;
	}
    ;

str_head
    : struct_union
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
    | struct_union IDENTIFIER
	{
	    if (!($$.a_type = name2namedef(ns_inter, $2))) {
		$$.a_type = newtype(ns_inter, (($1 == STRUCT) ?
					       STRUCT_TYPE : UNION_TYPE));
		add_namedef($$.a_type, $2);
	    }
	    $$.a_valid_type = 1;
	    $$.a_name = $2;
	    $$.a_valid_name = 1;
	    $$.a_base = struct_type;
	    $$.a_valid_base = 1;
	    $$.a_valid_class = 0;
	}
    ;

struct_union
    : STRUCT
    | UNION
    ;

/*
 * The fields are flipped into their proper order here.
 */
type_declaration_list		/* list of struct fields w/ type */
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
    : type_declaration
	{
	    $$ = $1;
	}
    | r_type_declaration_list ';' type_declaration
	{
	    $3->f_next = $1;
	    $$ = $3;
	}
    ;

type_declaration		/* a struct field w/ type */
    : type declarator_list
	{
	    $$ = $2;
	    pop_attributes;
	}
    | type
	{
	    $$ = newfield((char *)0, $1.a_type, 0, get_size($1.a_type));
	    pop_attributes;
	}
    ;

declarator_list		/* fields after TYPE for a struct */
    : declarator
	{
	    $$ = $1;
	}
    | declarator_list ',' declarator
	{
	    $3->f_next = $1;
	    $$ = $3;
	}
    ;

declarator			/* a single field in a struct */
    : non_function_declarator
	{
	    $$ = newfield($1.a_name, $1.a_type, 0, get_size($1.a_type));
	}
    | non_function_declarator ':' constant_expression
	{
	    $$ = newfield($1.a_name, $1.a_type, 0, -$3);
	}
    | ':' constant_expression
	{
	    $$ = newfield((char *)0, (typeptr)0, 0, -$2);
	}
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
	    t1->t_val.val_r.r_upper = save_as_string($3 - 1);
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

	    t->t_val.val_p = $2.a_type->t_val.val_f.f_typeptr;
	    $$ = $2;
	    $$.a_type->t_val.val_f.f_typeptr = t;
	    $$.a_base = ulong_type;
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
	    t1->t_val.val_r.r_upper = save_as_string($3 - 1);
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
		(void) gram_enter_sym(&$1, __LINE__, (cnode_list *)0);
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
		(void) gram_enter_sym(&$1, __LINE__, (cnode_list *)0);
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
	    (void) gram_enter_sym(&$1, __LINE__, (cnode_list *)0);
	    if (mk_ident(&id, $1.a_name)) {
		yyerror("identifier %s not defined", $1.a_name);
		YYERROR;
	    }
	    if (mk_asgn(&asgn, &id, $2, &$3))
		YYERROR;
	    $$ = mk_expr_stmt(asgn.c_expr);
	}
    | non_function_declarator '=' '{' init_list opt_comma '}'
	{
	    if ($1.a_class == typedef_class) {
		yyerror("Cannot initialize a typedef");
		YYERROR;
	    }
	    (void) gram_enter_sym(&$1, __LINE__, $4);
	    $$ = NO_STMT;
	}
    ;

init_list
    : initializer
	{
	    $$ = $1;
	}
    | init_list ',' initializer
	{
	    $$ = $3;
	    $$->cl_next = $1;
	    $$->cl_down = 0;
	}
    ;

initializer
    : nce
	{
	    cnode_list *e = new(cnode_list);
	    e->cl_next = 0;
	    e->cl_down = 0;
	    e->cl_cnode = $1;
	    $$ = e;
	}
    | '{' init_list opt_comma '}'
	{
	    cnode_list *e = new(cnode_list);
	    e->cl_next = 0;
	    e->cl_down = $2;
	    $$ = e;
	}
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
	    if ($6 == NO_STMT)
		link_stmt(br, br);
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
	    if ($3 == NO_STMT)
		link_stmt($$, $$);
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
		cast_to(&$2, current_function->s_type,
			current_function->s_base);
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
	    PRINTF("did statement\n");
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
	{
	}
    | DEFAULT ':'
	{
	}
    ;

/* expression */

opt_expression
    : /* NULL */
	{
	    $$.c_type = 0;
	    $$.c_base = int_type;
	    $$.c_expr = 0;
	    $$.c_const = 1;
	    $$.c_bitfield = 0;
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
	    }
	    if (!$1.c_const) {
		yyerror("expression is not a constant");
		YYERROR;
	    }
	    $$ = i_val($1.c_expr);
	}
    ;

expression
    : nce
    | expression ',' nce
	{
	    if (mk_binary(&$$, &$1, $2, &$3))
		YYERROR;
	}
    ;

/* nce is for non-comma-expression */
nce
    : nce '*' nce
	{
	    if (mk_binary(&$$, &$1, $2, &$3))
		YYERROR;
	}
    | nce '/' nce
	{
	    if (mk_binary(&$$, &$1, $2, &$3))
		YYERROR;
	}
    | nce '%' nce
	{
	    if (mk_binary(&$$, &$1, $2, &$3))
		YYERROR;
	}
    | nce '+' nce
	{
	    if (mk_binary(&$$, &$1, $2, &$3))
		YYERROR;
	}
    | nce '-' nce
	{
	    if (mk_binary(&$$, &$1, $2, &$3))
		YYERROR;
	}
    | nce RSHIFT nce
	{
	    if (mk_binary(&$$, &$1, $2, &$3))
		YYERROR;
	}
    | nce LSHIFT nce
	{
	    if (mk_binary(&$$, &$1, $2, &$3))
		YYERROR;
	}
    | nce '<' nce
	{
	    if (mk_binary(&$$, &$1, $2, &$3))
		YYERROR;
	    change_to_int(&$$);
	}
    | nce '>' nce
	{
	    if (mk_binary(&$$, &$1, $2, &$3))
		YYERROR;
	    change_to_int(&$$);
	}
    | nce GTOREQUAL nce
	{
	    if (mk_binary(&$$, &$1, $2, &$3))
		YYERROR;
	    change_to_int(&$$);
	}
    | nce LTOREQUAL nce
	{
	    if (mk_binary(&$$, &$1, $2, &$3))
		YYERROR;
	    change_to_int(&$$);
	}
    | nce EQUALITY nce
	{
	    if (mk_binary(&$$, &$1, $2, &$3))
		YYERROR;
	    change_to_int(&$$);
	}
    | nce NOTEQUAL nce
	{
	    if (mk_binary(&$$, &$1, $2, &$3))
		YYERROR;
	    change_to_int(&$$);
	}
    | nce '&' nce
	{
	    if (mk_binary(&$$, &$1, $2, &$3))
		YYERROR;
	}
    | nce '^' nce
	{
	    if (mk_binary(&$$, &$1, $2, &$3))
		YYERROR;
	}
    | nce '|' nce
	{
	    if (mk_binary(&$$, &$1, $2, &$3))
		YYERROR;
	}
    | nce ANDAND nce
	{
	    if (mk_binary(&$$, &$1, $2, &$3))
		YYERROR;
	}
    | nce OROR nce
	{
	    if (mk_binary(&$$, &$1, $2, &$3))
		YYERROR;
	}
    | nce '?' nce ':' nce
	{
	    if (mk_qc_op(&$$, &$1, &$3, &$5))
		YYERROR;
	}
    | lvalue asgn_op nce		%prec '='
	{
	    if (mk_asgn(&$$, &$1, $2, &$3))
		YYERROR;
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
    | XOREQUAL
    ;

pvalue
    : lvalue				%prec DOSHIFT
	{
	    PRINTF("hit this %d\n", $1.c_expr->e_size);
	    mk_l2p(&$$, &$1);
	}
    | '(' expression ')'
	{
	    $$ = $2;
	}
    | '(' type_name ')' pvalue
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
    | '&' lvalue
	{
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
    ;

lvalue
    /*
     * The %prec clause below was changed from DOSHIFT to ELSE to fix
     * the problem that sizeof(*foo) was not parsing.  It is not clear
     * if this is a true fix. 950203 - pedz
     */
    : name				%prec ELSE
	{
	    $$ = $1;
	}
    | '*' pvalue
	{
	    if ($2.c_type->t_type == PTR_TYPE) {
		$$.c_type = $2.c_type->t_val.val_p;
	    } else if ($2.c_type->t_type == ARRAY_TYPE) {
		$$.c_type = $2.c_type->t_val.val_a.a_typeptr;
	    } else {
		yyerror("pointer type expected");
		YYERROR;
	    }
	    $$.c_base = base_type($$.c_type);
	    $$.c_expr = new_expr();
	    *$$.c_expr = *$2.c_expr;
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
	    $$.c_expr->e_size = get_size($$.c_type) / 8;
#else
	    $$.c_expr->e_size = sizeof(void *);
#endif
	    PRINTF("* %08x %d\n", $$.c_expr, $$.c_expr->e_size);
	    $$.c_const = 0;
	    $$.c_bitfield = 0;
	}
    ;

name
    : IDENTIFIER
	{
	    if (mk_ident(&$$, $1)) {
		yyerror("identifier %s not defined", $1);
		YYERROR;
	    }
	}
    | '(' lvalue ')'
	{
	    $$ = $2;
	}
    | name '.' IDENTIFIER
	{
	    if (mk_dot(&$$, &$1, $3))
		YYERROR;
		
	}
    | name '(' opt_expression_list ')' PTROP IDENTIFIER
	{
	    cnode ctemp;

	    if ($1.c_type->t_type != PROC_TYPE) {
		yyerror("function name expected");
		YYERROR;
	    }
	    mk_fcall(&ctemp, &$1, $3);

	    if (mk_ptr(&$$, &ctemp, $6))
		YYERROR;
	}
    | name PTROP IDENTIFIER
	{
	    if (mk_ptr(&$$, &$1, $3))
		YYERROR;
	}
    | '(' expression ')' PTROP IDENTIFIER
	{
	    if (mk_ptr(&$$, &$2, $5))
		YYERROR;
	}
    | name '[' expression ']'
	{
	    if (mk_array(&$$, &$1, &$3))
		YYERROR;
	}
    | '(' expression ')' '[' expression ']'
	{
	    if (mk_array(&$$, &$2, &$5))
		YYERROR;
	}
    | name '.' error
	{
	    yyerror("field name expected");
	}
    | name PTROP error
	{
	    yyerror("field name expected");
	}
    | name '[' expression error
	{
	    yyerror("']' expected");
	}
    | '(' lvalue error
	{
	    yyerror("')' expected");
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
	    e->cl_down = 0;
	    e->cl_cnode = $1;
	    $$ = e;
	}
    | expression_list ',' nce
	{
	    cnode_list *e = new(cnode_list);

	    e->cl_next = $1;
	    e->cl_down = 0;
	    e->cl_cnode = $3;
	    $$ = e;
	}
    ;

constant
    : RCON
	{
	    $$.c_type = find_type(ns_inter, TP_DOUBLE);
	    $$.c_base = double_type;
	    $$.c_expr = new_expr();
	    $$.c_expr->e_func.d = d_leaf;
	    $$.c_expr->e_d = $1;
	    $$.c_expr->e_size = sizeof(double);
	    $$.c_const = 1;
	    $$.c_bitfield = 0;
	}
    | ICON
	{
	    mk_const(ns_inter, &$$, $1);
	}
    | STRING
	{
	    $$.c_type = mk_strtype(ns_inter, strlen($1));
	    $$.c_base = base_type($$.c_type);
	    $$.c_expr = new_expr();
	    $$.c_expr->e_func.ul = ul_leaf;
	    $$.c_expr->e_ul = (unsigned long)$1;
	    $$.c_expr->e_size = get_size($$.c_type) / 8;
	    $$.c_const = 1;
	    $$.c_bitfield = 0;
	}
    | SIZEOF nce
	{
	    mk_const(ns_inter, &$$, get_size($2.c_type) / 8);
	}
    | SIZEOF '(' type_name ')'
	{
	    mk_const(ns_inter, &$$, get_size($3.a_type) / 8);
	}
    | SIZEOF '(' type_name error
	{
	    yyerror("')' expected");
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
	    t1->t_val.val_r.r_upper = save_as_string($3 - 1);
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
#include "scan.h"

void yyerror(char *s, ...)
{
    va_list Argp;
    char smallbuf[256];
    char *f = yyfilename ? yyfilename : "<stdin>";

    va_start(Argp, s);
    vsprintf(smallbuf, s, Argp);
    va_end(Argp);
    fprintf(stderr, "%s:%d %s\n", f, yylineno, smallbuf);
}

static symptr gram_enter_sym(anode *attr, int line, cnode_list *init)
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
    ret = enter_sym(ns_inter, attr->a_name, 0);

    /*
     * There are many cases where a symbol is not a duplicate but this solves
     * most of them.
     */
    if (ret->s_defined)
	if (ret->s_nesting == nesting_level) {
	    if (0) {
		yyerror("Duplicate symbol");
		return 0;
	    } else {
		return ret;
	    }
	} else {
	    ret = enter_sym(ns_inter, attr->a_name, 1);
	}

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
#if 0
	    param_index += size;
#else
	    param_index += (((size + sizeof(long)-1)/sizeof(long))*sizeof(long));
#endif
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
    param_index = sizeof(frame_ptr);
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
	(void) gram_enter_sym(&a1->a_anode, __LINE__, (cnode_list *)0);
    }
}

#if 0
static void prompt(void)
{
    extern FILE *yyin;

    if (isatty(fileno(yyin))) {
	putchar('>');
	putchar(' ');
	fflush(stdout);
    }
}
#endif

static void change_to_int(cnode *c)
{
    c->c_type = find_type(ns_inter, TP_INT);
    c->c_base = int_type;
    c->c_expr->e_size = sizeof(int);
}

static char *save_as_string(int i)
{
    char buf[32];

    sprintf(buf, "%d", i);
    return store_string(ns_inter, buf, 0, (char *)0);
}
