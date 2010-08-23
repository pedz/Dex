static char sccs_id[] = "@(#)disasm.c	1.7";

#include <sys/types.h>
#include "map.h"
#include "dex.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEBUG_BIT DISASM_C_BIT

#define BROKEN

/*
 * Yet another disassembler for the gipper.
 */
union inst {
    struct d_form {
	uint _opcd : 6;
	uint _rt : 5;
	uint _ra : 5;
	uint _d : 16;
    } _d_form;
    struct b_form {
	uint _opcd : 6;
	uint _bo : 5;
	uint _bi : 5;
	uint _bd : 14;
	uint _aa : 1;
	uint _lk : 1;
    } _b_form;
    struct i_form {
	uint _opcd : 6;
	uint _li : 24;
	uint _aa : 1;
	uint _lk : 1;
    } _i_form;
    struct sc_form {
	uint _opcd : 6;
	uint : 5;
	uint : 5;
	uint _fl1 : 4;
	uint _lev : 7;
	uint _fl2 : 3;
	uint _sa : 1;
	uint _lk : 1;
    } _sc_form;
    struct svc_form {
	uint _opcd : 6;
	uint : 5;
	uint : 5;
	uint _sv : 14;
	uint _sa : 1;
	uint _lk : 1;
    } _svc_form;
    struct x_form {
	uint _opcd : 6;
	uint _rt : 5;
	uint _ra : 5;
	uint _rb : 5;
	uint _eo : 10;
	uint _rc : 1;
    } _x_form;
    struct xl_form {
	uint _opcd : 6;
	uint _bt : 5;
	uint _ba : 5;
	uint _bb : 5;
	uint _eo : 10;
	uint _lk : 1;
    } _xl_form;
    struct xfx_form {
	uint _opcd : 6;
	uint _rt : 5;
	uint _fxm : 10;
	uint _eo : 10;
	uint _rc : 1;
    } _xfx_form;
    struct xfl_form {
	uint _opcd : 6;
	uint _flm : 10;
	uint _frb : 5;
	uint _eo : 10;
	uint _rc : 1;
    } _xfl_form;
    struct xo_form {
	uint _opcd : 6;
	uint _rt : 5;
	uint _ra : 5;
	uint _rb : 5;
	uint _oe : 1;
	uint _eo1 : 9;
	uint _rc : 1;
    } _xo_form;
    struct a_form {
	uint _opcd : 6;
	uint _frt : 5;
	uint _fra : 5;
	uint _frb : 5;
	uint _frc : 5;
	uint _xo : 5;
	uint _rc : 1;
    } _a_form;
    struct m_form {
	uint _opcd : 6;
	uint _rs : 5;
	uint _ra : 5;
	uint _rb : 5;
	uint _mb : 5;
	uint _me : 5;
	uint _rc : 1;
    } _m_form;
    struct md_form {
	uint _opcd : 6;
	uint _rs : 5;
	uint _ra : 5;
	uint _sh0 : 5;
	uint _mb : 6;
	uint _eo : 3;
	uint _sh1 : 1;
	uint _rc : 1;
    } _md_form;
    struct mds_form {
	uint _opcd : 6;
	uint _rs : 5;
	uint _ra : 5;
	uint _rb : 5;
	uint _mb : 6;
	uint _eo : 4;
	uint _rc : 1;
    } _mds_form;
    struct ds_form {
	uint _opcd : 6;
	uint _rt : 5;
	uint _ra : 5;
	uint _ds : 14;
	uint _eo : 2;
    } _ds_form;
    int32_t _i;
};

enum form { D, B, I, SC, SVC, X, XL, XFX, XFL, XO, A, M, MD, MDS, DS };
enum notes { P, P1 };

