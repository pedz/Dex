%{

/*
 * The stab grammar is defined here:
 * http://publib.boulder.ibm.com/infocenter/pseries/v5r3/topic/com.ibm.aix.files/doc/aixfiles/XCOFF.htm#jtsci131jbau
 */

static char sccs_id[] = "@(#)stab.y	1.11";

#include <strings.h>
#include <stdlib.h>
#include <stdio.h>
#include "map.h"
#include "sym.h"
#include "tree.h"
#include "dex.h"
#include "stab_pre.h"

static char buf[128];			/* buf for names and stuff */
static char *nextp = buf;		/* pointer into aclbuf */

static void bogus(int n);

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

static ns *cur_ns;
static symptr cur_sym;
static char *parse_line;
static char *parse_position;
static char small_buf[32];
static int yylex(void);
static yyerror(char *s);
static void bogus(int n);

int yyparse(void);

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
%type <name> NAME STRING bound DIGITSTRING
%type <param> tparamlist tparam
%type <rval> REAL
%type <type> typedef array subrange proceduretype record
%type <type> symbol_declaration
%type <val> INTEGER HEXINTEGER typeid numparams passby typenum
%type <val> ordvalue numelements numbits numbytes bitpattern bitoffset
%type <val> variable parameter proc procedure

%%
stab_def
    : /* empty */
    | NAME
	{
	}
    | NAME ':' class
	{
	}
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
	    cur_sym = enter_sym(cur_ns, $1, 0);
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
    ;

class
    : 'c' '=' constant ';'
    | label
    ;

symbol_declaration
    : parameter
	{
	    $$ = find_type(cur_ns, $1);
	}
    | procedure
	{
	    $$ = newtype(cur_ns, PROC_TYPE);
	    $$->t_val.val_f.f_typeptr = find_type(cur_ns, $1);
	    $$->t_val.val_f.f_params = -1;
	    $$->t_val.val_f.f_paramlist = 0;
	}
    | variable
	{
	    $$ = find_type(cur_ns, $1);
	}
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
	{
	    yyerror("Can't do this");
	}
    ;

variable
    : local_variable typeid		/* local */
	{
	    $$ = $2;
	}
    | global_variable typeid		/* float */
	{
	    $$ = $2;
	}
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
	{
	    $$ = $2;
	}
    | 'm' typeid			/* modula-2 ext. pascal */
	{
	    $$ = $2;
	}
    | 'J' typeid			/* internal function */
	{
	    $$ = $2;
	}
    | 'F' typeid			/* externalfunction */
	{
	    $$ = $2;
	}
    | 'l'				/* internal procedure */
	{
	    $$ = -1;
	}
    | 'P'				/* external procedure */
	{
	    $$ = -1;
	}
    | 'Q'				/* private procedure */
	{
	    $$ = -1;
	}
    ;

typeid
    : INTEGER
    | typenum typedef
	{
	    if ($2) {
		$2->t_attrs = 0;
		(void) insert_type($1, $2);
	    }
	    $$ = $1;
	}
    | typenum typeattrs typedef
	{
	    if ($3) {
		$3->t_attrs = $2;
		(void) insert_type($1, $3);
	    }
	    $$ = $1;
	}
    ;

typenum
    : INTEGER '='
	{
	    $$ = $1;
	}
    ;

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

	    $$ = newtype(cur_ns, t->t_type);
	    copy_type_and_record($$, t);
	}
    | 'b' typeid ';' numbytes		/* pascal space type */
	{
	    $$ = newtype(cur_ns, RANGE_TYPE);
	    bogus(__LINE__);
	}
    | 'c' typeid ';' numbits		/* complex type typeid */
	{
	    $$ = newtype(cur_ns, COMPLEX_TYPE);
	    $$->t_val.val_g.g_typeptr = find_type(cur_ns, $2);
	    $$->t_val.val_g.g_size = $4;
	}
    | 'd' typeid			/* file of type typeid */
	{
	    $$ = newtype(cur_ns, RANGE_TYPE);
	    bogus(__LINE__);
	}
    | 'e' enumspec opt_semi			/* enumerated type */
	{
	    $$ = newtype(cur_ns, ENUM_TYPE);
	    $$->t_val.val_e = $2;
	}
    | 'g' typeid ';' numbits		/* floating-point type of numbits */
	{
	    $$ = newtype(cur_ns, FLOAT_TYPE);
	    $$->t_val.val_g.g_typeptr = find_type(cur_ns, $2);
	    $$->t_val.val_g.g_size = $4;
	}
    | 'i' NAME ':' NAME ';'		/* import type modulename:name */
	{
	    $$ = newtype(cur_ns, RANGE_TYPE);
	    bogus(__LINE__);
	}
    | 'i' NAME ':' NAME ',' typeid ';'	/* import modulename:name typeid */
	{
	    $$ = newtype(cur_ns, RANGE_TYPE);
	    bogus(__LINE__);
	}
    | 'k' typeid
	{
	    $$ = newtype(cur_ns, CONSTANT_TYPE);
	    $$->t_val.val_k = find_type(cur_ns, $2);
	}
    | 'n' typeid ';' numbytes		/* string type with max length */
	{
	    $$ = newtype(cur_ns, RANGE_TYPE);
	    bogus(__LINE__);
	}
    | 'o' NAME ';'			/* opaque type */
	{
	    $$ = newtype(cur_ns, RANGE_TYPE);
	    bogus(__LINE__);
	}
    | 'o' NAME ',' typeid		/* opaque define typeid */
	{
	    $$ = newtype(cur_ns, RANGE_TYPE);
	    bogus(__LINE__);
	}
    | 'w'				/* wide character */
	{
	    $$ = newtype(cur_ns, RANGE_TYPE);
	    bogus(__LINE__);
	}
    | 'z' typeid ';' numbytes		/* pascal gstring type */
	{
	    $$ = newtype(cur_ns, RANGE_TYPE);
	    bogus(__LINE__);
	}
    | 'C' usage				/* cobol picture */
	{
	    $$ = newtype(cur_ns, RANGE_TYPE);
	    bogus(__LINE__);
	}
    | 'D' INTEGER ';' typeid		/* Decimal floating point */
	{
	    $$ = newtype(cur_ns, DECIMAL_FLOAT_TYPE);
	    $$->t_val.val_g.g_typeptr = find_type(cur_ns, $2);
	    $$->t_val.val_g.g_size = $4;
	}
    | 'K' cobolfiledesc			/* cobol file descriptor */
	{
	    $$ = newtype(cur_ns, RANGE_TYPE);
	    bogus(__LINE__);
	}
    | 'M' typeid ';' bound		/* multiple instance */
	{
	    $$ = newtype(cur_ns, RANGE_TYPE);
	    bogus(__LINE__);
	}
    | 'N' typeid			/* pascal stringptr */
	{
	    $$ = newtype(cur_ns, STRINGPTR_TYPE);
	    $$->t_val.val_N = $2;
	}
    | 'S' typeid			/* set of type typeid */
	{
	    $$ = newtype(cur_ns, RANGE_TYPE);
	    bogus(__LINE__);
	}
    | '*' typeid			/* pointer to type typeid */
	{
	    $$ = newtype(cur_ns, PTR_TYPE);
	    $$->t_val.val_p = find_type(cur_ns, $2);
	}
    | 'V' typeid
	{
	    $$ = newtype(cur_ns, VOLATILE);
	    $$->t_val.val_v = find_type(cur_ns, $2);
	}
    | 'Z'
	{
	    $$ = newtype(cur_ns, ELLIPSES);
	}
    | array
    | subrange
    | proceduretype
    | record
    ;

