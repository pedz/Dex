static char sccs_id[] = "@(#)disasm.c	1.3";

#include <sys/types.h>
#include "map.h"
#include "dex.h"

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
    long _l;
};

enum form { D, B, I, SC, SVC, X, XL, XFX, XFL, XO, A, M };
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
 {      "a",     "add", 0x1f, 0x00a,  XO,         "%rt,%ra,%rb",   2},
 {    "abs",         0, 0x1f, 0x168,  XO,             "%rt,%ra",   2},
 {     "ae",    "adde", 0x1f, 0x08a,  XO,         "%rt,%ra,%rb",   2},
 {     "ai",   "addic", 0x0c,     0,   D,         "%rt,%ra,%si",   0},
 {    "ai.",  "addic.", 0x0d,     0,   D,         "%rt,%ra,%si",   0},
 {    "ame",   "addme", 0x1f, 0x0ea,  XO,             "%rt,%ra",   2},
 {    "and",     "and", 0x1f, 0x01c,   X,         "%ra,%rs,%rb",   1},
 {   "andc",    "andc", 0x1f, 0x03c,   X,         "%ra,%rs,%rb",   1},
 { "andil.",   "andi.", 0x1c,     0,   D,         "%ra,%rs,%ui",   0},
 { "andiu.",  "andis.", 0x1d,     0,   D,         "%ra,%rs,%ui",   0},
 {    "aze",   "addze", 0x1f, 0x0ca,  XO,             "%rt,%ra",   0},
 {      "b",       "b", 0x12,     0,   I,                 "%li",   3},
 {     "bc",      "bc", 0x10,     0,   B,         "%bo,%bi,%bd",   3},
 {    "bcc",   "bcctr", 0x13, 0x210,  XL,             "%bo,%bi",   4},
 {    "bcr",    "bclr", 0x13, 0x010,  XL,             "%bo,%bi",   4},
 {    "cal",    "addi", 0x0e,     0,   D,         "%rt,%d(%ra)",   0},
 {    "cau",   "addis", 0x0f,     0,   D,         "%rt,%ra,%ui",   0},
 {    "cax",     "add", 0x1f, 0x10a,  XO,         "%rt,%ra,%rb",   2},
 {   "clcs",         0, 0x1f, 0x213,   X,             "%rt,%ra",   8},
 {    "clf",     "clf", 0x1f, 0x076,   X,             "%ra,%rb",   0},
 {    "cli",         0, 0x1f, 0x1f6,   X,             "%ra,%rb",   P},
 {    "cmp",     "cmp", 0x1f, 0x000,   X,         "%bf,%ra,%rb",   0},
 {   "cmpi",    "cmpi", 0x0b,     0,   D,         "%bf,%ra,%si",   0},
 {   "cmpl",    "cmpl", 0x1f, 0x020,   X,         "%bf,%ra,%rb",   0},
 {  "cmpli",   "cmpli", 0x0a,     0,   D,         "%bf,%ra,%ui",   0},
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
 {  "dclst",         0, 0x1f, 0x276,   X,             "%ra,%rb",   0},
 {   "dclz",    "dcbz", 0x1f, 0x3f6,   X,             "%ra,%rb",   P},
 {    "dcs",    "sync", 0x1f, 0x256,   X,                    "",   0},
 {    "div",         0, 0x1f, 0x14b,  XO,         "%rt,%ra,%rb",  28},
 {   "divs",         0, 0x1f, 0x16b,  XO,         "%rt,%ra,%rb",  28},
 {        0,    "divw", 0x1f, 0x1eb,  XO,         "%rt,%ra,%rb",  26},
 {        0,   "divwu", 0x1f, 0x1cb,  XO,         "%rt,%ra,%rb",  26},
 {    "doz",         0, 0x1f, 0x108,  XO,         "%rt,%ra,%rb",  28},
 {   "dozi",         0, 0x09,     0,   D,         "%rt,%ra,%si",   8},
 {        0,   "eciwx", 0x1f, 0x136,   X,         "%rt,%ra,%rb",   6},
 {        0,   "ecowx", 0x1f, 0x1b6,   X,         "%rs,%ra,%rb",   6},
 {        0,   "eieio", 0x1f, 0x356,   X,                    "",   6},
 {    "eqv",     "eqv", 0x1f, 0x11c,   X,         "%ra,%rs,%rb",   1},
 {   "exts",   "extsh", 0x1f, 0x39a,   X,             "%ra,%rs",   1},
 {        0,   "extsb", 0x1f, 0x3ba,   X,             "%ra,%rs",  16},
 {     "fa",    "fadd", 0x3f, 0x015,   A,      "%frt,%fra,%frb",   1},
 {        0,   "fadds", 0x3b, 0x015,   A,      "%frt,%fra,%frb",  16},
 {   "fabs",    "fabs", 0x3f, 0x108,   X,           "%frt,%frb",   1},
 {  "fcmpo",   "fcmpo", 0x3f, 0x020,   X,       "%bf,%fra,%frb",   0},
 {  "fcmpu",   "fcmpu", 0x3f, 0x000,   X,       "%bf,%fra,%frb",   0},
 {   "fcir",   "fctiw", 0x3f, 0x00e,   X,           "%frt,%frb",  67},
 {  "fcirz",  "fctiwz", 0x3f, 0x00f,   X,           "%frt,%frb",  67},
 {     "fd",    "fdiv", 0x3f, 0x012,   A,      "%frt,%fra,%frb",   1},
 {        0,   "fdivs", 0x3b, 0x012,   A,      "%frt,%fra,%frb",  16},
 {     "fm",    "fmul", 0x3f, 0x019,   A,      "%frt,%fra,%frc",   1},
 {    "fma",   "fmadd", 0x3f, 0x01d,   A, "%frt,%fra,%frc,%frb",   1},
 {        0,  "fmadds", 0x3b, 0x01d,   A, "%frt,%fra,%frc,%frb",  16},
 {    "fmr",     "fmr", 0x3f, 0x048,   X,           "%frt,%frb",   1},
 {    "fms",   "fmsub", 0x3f, 0x01c,   A, "%frt,%fra,%frc,%frb",   1},
 {        0,  "fmsubs", 0x3b, 0x01c,   A, "%frt,%fra,%frc,%frb",  16},
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
 {  "fsqrt",   "fsqrt", 0x3f, 0x016,   A,           "%frt,%frb", 167},
 {        0,  "fsqrts", 0x3b, 0x016,   A,           "%frt,%frb",  16},
 {     "fs",    "fsub", 0x3f, 0x014,   A,      "%frt,%fra,%frb",   1},
 {        0,   "fsubs", 0x3b, 0x014,   A,      "%frt,%fra,%frb",  16},
 {        0,    "icbi", 0x1f, 0x3d6,   X,             "%ra,%rb",   6},
 {    "ics",   "isync", 0x13, 0x096,  XL,                    "",   0},
 {      "l",     "lwz", 0x20,     0,   D,         "%rt,%d(%ra)",   0},
 {   "lbrx",   "lwbrx", 0x1f, 0x216,   X,         "%rt,%ra,%rb",   0},
 {    "lbz",     "lbz", 0x22,     0,   D,         "%rt,%d(%ra)",   0},
 {   "lbzu",    "lbzu", 0x23,     0,   D,         "%rt,%d(%ra)",   0},
 {  "lbzux",   "lbzux", 0x1f, 0x077,   X,         "%rt,%ra,%rb",   0},
 {   "lbzx",    "lbzx", 0x1f, 0x057,   X,         "%rt,%ra,%rb",   0},
 {    "lfd",     "lfd", 0x32,     0,   D,        "%frt,%d(%ra)",   0},
 {   "lfdu",    "lfdu", 0x33,     0,   D,        "%frt,%d(%ra)",   0},
 {  "lfdux",   "lfdux", 0x1f, 0x277,   X,        "%frt,%ra,%rb",   0},
 {   "lfdx",    "lfdx", 0x1f, 0x257,   X,        "%frt,%ra,%rb",   0},
 {    "lfq",         0, 0x38,     0,   D,        "%frt,%d(%ra)",   7},
 {   "lfqu",         0, 0x39,     0,   D,        "%frt,%d(%ra)",   7},
 {   "lfqx",         0, 0x1f, 0x337,   X,        "%frt,%ra,%rb",   7},
 {   "lfqx",         0, 0x1f, 0x317,   X,        "%frt,%ra,%rb",   7},
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
 {  "lscbx",         0, 0x1f, 0x115,   X,         "%rt,%ra,%rb",  18},
 {    "lsi",    "lswi", 0x1f, 0x255,   X,         "%rt,%ra,%nb",   0},
 {    "lsx",    "lswx", 0x1f, 0x215,   X,         "%rt,%ra,%rb",   0},
 {        0,   "lwarx", 0x1f, 0x014,   X,         "%rt,%ra,%rb",   6},
 {     "lu",    "lwzu", 0x21,     0,   D,         "%rt,%d(%ra)",   0},
 {    "lux",   "lwzux", 0x1f, 0x037,   X,         "%rt,%ra,%rb",   0},
 {     "lx",    "lwzx", 0x1f, 0x017,   X,         "%rt,%ra,%rb",   0},
 {  "maskg",         0, 0x1f, 0x01d,   X,         "%ra,%rs,%rb",   0},
 { "maskir",         0, 0x1f, 0x21d,   X,         "%ra,%rs,%rb",   0},
 {   "mcrf",    "mcrf", 0x13, 0x000,  XL,            "%bf,%bfa",   0},
 {  "mcrfs",   "mcrfs", 0x3f, 0x040,   X,            "%bf,%bfa",   0},
 {  "mcrxr",   "mcrxr", 0x1f, 0x200,   X,                 "%bf",   0},
 {   "mfcr",    "mfcr", 0x1f, 0x013,   X,                 "%rt",   0},
 {   "mffs",    "mffs", 0x3f, 0x247,   X,                "%frt",   0},
 {  "mfmsr",   "mfmsr", 0x1f, 0x053,   X,                 "%rt",   0},
 {  "mfspr",   "mfspr", 0x1f, 0x153,   X,            "%rt,%spr",  P1},
 {   "mfsr",    "mfsr", 0x1f, 0x253,   X,             "%rt,%sr",  P1},
 {  "mfsri",         0, 0x1f, 0x273,   X,         "%rt,%ra,%rb",   0},
 {        0,    "mftb", 0x1f, 0x173, XFX,            "%rt,%tbr",   0},
 {  "mtcrf",   "mtcrf", 0x1f, 0x090, XFX,            "%fxm,%rs",   0},
 { "mtfsb0",  "mtfsb0", 0x3f, 0x046,   X,                 "%bt",   1},
 { "mtfsb1",  "mtfsb1", 0x3f, 0x026,   X,                 "%bt",   1},
 {  "mtfsf",   "mtfsf", 0x3f, 0x2c7, XFL,           "%flm,%frb",   1},
 { "mtfsfi",  "mtfsfi", 0x3f, 0x086,   X,              "%bf,%i",   1},
 {  "mtmsr",   "mtmsr", 0x1f, 0x092,   X,                 "%rs",   P},
 {  "mtspr",   "mtspr", 0x1f, 0x1d3,   X,            "%spr,%rs",  P1},
 {   "mtsr",    "mtsr", 0x1f, 0x0d2,   X,             "%sr,%rs",   P},
 {  "mtsri",   "mtsri", 0x1f, 0x0f2,   X,         "%rs,%ra,%rb",   P},
 {    "mul",         0, 0x1f, 0x06b,   X,         "%rt,%ra,%rb",  28},
 {        0,   "mulhw", 0x1f, 0x04b,  XO,         "%rt,%ra,%rb",  26},
 {        0,  "mulhwu", 0x1f, 0x00b,  XO,         "%rt,%ra,%rb",  26},
 {   "muli",    "muli", 0x07,     0,   D,         "%rt,%ra,%si",   0},
 {   "muls",   "mullw", 0x1f, 0x0eb,  XO,         "%rt,%ra,%rb",   2},
 {   "nabs",         0, 0x1f, 0x1e8,  XO,             "%rt,%ra",  28},
 {   "nand",    "nand", 0x1f, 0x1dc,   X,         "%ra,%rs,%rb",   1},
 {    "neg",     "neg", 0x1f, 0x068,  XO,             "%rt,%ra",   2},
 {    "nor",     "nor", 0x1f, 0x07c,   X,         "%ra,%rs,%rb",   1},
 {     "or",      "or", 0x1f, 0x1bc,   X,         "%ra,%rs,%rb",   1},
 {    "orc",     "orc", 0x1f, 0x19c,   X,         "%ra,%rs,%rb",   1},
 {   "oril",     "ori", 0x18,     0,   D,         "%ra,%rs,%ui",   0},
 {   "oriu",    "oris", 0x19,     0,   D,         "%ra,%rs,%ui",   0},
 {    "rac",         0, 0x1f, 0x332,   X,         "%rt,%ra,%rb", 1+P},
 {    "rfi",     "rfi", 0x13, 0x032,  XL,                    "",   P},
 {  "rfsvc",         0, 0x13, 0x052,  XL,                    "",   0},
 {  "rlimi",  "rlwimi", 0x14,     0,   M, "%ra,%rs,%sh,%mb,%me",   1},
 {  "rlinm",  "rlwinm", 0x15,     0,   M, "%ra,%rs,%sh,%mb,%me",   1},
 {   "rlmi",         0, 0x16,     0,   M, "%ra,%rs,%sh,%mb,%me",   1},
 {   "rlnm",   "rlwnm", 0x17,     0,   M, "%ra,%rs,%sh,%mb,%me",   1},
 {   "rrib",         0, 0x1f, 0x219,   X,         "%ra,%rs,%rb",  18},
 {     "sf",   "subfc", 0x1f, 0x008,  XO,         "%rt,%ra,%rb",   2},
 {    "sfe",   "subfe", 0x1f, 0x088,  XO,         "%rt,%ra,%rb",   2},
 {    "sfi",  "subfic", 0x08,     0,   D,         "%rt,%ra,%si",   0},
 {   "sfme",  "subfme", 0x1f, 0x0e8,  XO,             "%rt,%ra",   2},
 {   "sfze",  "subfze", 0x1f, 0x0c8,  XO,             "%rt,%ra",   2},
 {     "sl",     "slw", 0x1f, 0x018,   X,         "%rt,%rs,%rb",   1},
 {    "sle",         0, 0x1f, 0x099,   X,         "%ra,%rs,%rb",  18},
 {   "sleq",         0, 0x1f, 0x0d9,   X,         "%ra,%rs,%rb",  18},
 {   "sliq",         0, 0x1f, 0x0b8,   X,         "%ra,%rs,%sh",  18},
 {   "slliq",        0, 0x1f, 0x0f8,   X,         "%ra,%rs,%sh",  18},
 {   "sllq",         0, 0x1f, 0x0d8,   X,         "%ra,%rs,%rb",  18},
 {    "slq",         0, 0x1f, 0x098,   X,         "%ra,%rs,%rb",  18},
 {     "sr",     "srw", 0x1f, 0x218,   X,         "%ra,%rs,%rb",   1},
 {    "sra",    "sraw", 0x1f, 0x318,   X,         "%ra,%rs,%rb",   1},
 {   "srai",   "srawi", 0x1f, 0x338,   X,         "%ra,%rs,%sh",   1},
 {  "sriaq",         0, 0x1f, 0x3b8,   X,         "%ra,%rs,%sh",  18},
 {   "sraq",         0, 0x1f, 0x398,   X,         "%ra,%rs,%rb",  18},
 {    "sre",         0, 0x1f, 0x299,   X,         "%ra,%rs,%rb",  18},
 {   "srea",         0, 0x1f, 0x399,   X,         "%ra,%rs,%rb",  18},
 {   "sreq",         0, 0x1f, 0x2d9,   X,         "%ra,%rs,%rb",  18},
 {   "sriq",         0, 0x1f, 0x2b8,   X,         "%ra,%rs,%rb",  18},
 {  "srliq",   "srliq", 0x1f, 0x2f8,   X,         "%ra,%rs,%rb",  18},
 {   "srlq",         0, 0x1f, 0x2d8,   X,         "%ra,%rs,%rb",  18},
 {    "srq",         0, 0x1f, 0x298,   X,         "%ra,%rs,%rb",  18},
 {     "st",     "stw", 0x24,     0,   D,         "%rs,%d(%ra)",   0},
 {    "stb",     "stb", 0x26,     0,   D,         "%rs,%d(%ra)",   0},
 {  "stbrx",  "stwbrx", 0x1f, 0x296,   X,         "%rs,%ra,%rb",   0},
 {   "stbu",    "stbu", 0x27,     0,   D,         "%rs,%d(%ra)",   0},
 {  "stbux",   "stbux", 0x1f, 0x0f7,   X,         "%rs,%ra,%rb",   0},
 {   "stbx",    "stbx", 0x1f, 0x0d7,   X,         "%rs,%ra,%rb",   0},
 {   "stfd",    "stfd", 0x36,     0,   D,        "%frs,%d(%ra)",   0},
 {  "stfdu",   "stfdu", 0x37,     0,   D,        "%frs,%d(%ra)",   0},
 { "stfdux",  "stfdux", 0x1f, 0x2f7,   X,        "%frs,%ra,%rb",   0},
 {  "stfdx",   "stfdx", 0x1f, 0x2d7,   X,        "%frs,%ra,%rb",   0},
 {        0,  "stfiwx", 0x1f, 0x3d7,   X,        "%frs,%ra,%rb",   0},
 {   "stfq",         0, 0x3c,     0,   D,        "%frs,%d(%ra)",   7},
 {  "stfqu",         0, 0x3d,     0,   D,        "%frs,%d(%ra)",   7},
 { "stfqux",         0, 0x1f, 0x3b7,   X,        "%frs,%ra,%rb",   7},
 {  "stfqx",         0, 0x1f, 0x397,   X,        "%frs,%ra,%rb",   7},
 {   "stfs",   "stfsu", 0x34,     0,   D,        "%frs,%d(%ra)",   0},
 {  "stfsu",   "stfsu", 0x35,     0,   D,        "%frs,%d(%ra)",   0},
 { "stfsux",  "stfsux", 0x1f, 0x2b7,   X,        "%frs,%ra,%rb",   0},
 {  "stfsx",   "stfsx", 0x1f, 0x297,   X,        "%frs,%ra,%rb",   0},
 {    "sth",     "sth", 0x2c,     0,   D,         "%rs,%d(%ra)",   0},
 { "sthbrx",  "sthbrx", 0x1f, 0x396,   X,         "%rs,%ra,%rb",   0},
 {   "sthu",    "sthu", 0x2d,     0,   D,         "%rs,%d(%ra)",   0},
 {  "sthux",   "sthux", 0x1f, 0x1b7,   X,         "%rs,%ra,%rb",   0},
 {   "sthx",    "sthx", 0x1f, 0x197,   X,         "%rs,%ra,%rb",   0},
 {    "stm",     "stm", 0x2f,     0,   D,         "%rs,%d(%ra)",   0},
 {   "stsi",   "stswi", 0x1f, 0x2d5,   X,         "%rt,%ra,%nb",   0},
 {   "stsx",   "stswx", 0x1f, 0x295,   X,         "%rt,%ra,%rb",   0},
 {        0,   "stwcx", 0x1f, 0x096,   X,         "%rs,%ra,%rb",   6},
 {    "stu",    "stwu", 0x25,     0,   D,         "%rt,%d(%ra)",   0},
 {   "stux",   "stwux", 0x1f, 0x0b7,   X,         "%rt,%ra,%rb",   0},
 {    "stx",    "stwx", 0x1f, 0x097,   X,         "%rt,%ra,%rb",   0},
 {        0,    "subf", 0x1f, 0x028,  XO,         "%rt,%ra,%rb",  26},
 {   "svca",      "sc", 0x11,     0,  SC,      "%lev,%fl1,%fl2",  35},
 {      "t",      "tw", 0x1f, 0x004,   X,         "%to,%ra,%rb",   0},
 {     "ti",     "twi", 0x03,     0,   D,         "%to,%ra,%si",   0},
 {        0,   "tlbia", 0x1f, 0x172,   X,                    "", P+6},
 {   "tlbi",   "tlbie", 0x1f, 0x132,   X,             "%ra,%rb",   P},
 {        0, "tlbsync", 0x1f, 0x236,   X,                    "",   P},
 {    "xor",     "xor", 0x1f, 0x13c,   X,         "%ra,%rs,%rb",   1},
 {  "xoril",    "xori", 0x1a,     0,   D,         "%ra,%rs,%ui",   0},
 {  "xoriu",   "xoris", 0x1b,     0,   D,         "%ra,%rs,%ui",   0},
 {  "bogus",   "bogus", 0x3f, 0x3ff,   D,                    "",   0},
 /*
  * These opcodes are defined to make the binary search work
  * properly.  If BROKEN is defined, then these are not defined and
  * the find_instr routine changes to reproduce the way it use to work
  * which was broken.
  */
