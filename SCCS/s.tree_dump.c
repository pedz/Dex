h23074
s 00168/00109/00608
d D 1.8 02/03/14 16:12:35 pedz 8 7
e
s 00000/00000/00717
d D 1.7 00/09/18 17:57:03 pedz 7 6
c Checking before V5 conversion
e
s 00049/00003/00668
d D 1.6 00/05/29 15:09:09 pedz 6 5
c Just before IA64 conversion
e
s 00004/00000/00667
d D 1.5 00/02/08 17:36:49 pedz 5 4
e
s 00003/00001/00664
d D 1.4 98/10/23 12:26:14 pedz 4 3
e
s 00014/00001/00651
d D 1.3 95/04/25 10:10:31 pedz 3 2
e
s 00041/00000/00611
d D 1.2 95/02/09 21:36:08 pedz 2 1
e
s 00611/00000/00000
d D 1.1 95/02/01 10:57:40 pedz 1 0
c date and time created 95/02/01 10:57:40 by pedz
e
u
U
t
T
I 1
static char sccs_id[] = "%W%";

I 8
#include <stdio.h>
E 8
I 4
#include "dex.h"
E 4
#include "map.h"
#include "sym.h"
#include "tree.h"
#include "asgn_expr.h"
#include "base_expr.h"
I 2
#include "binary_expr.h"
#include "cast_expr.h"
#include "unary_expr.h"
E 2
#include "fcall.h"
I 2
#include "builtins.h"
I 8
#define DEBUG_BIT TREE_DUMP_C_BIT
E 8
E 2

enum args {
    E_ADDR, E_BOTH, E_DOUBLE, E_FCALL, E_FLOAT, E_INT, E_LEFT, E_LONG,
    E_NULL, E_QC, E_SCHAR, E_SHORT, E_ST, E_UCHAR, E_UINT, E_ULONG,
    E_USHORT,
};

struct tab {
    int (*t_func)();
    char *t_name;
    enum args t_args;
};

#define E(f) (int (*)())f, # f

