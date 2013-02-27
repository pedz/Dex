h39149
s 00124/00013/01086
d D 1.11 13/02/27 12:03:57 pedzan 11 10
e
s 00000/00000/01099
d D 1.10 10/08/23 17:01:54 pedzan 10 9
e
s 00009/00004/01090
d D 1.9 02/03/14 16:12:33 pedz 9 8
e
s 00000/00000/01094
d D 1.8 00/09/18 17:57:02 pedz 8 7
c Checking before V5 conversion
e
s 00049/00039/01045
d D 1.7 98/10/23 12:26:11 pedz 7 6
e
s 00010/00000/01074
d D 1.6 97/05/13 16:02:59 pedz 6 5
c Check pointer
e
s 00000/00000/01074
d D 1.5 95/05/17 16:39:13 pedz 5 4
c Check point
e
s 00001/00001/01073
d D 1.4 95/04/25 10:10:19 pedz 4 3
e
s 00034/00005/01040
d D 1.3 95/02/09 21:35:55 pedz 3 2
e
s 00184/00156/00861
d D 1.2 95/02/01 10:37:04 pedz 2 1
e
s 01017/00000/00000
d D 1.1 94/08/22 17:56:35 pedz 1 0
c date and time created 94/08/22 17:56:35 by pedz
e
u
U
t
T
I 1
%{

I 11
/*
 * The stab grammar is defined here:
 * http://publib.boulder.ibm.com/infocenter/pseries/v5r3/topic/com.ibm.aix.files/doc/aixfiles/XCOFF.htm#jtsci131jbau
 */

E 11
static char sccs_id[] = "%W%";

#include <strings.h>
#include <stdlib.h>
I 9
#include <stdio.h>
E 9
#include "map.h"
#include "sym.h"
I 9
#include "tree.h"
E 9
D 2
#include "tree.h"
E 2
#include "dex.h"
I 9
#include "stab_pre.h"

E 9
static char buf[128];			/* buf for names and stuff */
static char *nextp = buf;		/* pointer into aclbuf */

D 7
static void bogus();
E 7
I 7
static void bogus(int n);
E 7
D 2
static char *current_name;
static int current_typedef;
static int current_typeid;
E 2

/*
 * The following defines attempt to redefine everything that yacc
 * craetes as global so that we can have two yacc's in the same
 * program.
 */
#define yyact STABact
#define yychar STABchar
#define yychk STABchk
#define yydebug STABdebug
#define yydef STABdef
#define yyerrflag STABerrflag
#define yyerror STABerror
#define yyexca STABexca
#define yylex STABlex
#define yylval STABlval
#define yynerrs STABnerrs
#define yypact STABpact
#define yyparse STABparse
#define yypgo STABpgo
#define yyps STABps
#define yypv STABpv
#define yypvt STABpvt
#define yyr1 STABr1
#define yyr2 STABr2
#define yys STABs
#define yystate STABstate
#define yytmp STABtmp
#define yyv STABv
#define yyval STABval
#define yytoks STABtoks
#define yyreds STABreds

I 6
static ns *cur_ns;
static symptr cur_sym;
static char *parse_line;
static char *parse_position;
static char small_buf[32];
D 9
static yylex();
E 9
I 9
static int yylex(void);
E 9
static yyerror(char *s);
D 7
static void bogus();
E 7
I 7
static void bogus(int n);
E 7
D 9
int parse_stab(ns *ns, char *s, int len, symptr *s_out);
E 9

I 9
int yyparse(void);

E 9
E 6
%}

%union {
    attrptr attr;
    char *name;
    double rval;
    enumptr eptr;
    fieldptr field;
    int c;
    int val;
    paramptr param;
    typeptr type;
}

%type <attr> typeattrs typeattrlist typeattr
%type <eptr> enumspec enumlist enum
%type <field> ofieldlist fieldlist rfieldlist field
D 7
%type <name> NAME STRING
E 7
I 7
%type <name> NAME STRING bound DIGITSTRING
E 7
%type <param> tparamlist tparam
%type <rval> REAL
%type <type> typedef array subrange proceduretype record
I 2
%type <type> symbol_declaration
E 2
D 7
%type <val> INTEGER HEXINTEGER typeid bound numparams passby typenum
E 7
I 7
%type <val> INTEGER HEXINTEGER typeid numparams passby typenum
E 7
%type <val> ordvalue numelements numbits numbytes bitpattern bitoffset
D 2
%type <val> namedtype variable parameter
E 2
I 2
%type <val> variable parameter proc procedure
E 2
D 11
    
E 11
I 11

E 11
%%
D 2
file_def
    : stab_def '\n'
    | file_def stab_def '\n'
    ;

E 2
stab_def
    : /* empty */
D 2
    | NAME ':' { current_name = $1; } class
    | ':' { current_name = 0; } class
E 2
    | NAME
I 3
	{
	}
E 3
I 2
    | NAME ':' class
I 3
	{
	}
E 3
    | ':' class
    | NAME ':' 't' typeid
	{
	    add_typedef(find_type(cur_ns, $4), $1);
	}
    | ':' 't' typeid
    | NAME ':' 'T' typeid
	{
	    add_namedef(find_type(cur_ns, $4), $1);
	}
    | ':' 'T' typeid
    | NAME ':' symbol_declaration
	{
D 4
	    cur_sym = enter_sym(cur_ns, $1);
E 4
I 4
	    cur_sym = enter_sym(cur_ns, $1, 0);
E 4
	    /*
	     * We don't really care if the symbol is already defined
	     * or not.  We do not set s_defined or s_offset and let
	     * the guy that called us figure out what to do.
	     */
	    cur_sym->s_type = $3;
	    cur_sym->s_base = base_type(cur_sym->s_type);
	    cur_sym->s_size = get_size(cur_sym->s_type) / 8;
	    cur_sym->s_nesting = 1;
	    cur_sym->s_typed = 1;
	}
E 2
    ;

