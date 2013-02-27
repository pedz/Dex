h03004
s 01026/00000/00000
d D 1.1 02/03/14 16:46:01 pedz 1 0
c date and time created 02/03/14 16:46:01 by pedz
e
u
U
t
T
I 1
/*
 * I wish I had a penny for every time I've written this program.
 */

static char sccsid[] = "%W%";

#define __XCOFF32__
#define __XCOFF64__

#include <a.out.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <time.h>
#include <stdarg.h>
#include <stdlib.h>


char *prog;

#if 0
/* I'm confused why my typchk agrees with the comments but disagrees
 * with the definition.
 */
struct typchk {
    short t_size;
    short t_lang;
    int t_ghash;
    int t_lhash;
};
#endif

char *lang[] = {
    "C",
    "Fortran",
    "Pascal",
    "Ada",
    "PL/I",
    "Basic",
    "Lisp",
    "Cobol",
    "Modula2",
    "C++",
    "RPG",
    "PL8",
    "Assembly"
};

struct flags {
    char *f_name;
    int f_mask;
    int f_bits;
};

struct flags header_flags[] = {
    "relflg", F_RELFLG, F_RELFLG,
    "exec", F_EXEC, F_EXEC,
    "lnno", F_LNNO, F_LNNO,
    "lsyms", F_LSYMS, F_LSYMS,
    "ar16wr", F_AR16WR, F_AR16WR,
    "ar32wr", F_AR32WR, F_AR32WR,
    "ar32w", F_AR32W, F_AR32W,
    "dynload", F_DYNLOAD, F_DYNLOAD,
    "shrobj", F_SHROBJ, F_SHROBJ,
    0
};

struct flags section_flags[] = {
    "reg", 0xffff, STYP_REG,
    "pad", 0xffff, STYP_PAD,
    "text", 0xffff, STYP_TEXT,
    "data", 0xffff, STYP_DATA,
    "bss", 0xffff, STYP_BSS,
    "except", 0xffff, STYP_EXCEPT,
    "loader", 0xffff, STYP_LOADER,
    "debug", 0xffff, STYP_DEBUG,
    "typchk", 0xffff, STYP_TYPCHK,
    "ovrflo", 0xffff, STYP_OVRFLO,
    0
};

struct flags sym_flags[] = {
    "export", L_EXPORT, L_EXPORT,
    "entry", L_ENTRY, L_ENTRY,
    "import", L_IMPORT, L_IMPORT,
    "ER", 7, XTY_ER,
    "SD", 7, XTY_SD,
    "LD", 7, XTY_LD,
    "CM", 7, XTY_CM,
#ifdef XTY_EM
    "EM", 7, XTY_EM,
#endif
#ifdef XTY_US
    "US", 7, XTY_US,
#endif
#ifdef XTY_HL
    "HL", 7, XTY_HL,
#endif
    0
};

#ifndef XMC_TD
#define XMC_TD 16
#endif

struct flags store_flags[] = {
    "PR", 0xff, XMC_PR,
    "RO", 0xff, XMC_RO,
    "DB", 0xff, XMC_DB,
    "GL", 0xff, XMC_GL,
    "XO", 0xff, XMC_XO,
    "SV", 0xff, XMC_SV,
    "TI", 0xff, XMC_TI,
    "TB", 0xff, XMC_TB,
    "RW", 0xff, XMC_RW,
    "TC0", 0xff, XMC_TC0,
    "TC", 0xff, XMC_TC,
    "TD", 0xff, XMC_TD,
    "DS", 0xff, XMC_DS,
    "UA", 0xff, XMC_UA,
    "BS", 0xff, XMC_BS,
    "UC", 0xff, XMC_UC,
    0
};

struct flags rel_flags[] = {
    "POS", 0xff, R_POS,
    "NEG", 0xff, R_NEG,
    "REL", 0xff, R_REL,
    "TOC", 0xff, R_TOC,
    "TRL", 0xff, R_TRL,
    "TRLA", 0xff, R_TRLA,
    "GL", 0xff, R_GL,
    "TCL", 0xff, R_TCL,
    "RL", 0xff, R_RL,
    "RLA", 0xff, R_RLA,
    "REF", 0xff, R_REF,
    "BA", 0xff, R_BA,
    "RBA", 0xff, R_RBA,
    "RBAC", 0xff, R_RBAC,
    "BR", 0xff, R_BR,
    "RBR", 0xff, R_RBR,
    "RBRC", 0xff, R_RBRC,
    "RTB", 0xff, R_RTB,
    "RRTBI", 0xff, R_RRTBI,
    "RRTBA", 0xff, R_RRTBA,
    0
};

