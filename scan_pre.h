
/* @(#)scan_pre.h	1.1 */

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