class
    : 'c' '=' constant ';'
D 2
    | namedtype
    | parameter
E 2
I 2
    | label
    ;

symbol_declaration
    : parameter
	{
	    $$ = find_type(cur_ns, $1);
	}
E 2
    | procedure
I 2
	{
	    $$ = newtype(cur_ns, PROC_TYPE);
	    $$->t_val.val_f.f_typeptr = find_type(cur_ns, $1);
	    $$->t_val.val_f.f_params = -1;
	    $$->t_val.val_f.f_paramlist = 0;
	}
E 2
    | variable
D 2
    | label
E 2
I 2
	{
	    $$ = find_type(cur_ns, $1);
	}
E 2
    ;

constant
    : 'b' ordvalue			/* boolean constant */
    | 'c' ordvalue			/* character constant */
    | 'e' typeid ',' ordvalue		/* enumeration constant */
    | 'i' INTEGER			/* integer constant */
    | 'r' REAL				/* real constant */
    | 's' STRING			/* string constant */
    | 'S' typeid ',' numelements ',' numbits ',' bitpattern /* set constant */
    ;

ordvalue : INTEGER ;
numelements : INTEGER ;
numbits : INTEGER ;
numbytes : INTEGER ;
bitpattern : HEXINTEGER ;

D 2
namedtype
    : 't'				/* used-defined type (typedef) */
	{
	    current_typedef = 1;
	}
      typeid
	{
	    $$ = $3;
	}
    | 'T'				/* struct union, etc */
	{
	    current_typedef = 0;
	}
      typeid
	{
	    $$ = $3;
	}
    ;

E 2
parameter
    : 'a' typeid			/* pass by ref in general register */
	{
	    $$ = $2;
	}
    | 'p' typeid			/* pass by value on stack */
	{
	    $$ = $2;
	}
    | 'v' typeid			/* pass by reference on stack */
	{
	    $$ = $2;
	}
    | 'C' typeid			/* constant passed by value */
	{
	    $$ = $2;
	}
    | 'D' typeid			/* passed by value in float regs */
	{
	    $$ = $2;
	}
    | 'R' typeid			/* pass by value in gen register */
	{
	    $$ = $2;
	}
    ;

procedure
    : proc				/* procedure at current scoping */
    | proc ',' NAME ',' NAME		/* procedure named 1st local to 2nd */
I 2
	{
	    yyerror("Can't do this");
	}
E 2
    ;

variable
D 2
    : typeid				/* local */
E 2
I 2
    : local_variable typeid		/* local */
E 2
	{
D 2
	    $$ = $1;
E 2
I 2
	    $$ = $2;
E 2
	}
D 2
    | 'd' typeid			/* float */
E 2
I 2
    | global_variable typeid		/* float */
E 2
	{
	    $$ = $2;
	}
D 2
    | 'r' typeid			/* register */
E 2
I 2
    ;

local_variable
    : /* NULL */
    | 'd'
    | 'r'
    ;

global_variable
    : 'G'
    | 'S'
    | 'V'
    ;

label : 'L' ;				/* label name */

proc
    : 'f' typeid			/* private func of type typeid */
E 2
	{
	    $$ = $2;
	}
D 2
    | 'G' typeid			/* global */
E 2
I 2
    | 'm' typeid			/* modula-2 ext. pascal */
E 2
	{
	    $$ = $2;
	}
D 2
    | 'S' typeid			/* c static global */
E 2
I 2
    | 'J' typeid			/* internal function */
E 2
	{
	    $$ = $2;
	}
D 2
    | 'V' typeid			/* c static local */
E 2
I 2
    | 'F' typeid			/* externalfunction */
E 2
	{
	    $$ = $2;
	}
D 2
    ;

label : 'L' ;				/* label name */

proc
    : 'f' typeid			/* private func of type typeid */
    | 'm' typeid			/* modula-2 ext. pascal */
    | 'J' typeid			/* internal function */
    | 'F' typeid			/* externalfunction */
E 2
    | 'l'				/* internal procedure */
I 2
	{
	    $$ = -1;
	}
E 2
    | 'P'				/* external procedure */
I 2
	{
	    $$ = -1;
	}
E 2
    | 'Q'				/* private procedure */
I 2
	{
	    $$ = -1;
	}
E 2
    ;

typeid
    : INTEGER
D 11
    | typenum typedef 
E 11
I 11
    | typenum typedef
E 11
	{
D 2
	    if ($2)
E 2
I 2
	    if ($2) {
E 2
		$2->t_attrs = 0;
I 2
		(void) insert_type($1, $2);
	    }
	    $$ = $1;
E 2
	}
    | typenum typeattrs typedef
	{
D 2
	    if ($3)
E 2
I 2
	    if ($3) {
E 2
		$3->t_attrs = $2;
I 2
		(void) insert_type($1, $3);
	    }
	    $$ = $1;
E 2
	}
    ;

typenum
    : INTEGER '='
	{
D 2
	    current_typeid = $1;
E 2
I 2
	    $$ = $1;
E 2
	}

typeattrs
    : '@' typeattrlist ';'
	{
	    $$ = $2;
	}
    ;

typeattrlist
    : typeattr
    | typeattrlist ';' '@' typeattr
	{
	    $4->a_next = $1;
	    $$ = $4;
	}
    ;

typeattr
    : 'a' INTEGER			/* align boundary */
	{
	    $$ = newattr(ALIGN_ATTR, $2);
	}
    | 's' INTEGER			/* size in bits */
	{
	    $$ = newattr(SIZE_ATTR, $2);
	}
    | 'p' INTEGER			/* pointer class */
	{
	    $$ = newattr(PTR_ATTR, $2);
	}
    | 'P'				/* packed type */
	{
	    $$ = newattr(PACKED_ATTR, 0);
	}
    ;					/* others should be skipped */