struct flags storage[] = {
    "C_EFCN", 0xff, C_EFCN,
    "C_NULL", 0xff, C_NULL,
    "C_AUTO", 0xff, C_AUTO,
    "C_EXT", 0xff, C_EXT,
    "C_STAT", 0xff, C_STAT,
    "C_REG", 0xff, C_REG,
    "C_EXTDEF", 0xff, C_EXTDEF,
    "C_LABEL", 0xff, C_LABEL,
    "C_ULABEL", 0xff, C_ULABEL,
    "C_MOS", 0xff, C_MOS,
    "C_ARG", 0xff, C_ARG,
    "C_STRTAG", 0xff, C_STRTAG,
    "C_MOU", 0xff, C_MOU,
    "C_UNTAG", 0xff, C_UNTAG,
    "C_TPDEF", 0xff, C_TPDEF,
    "C_USTATIC", 0xff, C_USTATIC,
    "C_ENTAG", 0xff, C_ENTAG,
    "C_MOE", 0xff, C_MOE,
    "C_REGPARM", 0xff, C_REGPARM,
    "C_FIELD", 0xff, C_FIELD,
    "C_BLOCK", 0xff, C_BLOCK,
    "C_FCN", 0xff, C_FCN,
    "C_EOS", 0xff, C_EOS,
    "C_FILE", 0xff, C_FILE,
    "C_LINE", 0xff, C_LINE,
    "C_ALIAS", 0xff, C_ALIAS,
    "C_HIDDEN", 0xff, C_HIDDEN,
    "C_HIDEXT", 0xff, C_HIDEXT,
    "C_BINCL", 0xff, C_BINCL,
    "C_EINCL", 0xff, C_EINCL,
    "C_GSYM", 0xff, C_GSYM,
    "C_LSYM", 0xff, C_LSYM,
    "C_PSYM", 0xff, C_PSYM,
    "C_RSYM", 0xff, C_RSYM,
    "C_RPSYM", 0xff, C_RPSYM,
    "C_STSYM", 0xff, C_STSYM,
    "C_TCSYM", 0xff, C_TCSYM,
    "C_BCOMM", 0xff, C_BCOMM,
    "C_ECOML", 0xff, C_ECOML,
    "C_ECOMM", 0xff, C_ECOMM,
    "C_DECL", 0xff, C_DECL,
    "C_ENTRY", 0xff, C_ENTRY,
    "C_FUN", 0xff, C_FUN,
    "C_BSTAT", 0xff, C_BSTAT,
    "C_ESTAT", 0xff, C_ESTAT,
    0
};

int indent;

int my_printf(char *fmt, ...)
{
    static col;
    char buffer[2048];
    char *s, *s_start;
    char c;
    va_list Argp;
    int printed = 0;
    int space_needed;
    int length;

    va_start(Argp, fmt);
    if (!strcmp(fmt, "%s")) {
	s_start = s = va_arg(Argp, char *);
	length = strlen(s);
    } else {
	vsprintf(buffer, fmt, Argp);
	length = strlen(buffer);
	s_start = s = buffer;
    }
    va_end(Argp);
    if (space_needed = (col != 0) && (*s != '\n'))
	++col;
    while (c = *s++) {
	if (c == '\n')
	    col = 0;
	else {
	    ++col;
	    if (col >= 79 && !printed && length < 80) {
		printf("\n%*s%s", indent, "", s_start);
		col = indent;		/* need to recaluclate col */
		s = s_start;
		printed = 1;
	    }
	}
    }

    if (!printed) {
	if (space_needed)
	    printf(" ");
	printf("%s", s_start);
    }
    return length;
}

char *print_hex(int size, ...)
{
    static char print_buf[20*32];
    static char *ptr = print_buf;
    char *ret = ptr;
    va_list Argp;
    /*
     * These are unsigned because I don't want sign extension when
     * they are promoted to int's and passed to sprintf.
     */
    unsigned char ac;
    unsigned short as;
    unsigned int ai;
    unsigned long long al;

    va_start(Argp, size);

    switch (size) {
    case sizeof(char):
	ac = va_arg(Argp, unsigned char);
	sprintf(ptr, "%0*x", size*2, ac);
	break;
    case sizeof(short):
	as = va_arg(Argp, unsigned short);
	sprintf(ptr, "%0*x", size*2, as);
	break;
    case sizeof(int):
	ai = va_arg(Argp, unsigned int);
	sprintf(ptr, "%0*x", size*2, ai);
	break;
    case sizeof(long long):
	al = va_arg(Argp, unsigned long long);
	sprintf(ptr, "%0*llx", size*2, al);
	break;
    default:
	fprintf(stderr, "Bad size in print_field %d\n", size);
	exit(1);
    }
    ptr += 20;
    if ((ptr + 20) > (print_buf + sizeof(print_buf)))
	ptr = print_buf;
    return ret;
}

#define PD(xxx) print_dec(sizeof(xxx), xxx)
#define PH(xxx) print_hex(sizeof(xxx), xxx)

char *print_dec(int size, ...)
{
    static char print_buf[20*32];
    static char *ptr = print_buf;
    char *ret = ptr;
    va_list Argp;
    /*
     * These are unsigned because I don't want sign extension when
     * they are promoted to int's and passed to sprintf.
     */
    unsigned char ac;
    unsigned short as;
    unsigned int ai;
    unsigned long long al;

    va_start(Argp, size);

    switch (size) {
    case sizeof(char):
	ac = va_arg(Argp, unsigned char);
	sprintf(ptr, "%d", ac);
	break;
    case sizeof(short):
	as = va_arg(Argp, unsigned short);
	sprintf(ptr, "%d", as);
	break;
    case sizeof(int):
	ai = va_arg(Argp, unsigned int);
	sprintf(ptr, "%d", ai);
	break;
    case sizeof(long long):
	al = va_arg(Argp, unsigned long long);
	sprintf(ptr, "%lld", al);
	break;
    default:
	fprintf(stderr, "Bad size in print_field %d\n", size);
	exit(1);
    }
    ptr += 20;
    if ((ptr + 20) > (print_buf + sizeof(print_buf)))
	ptr = print_buf;
    return ret;
}