static struct tab name_tab[] = {
    E(d__d), E_LEFT,
    E(d__f), E_LEFT,
    E(d__i), E_LEFT,
    E(d__l), E_LEFT,
I 8
    E(d__ll), E_LEFT,
E 8
    E(d__s), E_LEFT,
    E(d__sc), E_LEFT,
    E(d__uc), E_LEFT,
    E(d__ui), E_LEFT,
    E(d__ul), E_LEFT,
I 8
    E(d__ull), E_LEFT,
E 8
    E(d__us), E_LEFT,
    E(d_andand), E_BOTH,
    E(d_asgn), E_BOTH,
    E(d_comma), E_BOTH,
    E(d_dec), E_BOTH,
    E(d_divasgn), E_BOTH,
    E(d_divide), E_BOTH,
    E(d_eq), E_BOTH,
I 6
D 8
#ifdef Not_Used
E 6
    E(d_f2v), E_LEFT,
I 6
#endif
E 8
E 6
    E(d_fcall), E_FCALL,
    E(d_gaddr), E_ADDR,
    E(d_ge), E_BOTH,
    E(d_gt), E_BOTH,
    E(d_inc), E_BOTH,
    E(d_l2p), E_LEFT,
    E(d_laddr), E_ADDR,
    E(d_le), E_BOTH,
    E(d_leaf), E_DOUBLE,
I 2
    E(d_lnot), E_LEFT,
E 2
    E(d_lt), E_BOTH,
    E(d_minue), E_BOTH,
    E(d_minusasgn), E_BOTH,
    E(d_ne), E_BOTH,
    E(d_null), E_NULL,
    E(d_oror), E_BOTH,
    E(d_plus), E_BOTH,
    E(d_plusasgn), E_BOTH,
    E(d_qc), E_QC,
    E(d_times), E_BOTH,
    E(d_timesasgn), E_BOTH,
I 2
    E(d_umin), E_LEFT,
I 6
D 8
#ifdef Not_Used
E 6
E 2
    E(d_v2f), E_LEFT,
I 6
#endif
E 8
E 6
    E(f__d), E_LEFT,
    E(f__f), E_LEFT,
    E(f__i), E_LEFT,
    E(f__l), E_LEFT,
I 8
    E(f__ll), E_LEFT,
E 8
    E(f__s), E_LEFT,
    E(f__sc), E_LEFT,
    E(f__uc), E_LEFT,
    E(f__ui), E_LEFT,
    E(f__ul), E_LEFT,
I 8
    E(f__ull), E_LEFT,
E 8
    E(f__us), E_LEFT,
    E(f_andand), E_BOTH,
    E(f_asgn), E_BOTH,
    E(f_comma), E_BOTH,
    E(f_dec), E_BOTH,
    E(f_divasgn), E_BOTH,
    E(f_divide), E_BOTH,
    E(f_eq), E_BOTH,
I 6
D 8
#ifdef Not_Used
E 6
    E(f_f2v), E_LEFT,
I 6
#endif
E 8
E 6
    E(f_fcall), E_FCALL,
    E(f_gaddr), E_ADDR,
    E(f_ge), E_BOTH,
    E(f_gt), E_BOTH,
    E(f_inc), E_BOTH,
    E(f_l2p), E_LEFT,
    E(f_laddr), E_ADDR,
    E(f_le), E_BOTH,
    E(f_leaf), E_FLOAT,
I 2
    E(f_lnot), E_LEFT,
E 2
    E(f_lt), E_BOTH,
    E(f_minue), E_BOTH,
    E(f_minusasgn), E_BOTH,
    E(f_ne), E_BOTH,
    E(f_null), E_NULL,
    E(f_oror), E_BOTH,
    E(f_plus), E_BOTH,
    E(f_plusasgn), E_BOTH,
    E(f_qc), E_QC,
    E(f_times), E_BOTH,
    E(f_timesasgn), E_BOTH,
I 2
    E(f_umin), E_LEFT,
I 6
D 8
#ifdef Not_Used
E 6
E 2
    E(f_v2f), E_LEFT,
I 6
#endif
E 8
E 6
    E(i__d), E_LEFT,
    E(i__f), E_LEFT,
    E(i__i), E_LEFT,
    E(i__l), E_LEFT,
I 8
    E(i__ll), E_LEFT,
E 8
    E(i__s), E_LEFT,
    E(i__sc), E_LEFT,
    E(i__uc), E_LEFT,
    E(i__ui), E_LEFT,
    E(i__ul), E_LEFT,
I 8
    E(i__ull), E_LEFT,
E 8
    E(i__us), E_LEFT,
    E(i_and), E_BOTH,
    E(i_andand), E_BOTH,
    E(i_andasgn), E_BOTH,
    E(i_asgn), E_BOTH,
I 2
    E(i_bnot), E_LEFT,
E 2
    E(i_comma), E_BOTH,
    E(i_dec), E_BOTH,
    E(i_divasgn), E_BOTH,
    E(i_divide), E_BOTH,
    E(i_eq), E_BOTH,
I 6
D 8
#ifdef Not_Used
E 6
    E(i_f2v), E_LEFT,
I 6
#endif
E 8
E 6
    E(i_fcall), E_FCALL,
    E(i_gaddr), E_ADDR,
    E(i_ge), E_BOTH,
    E(i_gt), E_BOTH,
    E(i_inc), E_BOTH,
    E(i_l2p), E_LEFT,
    E(i_laddr), E_ADDR,
    E(i_le), E_BOTH,
    E(i_leaf), E_INT,
I 2
    E(i_lnot), E_LEFT,
E 2
    E(i_lsasgn), E_BOTH,
    E(i_lshift), E_BOTH,
    E(i_lt), E_BOTH,
    E(i_minue), E_BOTH,
    E(i_minusasgn), E_BOTH,
    E(i_mod), E_BOTH,
    E(i_modasgn), E_BOTH,
    E(i_ne), E_BOTH,
    E(i_null), E_NULL,
    E(i_or), E_BOTH,
    E(i_orasgn), E_BOTH,
    E(i_oror), E_BOTH,
    E(i_plus), E_BOTH,
    E(i_plusasgn), E_BOTH,
    E(i_qc), E_QC,
    E(i_rsasgn), E_BOTH,
    E(i_rshift), E_BOTH,
    E(i_times), E_BOTH,
    E(i_timesasgn), E_BOTH,
I 2
    E(i_umin), E_LEFT,
I 6
D 8
#ifdef Not_Used
E 6
E 2
    E(i_v2f), E_LEFT,
I 6
#endif
E 8
E 6
    E(i_xor), E_BOTH,
I 2
    E(int_dis), E_INT,
    E(int_find), E_INT,
I 3
    E(int_instr), E_INT,
E 3
    E(int_load), E_INT,
    E(int_printf), E_INT,
I 3
    E(int_purge_all_pages), E_INT,
    E(int_purge_user_pages), E_INT,
    E(int_regs_instr), E_INT,
I 4
    E(int_return_range), E_INT,
E 4
    E(int_sprintf), E_INT,
    E(int_strcat), E_INT,
    E(int_strchr), E_INT,
    E(int_strcmp), E_INT,
    E(int_strcpy), E_INT,
    E(int_strncat), E_INT,
    E(int_strncmp), E_INT,
    E(int_strncpy), E_INT,
    E(int_strrchr), E_INT,
I 6
D 8
#ifdef Not_Used
E 6
E 3
    E(int_v2f), E_INT,
I 6
#endif
E 8
E 6
E 2
    E(l__d), E_LEFT,
    E(l__f), E_LEFT,
    E(l__i), E_LEFT,
    E(l__l), E_LEFT,
I 8
    E(l__ll), E_LEFT,
E 8
    E(l__s), E_LEFT,
    E(l__sc), E_LEFT,
    E(l__uc), E_LEFT,
    E(l__ui), E_LEFT,
    E(l__ul), E_LEFT,
I 8
    E(l__ull), E_LEFT,
E 8
    E(l__us), E_LEFT,
    E(l_and), E_BOTH,
    E(l_andand), E_BOTH,
    E(l_andasgn), E_BOTH,
    E(l_asgn), E_BOTH,
I 2
    E(l_bnot), E_LEFT,
E 2
    E(l_comma), E_BOTH,
    E(l_dec), E_BOTH,
    E(l_divasgn), E_BOTH,
    E(l_divide), E_BOTH,
    E(l_eq), E_BOTH,
I 6
D 8
#ifdef Not_Used
E 6
    E(l_f2v), E_LEFT,
I 6
#endif
E 8
E 6
    E(l_fcall), E_FCALL,
    E(l_gaddr), E_ADDR,
    E(l_ge), E_BOTH,
    E(l_gt), E_BOTH,
    E(l_inc), E_BOTH,
    E(l_l2p), E_LEFT,
    E(l_laddr), E_ADDR,
    E(l_le), E_BOTH,
    E(l_leaf), E_LONG,
I 2
    E(l_lnot), E_LEFT,
E 2
    E(l_lsasgn), E_BOTH,
    E(l_lshift), E_BOTH,
    E(l_lt), E_BOTH,
    E(l_minue), E_BOTH,
    E(l_minusasgn), E_BOTH,
    E(l_mod), E_BOTH,
    E(l_modasgn), E_BOTH,
    E(l_ne), E_BOTH,
    E(l_null), E_NULL,
    E(l_or), E_BOTH,
    E(l_orasgn), E_BOTH,
    E(l_oror), E_BOTH,
    E(l_plus), E_BOTH,
    E(l_plusasgn), E_BOTH,
    E(l_qc), E_QC,
    E(l_rsasgn), E_BOTH,
    E(l_rshift), E_BOTH,
    E(l_times), E_BOTH,
    E(l_timesasgn), E_BOTH,
I 2
    E(l_umin), E_LEFT,
I 6
D 8
#ifdef Not_Used
E 6
E 2
    E(l_v2f), E_LEFT,
I 6
#endif
E 8
E 6
    E(l_xor), E_BOTH,
I 8
    E(ll__d), E_LEFT,
    E(ll__f), E_LEFT,
    E(ll__i), E_LEFT,
    E(ll__l), E_LEFT,
    E(ll__ll), E_LEFT,
    E(ll__s), E_LEFT,
    E(ll__sc), E_LEFT,
    E(ll__uc), E_LEFT,
    E(ll__ui), E_LEFT,
    E(ll__ul), E_LEFT,
    E(ll__ull), E_LEFT,
    E(ll__us), E_LEFT,
    E(ll_and), E_BOTH,
    E(ll_andand), E_BOTH,
    E(ll_andasgn), E_BOTH,
    E(ll_asgn), E_BOTH,
    E(ll_bnot), E_LEFT,
    E(ll_comma), E_BOTH,
    E(ll_dec), E_BOTH,
    E(ll_divasgn), E_BOTH,
    E(ll_divide), E_BOTH,
    E(ll_eq), E_BOTH,
    E(ll_fcall), E_FCALL,
    E(ll_gaddr), E_ADDR,
    E(ll_ge), E_BOTH,
    E(ll_gt), E_BOTH,
    E(ll_inc), E_BOTH,
    E(ll_l2p), E_LEFT,
    E(ll_laddr), E_ADDR,
    E(ll_le), E_BOTH,
    E(ll_leaf), E_LONG,
    E(ll_lnot), E_LEFT,
    E(ll_lsasgn), E_BOTH,
    E(ll_lshift), E_BOTH,
    E(ll_lt), E_BOTH,
    E(ll_minue), E_BOTH,
    E(ll_minusasgn), E_BOTH,
    E(ll_mod), E_BOTH,
    E(ll_modasgn), E_BOTH,
    E(ll_ne), E_BOTH,
    E(ll_null), E_NULL,
    E(ll_or), E_BOTH,
    E(ll_orasgn), E_BOTH,
    E(ll_oror), E_BOTH,
    E(ll_plus), E_BOTH,
    E(ll_plusasgn), E_BOTH,
    E(ll_qc), E_QC,
    E(ll_rsasgn), E_BOTH,
    E(ll_rshift), E_BOTH,
    E(ll_times), E_BOTH,
    E(ll_timesasgn), E_BOTH,
    E(ll_umin), E_LEFT,
    E(ll_xor), E_BOTH,
E 8
    E(s__d), E_LEFT,
    E(s__f), E_LEFT,
    E(s__i), E_LEFT,
    E(s__l), E_LEFT,
I 8
    E(s__ll), E_LEFT,
E 8
    E(s__s), E_LEFT,
    E(s__sc), E_LEFT,
    E(s__uc), E_LEFT,
    E(s__ui), E_LEFT,
    E(s__ul), E_LEFT,
I 8
    E(s__ull), E_LEFT,
E 8
    E(s__us), E_LEFT,
    E(s_and), E_BOTH,
    E(s_andand), E_BOTH,
    E(s_andasgn), E_BOTH,
    E(s_asgn), E_BOTH,
I 2
    E(s_bnot), E_LEFT,
E 2
    E(s_comma), E_BOTH,
    E(s_dec), E_BOTH,
    E(s_divasgn), E_BOTH,
    E(s_divide), E_BOTH,
    E(s_eq), E_BOTH,
I 6
D 8
#ifdef Not_Used
E 6
    E(s_f2v), E_LEFT,
I 6
#endif
E 8
E 6
    E(s_fcall), E_FCALL,
    E(s_gaddr), E_ADDR,
    E(s_ge), E_BOTH,
    E(s_gt), E_BOTH,
    E(s_inc), E_BOTH,
    E(s_l2p), E_LEFT,
    E(s_laddr), E_ADDR,
    E(s_le), E_BOTH,
    E(s_leaf), E_SHORT,
I 2
    E(s_lnot), E_LEFT,
E 2
    E(s_lsasgn), E_BOTH,
    E(s_lshift), E_BOTH,
    E(s_lt), E_BOTH,
    E(s_minue), E_BOTH,
    E(s_minusasgn), E_BOTH,
    E(s_mod), E_BOTH,
    E(s_modasgn), E_BOTH,
    E(s_ne), E_BOTH,
    E(s_null), E_NULL,
    E(s_or), E_BOTH,
    E(s_orasgn), E_BOTH,
    E(s_oror), E_BOTH,
    E(s_plus), E_BOTH,
    E(s_plusasgn), E_BOTH,
    E(s_qc), E_QC,
    E(s_rsasgn), E_BOTH,
    E(s_rshift), E_BOTH,
    E(s_times), E_BOTH,
    E(s_timesasgn), E_BOTH,
I 2
    E(s_umin), E_LEFT,
I 6
D 8
#ifdef Not_Used
E 6
E 2
    E(s_v2f), E_LEFT,
I 6
#endif
E 8
E 6
    E(s_xor), E_BOTH,
    E(sc__d), E_LEFT,
    E(sc__f), E_LEFT,
    E(sc__i), E_LEFT,
    E(sc__l), E_LEFT,
I 8
    E(sc__ll), E_LEFT,
E 8
    E(sc__s), E_LEFT,
    E(sc__sc), E_LEFT,
    E(sc__uc), E_LEFT,
    E(sc__ui), E_LEFT,
    E(sc__ul), E_LEFT,
I 8
    E(sc__ull), E_LEFT,
E 8
    E(sc__us), E_LEFT,
    E(sc_and), E_BOTH,
    E(sc_andand), E_BOTH,
    E(sc_andasgn), E_BOTH,
    E(sc_asgn), E_BOTH,
I 2
    E(sc_bnot), E_LEFT,
E 2
    E(sc_comma), E_BOTH,
    E(sc_dec), E_BOTH,
    E(sc_divasgn), E_BOTH,
    E(sc_divide), E_BOTH,
    E(sc_eq), E_BOTH,
I 6
D 8
#ifdef Not_Used
E 6
    E(sc_f2v), E_LEFT,
I 6
#endif
E 8
E 6
    E(sc_fcall), E_FCALL,
    E(sc_gaddr), E_ADDR,
    E(sc_ge), E_BOTH,
    E(sc_gt), E_BOTH,
    E(sc_inc), E_BOTH,
    E(sc_l2p), E_LEFT,
    E(sc_laddr), E_ADDR,
    E(sc_le), E_BOTH,
    E(sc_leaf), E_SCHAR,
I 2
    E(sc_lnot), E_LEFT,
E 2
    E(sc_lsasgn), E_BOTH,
    E(sc_lshift), E_BOTH,
    E(sc_lt), E_BOTH,
    E(sc_minue), E_BOTH,
    E(sc_minusasgn), E_BOTH,
    E(sc_mod), E_BOTH,
    E(sc_modasgn), E_BOTH,
    E(sc_ne), E_BOTH,
    E(sc_null), E_NULL,
    E(sc_or), E_BOTH,
    E(sc_orasgn), E_BOTH,
    E(sc_oror), E_BOTH,
    E(sc_plus), E_BOTH,
    E(sc_plusasgn), E_BOTH,
    E(sc_qc), E_QC,
    E(sc_rsasgn), E_BOTH,
    E(sc_rshift), E_BOTH,
    E(sc_times), E_BOTH,
    E(sc_timesasgn), E_BOTH,
I 2
    E(sc_umin), E_LEFT,
I 6
D 8
#ifdef Not_Used
E 6
E 2
    E(sc_v2f), E_LEFT,
I 6
#endif
E 8
E 6
    E(sc_xor), E_BOTH,
    E(st_asgn), E_BOTH,
    E(st_comma), E_BOTH,
I 6
D 8
#ifdef Not_Used
E 6
    E(st_f2v), E_LEFT,
I 6
#endif
E 8
E 6
    E(st_fcall), E_FCALL,
    E(st_gaddr), E_ADDR,
    E(st_l2p), E_LEFT,
    E(st_laddr), E_ADDR,
    E(st_leaf), E_ST,
    E(st_null), E_NULL,
    E(st_qc), E_QC,
I 6
D 8
#ifdef Not_Used
E 6
    E(st_v2f), E_LEFT,
I 6
#endif
E 8
E 6
    E(uc__d), E_LEFT,
    E(uc__f), E_LEFT,
    E(uc__i), E_LEFT,
    E(uc__l), E_LEFT,
I 8
    E(uc__ll), E_LEFT,
E 8
    E(uc__s), E_LEFT,
    E(uc__sc), E_LEFT,
    E(uc__uc), E_LEFT,
    E(uc__ui), E_LEFT,
    E(uc__ul), E_LEFT,
I 8
    E(uc__ull), E_LEFT,
E 8
    E(uc__us), E_LEFT,
    E(uc_and), E_BOTH,
    E(uc_andand), E_BOTH,
    E(uc_andasgn), E_BOTH,
    E(uc_asgn), E_BOTH,
I 2
    E(uc_bnot), E_LEFT,
E 2
    E(uc_comma), E_BOTH,
    E(uc_dec), E_BOTH,
    E(uc_divasgn), E_BOTH,
    E(uc_divide), E_BOTH,
    E(uc_eq), E_BOTH,
I 6
D 8
#ifdef Not_Used
E 6
    E(uc_f2v), E_LEFT,
I 6
#endif
E 8
E 6
    E(uc_fcall), E_FCALL,
    E(uc_gaddr), E_ADDR,
    E(uc_ge), E_BOTH,
    E(uc_gt), E_BOTH,
    E(uc_inc), E_BOTH,
    E(uc_l2p), E_LEFT,
    E(uc_laddr), E_ADDR,
    E(uc_le), E_BOTH,
    E(uc_leaf), E_UCHAR,
I 2
    E(uc_lnot), E_LEFT,
E 2
    E(uc_lsasgn), E_BOTH,
    E(uc_lshift), E_BOTH,
    E(uc_lt), E_BOTH,
    E(uc_minue), E_BOTH,
    E(uc_minusasgn), E_BOTH,
    E(uc_mod), E_BOTH,
    E(uc_modasgn), E_BOTH,
    E(uc_ne), E_BOTH,
    E(uc_null), E_NULL,
    E(uc_or), E_BOTH,
    E(uc_orasgn), E_BOTH,
    E(uc_oror), E_BOTH,
    E(uc_plus), E_BOTH,
    E(uc_plusasgn), E_BOTH,
    E(uc_qc), E_QC,
    E(uc_rsasgn), E_BOTH,
    E(uc_rshift), E_BOTH,
    E(uc_times), E_BOTH,
    E(uc_timesasgn), E_BOTH,
I 2
    E(uc_umin), E_LEFT,
I 6
D 8
#ifdef Not_Used
E 6
E 2
    E(uc_v2f), E_LEFT,
I 6
#endif
E 8
E 6
    E(uc_xor), E_BOTH,
    E(ui__d), E_LEFT,
    E(ui__f), E_LEFT,
    E(ui__i), E_LEFT,
    E(ui__l), E_LEFT,
I 8
    E(ui__ll), E_LEFT,
E 8
    E(ui__s), E_LEFT,
    E(ui__sc), E_LEFT,
    E(ui__uc), E_LEFT,
    E(ui__ui), E_LEFT,
    E(ui__ul), E_LEFT,
I 8
    E(ui__ull), E_LEFT,
E 8
    E(ui__us), E_LEFT,
    E(ui_and), E_BOTH,
    E(ui_andand), E_BOTH,
    E(ui_andasgn), E_BOTH,
    E(ui_asgn), E_BOTH,
I 2
    E(ui_bnot), E_LEFT,
E 2
    E(ui_comma), E_BOTH,
    E(ui_dec), E_BOTH,
    E(ui_divasgn), E_BOTH,
    E(ui_divide), E_BOTH,
    E(ui_eq), E_BOTH,
I 6
D 8
#ifdef Not_Used
E 6
    E(ui_f2v), E_LEFT,
I 6
#endif
E 8
E 6
    E(ui_fcall), E_FCALL,
    E(ui_gaddr), E_ADDR,
    E(ui_ge), E_BOTH,
    E(ui_gt), E_BOTH,
    E(ui_inc), E_BOTH,
    E(ui_l2p), E_LEFT,
    E(ui_laddr), E_ADDR,
    E(ui_le), E_BOTH,
    E(ui_leaf), E_UINT,
I 2
    E(ui_lnot), E_LEFT,
E 2
    E(ui_lsasgn), E_BOTH,
    E(ui_lshift), E_BOTH,
    E(ui_lt), E_BOTH,
    E(ui_minue), E_BOTH,
    E(ui_minusasgn), E_BOTH,
    E(ui_mod), E_BOTH,
    E(ui_modasgn), E_BOTH,
    E(ui_ne), E_BOTH,
    E(ui_null), E_NULL,
    E(ui_or), E_BOTH,
    E(ui_orasgn), E_BOTH,
    E(ui_oror), E_BOTH,
    E(ui_plus), E_BOTH,
    E(ui_plusasgn), E_BOTH,
    E(ui_qc), E_QC,
    E(ui_rsasgn), E_BOTH,
    E(ui_rshift), E_BOTH,
    E(ui_times), E_BOTH,
    E(ui_timesasgn), E_BOTH,
I 2
    E(ui_umin), E_LEFT,
I 6
D 8
#ifdef Not_Used
E 6
E 2
    E(ui_v2f), E_LEFT,
I 6
#endif
E 8
E 6
    E(ui_xor), E_BOTH,
    E(ul__d), E_LEFT,
    E(ul__f), E_LEFT,
    E(ul__i), E_LEFT,
    E(ul__l), E_LEFT,
I 8
    E(ul__ll), E_LEFT,
E 8
    E(ul__s), E_LEFT,
    E(ul__sc), E_LEFT,
    E(ul__uc), E_LEFT,
    E(ul__ui), E_LEFT,
    E(ul__ul), E_LEFT,
I 8
    E(ul__ull), E_LEFT,
E 8
    E(ul__us), E_LEFT,
    E(ul_and), E_BOTH,
    E(ul_andand), E_BOTH,
    E(ul_andasgn), E_BOTH,
    E(ul_asgn), E_BOTH,
I 2
    E(ul_bnot), E_LEFT,
E 2
    E(ul_comma), E_BOTH,
    E(ul_dec), E_BOTH,
    E(ul_divasgn), E_BOTH,
    E(ul_divide), E_BOTH,
    E(ul_eq), E_BOTH,
I 6
D 8
#ifdef Not_Used
E 6
    E(ul_f2v), E_LEFT,
I 6
#endif
E 8
E 6
    E(ul_fcall), E_FCALL,
    E(ul_gaddr), E_ADDR,
    E(ul_ge), E_BOTH,
    E(ul_gt), E_BOTH,
    E(ul_inc), E_BOTH,
    E(ul_l2p), E_LEFT,
    E(ul_laddr), E_ADDR,
    E(ul_le), E_BOTH,
    E(ul_leaf), E_ULONG,
I 2
    E(ul_lnot), E_LEFT,
E 2
    E(ul_lsasgn), E_BOTH,
    E(ul_lshift), E_BOTH,
    E(ul_lt), E_BOTH,
    E(ul_minue), E_BOTH,
    E(ul_minusasgn), E_BOTH,
    E(ul_mod), E_BOTH,
    E(ul_modasgn), E_BOTH,
    E(ul_ne), E_BOTH,
    E(ul_null), E_NULL,
    E(ul_or), E_BOTH,
    E(ul_orasgn), E_BOTH,
    E(ul_oror), E_BOTH,
    E(ul_plus), E_BOTH,
    E(ul_plusasgn), E_BOTH,
    E(ul_qc), E_QC,
    E(ul_rsasgn), E_BOTH,
    E(ul_rshift), E_BOTH,
    E(ul_times), E_BOTH,
    E(ul_timesasgn), E_BOTH,
I 2
    E(ul_umin), E_LEFT,
I 6
D 8
#ifdef Not_Used
E 6
E 2
    E(ul_v2f), E_LEFT,
I 6
#endif
E 8
E 6
    E(ul_xor), E_BOTH,
I 8
    E(ull__d), E_LEFT,
    E(ull__f), E_LEFT,
    E(ull__i), E_LEFT,
    E(ull__l), E_LEFT,
    E(ull__ll), E_LEFT,
    E(ull__s), E_LEFT,
    E(ull__sc), E_LEFT,
    E(ull__uc), E_LEFT,
    E(ull__ui), E_LEFT,
    E(ull__ul), E_LEFT,
    E(ull__ull), E_LEFT,
    E(ull__us), E_LEFT,
    E(ull_and), E_BOTH,
    E(ull_andand), E_BOTH,
    E(ull_andasgn), E_BOTH,
    E(ull_asgn), E_BOTH,
    E(ull_bnot), E_LEFT,
    E(ull_comma), E_BOTH,
    E(ull_dec), E_BOTH,
    E(ull_divasgn), E_BOTH,
    E(ull_divide), E_BOTH,
    E(ull_eq), E_BOTH,
    E(ull_fcall), E_FCALL,
    E(ull_gaddr), E_ADDR,
    E(ull_ge), E_BOTH,
    E(ull_gt), E_BOTH,
    E(ull_inc), E_BOTH,
    E(ull_l2p), E_LEFT,
    E(ull_laddr), E_ADDR,
    E(ull_le), E_BOTH,
    E(ull_leaf), E_ULONG,
    E(ull_lnot), E_LEFT,
    E(ull_lsasgn), E_BOTH,
    E(ull_lshift), E_BOTH,
    E(ull_lt), E_BOTH,
    E(ull_minue), E_BOTH,
    E(ull_minusasgn), E_BOTH,
    E(ull_mod), E_BOTH,
    E(ull_modasgn), E_BOTH,
    E(ull_ne), E_BOTH,
    E(ull_null), E_NULL,
    E(ull_or), E_BOTH,
    E(ull_orasgn), E_BOTH,
    E(ull_oror), E_BOTH,
    E(ull_plus), E_BOTH,
    E(ull_plusasgn), E_BOTH,
    E(ull_qc), E_QC,
    E(ull_rsasgn), E_BOTH,
    E(ull_rshift), E_BOTH,
    E(ull_times), E_BOTH,
    E(ull_timesasgn), E_BOTH,
    E(ull_umin), E_LEFT,
    E(ull_xor), E_BOTH,
E 8
    E(us__d), E_LEFT,
    E(us__f), E_LEFT,
    E(us__i), E_LEFT,
    E(us__l), E_LEFT,
I 8
    E(us__ll), E_LEFT,
E 8
    E(us__s), E_LEFT,
    E(us__sc), E_LEFT,
    E(us__uc), E_LEFT,
    E(us__ui), E_LEFT,
    E(us__ul), E_LEFT,
I 8
    E(us__ull), E_LEFT,
E 8
    E(us__us), E_LEFT,
    E(us_and), E_BOTH,
    E(us_andand), E_BOTH,
    E(us_andasgn), E_BOTH,
    E(us_asgn), E_BOTH,
I 2
    E(us_bnot), E_LEFT,
E 2
    E(us_comma), E_BOTH,
    E(us_dec), E_BOTH,
    E(us_divasgn), E_BOTH,
    E(us_divide), E_BOTH,
    E(us_eq), E_BOTH,
I 6
D 8
#ifdef Not_Used
E 6
    E(us_f2v), E_LEFT,
I 6
#endif
E 8
E 6
    E(us_fcall), E_FCALL,
    E(us_gaddr), E_ADDR,
    E(us_ge), E_BOTH,
    E(us_gt), E_BOTH,
    E(us_inc), E_BOTH,
    E(us_l2p), E_LEFT,
    E(us_laddr), E_ADDR,
    E(us_le), E_BOTH,
    E(us_leaf), E_USHORT,
I 2
    E(us_lnot), E_LEFT,
E 2
    E(us_lsasgn), E_BOTH,
    E(us_lshift), E_BOTH,
    E(us_lt), E_BOTH,
    E(us_minue), E_BOTH,
    E(us_minusasgn), E_BOTH,
    E(us_mod), E_BOTH,
    E(us_modasgn), E_BOTH,
    E(us_ne), E_BOTH,
    E(us_null), E_NULL,
    E(us_or), E_BOTH,
    E(us_orasgn), E_BOTH,
    E(us_oror), E_BOTH,
    E(us_plus), E_BOTH,
    E(us_plusasgn), E_BOTH,
    E(us_qc), E_QC,
    E(us_rsasgn), E_BOTH,
    E(us_rshift), E_BOTH,
    E(us_times), E_BOTH,
    E(us_timesasgn), E_BOTH,
I 2
    E(us_umin), E_LEFT,
I 6
D 8
#ifdef Not_Used
E 6
E 2
    E(us_v2f), E_LEFT,
I 6
#endif
E 8
E 6
    E(us_xor), E_BOTH,
};
#define TAB_SIZE (sizeof(name_tab) / sizeof(name_tab[0]))