typedef
    : INTEGER				/* previously defined */
	{
	    typeptr t = find_type(cur_ns, $1);

D 2
	    $$ = newtype(cur_ns, t->t_type, current_name, current_typedef);
	    insert_type(current_typeid, $$);
E 2
I 2
	    $$ = newtype(cur_ns, t->t_type);
E 2
D 7
	    copy_type($$, t);
E 7
I 7
	    copy_type_and_record($$, t);
E 7
	}
    | 'b' typeid ';' numbytes		/* pascal space type */
	{
D 2
	    $$ = newtype(cur_ns, RANGE_TYPE, current_name, current_typedef);
	    insert_type(current_typeid, $$);
E 2
I 2
	    $$ = newtype(cur_ns, RANGE_TYPE);
E 2
D 7
	    bogus();
E 7
I 7
	    bogus(__LINE__);
E 7
	}
    | 'c' typeid ';' numbits		/* complex type typeid */
	{
D 2
	    $$ = newtype(cur_ns, COMPLEX_TYPE, current_name, current_typedef);
	    insert_type(current_typeid, $$);
E 2
I 2
	    $$ = newtype(cur_ns, COMPLEX_TYPE);
E 2
	    $$->t_val.val_g.g_typeptr = find_type(cur_ns, $2);
	    $$->t_val.val_g.g_size = $4;
	}
    | 'd' typeid			/* file of type typeid */
	{
D 2
	    $$ = newtype(cur_ns, RANGE_TYPE, current_name, current_typedef);
	    insert_type(current_typeid, $$);
E 2
I 2
	    $$ = newtype(cur_ns, RANGE_TYPE);
E 2
D 7
	    bogus();
E 7
I 7
	    bogus(__LINE__);
E 7
	}
D 2
    | 'e' enumspec ';'			/* enumerated type */
E 2
I 2
    | 'e' enumspec opt_semi			/* enumerated type */
E 2
	{
D 2
	    $$ = newtype(cur_ns, ENUM_TYPE, current_name, current_typedef);
	    insert_type(current_typeid, $$);
E 2
I 2
	    $$ = newtype(cur_ns, ENUM_TYPE);
E 2
	    $$->t_val.val_e = $2;
	}
    | 'g' typeid ';' numbits		/* floating-point type of numbits */
	{
D 2
	    $$ = newtype(cur_ns, FLOAT_TYPE, current_name, current_typedef);
	    insert_type(current_typeid, $$);
E 2
I 2
	    $$ = newtype(cur_ns, FLOAT_TYPE);
E 2
	    $$->t_val.val_g.g_typeptr = find_type(cur_ns, $2);
	    $$->t_val.val_g.g_size = $4;
	}
    | 'i' NAME ':' NAME ';'		/* import type modulename:name */
	{
D 2
	    $$ = newtype(cur_ns, RANGE_TYPE, current_name, current_typedef);
	    insert_type(current_typeid, $$);
E 2
I 2
	    $$ = newtype(cur_ns, RANGE_TYPE);
E 2
D 7
	    bogus();
E 7
I 7
	    bogus(__LINE__);
E 7
	}
    | 'i' NAME ':' NAME ',' typeid ';'	/* import modulename:name typeid */
	{
D 2
	    $$ = newtype(cur_ns, RANGE_TYPE, current_name, current_typedef);
	    insert_type(current_typeid, $$);
E 2
I 2
	    $$ = newtype(cur_ns, RANGE_TYPE);
E 2
D 7
	    bogus();
E 7
I 7
	    bogus(__LINE__);
E 7
	}
I 2
    | 'k' typeid
	{
	    $$ = newtype(cur_ns, CONSTANT_TYPE);
	    $$->t_val.val_k = find_type(cur_ns, $2);
	}
E 2
    | 'n' typeid ';' numbytes		/* string type with max length */
	{
D 2
	    $$ = newtype(cur_ns, RANGE_TYPE, current_name, current_typedef);
	    insert_type(current_typeid, $$);
E 2
I 2
	    $$ = newtype(cur_ns, RANGE_TYPE);
E 2
D 7
	    bogus();
E 7
I 7
	    bogus(__LINE__);
E 7
	}
    | 'o' NAME ';'			/* opaque type */
	{
D 2
	    $$ = newtype(cur_ns, RANGE_TYPE, current_name, current_typedef);
	    insert_type(current_typeid, $$);
E 2
I 2
	    $$ = newtype(cur_ns, RANGE_TYPE);
E 2
D 7
	    bogus();
E 7
I 7
	    bogus(__LINE__);
E 7
	}
    | 'o' NAME ',' typeid		/* opaque define typeid */
	{
D 2
	    $$ = newtype(cur_ns, RANGE_TYPE, current_name, current_typedef);
	    insert_type(current_typeid, $$);
E 2
I 2
	    $$ = newtype(cur_ns, RANGE_TYPE);
E 2
D 7
	    bogus();
E 7
I 7
	    bogus(__LINE__);
E 7
	}
    | 'w'				/* wide character */
	{
D 2
	    $$ = newtype(cur_ns, RANGE_TYPE, current_name, current_typedef);
	    insert_type(current_typeid, $$);
E 2
I 2
	    $$ = newtype(cur_ns, RANGE_TYPE);
E 2
D 7
	    bogus();
E 7
I 7
	    bogus(__LINE__);
E 7
	}
    | 'z' typeid ';' numbytes		/* pascal gstring type */
	{
D 2
	    $$ = newtype(cur_ns, RANGE_TYPE, current_name, current_typedef);
	    insert_type(current_typeid, $$);
E 2
I 2
	    $$ = newtype(cur_ns, RANGE_TYPE);
E 2
D 7
	    bogus();
E 7
I 7
	    bogus(__LINE__);
E 7
	}
    | 'C' usage				/* cobol picture */
	{
D 2
	    $$ = newtype(cur_ns, RANGE_TYPE, current_name, current_typedef);
	    insert_type(current_typeid, $$);
E 2
I 2
	    $$ = newtype(cur_ns, RANGE_TYPE);
E 2
D 7
	    bogus();
E 7
I 7
	    bogus(__LINE__);
E 7
	}
