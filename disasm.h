
/* @(#)disasm.h	1.1 */

char *instr(long *addr);
void dis(long *addr, int count);
int regs_instr(long *addr, int count);
