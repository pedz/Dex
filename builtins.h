
/* @(#)builtins.h	1.2 */

int int_regs_instr(expr *n);
char *int_instr(expr *n);
char *int_strcpy(expr *n);
char *int_strncpy(expr *n);
char *int_strcat(expr *n);
char *int_strncat(expr *n);
int int_strcmp(expr *n);
int int_strncmp(expr *n);
char *int_strchr(expr *n);
char *int_strrchr(expr *n);
int int_sprintf(expr *n);
int int_printf(expr *n);
int int_v2f(expr *n);
int int_find(expr *n);
int int_dis(expr *n);
int int_load(expr *n);
int int_purge_user_pages(expr *n);
int int_purge_all_pages(expr *n);