I 11
    | 'D' INTEGER ';' typeid		/* Decimal floating point */
	{
	    $$ = newtype(cur_ns, DECIMAL_FLOAT_TYPE);
	    $$->t_val.val_g.g_typeptr = find_type(cur_ns, $2);
	    $$->t_val.val_g.g_size = $4;
	}
E 11
    | 'K' cobolfiledesc			/* cobol file descriptor */
	{
D 2
	    $$ = newtype(cur_ns, RANGE_TYPE, current_name, current_typedef);
	    insert_type(current_typeid, $$);
E 2
I 2
	    $$ = newtype(cur_ns, RANGE_TYPE);
E 2
D 7
	    bogus();
E 7
I 7
	    bogus(__LINE__);
E 7
	}
    | 'M' typeid ';' bound		/* multiple instance */
	{
D 2
	    $$ = newtype(cur_ns, RANGE_TYPE, current_name, current_typedef);
	    insert_type(current_typeid, $$);
E 2
I 2
	    $$ = newtype(cur_ns, RANGE_TYPE);
E 2
D 7
	    bogus();
E 7
I 7
	    bogus(__LINE__);
E 7
	}
    | 'N' typeid			/* pascal stringptr */
	{
D 2
	    $$ = newtype(cur_ns,STRINGPTR_TYPE,current_name,current_typedef);
	    insert_type(current_typeid, $$);
E 2
I 2
	    $$ = newtype(cur_ns, STRINGPTR_TYPE);
E 2
	    $$->t_val.val_N = $2;
	}
    | 'S' typeid			/* set of type typeid */
	{
D 2
	    $$ = newtype(cur_ns, RANGE_TYPE, current_name, current_typedef);
	    insert_type(current_typeid, $$);
E 2
I 2
	    $$ = newtype(cur_ns, RANGE_TYPE);
E 2
D 7
	    bogus();
E 7
I 7
	    bogus(__LINE__);
E 7
	}
    | '*' typeid			/* pointer to type typeid */
	{
D 2
	    $$ = newtype(cur_ns, PTR_TYPE, current_name, current_typedef);
	    insert_type(current_typeid, $$);
E 2
I 2
	    $$ = newtype(cur_ns, PTR_TYPE);
E 2
	    $$->t_val.val_p = find_type(cur_ns, $2);
	}
I 2
    | 'V' typeid
	{
	    $$ = newtype(cur_ns, VOLATILE);
	    $$->t_val.val_v = find_type(cur_ns, $2);
	}
E 2
    | 'Z'
	{
D 2
	    $$ = newtype(cur_ns, ELLIPSES, current_name, current_typedef);
	    insert_type(current_typeid, $$);
E 2
I 2
	    $$ = newtype(cur_ns, ELLIPSES);
E 2
	}
    | array
    | subrange
    | proceduretype
    | record
    ;

I 2
opt_semi
    : /* NULL */
    | ';'
    ;

E 2
enumspec
    : typeid ':' enumlist
	{
	    $$ = $3;
	}
    | enumlist
	{
	    $$ = $1;
	}
    ;

enumlist
    : enum
    | enumlist enum
	{
	    $2->e_next = $1;
	    $$ = $2;
	}
    ;

enum
    : NAME ':' ordvalue ','
	{
	    $$ = newenum($1, $3);
	}
    ;

array
    : 'a' typedef ';' typeid		/* array */
	{
D 2
	    $$ = newtype(cur_ns, ARRAY_TYPE, current_name, current_typedef);
	    insert_type(current_typeid, $$);
E 2
I 2
	    $$ = newtype(cur_ns, ARRAY_TYPE);
E 2
	    $$->t_val.val_a.a_typedef = $2;
	    $$->t_val.val_a.a_typeptr = find_type(cur_ns, $4);
	}
    | 'A' typeid			/* open array */
	{
D 2
	    $$ = newtype(cur_ns, ARRAY_TYPE, current_name, current_typedef);
	    insert_type(current_typeid, $$);
E 2
I 2
	    $$ = newtype(cur_ns, ARRAY_TYPE);
E 2
	    $$->t_val.val_a.a_typedef = 0;
	    $$->t_val.val_a.a_typeptr = find_type(cur_ns, $2);
	}
D 11
    | 'D' INTEGER ';' typeid		/* n-dimen. dynamic array */
	{
D 2
	    $$ = newtype(cur_ns, RANGE_TYPE, current_name, current_typedef);
	    insert_type(current_typeid, $$);
E 2
I 2
	    $$ = newtype(cur_ns, RANGE_TYPE);
E 2
D 7
	    bogus();
E 7
I 7
	    bogus(__LINE__);
E 7
	}
E 11
    | 'E' INTEGER ';' typeid		/* n-dimen. subarray  */
	{
D 2
	    $$ = newtype(cur_ns, RANGE_TYPE, current_name, current_typedef);
	    insert_type(current_typeid, $$);
E 2
I 2
	    $$ = newtype(cur_ns, RANGE_TYPE);
E 2
D 7
	    bogus();
E 7
I 7
	    bogus(__LINE__);
E 7
	}
    | 'P' typedef ';' typeid		/* packed array */
	{
D 2
	    $$ = newtype(cur_ns, RANGE_TYPE, current_name, current_typedef);
	    insert_type(current_typeid, $$);
E 2
I 2
	    $$ = newtype(cur_ns, RANGE_TYPE);
E 2
D 7
	    bogus();
E 7
I 7
	    bogus(__LINE__);
E 7
	}
    ;

subrange
    : 'r' typeid ';' bound ';' bound /* subrange with bounds */
	{
D 2
	    $$ = newtype(cur_ns, RANGE_TYPE, current_name, current_typedef);
	    insert_type(current_typeid, $$);
E 2
I 2
	    $$ = newtype(cur_ns, RANGE_TYPE);
E 2
	    $$->t_val.val_r.r_typeptr = find_type(cur_ns, $2);
	    $$->t_val.val_r.r_lower = $4;
	    $$->t_val.val_r.r_upper = $6;
	}
    ;