static void print_flags(char *text, struct flags *f, int bits)
{
    char buffer[64];
    char *p = buffer;

    p += sprintf(p, "%s", text);
    while (f->f_name) {
	if ((bits & f->f_mask) == f->f_bits)
	    p += sprintf(p, " %s", f->f_name);
	++f;
    }
    my_printf("%s", buffer);
}

int main_64(caddr_t m)
{
    struct filehdr_64 *fhdr;
    char *dbx_strings = 0;
    struct typchk *tptr;		/* type check info */
    int t_cnt = 0;			/* type check count */
    time_t tmptime;
    int width;
    char *ct;

    fhdr = (struct filehdr_64 *)m;
    my_printf("Header info\n");
    tmptime = fhdr->f_timdat;
    ct = ctime(&tmptime);
    my_printf("magic:%s", PH(fhdr->f_magic));
    my_printf("sections:%s", PD(fhdr->f_nscns));
    my_printf("sym ptr/cnt:%s/%s", PH(fhdr->f_symptr), PD(fhdr->f_nsyms));
    my_printf("%.*s", strlen(ct)-1, ct);
    my_printf("opthdr:%s", PD(fhdr->f_opthdr));
    print_flags("flags:", header_flags, fhdr->f_flags);

    if (fhdr->f_opthdr) {
	struct aouthdr_64 *ohdr = (struct aouthdr_64 *)(m + sizeof(*fhdr));

	my_printf("\n\nOptional header info\n");
	my_printf("magic:%s", PH(ohdr->magic));
	my_printf("vstamp:%s", PD(ohdr->vstamp));
	my_printf("tsize:%s", PD(ohdr->tsize));
	my_printf("dsize:%s", PD(ohdr->dsize));
	my_printf("bsize:%s", PD(ohdr->bsize));
	my_printf("entry:%s", PH(ohdr->entry));
	my_printf("text_start:%s", PH(ohdr->text_start));
	my_printf("data_start:%s", PH(ohdr->data_start));
	my_printf("o_toc:%s", PH(ohdr->o_toc));
	my_printf("Section numbers: entry:%s", PD(ohdr->o_snentry));
	my_printf("text:%s", PD(ohdr->o_sntext));
	my_printf("data:%s", PD(ohdr->o_sndata));
	my_printf("toc:%s", PD(ohdr->o_sntoc));
	my_printf("loader:%s", PD(ohdr->o_snloader));
	my_printf("bss:%s", PD(ohdr->o_snbss));
	my_printf("algntext:%s", PH(ohdr->o_algntext));
	my_printf("algndata:%s", PH(ohdr->o_algndata));
	my_printf("modtype[0]:%s", PD(ohdr->o_modtype[0]));
	my_printf("modtype[1]:%s", PD(ohdr->o_modtype[1]));
	my_printf("maxstack:%s", PH(ohdr->o_maxstack));
	my_printf("maxdata:%s", PH(ohdr->o_maxdata));
    }

    if (fhdr->f_nscns) {
	struct scnhdr_64 *shdr =
	    (struct scnhdr_64 *)(m + sizeof(*fhdr) + fhdr->f_opthdr);
	int i;

	my_printf("\n\nSection headers\n");
	indent = sizeof(shdr->s_name) + 2;
	for (i = 0; i < fhdr->f_nscns; ++i, ++shdr) {
	    my_printf("%*.*s:", sizeof(shdr->s_name), sizeof(shdr->s_name),
		      shdr->s_name);
	    my_printf("paddr:%s", PH(shdr->s_paddr));
	    my_printf("vaddr:%s", PH(shdr->s_vaddr));
	    my_printf("size:%s", PH(shdr->s_size));
	    my_printf("scnptr:%s", PH(shdr->s_scnptr));
	    my_printf(" relptr:%s", PH(shdr->s_relptr));
	    my_printf("lnnoptr:%s", PH(shdr->s_lnnoptr));
	    my_printf("nreloc:%s", PD(shdr->s_nreloc));
	    my_printf("nlnno:%s", PD(shdr->s_nlnno));
	    print_flags("type:", section_flags, shdr->s_flags);
	    switch (shdr->s_flags & 0xffff) {
	    case STYP_DEBUG:
		dbx_strings = m + shdr->s_scnptr;
		break;
	    case STYP_TYPCHK:
		tptr = (struct typchk *)(m + shdr->s_scnptr);
		t_cnt = shdr->s_size / 12;
		break;
	    }
	    my_printf("\n");
	}
	indent = 0;
    }

    if (fhdr->f_opthdr) {
	struct aouthdr_64 *ohdr = (struct aouthdr_64 *)(m + sizeof(*fhdr));

	if (ohdr->o_snloader) {
	    struct scnhdr_64 *shdr =
		(struct scnhdr_64 *)(m + sizeof(*fhdr) + fhdr->f_opthdr);

	    shdr += (ohdr->o_snloader - 1);
	    if (shdr->s_scnptr) {
		struct ldhdr_64 *lhdr = (struct ldhdr_64 *)(m+shdr->s_scnptr);
		int i;
		char *strings = (char *)lhdr + lhdr->l_stoff;
		char *ftable = (char *)lhdr + lhdr->l_impoff;
		struct ldrel_64 *lrel = (struct ldrel_64 *)((long long)lhdr +
							    lhdr->l_rldoff);
		struct ldsym_64 *lsym = (struct ldsym_64 *)((long long)lhdr +
							    lhdr->l_symoff);

		my_printf("\n\nLoader Section\n");
		my_printf("version:%s", PD(lhdr->l_version));
		my_printf("nsyms:%s", PD(lhdr->l_nsyms));
		my_printf("nreloc:%s", PD(lhdr->l_nreloc));
		my_printf("istlen:%s", PD(lhdr->l_istlen));
		my_printf("nimpid:%s", PD(lhdr->l_nimpid));
		my_printf("stlen:%s", PD(lhdr->l_stlen));
		my_printf("impoff:%s", PH(lhdr->l_impoff));
		my_printf("stoff:%s", PH(lhdr->l_stoff));
		my_printf("symoff:%s", PH(lhdr->l_symoff));
		my_printf("rldoff:%s", PH(lhdr->l_rldoff));
		
		my_printf("\n\nLoader Import File Table\n");
		for (i = 0; i < lhdr->l_nimpid; ++i) {
		    my_printf("[%s]Path: %s", PD(i), ftable);
		    ftable += strlen(ftable) + 1;
		    my_printf("Base: %s", ftable);
		    ftable += strlen(ftable) + 1;
		    my_printf("Member: (%s)\n", ftable);
		    ftable += strlen(ftable) + 1;
		}

		for (i = lhdr->l_nsyms, width = 1; i > 10; i /= 10, ++width);
		indent = width + 3;
		my_printf("\nLoader Symbol entries\n");
		for (i = 0; i < lhdr->l_nsyms; ++i, ++lsym) {
		    my_printf("[%*s]", width, PD(i));
		    my_printf("%s", strings + lsym->l_offset);
		    my_printf("value:%s", PH(lsym->l_value));
		    my_printf("scnum:%s", PD(lsym->l_scnum));
		    print_flags("flags:", sym_flags, lsym->l_smtype);
		    my_printf("ifile:%s", PD(lsym->l_ifile));
		    my_printf("parm:%s", PD(lsym->l_parm));
		    print_flags("storage class:", store_flags, lsym->l_smclas);
		    my_printf("\n");
		}
		indent = 0;

		my_printf("\nLoader Relocation entries\n");
		for (i = 0; i < lhdr->l_nreloc; ++i, ++lrel) {
		    my_printf("vaddr:%s", PH(lrel->l_vaddr));
		    my_printf("symndx:%s", PD(lrel->l_symndx));
		    my_printf("rtype:%s", PH(lrel->l_rtype));
		    my_printf("rsecnm:%s\n", PD(lrel->l_rsecnm));
		}
	    }
	}
    }

    if (t_cnt) {
	int i;

	my_printf("\n\nType Check Section\n");
	for (i = 0; i < t_cnt; ++tptr)
	    my_printf("[%s]%s %s %s\n", PD(++i), lang[tptr->t_lang],
		      PH(tptr->t_ghash), PH(tptr->t_lhash));
    }

    /* TODO except section */
    
    /* Relocation information */
    if (fhdr->f_nscns) {
	struct scnhdr_64 *shdr =
	    (struct scnhdr_64 *)(m + sizeof(*fhdr) + fhdr->f_opthdr);
	int i;

	for (i = 0; i < fhdr->f_nscns; ++i, ++shdr) {
	    if (shdr->s_relptr) {
		int j;
		caddr_t base = m + shdr->s_relptr;
		
		my_printf("\n\nRelocation entries for %.8s section\n",
			  shdr->s_name);
		for (j = 0; j < shdr->s_nreloc; ++j, base += RELSZ_64) {
		    struct reloc_64 *r = (struct reloc_64 *)base;
		    
		    my_printf("vaddr:%s", PH(r->r_vaddr));
		    my_printf("symndx:%s", PD(r->r_symndx));
		    my_printf("sign:%s", PD(RELOC_RSIGN(*r) ? 1 : 0));
		    my_printf("len:%s", PD(RELOC_RLEN(*r) + 1));
		    print_flags("rtype", rel_flags, RELOC_RTYPE(*r));
		    my_printf("\n");
		}
	    }
	}
    }
    
    /* Line number information */
    if (fhdr->f_nscns) {
	struct scnhdr_64 *shdr =
	    (struct scnhdr_64 *)(m + sizeof(*fhdr) + fhdr->f_opthdr);
	int i;

	for (i = 0; i < fhdr->f_nscns; ++i, ++shdr) {
	    if (shdr->s_lnnoptr) {
		int j;
		caddr_t base = m + shdr->s_lnnoptr;
		
		my_printf("\n\nLine number entries for %.8s section\n",
			  shdr->s_name);
		for (j = 0; j < shdr->s_nlnno; ++j, base += LINESZ_64) {
		    struct lineno_64 *l = (struct lineno_64 *)base;
		    
		    if (l->l_lnno)
			my_printf("paddr:%s lnno:%s\n", PH(l->l_addr.l_paddr),
				  PD(l->l_lnno));
		    else
			my_printf("symndx:%s\n", PD(l->l_addr.l_symndx));
		}
	    }
	}
    }
    
    /* Symbol table */
    if (fhdr->f_symptr) {
	caddr_t base = m + fhdr->f_symptr;
	char *strings = base + (fhdr->f_nsyms * SYMESZ);
	int tab = 3;			/* number of columes ([%d], i) takes */
	int next_tab = 10;
	int i;

	for (i = fhdr->f_nsyms, width = 1; i > 10; i /= 10, ++width);
	indent = width + 3;

	my_printf("\nSymbol table\n");
	for (i = 0; i < fhdr->f_nsyms; ) {
	    struct syment_64 *s = (struct syment_64 *)base;
	    int j;

	    my_printf("[%*s]", width, PD(i));
	    if (s->n_sclass & DBXMASK)
		my_printf("%s", dbx_strings + s->n_offset);
	    else
		my_printf("%s", strings + s->n_offset);
	    my_printf("value:%s", PH(s->n_value));
	    my_printf("scnum:%s", PD(s->n_scnum));
	    my_printf("type:%s", PH(s->n_type));
	    my_printf("numaux:%s", PD(s->n_numaux));
	    print_flags("sclass:", storage, s->n_sclass);
	    my_printf("\n");

	    base += SYMESZ_64;
	    ++i;
	    for (j = 1; j <= s->n_numaux; ++j) {
		union auxent_64 *a = (union auxent_64 *)base;
		
		my_printf("{%*s}", width, PD(i));
		switch (s->n_sclass) {
		case C_BLOCK:
		case C_FCN:
		    my_printf("lnno:%s", PD(a->x_sym.x_misc.x_lnsz.x_lnno));
		    my_printf("auxtype:%s", PH(a->x_sym.x_auxtype));
		    break;

		case C_EXT:
		case C_HIDEXT:
		    if (j == s->n_numaux) {
			unsigned long long scnlen =
			    ((a->x_csect.x_scnlen_hi << 32) |
			     a->x_csect.x_scnlen_lo);

			if ((a->x_csect.x_smtyp & 7) == XTY_LD)
			    my_printf("csect:%s", PD(scnlen));
			else
			    my_printf("scnlen:%s", PH(scnlen));
			my_printf("parmhash:%s",
				  PD((a->x_csect.x_parmhash +
				      sizeof(struct typchk) - 2) /
				     sizeof(struct typchk)));
			my_printf("snhash:%s", PD(a->x_csect.x_snhash));
			my_printf("smalgn:2^%s",
				  PD((a->x_csect.x_smtyp >> 3) & 0x1f));
			print_flags("smtype", sym_flags,
				    a->x_csect.x_smtyp & 7);
			print_flags("storage class", store_flags,
				    a->x_csect.x_smclas);
		    } else {
			my_printf("lnno:%s",
				  PD(a->x_sym.x_misc.x_lnsz.x_lnno));
			my_printf("lnnoptr:%s",
				  PH(a->x_fcn.x_lnnoptr));
			my_printf("endndx:%s",
				  PD(a->x_fcn.x_endndx));
		    }
		    break;

		case C_FILE:
		    if (a->x_file._x.x_zeroes)
			my_printf("%.*s", sizeof(a->x_file.x_fname),
				  a->x_file.x_fname);
		    else
			my_printf("%s", strings + a->x_file._x.x_offset);
		    my_printf("ftype:%s", PD(a->x_file._x.x_ftype));
		    break;

		default:
		    my_printf("word0:%s", PH(((int *)a)[0]));
		    my_printf("word1:%s", PH(((int *)a)[1]));
		    my_printf("word2:%s", PH(((int *)a)[2]));
		    my_printf("word3:%s", PH(((int *)a)[3]));
		    my_printf("short:%s", PH(((short *)a)[8]));
		    break;
		}
		my_printf("\n");
		base += AUXESZ_64;
		++i;
	    }
	}
    }
    return 0;
}