#ifndef BROKEN
 {      "a",     "add", 0x1f, 0x20a,  XO,         "%rt,%ra,%rb",   2},
 {    "abs",         0, 0x1f, 0x368,  XO,             "%rt,%ra",   2},
 {     "ae",    "adde", 0x1f, 0x28a,  XO,         "%rt,%ra,%rb",   2},
 {    "ame",   "addme", 0x1f, 0x2ea,  XO,             "%rt,%ra",   2},
 {    "aze",   "addze", 0x1f, 0x2ca,  XO,             "%rt,%ra",   0},
 {    "cax",     "add", 0x1f, 0x30a,  XO,         "%rt,%ra,%rb",   2},
 {    "div",         0, 0x1f, 0x34b,  XO,         "%rt,%ra,%rb",  28},
 {   "divs",         0, 0x1f, 0x36b,  XO,         "%rt,%ra,%rb",  28},
 {        0,    "divw", 0x1f, 0x3eb,  XO,         "%rt,%ra,%rb",  26},
 {        0,   "divwu", 0x1f, 0x3cb,  XO,         "%rt,%ra,%rb",  26},
 {    "doz",         0, 0x1f, 0x308,  XO,         "%rt,%ra,%rb",  28},
 {        0,   "mulhw", 0x1f, 0x24b,  XO,         "%rt,%ra,%rb",  26},
 {        0,  "mulhwu", 0x1f, 0x20b,  XO,         "%rt,%ra,%rb",  26},
 {   "muls",   "mullw", 0x1f, 0x2eb,  XO,         "%rt,%ra,%rb",   2},
 {   "nabs",         0, 0x1f, 0x3e8,  XO,             "%rt,%ra",  28},
 {    "neg",     "neg", 0x1f, 0x268,  XO,             "%rt,%ra",   2},
 {     "sf",   "subfc", 0x1f, 0x208,  XO,         "%rt,%ra,%rb",   2},
 {    "sfe",   "subfe", 0x1f, 0x288,  XO,         "%rt,%ra,%rb",   2},
 {   "sfme",  "subfme", 0x1f, 0x2e8,  XO,             "%rt,%ra",   2},
 {   "sfze",  "subfze", 0x1f, 0x2c8,  XO,             "%rt,%ra",   2},
 {        0,    "subf", 0x1f, 0x228,  XO,         "%rt,%ra,%rb",  26},
