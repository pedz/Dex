
/* @(#)fcall.h	1.3 */

extern long frame_ptr;
extern long stack_ptr;

void alloc_stack(int n);
signed char sc_fcall(expr *n);
unsigned char uc_fcall(expr *n);
int i_fcall(expr *n);
unsigned int ui_fcall(expr *n);
short s_fcall(expr *n);
unsigned short us_fcall(expr *n);
long l_fcall(expr *n);
unsigned long ul_fcall(expr *n);
float f_fcall(expr *n);
double d_fcall(expr *n);
st st_fcall(expr *n);
