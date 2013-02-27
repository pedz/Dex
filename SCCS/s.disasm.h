h30609
s 00005/00000/00006
d D 1.3 02/03/14 16:12:32 pedz 3 2
e
s 00000/00000/00006
d D 1.2 00/09/18 17:57:00 pedz 2 1
c Checking before V5 conversion
e
s 00006/00000/00000
d D 1.1 95/04/25 10:15:28 pedz 1 0
c date and time created 95/04/25 10:15:28 by pedz
e
u
U
t
T
I 1

/* %W% */

I 3
#ifndef __DISASM_H
#define __DISASM_H

E 3
char *instr(long *addr);
void dis(long *addr, int count);
int regs_instr(long *addr, int count);
I 3

#endif /* __DISASM_H */
E 3
E 1
