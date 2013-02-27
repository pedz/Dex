h12290
s 00021/00013/00009
d D 1.5 02/03/14 16:12:31 pedz 5 4
e
s 00000/00000/00022
d D 1.4 00/09/18 17:56:58 pedz 4 3
c Checking before V5 conversion
e
s 00012/00011/00010
d D 1.3 98/10/23 12:26:07 pedz 3 2
e
s 00013/00000/00008
d D 1.2 95/04/25 10:09:56 pedz 2 1
e
s 00008/00000/00000
d D 1.1 95/02/09 21:40:25 pedz 1 0
c date and time created 95/02/09 21:40:25 by pedz
e
u
U
t
T
I 1

/* %W% */

I 5
#ifndef __BUILTINS_H
#define __BUILTINS_H

int int_return_range(expr *n);
int int_regs_instr(expr *n);
E 5
I 2
D 3
int int_regs_instr(expr *n);
E 3
char *int_instr(expr *n);
D 3
char *int_strcpy(expr *n);
char *int_strncpy(expr *n);
E 3
D 5
char *int_strcat(expr *n);
D 3
char *int_strncat(expr *n);
int int_strcmp(expr *n);
int int_strncmp(expr *n);
E 3
char *int_strchr(expr *n);
E 5
I 3
char *int_strcpy(expr *n);
D 5
char *int_strncat(expr *n);
E 5
char *int_strncpy(expr *n);
I 5
char *int_strcat(expr *n);
char *int_strncat(expr *n);
int int_strcmp(expr *n);
int int_strlen(expr *n);
int int_strncmp(expr *n);
char *int_strchr(expr *n);
E 5
E 3
char *int_strrchr(expr *n);
D 3
int int_sprintf(expr *n);
E 2
int int_printf(expr *n);
int int_v2f(expr *n);
int int_find(expr *n);
E 3
D 5
int int_dis(expr *n);
I 3
int int_find(expr *n);
E 3
int int_load(expr *n);
I 2
D 3
int int_purge_user_pages(expr *n);
E 3
I 3
int int_printf(expr *n);
E 3
int int_purge_all_pages(expr *n);
E 5
I 3
int int_purge_user_pages(expr *n);
D 5
int int_regs_instr(expr *n);
int int_return_range(expr *n);
E 5
I 5
int int_purge_all_pages(expr *n);
E 5
int int_sprintf(expr *n);
D 5
int int_strcmp(expr *n);
int int_strncmp(expr *n);
int int_v2f(expr *n);
E 5
I 5
int int_printf(expr *n);
long int_v2f(expr *n);
long int_f2v(expr *n);
int int_find(expr *n);
int int_dis(expr *n);
int int_load(expr *n);
void builtin_init(void);

#endif /* __BUILTINS_H */
E 5
E 3
E 2
E 1
