h08020
s 00001/00001/00025
d D 1.5 10/08/23 17:01:53 pedzan 5 4
e
s 00008/00000/00018
d D 1.4 02/03/14 16:12:32 pedz 4 3
e
s 00000/00000/00018
d D 1.3 00/09/18 17:57:00 pedz 3 2
c Checking before V5 conversion
e
s 00003/00000/00015
d D 1.2 95/02/01 10:36:54 pedz 2 1
e
s 00015/00000/00000
d D 1.1 94/08/22 17:56:33 pedz 1 0
c date and time created 94/08/22 17:56:33 by pedz
e
u
U
t
T
I 1

/* %W% */

I 4
#ifndef __FCALL_H
#define __FCALL_H

E 4
I 2
extern long frame_ptr;
extern long stack_ptr;

I 4
int setup_pseudos(void);
E 4
E 2
void alloc_stack(int n);
signed char sc_fcall(expr *n);
unsigned char uc_fcall(expr *n);
int i_fcall(expr *n);
unsigned int ui_fcall(expr *n);
short s_fcall(expr *n);
unsigned short us_fcall(expr *n);
long l_fcall(expr *n);
unsigned long ul_fcall(expr *n);
I 4
long long ll_fcall(expr *n);
unsigned long long ull_fcall(expr *n);
E 4
float f_fcall(expr *n);
double d_fcall(expr *n);
D 5
st st_fcall(expr *n);
E 5
I 5
st_t st_fcall(expr *n);
E 5
I 4

#endif /* __FCALL_H */
E 4
E 1