bound
D 7
    : INTEGER				/* constant bound */
    | boundtype INTEGER			/* var or dynamic bound */
E 7
I 7
    : DIGITSTRING			/* constant bound */
    | boundtype DIGITSTRING		/* var or dynamic bound */
E 7
	{
D 7
	    bogus();
E 7
I 7
	    bogus(__LINE__);
E 7
	}
    | 'J'				/* bound is indeterminable */
	{
D 7
	    bogus();
E 7
I 7
	    $$ = 0;
E 7
	}
    ;

boundtype
    : 'A'				/* passed by ref on stack */
D 7
    | 'T'				/* passed by vlue on stack */
E 7
I 7
    | 'T'				/* passed by value on stack */
E 7
    | 'a'				/* passed by ref in register */
    | 't'				/* passed by value in register */
    ;

proceduretype
D 11
    : 'f' typeid ';'		/* func returning type typeid */
E 11
I 11
    : 'f' typeid opt_semi		/* func returning type typeid */
E 11
	{
D 2
	    $$ = newtype(cur_ns, PROC_TYPE, current_name, current_typedef);
	    insert_type(current_typeid, $$);
E 2
I 2
	    $$ = newtype(cur_ns, PROC_TYPE);
E 2
	    $$->t_val.val_f.f_typeptr = find_type(cur_ns, $2);
	    $$->t_val.val_f.f_params = -1;
	    $$->t_val.val_f.f_paramlist = 0;
	}
    | 'f' typeid ',' numparams ';' tparamlist ';' /* typeid func w n params*/
	{
D 2
	    $$ = newtype(cur_ns, PROC_TYPE, current_name, current_typedef);
	    insert_type(current_typeid, $$);
E 2
I 2
	    $$ = newtype(cur_ns, PROC_TYPE);
E 2
	    $$->t_val.val_f.f_typeptr = find_type(cur_ns, $2);
	    $$->t_val.val_f.f_params = $4;
	    $$->t_val.val_f.f_paramlist = $6;
	}
    | 'p' numparams ';' tparamlist ';'	/* proc with n params */
	{
D 2
	    $$ = newtype(cur_ns, RANGE_TYPE, current_name, current_typedef);
	    insert_type(current_typeid, $$);
E 2
I 2
	    $$ = newtype(cur_ns, RANGE_TYPE);
E 2
D 7
	    bogus();
E 7
I 7
	    bogus(__LINE__);
E 7
	}
    | 'R' numparams ';' namedtparamlist ';' /* pascal sub parameter */
	{
D 2
	    $$ = newtype(cur_ns, RANGE_TYPE, current_name, current_typedef);
	    insert_type(current_typeid, $$);
E 2
I 2
	    $$ = newtype(cur_ns, RANGE_TYPE);
E 2
D 7
	    bogus();
E 7
I 7
	    bogus(__LINE__);
E 7
	}
    | 'F' typeid ',' numparams ';' namedtparamlist ';' /* func parameter */
	{
D 2
	    $$ = newtype(cur_ns, RANGE_TYPE, current_name, current_typedef);
	    insert_type(current_typeid, $$);
E 2
I 2
	    $$ = newtype(cur_ns, RANGE_TYPE);
E 2
D 7
	    bogus();
E 7
I 7
	    bogus(__LINE__);
E 7
	}
    ;

numparams : INTEGER ;

tparamlist
    : tparam				/* type of param and pass method */
    | tparamlist tparam
	{
	    $2->p_next = $1;
	    $$ = $2;
	}
    ;

tparam
    : typeid ',' passby ';'
	{
	    $$ = newparam($1, $3);
	}
    ;

namedtparamlist
    : namedtparam
    | namedtparamlist namedtparam
    ;

namedtparam
D 3
    : NAME ':' typeid ',' passby ';' ;
E 3
I 3
    : NAME ':' typeid ',' passby ';'
	{
	}
    ;
E 3

record
    : 's' numbytes ofieldlist ';'	/* struct */
	{
D 2
	    $$ = newtype(cur_ns, STRUCT_TYPE, current_name, current_typedef);
	    insert_type(current_typeid, $$);
E 2
I 2
	    $$ = newtype(cur_ns, STRUCT_TYPE);
E 2
	    $$->t_val.val_s.s_size = $2;
	    $$->t_val.val_s.s_fields = $3;
	}
    | 'u' numbytes ofieldlist ';'	/* union */
	{
D 2
	    $$ = newtype(cur_ns, UNION_TYPE, current_name, current_typedef);
	    insert_type(current_typeid, $$);
E 2
I 2
	    $$ = newtype(cur_ns, UNION_TYPE);
E 2
	    $$->t_val.val_s.s_size = $2;
	    $$->t_val.val_s.s_fields = $3;
	}
    | 'v' numbytes ofieldlist variantpart ';' /* variant record */
	{
D 2
	    $$ = newtype(cur_ns, RANGE_TYPE, current_name, current_typedef);
	    insert_type(current_typeid, $$);
E 2
I 2
	    $$ = newtype(cur_ns, RANGE_TYPE);
E 2
D 7
	    bogus();
E 7
I 7
	    bogus(__LINE__);
E 7
	}
    | 'G' redefinition 'n' numbits fieldlist ';' /* cobol group w/o conds */
	{
D 2
	    $$ = newtype(cur_ns, RANGE_TYPE, current_name, current_typedef);
	    insert_type(current_typeid, $$);
E 2
I 2
	    $$ = newtype(cur_ns, RANGE_TYPE);
E 2
D 7
	    bogus();
E 7
I 7
	    bogus(__LINE__);
E 7
	}
    | 'G' 'n' numbits fieldlist ';'
	{
D 2
	    $$ = newtype(cur_ns, RANGE_TYPE, current_name, current_typedef);
	    insert_type(current_typeid, $$);
E 2
I 2
	    $$ = newtype(cur_ns, RANGE_TYPE);
E 2
D 7
	    bogus();
E 7
I 7
	    bogus(__LINE__);
E 7
	}
    | 'G' redefinition 'c' numbits condition fieldlist ';' /* with conds */
	{
D 2
	    $$ = newtype(cur_ns, RANGE_TYPE, current_name, current_typedef);
	    insert_type(current_typeid, $$);
E 2
I 2
	    $$ = newtype(cur_ns, RANGE_TYPE);
E 2
D 7
	    bogus();
E 7
I 7
	    bogus(__LINE__);
E 7
	}
    | 'G' 'c' numbits condition fieldlist ';'
	{
D 2
	    $$ = newtype(cur_ns, RANGE_TYPE, current_name, current_typedef);
	    insert_type(current_typeid, $$);
E 2
I 2
	    $$ = newtype(cur_ns, RANGE_TYPE);
E 2
D 7
	    bogus();
E 7
I 7
	    bogus(__LINE__);
E 7
	}
