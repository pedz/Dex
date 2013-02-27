h00588
s 00073/00014/01085
d D 1.11 10/08/23 17:01:53 pedzan 11 10
e
s 00155/00101/00944
d D 1.10 02/03/14 16:12:33 pedz 10 9
e
s 00001/00000/01044
d D 1.9 00/09/18 17:57:01 pedz 9 8
c Checking before V5 conversion
e
s 00001/00001/01043
d D 1.8 00/05/29 15:09:07 pedz 8 7
c Just before IA64 conversion
e
s 00038/00011/01006
d D 1.7 00/02/08 17:36:47 pedz 7 6
e
s 00087/00009/00930
d D 1.6 98/10/23 12:26:10 pedz 6 5
e
s 00034/00031/00905
d D 1.5 97/05/13 16:02:57 pedz 5 4
c Check pointer
e
s 00114/00034/00822
d D 1.4 95/05/17 16:39:10 pedz 4 3
c Check point
e
s 00046/00012/00810
d D 1.3 95/04/25 10:10:12 pedz 3 2
e
s 00030/00008/00792
d D 1.2 95/02/09 21:35:48 pedz 2 1
e
s 00800/00000/00000
d D 1.1 95/02/01 10:57:37 pedz 1 0
c date and time created 95/02/01 10:57:37 by pedz
e
u
U
t
T
I 1
static char sccs_id[] = "%W%";

I 10
#ifdef __64BIT__
#define __XCOFF64__
#endif

E 10
#include <a.out.h>
D 10
#include <string.h>
E 10
I 10
#include <strings.h>
E 10
#include <stdio.h>
#include <fcntl.h>
I 3
D 10

/*
 * I would prefer to use mmap but it has a bug in it right now that
 * shmat does not have so I'll use shmat for now.
 */
#define USE_MMAP
#ifdef USE_MMAP
E 10
E 3
#include <sys/mman.h>
#include <sys/stat.h>
I 3
D 10
#else
#include <sys/shm.h>
#endif
E 10
I 10
#include <stdlib.h>
E 10

E 3
#include "map.h"
#include "sym.h"
#include "dex.h"
I 10
#include "tree.h"
#include "stab_pre.h"
E 10

I 10
#define DEBUG_BIT LOAD_C_BIT

E 10
/*
 * This file implements the "load" command which loads in a object
 * file or an executable and sucks out all of the information that it
 * possibly can from it.  Currently I hope to get the symbol table if
 * it is present.  If it is not present then I will get the loader
 * information.  I will also suck in all of the debug information if
 * it is present including the file names and line number.  I do not
 * think I will use the typcheck information or the exception section.
 * I have hopes eventually of using the relocation section to that a
 * positive confirmation can be made that this particular file matches
 * what was loaded in the system.  That may be a bit tricky though.
 */
#define MAP(class, type) (((class) << 3) | ((type) & 7))

I 11
static typeptr get_char(ns *nspace, typeptr *int_type_p)
{
    typeptr ret;

    if (ret = *int_type_p)
	return ret;
    ret = *int_type_p = name2typedef(nspace, "char");
    return ret;
}

static typeptr get_short(ns *nspace, typeptr *int_type_p)
{
    typeptr ret;

    if (ret = *int_type_p)
	return ret;
    ret = *int_type_p = name2typedef(nspace, "short");
    return ret;
}

E 11
static typeptr get_int(ns *nspace, typeptr *int_type_p)
{
    typeptr ret;

    if (ret = *int_type_p)
	return ret;
    ret = *int_type_p = name2typedef(nspace, "int");
    return ret;
}

I 10
/*
 * A "long" in this particular case is always 64 bits.  What we do is
 * look up "long" and get its size.  If it is only 32 bits, we look up
 * "long long" and use it.
 */
static typeptr get_long(ns *nspace, typeptr *long_type_p)
{
    typeptr ret;

    if (ret = *long_type_p)
	return ret;
    if (get_size(ret = name2typedef(nspace, "long")) < 64)
	ret = name2typedef(nspace, "long long");
    *long_type_p = ret;
    return ret;
}

E 10
static typeptr get_func_int(ns *nspace,
			    typeptr *int_type_p,
			    typeptr *func_int_type_p)
{
    typeptr ret;

    if (ret = *func_int_type_p)
	return ret;
    ret = *func_int_type_p = newtype(nspace, PROC_TYPE);
    ret->t_val.val_f.f_typeptr = get_int(nspace, int_type_p);
    ret->t_val.val_f.f_params = -1;
    ret->t_val.val_f.f_paramlist = 0;
    return ret;
}
D 10

static typeptr get_ptr_int(ns *nspace,
			   typeptr *int_type_p,
			   typeptr *ptr_int_type_p)
{
    typeptr ret;
E 10
    
D 10
    if (ret = *ptr_int_type_p)
	return ret;
    ret = *ptr_int_type_p = newtype(nspace, PTR_TYPE);
    ret->t_val.val_p = get_int(nspace, int_type_p);
    return ret;
}
    
E 10
static typeptr get_ptr_func_int(ns *nspace,
				typeptr *int_type_p,
				typeptr *func_int_type_p,
				typeptr *ptr_func_int_type_p)
{
    typeptr ret;

    if (ret = *ptr_func_int_type_p)
	return ret;
    ret = *ptr_func_int_type_p = newtype(nspace, PTR_TYPE);
    ret->t_val.val_p = get_func_int(nspace, int_type_p, func_int_type_p);
    return ret;
}

I 4
struct for_ref {
    struct for_ref *fr_next;
    int fr_index;
    symptr fr_sym;
};

I 6
static char *stab_buf;
D 10
static int stab_buf_size;
E 10
I 10
static size_t stab_buf_size;
E 10
E 6

I 6
static void add_to_buf(char *s, int size)
{
D 10
    int new_stab_buf_size = strlen(stab_buf) + size + 1;
E 10
I 10
    size_t new_stab_buf_size = strlen(stab_buf) + size + 1;
E 10

    if (new_stab_buf_size >= stab_buf_size)
	if (stab_buf)
D 10
	    stab_buf = srealloc(stab_buf, new_stab_buf_size);
E 10
I 10
	    stab_buf = srealloc(stab_buf, new_stab_buf_size, stab_buf_size);
E 10
	else
	    stab_buf = smalloc(new_stab_buf_size);
    strncat(stab_buf, s, size);
    if (stab_buf[new_stab_buf_size - 2] == '?')
	stab_buf[new_stab_buf_size - 2] = 0;
    else
	stab_buf[new_stab_buf_size - 1] = 0;
}

E 6
E 4
D 8
void load(char *path, int text_base, int data_base)
E 8
I 8
D 10
void load(char *path, long text_base, long data_base)
E 10
I 10
static void resolv_dup(symptr *sym_p, char **name_p, ns *ns, void *offset)
E 10
E 8
{
I 10
    symptr sym = *sym_p;

    if ((sym = *sym_p)->s_defined && sym->s_offset != offset) {
	char *name = *name_p;
	char buf[16];
	
	sprintf(buf, "$dup%d", ns->ns_lastdup++);
	name = store_string(ns, name, 0, buf);
	sym = enter_sym(ns, name, 0);

	*sym_p = sym;
	*name_p = name;
    }
}