struct instr {
    char *i_name;
    char *i_pname;
    int i_opcd;
    int i_eo;
    int i_form;
    char *i_arg;
    int i_notes;
} table[] = {
/*        name   PPC name    op     eo form                    arg notes */
    {    "abs",         0, 0x1f, 0x168,  XO,             "%rt,%ra",   2},
    {    "abs",         0, 0x1f, 0x368,  XO,             "%rt,%ra",   2},/*B*/
    {    "clf",         0, 0x1f, 0x076,   X,             "%ra,%rb",   0},
    {    "cli",         0, 0x1f, 0x1f6,   X,             "%ra,%rb",   P},
    {    "div",         0, 0x1f, 0x14b,  XO,         "%rt,%ra,%rb",  28},
    {    "div",         0, 0x1f, 0x34b,  XO,         "%rt,%ra,%rb",  28},/*B*/
    {    "doz",         0, 0x1f, 0x108,  XO,         "%rt,%ra,%rb",  28},
    {    "doz",         0, 0x1f, 0x308,  XO,         "%rt,%ra,%rb",  28},/*B*/
    {    "lfq",         0, 0x38,     0,   D,        "%frt,%d(%ra)",   7},
    {    "mul",         0, 0x1f, 0x06b,   X,         "%rt,%ra,%rb",  28},
    {    "rac",         0, 0x1f, 0x332,   X,         "%rt,%ra,%rb", 1+P},
    {    "sle",         0, 0x1f, 0x099,   X,         "%ra,%rs,%rb",  18},
    {    "slq",         0, 0x1f, 0x098,   X,         "%ra,%rs,%rb",  18},
    {    "sre",         0, 0x1f, 0x299,   X,         "%ra,%rs,%rb",  18},
    {    "srq",         0, 0x1f, 0x298,   X,         "%ra,%rs,%rb",  18},
    {   "clcs",         0, 0x1f, 0x213,   X,             "%rt,%ra",   8},
    {   "divs",         0, 0x1f, 0x16b,  XO,         "%rt,%ra,%rb",  28},
    {   "divs",         0, 0x1f, 0x36b,  XO,         "%rt,%ra,%rb",  28},/*B*/
    {   "dozi",         0, 0x09,     0,   D,         "%rt,%ra,%si",   8},
    {   "lfqu",         0, 0x39,     0,   D,        "%frt,%d(%ra)",   7},
    {   "lfqx",         0, 0x1f, 0x317,   X,        "%frt,%ra,%rb",   7},
    {   "lfqx",         0, 0x1f, 0x337,   X,        "%frt,%ra,%rb",   7},
    {   "nabs",         0, 0x1f, 0x1e8,  XO,             "%rt,%ra",  28},
    {   "nabs",         0, 0x1f, 0x3e8,  XO,             "%rt,%ra",  28},/*B*/
    {   "rlmi",         0, 0x16,     0,   M, "%ra,%rs,%sh,%mb,%me",   1},
    {   "rrib",         0, 0x1f, 0x219,   X,         "%ra,%rs,%rb",  18},
    {   "sleq",         0, 0x1f, 0x0d9,   X,         "%ra,%rs,%rb",  18},
    {   "sliq",         0, 0x1f, 0x0b8,   X,         "%ra,%rs,%sh",  18},
    {   "sllq",         0, 0x1f, 0x0d8,   X,         "%ra,%rs,%rb",  18},
    {   "sraq",         0, 0x1f, 0x398,   X,         "%ra,%rs,%rb",  18},
    {   "srea",         0, 0x1f, 0x399,   X,         "%ra,%rs,%rb",  18},
    {   "sreq",         0, 0x1f, 0x2d9,   X,         "%ra,%rs,%rb",  18},
    {   "sriq",         0, 0x1f, 0x2b8,   X,         "%ra,%rs,%rb",  18},
    {   "srlq",         0, 0x1f, 0x2d8,   X,         "%ra,%rs,%rb",  18},
    {   "stfq",         0, 0x3c,     0,   D,        "%frs,%d(%ra)",   7},
    {  "dclst",         0, 0x1f, 0x276,   X,             "%ra,%rb",   0},
    {  "lscbx",         0, 0x1f, 0x115,   X,         "%rt,%ra,%rb",  18},
    {  "maskg",         0, 0x1f, 0x01d,   X,         "%ra,%rs,%rb",   0},
    {  "mfsri",         0, 0x1f, 0x273,   X,         "%rt,%ra,%rb",   0},
    {  "rfsvc",         0, 0x13, 0x052,  XL,                    "",   0},
    {  "slliq",         0, 0x1f, 0x0f8,   X,         "%ra,%rs,%sh",  18},
    {  "sriaq",         0, 0x1f, 0x3b8,   X,         "%ra,%rs,%sh",  18},
    {  "stfqu",         0, 0x3d,     0,   D,        "%frs,%d(%ra)",   7},
    {  "stfqx",         0, 0x1f, 0x397,   X,        "%frs,%ra,%rb",   7},
    { "maskir",         0, 0x1f, 0x21d,   X,         "%ra,%rs,%rb",   0},
    { "stfqux",         0, 0x1f, 0x3b7,   X,        "%frs,%ra,%rb",   7},
    {    "cax",     "add", 0x1f, 0x10a,  XO,         "%rt,%ra,%rb",   2},
    {    "cax",     "add", 0x1f, 0x30a,  XO,         "%rt,%ra,%rb",   2},/*B*/
    {      "a",    "addc", 0x1f, 0x00a,  XO,         "%rt,%ra,%rb",   2},
    {      "a",    "addc", 0x1f, 0x20a,  XO,         "%rt,%ra,%rb",   2},/*B*/
    {     "ae",    "adde", 0x1f, 0x08a,  XO,         "%rt,%ra,%rb",   2},
    {     "ae",    "adde", 0x1f, 0x28a,  XO,         "%rt,%ra,%rb",   2},/*B*/
    {    "cal",    "addi", 0x0e,     0,   D,         "%rt,%d(%ra)",   0},
    {     "ai",   "addic", 0x0c,     0,   D,         "%rt,%ra,%si",   0},
    {    "ai.",  "addic.", 0x0d,     0,   D,         "%rt,%ra,%si",   0},
    {    "cau",   "addis", 0x0f,     0,   D,         "%rt,%ra,%ui",   0},
    {    "ame",   "addme", 0x1f, 0x0ea,  XO,             "%rt,%ra",   2},
    {    "ame",   "addme", 0x1f, 0x2ea,  XO,             "%rt,%ra",   2},/*B*/
    {    "aze",   "addze", 0x1f, 0x0ca,  XO,             "%rt,%ra",   0},
    {    "aze",   "addze", 0x1f, 0x2ca,  XO,             "%rt,%ra",   0},/*B*/
    {    "and",     "and", 0x1f, 0x01c,   X,         "%ra,%rs,%rb",   1},
    {   "andc",    "andc", 0x1f, 0x03c,   X,         "%ra,%rs,%rb",   1},
    { "andil.",   "andi.", 0x1c,     0,   D,         "%ra,%rs,%ui",   0},
    { "andiu.",  "andis.", 0x1d,     0,   D,         "%ra,%rs,%ui",   0},
    {      "b",       "b", 0x12,     0,   I,                 "%li",   3},
    {     "bc",      "bc", 0x10,     0,   B,         "%bo,%bi,%bd",   3},
    {    "bcc",   "bcctr", 0x13, 0x210,  XL,             "%bo,%bi",   4},
    {    "bcr",    "bclr", 0x13, 0x010,  XL,             "%bo,%bi",   4},
    {  "bogus",   "bogus", 0x3f, 0x3ff,   D,                    "",   0},
    {    "cmp",     "cmp", 0x1f, 0x000,   X,         "%bf,%ra,%rb",   0},
    {   "cmpi",    "cmpi", 0x0b,     0,   D,         "%bf,%ra,%si",   0},
    {   "cmpl",    "cmpl", 0x1f, 0x020,   X,         "%bf,%ra,%rb",   0},
    {  "cmpli",   "cmpli", 0x0a,     0,   D,         "%bf,%ra,%ui",   0},
    {        0,  "cntlzd", 0x1f, 0x03a,   X,             "%ra,%rs",   0},
    {  "cntlz",  "cntlzw", 0x1f, 0x01a,   X,             "%ra,%rs",   1},
    {  "crand",   "crand", 0x13, 0x101,  XL,         "%bt,%ba,%bb",   0},
    { "crandc",  "crandc", 0x13, 0x081,  XL,         "%bt,%ba,%bb",   0},
    {  "creqv",   "creqv", 0x13, 0x121,  XL,         "%bt,%ba,%bb",   0},
    { "crnand",  "crnand", 0x13, 0x0e1,  XL,         "%bt,%ba,%bb",   0},
    {  "crnor",   "crnor", 0x13, 0x021,  XL,         "%bt,%ba,%bb",   0},
    {   "cror",    "cror", 0x13, 0x1c1,  XL,         "%bt,%ba,%bb",   0},
    {  "crorc",   "crorc", 0x13, 0x1a1,  XL,         "%bt,%ba,%bb",   0},
    {  "crxor",   "crxor", 0x13, 0x0c1,  XL,         "%bt,%ba,%bb",   0},
    {        0,    "dcbf", 0x1f, 0x056,   X,             "%ra,%rb",   6},
    {        0,    "dcbi", 0x1f, 0x1d6,   X,             "%ra,%rb",   6},
    {        0,   "dcbst", 0x1f, 0x036,   X,             "%ra,%rb",   6},
    {        0,    "dcbt", 0x1f, 0x116,   X,             "%ra,%rb",   6},
    {        0,  "dcbtst", 0x1f, 0x0f6,   X,             "%ra,%rb",   6},
    {   "dclz",    "dcbz", 0x1f, 0x3f6,   X,             "%ra,%rb",   P},
    {        0,    "divd", 0x1f, 0x1e9,  XO,         "%rt,%ra,%rb",   0},
    {        0,    "divd", 0x1f, 0x3e9,  XO,         "%rt,%ra,%rb",   0},/*B*/
    {        0,   "divdu", 0x1f, 0x1c9,  XO,         "%rt,%ra,%rb",   0},
    {        0,   "divdu", 0x1f, 0x3c9,  XO,         "%rt,%ra,%rb",   0},/*B*/
    {        0,    "divw", 0x1f, 0x1eb,  XO,         "%rt,%ra,%rb",  26},
    {        0,    "divw", 0x1f, 0x3eb,  XO,         "%rt,%ra,%rb",  26},/*B*/
    {        0,   "divwu", 0x1f, 0x1cb,  XO,         "%rt,%ra,%rb",  26},
    {        0,   "divwu", 0x1f, 0x3cb,  XO,         "%rt,%ra,%rb",  26},/*B*/
    {        0,   "eciwx", 0x1f, 0x136,   X,         "%rt,%ra,%rb",   6},
    {        0,   "ecowx", 0x1f, 0x1b6,   X,         "%rs,%ra,%rb",   6},
    {        0,   "eieio", 0x1f, 0x356,   X,                    "",   6},
    {    "eqv",     "eqv", 0x1f, 0x11c,   X,         "%ra,%rs,%rb",   1},
    {        0,   "extsb", 0x1f, 0x3ba,   X,             "%ra,%rs",  16},
    {   "exts",   "extsh", 0x1f, 0x39a,   X,             "%ra,%rs",   1},
    {        0,   "extsw", 0x1f, 0x380,   X,             "%ra,%rs",   1},
    {   "fabs",    "fabs", 0x3f, 0x108,   X,           "%frt,%frb",   1},
    {     "fa",    "fadd", 0x3f, 0x015,   A,      "%frt,%fra,%frb",   1},
    {        0,   "fadds", 0x3b, 0x015,   A,      "%frt,%fra,%frb",  16},
    {        0,   "fcfid", 0x3f, 0x34e,   X,           "%frt,%frb",   1},
    {  "fcmpo",   "fcmpo", 0x3f, 0x020,   X,       "%bf,%fra,%frb",   0},
    {  "fcmpu",   "fcmpu", 0x3f, 0x000,   X,       "%bf,%fra,%frb",   0},
    {        0,   "fctid", 0x3f, 0x32e,   X,           "%frt,%frb",   1},
    {        0,  "fctidz", 0x3f, 0x32f,   X,           "%frt,%frb",   1},
    {   "fcir",   "fctiw", 0x3f, 0x00e,   X,           "%frt,%frb",  67},
    {  "fcirz",  "fctiwz", 0x3f, 0x00f,   X,           "%frt,%frb",  67},
    {     "fd",    "fdiv", 0x3f, 0x012,   A,      "%frt,%fra,%frb",   1},
    {        0,   "fdivs", 0x3b, 0x012,   A,      "%frt,%fra,%frb",  16},
    {    "fma",   "fmadd", 0x3f, 0x01d,   A, "%frt,%fra,%frc,%frb",   1},
    {        0,  "fmadds", 0x3b, 0x01d,   A, "%frt,%fra,%frc,%frb",  16},
    {    "fmr",     "fmr", 0x3f, 0x048,   X,           "%frt,%frb",   1},
    {    "fms",   "fmsub", 0x3f, 0x01c,   A, "%frt,%fra,%frc,%frb",   1},
    {        0,  "fmsubs", 0x3b, 0x01c,   A, "%frt,%fra,%frc,%frb",  16},
    {     "fm",    "fmul", 0x3f, 0x019,   A,      "%frt,%fra,%frc",   1},
    {        0,   "fmuls", 0x3b, 0x019,   A,      "%frt,%fra,%frc",  16},
    {  "fnabs",   "fnabs", 0x3f, 0x088,   X,           "%frt,%frb",   1},
    {   "fneg",    "fneg", 0x3f, 0x028,   X,           "%frt,%frb",   1},
    {   "fnma",  "fnmadd", 0x3f, 0x01f,   A, "%frt,%fra,%frc,%frb",   1},
    {        0, "fnmadds", 0x3b, 0x01f,   A, "%frt,%fra,%frc,%frb",  16},
    {   "fmns",  "fnmsub", 0x3f, 0x01e,   A, "%frt,%fra,%frc,%frb",   1},
    {        0, "fnmsubs", 0x3b, 0x01e,   A, "%frt,%fra,%frc,%frb",  16},
    {        0,    "fres", 0x3b, 0x018,   A,           "%frt,%frb",  16},
    {   "frsp",    "frsp", 0x3f, 0x00c,   X,           "%frt,%frb",   1},
    {        0, "frsqrte", 0x3f, 0x01a,   A,           "%frt,%frb",  16},
    {        0,    "fsel", 0x3f, 0x017,   A, "%frt,%fra,%frc,%frb",  16},

    {  "fsqrt",   "fsqrt", 0x3f, 0x016,   A,           "%frt,%frb", 167},/*?*/
    {        0,  "fsqrts", 0x3b, 0x016,   A,           "%frt,%frb",  16},/*?*/

    {     "fs",    "fsub", 0x3f, 0x014,   A,      "%frt,%fra,%frb",   1},
    {        0,   "fsubs", 0x3b, 0x014,   A,      "%frt,%fra,%frb",  16},
    {        0,    "icbi", 0x1f, 0x3d6,   X,             "%ra,%rb",   6},
    {    "ics",   "isync", 0x13, 0x096,  XL,                    "",   0},
    {    "lbz",     "lbz", 0x22,     0,   D,         "%rt,%d(%ra)",   0},
    {   "lbzu",    "lbzu", 0x23,     0,   D,         "%rt,%d(%ra)",   0},
    {  "lbzux",   "lbzux", 0x1f, 0x077,   X,         "%rt,%ra,%rb",   0},
    {   "lbzx",    "lbzx", 0x1f, 0x057,   X,         "%rt,%ra,%rb",   0},
    {        0,      "ld", 0x3a,     0,  DS,        "%rt,%ds(%ra)",   0},
    {        0,   "ldarx", 0x1f, 0x054,   X,         "%rt,%ra,%rb",   0},
    {        0,     "ldu", 0x3a,     1,  DS,        "%rt,%ds(%ra)",   0},
    {        0,    "ldux", 0x1f, 0x035,   X,         "%rt,%ra,%rb",   0},
    {        0,     "ldx", 0x1f, 0x015,   X,         "%rt,%ra,%rb",   0},
    {    "lfd",     "lfd", 0x32,     0,   D,        "%frt,%d(%ra)",   0},
    {   "lfdu",    "lfdu", 0x33,     0,   D,        "%frt,%d(%ra)",   0},
    {  "lfdux",   "lfdux", 0x1f, 0x277,   X,        "%frt,%ra,%rb",   0},
    {   "lfdx",    "lfdx", 0x1f, 0x257,   X,        "%frt,%ra,%rb",   0},
    {    "lfs",     "lfs", 0x30,     0,   D,        "%frt,%d(%ra)",   0},
    {   "lfsu",    "lfsu", 0x31,     0,   D,        "%frt,%d(%ra)",   0},
    {  "lfsux",   "lfsux", 0x1f, 0x237,   X,        "%frt,%ra,%rb",   0},
    {   "lfsx",    "lfsx", 0x1f, 0x217,   X,        "%frt,%ra,%rb",   0},
    {    "lha",     "lha", 0x2a,     0,   D,         "%rt,%d(%ra)",   0},
    {   "lhau",    "lhau", 0x2b,     0,   D,         "%rt,%d(%ra)",   0},
    {  "lhaux",   "lhaux", 0x1f, 0x177,   X,         "%rt,%ra,%rb",   0},
    {   "lhax",    "lhax", 0x1f, 0x157,   X,         "%rt,%ra,%rb",   0},
    {  "lhbrx",   "lhbrx", 0x1f, 0x316,   X,         "%rt,%ra,%rb",   0},
    {    "lhz",     "lhz", 0x28,     0,   D,         "%rt,%d(%ra)",   0},
    {   "lhzu",    "lhzu", 0x29,     0,   D,         "%rt,%d(%ra)",   0},
    {  "lhzux",   "lhzux", 0x1f, 0x137,   X,         "%rt,%ra,%rb",   0},
    {   "lhzx",    "lhzx", 0x1f, 0x117,   X,         "%rt,%ra,%rb",   0},
    {     "lm",     "lmw", 0x2e,     0,   D,         "%rt,%d(%ra)",   0},
    {    "lsi",    "lswi", 0x1f, 0x255,   X,         "%rt,%ra,%nb",   0},
    {    "lsx",    "lswx", 0x1f, 0x215,   X,         "%rt,%ra,%rb",   0},
    {        0,     "lwa", 0x3a,     2,  DS,        "%rt,%ds(%ra)",   0},
    {        0,   "lwarx", 0x1f, 0x014,   X,         "%rt,%ra,%rb",   6},
    {        0,   "lwaux", 0x1f, 0x155,   X,         "%rt,%ra,%rb",   0},
    {        0,    "lwax", 0x1f, 0x014,   X,         "%rt,%ra,%rb",   0},
    {   "lbrx",   "lwbrx", 0x1f, 0x216,   X,         "%rt,%ra,%rb",   0},
    {      "l",     "lwz", 0x20,     0,   D,         "%rt,%d(%ra)",   0},
    {     "lu",    "lwzu", 0x21,     0,   D,         "%rt,%d(%ra)",   0},
    {    "lux",   "lwzux", 0x1f, 0x037,   X,         "%rt,%ra,%rb",   0},
    {     "lx",    "lwzx", 0x1f, 0x017,   X,         "%rt,%ra,%rb",   0},
    {   "mcrf",    "mcrf", 0x13, 0x000,  XL,            "%bf,%bfa",   0},
    {  "mcrfs",   "mcrfs", 0x3f, 0x040,   X,            "%bf,%bfa",   0},
    {  "mcrxr",   "mcrxr", 0x1f, 0x200,   X,                 "%bf",   0},
    {   "mfcr",    "mfcr", 0x1f, 0x013,   X,                 "%rt",   0},
    {   "mffs",    "mffs", 0x3f, 0x247,   X,                "%frt",   0},
    {  "mfmsr",   "mfmsr", 0x1f, 0x053,   X,                 "%rt",   0},
    {  "mfspr",   "mfspr", 0x1f, 0x153,   X,            "%rt,%spr",  P1},
    {   "mfsr",    "mfsr", 0x1f, 0x253,   X,             "%rt,%sr",  P1},
    {        0,  "mfsrin", 0x1f, 0x293,   X,             "%rt,%rb",  P1},
    {        0,    "mftb", 0x1f, 0x173, XFX,            "%rt,%tbr",   0},
    {  "mtcrf",   "mtcrf", 0x1f, 0x090, XFX,            "%fxm,%rs",   0},
    { "mtfsb0",  "mtfsb0", 0x3f, 0x046,   X,                 "%bt",   1},
    { "mtfsb1",  "mtfsb1", 0x3f, 0x026,   X,                 "%bt",   1},
    {  "mtfsf",   "mtfsf", 0x3f, 0x2c7, XFL,           "%flm,%frb",   1},
    { "mtfsfi",  "mtfsfi", 0x3f, 0x086,   X,              "%bf,%i",   1},
    {  "mtmsr",   "mtmsr", 0x1f, 0x092,   X,                 "%rs",   P},
    {  "mtspr",   "mtspr", 0x1f, 0x1d3,   X,            "%spr,%rs",  P1},
    {   "mtsr",    "mtsr", 0x1f, 0x0d2,   X,             "%sr,%rs",   P},
    {  "mtsri",  "mtsrin", 0x1f, 0x0f2,   X,         "%rs,%ra,%rb",   P},
    {        0,   "mulhd", 0x1f, 0x049,  XO,         "%rt,%ra,%rb",  26},
    {        0,   "mulhd", 0x1f, 0x249,  XO,         "%rt,%ra,%rb",  26},/*B*/
    {        0,  "mulhdu", 0x1f, 0x009,  XO,         "%rt,%ra,%rb",  26},
    {        0,  "mulhdu", 0x1f, 0x209,  XO,         "%rt,%ra,%rb",  26},/*B*/
    {        0,   "mulhw", 0x1f, 0x04b,  XO,         "%rt,%ra,%rb",  26},
    {        0,   "mulhw", 0x1f, 0x24b,  XO,         "%rt,%ra,%rb",  26},/*B*/
    {        0,  "mulhwu", 0x1f, 0x00b,  XO,         "%rt,%ra,%rb",  26},
    {        0,  "mulhwu", 0x1f, 0x20b,  XO,         "%rt,%ra,%rb",  26},/*B*/
    {        0,   "mulld", 0x1f, 0x0e9,  XO,         "%rt,%ra,%rb",  26},
    {        0,   "mulld", 0x1f, 0x2e9,  XO,         "%rt,%ra,%rb",  26},/*B*/
    {   "muli",   "mulli", 0x07,     0,   D,         "%rt,%ra,%si",   0},
    {   "muls",   "mullw", 0x1f, 0x0eb,  XO,         "%rt,%ra,%rb",   2},
    {   "muls",   "mullw", 0x1f, 0x2eb,  XO,         "%rt,%ra,%rb",   2},/*B*/
    {   "nand",    "nand", 0x1f, 0x1dc,   X,         "%ra,%rs,%rb",   1},
    {    "neg",     "neg", 0x1f, 0x068,  XO,             "%rt,%ra",   2},
    {    "neg",     "neg", 0x1f, 0x268,  XO,             "%rt,%ra",   2},/*B*/
    {    "nor",     "nor", 0x1f, 0x07c,   X,         "%ra,%rs,%rb",   1},
    {        0,  "oldimi", 0x1e,     3,  MD,     "%ra,%rs,%sh,%mb",   1},/*?*/
    {     "or",      "or", 0x1f, 0x1bc,   X,         "%ra,%rs,%rb",   1},
    {    "orc",     "orc", 0x1f, 0x19c,   X,         "%ra,%rs,%rb",   1},
    {   "oril",     "ori", 0x18,     0,   D,         "%ra,%rs,%ui",   0},
    {   "oriu",    "oris", 0x19,     0,   D,         "%ra,%rs,%ui",   0},
    {    "rfi",     "rfi", 0x13, 0x032,  XL,                    "",   P},
    {        0,   "rldcl", 0x1e,     8, MDS,     "%ra,%rs,%rb,%mb",   1},
    {        0,   "rldcr", 0x1e,     9, MDS,     "%ra,%rs,%rb,%me",   1},
    {        0,   "rldic", 0x1e,     2,  MD,     "%ra,%rs,%sh,%mb",   1},
    {        0,  "rldicl", 0x1e,     0,  MD,     "%ra,%rs,%sh,%mb",   1},
    {        0,  "rldicr", 0x1e,     1,  MD,     "%ra,%rs,%sh,%me",   1},
    {        0,  "rldimi", 0x1e,     3,  MD,     "%ra,%rs,%sh,%me",   1},
    {  "rlimi",  "rlwimi", 0x14,     0,   M, "%ra,%rs,%sh,%mb,%me",   1},
    {  "rlinm",  "rlwinm", 0x15,     0,   M, "%ra,%rs,%sh,%mb,%me",   1},
    {   "rlnm",   "rlwnm", 0x17,     0,   M, "%ra,%rs,%sh,%mb,%me",   1},
    {   "svca",      "sc", 0x11,     0,  SC,      "%lev,%fl1,%fl2",  35},
    {        0,      "si", 0x0c,     0,   D,         "%rt,%ra,%si",   0},
    {        0,     "si.", 0x0d,     0,   D,         "%rt,%ra,%si",   0},
    {        0,   "slbia", 0x1f, 0x1f2,   X,                    "",   0},
    {        0,   "slbie", 0x1f, 0x1b2,   X,                    "",   0},
    {        0,     "sld", 0x1f, 0x01b,   X,         "%ra,%rs,%rb",   1},
    {     "sl",     "slw", 0x1f, 0x018,   X,         "%rt,%rs,%rb",   1},
    {        0,    "srad", 0x1f, 0x31a,   X,         "%ra,%rs,%rb",   1},
    {        0,   "sradi", 0x1f, 0x19d,   X,         "%ra,%rs,%sh",   1},
    {    "sra",    "sraw", 0x1f, 0x318,   X,         "%ra,%rs,%rb",   1},
    {   "srai",   "srawi", 0x1f, 0x338,   X,         "%ra,%rs,%sh",   1},
    {  "srliq",   "srliq", 0x1f, 0x2f8,   X,         "%ra,%rs,%rb",  18},/*?*/
    {     "sr",     "srw", 0x1f, 0x218,   X,         "%ra,%rs,%rb",   1},
    {    "stb",     "stb", 0x26,     0,   D,         "%rs,%d(%ra)",   0},
    {   "stbu",    "stbu", 0x27,     0,   D,         "%rs,%d(%ra)",   0},
    {  "stbux",   "stbux", 0x1f, 0x0f7,   X,         "%rs,%ra,%rb",   0},
    {   "stbx",    "stbx", 0x1f, 0x0d7,   X,         "%rs,%ra,%rb",   0},
    {        0,     "std", 0x3e,     0,  DS,        "%rs,%ds(%ra)",   0},
    {        0,   "stdcx", 0x31, 0x0d6,   X,         "%rs,%ra,%rb",   1},
    {        0,    "stdu", 0x3e,     1,  DS,        "%rs,%ds(%ra)",   0},
    {        0,   "stdux", 0x31, 0x0b5,   X,         "%rs,%ra,%rb",   1},
    {        0,    "stdx", 0x31, 0x095,   X,         "%rs,%ra,%rb",   1},
    {   "stfd",    "stfd", 0x36,     0,   D,        "%frs,%d(%ra)",   0},
    {  "stfdu",   "stfdu", 0x37,     0,   D,        "%frs,%d(%ra)",   0},
    { "stfdux",  "stfdux", 0x1f, 0x2f7,   X,        "%frs,%ra,%rb",   0},
    {  "stfdx",   "stfdx", 0x1f, 0x2d7,   X,        "%frs,%ra,%rb",   0},
    {        0,  "stfiwx", 0x1f, 0x3d7,   X,        "%frs,%ra,%rb",   0},
    {   "stfs",    "stfs", 0x34,     0,   D,        "%frs,%d(%ra)",   0},
    {  "stfsu",   "stfsu", 0x35,     0,   D,        "%frs,%d(%ra)",   0},
    { "stfsux",  "stfsux", 0x1f, 0x2b7,   X,        "%frs,%ra,%rb",   0},
    {  "stfsx",   "stfsx", 0x1f, 0x297,   X,        "%frs,%ra,%rb",   0},
    {    "sth",     "sth", 0x2c,     0,   D,         "%rs,%d(%ra)",   0},
    { "sthbrx",  "sthbrx", 0x1f, 0x396,   X,         "%rs,%ra,%rb",   0},
    {   "sthu",    "sthu", 0x2d,     0,   D,         "%rs,%d(%ra)",   0},
    {  "sthux",   "sthux", 0x1f, 0x1b7,   X,         "%rs,%ra,%rb",   0},
    {   "sthx",    "sthx", 0x1f, 0x197,   X,         "%rs,%ra,%rb",   0},
    {    "stm",    "stmw", 0x2f,     0,   D,         "%rs,%d(%ra)",   0},
    {   "stsi",   "stswi", 0x1f, 0x2d5,   X,         "%rt,%ra,%nb",   0},
    {   "stsx",   "stswx", 0x1f, 0x295,   X,         "%rt,%ra,%rb",   0},
    {     "st",     "stw", 0x24,     0,   D,         "%rs,%d(%ra)",   0},
    {  "stbrx",  "stwbrx", 0x1f, 0x296,   X,         "%rs,%ra,%rb",   0},
    {        0,   "stwcx", 0x1f, 0x096,   X,         "%rs,%ra,%rb",   6},
    {    "stu",    "stwu", 0x25,     0,   D,         "%rt,%d(%ra)",   0},
    {   "stux",   "stwux", 0x1f, 0x0b7,   X,         "%rt,%ra,%rb",   0},
    {    "stx",    "stwx", 0x1f, 0x097,   X,         "%rt,%ra,%rb",   0},
    {        0,    "subf", 0x1f, 0x028,  XO,         "%rt,%ra,%rb",  26},
    {        0,    "subf", 0x1f, 0x228,  XO,         "%rt,%ra,%rb",  26},/*B*/
    {     "sf",   "subfc", 0x1f, 0x008,  XO,         "%rt,%ra,%rb",   2},
    {     "sf",   "subfc", 0x1f, 0x208,  XO,         "%rt,%ra,%rb",   2},/*B*/
    {    "sfe",   "subfe", 0x1f, 0x088,  XO,         "%rt,%ra,%rb",   2},
    {    "sfe",   "subfe", 0x1f, 0x288,  XO,         "%rt,%ra,%rb",   2},/*B*/
    {    "sfi",  "subfic", 0x08,     0,   D,         "%rt,%ra,%si",   0},
    {   "sfme",  "subfme", 0x1f, 0x0e8,  XO,             "%rt,%ra",   2},
    {   "sfme",  "subfme", 0x1f, 0x2e8,  XO,             "%rt,%ra",   2},/*B*/
    {   "sfze",  "subfze", 0x1f, 0x0c8,  XO,             "%rt,%ra",   2},
    {   "sfze",  "subfze", 0x1f, 0x2c8,  XO,             "%rt,%ra",   2},/*B*/
    {    "dcs",    "sync", 0x1f, 0x256,   X,                    "",   0},
    {        0,      "td", 0x1f, 0x044,   X,         "%to,%ra,%rb",   0},
    {        0,     "tdi", 0x02,     0,   D,         "%to,%ra,%si",   0},
    {        0,   "tlbia", 0x1f, 0x172,   X,                    "", P+6},/*?*/
    {   "tlbi",   "tlbie", 0x1f, 0x132,   X,             "%ra,%rb",   P},
    {        0, "tlbsync", 0x1f, 0x236,   X,                    "",   P},
    {      "t",      "tw", 0x1f, 0x004,   X,         "%to,%ra,%rb",   0},
    {     "ti",     "twi", 0x03,     0,   D,         "%to,%ra,%si",   0},
    {    "xor",     "xor", 0x1f, 0x13c,   X,         "%ra,%rs,%rb",   1},
    {  "xoril",    "xori", 0x1a,     0,   D,         "%ra,%rs,%ui",   0},
    {  "xoriu",   "xoris", 0x1b,     0,   D,         "%ra,%rs,%ui",   0},
};