D 11
    | 'Y' numbytes classkey OptPBV OptBaseSpecList '(' ExtendedFieldList
	  OptNameResolutionList ';'
E 11
I 11
    | 'Y' numbytes classkey OptPBV OptBaseSpecList '(' ExtendedFieldList OptNameResolutionList ';'
E 11
	{
	    $$ = 0;
	}
    ;

classkey
D 11
    : 's'
    | 'u'
    | 'c'
E 11
I 11
    : 's'			/* struct */
    | 'u'			/* union */
    | 'c'			/* class */
E 11
    ;

/*
 * These guys are new for C++ and they are not fully implemented
 */
OptPBV
    : /* Empty */
I 11
    | 'V'			/* pass by value */
E 11
    ;

OptBaseSpecList
    : /* Empty */
    ;

ExtendedFieldList
    : /* Empty */
I 11
    | ExtendedField ExtendedFieldList
E 11
    ;
I 11

ExtendedField
    : GenSpec AccessSpec AnonSpec DataMember
    | GenSpec VirtualSpec AccessSpec OptVirtualFuncIndex MemberFunction
    | AccessSpec AnonSpec NestedClass
    | AnonSpec FriendClass
    | AnonSpec FriendFunction
    ;

OptVirtualFuncIndex
    : /* Empty */
    | INTEGER
    ;

MemberFunction
    : '[' FuncType MemberFuncAttrs ':' NAME ':' typeid ';'
    ;

MemberFuncAttrs
    : IsStatic IsInline IsConst IsVolatile
    ;

IsInline
    : /* Empty */
    | 'i'			/* inline function */
    ;

IsConst
    : /* Empty */
    | 'k'			/* const member function */
    ;        

IsVolatile
    : /* Empty */
    | 'V'			/* Volatile member function */
    ;

NestedClass
    : 'N' typeid ';' /* #  */
    ;

FriendClass
    : '(' typeid ';' /* # */
    ;

FriendFunction
    : ']' NAME ':' typeid ';' /* # */
    ;

FuncType
    : 'f'			/* Member function */
    | 'c'			/* Constructor */
    | 'd'			/* Destructor */
    ;

GenSpec
    : /* Empty */
    | 'c'			/* Compiler Generated */
    ;

AccessSpec
    : 'i'			/* Private  */
    | 'o'			/* Protected */
    | 'u' 			/* Public */
    ;

AnonSpec
    : /* Empty */
    | 'a'			/* Anonymous union member */
    ;

VirtualSpec
    : /* Empty */
    | 'v' 'p' 			/* Pure Virtual */
    | 'v'			/* Virtual */
    ;

DataMember
    : MemberAttrs ':' field
    ;

MemberAttrs
    : IsStatic IsVtblPtr IsVBasePtr
    ;

IsStatic
    : /* Empty */
    | 's'			/* Member is static */
    ;

/* Some of the shift / reduce errors come from here.  The optional
   IsVBasePtr comes after NAME so its not clear what to do with
   trailing 'b and 'r' */
IsVtblPtr
    : /* Empty */
    | 'p' INTEGER NAME 		/* Member is vtbl pointer; NAME is the external name of v-table. */
    ;

IsVBasePtr
    : /* Empty */
    | 'b'			/* Member is vbase pointer. */
    | 'r'			/* Member is vbase self-pointer. */
    ;

E 11

OptNameResolutionList
    : /* Empty */
    ;

ofieldlist
    : /* empty */
	{
	    $$ = 0;
	}
    | fieldlist
    ;

fieldlist
    : rfieldlist
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

/* Field list but it is constructed backwards */
rfieldlist
    : field
    | rfieldlist field
	{
	    $2->f_next = $1;
	    $$ = $2;
	}
    ;

field
D 3
    : NAME ':' typeid ',' bitoffset ',' numbits ';'
E 3
I 3
    : ':' typeid ',' bitoffset ',' numbits ';'
E 3
	{
D 3
	    $$ = newfield($1, $3, $5, $7);
E 3
I 3
	    $$ = newfield((char *)0, find_type(cur_ns, $2), $4, $6);
E 3
	}
I 3
    | NAME ':' typeid ',' bitoffset ',' numbits ';'
	{
	    $$ = newfield($1, find_type(cur_ns, $3), $5, $7);
	}
E 3
    ;

variantpart
    : '[' vtag vfieldlist ']'
    ;

vtag
    : OPAREN field			/* member of var record */
    | OPAREN NAME ':' ';'		/* variant key name */
    ;

vfieldlist
    : vlist				/* member of var record */
    | vfieldlist vlist
    ;

vlist
    : vfield				/* member of var record */
    | vfield variantpart
    ;

vfield
    : OPAREN vrangelist ':'		/* variant with no field list */
    | OPAREN vrangelist ':' fieldlist	/* var with field list */
    ;

