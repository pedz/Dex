h02835
s 00001/00001/00534
d D 1.9 10/08/23 17:24:40 pedzan 9 8
c Checking in changes made over the years
e
s 00128/00041/00407
d D 1.8 02/03/14 16:12:33 pedz 8 7
e
s 00000/00000/00448
d D 1.7 00/09/18 17:57:02 pedz 7 6
c Checking before V5 conversion
e
s 00005/00004/00443
d D 1.6 00/05/29 15:09:08 pedz 6 5
c Just before IA64 conversion
e
s 00066/00001/00381
d D 1.5 97/05/13 16:02:58 pedz 5 4
c Check pointer
e
s 00002/00000/00380
d D 1.4 95/04/25 10:10:16 pedz 4 3
e
s 00016/00002/00364
d D 1.3 95/02/09 21:35:53 pedz 3 2
e
s 00067/00011/00299
d D 1.2 95/02/01 10:37:02 pedz 2 1
e
s 00310/00000/00000
d D 1.1 94/08/22 17:56:35 pedz 1 0
c date and time created 94/08/22 17:56:35 by pedz
e
u
U
t
T
I 1
%{
static char sccs_id[] = "%W%";

#define yylval GRAMlval

#include <stdlib.h>
#include <string.h>
I 8
#include <unistd.h>
#include <mbstr.h>
E 8
I 6
#include "dex.h"
E 6
#include "map.h"
#include "sym.h"
#include "tree.h"
#include "stmt.h"
#include "gram.h"
#include "inter.h"
D 6
#include "dex.h"
E 6
I 6
D 8
#include "scan.h"
E 8
I 8
#include "scan_pre.h"
#if USE_READLINE
#define READLINE_LIBRARY
#include "lib/history.h"
#include "lib/readline.h"
#endif
E 8
E 6

I 8
#define DEBUG_BIT SCAN_L_BIT

E 8
typedef struct keywords *keyptr;
struct keywords {
    char *name;
    int token;
} keywords[] = {
    { "auto", AUTO },
    { "break", BREAK },
    { "case", CASE },
    { "char", CHAR },
    { "continue", CONTINUE },
    { "default", DEFAULT },
    { "do", DO },
    { "double", DOUBLE },
D 2
    { "dump", DUMP },
E 2
I 2
    { "dump", DUMP },			/* Extension */
E 2
    { "else", ELSE },
D 9
    { "entry", ENTRY },
E 9
I 9
    /* { "entry", ENTRY }, */
E 9
    { "enum", ENUM },
    { "extern", EXTERN },
    { "float", FLOAT },
    { "for", FOR },
    { "goto", GOTO },
    { "if", IF },
    { "int", INT },
I 2
D 3
    { "load", LOAD },
E 3
E 2
    { "long", LONG },
D 2
    { "nm", NM },
    { "print", PRINT },
E 2
I 2
    { "print", PRINT },			/* Extension */
E 2
    { "register", REGISTER },
    { "return", RETURN },
    { "short", SHORT },
    { "signed", SIGNED },
    { "sizeof", SIZEOF },
D 2
    { "stab", STAB},
E 2
I 2
    { "source", SOURCE },		/* Extension */
E 2
    { "static", STATIC },
I 4
    { "stmtdump", STMT_DUMP },		/* Extension */
E 4
    { "struct", STRUCT },
    { "switch", SWITCH },
D 2
    { "symdump", SYM_DUMP },
E 2
I 2
    { "symdump", SYM_DUMP },		/* Extension */
E 2
    { "typedef", TYPEDEF },
I 4
    { "typedump", TYPE_DUMP },		/* Extension */
E 4
    { "union", UNION },
    { "unsigned", UNSIGNED },
    { "void", VOID },
    { "while", WHILE },
    0,
};

#define yylex GRAMlex
char string_buf[1024];
char *str_ptr;
I 8
large_t ltemp;
E 8

I 2
#ifdef getc
#undef getc
#endif

I 8
int yylook(void);
extern int yywrap(void);
int yywinput(void);

E 8
E 2
%}

D 3
%start normal string
E 3
I 3
%start normal string comment
E 3

