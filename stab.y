%{

static char sccs_id[] = "@(#)stab.y	1.1";

#include <strings.h>
#include <stdlib.h>
#include "map.h"
#include "sym.h"
#include "tree.h"
#include "dex.h"
static char buf[128];			/* buf for names and stuff */
static char *nextp = buf;		/* pointer into aclbuf */

static void bogus();
static char *current_name;
static int current_typedef;
static int current_typeid;

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
%type <name> NAME STRING
%type <param> tparamlist tparam
%type <rval> REAL
%type <type> typedef array subrange proceduretype record
%type <val> INTEGER HEXINTEGER typeid bound numparams passby typenum
%type <val> ordvalue numelements numbits numbytes bitpattern bitoffset
%type <val> namedtype variable parameter
    
%%
file_def
    : stab_def '\n'
    | file_def stab_def '\n'
    ;

stab_def
    : /* empty */
    | NAME ':' { current_name = $1; } class
    | ':' { current_name = 0; } class
    | NAME
    ;

class
    : 'c' '=' constant ';'
    | namedtype
    | parameter
    | procedure
    | variable
    | label
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
    ;

variable
    : typeid				/* local */
	{
	    $$ = $1;
	}
    | 'd' typeid			/* float */
	{
	    $$ = $2;
	}
    | 'r' typeid			/* register */
	{
	    $$ = $2;
	}
    | 'G' typeid			/* global */
	{
	    $$ = $2;
	}
    | 'S' typeid			/* c static global */
	{
	    $$ = $2;
	}
    | 'V' typeid			/* c static local */
	{
	    $$ = $2;
	}
    ;

label : 'L' ;				/* label name */

proc
    : 'f' typeid			/* private func of type typeid */
    | 'm' typeid			/* modula-2 ext. pascal */
    | 'J' typeid			/* internal function */
    | 'F' typeid			/* externalfunction */
    | 'l'				/* internal procedure */
    | 'P'				/* external procedure */
    | 'Q'				/* private procedure */
    ;

typeid
    : INTEGER
    | typenum typedef 
	{
	    if ($2)
		$2->t_attrs = 0;
	}
    | typenum typeattrs typedef
	{
	    if ($3)
		$3->t_attrs = $2;
	}
    ;

typenum
    : INTEGER '='
	{
	    current_typeid = $1;
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

	    $$ = newtype(cur_ns, t->t_type, current_name, current_typedef);
	    insert_type(current_typeid, $$);
	    copy_type($$, t);
	}
    | 'b' typeid ';' numbytes		/* pascal space type */
	{
	    $$ = newtype(cur_ns, RANGE_TYPE, current_name, current_typedef);
	    insert_type(current_typeid, $$);
	    bogus();
	}
    | 'c' typeid ';' numbits		/* complex type typeid */
	{
	    $$ = newtype(cur_ns, COMPLEX_TYPE, current_name, current_typedef);
	    insert_type(current_typeid, $$);
	    $$->t_val.val_g.g_typeptr = find_type(cur_ns, $2);
	    $$->t_val.val_g.g_size = $4;
	}
    | 'd' typeid			/* file of type typeid */
	{
	    $$ = newtype(cur_ns, RANGE_TYPE, current_name, current_typedef);
	    insert_type(current_typeid, $$);
	    bogus();
	}
    | 'e' enumspec ';'			/* enumerated type */
	{
	    $$ = newtype(cur_ns, ENUM_TYPE, current_name, current_typedef);
	    insert_type(current_typeid, $$);
	    $$->t_val.val_e = $2;
	}
    | 'g' typeid ';' numbits		/* floating-point type of numbits */
	{
	    $$ = newtype(cur_ns, FLOAT_TYPE, current_name, current_typedef);
	    insert_type(current_typeid, $$);
	    $$->t_val.val_g.g_typeptr = find_type(cur_ns, $2);
	    $$->t_val.val_g.g_size = $4;
	}
    | 'i' NAME ':' NAME ';'		/* import type modulename:name */
	{
	    $$ = newtype(cur_ns, RANGE_TYPE, current_name, current_typedef);
	    insert_type(current_typeid, $$);
	    bogus();
	}
    | 'i' NAME ':' NAME ',' typeid ';'	/* import modulename:name typeid */
	{
	    $$ = newtype(cur_ns, RANGE_TYPE, current_name, current_typedef);
	    insert_type(current_typeid, $$);
	    bogus();
	}
    | 'n' typeid ';' numbytes		/* string type with max length */
	{
	    $$ = newtype(cur_ns, RANGE_TYPE, current_name, current_typedef);
	    insert_type(current_typeid, $$);
	    bogus();
	}
    | 'o' NAME ';'			/* opaque type */
	{
	    $$ = newtype(cur_ns, RANGE_TYPE, current_name, current_typedef);
	    insert_type(current_typeid, $$);
	    bogus();
	}
    | 'o' NAME ',' typeid		/* opaque define typeid */
	{
	    $$ = newtype(cur_ns, RANGE_TYPE, current_name, current_typedef);
	    insert_type(current_typeid, $$);
	    bogus();
	}
    | 'w'				/* wide character */
	{
	    $$ = newtype(cur_ns, RANGE_TYPE, current_name, current_typedef);
	    insert_type(current_typeid, $$);
	    bogus();
	}
    | 'z' typeid ';' numbytes		/* pascal gstring type */
	{
	    $$ = newtype(cur_ns, RANGE_TYPE, current_name, current_typedef);
	    insert_type(current_typeid, $$);
	    bogus();
	}
    | 'C' usage				/* cobol picture */
	{
	    $$ = newtype(cur_ns, RANGE_TYPE, current_name, current_typedef);
	    insert_type(current_typeid, $$);
	    bogus();
	}
    | 'K' cobolfiledesc			/* cobol file descriptor */
	{
	    $$ = newtype(cur_ns, RANGE_TYPE, current_name, current_typedef);
	    insert_type(current_typeid, $$);
	    bogus();
	}
    | 'M' typeid ';' bound		/* multiple instance */
	{
	    $$ = newtype(cur_ns, RANGE_TYPE, current_name, current_typedef);
	    insert_type(current_typeid, $$);
	    bogus();
	}
    | 'N' typeid			/* pascal stringptr */
	{
	    $$ = newtype(cur_ns,STRINGPTR_TYPE,current_name,current_typedef);
	    insert_type(current_typeid, $$);
	    $$->t_val.val_N = $2;
	}
    | 'S' typeid			/* set of type typeid */
	{
	    $$ = newtype(cur_ns, RANGE_TYPE, current_name, current_typedef);
	    insert_type(current_typeid, $$);
	    bogus();
	}
    | '*' typeid			/* pointer to type typeid */
	{
	    $$ = newtype(cur_ns, PTR_TYPE, current_name, current_typedef);
	    insert_type(current_typeid, $$);
	    $$->t_val.val_p = find_type(cur_ns, $2);
	}
    | 'Z'
	{
	    $$ = newtype(cur_ns, ELLIPSES, current_name, current_typedef);
	    insert_type(current_typeid, $$);
	}
    | array
    | subrange
    | proceduretype
    | record
    ;

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
	    $$ = newtype(cur_ns, ARRAY_TYPE, current_name, current_typedef);
	    insert_type(current_typeid, $$);
	    $$->t_val.val_a.a_typedef = $2;
	    $$->t_val.val_a.a_typeptr = find_type(cur_ns, $4);
	}
    | 'A' typeid			/* open array */
	{
	    $$ = newtype(cur_ns, ARRAY_TYPE, current_name, current_typedef);
	    insert_type(current_typeid, $$);
	    $$->t_val.val_a.a_typedef = 0;
	    $$->t_val.val_a.a_typeptr = find_type(cur_ns, $2);
	}
    | 'D' INTEGER ';' typeid		/* n-dimen. dynamic array */
	{
	    $$ = newtype(cur_ns, RANGE_TYPE, current_name, current_typedef);
	    insert_type(current_typeid, $$);
	    bogus();
	}
    | 'E' INTEGER ';' typeid		/* n-dimen. subarray  */
	{
	    $$ = newtype(cur_ns, RANGE_TYPE, current_name, current_typedef);
	    insert_type(current_typeid, $$);
	    bogus();
	}
    | 'P' typedef ';' typeid		/* packed array */
	{
	    $$ = newtype(cur_ns, RANGE_TYPE, current_name, current_typedef);
	    insert_type(current_typeid, $$);
	    bogus();
	}
    ;