vrangelist
    : vrange			/* member of var record */
    | vrangelist ',' vrange
    ;

vrange
    : 'b' ordvalue			/* boolean variant */
    | 'c' ordvalue			/* character variant */
    | 'e' typeid ',' ordvalue		/* enumeration variant */
    | 'i' INTEGER			/* integer variant */
D 7
    | 'r' typeid ';' bound ';' bound	/* subrante variant */
E 7
I 7
    | 'r' typeid ';' bound ';' bound	/* subrange variant */
E 7
    ;

bitoffset : INTEGER ;

usage
    : picstoragetype numbits ',' editdescription ',' picsize ';'
    | redefinition picstoragetype numbits ',' editdescription ',' picsize ';'
    | picstoragetype numbits ',' editdescription ',' picsize ',' condition ';'
    | redefinition picstoragetype numbits ',' editdescription ',' picsize ','
	condition ';'
    ;

redefinition
    : 'r' NAME ','
    ;

picstoragetype
    : 'a'				/* alphabetic */
    | 'b'				/* alphabetic edited */
    | 'c'				/* alphanumeric */
    | 'd'				/* alphanumeric edited */
    | 'e'				/* numeric signed trailing included */
    | 'f'				/* numeric signed trailing seperate */
    | 'g'				/* numeric signed leading included */
    | 'i'				/* numeric signed default cmpe */
    | 'j'				/* numeric unsigned default comp */
    | 'k'				/* numeric packed decimal signed */
    | 'l'				/* numeric packed decimal unsigned */
    | 'm'				/* numeric unsigned cmp-x */
    | 'n'				/* numeric unsigned comp-5 */
    | 'o'				/* numeric signed comp-5 */
    | 'p'				/* numeric edited */
    | 'q'				/* numeric unsigned */
    | 's'				/* indexed item */
    | 't'				/* pointer */
    ;

editdescription
    : STRING				/* edit characters in an alpha RC */
I 3
	{
	}
E 3
    | INTEGER				/* dec point in numeric pic */
I 3
	{
	}
E 3
    ;

picsize
    : INTEGER				/* number of repeated 9's */
I 3
	{
	}
E 3
    ;

condition
    : NAME ':' INTEGER '=' 'q' conditiontype ',' valuelist ';'
I 3
	{
	}
E 3
    ;

conditiontype
    : conditionprimitive ',' kanjichar
    ;

conditionprimitive
    : 'n' sign decimalsite		/* numeric conditional */
    | 'a'				/* alphanumeric conditional */
    | 'f'				/* figurative conditional */
    ;

sign
    : '+'				/* positive */
    | '-'				/* negative */
    | '0'				/* no explicit sign */
    ;

D 3
decimalsite : INTEGER ;
kanjichar : INTEGER ;
E 3
I 3
decimalsite
    : INTEGER
	{
	}
    ;
kanjichar
    : INTEGER
	{
	}
    ;
E 3
valuelist
    : value
    | valuelist value
    ;

value
    : INTEGER ':' STRING		/* int is length of string */
I 3
	{
	}
E 3
    ;

cobolfiledesc
    : organization accessmethod numbytes
    ;

organization
    : 'i'				/* indexed */
    | 'l'				/* line sequential */
    | 'r'				/* relative */
    | 's'				/* sequential */
    ;

accessmethod
    : 'd'				/* dynamic */
    | 'o'				/* sort */
    | 'r'				/* random */
    | 's'				/* sequential */
    ;

passby : INTEGER ;			/* 0=ref 1=value */
OPAREN : '(' ;
CPAREN : ')' ;

/*
 * Develop the primitives of NAME, INTEGER, HEXINTEGER, REAL, and
 * STRING
 */

NAME
    : { nextp = buf; } name
	{
	    *nextp = 0;
D 2
	    $$ = strcpy(smalloc(nextp - buf + 1, __FILE__, __LINE__), buf);
E 2
I 2
	    $$ = store_string(cur_ns, buf, 0, (char *)0);
E 2
	}
    ;

INTEGER
I 7
    : DIGITSTRING
	{
	    *nextp = 0;
	    $$ = atoi($1);
	}
    ;

DIGITSTRING
E 7
    : { nextp = buf; } '-' { *nextp++ = $<val>2; } digits
	{
	    *nextp = 0;
D 7
	    $$ = atoi(buf);
E 7
I 7
	    $$ = store_string(cur_ns, buf, 0, (char *)0);
E 7
	}
    | { nextp = buf; } digits
	{
	    *nextp = 0;
D 7
	    $$ = atoi(buf);
E 7
I 7
	    $$ = store_string(cur_ns, buf, 0, (char *)0);
E 7
	}
    ;

HEXINTEGER
    : { nextp = buf; } hexinteger
	{
	    *nextp = 0;
	    sscanf(buf, "%x", &$$);
	}
    ;

REAL
    : { nextp = buf; } real
	{
	    *nextp = 0;
	    sscanf(buf, "%f", &$$);
	}
    ;

STRING
    : { nextp = buf; } '\"' str '\"'
	{
	    *nextp = '\0';
D 2
	    $$ = strcpy(smalloc(nextp - buf + 1, __FILE__, __LINE__), buf);
E 2
I 2
	    $$ = store_string(cur_ns, buf, 0, (char *)0);
E 2
	}
    | { nextp = buf; } '\'' str '\''
	{
	    *nextp = '\0';
D 2
	    $$ = strcpy(smalloc(nextp - buf + 1, __FILE__, __LINE__), buf);
E 2
I 2
	    $$ = store_string(cur_ns, buf, 0, (char *)0);
E 2
	}
    ;

name
    : lead_letter
    | name letter
    | name digit
    ;

hexinteger
    : hexdigit
    | hexinteger hexdigit
    ;