static int err(char *buf, int line)
{
    return sprintf(buf, "unexpected form %s:%d ", __FILE__, line);
}

static int tcmp(const void *p1, const void *p2)
{
    struct instr *i1 = (struct instr *)p1;
    struct instr *i2 = (struct instr *)p2;
    int diff;

    if (diff = (i1->i_opcd - i2->i_opcd))
	return diff;
    return i1->i_eo - i2->i_eo;
}

static struct instr *find_instr(union inst i)
{
    int low, high, mid;
    int diff;
    struct instr *t;
    static sorted = 0;

    if (!sorted) {
	qsort(table, A_SIZE(table), sizeof(table[0]), tcmp);
	sorted = 1;
#ifdef FIND_DEBUG
	for (low = 0; low < A_SIZE(table); ++low) {
	    t = table + low;
	    printf("%2d %10s %2x %3x\n", low, t->i_name, t->i_opcd, t->i_eo);
	}
#endif
    }

    /*
     * Because the extended opcode field tests different bits for
     * different instruction forms, we can not do a full binary
     * search.  Instead we do a binary search down to the opcode level
     * and then do a sequential search looking for a match on the
     * extended opcode.
     */
    low = 0;
    high = A_SIZE(table) - 2;
    while (low <= high) {
	t = table + (mid = ((low + high) >> 1));
#ifdef FIND_DEBUG
	printf("%2d %2d %2d %2x %2x\n", low, mid, high, t->i_opcd,
	       i._d_form._opcd);
#endif
	if (t->i_opcd >= i._d_form._opcd)
	    high = mid - 1;
	else
	    low = mid + 1;
    }