D 8
static void internal_tree_dump(expr *e);
static dump_fcall(expr *e);
E 8
I 8
static void internal_tree_dump(FILE *f, expr *e);
static dump_fcall(FILE *f, expr *e);
E 8

D 8
static dump_fcall(expr *e)
E 8
I 8
static dump_fcall(FILE *f, expr *e)
E 8
{
    cnode_list *cl;

D 8
    internal_tree_dump(e->e_call);
E 8
I 8
    internal_tree_dump(f, e->e_call);
E 8
    for (cl = e->e_args; cl; cl = cl->cl_next) {
D 8
	printf(" %s", type_2_string[cl->cl_cnode.c_base]);
	internal_tree_dump(cl->cl_cnode.c_expr);
E 8
I 8
	fprintf(f, " %s", type_2_string[cl->cl_cnode.c_base]);
	internal_tree_dump(f, cl->cl_cnode.c_expr);
E 8
    }
}

D 8
static void internal_tree_dump(expr *e)
E 8
I 8
static void internal_tree_dump(FILE *f, expr *e)
E 8
{
    struct tab *t, *t_end;

    /* Slow linear search -- gack */
    for (t_end = (t = name_tab) + TAB_SIZE;
	 t < t_end && t->t_func != e->e_func.i;
	 ++t);
    if (t == t_end) {			/* not found */
D 4
	printf("expr at 0x%08 not found\n", e);
E 4
I 4
D 8
	printf("expr at 0x%08x not found\n", e);
E 8
I 8
	fprintf(f, "expr at 0x%s not found\n", P(e));
E 8
E 4
	return;
    }

D 8
    printf("(%s ", t->t_name);
E 8
I 8
    fprintf(f, "(%s ", t->t_name);
E 8
    switch (t->t_args) {
    case E_ADDR:
D 8
	printf(" 0x%08x", e->e_addr);
E 8
I 8
	fprintf(f, " 0x%s", P(e->e_addr));
E 8
	break;
    case E_BOTH:
I 2
	if (e->e_bsize)
D 8
	    printf("size:%d offset:%d ", e->e_bsize, e->e_boffset);
E 8
I 8
	    fprintf(f, "size:%d offset:%d ", e->e_bsize, e->e_boffset);
E 8
I 5
D 6
	else if (e->e_size == sizeof(ularge_t))
E 6
I 6
	else if (e->e_size > sizeof(long))
E 6
D 8
	    printf("ularge_t ");
E 5
E 2
	internal_tree_dump(e->e_left);
	printf(" ");
	internal_tree_dump(e->e_right);
E 8
I 8
	    fprintf(f, "ularge_t ");
	internal_tree_dump(f, e->e_left);
	fprintf(f, " ");
	internal_tree_dump(f, e->e_right);
E 8
	break;
    case E_DOUBLE:
D 8
	printf(" %f", e->e_d);
E 8
I 8
	fprintf(f, " %f", e->e_d);
E 8
	break;
    case E_FCALL:
D 8
	dump_fcall(e);
E 8
I 8
	dump_fcall(f, e);
E 8
	break;
    case E_FLOAT:
D 8
	printf(" %f", e->e_f);
E 8
I 8
	fprintf(f, " %f", e->e_f);
E 8
	break;
    case E_INT:
D 8
	printf(" %d", e->e_i);
E 8
I 8
	fprintf(f, " %d", e->e_i);
E 8
	break;
    case E_LEFT:
I 2
	if (e->e_bsize)
D 8
	    printf("size:%d offset:%d ", e->e_bsize, e->e_boffset);
E 8
I 8
	    fprintf(f, "size:%d offset:%d ", e->e_bsize, e->e_boffset);
E 8
I 5
D 6
	else if (e->e_size == sizeof(ularge_t))
	    printf("ularge_t ");
E 6
I 6
	else if (e->e_size > sizeof(long))
D 8
	    printf("Ularge_t ");
E 6
E 5
E 2
	internal_tree_dump(e->e_left);
E 8
I 8
	    fprintf(f, "Ularge_t ");
	internal_tree_dump(f, e->e_left);
E 8
	break;
    case E_LONG:
D 3
	printf(" %d", e->e_f);
E 3
I 3
D 8
	printf(" %d", e->e_l);
E 8
I 8
	fprintf(f, " %d", e->e_l);
E 8
E 3
	break;
    case E_NULL:
	break;
    case E_QC:
D 8
	internal_tree_dump(e->e_left);
	printf(" ");
	internal_tree_dump(e->e_right->e_left);
	printf(" ");
	internal_tree_dump(e->e_right->e_right);
E 8
I 8
	internal_tree_dump(f, e->e_left);
	fprintf(f, " ");
	internal_tree_dump(f, e->e_right->e_left);
	fprintf(f, " ");
	internal_tree_dump(f, e->e_right->e_right);
E 8
	break;
    case E_SCHAR:
D 8
	printf(" %d", e->e_c);
E 8
I 8
	fprintf(f, " %d", e->e_sc);
E 8
	break;
    case E_SHORT:
D 8
	printf(" %d", e->e_s);
E 8
I 8
	fprintf(f, " %d", e->e_s);
E 8
	break;
    case E_ST:
D 8
	printf(" 0x%08x", e->e_st);
E 8
I 8
	fprintf(f, " 0x%s", P(e->e_st));
E 8
	break;
    case E_UCHAR:
D 8
	printf(" %u", e->e_uc);
E 8
I 8
	fprintf(f, " %u", e->e_uc);
E 8
	break;
    case E_UINT:
D 8
	printf(" %u", e->e_ui);
E 8
I 8
	fprintf(f, " %u", e->e_ui);
E 8
	break;
    case E_ULONG:
D 8
	printf(" %u", e->e_ul);
E 8
I 8
	fprintf(f, " %u", e->e_ul);
E 8
	break;
    case E_USHORT:
D 8
	printf(" %u", e->e_us);
E 8
I 8
	fprintf(f, " %u", e->e_us);
E 8
	break;
    }
D 8
    printf(")");
E 8
I 8
    fprintf(f, ")");
E 8
}

D 8
void tree_dump(expr *e)
E 8
I 8
void tree_dump(FILE *f, expr *e)
E 8
{
D 8
    printf("expr 0x%08x:", e);
    internal_tree_dump(e);
    printf("\n");
E 8
I 8
    fprintf(f, "expr 0x%s:", P(e));
    internal_tree_dump(f, e);
    fprintf(f, "\n");
E 8
}
E 1
