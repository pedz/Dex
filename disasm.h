
/* @(#)disasm.h	1.3 */

#ifndef __DISASM_H
#define __DISASM_H

char *instr(long *addr);
void dis(long *addr, int count);
int regs_instr(long *addr, int count);

#endif /* __DISASM_H */