#endif
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
	    printf("%10s %2x %3x\n", t->i_name, t->i_opcd, t->i_eo);
	}
#endif
    }

    low = 0;
    high = A_SIZE(table);
#ifdef FIND_DEBUG
    printf("%2x %3x\n", i._d_form._opcd, i._x_form._eo);
#endif
    while (low <= high) {
	t = table + (mid = ((low + high) >> 1));
	if (!(diff = t->i_opcd - i._d_form._opcd)) {
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
		printf("x   %03x %03x %08x ", t->i_eo, i._x_form._eo, i._l);
#endif
		break;
	    case XL:
		if (!(diff = t->i_eo - i._xl_form._eo))
		    return t;
#ifdef FIND_DEBUG
		printf("xl  %03x %03x %08x ", t->i_eo, i._xl_form._eo,
		       i._l);
#endif
		break;
	    case XFX:
		if (!(diff = t->i_eo - i._xfx_form._eo))
		    return t;
#ifdef FIND_DEBUG
		printf("xfx %03x %03x %08x ", t->i_eo, i._xfx_form._eo, i._l);
#endif
		break;
	    case XFL:
		if (!(diff = t->i_eo - i._xfl_form._eo))
		    return t;
#ifdef FIND_DEBUG
		printf("xfl %03x %03x %08x ", t->i_eo, i._xfl_form._eo, i._l);
#endif
		break;
	    case XO:
#ifndef BROKEN
		/*
		 * Note: we compare to the x_form.eo because we have
		 * made duplicate entries for the two ways the oe bit
		 * can be set.  Otherwise the binary search messes up.
		 */
		if (!(diff = t->i_eo - i._x_form._eo))
		    return t;
#ifdef FIND_DEBUG
		printf("xo  %03x %03x %08x ", t->i_eo, i._x_form._eo, i._l);
#endif
#else
		if (!(diff = t->i_eo - i._xo_form._eo1))
		    return t;
#ifdef FIND_DEBUG
		printf("xo  %03x %03x %08x ", t->i_eo, i._xo_form._eo1, i._l);
#endif
#endif
		break;
	    case A:
		if (!(diff = t->i_eo - i._a_form._xo))
		    return t;
#ifdef FIND_DEBUG
		printf("a   %03x %03x %08x ", t->i_eo, i._a_form._xo, i._l);
#endif
		break;
	    }
	}