int main(int argc, char *argv[])
{
    int fd;
    struct stat sbuf;
    caddr_t m;
    struct filehdr *fhdr;
    char *dbx_strings = 0;
    struct typchk *tptr;		/* type check info */
    int t_cnt = 0;			/* type check count */
    time_t tmptime;
    char *ct;
    int width;

    if (prog = rindex(argv[0], '/'))
	++prog;
    else
	prog = argv[0];

    if (argc != 2) {
	fprintf(stderr, "Usage: %s <filename>\n", prog);
	return 1;
    }
    
    if ((fd = open(argv[1], O_RDONLY)) < 0) {
	fprintf(stderr, "%s: ", prog);
	perror(argv[1]);
	return 1;
    }
    if (fstat(fd, &sbuf) < 0) {
	fprintf(stderr, "%s: ", prog);
	perror("stat");
	return 1;
    }
    m = mmap(0, sbuf.st_size, PROT_READ, MAP_FILE|MAP_VARIABLE, fd, 0);
    if ((int)m == -1) {
	fprintf(stderr, "%s: ", prog);
	perror("mmap");
	return 1;
    }

    fhdr = (struct filehdr *)m;
#ifdef U64_TOCMAGIC
    if (fhdr->f_magic == U64_TOCMAGIC)
	return main_64(m);
#endif

    my_printf("Header info\n");
    tmptime = fhdr->f_timdat;
    ct = ctime(&tmptime);
    my_printf("magic:%s", PH(fhdr->f_magic));
    my_printf("sections:%s", PD(fhdr->f_nscns));
    my_printf("sym ptr/cnt:%s/%s", PH(fhdr->f_symptr), PD(fhdr->f_nsyms));
    my_printf("%.*s", strlen(ct)-1, ct);
    my_printf("opthdr:%s", PD(fhdr->f_opthdr));
    print_flags("flags:", header_flags, fhdr->f_flags);

    if (fhdr->f_opthdr) {
	struct aouthdr *ohdr = (struct aouthdr *)(m + sizeof(*fhdr));

	my_printf("\n\nOptional header info\n");
	my_printf("magic:%s", PH(ohdr->magic));
	my_printf("vstamp:%s", PD(ohdr->vstamp));
	my_printf("tsize:%s", PD(ohdr->tsize));
	my_printf("dsize:%s", PD(ohdr->dsize));
	my_printf("bsize:%s", PD(ohdr->bsize));
	my_printf("entry:%s", PH(ohdr->entry));
	my_printf("text_start:%s", PH(ohdr->text_start));
	my_printf("data_start:%s", PH(ohdr->data_start));
	my_printf("o_toc:%s", PH(ohdr->o_toc));
	my_printf("Section numbers: entry:%s", PD(ohdr->o_snentry));
	my_printf("text:%s", PD(ohdr->o_sntext));
	my_printf("data:%s", PD(ohdr->o_sndata));
	my_printf("toc:%s", PD(ohdr->o_sntoc));
	my_printf("loader:%s", PD(ohdr->o_snloader));
	my_printf("bss:%s", PD(ohdr->o_snbss));
	my_printf("algntext:%s", PH(ohdr->o_algntext));
	my_printf("algndata:%s", PH(ohdr->o_algndata));
	my_printf("modtype[0]:%s", PD(ohdr->o_modtype[0]));
	my_printf("modtype[1]:%s", PD(ohdr->o_modtype[1]));
	my_printf("maxstack:%s", PH(ohdr->o_maxstack));
	my_printf("maxdata:%s", PH(ohdr->o_maxdata));
    }

    if (fhdr->f_nscns) {
	struct scnhdr *shdr =
	    (struct scnhdr *)(m + sizeof(*fhdr) + fhdr->f_opthdr);
	int i;

	my_printf("\n\nSection headers\n");
	indent = sizeof(shdr->s_name) + 2;
	for (i = 0; i < fhdr->f_nscns; ++i, ++shdr) {
	    my_printf("%*.*s:", sizeof(shdr->s_name), sizeof(shdr->s_name),
		      shdr->s_name);
	    my_printf("paddr:%s", PH(shdr->s_paddr));
	    my_printf("vaddr:%s", PH(shdr->s_vaddr));
	    my_printf("size:%s", PH(shdr->s_size));
	    my_printf("scnptr:%s", PH(shdr->s_scnptr));
	    my_printf(" relptr:%s", PH(shdr->s_relptr));
	    my_printf("lnnoptr:%s", PH(shdr->s_lnnoptr));
	    my_printf("nreloc:%s", PD(shdr->s_nreloc));
	    my_printf("nlnno:%s", PD(shdr->s_nlnno));
	    print_flags("type:", section_flags, shdr->s_flags);
	    switch (shdr->s_flags & 0xffff) {
	    case STYP_DEBUG:
		dbx_strings = m + shdr->s_scnptr;
		break;
	    case STYP_TYPCHK:
		tptr = (struct typchk *)(m + shdr->s_scnptr);
		t_cnt = shdr->s_size / 12;
		break;
	    }
	    my_printf("\n");
	}
	indent = 0;
    }

    if (fhdr->f_opthdr) {
	struct aouthdr *ohdr = (struct aouthdr *)(m + sizeof(*fhdr));

	if (ohdr->o_snloader) {
	    struct scnhdr *shdr =
		(struct scnhdr *)(m + sizeof(*fhdr) + fhdr->f_opthdr);

	    shdr += (ohdr->o_snloader - 1);
	    if (shdr->s_scnptr) {
		struct ldhdr *lhdr = (struct ldhdr *)(m+shdr->s_scnptr);
		int i;
		struct ldsym *lsym = (struct ldsym *)(lhdr + 1);
		char *strings = (char *)lhdr + lhdr->l_stoff;
		char *ftable = (char *)lhdr + lhdr->l_impoff;
		struct ldrel *lrel;

		my_printf("\n\nLoader Section\n");
		my_printf("version:%s", PD(lhdr->l_version));
		my_printf("nsyms:%s", PD(lhdr->l_nsyms));
		my_printf("nreloc:%s", PD(lhdr->l_nreloc));
		my_printf("istlen:%s", PD(lhdr->l_istlen));
		my_printf("nimpid:%s", PD(lhdr->l_nimpid));
		my_printf("impoff:%s", PH(lhdr->l_impoff));
		my_printf("stlen:%s", PD(lhdr->l_stlen));
		my_printf("stoff:%s", PH(lhdr->l_stoff));
		
		my_printf("\n\nLoader Import File Table\n");
		for (i = 0; i < lhdr->l_nimpid; ++i) {
		    my_printf("[%s]Path: %s", PD(i), ftable);
		    ftable += strlen(ftable) + 1;
		    my_printf("Base: %s", ftable);
		    ftable += strlen(ftable) + 1;
		    my_printf("Member: (%s)\n", ftable);
		    ftable += strlen(ftable) + 1;
		}

		for (i = lhdr->l_nsyms, width = 1; i > 10; i /= 10, ++width);
		indent = width + 3;
		my_printf("\nLoader Symbol entries\n");
		for (i = 0; i < lhdr->l_nsyms; ++i, ++lsym) {
		    my_printf("[%*s]", width, PD(i));
		    if (lsym->l_zeroes)
			my_printf("%.*s", sizeof(lsym->l_name), lsym->l_name);
		    else
			my_printf("%s", strings + lsym->l_offset);
		    my_printf("value:%s", PH(lsym->l_value));
		    my_printf("scnum:%s", PD(lsym->l_scnum));
		    print_flags("flags:", sym_flags, lsym->l_smtype);
		    my_printf("ifile:%s", PD(lsym->l_ifile));
		    my_printf("parm:%s", PD(lsym->l_parm));
		    print_flags("storage class:", store_flags, lsym->l_smclas);
		    my_printf("\n");
		}
		indent = 0;

		my_printf("\nLoader Relocation entries\n");
		lrel = (struct ldrel *)lsym;
		for (i = 0; i < lhdr->l_nreloc; ++i, ++lrel) {
		    my_printf("vaddr:%s", PH(lrel->l_vaddr));
		    my_printf("symndx:%s", PD(lrel->l_symndx));
		    my_printf("rtype:%s", PH(lrel->l_rtype));
		    my_printf("rsecnm:%s\n", PD(lrel->l_rsecnm));
		}
	    }
	}
    }

    if (t_cnt) {
	int i;

	my_printf("\n\nType Check Section\n");
	for (i = 0; i < t_cnt; ++tptr)
	    my_printf("[%s]%s %s %s\n", PD(++i), lang[tptr->t_lang],
		      PH(tptr->t_ghash), PH(tptr->t_lhash));
    }

    /* TODO except section */
    
    /* Relocation information */
    if (fhdr->f_nscns) {
	struct scnhdr *shdr =
	    (struct scnhdr *)(m + sizeof(*fhdr) + fhdr->f_opthdr);
	int i;

	for (i = 0; i < fhdr->f_nscns; ++i, ++shdr) {
	    if (shdr->s_relptr) {
		int j;
		caddr_t base = m + shdr->s_relptr;
		
		my_printf("\n\nRelocation entries for %.8s section\n",
			  shdr->s_name);
		for (j = 0; j < shdr->s_nreloc; ++j, base += RELSZ) {
		    struct reloc *r = (struct reloc *)base;
		    
		    my_printf("vaddr:%s", PH(r->r_vaddr));
		    my_printf("symndx:%s", PD(r->r_symndx));
		    my_printf("sign:%s", PD(RELOC_RSIGN(*r) ? 1 : 0));
		    my_printf("len:%s", PD(RELOC_RLEN(*r) + 1));
		    print_flags("rtype", rel_flags, RELOC_RTYPE(*r));
		    my_printf("\n");
		}
	    }
	}
    }
    
    /* Line number information */
    if (fhdr->f_nscns) {
	struct scnhdr *shdr =
	    (struct scnhdr *)(m + sizeof(*fhdr) + fhdr->f_opthdr);
	int i;

	for (i = 0; i < fhdr->f_nscns; ++i, ++shdr) {
	    if (shdr->s_lnnoptr) {
		int j;
		caddr_t base = m + shdr->s_lnnoptr;
		
		my_printf("\n\nLine number entries for %.8s section\n",
			  shdr->s_name);
		for (j = 0; j < shdr->s_nlnno; ++j, base += LINESZ) {
		    struct lineno *l = (struct lineno *)base;
		    
		    if (l->l_lnno)
			my_printf("paddr:%s lnno:%s\n", PH(l->l_addr.l_paddr),
				  PD(l->l_lnno));
		    else
			my_printf("symndx:%s\n", PD(l->l_addr.l_symndx));
		}
	    }
	}
    }
    
    /* Symbol table */
    if (fhdr->f_symptr) {
	caddr_t base = m + fhdr->f_symptr;
	char *strings = base + (fhdr->f_nsyms * SYMESZ);
	int tab = 3;			/* number of columes ([%d], i) takes */
	int next_tab = 10;
	int i;

	for (i = fhdr->f_nsyms, width = 1; i > 10; i /= 10, ++width);
	indent = width + 3;

	my_printf("\nSymbol table\n");
	for (i = 0; i < fhdr->f_nsyms; ) {
	    struct syment *s = (struct syment *)base;
	    int j;

	    my_printf("[%*s]", width, PD(i));
	    if (s->n_zeroes)
		my_printf("%.*s", sizeof(s->n_name), s->n_name);
	    else if (s->n_sclass & DBXMASK)
		my_printf("%s", dbx_strings + s->n_offset);
	    else
		my_printf("%s", strings + s->n_offset);
	    my_printf("value:%s", PH(s->n_value));
	    my_printf("scnum:%s", PD(s->n_scnum));
	    my_printf("type:%s", PH(s->n_type));
	    my_printf("numaux:%s", PD(s->n_numaux));
	    print_flags("sclass:", storage, s->n_sclass);
	    my_printf("\n");

	    base += SYMESZ;
	    ++i;
	    for (j = 1; j <= s->n_numaux; ++j) {
		union auxent *a = (union auxent *)base;
		
		my_printf("{%*s}", width, PD(i));
		switch (s->n_sclass) {
		case C_BLOCK:
		case C_FCN:
		    my_printf("tagndx:%s", PH(a->x_sym.x_tagndx));
		    my_printf("lnno:%s", PD(a->x_sym.x_misc.x_lnsz.x_lnno));
		    my_printf("size:%s", PD(a->x_sym.x_misc.x_lnsz.x_size));
		    my_printf("lnnoptr:%s",
			      PH(a->x_sym.x_fcnary.x_fcn.x_lnnoptr));
		    my_printf("endndx:%s",
			      PD(a->x_sym.x_fcnary.x_fcn.x_endndx));
		    my_printf("tvndx:%s", PH(a->x_sym.x_tvndx));
		    break;

		case C_STAT:
		    my_printf("scnlen:%s", PH(a->x_scn.x_scnlen));
		    my_printf("nreloc:%s", PD(a->x_scn.x_nreloc));
		    my_printf("nlinno:%s", PD(a->x_scn.x_nlinno));
		    break;

		case C_EXT:
		case C_HIDEXT:
		    if (j == s->n_numaux) {
			if ((a->x_csect.x_smtyp & 7) == XTY_LD)
			    my_printf("csect:%s", PD(a->x_csect.x_scnlen));
			else
			    my_printf("scnlen:%s", PH(a->x_csect.x_scnlen));
			my_printf("parmhash:%s",
				  PD((a->x_csect.x_parmhash +
				      sizeof(struct typchk) - 2) /
				     sizeof(struct typchk)));
			my_printf("snhash:%s", PD(a->x_csect.x_snhash));
			my_printf("smalgn:2^%s",
				  PD((a->x_csect.x_smtyp >> 3) & 0x1f));
			print_flags("symtype", sym_flags,
				    a->x_csect.x_smtyp & 7);
			my_printf("%*sstab:%s", tab, "",
				  PH(a->x_csect.x_stab));
			my_printf("snstab:%s",
				  PD(a->x_csect.x_snstab));
			print_flags("storage class", store_flags,
				    a->x_csect.x_smclas);
		    } else {
			my_printf("tagndx:%s", PH(a->x_sym.x_tagndx));
			my_printf("lnno:%s",
				  PD(a->x_sym.x_misc.x_lnsz.x_lnno));
			my_printf("size:%s",
				  PD(a->x_sym.x_misc.x_lnsz.x_size));
			my_printf("lnnoptr:%s",
				  PH(a->x_sym.x_fcnary.x_fcn.x_lnnoptr));
			my_printf("endndx:%s",
				  PD(a->x_sym.x_fcnary.x_fcn.x_endndx));
			my_printf("tvndx:%s", PH(a->x_sym.x_tvndx));
		    }
		    break;

		case C_FILE:
		    if (a->x_file._x.x_zeroes)
			my_printf("%.*s", sizeof(a->x_file.x_fname),
				  a->x_file.x_fname);
		    else
			my_printf("%s", strings + a->x_file._x.x_offset);
		    my_printf("ftype:%s", PD(a->x_file._x.x_ftype));
		    break;

		default:
		    my_printf("word0:%s", PH(((int *)a)[0]));
		    my_printf("word1:%s", PH(((int *)a)[1]));
		    my_printf("word2:%s", PH(((int *)a)[2]));
		    my_printf("word3:%s", PH(((int *)a)[3]));
		    my_printf("short:%s", PH(((short *)a)[8]));
		    break;
		}
		my_printf("\n");
		base += AUXESZ;
		++i;
	    }
	}
    }
    return 0;
}
E 1
