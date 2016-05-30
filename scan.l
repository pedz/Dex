%{
static char sccs_id[] = "@(#)scan.l	1.9";

#define yylval GRAMlval

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <mbstr.h>
#include "dex.h"
#include "map.h"
#include "sym.h"
#include "tree.h"
#include "stmt.h"
#include "gram.h"
#include "inter.h"
#include "scan_pre.h"
#if USE_READLINE
#define READLINE_LIBRARY
#include "lib/history.h"
#include "lib/readline.h"
#endif

#define DEBUG_BIT SCAN_L_BIT

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
    { "dump", DUMP },			/* Extension */
    { "else", ELSE },
    /* { "entry", ENTRY }, */
    { "enum", ENUM },
    { "extern", EXTERN },
    { "float", FLOAT },
    { "for", FOR },
    { "goto", GOTO },
    { "if", IF },
    { "int", INT },
    { "long", LONG },
    { "print", PRINT },			/* Extension */
    { "register", REGISTER },
    { "return", RETURN },
    { "short", SHORT },
    { "signed", SIGNED },
    { "sizeof", SIZEOF },
    { "source", SOURCE },		/* Extension */
    { "static", STATIC },
    { "stmtdump", STMT_DUMP },		/* Extension */
    { "struct", STRUCT },
    { "switch", SWITCH },
    { "symdump", SYM_DUMP },		/* Extension */
    { "typedef", TYPEDEF },
    { "typedump", TYPE_DUMP },		/* Extension */
    { "union", UNION },
    { "unsigned", UNSIGNED },
    { "void", VOID },
    { "while", WHILE },
    0,
};

#define yylex GRAMlex
char string_buf[1024];
char *str_ptr;
large_t ltemp;

#ifdef getc
#undef getc
#endif

int yylook(void);
extern int yywrap(void);
int yywinput(void);

%}

%start normal string comment

%%
<normal>[$a-zA-Z_][$a-zA-Z0-9_]* {
    keyptr k;
    typeptr t;

    for (k = keywords; k->name; ++k)
	if (!strcmp(k->name, yytext)) {
	    yylval.ival = k->token;
	    return k->token;
	}
    if (t = name2typedef_all(yytext)) { /* look for typedefs */
	yylval.type = t;
	return TYPEDEFNAME;
    }

    yylval.str = store_string(ns_inter, yytext, 0, (char *)0);
    return IDENTIFIER;
}

<normal>[1-9][0-9]* {
    sscanf(yytext, "%lld", &ltemp);
    if (ltemp & 0xffffffff00000000LL) {
	yylval.llval = ltemp;
	return LLCON;
    }
    yylval.ival = (int)ltemp;
    return ICON;
}

<normal>0[0-7]* {
    sscanf(yytext, "%llo", &ltemp);
    if (ltemp & 0xffffffff00000000LL) {
	yylval.llval = ltemp;
	return LLCON;
    }
    yylval.ival = (int)ltemp;
    return ICON;
}

<normal>"0x"[0-9a-fA-F]+ {
    sscanf(yytext, "%llx", &ltemp);
    if (ltemp & 0xffffffff00000000LL) {
	yylval.llval = ltemp;
	return LLCON;
    }
    yylval.ival = (int)ltemp;
    return ICON;
}

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

<normal>'([^\\]|\\(.|[0-7][0-7][0-7]?))' {
    int i;
    char *p;

    if (yytext[1] == '\\') {
	switch (yytext[2]) {
	case '0': case '1': case '2': case '3':
	case '4': case '5': case '6': case '7':
	    for (i = 0, p = yytext + 2; *p != '\''; ++p) {
		i *= 8;
		i += *p - '0';
	    }
	    yylval.ival = i;
	    break;
	case '\'': yylval.ival = '\''; break;
	case '\\': yylval.ival = '\\'; break;
	case 'b': yylval.ival = '\b'; break;
	case 'f': yylval.ival = '\f'; break;
	case 'n': yylval.ival = '\n'; break;
	case 'r': yylval.ival = '\r'; break;
	case 't': yylval.ival = '\t'; break;
	default: yylval.ival = yytext[2]; break;
	}
    } else {
	yylval.ival = yytext[1];
    }
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
    yylval.ival = PTROP;
    return PTROP;
}

<normal>"<<" {
    yylval.ival = LSHIFT;
    return LSHIFT;
}

<normal>">>" {
    yylval.ival = RSHIFT;
    return RSHIFT;
}

<normal>"++" {
    yylval.ival = INCOP;
    return INCOP;
}

<normal>"--" {
    yylval.ival = DECOP;
    return DECOP;
}

<normal>"<=" {
    yylval.ival = LTOREQUAL;
    return LTOREQUAL;
}

<normal>">=" {
    yylval.ival = GTOREQUAL;
    return GTOREQUAL;
}

<normal>"==" {
    yylval.ival = EQUALITY;
    return EQUALITY;
}

<normal>"!=" {
    yylval.ival = NOTEQUAL;
    return NOTEQUAL;
}

<normal>"&&" {
    yylval.ival = ANDAND;
    return ANDAND;
}

<normal>"||" {
    yylval.ival = OROR;
    return OROR;
}

<normal>"+=" {
    yylval.ival = PLUSEQUAL;
    return PLUSEQUAL;
}

<normal>"-=" {
    yylval.ival = MINUSEQUAL;
    return MINUSEQUAL;
}

<normal>"*=" {
    yylval.ival = TIMESEQUAL;
    return TIMESEQUAL;
}

<normal>"/=" {
    yylval.ival = DIVEQUAL;
    return DIVEQUAL;
}

<normal>"%=" {
    yylval.ival = MODEQUAL;
    return MODEQUAL;
}

<normal>"<<=" {
    yylval.ival = LSEQUAL;
    return LSEQUAL;
}

<normal>">>=" {
    yylval.ival = RSEQUAL;
    return RSEQUAL;
}

<normal>"&=" {
    yylval.ival = ANDEQUAL;
    return ANDEQUAL;
}

<normal>"^=" {
    yylval.ival = XOREQUAL;
    return XOREQUAL;
}

<normal>"|=" {
    yylval.ival = OREQUAL;
    return OREQUAL;
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
}

<string>"\"" {
    BEGIN normal;
    *str_ptr++ = 0;
    yylval.str = store_string(ns_inter, string_buf, 0, (char *)0);
    return STRING;
}

<string>"\n" {
    fprintf(stderr, "%s: newline in string at line %d of %s\n",
	    progname, yyfilename ? yyfilename : "<stdin>", yylineno);
    BEGIN normal;
    *str_ptr++ = 0;
    yylval.str = store_string(ns_inter, string_buf, 0, (char *)0);
    return STRING;
}

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

<normal>"\n# "[0-9]+" \"".*"\"\n" {
}

<normal>" "|"\t" ;

<normal>"\n" {
#define xxx(a) # a
    DEBUG_PRINTF(("%s: %s:%d\n", xxx(yylex),
		  yyfilename ? yyfilename : "<stdin>", yylineno));
}

<normal>. {
    yylval.ival = yytext[0];
    return yytext[0];
}

.|"\n" {
    BEGIN normal;
    unput(yytext[0]);
}
%%

struct fs {
    struct fs *fs_next;
    char *fs_name;
    FILE *fs_file;
    int fs_line;
};
static struct fs *file_stack;
char *yyfilename = "<stdin>";

#if USE_READLINE

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

static int getc(FILE *in)
{
    int ret;

    while ((ret = isatty(fileno(yyin)) ? do_getc(yyin) : fgetc(yyin)) == EOF) {
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
