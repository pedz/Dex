/* @(#)cast_expr.h	1.3 */

#ifndef __CAST_EXPR_H
#define __CAST_EXPR_H

signed char sc__sc(expr *n);
signed char sc__uc(expr *n);
signed char sc__i(expr *n);
signed char sc__ui(expr *n);
signed char sc__s(expr *n);
signed char sc__us(expr *n);
signed char sc__l(expr *n);
signed char sc__ul(expr *n);
signed char sc__ll(expr *n);
signed char sc__ull(expr *n);
signed char sc__f(expr *n);
signed char sc__d(expr *n);

unsigned char uc__sc(expr *n);
unsigned char uc__uc(expr *n);
unsigned char uc__i(expr *n);
unsigned char uc__ui(expr *n);
unsigned char uc__s(expr *n);
unsigned char uc__us(expr *n);
unsigned char uc__l(expr *n);
unsigned char uc__ul(expr *n);
unsigned char uc__ll(expr *n);
unsigned char uc__ull(expr *n);
unsigned char uc__f(expr *n);
unsigned char uc__d(expr *n);

int i__sc(expr *n);
int i__uc(expr *n);
int i__i(expr *n);
int i__ui(expr *n);
int i__s(expr *n);
int i__us(expr *n);
int i__l(expr *n);
int i__ul(expr *n);
int i__ll(expr *n);
int i__ull(expr *n);
int i__f(expr *n);
int i__d(expr *n);

unsigned int ui__sc(expr *n);
unsigned int ui__uc(expr *n);
unsigned int ui__i(expr *n);
unsigned int ui__ui(expr *n);
unsigned int ui__s(expr *n);
unsigned int ui__us(expr *n);
unsigned int ui__l(expr *n);
unsigned int ui__ul(expr *n);
unsigned int ui__ll(expr *n);
unsigned int ui__ull(expr *n);
unsigned int ui__f(expr *n);
unsigned int ui__d(expr *n);

short s__sc(expr *n);
short s__uc(expr *n);
short s__i(expr *n);
short s__ui(expr *n);
short s__s(expr *n);
short s__us(expr *n);
short s__l(expr *n);
short s__ul(expr *n);
short s__ll(expr *n);
short s__ull(expr *n);
short s__f(expr *n);
short s__d(expr *n);

unsigned short us__sc(expr *n);
unsigned short us__uc(expr *n);
unsigned short us__i(expr *n);
unsigned short us__ui(expr *n);
unsigned short us__s(expr *n);
unsigned short us__us(expr *n);
unsigned short us__l(expr *n);
unsigned short us__ul(expr *n);
unsigned short us__ll(expr *n);
unsigned short us__ull(expr *n);
unsigned short us__f(expr *n);
unsigned short us__d(expr *n);

long l__sc(expr *n);
long l__uc(expr *n);
long l__i(expr *n);
long l__ui(expr *n);
long l__s(expr *n);
long l__us(expr *n);
long l__l(expr *n);
long l__ul(expr *n);
long l__ll(expr *n);
long l__ull(expr *n);
long l__f(expr *n);
long l__d(expr *n);

unsigned long ul__sc(expr *n);
unsigned long ul__uc(expr *n);
unsigned long ul__i(expr *n);
unsigned long ul__ui(expr *n);
unsigned long ul__s(expr *n);
unsigned long ul__us(expr *n);
unsigned long ul__l(expr *n);
unsigned long ul__ul(expr *n);
unsigned long ul__ll(expr *n);
unsigned long ul__ull(expr *n);
unsigned long ul__f(expr *n);
unsigned long ul__d(expr *n);

long long ll__sc(expr *n);
long long ll__uc(expr *n);
long long ll__i(expr *n);
long long ll__ui(expr *n);
long long ll__s(expr *n);
long long ll__us(expr *n);
long long ll__l(expr *n);
long long ll__ul(expr *n);
long long ll__ll(expr *n);
long long ll__ull(expr *n);
long long ll__f(expr *n);
long long ll__d(expr *n);

unsigned long long ull__sc(expr *n);
unsigned long long ull__uc(expr *n);
unsigned long long ull__i(expr *n);
unsigned long long ull__ui(expr *n);
unsigned long long ull__s(expr *n);
unsigned long long ull__us(expr *n);
unsigned long long ull__l(expr *n);
unsigned long long ull__ul(expr *n);
unsigned long long ull__ll(expr *n);
unsigned long long ull__ull(expr *n);
unsigned long long ull__f(expr *n);
unsigned long long ull__d(expr *n);

float f__sc(expr *n);
float f__uc(expr *n);
float f__i(expr *n);
float f__ui(expr *n);
float f__s(expr *n);
float f__us(expr *n);
float f__l(expr *n);
float f__ul(expr *n);
float f__ll(expr *n);
float f__ull(expr *n);
float f__f(expr *n);
float f__d(expr *n);

double d__sc(expr *n);
double d__uc(expr *n);
double d__i(expr *n);
double d__ui(expr *n);
double d__s(expr *n);
double d__us(expr *n);
double d__l(expr *n);
double d__ul(expr *n);
double d__ll(expr *n);
double d__ull(expr *n);
double d__f(expr *n);
double d__d(expr *n);

#endif /* __CAST_EXPR_H */
