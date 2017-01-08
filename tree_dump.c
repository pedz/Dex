static char sccs_id[] = "@(#)tree_dump.c	1.8";

#include <stdio.h>
#include "dex.h"
#include "map.h"
#include "sym.h"
#include "tree.h"
#include "asgn_expr.h"
#include "base_expr.h"
#include "binary_expr.h"
#include "cast_expr.h"
#include "unary_expr.h"
#include "fcall.h"
#define DEBUG_BIT TREE_DUMP_C_BIT

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
    E(d__ll), E_LEFT,
    E(d__s), E_LEFT,
    E(d__sc), E_LEFT,
    E(d__uc), E_LEFT,
    E(d__ui), E_LEFT,
    E(d__ul), E_LEFT,
    E(d__ull), E_LEFT,
    E(d__us), E_LEFT,
    E(d_andand), E_BOTH,
    E(d_asgn), E_BOTH,
    E(d_comma), E_BOTH,
    E(d_dec), E_BOTH,
    E(d_divasgn), E_BOTH,
    E(d_divide), E_BOTH,
    E(d_eq), E_BOTH,
    E(d_fcall), E_FCALL,
    E(d_gaddr), E_ADDR,
    E(d_ge), E_BOTH,
    E(d_gt), E_BOTH,
    E(d_inc), E_BOTH,
    E(d_l2p), E_LEFT,
    E(d_laddr), E_ADDR,
    E(d_le), E_BOTH,
    E(d_leaf), E_DOUBLE,
    E(d_lnot), E_LEFT,
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
    E(d_umin), E_LEFT,
    E(f__d), E_LEFT,
    E(f__f), E_LEFT,
    E(f__i), E_LEFT,
    E(f__l), E_LEFT,
    E(f__ll), E_LEFT,
    E(f__s), E_LEFT,
    E(f__sc), E_LEFT,
    E(f__uc), E_LEFT,
    E(f__ui), E_LEFT,
    E(f__ul), E_LEFT,
    E(f__ull), E_LEFT,
    E(f__us), E_LEFT,
    E(f_andand), E_BOTH,
    E(f_asgn), E_BOTH,
    E(f_comma), E_BOTH,
    E(f_dec), E_BOTH,
    E(f_divasgn), E_BOTH,
    E(f_divide), E_BOTH,
    E(f_eq), E_BOTH,
    E(f_fcall), E_FCALL,
    E(f_gaddr), E_ADDR,
    E(f_ge), E_BOTH,
    E(f_gt), E_BOTH,
    E(f_inc), E_BOTH,
    E(f_l2p), E_LEFT,
    E(f_laddr), E_ADDR,
    E(f_le), E_BOTH,
    E(f_leaf), E_FLOAT,
    E(f_lnot), E_LEFT,
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
    E(f_umin), E_LEFT,
    E(i__d), E_LEFT,
    E(i__f), E_LEFT,
    E(i__i), E_LEFT,
    E(i__l), E_LEFT,
    E(i__ll), E_LEFT,
    E(i__s), E_LEFT,
    E(i__sc), E_LEFT,
    E(i__uc), E_LEFT,
    E(i__ui), E_LEFT,
    E(i__ul), E_LEFT,
    E(i__ull), E_LEFT,
    E(i__us), E_LEFT,
    E(i_and), E_BOTH,
    E(i_andand), E_BOTH,
    E(i_andasgn), E_BOTH,
    E(i_asgn), E_BOTH,
    E(i_bnot), E_LEFT,
    E(i_comma), E_BOTH,
    E(i_dec), E_BOTH,
    E(i_divasgn), E_BOTH,
    E(i_divide), E_BOTH,
    E(i_eq), E_BOTH,
    E(i_fcall), E_FCALL,
    E(i_gaddr), E_ADDR,
    E(i_ge), E_BOTH,
    E(i_gt), E_BOTH,
    E(i_inc), E_BOTH,
    E(i_l2p), E_LEFT,
    E(i_laddr), E_ADDR,
    E(i_le), E_BOTH,
    E(i_leaf), E_INT,
    E(i_lnot), E_LEFT,
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
    E(i_umin), E_LEFT,
    E(i_xor), E_BOTH,
    /*
     * I took these out as an experiment to see if the routines in
     * builtins.h could be made static.
     */
#if 0
    E(int_dis), E_INT,
    E(int_find), E_INT,
    E(int_instr), E_INT,
    E(int_load), E_INT,
    E(int_printf), E_INT,
    E(int_purge_all_pages), E_INT,
    E(int_purge_user_pages), E_INT,
    E(int_regs_instr), E_INT,
    E(int_return_range), E_INT,
    E(int_sprintf), E_INT,
    E(int_strcat), E_INT,
    E(int_strchr), E_INT,
    E(int_strcmp), E_INT,
    E(int_strcpy), E_INT,
    E(int_strncat), E_INT,
    E(int_strncmp), E_INT,
    E(int_strncpy), E_INT,
    E(int_strrchr), E_INT,