opt_semi
    : /* NULL */
    | ';'
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
	    $$ = newtype(cur_ns, ARRAY_TYPE);
	    $$->t_val.val_a.a_typedef = $2;
	    $$->t_val.val_a.a_typeptr = find_type(cur_ns, $4);
	}
    | 'A' typeid			/* open array */
	{
	    $$ = newtype(cur_ns, ARRAY_TYPE);
	    $$->t_val.val_a.a_typedef = 0;
	    $$->t_val.val_a.a_typeptr = find_type(cur_ns, $2);
	}
    | 'E' INTEGER ';' typeid		/* n-dimen. subarray  */
	{
	    $$ = newtype(cur_ns, RANGE_TYPE);
	    bogus(__LINE__);
	}
    | 'P' typedef ';' typeid		/* packed array */
	{
	    $$ = newtype(cur_ns, RANGE_TYPE);
	    bogus(__LINE__);
	}
    ;

subrange
    : 'r' typeid ';' bound ';' bound /* subrange with bounds */
	{
	    $$ = newtype(cur_ns, RANGE_TYPE);
	    $$->t_val.val_r.r_typeptr = find_type(cur_ns, $2);
	    $$->t_val.val_r.r_lower = $4;
	    $$->t_val.val_r.r_upper = $6;
	}
    ;