int load(char *path, long text_base, long data_base)
{
E 10
    int fd;
D 3
    struct stat sbuf;
E 3
    caddr_t m;
    struct filehdr *fhdr;
    char *dbx_strings = 0;
    struct typchk *tptr;		/* type check info */
    int t_cnt = 0;			/* type check count */
    struct aouthdr *ohdr;
    ns *load_ns;
    char *name;
    int size;
    long lnnoptr_base;
I 11
    typeptr char_type = 0;
    typeptr short_type = 0;
E 11
    typeptr int_type = 0;
D 10
    typeptr ptr_int_type = 0;
E 10
I 10
    typeptr long_type = 0;
E 10
    typeptr func_int_type = 0;
    typeptr ptr_func_int_type = 0;
    char *suffix;
    typeptr thistype;
    int haddot;
I 6
    int have_stab_buf = 0;
E 6
I 3
D 10
#ifdef USE_MMAP
E 10
    struct stat sbuf;
D 10
#endif
E 10
E 3

I 10
    DEBUG_PRINTF(("load: called with %s 0x%s 0x%s\n",
		  path, P(text_base), P(data_base)));

E 10
    if ((fd = open(path, O_RDONLY)) < 0) {
	perror(path);
D 10
	return;
E 10
I 10
	return -1;
E 10
    }
I 3

D 10
#ifdef USE_MMAP
E 10
E 3
    if (fstat(fd, &sbuf) < 0) {
	perror("stat");
	close(fd);
D 10
	return;
E 10
I 10
	return -1;
E 10
    }
D 4
    m = mmap(0x40000000, sbuf.st_size, PROT_READ, MAP_FILE|MAP_VARIABLE, fd, 0);
E 4
I 4
D 5
    m = mmap((void *)0x40000000, sbuf.st_size, PROT_READ,
E 5
I 5
D 10
    m = mmap(LOAD_BASE, sbuf.st_size, PROT_READ,
E 10
I 10
    /*
     * This is done as MAP_VARIABLE because when the unix file is
     * loaded, map_top has not been set yet.  It really does not
     * matter where the file is mapped because it is unmapped after we
     * are done and we do not do any other mapping while it is
     * mapped.
     */
I 11
    if (debug_mask & MMAP_BIT)
	printf("mmap: s=%s l=%s e=%s\n", P(map_top),
	       P(sbuf.st_size),
	       P(map_top + sbuf.st_size));
E 11
    m = mmap((void *)map_top, sbuf.st_size, PROT_READ,
E 10
E 5
	     MAP_FILE|MAP_VARIABLE, fd, 0);
E 4
D 10
    if ((int)m == -1) {
E 10
I 10
    DEBUG_PRINTF(("load: map_top=0x%s m=0x%s\n", P(map_top), P(m)));

    if (m == (caddr_t)-1) {
E 10
D 5
	perror("mmap");
E 5
I 5
	perror("mmap3");
E 5
	close(fd);
D 10
	return;
E 10
I 10
	return -1;
E 10
    }
I 3
D 10
#else
    m = shmat(fd, 0x40000000, SHM_MAP|SHM_RDONLY);
    if ((int)m == -1) {
	perror("shmat");
	close(fd);
	return;
    }
#endif
E 10
I 10
    DEBUG_PRINTF(("load: m=%s\n", P(m)));
E 10

E 3
    load_ns = ns_create((ns *)0, path);
    load_base_types(load_ns);
    load_ns->ns_text_start = text_base;
    load_ns->ns_data_start = data_base;
    int_type = name2typedef(load_ns, "int");

    fhdr = (struct filehdr *)m;

    if (fhdr->f_opthdr)
	ohdr = (struct aouthdr *)(m + sizeof(struct filehdr));
    else
	ohdr = 0;

    if (fhdr->f_nscns) {
	struct scnhdr *shdr =
	    (struct scnhdr *)(m + sizeof(struct filehdr) + fhdr->f_opthdr);
	int i;

I 10
	DEBUG_PRINTF(("load: doing %d sections\n", fhdr->f_nscns));
E 10
	for (i = 0; i < fhdr->f_nscns; ++i, ++shdr) {
	    switch (shdr->s_flags & 0xffff) {
	    case STYP_TEXT:
		load_ns->ns_text_size = shdr->s_size;
		if (lnnoptr_base = shdr->s_lnnoptr) {
		    int cnt = shdr->s_nlnno;
D 10
		    int size = sizeof(LINENO) * cnt;
E 10
I 10
		    size_t size = sizeof(LINENO) * cnt;
E 10
		    caddr_t from, to;

D 5
		    load_ns->ns_lines = smalloc(size, __FILE__, __LINE__);
E 5
I 5
		    load_ns->ns_lines = smalloc(size);
E 5
		    from = m + shdr->s_lnnoptr;
		    to = (caddr_t)load_ns->ns_lines;
I 4
D 10
		    bzero(to, size);
E 10
E 4
		    while (--cnt >= 0) {
I 5
			struct lineno *l = (struct lineno *)to;

E 5
			bcopy(from, to, LINESZ);
I 5
			l->l_addr.l_paddr += text_base;
E 5
			from += LINESZ;
D 4
			to += LINESZ;
			bzero(to, sizeof(struct lineno) - LINESZ);
			to += (sizeof(struct lineno) - LINESZ);
E 4
I 4
			to += sizeof(LINENO);
E 4
		    }
		}
		break;
	    case STYP_DATA:
		load_ns->ns_data_size += shdr->s_size;
		break;
	    case STYP_BSS:
		load_ns->ns_data_size += shdr->s_size;
		break;
	    case STYP_DEBUG:
		dbx_strings = m + shdr->s_scnptr;
		break;
	    case STYP_TYPCHK:
		tptr = (struct typchk *)(m + shdr->s_scnptr);
		t_cnt = shdr->s_size / 12;
		break;
	    }
	}
    }

D 2
    if (ohdr && ohdr->o_snloader) {
E 2
I 2
    if (text_base != -1 && ohdr && ohdr->o_snloader) {
E 2
	struct scnhdr *shdr =
	    (struct scnhdr *)(m + sizeof(struct filehdr) + fhdr->f_opthdr);

	shdr += (ohdr->o_snloader - 1);
	if (shdr->s_scnptr) {
	    struct ldhdr *lhdr = (struct ldhdr *)(m + shdr->s_scnptr);
	    int i;
	    struct ldsym *lsym = (struct ldsym *)(lhdr + 1);
	    char *strings = (char *)lhdr + lhdr->l_stoff;
	    struct ldrel *lrel;

I 10
	    DEBUG_PRINTF(("load: doing %d loader symbols\n",
			  lhdr->l_nsyms));

E 10
	    for (i = 0; i < lhdr->l_nsyms; ++i, ++lsym) {
		if (lsym->l_smtype & L_EXPORT) {
		    symptr s;
		    void *offset;

I 7
#ifdef l_zeroes
		    if (lsym->l_zeroes) {
			name = lsym->l_name;
			size = name[sizeof(lsym->l_name) - 1] ?
			    sizeof(lsym->l_name) :
			    0;
		    }
		    else
#endif
		    {
			name = strings + lsym->l_offset;
			size = 0;
		    }

E 7
		    /*
		     * I have looked at the name list for /unix and
		     * came up with the following mappings.  So far,
		     * there are eight combinations of smclas and
		     * smtype that I have looked at.  I will code
		     * these up and then print a nice message when I
		     * see things I haven't seen before.
		     */
		    switch (MAP(lsym->l_smclas, lsym->l_smtype)) {
		    case MAP(XMC_DS, XTY_ER): /* import function */
I 2
		    case MAP(XMC_RW, XTY_ER): /* import data */
		    case MAP(XMC_UA, XTY_ER): /* import data */
E 2
			continue;

D 2
		    case MAP(XMC_XO, XTY_ER): /* function */
		    case MAP(XMC_DS, XTY_SD): /* toc to function */
E 2
		    case MAP(XMC_SV, XTY_SD): /* Supervisor function */
I 6
#ifdef XMC_SV64
		    case MAP(XMC_SV64, XTY_SD):
#endif
#ifdef XMC_SV3264
		    case MAP(XMC_SV3264, XTY_SD):
#endif
E 6
I 2
			suffix = "$sv";
			thistype = get_ptr_func_int(load_ns,
						    &int_type,
						    &func_int_type,
						    &ptr_func_int_type);
			break;

I 6
		    case MAP(XMC_DS, XTY_LD): /* toc to function */
E 6
		    case MAP(XMC_DS, XTY_SD): /* toc to function */
			suffix = "$ds";
			thistype = get_ptr_func_int(load_ns,
						    &int_type,
						    &func_int_type,
						    &ptr_func_int_type);
			break;
		    
		    case MAP(XMC_XO, XTY_ER): /* function */
E 2
			suffix = 0;
			thistype = get_func_int(load_ns,
						&int_type,
						&func_int_type);
			break;

I 9
		    case MAP(XMC_PR, XTY_SD):
E 9
		    case MAP(XMC_PR, XTY_LD): /* data */
I 7
		    case MAP(XMC_RW, XTY_LD): /* data */
E 7
		    case MAP(XMC_RW, XTY_CM): /* data */
		    case MAP(XMC_RW, XTY_SD): /* data */
			suffix = 0;
D 10
			thistype = get_int(load_ns, &int_type);
E 10
I 10
			/*
			 * The loader section does not have sizes.  We
D 11
			 * guess that everything is long.
E 11
I 11
			 * guess that everything is int.
E 11
			 */
D 11
			thistype = get_long(load_ns, &long_type);
E 11
I 11
			thistype = get_int(load_ns, &long_type);
E 11
E 10
			break;

D 2
		    case MAP(XMC_UA, XTY_ER): /* import data */
			continue;

E 2
		    default:
			fprintf(stderr,
				"Unknown loader class/type of %d/%d for %s\n",
D 6
				lsym->l_smclas, lsym->l_smtype, lsym->l_name);
E 6
I 6
D 7
				lsym->l_smclas, lsym->l_smtype & 7, lsym->l_name);
E 7
I 7
				lsym->l_smclas, lsym->l_smtype & 7, name);
E 7
E 6
			continue;
		    }

D 7
		    if (lsym->l_zeroes) {
			name = lsym->l_name;
D 6
			size = sizeof(lsym->l_name);
E 6
I 6
			size = name[sizeof(lsym->l_name) - 1] ?
			    sizeof(lsym->l_name) :
			    0;
E 6
		    } else {
			name = strings + lsym->l_offset;
			size = 0;
		    }
E 7
		    if (haddot = (name[0] == '.')) {
			++name;
			if (size)
			    --size;
		    }
		    if (lsym->l_scnum == ohdr->o_sntext)
			offset = (void *)(text_base + lsym->l_value);
		    else if (lsym->l_scnum == ohdr->o_sndata ||
			     lsym->l_scnum == ohdr->o_snbss)
			offset = (void *)(data_base + lsym->l_value);
		    else if (lsym->l_scnum == 0) /* Treat these as absolute */
			offset = (void *)lsym->l_value;
		    else
D 10
			fprintf(stderr, "Bad loader section number for %s(%d)\n",
E 10
I 10
			fprintf(stderr,
				"Bad loader section number for %s(%d)\n",
E 10
				name, lsym->l_scnum);

		    name = store_string(load_ns, name, size, suffix);
D 3
		    s = enter_sym(load_ns, name);
E 3
I 3
		    s = enter_sym(load_ns, name, 0);
E 3
		    if (s->s_defined && s->s_offset == (void *)offset)
			continue;
		    /*
		     * If we hit a duplicate symbol at a different address we
		     * try a number of tricks to get things resolved.
		     * First, we check to see if one had a dot in
		     * front and the other does not.  If that is the
		     * case, then the one without the dot is assumed
		     * to be a toc entry for the one with a dot.
		     * If that is not the case then we enter a
		     * duplicate symbol with $dup as a suffix.
		     */
		    if (s->s_defined && s->s_offset != (void *)offset) {
			if (s->s_haddot != haddot) {
			    char *n2 = store_string(load_ns, name, 0, "$toc");
D 3
			    symptr s2 = enter_sym(load_ns, n2);
E 3
I 3
			    symptr s2 = enter_sym(load_ns, n2, 0);
E 3
			    void *old_offset = s->s_offset;
			    typeptr thistype2;

D 10
			    while (s2->s_defined && s2->s_offset != offset) {
				n2 = store_string(load_ns, n2, 0, "$dup");
D 3
				s2 = enter_sym(load_ns, n2);
E 3
I 3
				s2 = enter_sym(load_ns, n2, 0);
E 3
			    }
E 10
I 10
			    resolv_dup(&s2, &n2, load_ns, offset);
E 10
			    if (s2->s_defined)
				continue;

			    thistype2 = newtype(load_ns, PTR_TYPE);
			    thistype2->t_val.val_p = thistype;
			    s2->s_base = base_type(thistype2);
			    s2->s_type = thistype2;
D 10
			    s2->s_size = sizeof(int);
E 10
I 10
			    s2->s_size = sizeof(void *);
E 10
			    s2->s_nesting = 0;
			    s2->s_global = 1;
			    s2->s_defined = 1;

			    if (haddot)	{ /* the old guy is the toc */
				s2->s_offset = old_offset;
				s->s_offset = offset;
I 10
				DEBUG_PRINTF(("load: 1\n"));
E 10
			    } else {	/* the new guy is the toc */
				s->s_offset = old_offset;
				s2->s_offset = offset;
I 10
				DEBUG_PRINTF(("load: 2\n"));
E 10
			    }

			    s2->s_haddot = 0;
			    s->s_haddot = 1;
			    continue;
			}
D 10
			while (s->s_defined && s->s_offset != offset) {
			    name = store_string(load_ns, name, 0, "$dup");
D 3
			    s = enter_sym(load_ns, name);
E 3
I 3
			    s = enter_sym(load_ns, name, 0);
E 3
			}
E 10
I 10
			resolv_dup(&s, &name, load_ns, offset);
E 10
			if (s->s_defined)
			    continue;
		    }
		    s->s_defined = 1;
		    s->s_offset = (void *)offset;
		    s->s_base = base_type(thistype);
		    s->s_type = thistype;
D 10
		    s->s_size = sizeof(int);
E 10
I 10
		    s->s_size = get_size(thistype) / 8;
E 10
		    s->s_nesting = 0;
		    s->s_global = 1;
		    s->s_haddot = haddot;
		}
	    }

	    lrel = (struct ldrel *)lsym;
	    for (i = 0; i < lhdr->l_nreloc; ++i, ++lrel) {
	    }
	}
    }

    /* Nothing to do with the type check section */
    /* Nothing to do with the except section */
    
    /* Relocation information */
D 2
    if (fhdr->f_nscns) {
E 2
I 2
    if (text_base != -1 && fhdr->f_nscns) {
E 2
	struct scnhdr *shdr =
	    (struct scnhdr *)(m + sizeof(struct filehdr) + fhdr->f_opthdr);
	int i;

I 10
	DEBUG_PRINTF(("load: doing relocation section\n"));
E 10
	for (i = 0; i < fhdr->f_nscns; ++i, ++shdr) {
	    if (shdr->s_relptr) {
		int j;
		caddr_t base = m + shdr->s_relptr;
		
		for (j = 0; j < shdr->s_nreloc; ++j, base += RELSZ) {
		    struct reloc *r = (struct reloc *)base;
		    
		}
	    }
	}
    }
    
D 5
    /* Line number information */
D 2
    if (fhdr->f_nscns) {
E 2
I 2
    if (text_base != -1 && fhdr->f_nscns) {
E 2
	struct scnhdr *shdr =
	    (struct scnhdr *)(m + sizeof(struct filehdr) + fhdr->f_opthdr);
	int i;

	for (i = 0; i < fhdr->f_nscns; ++i, ++shdr) {
	    if (shdr->s_lnnoptr) {
		int j;
		caddr_t base = m + shdr->s_lnnoptr;
		
		for (j = 0; j < shdr->s_nlnno; ++j, base += LINESZ) {
		    struct lineno *l = (struct lineno *)base;
		    
		    if (l->l_lnno)
			/* This is a line number */;
		    else
			/* This is a symbol index */;
		}
	    }
	}
    }
    
E 5
    /* Symbol table */
    if (fhdr->f_symptr) {
	caddr_t base = m + fhdr->f_symptr;
	char *strings = base + (fhdr->f_nsyms * SYMESZ);
	ns *cur_file = load_ns;
	ns *cur_func = 0;
	ns *cur_block = cur_file;
	int need_load = 0;
	int end_func = -1;
	struct syment *s;
	symptr sptr, dbx_sptr;
D 4
	symptr last_csect;
E 4
I 4
	symptr last_csect = 0;
E 4
	int last_csect_index = -1;
I 4
	symptr last_code_csect = 0;
	int last_code_csect_index = -1;
E 4
	void *static_offset;
D 4
	int i;
E 4
I 4
	int static_forward_reference = 0;
	int sym_index;
	int cur_index;
	struct for_ref *ref_head = 0;
I 10
	long scnlen;
E 10
E 4

#define Set_cur_block(arg) { \
	cur_block = (arg); \
D 10
	int_type = ptr_int_type = func_int_type = ptr_func_int_type = 0; \
E 10
I 10
	long_type = int_type = func_int_type = ptr_func_int_type = 0; \
E 10
}

I 10
	DEBUG_PRINTF(("load: doing %d symbols\n", fhdr->f_nsyms));
E 10
D 4
	for (i = 0;
	     i < fhdr->f_nsyms;
	     i += s->n_numaux, base += s->n_numaux * SYMESZ) {
E 4
I 4
	for (cur_index = 0;
	     cur_index < fhdr->f_nsyms;
	     cur_index += s->n_numaux, base += s->n_numaux * SYMESZ) {
E 4
	    union auxent *aux;
	    char *suffix;
	    typeptr thistype;
	    void *offset;

	    s = (struct syment *)base;
I 4
	    sym_index = cur_index;

E 4
	    base += SYMESZ;
D 4
	    ++i;
E 4
I 4
	    ++cur_index;
E 4

I 2
	    /*
	     * If text_base is -1, that means we want only the type
D 3
	     * definitions from this file.
E 3
I 3
	     * definitions from this file.  We also need to grok the
	     * C_FILE entries so that the name spaces do not get
	     * confused.
E 3
	     */
D 3
	    if (text_base == -1 && s->n_sclass != C_DECL)
		continue;
E 3
I 3
	    if (text_base == -1)
		switch (s->n_sclass) {
		case C_DECL:		/* I expext this list to grow */
		case C_FILE:
		    break;
		default:
		    continue;
		}
E 3

E 2
	    if (s->n_numaux)
		aux = (union auxent *)(base + (AUXESZ * (s->n_numaux - 1)));
	    else
		aux = 0;

D 4
	    if (i == end_func) {
E 4
I 4
	    if (sym_index == end_func) {
E 4
		cur_func = 0;
		Set_cur_block(cur_file);
		end_func = 0;
	    }

I 7
#ifdef n_zeroes
E 7
	    if (s->n_zeroes) {
		name = s->n_name;
D 6
		size = sizeof(s->n_name);
E 6
I 6
		size = name[sizeof(s->n_name) - 1] ? sizeof(s->n_name) : 0;
E 6
D 7
	    } else {
E 7
I 7
	    } else
#endif
	    {
E 7
		/* Empty names -- skip over these puppies */
		if (!s->n_offset)
D 4
		    continue;
		name = ((s->n_sclass & DBXMASK) ? dbx_strings : strings) +
		    s->n_offset;
E 4
I 4
		    name = "$NONAME";	/* code csect typically have no name */
		else
		    name = ((s->n_sclass & DBXMASK) ? dbx_strings : strings) +
			s->n_offset;
E 4
		size = 0;
	    }
I 10
	    if (name[0] == '\0')
		name = "$NONAME";

E 10
	    if (haddot = (name[0] == '.')) {
		++name;
		if (size)
		    --size;
	    }

I 10
#if 0
E 10
	    /*
	     * I can't figure out what the STATIC stuff is before the
	     * first C_FILE.  I want to keep them for each file so we
	     * have a chance of finding static symbols.  This
	     * particular thing causes duplicate symbols at different
	     * addresses.
	     */
D 10
	    if (cur_file == load_ns && !strncmp(name, "_$STATIC", 8))
E 10
I 10
	    if (cur_file == load_ns &&
		(!strncmp(name, "_$STATIC", 8) ||
		 !strncmp(name, "$NONAME", 7)))
E 10
		continue;
I 10
#endif
E 10

	    if (aux && s->n_sclass != C_FILE && s->n_sclass != C_FCN &&
		s->n_sclass != C_BLOCK) {
I 10
#ifdef __XCOFF64__
		scnlen = (aux->x_csect.x_scnlen_hi << 32) |
		    aux->x_csect.x_scnlen_lo;
#else
		scnlen = aux->x_csect.x_scnlen;
#endif

E 10
		switch (MAP(aux->x_csect.x_smclas, aux->x_csect.x_smtyp)) {
		    /*
		     * A PR SD entry defines a csect and the PR LD
		     * entries define labels or entry points within
		     * that csect. scnlen of the LD entry is actually
		     * the symbol index for the SD entry.
		     * 
		     * PR SD's are not really functions but csects.
		     * PR LD's are assumed to be functions even if the
		     * type doesn't say so.
		     * 
		     * RW SD's and RW LD's work the same and are
		     * assume to be data.
		     * 
		     * RO SD's and RO LD's work the same and are
		     * assume to be data.
		     * 
		     * GL SD's and GL LD's are glink (12 byte) entries.
		     */
		case MAP(XMC_PR, XTY_SD):
		case MAP(XMC_PR, XTY_LD):
I 6
		    /* I'm not 100% positive about the XMC_DB */
		case MAP(XMC_DB, XTY_SD):
		case MAP(XMC_DB, XTY_LD):
E 6
		    suffix = 0;
D 6
		    thistype = get_func_int(cur_block,
					    &int_type,
					    &func_int_type);
E 6
I 6
		    /*
		     * New note 980915 by EPS... I added the test for
		     * haddot because of 'key_value' looked like a
		     * function pointer.  I'm hoping all the PR LD
		     * entries are from .s files and follow the ENTRY
		     * or DATA conventions.  If not, I'm screwed.
		     */
		    if (haddot)
			thistype = get_func_int(cur_block,
						&int_type,
						&func_int_type);
I 11
		    else if (scnlen == sizeof(char))
			thistype = get_char(cur_block, &char_type);
		    else if (scnlen == sizeof(short))
			thistype = get_short(cur_block, &short_type);
E 11
I 10
		    else if (scnlen == sizeof(int))
			thistype = get_int(cur_block, &int_type);
E 10
		    else
D 10
			thistype = get_int(cur_block,
					   &int_type);
E 10
I 10
			thistype = get_long(cur_block, &long_type);
E 10
E 6
		    break;

		case MAP(XMC_RO, XTY_SD):
		case MAP(XMC_RO, XTY_LD):
		case MAP(XMC_RW, XTY_SD):
		case MAP(XMC_RW, XTY_LD):
		    suffix = 0;
D 10
		    thistype = get_int(cur_block,
				       &int_type);
E 10
I 10
D 11
		    if (scnlen == sizeof(int))
E 11
I 11
		    if (scnlen == sizeof(char))
			thistype = get_char(cur_block, &char_type);
		    else if (scnlen == sizeof(short))
			thistype = get_short(cur_block, &short_type);
		    else if (scnlen == sizeof(int))
E 11
			thistype = get_int(cur_block, &int_type);
		    else
			thistype = get_long(cur_block, &long_type);
E 10
		    break;

		case MAP(XMC_GL, XTY_SD):
		case MAP(XMC_GL, XTY_LD):
		    suffix = "$glink";
		    thistype = get_func_int(cur_block,
					    &int_type,
					    &func_int_type);
		    break;

		    /*
		     * ER entries are external references to imported
		     * symbols.  The XO class defines pseudo machine
		     * opcodes which are at absolute addresses.
		     */
		case MAP(XMC_UA, XTY_ER):
I 2
		case MAP(XMC_RW, XTY_ER):
E 2
		case MAP(XMC_DS, XTY_ER):
		    continue;
I 2

E 2
		case MAP(XMC_XO, XTY_ER):
		    suffix = 0;
		    thistype = get_func_int(cur_block,
					    &int_type,
					    &func_int_type);
		    break;

I 5
#ifdef XMC_TD
E 5
		    /*
		     * A TC SD entry may point to a TD CM or a TD SD
		     * entry which appears to be the actual data.
		     */
		case MAP(XMC_TD, XTY_CM): /* An TC SD entry may point to a  */
		case MAP(XMC_TD, XTY_SD): /* TOC data entry */
		    suffix = 0;
D 10
		    thistype = get_int(cur_block, &int_type);
E 10
I 10
D 11
		    if (scnlen == sizeof(int))
E 11
I 11
		    if (scnlen == sizeof(char))
			thistype = get_char(cur_block, &char_type);
		    else if (scnlen == sizeof(short))
			thistype = get_short(cur_block, &short_type);
		    else if (scnlen == sizeof(int))
E 11
			thistype = get_int(cur_block, &int_type);
		    else
			thistype = get_long(cur_block, &long_type);
E 10
		    break;
I 5
#endif
E 5

		case MAP(XMC_RW, XTY_CM): /* BSS */
		    suffix = 0;
D 10
		    thistype = get_int(cur_block, &int_type);
E 10
I 10
D 11
		    if (scnlen == sizeof(int))
E 11
I 11
		    if (scnlen == sizeof(char))
			thistype = get_char(cur_block, &char_type);
		    else if (scnlen == sizeof(short))
			thistype = get_short(cur_block, &short_type);
		    else if (scnlen == sizeof(int))
E 11
			thistype = get_int(cur_block, &int_type);
		    else
			thistype = get_long(cur_block, &long_type);
E 10
		    break;

		case MAP(XMC_TC, XTY_SD): /* TOC entry */
		case MAP(XMC_TC0,XTY_SD): /* TOC anchor */
		    suffix = "$toc";
		    thistype = get_ptr_func_int(cur_block,
						&int_type,
						&func_int_type,
						&ptr_func_int_type);
		    break;

I 4
		case MAP(XMC_DS, XTY_LD): /* 3 word descriptor function */
E 4
		case MAP(XMC_DS, XTY_SD): /* 3 word descriptor function */
		    suffix = "$ds";
		    thistype = get_ptr_func_int(cur_block,
						&int_type,
						&func_int_type,
						&ptr_func_int_type);
		    break;
		    
I 4
		case MAP(XMC_BS, XTY_CM):
		    continue;

I 5
		case MAP(XMC_SV, XTY_SD): /* Supervisor function */
I 6
#ifdef XMC_SV64
		case MAP(XMC_SV64, XTY_SD): /* Supervisor function */
#endif
#ifdef XMC_SV3264
		case MAP(XMC_SV3264, XTY_SD): /* Supervisor function */
#endif
E 6
		    suffix = "$sv";
		    thistype = get_ptr_func_int(load_ns,
						&int_type,
						&func_int_type,
						&ptr_func_int_type);
		    break;

E 5
E 4
		default:
D 10
		    fprintf(stderr, "Unknown class/type pair of %d/%d for %s\n",
E 10
I 10
		    fprintf(stderr,
			    "Unknown class/type pair of %d/%d for %s\n",
E 10
D 4
			    aux->x_csect.x_smclas, aux->x_csect.x_smtyp,
E 4
I 4
			    aux->x_csect.x_smclas, aux->x_csect.x_smtyp & 7,
E 4
			    name);
		    continue;
		}
	    } else {
		suffix = 0;
D 10
		thistype = get_int(cur_block, &int_type);
E 10
I 10
D 11
		if (scnlen == sizeof(int))
E 11
I 11
		if (scnlen == sizeof(char))
		    thistype = get_char(cur_block, &char_type);
		else if (scnlen == sizeof(short))
		    thistype = get_short(cur_block, &short_type);
		else if (scnlen == sizeof(int))
E 11
		    thistype = get_int(cur_block, &int_type);
		else
		    thistype = get_long(cur_block, &long_type);
E 10
	    }

	    switch (s->n_sclass) {
	    case C_BLOCK:
		if (s->n_numaux != 1)
D 4
		    fprintf(stderr, "Missing aux entry for %d\n", i-1);
E 4
I 4
		    fprintf(stderr, "Missing aux entry for %d\n", sym_index);
E 4
		if (!cur_func) {
D 4
		    fprintf(stderr, ".bb outsize of a func at %d\n", i-1);
E 4
I 4
D 10
		    fprintf(stderr, ".bb outsize of a func at %d\n", sym_index);
E 10
I 10
		    fprintf(stderr, ".bb outsize of a func at %d\n",
			    sym_index);
E 10
E 4
		    continue;
		}
		if (!strcmp(name, "bb")) {	/* Begin Block */
		    Set_cur_block(ns_create(cur_block, ""));
		    new_symbols(cur_block);
		    cur_block->ns_text_start = text_base + s->n_value;
		    cur_block->ns_lineoffset =
			((union auxent *)base)->x_sym.x_misc.x_lnsz.x_lnno;
		} else {			/* End Block */
		    if (!cur_block) {
D 4
			fprintf(stderr, ".eb without cur_block on %d\n", i-1);
E 4
I 4
			fprintf(stderr, ".eb without cur_block on %d\n",
				sym_index);
E 4
			continue;
		    }
		    cur_block->ns_text_size = (text_base + s->n_value) -
			cur_block->ns_text_start;
		    Set_cur_block(cur_block->ns_parent);
		}
		break;

	    case C_HIDEXT:
	    case C_EXT:
I 11
	    case C_WEAKEXT:
E 11
		if (s->n_scnum == ohdr->o_sntext)
		    offset = (void *)(text_base + s->n_value);
		else if (s->n_scnum == ohdr->o_sndata ||
			 s->n_scnum == ohdr->o_snbss)
		    offset = (void *)(data_base + s->n_value);
		else if (s->n_scnum == 0) /* Treat these as absolute */
		    offset = (void *)s->n_value;
		else
		    fprintf(stderr, "Bad symbol section number for %s(%d)\n",
			    name, s->n_scnum);

		name = store_string(cur_block, name, size, suffix);
		if (s->n_numaux < 1)
		    fprintf(stderr, "No aux entries for %s\n", name);

D 3
		sptr = enter_sym(cur_block, name);
E 3
I 3
		sptr = enter_sym(cur_block, name, 0);
E 3
		if (sptr->s_defined && sptr->s_offset == (void *)offset)
		    goto after_enter_sym;

		/*
		 * This is the same code as above for duplicate
		 * symbols at different addresses.
		 */
		if (sptr->s_defined && sptr->s_offset != (void *)offset) {
		    if (sptr->s_haddot != haddot) {
			char *n2 = store_string(cur_block, name, 0, "$toc");
D 3
			symptr s2 = enter_sym(cur_block, n2);
E 3
I 3
			symptr s2 = enter_sym(cur_block, n2, 0);
E 3
			void *old_offset = sptr->s_offset;
			typeptr thistype2;

D 10
			while (s2->s_defined && s2->s_offset != offset) {
			    n2 = store_string(cur_block, n2, 0, "$dup");
D 3
			    s2 = enter_sym(cur_block, n2);
E 3
I 3
			    s2 = enter_sym(cur_block, n2, 0);
E 3
			}
E 10
I 10
			resolv_dup(&s2, &n2, cur_block, offset);
E 10
			if (s2->s_defined)
			    goto after_enter_sym;

			thistype2 = newtype(cur_block, PTR_TYPE);
			thistype2->t_val.val_p = thistype;
			s2->s_base = base_type(thistype2);
			s2->s_type = thistype2;
D 10
			s2->s_size = sizeof(int);
E 10
I 10
			s2->s_size = sizeof(void *);
E 10
			s2->s_nesting = 0;
			s2->s_global = 1;
			s2->s_defined = 1;

			if (haddot)	{ /* the old guy is the toc */
			    s2->s_offset = old_offset;
			    sptr->s_offset = offset;
			} else {	/* the new guy is the toc */
			    sptr->s_offset = old_offset;
			    s2->s_offset = offset;
			}

			s2->s_haddot = 0;
			sptr->s_haddot = 1;
			goto after_enter_sym;
		    }
D 10
		    while (sptr->s_defined && sptr->s_offset != offset) {
			name = store_string(cur_block, name, 0, "$dup");
D 3
			sptr = enter_sym(cur_block, name);
E 3
I 3
			sptr = enter_sym(cur_block, name, 0);
E 3
		    }
E 10
I 10
		    resolv_dup(&sptr, &name, cur_block, offset);
E 10
		    if (sptr->s_defined)
			goto after_enter_sym;
		}

		sptr->s_defined = 1;
		sptr->s_offset = (void *)offset;
		sptr->s_base = base_type(thistype);
		sptr->s_type = thistype;
I 7
D 10
#ifdef __XCOFF64__
		sptr->s_size = (aux->x_csect.x_scnlen_hi << 32) |
				     aux->x_csect.x_scnlen_lo;
#else
E 7
		sptr->s_size = aux->x_csect.x_scnlen; /* #### not always */
I 7
#endif
E 10
I 10
		sptr->s_size = scnlen;
E 10
E 7
		sptr->s_nesting = 0;
		sptr->s_global = 1;
		sptr->s_haddot = haddot;

	    after_enter_sym:
		if ((aux->x_csect.x_smtyp & 7) == XTY_SD ||
		    (aux->x_csect.x_smtyp & 7) == XTY_CM) {
		    last_csect = sptr;
D 4
		    last_csect_index = i-1;
E 4
I 4
		    last_csect_index = sym_index;
		    if (aux->x_csect.x_smclas == XMC_PR) {
			last_code_csect = sptr;
			last_code_csect_index = sym_index;
		    }
E 4
		}
I 4
		/*
		 * Resolve forward references
		 */
		while (ref_head && sym_index == ref_head->fr_index) {
		    struct for_ref *temp = ref_head;
E 4

I 4
		    ref_head = ref_head->fr_next;
		    temp->fr_sym->s_offset = (void *)
D 10
			((ulong)temp->fr_sym->s_offset + (ulong)sptr->s_offset);
E 10
I 10
			((ulong)temp->fr_sym->s_offset +
			 (ulong)sptr->s_offset);
E 10
		    free(temp);
		}

E 4
		/*
		 * If this is a new function that looks interesting,
		 * we create a name space for it.
		 * 
		 * We have two choices: either create the name space
		 * here or wait until we hit the .bf entry to create
		 * the name space.  I hope this works because it puts
		 * the parameters in the right place.
		 */
		if (ISFCN(s->n_type) && s->n_numaux > 1) {
		    union auxent *a1 = (union auxent *)base;

		    if (s->n_scnum != ohdr->o_sntext)
D 4
			fprintf(stderr, "Strange symentry %d\n", i-1);
E 4
I 4
D 5
			fprintf(stderr, "Strange symentry %d\n", sym_index);
E 5
I 5
			break;
E 5
E 4
		    /*
I 5
		     * fprintf(stderr, "Strange symentry %d\n", sym_index);
		     */
		    /*
E 5
		     * Another hack... frequently there are two aux
		     * entries even if there is no debug info so we
		     * check the lnnoptr for another excuse not to
		     * create a seperate name space for this function.
		     */
I 7
#ifdef __XCOFF64__
D 10
		    if (a1->x_fcn.x_lnnoptr) {
E 10
I 10
		    if (load_ns->ns_lines &&
			a1->x_fcn.x_lnnoptr) {
E 10
			Set_cur_block(cur_func = ns_create(cur_file, name));
			new_symbols(cur_func);
			cur_func->ns_text_start = text_base + s->n_value;
			cur_func->ns_lines = load_ns->ns_lines +
			    ((a1->x_fcn.x_lnnoptr - lnnoptr_base)
			     / LINESZ);
			end_func = a1->x_fcn.x_endndx;
			cur_func->ns_text_size = a1->x_fcn.x_fsize;
		    }
#else
E 7
D 10
		    if (a1->x_sym.x_fcnary.x_fcn.x_lnnoptr) {
E 10
I 10
		    if (load_ns->ns_lines &&
			a1->x_sym.x_fcnary.x_fcn.x_lnnoptr) {
E 10
			Set_cur_block(cur_func = ns_create(cur_file, name));
			new_symbols(cur_func);
			cur_func->ns_text_start = text_base + s->n_value;
			cur_func->ns_lines = load_ns->ns_lines +
D 10
			    ((a1->x_sym.x_fcnary.x_fcn.x_lnnoptr - lnnoptr_base)
			     / LINESZ);
E 10
I 10
			    ((a1->x_sym.x_fcnary.x_fcn.x_lnnoptr -
			      lnnoptr_base) / LINESZ);
E 10
			end_func = a1->x_sym.x_fcnary.x_fcn.x_endndx;
D 10
			cur_func->ns_text_size = a1->x_sym.x_misc.x_lnsz.x_size;
E 10
I 10
			cur_func->ns_text_size =
			    a1->x_sym.x_misc.x_lnsz.x_size;
E 10
		    }
I 7
#endif
E 7
		}
		break;

	    case C_FCN:
		if (!strcmp(name, "bf")) {	/* Function start */
		    /*
		     * The only thing useful from this is the lnno
		     * which needs to be added to the line number
		     * entries.
		     */
		    if (s->n_numaux != 1) {
D 4
			fprintf(stderr, "Missing aux entry for %d\n", i-1);
E 4
I 4
			fprintf(stderr, "Missing aux entry for %d\n",
				sym_index);
E 4
			continue;
		    }
		    if (!cur_func) {
D 4
			fprintf(stderr, ".bf outside of function at %d\n", i-1);
E 4
I 4
			fprintf(stderr, ".bf outside of function at %d\n",
				sym_index);
E 4
			continue;
		    }
		    cur_func->ns_lineoffset =
			((union auxent *)base)->x_sym.x_misc.x_lnsz.x_lnno;
		} else {			/* Function end */
		    /*
		     * We could double check the size here.
		     */
		}
		break;

	    case C_FILE:
		/* I don't know what this is but it looks worthless */
		if (s->n_type == 0xf903)
		    continue;
		if (s->n_numaux) {		/* must go find the filename */
		    int j;

		    for (j = 0; j < s->n_numaux; ++j) {
			aux = (union auxent *)(base + (j * AUXESZ));
			if (aux->x_file._x.x_ftype == XFT_FN) {
			    if (aux->x_file._x.x_zeroes) {
				name = aux->x_file.x_fname;
D 6
				size = sizeof(aux->x_file.x_fname);
E 6
I 6
				size = name[sizeof(aux->x_file.x_fname) - 1] ?
				    sizeof(aux->x_file.x_fname) :
				    0;
E 6
			    } else {
				name = strings + aux->x_file._x.x_offset;
				size = 0;
			    }
			    break;
			}
		    }
		}
		name = store_string(load_ns, name, size, suffix);
		Set_cur_block(cur_file = ns_create(load_ns, name));
		new_symbols(cur_file);
		need_load = 1;
		break;

		/* DBX entries */
	    case C_BSTAT:		/* symbol table index */
D 4
		if (last_csect_index != s->n_value)
		    fprintf(stderr, "C_BSTAT is confused %d should be %d\n",
			    last_csect_index, s->n_value);
		static_offset = last_csect->s_offset;
E 4
I 4
		if (last_csect_index != s->n_value) {
		    if (s->n_value > sym_index) { /* forward reference */
			static_forward_reference = s->n_value;
		    } else {		/* backward confused reference */
D 10
			fprintf(stderr, "C_BSTAT is confused %d should be %d\n",
E 10
I 10
			fprintf(stderr,
				"C_BSTAT is confused %d should be %d\n",
E 10
				last_csect_index, s->n_value);
		    }
		} else {		/* normal reference */
		    static_forward_reference = 0;
		    static_offset = last_csect->s_offset;
		}
E 4
		break;

	    case C_ESTAT:		/* 0 */
		break;

I 5
	    case C_LSYM:		/* offset in stack */
	    case C_PSYM:		/* offset in stack */
		/*
		 * The code below actually copes with this but it
		 * turns out that these clutter the symbol table and
		 * make life harder instead of easier.
		 */
		break;

E 5
	    case C_DECL:		/* undefined */
	    case C_GSYM:		/* undefined */
	    case C_FUN:			/* offset in csect */
	    case C_STSYM:		/* offset in csect */
D 5
	    case C_LSYM:		/* offset in stack */
	    case C_PSYM:		/* offset in stack */
E 5
		if (need_load) {
		    load_base_types(cur_file);
		    need_load = 0;
		}
D 4
		parse_stab(cur_block, name, size, &dbx_sptr);
E 4
I 4
		/*
		 * Hack number 44892: The dbx stab string for the
		 * function comes after the C_EXT which means that
		 * cur_block points to the function and not the file.
		 * So... in the case of a C_FUN, we pass cur_file to
		 * parse_stab instead of cur_block.  In pascal, this
		 * is going to break but... hahahahahahahaha.
I 6
		 * 
		 * Whisper quietly over the sound of jack hammers and
		 * chain saws... The stab string syntax says that if
		 * the string ends in a ? then it continues in the
		 * next stab entry.  Well, my stab parser gets
		 * impatient so I concatenate all of the things
		 * together before calling parse_stab.  The way I do
		 * this is to save the string in a buffer and loop
		 * back around.  This makes the assumption that the
		 * other fields are consistant in all the stab entries
		 * I concatenate together.
E 6
		 */
D 6
		parse_stab(s->n_sclass == C_FUN ? cur_file : cur_block, name,
			   size, &dbx_sptr);
E 6
I 6
		if (name[(size ? size : strlen(name)) - 1] == '?') {
		    add_to_buf(name, (size ? size : strlen(name)));
		    have_stab_buf = 1;
		    break;
		}
E 6
E 4

I 6
		if (have_stab_buf) {
		    add_to_buf(name, (size ? size : strlen(name)));
D 10
		    parse_stab(s->n_sclass == C_FUN ? cur_file : cur_block, stab_buf,
			       0, &dbx_sptr);
E 10
I 10
		    parse_stab((s->n_sclass == C_FUN ? cur_file : cur_block),
			       stab_buf, 0, &dbx_sptr);
E 10
		    have_stab_buf = 0;
		    stab_buf[0] = 0;
		} else {
D 10
		    parse_stab(s->n_sclass == C_FUN ? cur_file : cur_block, name,
			       size, &dbx_sptr);
E 10
I 10
		    parse_stab(s->n_sclass == C_FUN ? cur_file : cur_block,
			       name, size, &dbx_sptr);
E 10
		}

E 6
		/* These guys have an undefined n_value field */
		if (s->n_sclass == C_DECL || s->n_sclass == C_GSYM)
		    break;

		if (!dbx_sptr) {
		    fprintf(stderr, "Expected dbx_sptr to be set %d\n",
D 4
			    i-1);
E 4
I 4
			    sym_index);
E 4
		    break;
		}
		if (s->n_sclass == C_FUN) {
D 4
		    offset = (void *)((int)last_csect->s_offset + s->n_value);
E 4
I 4
D 11
		    offset = (void *)((int)last_code_csect->s_offset +
E 11
I 11
		    offset = (void *)((unsigned long)last_code_csect->s_offset +
E 11
				      s->n_value);
E 4
		    dbx_sptr->s_global = 1;
		} else if (s->n_sclass == C_STSYM) {
D 4
		    offset = (void *)((int)static_offset + s->n_value);
E 4
I 4
		    if (static_forward_reference) {
D 5
			struct for_ref *temp = smalloc(sizeof(*temp),
						       __FILE__, __LINE__);
E 5
I 5
			struct for_ref *temp = smalloc(sizeof(*temp));
E 5
			struct for_ref **pp, *p;

			temp->fr_next = 0;
			temp->fr_index = static_forward_reference;
			temp->fr_sym = dbx_sptr;
			/*
			 * Insert in order
			 */
			for (pp = &ref_head;
D 10
			     (p = *pp) && p->fr_index <= static_forward_reference;
E 10
I 10
			     (p = *pp) &&
				 (p->fr_index <= static_forward_reference);
E 10
			     pp = &p->fr_next);
			temp->fr_next = *pp;
			*pp = temp;

			offset = (void *)s->n_value;
		    } else {
D 11
			offset = (void *)((int)static_offset + s->n_value);
E 11
I 11
			offset = (void *)((unsigned long)static_offset + s->n_value);
E 11
		    }
E 4
		    dbx_sptr->s_global = 1;
		} else { /* C_LSYM or C_PSYM */
		    offset = (void *)s->n_value;
		    dbx_sptr->s_global = 0;
		}

		/*
		 * We could sanity check and make sure that s_global
		 * is off for C_LSYM and C_PSYM and set for the other
		 * guys.
		 */
D 11
		if (dbx_sptr->s_defined && dbx_sptr->s_offset != offset)
D 4
		    fprintf(stderr, "Duplicate global symbol %s\n",
			    dbx_sptr->s_name);
		dbx_sptr->s_offset = offset;
		dbx_sptr->s_defined = 1;
E 4
I 4
D 10
		    fprintf(stderr, "Duplicate global symbol %s at %d %x != %x\n",
E 10
I 10
		    fprintf(stderr,
			    "Duplicate global symbol %s at %d %x != %x\n",
E 10
			    dbx_sptr->s_name, sym_index, dbx_sptr->s_offset,
			    offset);
		else {
E 11
I 11
		if (dbx_sptr->s_defined && dbx_sptr->s_offset != offset) {
			fprintf(stderr,
				"Duplicate global symbol %s at %d %lx != %lx\n",
				dbx_sptr->s_name, sym_index, dbx_sptr->s_offset,
				offset);
		
			fprintf(stderr,
				"...n_sclass = %d csect_index = %d csect->offset = %lx s->n_value = %lx\n",
				s->n_sclass,
				last_code_csect_index,
				last_code_csect->s_offset,
				s->n_value
				);
		} else {
E 11
		    dbx_sptr->s_offset = offset;
		    dbx_sptr->s_defined = 1;
		}
I 5
		break;

	    case C_BINCL:		/* Don't care about include files */
	    case C_EINCL:
E 5
E 4
		break;

	    default:
		fprintf(stderr, "Unknown sclass: %d\n", s->n_sclass);
		break;
	    }
	}
I 4
	if (ref_head)
	    fprintf(stderr, "ref_head still points to something.\n");

E 4
#undef Set_cur_block
    }
    (void) close(fd);
I 11
    if (debug_mask & MMAP_BIT)
	printf("munmap: s=%s l=%s e=%s\n", P(m),
	       P(sbuf.st_size),
	       P(m + sbuf.st_size));
E 11
I 3
D 10
#ifdef USE_MMAP
E 10
E 3
    (void) munmap(m, sbuf.st_size);
I 3
D 10
#endif
E 10
I 6
    if (have_stab_buf)
D 10
	fprintf(stderr, "Load finished with %s still in stab_buf\n", stab_buf);
E 10
I 10
	fprintf(stderr, "Load finished with %s still in stab_buf\n",
		stab_buf);
E 10
    finish_copies();
I 10
    DEBUG_PRINTF(("load: done\n"));
    return 0;
E 10
E 6
E 3
}
E 1