#endif
    E(l__d), E_LEFT,
    E(l__f), E_LEFT,
    E(l__i), E_LEFT,
    E(l__l), E_LEFT,
    E(l__ll), E_LEFT,
    E(l__s), E_LEFT,
    E(l__sc), E_LEFT,
    E(l__uc), E_LEFT,
    E(l__ui), E_LEFT,
    E(l__ul), E_LEFT,
    E(l__ull), E_LEFT,
    E(l__us), E_LEFT,
    E(l_and), E_BOTH,
    E(l_andand), E_BOTH,
    E(l_andasgn), E_BOTH,
    E(l_asgn), E_BOTH,
    E(l_bnot), E_LEFT,
    E(l_comma), E_BOTH,
    E(l_dec), E_BOTH,
    E(l_divasgn), E_BOTH,
    E(l_divide), E_BOTH,
    E(l_eq), E_BOTH,
    E(l_fcall), E_FCALL,
    E(l_gaddr), E_ADDR,
    E(l_ge), E_BOTH,
    E(l_gt), E_BOTH,
    E(l_inc), E_BOTH,
    E(l_l2p), E_LEFT,
    E(l_laddr), E_ADDR,
    E(l_le), E_BOTH,
    E(l_leaf), E_LONG,
    E(l_lnot), E_LEFT,
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
    E(l_umin), E_LEFT,
    E(l_xor), E_BOTH,
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
    E(s__d), E_LEFT,
    E(s__f), E_LEFT,
    E(s__i), E_LEFT,
    E(s__l), E_LEFT,
    E(s__ll), E_LEFT,
    E(s__s), E_LEFT,
    E(s__sc), E_LEFT,
    E(s__uc), E_LEFT,
    E(s__ui), E_LEFT,
    E(s__ul), E_LEFT,
    E(s__ull), E_LEFT,
    E(s__us), E_LEFT,
    E(s_and), E_BOTH,
    E(s_andand), E_BOTH,
    E(s_andasgn), E_BOTH,
    E(s_asgn), E_BOTH,
    E(s_bnot), E_LEFT,
    E(s_comma), E_BOTH,
    E(s_dec), E_BOTH,
    E(s_divasgn), E_BOTH,
    E(s_divide), E_BOTH,
    E(s_eq), E_BOTH,
    E(s_fcall), E_FCALL,
    E(s_gaddr), E_ADDR,
    E(s_ge), E_BOTH,
    E(s_gt), E_BOTH,
    E(s_inc), E_BOTH,
    E(s_l2p), E_LEFT,
    E(s_laddr), E_ADDR,
    E(s_le), E_BOTH,
    E(s_leaf), E_SHORT,
    E(s_lnot), E_LEFT,
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
    E(s_umin), E_LEFT,
    E(s_xor), E_BOTH,
    E(sc__d), E_LEFT,
    E(sc__f), E_LEFT,
    E(sc__i), E_LEFT,
    E(sc__l), E_LEFT,
    E(sc__ll), E_LEFT,
    E(sc__s), E_LEFT,
    E(sc__sc), E_LEFT,
    E(sc__uc), E_LEFT,
    E(sc__ui), E_LEFT,
    E(sc__ul), E_LEFT,
    E(sc__ull), E_LEFT,
    E(sc__us), E_LEFT,
    E(sc_and), E_BOTH,
    E(sc_andand), E_BOTH,
    E(sc_andasgn), E_BOTH,
    E(sc_asgn), E_BOTH,
    E(sc_bnot), E_LEFT,
    E(sc_comma), E_BOTH,
    E(sc_dec), E_BOTH,
    E(sc_divasgn), E_BOTH,
    E(sc_divide), E_BOTH,
    E(sc_eq), E_BOTH,
    E(sc_fcall), E_FCALL,
    E(sc_gaddr), E_ADDR,
    E(sc_ge), E_BOTH,
    E(sc_gt), E_BOTH,
    E(sc_inc), E_BOTH,
    E(sc_l2p), E_LEFT,
    E(sc_laddr), E_ADDR,
    E(sc_le), E_BOTH,
    E(sc_leaf), E_SCHAR,
    E(sc_lnot), E_LEFT,
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
    E(sc_umin), E_LEFT,
    E(sc_xor), E_BOTH,
    E(st_asgn), E_BOTH,
    E(st_comma), E_BOTH,
    E(st_fcall), E_FCALL,
    E(st_gaddr), E_ADDR,
    E(st_l2p), E_LEFT,
    E(st_laddr), E_ADDR,
    E(st_leaf), E_ST,
    E(st_null), E_NULL,
    E(st_qc), E_QC,
    E(uc__d), E_LEFT,
    E(uc__f), E_LEFT,
    E(uc__i), E_LEFT,
    E(uc__l), E_LEFT,
    E(uc__ll), E_LEFT,
    E(uc__s), E_LEFT,
    E(uc__sc), E_LEFT,
    E(uc__uc), E_LEFT,
    E(uc__ui), E_LEFT,
    E(uc__ul), E_LEFT,
    E(uc__ull), E_LEFT,
    E(uc__us), E_LEFT,
    E(uc_and), E_BOTH,
    E(uc_andand), E_BOTH,
    E(uc_andasgn), E_BOTH,
    E(uc_asgn), E_BOTH,
    E(uc_bnot), E_LEFT,
    E(uc_comma), E_BOTH,
    E(uc_dec), E_BOTH,
    E(uc_divasgn), E_BOTH,
    E(uc_divide), E_BOTH,
    E(uc_eq), E_BOTH,
    E(uc_fcall), E_FCALL,
    E(uc_gaddr), E_ADDR,
    E(uc_ge), E_BOTH,
    E(uc_gt), E_BOTH,
    E(uc_inc), E_BOTH,
    E(uc_l2p), E_LEFT,
    E(uc_laddr), E_ADDR,
    E(uc_le), E_BOTH,
    E(uc_leaf), E_UCHAR,
    E(uc_lnot), E_LEFT,
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
    E(uc_umin), E_LEFT,
    E(uc_xor), E_BOTH,
    E(ui__d), E_LEFT,
    E(ui__f), E_LEFT,
    E(ui__i), E_LEFT,
    E(ui__l), E_LEFT,
    E(ui__ll), E_LEFT,
    E(ui__s), E_LEFT,
    E(ui__sc), E_LEFT,
    E(ui__uc), E_LEFT,
    E(ui__ui), E_LEFT,
    E(ui__ul), E_LEFT,
    E(ui__ull), E_LEFT,
    E(ui__us), E_LEFT,
    E(ui_and), E_BOTH,
    E(ui_andand), E_BOTH,
    E(ui_andasgn), E_BOTH,
    E(ui_asgn), E_BOTH,
    E(ui_bnot), E_LEFT,
    E(ui_comma), E_BOTH,
    E(ui_dec), E_BOTH,
    E(ui_divasgn), E_BOTH,
    E(ui_divide), E_BOTH,
    E(ui_eq), E_BOTH,
    E(ui_fcall), E_FCALL,
    E(ui_gaddr), E_ADDR,
    E(ui_ge), E_BOTH,
    E(ui_gt), E_BOTH,
    E(ui_inc), E_BOTH,
    E(ui_l2p), E_LEFT,
    E(ui_laddr), E_ADDR,
    E(ui_le), E_BOTH,
    E(ui_leaf), E_UINT,
    E(ui_lnot), E_LEFT,
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
    E(ui_umin), E_LEFT,
    E(ui_xor), E_BOTH,
    E(ul__d), E_LEFT,
    E(ul__f), E_LEFT,
    E(ul__i), E_LEFT,
    E(ul__l), E_LEFT,
    E(ul__ll), E_LEFT,
    E(ul__s), E_LEFT,
    E(ul__sc), E_LEFT,
    E(ul__uc), E_LEFT,
    E(ul__ui), E_LEFT,
    E(ul__ul), E_LEFT,
    E(ul__ull), E_LEFT,
    E(ul__us), E_LEFT,
    E(ul_and), E_BOTH,
    E(ul_andand), E_BOTH,
    E(ul_andasgn), E_BOTH,
    E(ul_asgn), E_BOTH,
    E(ul_bnot), E_LEFT,
    E(ul_comma), E_BOTH,
    E(ul_dec), E_BOTH,
    E(ul_divasgn), E_BOTH,
    E(ul_divide), E_BOTH,
    E(ul_eq), E_BOTH,
    E(ul_fcall), E_FCALL,
    E(ul_gaddr), E_ADDR,
    E(ul_ge), E_BOTH,
    E(ul_gt), E_BOTH,
    E(ul_inc), E_BOTH,
    E(ul_l2p), E_LEFT,
    E(ul_laddr), E_ADDR,
    E(ul_le), E_BOTH,
    E(ul_leaf), E_ULONG,
    E(ul_lnot), E_LEFT,
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
    E(ul_umin), E_LEFT,
    E(ul_xor), E_BOTH,
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
    E(us__d), E_LEFT,
    E(us__f), E_LEFT,
    E(us__i), E_LEFT,
    E(us__l), E_LEFT,
    E(us__ll), E_LEFT,
    E(us__s), E_LEFT,
    E(us__sc), E_LEFT,
    E(us__uc), E_LEFT,
    E(us__ui), E_LEFT,
    E(us__ul), E_LEFT,
    E(us__ull), E_LEFT,
    E(us__us), E_LEFT,
    E(us_and), E_BOTH,
    E(us_andand), E_BOTH,
    E(us_andasgn), E_BOTH,
    E(us_asgn), E_BOTH,
    E(us_bnot), E_LEFT,
    E(us_comma), E_BOTH,
    E(us_dec), E_BOTH,
    E(us_divasgn), E_BOTH,
    E(us_divide), E_BOTH,
    E(us_eq), E_BOTH,
    E(us_fcall), E_FCALL,
    E(us_gaddr), E_ADDR,
    E(us_ge), E_BOTH,
    E(us_gt), E_BOTH,
    E(us_inc), E_BOTH,
    E(us_l2p), E_LEFT,
    E(us_laddr), E_ADDR,
    E(us_le), E_BOTH,
    E(us_leaf), E_USHORT,
    E(us_lnot), E_LEFT,
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
    E(us_umin), E_LEFT,
    E(us_xor), E_BOTH,
};
#define TAB_SIZE (sizeof(name_tab) / sizeof(name_tab[0]))

