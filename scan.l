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
#include "builtins.h"
#if USE_READLINE
#define READLINE_LIBRARY
#define __GNUC__ 1
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
static int write_pipe_open = 0;

#ifdef getc
#undef getc
#endif

int yylook(void);
extern int yywrap(void);
int yywinput(void);
void add_char(int c);
char *trim_str(char *s, char *e);

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

<normal>[1-9][0-9]*[uU] {
    sscanf(yytext, "%llu", &ltemp);
    if (ltemp & 0xffffffff00000000LL) {
	yylval.llval = ltemp;
	return LLCON;
    }
    yylval.ival = (int)ltemp;
    return ICON;
}

<normal>0[0-7]*[uU]? {
    sscanf(yytext, "%llo", &ltemp);
    if (ltemp & 0xffffffff00000000LL) {
	yylval.llval = ltemp;
	return LLCON;
    }
    yylval.ival = (int)ltemp;
    return ICON;
}

<normal>"0x"[0-9a-fA-F]+[uU]? {
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

<normal>[1-9][0-9]*[uU][lL] {
    sscanf(yytext, "%llu", &ltemp);
    if (ltemp & 0xffffffff00000000LL) {
	yylval.llval = ltemp;
	return LLCON;
    }
    yylval.lval = (long)ltemp;
    return LCON;
}

<normal>0[0-7]*[uU]?[lL] {
    sscanf(yytext, "%llo", &ltemp);
    if (ltemp & 0xffffffff00000000LL) {
	yylval.llval = ltemp;
	return LLCON;
    }
    yylval.lval = (long)ltemp;
    return LCON;
}

<normal>"0x"[0-9a-fA-F]+[uU]?[lL] {
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

<normal>[1-9][0-9]*[uU][lL][lL] {
    sscanf(yytext, "%llu", &yylval.llval);
    return LLCON;
}

<normal>0[0-7]*[uU]?[lL][lL] {
    sscanf(yytext, "%llo", &yylval.llval);
    return LLCON;
}

<normal>"0x"[0-9a-fA-F]+[uU]?[lL][lL] {
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

<normal>";"(" "|"\t")*"|".* {
    char *s = trim_str(strchr(yytext, '|') + 1, yytext + yyleng - 1);

    if (!isatty(fileno(yyin))) {
      fprintf(stderr, "%s: \"; | command\" construct found at line %d of %s\n",
	      progname, yylineno, yyfilename ? yyfilename : "<badness>");
      fprintf(stderr, "The \"; | command\" construct can only be done interactively\n");
      exit(1);
    }

    write_pipe_open = 1;
    dexout = popen(s, "w");
    yylval.ival = ';';
    return ';';
}

<normal>"}"(" "|"\t")*"|".* {
    char *s = trim_str(strchr(yytext, '|') + 1, yytext + yyleng - 1);

    if (!isatty(fileno(yyin))) {
      fprintf(stderr, "%s: \"; | command\" construct found at line %d of %s\n",
	      progname, yylineno, yyfilename ? yyfilename : "<badness>");
      fprintf(stderr, "The \"} | command\" construct can only be done interactively\n");
      exit(1);
    }

    write_pipe_open = 1;
    dexout = popen(s, "w");
    yylval.ival = '}';
    return '}';
}

<normal>";"(" "|"\t")*">".* {
    char *s = trim_str(strchr(yytext, '>') + 1, yytext + yyleng - 1);

    if (!isatty(fileno(yyin))) {
      fprintf(stderr, "%s: \"; > file\" construct found at line %d of %s\n",
	      progname, yylineno, yyfilename ? yyfilename : "<badness>");
      fprintf(stderr, "The \"; > file\" construct can only be done interactively\n");
      exit(1);
    }

    dexout = fopen(s, "w");
    yylval.ival = ';';
    return ';';
}

<normal>"}"(" "|"\t")*">".* {
    char *s = trim_str(strchr(yytext, '>') + 1, yytext + yyleng - 1);

    if (!isatty(fileno(yyin))) {
      fprintf(stderr, "%s: \"; > file\" construct found at line %d of %s\n",
	      progname, yylineno, yyfilename ? yyfilename : "<badness>");
      fprintf(stderr, "The \"} > file\" construct can only be done interactively\n");
      exit(1);
    }

    dexout = fopen(s, "w");
    yylval.ival = '}';
    return '}';
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
    add_char('\0');
    yylval.str = store_string(ns_inter, string_buf, 0, (char *)0);
    return STRING;
}

<string>"\n" {
    fprintf(stderr, "%s: newline in string at line %d of %s\n",
	    progname, yylineno, yyfilename ? yyfilename : "<badness>");
    BEGIN normal;
    add_char('\0');
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
	add_char(i);
	break;
    case '"':  add_char('"'); break;
    case '\'': add_char('\''); break;
    case '\\': add_char('\\'); break;
    case 'b':  add_char('\b'); break;
    case 'f':  add_char('\f'); break;
    case 'n':  add_char('\n'); break;
    case 'r':  add_char('\r'); break;
    case 't':  add_char('\t'); break;
    default:   add_char(yytext[1]); break;
    }
}

<string>"$"[A-Za-z0-9_]+ {
    char *t = getenv(yytext + 1);
    int len;

    if (!t)
	t = yytext;
    while (*t)
	add_char(*t++);
}

<string>. {
    add_char(yytext[0]);
}

<normal>"\"" {
    str_ptr = string_buf;
    BEGIN string;
}

<normal>"\n# "[0-9]+" \"".*"\"\n" {
}

<normal>" "|"\t" ;

<normal>\n {
#define xxx(a) # a
    DEBUG_PRINTF(("%s: %s:%d\n", xxx(yylex),
		  yyfilename ? yyfilename : "<badness>", yylineno));
}

<normal>. {
    /*
     * Any other character like . or ; is returned.
     */
    yylval.ival = yytext[0];
    return yytext[0];
}

.|"\n" {
    /*
     * There is no mechanism to start off in a particular state so
     * this rule is here just to get us from the start state to the
     * normal state on the first character we see.
     */
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
char *yyfilename;

#if USE_READLINE

static char *history_file_path = ".dex_history";

static void write_history_file(void)
{
    if (history_file_path)
	write_history(history_file_path);
}

static void read_history_file(void)
{
    char *env = getenv("DEX_HISTFILE");

    if (env)
	history_file_path = env;
    read_history(history_file_path);
    atexit(write_history_file);
}

static char *do_fgets(FILE *in)
{
    static char *last;
    static int first_time = 1;

    if (first_time) {
      first_time = 0;
      read_history_file();
    }

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
		fprintf(stderr, "error from history_expand for '%s'\n", last);
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

    fflush(stdout);
    return fgets(buf, sizeof(buf), in);
}
#endif

/*
 * This routine can not really fail but it returns 1 so it can be used
 * inside the if clause.
 */
static int reset_files(void)
{
    if (dexin != stdin) {
	fclose(dexin);
	dexin = stdin;
    }

    if (dexout != stdout) {
	if (write_pipe_open) {
	    write_pipe_open = 0;
	    pclose(dexout);
	} else {
	    fclose(dexout);
	}
	dexout = stdout;
    }

    if (dexerr != stderr) {
	fclose(dexerr);
	dexerr = stderr;
    }
    return 1;
}

/*
 * fgets and readline both eat the ending new line.  This makes the
 * scanner harder to write for SHELL_COMMAND and PATH since they are
 * going to end with a new line.  It also screws with the one
 * character look ahead.
 *
 * if ptr is NULL, we call do_fgets.
 * else if *ptr is '\0', ptr is set to NULL and we return '\n'
 * else we return *ptr++;
 */
static int do_getc(FILE *in)
{
    static char *ptr;

    if (ptr == NULL &&
	reset_files() &&
	((ptr = do_fgets(in))) == NULL)
	return EOF;

    if (*ptr == '\0') {
	ptr = NULL;
	return '\n';
    }

    return *ptr++;
}

void set_source(FILE *f, int lineno, char *filename)
{
    yyin = f;
    yylineno = lineno;
    yyfilename = filename;
}

static int getc(FILE *in)
{
    int ret;

    if (yyin == (FILE *)-1)
	return EOF;

    while ((ret = isatty(fileno(yyin)) ? do_getc(yyin) : fgetc(yyin)) == EOF) {
	struct fs *f;

	if (!(f = file_stack))
	    break;
	fclose(yyin);
	set_source(file_stack->fs_file, file_stack->fs_line, file_stack->fs_name);
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
    set_source(temp, 1, s);
}

void add_char(int c)
{
    static int warned = 0;

    if (str_ptr >= (string_buf + sizeof(string_buf))) {
	if (warned == 0) {
	    fprintf(stderr, "%s: string too long at line %d of %s\n",
		    progname, yylineno, yyfilename ? yyfilename : "<badness>");
	    warned = 1;
	}
    } else {
	warned = 0;
	*str_ptr++ = c;
    }
}

char *trim_str(char *s, char *e)
{
    while (*s == ' ' || *s == '\t')
	++s;
    
    while (*e == ' ' || *e == '\t')
	*e-- = '\0';

    return s;
}