real
    : opsign digits opexp
    | opsign digits '.' { *nextp++ = $<val>3; } opexp
    | opsign digits '.' { *nextp++ = $<val>3; } digits opexp
    | opsign 'I' 'N' 'F'
	{
	    *nextp++ = 'I';
	    *nextp++ = 'N';
	    *nextp++ = 'F';
	}
    | 'Q' 'N' 'A' 'N'
	{
	    *nextp++ = 'Q';
	    *nextp++ = 'N';
	    *nextp++ = 'A';
	    *nextp++ = 'N';
	}
    | 'S' 'N' 'A' 'N'
	{
	    *nextp++ = 'S';
	    *nextp++ = 'N';
	    *nextp++ = 'A';
	    *nextp++ = 'N';
	}
    ;

str : strchar
    | str strchar
    ;

opexp
    : /* empty */
    | 'e' { *nextp++ = $<val>1; } opsign digits
    | 'E' { *nextp++ = $<val>1; } opsign digits
    ;

opsign
    : /* empty */
    | '+' { *nextp++ = $<val>1; }
    | '-' { *nextp++ = $<val>1; }
    ;

digits
    : digit
    | digits digit
    ;

strchar
    : STRCHAR
	{
	    *nextp++ = $<val>1;
	}
    ;

STRCHAR
    : LETTER | DIGIT | '\\' '\'' | '\\' '\"' | ';' | ':' ;

digit
    : DIGIT
	{
	    *nextp++ = $<val>1;
	}
    ;

DIGIT : '0' | '1' | '2' | '3' | '4' | '5' | '6' | '7' | '8' | '9' ;

hexdigit
    : HEXDIGIT
	{
	    *nextp++ = $<val>1;
	}
    ;

HEXDIGIT
    : DIGIT
    | 'a' | 'b' | 'c' | 'd' | 'e' | 'f'
    | 'A' | 'B' | 'C' | 'D' | 'E' | 'F'
    ;

lead_letter
    : LEAD_LETTER
	{
	    *nextp++ = $<val>1;
	}
    ;

letter
    : LETTER
	{
	    *nextp++ = $<val>1;
	}
    ;

LETTER
    : '-'
    | LEAD_LETTER
    ;

LEAD_LETTER
    : ' ' | '!' | '#' | '$' | '%' | '&' | CPAREN | '*' | '+'
    | '.' | '/' | '<' | '=' | '>' | '?' | '@' | 'A' | 'B' | 'C' | 'D'
    | 'E' | 'F' | 'G' | 'H' | 'I' | 'J' | 'K' | 'L' | 'M' | 'N' | 'O'
    | 'P' | 'Q' | 'R' | 'S' | 'T' | 'U' | 'V' | 'W' | 'X' | 'Y' | 'Z'
    | '^' | '_' | '`' | 'a' | 'b' | 'c' | 'd' | 'e' | 'f' | 'g' | 'h'
    | 'i' | 'j' | 'k' | 'l' | 'm' | 'n' | 'o' | 'p' | 'q' | 'r' | 's'
    | 't' | 'u' | 'v' | 'w' | 'x' | 'y' | 'z' | '{' | '|' | '}' | '~'
    | '\177'
    ;
%%
D 9
#include <stdio.h>
E 9
I 9
#define DEBUG_BIT STAB_Y_BIT
E 9

D 2
static lineno = 1;
static charno = 0;
static FILE *stabfile;
E 2
I 2
/*
 * This grammer and parser is set up so that a null terminated string
 * is passed into parse_stab along with the name space to use.  Static
 * global variables are setup and then yyparse is called.  yyparse
 * will eventually call yylex which will return the characters one by
 * one until the end of the string is hit at which time EOF is
 * returned.  The grammer is suppose to reduce at the end of the
 * string.  If it does not, then we have problems but it will
 * eventually error out when it gets back EOF's.
 */
E 2
static ns *cur_ns;
I 2
static symptr cur_sym;
static char *parse_line;
static char *parse_position;
static char small_buf[32];
E 2

D 9
static yylex()				/* actually STABlex */
E 9
I 9
static int yylex(void)			/* actually STABlex */
E 9
{
D 2
    int c = getc(stabfile);
    extern int yydebug;
E 2
I 2
    int c;
E 2

D 2
    ++charno;
    if (c == '\n') {
	++lineno;
	charno = 0;
    }
E 2
I 2
    if (c = *parse_position)
	++parse_position;
    else
	c = EOF;
E 2
    yylval.c = c;
    return c;
}

static yyerror(char *s)			/* actually STABerror */
{
D 2
    fprintf(stderr, "%s parsing stab file at line %d and char %d\n", s,
	    lineno, charno);
E 2
I 2
    fprintf(stderr, "%s parsing stab string %s at character %d\n", s,
	    parse_line, parse_position - parse_line);
E 2
}

D 7
static void bogus()
E 7
I 7
static void bogus(int n)
E 7
{
D 7
    yyerror("unimplmented construct");
E 7
I 7
    char buf[64];
    sprintf(buf, "unimplmented construct from line %d of stab.y", n);
    yyerror(buf);
E 7
    exit(1);
}

D 2
int parse_stab(char *s)
E 2
I 2
int parse_stab(ns *ns, char *s, int len, symptr *s_out)
E 2
{
D 2
    if (!(stabfile = fopen(s, "r"))) {
	fprintf(stderr, "%s: ", progname);
	perror(s);
	exit(1);
    }
    cur_ns = ns_create(s);
    if (yyparse()) {
	fprintf(stderr, "%s: stab file didn't parse\n", progname);
	exit(1);
    }
E 2
I 2
    int ret;

    if (len) {
	strncpy(small_buf, s, len);
	small_buf[sizeof(small_buf) - 1] = 0;
	parse_line = small_buf;
    } else
	parse_line = s;
    parse_position = parse_line;
    cur_ns = ns;
    cur_sym = 0;
    ret = yyparse();
    if (s_out)
	*s_out = cur_sym;
    return ret;
E 2
}
E 1