    for (t = table + low; t->i_opcd == i._d_form._opcd; ++t)
	switch (t->i_form) {
	case B:
	case D:
	case I:
	case M:
	case SC:
	case SVC:
	    return t;

	case X:
	    if (!(diff = t->i_eo - i._x_form._eo))
		return t;
#ifdef FIND_DEBUG
	    printf("x   %03x %03x %08x\n", t->i_eo, i._x_form._eo, i._i);
#endif
	    break;

	case XL:
	    if (!(diff = t->i_eo - i._xl_form._eo))
		return t;
#ifdef FIND_DEBUG
	    printf("xl  %03x %03x %08x\n", t->i_eo, i._xl_form._eo,
		   i._i);
#endif
	    break;

	case XFX:
	    if (!(diff = t->i_eo - i._xfx_form._eo))
		return t;
#ifdef FIND_DEBUG
	    printf("xfx %03x %03x %08x\n", t->i_eo, i._xfx_form._eo, i._i);
#endif
	    break;

	case XFL:
	    if (!(diff = t->i_eo - i._xfl_form._eo))
		return t;
#ifdef FIND_DEBUG
	    printf("xfl %03x %03x %08x\n", t->i_eo, i._xfl_form._eo, i._i);
#endif
	    break;
	case XO:
	    if (!(diff = t->i_eo - i._xo_form._eo1))
		return t;
#ifdef FIND_DEBUG
	    printf("xo  %03x %03x %08x\n", t->i_eo, i._xo_form._eo1, i._i);
#endif
	    break;
	case A:
	    if (!(diff = t->i_eo - i._a_form._xo))
		return t;
#ifdef FIND_DEBUG
	    printf("a   %03x %03x %08x\n", t->i_eo, i._a_form._xo, i._i);
#endif
	    break;

	case MD:
	    if (!(diff = t->i_eo - i._md_form._eo))
		return t;
#ifdef FIND_DEBUG
	    printf("md  %03x %03x %08x\n", t->i_eo, i._md_form._eo, i._i);
#endif
	    break;

	case MDS:
	    if (!(diff = t->i_eo - i._mds_form._eo))
		return t;
#ifdef FIND_DEBUG
	    printf("mds %03x %03x %08x\n", t->i_eo, i._mds_form._eo, i._i);
#endif
	    break;

	case DS:
	    if (!(diff = t->i_eo - i._ds_form._eo))
		return t;
#ifdef FIND_DEBUG
	    printf("ds  %03x %03x %08x\n", t->i_eo, i._ds_form._eo, i._i);
#endif
	    break;
	}
    return 0;
}

