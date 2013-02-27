h35494
s 00019/00000/00000
d D 1.1 02/03/14 16:44:15 pedz 1 0
c date and time created 02/03/14 16:44:15 by pedz
e
u
U
t
T
I 1

/* %W% */

#ifndef __SCAN_PRE_H
#define __SCAN_PRE_H

/*
 * lex does not produce a header file.  These variables are defined by
 * the scanner.  Their types need to be checked when porting because
 * they may change.
 */

extern int yylineno;
extern char *yyfilename;

void add_source(char *s);
int GRAMlex(void);

#endif /* __SCAN_PRE_H */
E 1