static void internal_tree_dump(FILE *f, expr *e);
static dump_fcall(FILE *f, expr *e);

static dump_fcall(FILE *f, expr *e)
{
    cnode_list *cl;

    internal_tree_dump(f, e->e_call);
    for (cl = e->e_args; cl; cl = cl->cl_next) {
	fprintf(f, " %s", type_2_string[cl->cl_cnode.c_base]);
	internal_tree_dump(f, cl->cl_cnode.c_expr);
    }
}

static void internal_tree_dump(FILE *f, expr *e)
{
    struct tab *t, *t_end;

    /* Slow linear search -- gack */
    for (t_end = (t = name_tab) + TAB_SIZE;
	 t < t_end && t->t_func != e->e_func.i;
	 ++t);
    if (t == t_end) {			/* not found */
	fprintf(f, "expr at %s not found\n", P(e));
	return;
    }

    fprintf(f, "(%s ", t->t_name);
    switch (t->t_args) {
    case E_ADDR:
	fprintf(f, " %s", P(e->e_addr));
	break;
    case E_BOTH:
	if (e->e_bsize)
	    fprintf(f, "size:%d offset:%d ", e->e_bsize, e->e_boffset);
	else if (e->e_size > sizeof(long))
	    fprintf(f, "ularge_t ");
	internal_tree_dump(f, e->e_left);
	fprintf(f, " ");
	internal_tree_dump(f, e->e_right);
	break;
    case E_DOUBLE:
	fprintf(f, " %f", e->e_d);
	break;
    case E_FCALL:
	dump_fcall(f, e);
	break;
    case E_FLOAT:
	fprintf(f, " %f", e->e_f);
	break;
    case E_INT:
	fprintf(f, " %d", e->e_i);
	break;
    case E_LEFT:
	if (e->e_bsize)
	    fprintf(f, "size:%d offset:%d ", e->e_bsize, e->e_boffset);
	else if (e->e_size > sizeof(long))
	    fprintf(f, "Ularge_t ");
	internal_tree_dump(f, e->e_left);
	break;
    case E_LONG:
	fprintf(f, " %d", e->e_l);
	break;
    case E_NULL:
	break;
    case E_QC:
	internal_tree_dump(f, e->e_left);
	fprintf(f, " ");
	internal_tree_dump(f, e->e_right->e_left);
	fprintf(f, " ");
	internal_tree_dump(f, e->e_right->e_right);
	break;
    case E_SCHAR:
	fprintf(f, " %d", e->e_sc);
	break;
    case E_SHORT:
	fprintf(f, " %d", e->e_s);
	break;
    case E_ST:
	fprintf(f, " %s", P(e->e_st));
	break;
    case E_UCHAR:
	fprintf(f, " %u", e->e_uc);
	break;
    case E_UINT:
	fprintf(f, " %u", e->e_ui);
	break;
    case E_ULONG:
	fprintf(f, " %u", e->e_ul);
	break;
    case E_USHORT:
	fprintf(f, " %u", e->e_us);
	break;
    }
    fprintf(f, ")");
}

void tree_dump(FILE *f, expr *e)
{
    fprintf(f, "expr %s:", P(e));
    internal_tree_dump(f, e);
    fprintf(f, "\n");
}