#define MATCH(s, p) \
	(!strncmp(s, p, sizeof(p)-1) && \
	 (!s[sizeof(p)-1] || \
	  s[sizeof(p)-1] < 'a' || s[sizeof(p)-1] > 'z'))

char *instr(int *addr)
{
    static char buf[64];
    char ibuf[64];
    char *bufp;
    union inst i;
    char *s, c;
    struct instr *t;

    i._i = *v2f_type(int *, addr);
    if (!(t = find_instr(i)))
	return " illegal instruction ";
    if (t->i_name)
	strcpy(ibuf, t->i_name);
    else
	strcat(ibuf, t->i_pname);
    if (t->i_notes) {
	int ntemp;

	for (ntemp = t->i_notes; ntemp; ntemp = ntemp / 10) {
	    switch (ntemp % 10) {
	    case 1:
		if (t->i_form == A) {
		    if (i._a_form._rc)
			strcat(ibuf, ".");
		} else if (t->i_form == M) {
		    if (i._m_form._rc)
			strcat(ibuf, ".");
		} else if (t->i_form == X) {
		    if (i._x_form._rc)
			strcat(ibuf, ".");
		} else if (t->i_form == XFX) {
		    if (i._xfx_form._rc)
			strcat(ibuf, ".");
		} else if (t->i_form == XFL) {
		    if (i._xfl_form._rc)
			strcat(ibuf, ".");
		} else if (t->i_form == MD) {
		    if (i._md_form._rc)
			strcat(ibuf, ".");
		} else if (t->i_form == MDS) {
		    if (i._mds_form._rc)
			strcat(ibuf, ".");
		} else
		    strcat(ibuf, " note 1 error ");
		break;
	    case 2:
		if (t->i_form == XO) {
		    if (i._xo_form._oe)
			strcat(ibuf, "o");
		    if (i._xo_form._rc)
			strcat(ibuf, ".");
		} else
		    strcat(ibuf, " note 2 error ");
		break;
	    case 3:
		if (t->i_form == B) {
		    if (i._b_form._lk)
			strcat(ibuf, "l");
		    if (i._b_form._aa)
			strcat(ibuf, "a");
		} else if (t->i_form == I) {
		    if (i._i_form._lk)
			strcat(ibuf, "l");
		    if (i._i_form._aa)
			strcat(ibuf, "a");
		} else
		    strcat(ibuf, " note 3 error ");
		break;
	    case 4:
		if (t->i_form == SC) {
		    if (i._sc_form._lk)
			strcat(ibuf, "l");
		} else if (t->i_form == SVC) {
		    if (i._svc_form._lk)
			strcat(ibuf, "l");
		} else if (t->i_form == XL) {
		    if (i._xl_form._lk)
			strcat(ibuf, "l");
		} else
		    strcat(ibuf, " note 4 error ");
		break;
	    }
	}
    }
    bufp = buf + sprintf(buf, "%7s   ", ibuf);
    for (s = t->i_arg; s && (c = *s++); ) {
	if (c != '%') {
	    bufp += sprintf(bufp, "%c", c);
	    continue;
	}

	if (MATCH(s, "ba")) {
	    if (t->i_form == XL)
		bufp += sprintf(bufp, "cr%d", i._xl_form._ba);
	    else
		bufp += err(bufp, __LINE__);
	    s += 2;
	} else if (MATCH(s, "bb")) {
	    if (t->i_form == XL)
		bufp += sprintf(bufp, "cr%d", i._xl_form._bb);
	    else
		bufp += err(bufp, __LINE__);
	    s += 2;
	} else if (MATCH(s, "bd")) {
	    if (t->i_form == B) {
		int d = i._b_form._bd;
		if (d & 0x2000)
		    d |= 0xffffc000;
		if (i._b_form._aa)
		    bufp += sprintf(bufp, "0x%x", d << 2);
		else
		    bufp += sprintf(bufp, "0x%x", addr + d);
	    } else
		bufp += err(bufp, __LINE__);
	    s += 2;
	} else if (MATCH(s, "bf")) {
	    if (t->i_form == X)
		bufp += sprintf(bufp, "cr%d", i._x_form._rt >> 2);
	    else if (t->i_form == XL)
		bufp += sprintf(bufp, "cr%d", i._xl_form._bt >> 2);
	    else if (t->i_form == D)
		bufp += sprintf(bufp, "cr%d", i._d_form._rt >> 2);
	    else
		bufp += err(bufp, __LINE__);
	    s += 2;
	} else if (MATCH(s, "bfa")) {
	    if (t->i_form == X)
		bufp += sprintf(bufp, "cr%d", i._x_form._ra >> 2);
	    else if (t->i_form == XL)
		bufp += sprintf(bufp, "cr%d", i._xl_form._ba >> 2);
	    else
		bufp += err(bufp, __LINE__);
	    s += 3;
	} else if (MATCH(s, "bi")) {
	    if (t->i_form == XL)
		bufp += sprintf(bufp, "%x", i._xl_form._ba);
	    else if (t->i_form == B)
		bufp += sprintf(bufp, "%x", i._b_form._bi);
	    else
		bufp += err(bufp, __LINE__);
	    s += 2;
	} else if (MATCH(s, "bo")) {
	    if (t->i_form == B)
		bufp += sprintf(bufp, "%d", i._b_form._bo);
	    else if (t->i_form == XL)
		bufp += sprintf(bufp, "%d", i._xl_form._bt);
	    else
		bufp += err(bufp, __LINE__);
	    s += 2;
	} else if (MATCH(s, "bt")) {
	    if (t->i_form == XL)
		bufp += sprintf(bufp, "cr%d", i._xl_form._bt);
	    else if (t->i_form == X)
		bufp += sprintf(bufp, "cr%d", i._x_form._rt);
	    else
		bufp += err(bufp, __LINE__);
	    s += 2;
	} else if (MATCH(s, "d")) {
	    if (t->i_form == D) {
		int d = i._d_form._d;
		if (d >= 0x8000)
		    d |= 0xffff0000;
		bufp += sprintf(bufp, "0x%x", d);
	    } else
		bufp += err(bufp, __LINE__);
	    s += 1;
	} else if (MATCH(s, "ds")) {
	    if (t->i_form == DS) {
		int d = i._ds_form._ds << 2;
		if (d >= 0x8000)
		    d |= 0xffff0000;
		bufp += sprintf(bufp, "0x%x", d);
	    } else
		bufp += err(bufp, __LINE__);
	    s += 2;
	} else if (MATCH(s, "fl1")) {
	    if (t->i_form == SC)
		bufp += sprintf(bufp, "0x%x", i._sc_form._fl1);
	    else
		bufp += err(bufp, __LINE__);
	    s += 3;
	} else if (MATCH(s, "fl2")) {
	    if (t->i_form == SC)
		bufp += sprintf(bufp, "0x%x", i._sc_form._fl2);
	    else
		bufp += err(bufp, __LINE__);
	    s += 3;
	} else if (MATCH(s, "flm")) {
	    if (t->i_form == XFL)
		bufp += sprintf(bufp, "fpscr%d", i._xfl_form._flm);
	    else
		bufp += err(bufp, __LINE__);
	    s += 3;
	} else if (MATCH(s, "fra")) {
	    if (t->i_form == A)
		bufp += sprintf(bufp, "fpr%d", i._a_form._fra);
	    else if (t->i_form == X)
		bufp += sprintf(bufp, "fpr%d", i._x_form._ra);
	    else
		bufp += err(bufp, __LINE__);
	    s += 3;
	} else if (MATCH(s, "frb")) {
	    if (t->i_form == A)
		bufp += sprintf(bufp, "fpr%d", i._a_form._frb);
	    else if (t->i_form == XFL)
		bufp += sprintf(bufp, "fpr%d", i._xfl_form._frb);
	    else if (t->i_form == X)
		bufp += sprintf(bufp, "fpr%d", i._x_form._rb);
	    else
		bufp += err(bufp, __LINE__);
	    s += 3;
	} else if (MATCH(s, "frc")) {
	    if (t->i_form == A)
		bufp += sprintf(bufp, "fpr%d", i._a_form._frc);
	    else
		bufp += err(bufp, __LINE__);
	    s += 3;
	} else if (MATCH(s, "frs")) {
	    if (t->i_form == D)
		bufp += sprintf(bufp, "fpr%d", i._d_form._rt);
	    else if (t->i_form == X)
		bufp += sprintf(bufp, "fpr%d", i._x_form._rt);
	    else
		bufp += err(bufp, __LINE__);
	    s += 3;
	} else if (MATCH(s, "frt")) {
	    if (t->i_form == A)
		bufp += sprintf(bufp, "fpr%d", i._a_form._frt);
	    else if (t->i_form == D)
		bufp += sprintf(bufp, "fpr%d", i._d_form._rt);
	    else if (t->i_form == X)
		bufp += sprintf(bufp, "fpr%d", i._x_form._rt);
	    else
		bufp += err(bufp, __LINE__);
	    s += 3;
	} else if (MATCH(s, "fxm")) {
	    if (t->i_form == XFX)
		bufp += sprintf(bufp, "cr%d", i._xfx_form._fxm);
	    else
		bufp += err(bufp, __LINE__);
	    s += 3;
	} else if (MATCH(s, "i")) {
	    if (t->i_form == X)
		bufp += sprintf(bufp, "0x%x", i._x_form._rb);
	    else
		bufp += err(bufp, __LINE__);
	    s += 1;
	} else if (MATCH(s, "lev")) {
	    if (t->i_form == SC)
		bufp += sprintf(bufp, "0x%x", i._sc_form._lev);
	    else
		bufp += err(bufp, __LINE__);
	    s += 3;
	} else if (MATCH(s, "li")) {
	    if (t->i_form == I) {
		int d = i._i_form._li;
		if (d & 0x00800000)
		    d |= 0xff000000;
		if (i._i_form._aa)
		    bufp += sprintf(bufp, "0x%x", d << 2);
		else
		    bufp += sprintf(bufp, "0x%x", addr + d);
	    } else
		bufp += err(bufp, __LINE__);
	    s += 2;
	} else if (MATCH(s, "mb")) {
	    if (t->i_form == M)
		bufp += sprintf(bufp, "%d", i._m_form._mb);
	    else if (t->i_form == MD)
		bufp += sprintf(bufp, "%d", i._md_form._mb);
	    else if (t->i_form == MDS)
		bufp += sprintf(bufp, "%d", i._mds_form._mb);
	    else
		bufp += err(bufp, __LINE__);
	    s += 2;
	} else if (MATCH(s, "me")) {
	    if (t->i_form == M)
		bufp += sprintf(bufp, "%d", i._m_form._me);
	    else if (t->i_form == MD)
		bufp += sprintf(bufp, "%d", i._md_form._mb);
	    else if (t->i_form == MDS)
		bufp += sprintf(bufp, "%d", i._mds_form._mb);
	    else
		bufp += err(bufp, __LINE__);
	    s += 2;
	} else if (MATCH(s, "nb")) {
	    if (t->i_form == X)
		bufp += sprintf(bufp, "0x%x", i._x_form._rb);
	    else
		bufp += err(bufp, __LINE__);
	    s += 2;
	} else if (MATCH(s, "ra")) {
	    if (t->i_form == D)
		bufp += sprintf(bufp, "r%d", i._d_form._ra);
	    else if (t->i_form == M)
		bufp += sprintf(bufp, "r%d", i._m_form._ra);
	    else if (t->i_form == X)
		bufp += sprintf(bufp, "r%d", i._x_form._ra);
	    else if (t->i_form == XO)
		bufp += sprintf(bufp, "r%d", i._xo_form._ra);
	    else if (t->i_form == MD)
		bufp += sprintf(bufp, "r%d", i._md_form._ra);
	    else if (t->i_form == MDS)
		bufp += sprintf(bufp, "r%d", i._mds_form._ra);
	    else if (t->i_form == DS)
		bufp += sprintf(bufp, "r%d", i._ds_form._ra);
	    else
		bufp += err(bufp, __LINE__);
	    s += 2;
	} else if (MATCH(s, "rb")) {
	    if (t->i_form == M)
		bufp += sprintf(bufp, "r%d", i._m_form._rb);
	    else if (t->i_form == X)
		bufp += sprintf(bufp, "r%d", i._x_form._rb);
	    else if (t->i_form == XO)
		bufp += sprintf(bufp, "r%d", i._xo_form._rb);
	    else if (t->i_form == MDS)
		bufp += sprintf(bufp, "r%d", i._mds_form._rb);
	    else
		bufp += err(bufp, __LINE__);
	    s += 2;
	} else if (MATCH(s, "rs")) {
	    if (t->i_form == M)
		bufp += sprintf(bufp, "r%d", i._m_form._rs);
	    else if (t->i_form == D)
		bufp += sprintf(bufp, "r%d", i._d_form._rt);
	    else if (t->i_form == X)
		bufp += sprintf(bufp, "r%d", i._x_form._rt);
	    else if (t->i_form == XFX)
		bufp += sprintf(bufp, "r%d", i._xfx_form._rt);
	    else if (t->i_form == MD)
		bufp += sprintf(bufp, "r%d", i._md_form._rs);
	    else if (t->i_form == MDS)
		bufp += sprintf(bufp, "r%d", i._mds_form._rs);
	    else if (t->i_form == DS)
		bufp += sprintf(bufp, "r%d", i._ds_form._rt);
	    else
		bufp += err(bufp, __LINE__);
	    s += 2;
	} else if (MATCH(s, "rt")) {
	    if (t->i_form == D)
		bufp += sprintf(bufp, "r%d", i._d_form._rt);
	    else if (t->i_form == X)
		bufp += sprintf(bufp, "r%d", i._x_form._rt);
	    else if (t->i_form == XFX)
		bufp += sprintf(bufp, "r%d", i._xfx_form._rt);
	    else if (t->i_form == XO)
		bufp += sprintf(bufp, "r%d", i._xo_form._rt);
	    else if (t->i_form == DS)
		bufp += sprintf(bufp, "r%d", i._ds_form._rt);
	    else
		bufp += err(bufp, __LINE__);
	    s += 2;
	} else if (MATCH(s, "sa")) {
	    if (t->i_form == SC)
		bufp += sprintf(bufp, "0x%x", i._sc_form._sa);
	    else
		bufp += err(bufp, __LINE__);
	    s += 2;
	} else if (MATCH(s, "sh")) {
	    if (t->i_form == M)
		bufp += sprintf(bufp, "0x%x", i._m_form._rb);
	    else if (t->i_form == X)
		bufp += sprintf(bufp, "0x%x", i._x_form._rb);
	    else if (t->i_form == MD)
		bufp += sprintf(bufp, "0x%x",
				((i._md_form._sh1<<4)| i._md_form._sh0));
	    else
		bufp += err(bufp, __LINE__);
	    s += 2;
	} else if (MATCH(s, "si")) {
	    if (t->i_form == D) {
		int d = i._d_form._d;
		if (d & 0x8000)
		    d |= 0xffff0000;
		bufp += sprintf(bufp, "0x%x", d);
	    } else
		bufp += err(bufp, __LINE__);
	    s += 2;
	} else if (MATCH(s, "spr")) {
	    if (t->i_form == X)
		switch (i._x_form._ra) {
		case   0: bufp += sprintf(bufp, "mq2 ");   break;
		case   1: bufp += sprintf(bufp, "xer");    break;
		case   8: bufp += sprintf(bufp, "lr");     break;
		case   9: bufp += sprintf(bufp, "ctr");    break;
		case  18: bufp += sprintf(bufp, "dsisr");  break;
		case  19: bufp += sprintf(bufp, "dar");    break;
		case  20: bufp += sprintf(bufp, "rtcu2 "); break;
		case  21: bufp += sprintf(bufp, "rtcl2 "); break;
		case  22: bufp += sprintf(bufp, "dec");    break;
		case  25: bufp += sprintf(bufp, "sdr1");   break;
		case  26: bufp += sprintf(bufp, "srr0");   break;
		case  27: bufp += sprintf(bufp, "srr1");   break;
		case 272: bufp += sprintf(bufp, "sprg0");  break;
		case 273: bufp += sprintf(bufp, "sprg1");  break;
		case 274: bufp += sprintf(bufp, "sprg2");  break;
		case 275: bufp += sprintf(bufp, "sprg3");  break;
		case 282: bufp += sprintf(bufp, "ear");    break;
		case 284: bufp += sprintf(bufp, "tbl");    break;
		case 285: bufp += sprintf(bufp, "tbu");    break;
		case 528: bufp += sprintf(bufp, "ibat0u"); break;
		case 529: bufp += sprintf(bufp, "ibat0l"); break;
		case 530: bufp += sprintf(bufp, "ibat1u"); break;
		case 531: bufp += sprintf(bufp, "ibat1l"); break;
		case 532: bufp += sprintf(bufp, "ibat2u"); break;
		case 533: bufp += sprintf(bufp, "ibat2l"); break;
		case 534: bufp += sprintf(bufp, "ibat3u"); break;
		case 535: bufp += sprintf(bufp, "ibat3l"); break;
		case 536: bufp += sprintf(bufp, "dbat0u"); break;
		case 537: bufp += sprintf(bufp, "dbat0l"); break;
		case 538: bufp += sprintf(bufp, "dbat1u"); break;
		case 539: bufp += sprintf(bufp, "dbat1l"); break;
		case 540: bufp += sprintf(bufp, "dbat2u"); break;
		case 541: bufp += sprintf(bufp, "dbat2l"); break;
		case 542: bufp += sprintf(bufp, "dbat3u"); break;
		case 543: bufp += sprintf(bufp, "dbat3l"); break;
		default:
		    bufp += sprintf(bufp, "spr%d", i._x_form._ra);
		    break;
		}
	    else
		bufp += err(bufp, __LINE__);
	    s += 3;
	} else if (MATCH(s, "sr")) {
	    if (t->i_form == X)
		bufp += sprintf(bufp, "sr%d", i._x_form._ra);
	    else
		bufp += err(bufp, __LINE__);
	    s += 2;
	} else if (MATCH(s, "svc")) {
	    if (t->i_form == SVC)
		bufp += sprintf(bufp, "0x%x", i._svc_form._sv);
	    else
		bufp += err(bufp, __LINE__);
	    s += 3;
	} else if (MATCH(s, "tbr")) {
	    bufp += err(bufp, __LINE__);
	    s += 3;
	} else if (MATCH(s, "to")) {
	    int d;

	    if (t->i_form == D)
		d = i._d_form._rt;
	    else if (t->i_form == X)
		d = i._x_form._rt;
	    else
		bufp += err(bufp, __LINE__);
	    switch (d) {
	    case 0x01: bufp += sprintf(bufp, "lgt");   break;
	    case 0x02: bufp += sprintf(bufp, "llt");   break;
	    case 0x03: bufp += sprintf(bufp, "lne");   break;
	    case 0x04: bufp += sprintf(bufp, "eq");    break;
	    case 0x05: bufp += sprintf(bufp, "lge");   break;
	    case 0x06: bufp += sprintf(bufp, "lle");   break;
	    case 0x08: bufp += sprintf(bufp, "gt");    break;
	    case 0x0c: bufp += sprintf(bufp, "nl");    break;
	    case 0x10: bufp += sprintf(bufp, "lt");    break;
	    case 0x14: bufp += sprintf(bufp, "le");    break;
	    case 0x18: bufp += sprintf(bufp, "ne");    break;
	    case 0x1f: bufp += sprintf(bufp, "a");     break;
	    default:   bufp += sprintf(bufp, "0x%x", d); break;
	    }
	    s += 2;
	} else if (MATCH(s, "ui")) {
	    if (t->i_form == D)
		bufp += sprintf(bufp, "0x%x", i._d_form._d);
	    else
		bufp += err(bufp, __LINE__);
	    s += 2;
	} else {
	    bufp += sprintf(bufp, "Confusion with %s", s);
	}
    }
    return buf;
}

void dis(int *addr, int count)
{
    for ( ; --count >= 0; ++addr)
	printf("%s%s\n", P(addr), instr(addr));
}

/*
 * Returns a bitmap of registers that are changed starting at ADDR for
 * COUNT instructions.
 */
int regs_instr(long *addr, int count)
{
    union inst i;
    struct instr *t;
    int ret = 0;

    while (--count >= 0) {
	i._i = *v2f_type(int *, addr++);
	if ((t = find_instr(i)) && !strncmp(t->i_arg, "%rt", 3))
	    switch (t->i_form) {
	    case D:
		ret |= 1 << i._d_form._rt;
		break;
	    case X:
		ret |= 1 << i._x_form._rt;
		break;
	    case XFX:
		ret |= 1 << i._xfx_form._rt;
		break;
	    case XO:
		ret |= 1 << i._xo_form._rt;
		break;
	    }
    }
    return ret;
}
