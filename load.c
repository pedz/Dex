static char sccs_id[] = "@(#)load.c	1.5";

#include <a.out.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>

/*
 * I would prefer to use mmap but it has a bug in it right now that
 * shmat does not have so I'll use shmat for now.
 */
#define USE_MMAP
#ifdef USE_MMAP
#include <sys/mman.h>
#include <sys/stat.h>
#else
#include <sys/shm.h>
#endif

#include "map.h"
#include "sym.h"
#include "dex.h"

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

static typeptr get_int(ns *nspace, typeptr *int_type_p)
{
    typeptr ret;

    if (ret = *int_type_p)
	return ret;
    ret = *int_type_p = name2typedef(nspace, "int");
    return ret;
}

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

static typeptr get_ptr_int(ns *nspace,
			   typeptr *int_type_p,
			   typeptr *ptr_int_type_p)
{
    typeptr ret;
    
    if (ret = *ptr_int_type_p)
	return ret;
    ret = *ptr_int_type_p = newtype(nspace, PTR_TYPE);
    ret->t_val.val_p = get_int(nspace, int_type_p);
    return ret;
}
    
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

struct for_ref {
    struct for_ref *fr_next;
    int fr_index;
    symptr fr_sym;
};


void load(char *path, int text_base, int data_base)
{
    int fd;
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
    typeptr int_type = 0;
    typeptr ptr_int_type = 0;
    typeptr func_int_type = 0;
    typeptr ptr_func_int_type = 0;
    char *suffix;
    typeptr thistype;
    int haddot;
#ifdef USE_MMAP
    struct stat sbuf;
#endif

    if ((fd = open(path, O_RDONLY)) < 0) {
	perror(path);
	return;
    }

#ifdef USE_MMAP
    if (fstat(fd, &sbuf) < 0) {
	perror("stat");
	close(fd);
	return;
    }
    m = mmap(LOAD_BASE, sbuf.st_size, PROT_READ,
	     MAP_FILE|MAP_VARIABLE, fd, 0);
    if ((int)m == -1) {
	perror("mmap3");
	close(fd);
	return;
    }
#else
    m = shmat(fd, 0x40000000, SHM_MAP|SHM_RDONLY);
    if ((int)m == -1) {
	perror("shmat");
	close(fd);
	return;
    }
#endif

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

	for (i = 0; i < fhdr->f_nscns; ++i, ++shdr) {
	    switch (shdr->s_flags & 0xffff) {
	    case STYP_TEXT:
		load_ns->ns_text_size = shdr->s_size;
		if (lnnoptr_base = shdr->s_lnnoptr) {
		    int cnt = shdr->s_nlnno;
		    int size = sizeof(LINENO) * cnt;
		    caddr_t from, to;

		    load_ns->ns_lines = smalloc(size);
		    from = m + shdr->s_lnnoptr;
		    to = (caddr_t)load_ns->ns_lines;
		    bzero(to, size);
		    while (--cnt >= 0) {
			struct lineno *l = (struct lineno *)to;

			bcopy(from, to, LINESZ);
			l->l_addr.l_paddr += text_base;
			from += LINESZ;
			to += sizeof(LINENO);
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

    if (text_base != -1 && ohdr && ohdr->o_snloader) {
	struct scnhdr *shdr =
	    (struct scnhdr *)(m + sizeof(struct filehdr) + fhdr->f_opthdr);

	shdr += (ohdr->o_snloader - 1);
	if (shdr->s_scnptr) {
	    struct ldhdr *lhdr = (struct ldhdr *)(m + shdr->s_scnptr);
	    int i;
	    struct ldsym *lsym = (struct ldsym *)(lhdr + 1);
	    char *strings = (char *)lhdr + lhdr->l_stoff;
	    struct ldrel *lrel;

	    for (i = 0; i < lhdr->l_nsyms; ++i, ++lsym) {
		if (lsym->l_smtype & L_EXPORT) {
		    symptr s;
		    void *offset;

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
		    case MAP(XMC_RW, XTY_ER): /* import data */
		    case MAP(XMC_UA, XTY_ER): /* import data */
			continue;

		    case MAP(XMC_SV, XTY_SD): /* Supervisor function */
			suffix = "$sv";
			thistype = get_ptr_func_int(load_ns,
						    &int_type,
						    &func_int_type,
						    &ptr_func_int_type);
			break;

		    case MAP(XMC_DS, XTY_SD): /* toc to function */
			suffix = "$ds";
			thistype = get_ptr_func_int(load_ns,
						    &int_type,
						    &func_int_type,
						    &ptr_func_int_type);
			break;
		    
		    case MAP(XMC_XO, XTY_ER): /* function */
			suffix = 0;
			thistype = get_func_int(load_ns,
						&int_type,
						&func_int_type);
			break;

		    case MAP(XMC_PR, XTY_LD): /* data */
		    case MAP(XMC_RW, XTY_CM): /* data */
		    case MAP(XMC_RW, XTY_SD): /* data */
			suffix = 0;
			thistype = get_int(load_ns, &int_type);
			break;

		    default:
			fprintf(stderr,
				"Unknown loader class/type of %d/%d for %s\n",
				lsym->l_smclas, lsym->l_smtype, lsym->l_name);
			continue;
		    }

		    if (lsym->l_zeroes) {
			name = lsym->l_name;
			size = sizeof(lsym->l_name);
		    } else {
			name = strings + lsym->l_offset;
			size = 0;
		    }
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
			fprintf(stderr, "Bad loader section number for %s(%d)\n",
				name, lsym->l_scnum);

		    name = store_string(load_ns, name, size, suffix);
		    s = enter_sym(load_ns, name, 0);
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
			    symptr s2 = enter_sym(load_ns, n2, 0);
			    void *old_offset = s->s_offset;
			    typeptr thistype2;

			    while (s2->s_defined && s2->s_offset != offset) {
				n2 = store_string(load_ns, n2, 0, "$dup");
				s2 = enter_sym(load_ns, n2, 0);
			    }
			    if (s2->s_defined)
				continue;

			    thistype2 = newtype(load_ns, PTR_TYPE);
			    thistype2->t_val.val_p = thistype;
			    s2->s_base = base_type(thistype2);
			    s2->s_type = thistype2;
			    s2->s_size = sizeof(int);
			    s2->s_nesting = 0;
			    s2->s_global = 1;
			    s2->s_defined = 1;

			    if (haddot)	{ /* the old guy is the toc */
				s2->s_offset = old_offset;
				s->s_offset = offset;
			    } else {	/* the new guy is the toc */
				s->s_offset = old_offset;
				s2->s_offset = offset;
			    }

			    s2->s_haddot = 0;
			    s->s_haddot = 1;
			    continue;
			}
			while (s->s_defined && s->s_offset != offset) {
			    name = store_string(load_ns, name, 0, "$dup");
			    s = enter_sym(load_ns, name, 0);
			}
			if (s->s_defined)
			    continue;
		    }
		    s->s_defined = 1;
		    s->s_offset = (void *)offset;
		    s->s_base = base_type(thistype);
		    s->s_type = thistype;
		    s->s_size = sizeof(int);
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
    if (text_base != -1 && fhdr->f_nscns) {
	struct scnhdr *shdr =
	    (struct scnhdr *)(m + sizeof(struct filehdr) + fhdr->f_opthdr);
	int i;

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
	symptr last_csect = 0;
	int last_csect_index = -1;
	symptr last_code_csect = 0;
	int last_code_csect_index = -1;
	void *static_offset;
	int static_forward_reference = 0;
	int sym_index;
	int cur_index;
	struct for_ref *ref_head = 0;

#define Set_cur_block(arg) { \
	cur_block = (arg); \
	int_type = ptr_int_type = func_int_type = ptr_func_int_type = 0; \
}

	for (cur_index = 0;
	     cur_index < fhdr->f_nsyms;
	     cur_index += s->n_numaux, base += s->n_numaux * SYMESZ) {
	    union auxent *aux;
	    char *suffix;
	    typeptr thistype;
	    void *offset;

	    s = (struct syment *)base;
	    sym_index = cur_index;

	    base += SYMESZ;
	    ++cur_index;

	    /*
	     * If text_base is -1, that means we want only the type
	     * definitions from this file.  We also need to grok the
	     * C_FILE entries so that the name spaces do not get
	     * confused.
	     */
	    if (text_base == -1)
		switch (s->n_sclass) {
		case C_DECL:		/* I expext this list to grow */
		case C_FILE:
		    break;
		default:
		    continue;
		}

	    if (s->n_numaux)
		aux = (union auxent *)(base + (AUXESZ * (s->n_numaux - 1)));
	    else
		aux = 0;

	    if (sym_index == end_func) {
		cur_func = 0;
		Set_cur_block(cur_file);
		end_func = 0;
	    }

	    if (s->n_zeroes) {
		name = s->n_name;
		size = sizeof(s->n_name);
	    } else {
		/* Empty names -- skip over these puppies */
		if (!s->n_offset)
		    name = "$NONAME";	/* code csect typically have no name */
		else
		    name = ((s->n_sclass & DBXMASK) ? dbx_strings : strings) +
			s->n_offset;
		size = 0;
	    }
	    if (haddot = (name[0] == '.')) {
		++name;
		if (size)
		    --size;
	    }

	    /*
	     * I can't figure out what the STATIC stuff is before the
	     * first C_FILE.  I want to keep them for each file so we
	     * have a chance of finding static symbols.  This
	     * particular thing causes duplicate symbols at different
	     * addresses.
	     */
	    if (cur_file == load_ns && !strncmp(name, "_$STATIC", 8))
		continue;

	    if (aux && s->n_sclass != C_FILE && s->n_sclass != C_FCN &&
		s->n_sclass != C_BLOCK) {
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
		    suffix = 0;
		    thistype = get_func_int(cur_block,
					    &int_type,
					    &func_int_type);
		    break;

		case MAP(XMC_RO, XTY_SD):
		case MAP(XMC_RO, XTY_LD):
		case MAP(XMC_RW, XTY_SD):
		case MAP(XMC_RW, XTY_LD):
		    suffix = 0;
		    thistype = get_int(cur_block,
				       &int_type);
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
		case MAP(XMC_RW, XTY_ER):
		case MAP(XMC_DS, XTY_ER):
		    continue;

		case MAP(XMC_XO, XTY_ER):
		    suffix = 0;
		    thistype = get_func_int(cur_block,
					    &int_type,
					    &func_int_type);
		    break;

#ifdef XMC_TD
		    /*
		     * A TC SD entry may point to a TD CM or a TD SD
		     * entry which appears to be the actual data.
		     */
		case MAP(XMC_TD, XTY_CM): /* An TC SD entry may point to a  */
		case MAP(XMC_TD, XTY_SD): /* TOC data entry */
		    suffix = 0;
		    thistype = get_int(cur_block, &int_type);
		    break;
#endif

		case MAP(XMC_RW, XTY_CM): /* BSS */
		    suffix = 0;
		    thistype = get_int(cur_block, &int_type);
		    break;

		case MAP(XMC_TC, XTY_SD): /* TOC entry */
		case MAP(XMC_TC0,XTY_SD): /* TOC anchor */
		    suffix = "$toc";
		    thistype = get_ptr_func_int(cur_block,
						&int_type,
						&func_int_type,
						&ptr_func_int_type);
		    break;

		case MAP(XMC_DS, XTY_LD): /* 3 word descriptor function */
		case MAP(XMC_DS, XTY_SD): /* 3 word descriptor function */
		    suffix = "$ds";
		    thistype = get_ptr_func_int(cur_block,
						&int_type,
						&func_int_type,
						&ptr_func_int_type);
		    break;
		    
		case MAP(XMC_BS, XTY_CM):
		    continue;

		case MAP(XMC_SV, XTY_SD): /* Supervisor function */
		    suffix = "$sv";
		    thistype = get_ptr_func_int(load_ns,
						&int_type,
						&func_int_type,
						&ptr_func_int_type);
		    break;

		default:
		    fprintf(stderr, "Unknown class/type pair of %d/%d for %s\n",
			    aux->x_csect.x_smclas, aux->x_csect.x_smtyp & 7,
			    name);
		    continue;
		}
	    } else {
		suffix = 0;
		thistype = get_int(cur_block, &int_type);
	    }

	    switch (s->n_sclass) {
	    case C_BLOCK:
		if (s->n_numaux != 1)
		    fprintf(stderr, "Missing aux entry for %d\n", sym_index);
		if (!cur_func) {
		    fprintf(stderr, ".bb outsize of a func at %d\n", sym_index);
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
			fprintf(stderr, ".eb without cur_block on %d\n",
				sym_index);
			continue;
		    }
		    cur_block->ns_text_size = (text_base + s->n_value) -
			cur_block->ns_text_start;
		    Set_cur_block(cur_block->ns_parent);
		}
		break;

	    case C_HIDEXT:
	    case C_EXT:
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

		sptr = enter_sym(cur_block, name, 0);
		if (sptr->s_defined && sptr->s_offset == (void *)offset)
		    goto after_enter_sym;

		/*
		 * This is the same code as above for duplicate
		 * symbols at different addresses.
		 */
		if (sptr->s_defined && sptr->s_offset != (void *)offset) {
		    if (sptr->s_haddot != haddot) {
			char *n2 = store_string(cur_block, name, 0, "$toc");
			symptr s2 = enter_sym(cur_block, n2, 0);
			void *old_offset = sptr->s_offset;
			typeptr thistype2;

			while (s2->s_defined && s2->s_offset != offset) {
			    n2 = store_string(cur_block, n2, 0, "$dup");
			    s2 = enter_sym(cur_block, n2, 0);
			}
			if (s2->s_defined)
			    goto after_enter_sym;

			thistype2 = newtype(cur_block, PTR_TYPE);
			thistype2->t_val.val_p = thistype;
			s2->s_base = base_type(thistype2);
			s2->s_type = thistype2;
			s2->s_size = sizeof(int);
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
		    while (sptr->s_defined && sptr->s_offset != offset) {
			name = store_string(cur_block, name, 0, "$dup");
			sptr = enter_sym(cur_block, name, 0);
		    }
		    if (sptr->s_defined)
			goto after_enter_sym;
		}

		sptr->s_defined = 1;
		sptr->s_offset = (void *)offset;
		sptr->s_base = base_type(thistype);
		sptr->s_type = thistype;
		sptr->s_size = aux->x_csect.x_scnlen; /* #### not always */
		sptr->s_nesting = 0;
		sptr->s_global = 1;
		sptr->s_haddot = haddot;

	    after_enter_sym:
		if ((aux->x_csect.x_smtyp & 7) == XTY_SD ||
		    (aux->x_csect.x_smtyp & 7) == XTY_CM) {
		    last_csect = sptr;
		    last_csect_index = sym_index;
		    if (aux->x_csect.x_smclas == XMC_PR) {
			last_code_csect = sptr;
			last_code_csect_index = sym_index;
		    }
		}
		/*
		 * Resolve forward references
		 */
		while (ref_head && sym_index == ref_head->fr_index) {
		    struct for_ref *temp = ref_head;

		    ref_head = ref_head->fr_next;
		    temp->fr_sym->s_offset = (void *)
			((ulong)temp->fr_sym->s_offset + (ulong)sptr->s_offset);
		    free(temp);
		}

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
			break;
		    /*
		     * fprintf(stderr, "Strange symentry %d\n", sym_index);
		     */
		    /*
		     * Another hack... frequently there are two aux
		     * entries even if there is no debug info so we
		     * check the lnnoptr for another excuse not to
		     * create a seperate name space for this function.
		     */
		    if (a1->x_sym.x_fcnary.x_fcn.x_lnnoptr) {
			Set_cur_block(cur_func = ns_create(cur_file, name));
			new_symbols(cur_func);
			cur_func->ns_text_start = text_base + s->n_value;
			cur_func->ns_lines = load_ns->ns_lines +
			    ((a1->x_sym.x_fcnary.x_fcn.x_lnnoptr - lnnoptr_base)
			     / LINESZ);
			end_func = a1->x_sym.x_fcnary.x_fcn.x_endndx;
			cur_func->ns_text_size = a1->x_sym.x_misc.x_lnsz.x_size;
		    }
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
			fprintf(stderr, "Missing aux entry for %d\n",
				sym_index);
			continue;
		    }
		    if (!cur_func) {
			fprintf(stderr, ".bf outside of function at %d\n",
				sym_index);
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
				size = sizeof(aux->x_file.x_fname);
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
		if (last_csect_index != s->n_value) {
		    if (s->n_value > sym_index) { /* forward reference */
			static_forward_reference = s->n_value;
		    } else {		/* backward confused reference */
			fprintf(stderr, "C_BSTAT is confused %d should be %d\n",
				last_csect_index, s->n_value);
		    }
		} else {		/* normal reference */
		    static_forward_reference = 0;
		    static_offset = last_csect->s_offset;
		}
		break;

	    case C_ESTAT:		/* 0 */
		break;

	    case C_LSYM:		/* offset in stack */
	    case C_PSYM:		/* offset in stack */
		/*
		 * The code below actually copes with this but it
		 * turns out that these clutter the symbol table and
		 * make life harder instead of easier.
		 */
		break;

	    case C_DECL:		/* undefined */
	    case C_GSYM:		/* undefined */
	    case C_FUN:			/* offset in csect */
	    case C_STSYM:		/* offset in csect */
		if (need_load) {
		    load_base_types(cur_file);
		    need_load = 0;
		}
		/*
		 * Hack number 44892: The dbx stab string for the
		 * function comes after the C_EXT which means that
		 * cur_block points to the function and not the file.
		 * So... in the case of a C_FUN, we pass cur_file to
		 * parse_stab instead of cur_block.  In pascal, this
		 * is going to break but... hahahahahahahaha.
		 */
		parse_stab(s->n_sclass == C_FUN ? cur_file : cur_block, name,
			   size, &dbx_sptr);

		/* These guys have an undefined n_value field */
		if (s->n_sclass == C_DECL || s->n_sclass == C_GSYM)
		    break;

		if (!dbx_sptr) {
		    fprintf(stderr, "Expected dbx_sptr to be set %d\n",
			    sym_index);
		    break;
		}
		if (s->n_sclass == C_FUN) {
		    offset = (void *)((int)last_code_csect->s_offset +
				      s->n_value);
		    dbx_sptr->s_global = 1;
		} else if (s->n_sclass == C_STSYM) {
		    if (static_forward_reference) {
			struct for_ref *temp = smalloc(sizeof(*temp));
			struct for_ref **pp, *p;

			temp->fr_next = 0;
			temp->fr_index = static_forward_reference;
			temp->fr_sym = dbx_sptr;
			/*
			 * Insert in order
			 */
			for (pp = &ref_head;
			     (p = *pp) && p->fr_index <= static_forward_reference;
			     pp = &p->fr_next);
			temp->fr_next = *pp;
			*pp = temp;

			offset = (void *)s->n_value;
		    } else {
			offset = (void *)((int)static_offset + s->n_value);
		    }
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
		if (dbx_sptr->s_defined && dbx_sptr->s_offset != offset)
		    fprintf(stderr, "Duplicate global symbol %s at %d %x != %x\n",
			    dbx_sptr->s_name, sym_index, dbx_sptr->s_offset,
			    offset);
		else {
		    dbx_sptr->s_offset = offset;
		    dbx_sptr->s_defined = 1;
		}
		break;

	    case C_BINCL:		/* Don't care about include files */
	    case C_EINCL:
		break;

	    default:
		fprintf(stderr, "Unknown sclass: %d\n", s->n_sclass);
		break;
	    }
	}
	if (ref_head)
	    fprintf(stderr, "ref_head still points to something.\n");

#undef Set_cur_block
    }
    (void) close(fd);
#ifdef USE_MMAP
    (void) munmap(m, sbuf.st_size);
#endif
}