subrange
    : 'r' typeid ';' bound ';' bound /* subrange with bounds */
	{
	    $$ = newtype(cur_ns, RANGE_TYPE, current_name, current_typedef);
	    insert_type(current_typeid, $$);
	    $$->t_val.val_r.r_typeptr = find_type(cur_ns, $2);
	    $$->t_val.val_r.r_lower = $4;
	    $$->t_val.val_r.r_upper = $6;
	}
    ;

bound
    : INTEGER				/* constant bound */
    | boundtype INTEGER			/* var or dynamic bound */
	{
	    bogus();
	}
    | 'J'				/* bound is indeterminable */
	{
	    bogus();
	}
    ;

boundtype
    : 'A'				/* passed by ref on stack */
    | 'T'				/* passed by vlue on stack */
    | 'a'				/* passed by ref in register */
    | 't'				/* passed by value in register */
    ;

proceduretype
    : 'f' typeid ';'		/* func returning type typeid */
	{
	    $$ = newtype(cur_ns, PROC_TYPE, current_name, current_typedef);
	    insert_type(current_typeid, $$);
	    $$->t_val.val_f.f_typeptr = find_type(cur_ns, $2);
	    $$->t_val.val_f.f_params = -1;
	    $$->t_val.val_f.f_paramlist = 0;
	}
    | 'f' typeid ',' numparams ';' tparamlist ';' /* typeid func w n params*/
	{
	    $$ = newtype(cur_ns, PROC_TYPE, current_name, current_typedef);
	    insert_type(current_typeid, $$);
	    $$->t_val.val_f.f_typeptr = find_type(cur_ns, $2);
	    $$->t_val.val_f.f_params = $4;
	    $$->t_val.val_f.f_paramlist = $6;
	}
    | 'p' numparams ';' tparamlist ';'	/* proc with n params */
	{
	    $$ = newtype(cur_ns, RANGE_TYPE, current_name, current_typedef);
	    insert_type(current_typeid, $$);
	    bogus();
	}
    | 'R' numparams ';' namedtparamlist ';' /* pascal sub parameter */
	{
	    $$ = newtype(cur_ns, RANGE_TYPE, current_name, current_typedef);
	    insert_type(current_typeid, $$);
	    bogus();
	}
    | 'F' typeid ',' numparams ';' namedtparamlist ';' /* func parameter */
	{
	    $$ = newtype(cur_ns, RANGE_TYPE, current_name, current_typedef);
	    insert_type(current_typeid, $$);
	    bogus();
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
    : NAME ':' typeid ',' passby ';' ;

record
    : 's' numbytes ofieldlist ';'	/* struct */
	{
	    $$ = newtype(cur_ns, STRUCT_TYPE, current_name, current_typedef);
	    insert_type(current_typeid, $$);
	    $$->t_val.val_s.s_size = $2;
	    $$->t_val.val_s.s_fields = $3;
	}
    | 'u' numbytes ofieldlist ';'	/* union */
	{
	    $$ = newtype(cur_ns, UNION_TYPE, current_name, current_typedef);
	    insert_type(current_typeid, $$);
	    $$->t_val.val_s.s_size = $2;
	    $$->t_val.val_s.s_fields = $3;
	}
    | 'v' numbytes ofieldlist variantpart ';' /* variant record */
	{
	    $$ = newtype(cur_ns, RANGE_TYPE, current_name, current_typedef);
	    insert_type(current_typeid, $$);
	    bogus();
	}
    | 'G' redefinition 'n' numbits fieldlist ';' /* cobol group w/o conds */
	{
	    $$ = newtype(cur_ns, RANGE_TYPE, current_name, current_typedef);
	    insert_type(current_typeid, $$);
	    bogus();
	}
    | 'G' 'n' numbits fieldlist ';'
	{
	    $$ = newtype(cur_ns, RANGE_TYPE, current_name, current_typedef);
	    insert_type(current_typeid, $$);
	    bogus();
	}
    | 'G' redefinition 'c' numbits condition fieldlist ';' /* with conds */
	{
	    $$ = newtype(cur_ns, RANGE_TYPE, current_name, current_typedef);
	    insert_type(current_typeid, $$);
	    bogus();
	}
    | 'G' 'c' numbits condition fieldlist ';'
	{
	    $$ = newtype(cur_ns, RANGE_TYPE, current_name, current_typedef);
	    insert_type(current_typeid, $$);
	    bogus();
	}
    | 'Y' numbytes classkey OptPBV OptBaseSpecList '(' ExtendedFieldList
	  OptNameResolutionList ';'
	{
	    $$ = 0;
	}
    ;

classkey
    : 's'
    | 'u'
    | 'c'
    ;

/*
 * These guys are new for C++ and they are not fully implemented
 */
OptPBV
    : /* Empty */
    ;

OptBaseSpecList
    : /* Empty */
    ;

ExtendedFieldList
    : /* Empty */
    ;

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
    : NAME ':' typeid ',' bitoffset ',' numbits ';'
	{
	    $$ = newfield($1, $3, $5, $7);
	}
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
    | 'r' typeid ';' bound ';' bound	/* subrante variant */
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
    | INTEGER				/* dec point in numeric pic */
    ;

picsize
    : INTEGER				/* number of repeated 9's */
    ;

condition
    : NAME ':' INTEGER '=' 'q' conditiontype ',' valuelist ';'
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

decimalsite : INTEGER ;
kanjichar : INTEGER ;
valuelist
    : value
    | valuelist value
    ;

value
    : INTEGER ':' STRING		/* int is length of string */
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
	    $$ = strcpy(smalloc(nextp - buf + 1, __FILE__, __LINE__), buf);
	}
    ;

INTEGER
    : { nextp = buf; } '-' { *nextp++ = $<val>2; } digits
	{
	    *nextp = 0;
	    $$ = atoi(buf);
	}
    | { nextp = buf; } digits
	{
	    *nextp = 0;
	    $$ = atoi(buf);
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
	    $$ = strcpy(smalloc(nextp - buf + 1, __FILE__, __LINE__), buf);
	}
    | { nextp = buf; } '\'' str '\''
	{
	    *nextp = '\0';
	    $$ = strcpy(smalloc(nextp - buf + 1, __FILE__, __LINE__), buf);
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
#include <stdio.h>

static lineno = 1;
static charno = 0;
static FILE *stabfile;
static ns *cur_ns;

static yylex()				/* actually STABlex */
{
    int c = getc(stabfile);
    extern int yydebug;

    ++charno;
    if (c == '\n') {
	++lineno;
	charno = 0;
    }
    yylval.c = c;
    return c;
}

static yyerror(char *s)			/* actually STABerror */
{
    fprintf(stderr, "%s parsing stab file at line %d and char %d\n", s,
	    lineno, charno);
}

static void bogus()
{
    yyerror("unimplmented construct");
    exit(1);
}

int parse_stab(char *s)
{
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
}