#ifdef FIND_DEBUG
	printf("%3d %3d %3d %4d\n", low, mid, high, diff);
#endif
	if (diff < 0)
	    low = mid + 1;
	else
	    high = mid - 1;
    }
    return 0;
}

#define MATCH(s, p) \
	(!strncmp(s, p, sizeof(p)-1) && \
	 (!s[sizeof(p)-1] || \
	  s[sizeof(p)-1] < 'a' || s[sizeof(p)-1] > 'z'))

char *instr(long *addr)
{
    static char buf[64];
    char ibuf[64];
    char *bufp;
    union inst i;
    char *s, c;
    struct instr *t;

    i._l = *v2f_type(long *, addr);
    if (!(t = find_instr(i)))
	return " illegal instruction ";
    if (t->i_name)
	strcpy(ibuf, t->i_name);
    else {
	strcpy(ibuf, "{");
	strcat(ibuf, t->i_pname);
    }
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
    if (!t->i_name)
	strcat(ibuf, "}");
    bufp = buf + sprintf(buf, "%9s   ", ibuf);
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
	    else
		bufp += err(bufp, __LINE__);
	    s += 2;
	} else if (MATCH(s, "me")) {
	    if (t->i_form == M)
		bufp += sprintf(bufp, "%d", i._m_form._me);
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
		bufp += sprintf(bufp, "spr%d", i._x_form._ra);
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

void dis(long *addr, int count)
{
    for ( ; --count >= 0; ++addr)
	printf("%08x%s\n", addr, instr(addr));
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
	i._l = *v2f_type(long *, addr++);
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
