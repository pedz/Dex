
/* @(#)gram_pre.h	1.1 */

#ifndef __GRAM_PRE_H
#define __GRAM_PRE_H

long global_index;

void yyerror(char *s, ...);
int GRAMparse(void);
void GRAMerror(char *s, ...);

#endif /* __GRAM_PRE_H */