%%
D 2
<normal>[a-zA-Z_][a-zA-Z0-9_]* {
E 2
I 2
<normal>[$a-zA-Z_][$a-zA-Z0-9_]* {
E 2
    keyptr k;
    typeptr t;

    for (k = keywords; k->name; ++k)
	if (!strcmp(k->name, yytext)) {
I 3
D 8
	    yylval.val = k->token;
E 8
I 8
	    yylval.ival = k->token;
E 8
E 3
	    return k->token;
	}
D 2
    if (t = name2typedef(yytext)) /* look for typedefs */
E 2
I 2
    if (t = name2typedef_all(yytext)) { /* look for typedefs */
	yylval.type = t;
E 2
	return TYPEDEFNAME;
I 2
    }
E 2

D 2
    yylval.str = store_name(ns_inter, yytext);
E 2
I 2
    yylval.str = store_string(ns_inter, yytext, 0, (char *)0);
E 2
    return IDENTIFIER;
}

<normal>[1-9][0-9]* {
D 6
    yylval.val = atoi(yytext);
E 6
I 6
D 8
    sscanf(yytext, "%lld", &yylval.val);
E 8
I 8
    sscanf(yytext, "%lld", &ltemp);
    if (ltemp & 0xffffffff00000000LL) {
	yylval.llval = ltemp;
	return LLCON;
    }
    yylval.ival = (int)ltemp;
E 8
E 6
    return ICON;
}

<normal>0[0-7]* {
D 6
    sscanf(yytext, "%o", &yylval.val);
E 6
I 6
D 8
    sscanf(yytext, "%llo", &yylval.val);
E 8
I 8
    sscanf(yytext, "%llo", &ltemp);
    if (ltemp & 0xffffffff00000000LL) {
	yylval.llval = ltemp;
	return LLCON;
    }
    yylval.ival = (int)ltemp;
E 8
E 6
    return ICON;
}

<normal>"0x"[0-9a-fA-F]+ {
D 6
    sscanf(yytext, "%x", &yylval.val);
E 6
I 6
D 8
    sscanf(yytext, "%llx", &yylval.val);
E 8
I 8
    sscanf(yytext, "%llx", &ltemp);
    if (ltemp & 0xffffffff00000000LL) {
	yylval.llval = ltemp;
	return LLCON;
    }
    yylval.ival = (int)ltemp;
E 8
E 6
    return ICON;
}

I 8
<normal>[1-9][0-9]*[lL] {
    sscanf(yytext, "%lld", &ltemp);
    if (ltemp & 0xffffffff00000000LL) {
	yylval.llval = ltemp;
	return LLCON;
    }
    yylval.lval = (long)ltemp;
    return LCON;
}

<normal>0[0-7]*[lL] {
    sscanf(yytext, "%llo", &ltemp);
    if (ltemp & 0xffffffff00000000LL) {
	yylval.llval = ltemp;
	return LLCON;
    }
    yylval.lval = (long)ltemp;
    return LCON;
}

<normal>"0x"[0-9a-fA-F]+[lL] {
    sscanf(yytext, "%llx", &ltemp);
    if (ltemp & 0xffffffff00000000LL) {
	yylval.llval = ltemp;
	return LLCON;
    }
    yylval.lval = (long)ltemp;
    return LCON;
}

<normal>[1-9][0-9]*[lL][lL] {
    sscanf(yytext, "%lld", &yylval.llval);
    return LLCON;
}

<normal>0[0-7]*[lL][lL] {
    sscanf(yytext, "%llo", &yylval.llval);
    return LLCON;
}

<normal>"0x"[0-9a-fA-F]+[lL][lL] {
    sscanf(yytext, "%llx", &yylval.llval);
    return LLCON;
}

E 8
<normal>'([^\\]|\\(.|[0-7][0-7][0-7]?))' {
    int i;
    char *p;

D 2
    if (yytext[1] = '\\')
E 2
I 2
    if (yytext[1] == '\\') {
E 2
	switch (yytext[2]) {
	case '0': case '1': case '2': case '3':
	case '4': case '5': case '6': case '7':
	    for (i = 0, p = yytext + 2; *p != '\''; ++p) {
		i *= 8;
		i += *p - '0';
	    }
D 8
	    yylval.val = i;
E 8
I 8
	    yylval.ival = i;
E 8
	    break;
D 8
	case '\'': yylval.val = '\''; break;
	case '\\': yylval.val = '\\'; break;
	case 'b': yylval.val = '\b'; break;
	case 'f': yylval.val = '\f'; break;
	case 'n': yylval.val = '\n'; break;
	case 'r': yylval.val = '\r'; break;
	case 't': yylval.val = '\t'; break;
	default: yylval.val = yytext[2]; break;
E 8
I 8
	case '\'': yylval.ival = '\''; break;
	case '\\': yylval.ival = '\\'; break;
	case 'b': yylval.ival = '\b'; break;
	case 'f': yylval.ival = '\f'; break;
	case 'n': yylval.ival = '\n'; break;
	case 'r': yylval.ival = '\r'; break;
	case 't': yylval.ival = '\t'; break;
	default: yylval.ival = yytext[2]; break;
E 8
	}
D 2
    else
E 2
I 2
    } else {
E 2
D 8
	yylval.val = yytext[1];
E 8
I 8
	yylval.ival = yytext[1];
E 8
I 2
    }
E 2
    return ICON;
}

<normal>[0-9]+("."[0-9]*)?[Ee][-+]?[0-9]+ {
    yylval.rval = atof(yytext);
    return RCON;
}

<normal>[0-9]*"."[0-9]+([Ee][-+]?[0-9]+)? {
    yylval.rval = atof(yytext);
    return RCON;
}

<normal>([0-9]+".")|"INF"|"QNAN"|"SNAN" {
    yylval.rval = atof(yytext);
    return RCON;
}

<normal>"->" {
D 8
    yylval.val = PTROP;
E 8
I 8
    yylval.ival = PTROP;
E 8
    return PTROP;
}

<normal>"<<" {
D 8
    yylval.val = LSHIFT;
E 8
I 8
    yylval.ival = LSHIFT;
E 8
    return LSHIFT;
}

<normal>">>" {
D 8
    yylval.val = RSHIFT;
E 8
I 8
    yylval.ival = RSHIFT;
E 8
    return RSHIFT;
}

<normal>"++" {
D 8
    yylval.val = INCOP;
E 8
I 8
    yylval.ival = INCOP;
E 8
    return INCOP;
}

<normal>"--" {
D 8
    yylval.val = DECOP;
E 8
I 8
    yylval.ival = DECOP;
E 8
    return DECOP;
}

<normal>"<=" {
D 8
    yylval.val = LTOREQUAL;
E 8
I 8
    yylval.ival = LTOREQUAL;
E 8
    return LTOREQUAL;
}

<normal>">=" {
D 8
    yylval.val = GTOREQUAL;
E 8
I 8
    yylval.ival = GTOREQUAL;
E 8
    return GTOREQUAL;
}

<normal>"==" {
D 8
    yylval.val = EQUALITY;
E 8
I 8
    yylval.ival = EQUALITY;
E 8
    return EQUALITY;
}

<normal>"!=" {
D 8
    yylval.val = NOTEQUAL;
E 8
I 8
    yylval.ival = NOTEQUAL;
E 8
    return NOTEQUAL;
}

<normal>"&&" {
D 8
    yylval.val = ANDAND;
E 8
I 8
    yylval.ival = ANDAND;
E 8
    return ANDAND;
}

<normal>"||" {
D 8
    yylval.val = OROR;
E 8
I 8
    yylval.ival = OROR;
E 8
    return OROR;
}

<normal>"+=" {
D 8
    yylval.val = PLUSEQUAL;
E 8
I 8
    yylval.ival = PLUSEQUAL;
E 8
    return PLUSEQUAL;
}

<normal>"-=" {
D 8
    yylval.val = MINUSEQUAL;
E 8
I 8
    yylval.ival = MINUSEQUAL;
E 8
    return MINUSEQUAL;
}

<normal>"*=" {
D 8
    yylval.val = TIMESEQUAL;
E 8
I 8
    yylval.ival = TIMESEQUAL;
E 8
    return TIMESEQUAL;
}

<normal>"/=" {
D 8
    yylval.val = DIVEQUAL;
E 8
I 8
    yylval.ival = DIVEQUAL;
E 8
    return DIVEQUAL;
}

<normal>"%=" {
D 8
    yylval.val = MODEQUAL;
E 8
I 8
    yylval.ival = MODEQUAL;
E 8
    return MODEQUAL;
}

<normal>"<<=" {
D 8
    yylval.val = LSEQUAL;
E 8
I 8
    yylval.ival = LSEQUAL;
E 8
    return LSEQUAL;
}

<normal>">>=" {
D 8
    yylval.val = RSEQUAL;
E 8
I 8
    yylval.ival = RSEQUAL;
E 8
    return RSEQUAL;
}

<normal>"&=" {
D 8
    yylval.val = ANDEQUAL;
E 8
I 8
    yylval.ival = ANDEQUAL;
E 8
    return ANDEQUAL;
}

<normal>"^=" {
D 8
    yylval.val = XOREQUAL;
E 8
I 8
    yylval.ival = XOREQUAL;
E 8
    return XOREQUAL;
}

<normal>"|=" {
D 8
    yylval.val = OREQUAL;
E 8
I 8
    yylval.ival = OREQUAL;
E 8
    return OREQUAL;
I 3
}

<normal>"/*" {
    BEGIN comment;
}

<comment>. {
}

<comment>\n {
}

<comment>"*/" {
    BEGIN normal;
E 3
}

<string>"\"" {
    BEGIN normal;
    *str_ptr++ = 0;
D 2
    yylval.str = store_name(ns_inter, string_buf);
E 2
I 2
    yylval.str = store_string(ns_inter, string_buf, 0, (char *)0);
E 2
    return STRING;
}

I 8
<string>"\n" {
    fprintf(stderr, "%s: newline in string at line %d of %s\n",
	    progname, yyfilename ? yyfilename : "<stdin>", yylineno);
    BEGIN normal;
    *str_ptr++ = 0;
    yylval.str = store_string(ns_inter, string_buf, 0, (char *)0);
    return STRING;
}

E 8
<string>\\(.|[0-7][0-7][0-7]?) {
    int i;
    char *p;

    switch (yytext[1]) {
    case '0': case '1': case '2': case '3':
    case '4': case '5': case '6': case '7':
	for (i = 0, p = yytext + 1; *p; ++p) {
	    i *= 8;
	    i += *p - '0';
	}
	*str_ptr++ = i;
	break;
    case '"': *str_ptr++ = '"'; break;
    case '\'': *str_ptr++ = '\''; break;
    case '\\': *str_ptr++ = '\\'; break;
    case 'b': *str_ptr++ = '\b'; break;
    case 'f': *str_ptr++ = '\f'; break;
    case 'n': *str_ptr++ = '\n'; break;
    case 'r': *str_ptr++ = '\r'; break;
    case 't': *str_ptr++ = '\t'; break;
    default: *str_ptr++ = yytext[1]; break;
    }
}

<string>"$"[A-Za-z0-9_]+ {
    char *t = getenv(yytext + 1);
    int len;

    if (!t)
	t = yytext;
    len = strlen(t);
    strcpy(str_ptr, t);
    str_ptr += len;
}

<string>. {
    *str_ptr++ = yytext[0];
}

<normal>"\"" {
    str_ptr = string_buf;
    BEGIN string;
}

I 5
<normal>"\n# "[0-9]+" \"".*"\"\n" {
}

E 5
D 8
<normal>" "|"\t"|"\n" ;
E 8
I 8
<normal>" "|"\t" ;
E 8

I 8
<normal>"\n" {
#define xxx(a) # a
    DEBUG_PRINTF(("%s: %s:%d\n", xxx(yylex),
		  yyfilename ? yyfilename : "<stdin>", yylineno));
}

E 8
<normal>. {
D 8
    yylval.val = yytext[0];
E 8
I 8
    yylval.ival = yytext[0];
E 8
    return yytext[0];
}

.|"\n" {
    BEGIN normal;
    unput(yytext[0]);
}
%%
I 2

struct fs {
    struct fs *fs_next;
    char *fs_name;
    FILE *fs_file;
    int fs_line;
};
static struct fs *file_stack;
D 8
char *yyfilename;
E 8
I 8
char *yyfilename = "<stdin>";
E 8

I 5
#if USE_READLINE
D 8
#include <readline/history.h>
#include <readline/readline.h>
E 8

static char *do_fgets(FILE *in)
{
    static char *last;

    for ( ; ; ) {
	if (last)
	    free(last);
	last = 0;
	last = readline("> ");
	if (last && *last) {
	    int rc;
	    char *expand;
	    
	    using_history();
	    rc = history_expand(last, &expand);
	    free(last);
	    last = expand;
	    if (rc)
		printf("%s\n", last);
	    if (rc < 0)
		continue;
	    if (!strcmp(last, "history")) {
		HIST_ENTRY **list = history_list();
		int i;

		if (list)
		    for (i = 0; list[i]; ++i)
			printf("%d: %s\n", i + history_base, list[i]->line);
		continue;
	    }
	    add_history(last);
	}
	break;
    }
    return last;
}
#else
static char *do_fgets(FILE *in)
{
    static char buf[1024];

    printf("> ");
    fflush(stdout);
    return fgets(buf, sizeof(buf), in);
}
#endif

static int do_getc(FILE *in)
{
    static char *ptr;
    int c;

    while (!ptr || !*ptr)
	if (!(ptr = do_fgets(in)))
	    return EOF;
    return *ptr++;
}

E 5
static int getc(FILE *in)
{
    int ret;

D 5
    while ((ret = fgetc(yyin)) == EOF) {
E 5
I 5
    while ((ret = isatty(fileno(yyin)) ? do_getc(yyin) : fgetc(yyin)) == EOF) {
E 5
	struct fs *f;

	if (!(f = file_stack))
	    break;
	fclose(yyin);
	yyin = file_stack->fs_file;
	yylineno = file_stack->fs_line;
	yyfilename = file_stack->fs_name;
	file_stack = file_stack->fs_next;
	free(f);
    }

    return ret;
}

void add_source(char *s)
{
    FILE *temp;
    struct fs *f;

    if (!(temp = fopen(s, "r"))) {
	perror(s);
	return;
    }
    f = new(struct fs);
    f->fs_next = file_stack;
    file_stack = f;
    f->fs_file = yyin;
    f->fs_line = yylineno;
    f->fs_name = yyfilename;
    yyin = temp;
    yylineno = 1;
    yyfilename = s;
}
E 2
E 1