bound
    : DIGITSTRING			/* constant bound */
    | boundtype DIGITSTRING		/* var or dynamic bound */
	{
	    bogus(__LINE__);
	}
    | 'J'				/* bound is indeterminable */
	{
	    $$ = 0;
	}
    ;

boundtype
    : 'A'				/* passed by ref on stack */
    | 'T'				/* passed by value on stack */
    | 'a'				/* passed by ref in register */
    | 't'				/* passed by value in register */
    ;

proceduretype
    : 'f' typeid opt_semi		/* func returning type typeid */
	{
	    $$ = newtype(cur_ns, PROC_TYPE);
	    $$->t_val.val_f.f_typeptr = find_type(cur_ns, $2);
	    $$->t_val.val_f.f_params = -1;
	    $$->t_val.val_f.f_paramlist = 0;
	}
    | 'f' typeid ',' numparams ';' tparamlist ';' /* typeid func w n params*/
	{
	    $$ = newtype(cur_ns, PROC_TYPE);
	    $$->t_val.val_f.f_typeptr = find_type(cur_ns, $2);
	    $$->t_val.val_f.f_params = $4;
	    $$->t_val.val_f.f_paramlist = $6;
	}
    | 'p' numparams ';' tparamlist ';'	/* proc with n params */
	{
	    $$ = newtype(cur_ns, RANGE_TYPE);
	    bogus(__LINE__);
	}
    | 'R' numparams ';' namedtparamlist ';' /* pascal sub parameter */
	{
	    $$ = newtype(cur_ns, RANGE_TYPE);
	    bogus(__LINE__);
	}
    | 'F' typeid ',' numparams ';' namedtparamlist ';' /* func parameter */
	{
	    $$ = newtype(cur_ns, RANGE_TYPE);
	    bogus(__LINE__);
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
    : NAME ':' typeid ',' passby ';'
	{
	}
    ;

record
    : 's' numbytes ofieldlist ';'	/* struct */
	{
	    $$ = newtype(cur_ns, STRUCT_TYPE);
	    $$->t_val.val_s.s_size = $2;
	    $$->t_val.val_s.s_fields = $3;
	}
    | 'u' numbytes ofieldlist ';'	/* union */
	{
	    $$ = newtype(cur_ns, UNION_TYPE);
	    $$->t_val.val_s.s_size = $2;
	    $$->t_val.val_s.s_fields = $3;
	}
    | 'v' numbytes ofieldlist variantpart ';' /* variant record */
	{
	    $$ = newtype(cur_ns, RANGE_TYPE);
	    bogus(__LINE__);
	}
    | 'G' redefinition 'n' numbits fieldlist ';' /* cobol group w/o conds */
	{
	    $$ = newtype(cur_ns, RANGE_TYPE);
	    bogus(__LINE__);
	}
    | 'G' 'n' numbits fieldlist ';'
	{
	    $$ = newtype(cur_ns, RANGE_TYPE);
	    bogus(__LINE__);
	}
    | 'G' redefinition 'c' numbits condition fieldlist ';' /* with conds */
	{
	    $$ = newtype(cur_ns, RANGE_TYPE);
	    bogus(__LINE__);
	}
    | 'G' 'c' numbits condition fieldlist ';'
	{
	    $$ = newtype(cur_ns, RANGE_TYPE);
	    bogus(__LINE__);
	}
    | 'Y' numbytes classkey OptPBV OptBaseSpecList '(' ExtendedFieldList OptNameResolutionList ';'
	{
	    $$ = 0;
	}
    ;

classkey
    : 's'			/* struct */
    | 'u'			/* union */
    | 'c'			/* class */
    ;

/*
 * These guys are new for C++ and they are not fully implemented
 */
OptPBV
    : /* Empty */
    | 'V'			/* pass by value */
    ;

OptBaseSpecList
    : /* Empty */
    ;

ExtendedFieldList
    : /* Empty */
    | ExtendedField ExtendedFieldList
    ;

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
    | 'u'			/* Public */
    ;

AnonSpec
    : /* Empty */
    | 'a'			/* Anonymous union member */
    ;

VirtualSpec
    : /* Empty */
    | 'v' 'p'			/* Pure Virtual */
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
    | 'p' INTEGER NAME		/* Member is vtbl pointer; NAME is the external name of v-table. */
    ;

IsVBasePtr
    : /* Empty */
    | 'b'			/* Member is vbase pointer. */
    | 'r'			/* Member is vbase self-pointer. */
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
    : ':' typeid ',' bitoffset ',' numbits ';'
	{
	    $$ = newfield((char *)0, find_type(cur_ns, $2), $4, $6);
	}
    | NAME ':' typeid ',' bitoffset ',' numbits ';'
	{
	    $$ = newfield($1, find_type(cur_ns, $3), $5, $7);
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
    | 'r' typeid ';' bound ';' bound	/* subrange variant */
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
	{
	}
    | INTEGER				/* dec point in numeric pic */
	{
	}
    ;

picsize
    : INTEGER				/* number of repeated 9's */
	{
	}
    ;

condition
    : NAME ':' INTEGER '=' 'q' conditiontype ',' valuelist ';'
	{
	}
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
valuelist
    : value
    | valuelist value
    ;

value
    : INTEGER ':' STRING		/* int is length of string */
	{
	}
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
	    $$ = store_string(cur_ns, buf, 0, (char *)0);
	}
    ;

INTEGER
    : DIGITSTRING
	{
	    *nextp = 0;
	    $$ = atoi($1);
	}
    ;

DIGITSTRING
    : { nextp = buf; } '-' { *nextp++ = $<val>2; } digits
	{
	    *nextp = 0;
	    $$ = store_string(cur_ns, buf, 0, (char *)0);
	}
    | { nextp = buf; } digits
	{
	    *nextp = 0;
	    $$ = store_string(cur_ns, buf, 0, (char *)0);
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
	    $$ = store_string(cur_ns, buf, 0, (char *)0);
	}
    | { nextp = buf; } '\'' str '\''
	{
	    *nextp = '\0';
	    $$ = store_string(cur_ns, buf, 0, (char *)0);
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
#define DEBUG_BIT STAB_Y_BIT

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
static ns *cur_ns;
static symptr cur_sym;
static char *parse_line;
static char *parse_position;
static char small_buf[32];

static int yylex(void)			/* actually STABlex */
{
    int c;

    if (c = *parse_position)
	++parse_position;
    else
	c = EOF;
    yylval.c = c;
    return c;
}

static yyerror(char *s)			/* actually STABerror */
{
    fprintf(stderr, "%s parsing stab string %s at character %d\n", s,
	    parse_line, parse_position - parse_line);
}

static void bogus(int n)
{
    char buf[64];
    sprintf(buf, "unimplmented construct from line %d of stab.y", n);
    yyerror(buf);
    exit(1);
}

int parse_stab(ns *ns, char *s, int len, symptr *s_out)
{
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
}
