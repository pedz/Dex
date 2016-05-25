
/* @(#)cdt.h	1.1 */

/*
  inc past bitmap
  inc past page data
  inc past cdt_len (cdt header and entries)
  inc past cdt_entry (new)
*/

/* #define CDT_DEBUG */
#if 0
#include <sys/types.h>

typedef uint_t		vmhandle32_t;	/* 32bit size invariant vmhandle_t*/
typedef int             vmid32_t;       /* 32bit size-invariant vmid_t  */

#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/mman.h>
#include <string.h>
#include <stdlib.h>
#endif

class CDT {
public:
    /*
     * These methods represent what is in the cdt_head structures.  We
     * do not do header_len because the unlimited versions make it
     * unusable.
     */
    virtual void header_setup(void *) = 0;
    virtual int header_magic(void) = 0;
    virtual char *header_name(void) = 0;
    virtual int header_name_size(void) = 0;
    virtual char *header_type_string(void) = 0;
    virtual int header_num_entries(void) = 0;

    /*
     * These methods represent what is in the cdt_entry structures.
     * We don't use d_xmemp for anything.
     */
    virtual char *entry_name(void) = 0;
    virtual int entry_name_size(void) = 0;
    virtual unsigned long long entry_len(void) = 0;
    virtual __ptr64 entry_addr(void) = 0;
    virtual int entry_addr_size(void) = 0;
    virtual unsigned long long entry_segval(void) = 0;
    virtual int entry_segval_size(void) = 0;
    virtual int entry_isreal(void) = 0;
    virtual int entry_npages(u_longlong_t addr, size_t len) = 0;
    int entry_bitmapsize(u_longlong_t addr, size_t len)
    {
	return (entry_npages(addr, len) + BITS_BM - 1) / BITS_BM;
    }

    /*
     * Sequencing methods
     */
    virtual char *first_group(char *) = 0;
    virtual int more_groups(void) = 0;
    virtual char *next_group(char *) = 0;
    virtual char *first_entry(char *) = 0;
    virtual int more_entrys(void) = 0;
    virtual char *next_entry(char *) = 0;
};

class CDT_32 : public CDT {
private:
    struct cdt_32 {
	struct cdt_head _cdt_head;
	struct cdt_entry32 cdt_entry[1];
    };

    struct cdt_32 *head;
    struct cdt_entry32 *first;
    struct cdt_entry32 *entry;
    struct cdt_entry32 *last;
    int in_first_group;
    int num_entries;

public:
    void header_setup(void *p)
    {
#ifdef CDT_DEBUG
	printf("32 setup %d\n", sizeof(*first));
#endif
	head = (struct cdt_32 *)p;
	first = &head->cdt_entry[0];
	last = (struct cdt_entry32 *)((char *)p + head->cdt_len);
	num_entries = last - first;
    }					       

    int header_magic(void)
    {
	return head->cdt_magic;
    }

    char *header_name(void)
    {
	return head->cdt_name;
    }
	
    int header_name_size(void)
    {
	return sizeof(head->cdt_name);
    }

    char *header_type_string(void)
    {
	return "32";
    }

    int header_num_entries(void)
    {
	return num_entries;
    }

    char *entry_name(void)
    {
	return entry->d_name;
    }

    int entry_name_size(void)
    {
	return sizeof(entry->d_name);
    }

    unsigned long long entry_len(void)
    {
	return entry->d_len;
    }

    __ptr64 entry_addr(void)
    {
	return (__ptr64) entry->d_ptr;
    }

    int entry_addr_size(void)
    {
	return sizeof(entry->d_ptr);
    }

    unsigned long long entry_segval(void)
    {
	return entry->_d._d_segval;
    }

    int entry_segval_size(void)
    {
	return sizeof(entry->_d._d_segval);
    }

    int entry_isreal(void)
    {
	return entry->_d._d_segval == DUMP_REAL_SEGVAL32;
    }

    virtual int entry_npages(u_longlong_t addr, size_t len)
    {
	return NPAGES32(addr, len);
    }

    char *first_group(char *v)
    {
	in_first_group = 1;
#ifdef CDT_DEBUG	
	printf("first_group %s returning %08x\n", header_type_string(), last);
#endif
	return (char *)last;
    }

    int more_groups(void)
    {
	return in_first_group;
    }

    char *next_group(char *v)
    {
	in_first_group = 0;
#ifdef CDT_DEBUG
	printf("next_group %s returning %08x\n", header_type_string(), v);
#endif
	return v;
    }

    char *first_entry(char *v)
    {
	entry = first;
	return v;
    }

    int more_entrys(void)
    {
	return entry < last;
    }

    char *next_entry(char *v)
    {
	++entry;
	return v;
    }
};

class CDT_64 : public CDT {
private:
    struct cdt_64 {
	struct cdt_head _cdt_head;
	struct cdt_entry64 cdt_entry[1];
    };

    struct cdt_64 *head;
    struct cdt_entry64 *first;
    struct cdt_entry64 *entry;
    struct cdt_entry64 *last;
    int in_first_group;
    int num_entries;

public:
    void header_setup(void *p)
    {
#ifdef CDT_DEBUG
	printf("64 setup %d\n", sizeof(*first));
#endif
	head = (struct cdt_64 *)p;
	first = &head->cdt_entry[0];
	last = (struct cdt_entry64 *)((char *)p + head->cdt_len);
	num_entries = last - first;
    }					       

    int header_magic(void)
    {
	return head->cdt_magic;
    }

    char *header_name(void)
    {
	return head->cdt_name;
    }
	
    int header_name_size(void)
    {
	return sizeof(head->cdt_name);
    }

    char *header_type_string(void)
    {
	return "64";
    }

    int header_num_entries(void)
    {
	return num_entries;
    }

    char *entry_name(void)
    {
	return entry->d_name;
    }

    int entry_name_size(void)
    {
	return sizeof(entry->d_name);
    }

    unsigned long long entry_len(void)
    {
	return entry->d_len;
    }

    __ptr64 entry_addr(void)
    {
	return (__ptr64) entry->d_ptr;
    }

    int entry_addr_size(void)
    {
	return sizeof(entry->d_ptr);
    }

    unsigned long long entry_segval(void)
    {
	return entry->__d_segval;
    }

    int entry_segval_size(void)
    {
	return sizeof(entry->__d_segval);
    }

    int entry_isreal(void)
    {
	return entry->__d_segval == DUMP_REAL_SEGVAL64;
    }

    virtual int entry_npages(u_longlong_t addr, size_t len)
    {
	return NPAGES64(addr, len);
    }

    char *first_group(char *v)
    {
	in_first_group = 1;
#ifdef CDT_DEBUG
	printf("first_group %s returning %08x\n", header_type_string(), last);
#endif
	return (char *)last;
    }

    int more_groups(void)
    {
	return in_first_group;
    }

    char *next_group(char *v)
    {
	in_first_group = 0;
#ifdef CDT_DEBUG
	printf("next_group %s returning %08x\n", header_type_string(), v);
#endif
	return v;
    }

    char *first_entry(char *v)
    {
	entry = first;
	return v;
    }

    int more_entrys(void)
    {
	return entry < last;
    }

    char *next_entry(char *v)
    {
	++entry;
	return v;
    }
};

class CDT_vr : public CDT {
private:
    struct cdt_vr *head;
    struct cdt_entry_vr *first;
    struct cdt_entry_vr *entry;
    struct cdt_entry_vr *last;
    int in_first_group;
    int num_entries;

public:
    void header_setup(void *p)
    {
#ifdef CDT_DEBUG
	printf("vr setup %d\n", sizeof(*first));
#endif
	head = (struct cdt_vr *)p;
	first = &head->cdt_entry[0];
	last = (struct cdt_entry_vr *)((char *)p + head->cdt_len);
	num_entries = last - first;
    }					       

    int header_magic(void)
    {
	return head->cdt_magic;
    }

    char *header_name(void)
    {
	return head->cdt_name;
    }
	
    int header_name_size(void)
    {
	return sizeof(head->cdt_name);
    }

    char *header_type_string(void)
    {
	return "vr";
    }

    int header_num_entries(void)
    {
	return num_entries;
    }

    char *entry_name(void)
    {
	return entry->d_name;
    }

    int entry_name_size(void)
    {
	return sizeof(entry->d_name);
    }

    unsigned long long entry_len(void)
    {
	return entry->d_len;
    }

    __ptr64 entry_addr(void)
    {
	return ((entry->d_addrflags & D_REAL) ?
		(__ptr64) entry->d_realaddr :
		(__ptr64) entry->d_ptr_v);
    }

    int entry_addr_size(void)
    {
	return (entry->d_addrflags & D_REAL) ?
	    sizeof(entry->d_realaddr) : sizeof(entry->d_ptr_v);
    }

    unsigned long long entry_segval(void)
    {
	return (entry->d_addrflags & D_REAL) ? 0 :
	    entry->d_segval_v;
    }

    int entry_segval_size(void)
    {
	return sizeof(entry->d_segval_v);
    }

    int entry_isreal(void)
    {
	return entry->d_addrflags & D_REAL;
    }

    virtual int entry_npages(u_longlong_t addr, size_t len)
    {
	return NPAGES_VR((entry->d_addrflags & D_REAL), addr, len);
    }

    char *first_group(char *v)
    {
	in_first_group = 1;
#ifdef CDT_DEBUG
	printf("first_group %s returning %08x\n", header_type_string(), last);
#endif
	return (char *)last;
    }

    int more_groups(void)
    {
	return in_first_group;
    }

    char *next_group(char *v)
    {
	in_first_group = 0;
#ifdef CDT_DEBUG
	printf("next_group %s returning %08x\n", header_type_string(), v);
#endif
	return v;
    }

    char *first_entry(char *v)
    {
	entry = first;
	return v;
    }

    int more_entrys(void)
    {
	return entry < last;
    }

    char *next_entry(char *v)
    {
	++entry;
	return v;
    }
};

class CDT_u32 : public CDT {
private:
    struct cdt_u *head;
    struct cdt_entry_u *first;
    struct cdt_entry_u *entry;
    struct cdt_entry_u *last;
    int in_first_entry;
    int num_entries;

public:
    void header_setup(void *p)
    {
#ifdef CDT_DEBUG
	printf("u32 setup %d\n", sizeof(*first));
#endif
	head = (struct cdt_u *)p;
	first = &head->cdtu_entry[0];
	num_entries = head->cdtu_head.cdtu_nentries;
	last = &head->cdtu_entry[num_entries];
    }					       

    int header_magic(void)
    {
	return head->cdtu_head.cdtu_magic;
    }

    char *header_name(void)
    {
	return head->cdtu_head.cdtu_name;
    }
	
    int header_name_size(void)
    {
	return sizeof(head->cdtu_head.cdtu_name);
    }

    char *header_type_string(void)
    {
	return "u32";
    }

    int header_num_entries(void)
    {
	return num_entries;
    }

    char *entry_name(void)
    {
	return entry->du_name;
    }

    int entry_name_size(void)
    {
	return sizeof(entry->du_name);
    }

    unsigned long long entry_len(void)
    {
	return entry->du_len;
    }

    __ptr64 entry_addr(void)
    {
	return entry->du_ptr;
    }

    int entry_addr_size(void)
    {
	return sizeof(entry->du_ptr);
    }

    unsigned long long entry_segval(void)
    {
	return entry->du_segval;
    }

    int entry_segval_size(void)
    {
	return sizeof(entry->du_segval);
    }

    int entry_isreal(void)
    {
	return entry->du_segval == DUMP_REAL_SEGVAL32;
    }

    virtual int entry_npages(u_longlong_t addr, size_t len)
    {
	return NPAGES64(addr, len);
    }

    char *first_group(char *v)
    {
#ifdef CDT_DEBUG
	printf("first_group %s returning %08x\n", header_type_string(),
	       first + 1);
#endif
	return (char *)(more_groups() ? (first + 1) : first);
    }

    int more_groups(void)
    {
	return first->du_magic == DMP_MAGIC_UD32;
    }

    char *next_group(char *v)
    {
	first = (struct cdt_entry_u *)v;
#ifdef CDT_DEBUG
	printf("next_group %s returning %08x\n", header_type_string(),
	       first + 1);
#endif
	return more_groups() ? (char *)(first + 1) : v;
    }

    char *first_entry(char *v)
    {
	entry = first;
	in_first_entry = 1;
	return v;
    }

    int more_entrys(void)
    {
	return in_first_entry;
    }

    char *next_entry(char *v)
    {
	in_first_entry = 0;
	return v;
    }
};

class CDT_u64 : public CDT {
private:
    struct cdt_u *head;
    struct cdt_entry_u *first;
    struct cdt_entry_u *entry;
    struct cdt_entry_u *last;
    int in_first_entry;
    int num_entries;

public:
    void header_setup(void *p)
    {
#ifdef CDT_DEBUG
	printf("u64 setup %d\n", sizeof(*first));
#endif
	head = (struct cdt_u *)p;
	first = &head->cdtu_entry[0];
	num_entries = head->cdtu_head.cdtu_nentries;
	last = &head->cdtu_entry[num_entries];
    }					       

    int header_magic(void)
    {
	return head->cdtu_head.cdtu_magic;
    }

    char *header_name(void)
    {
	return head->cdtu_head.cdtu_name;
    }
	
    int header_name_size(void)
    {
	return sizeof(head->cdtu_head.cdtu_name);
    }

    char *header_type_string(void)
    {
	return "u64";
    }

    int header_num_entries(void)
    {
	return num_entries;
    }

    char *entry_name(void)
    {
	return entry->du_name;
    }

    int entry_name_size(void)
    {
	return sizeof(entry->du_name);
    }

    unsigned long long entry_len(void)
    {
	return entry->du_len;
    }

    __ptr64 entry_addr(void)
    {
	return entry->du_ptr;
    }

    int entry_addr_size(void)
    {
	return sizeof(entry->du_ptr);
    }

    unsigned long long entry_segval(void)
    {
	return entry->du_segval;
    }

    int entry_segval_size(void)
    {
	return sizeof(entry->du_segval);
    }

    int entry_isreal(void)
    {
	return entry->du_segval == DUMP_REAL_SEGVAL64;
    }

    virtual int entry_npages(u_longlong_t addr, size_t len)
    {
	return NPAGES64(addr, len);
    }

    char *first_group(char *v)
    {
#ifdef CDT_DEBUG
	printf("first_group %s returning %08x\n", header_type_string(),
	       first + 1);
#endif
	return (char *)(more_groups() ? (first + 1) : first);
    }

    int more_groups(void)
    {
	return first->du_magic == DMP_MAGIC_UD64;
    }

    char *next_group(char *v)
    {
	first = (struct cdt_entry_u *)v;
#ifdef CDT_DEBUG
	printf("next_group %s returning %08x\n", header_type_string(),
	       first + 1);
#endif
	return more_groups() ? (char *)(first + 1) : v;
    }

    char *first_entry(char *v)
    {
	entry = first;
	in_first_entry = 1;
	return v;
    }

    int more_entrys(void)
    {
	return in_first_entry;
    }

    char *next_entry(char *v)
    {
	in_first_entry = 0;
	return v;
    }
};


class CDT_ras : public CDT {
private:
    struct cdt_head_ras *head;
    struct cdt_entry64 *first;
    struct cdt_entry64 *entry;
    struct cdt_entry64 *last;
    int in_first_group;
    int num_entries;

public:
    void header_setup(void *p)
    {
#ifdef CDT_DEBUG
	printf("ras setup %d\n", sizeof(*first));
#endif
	head = (struct cdt_head_ras *)p;
	first = (struct cdt_entry64 *)(((char *)p) + sizeof(struct cdt_head_ras_h) + head->cdtr_pathlen);
	last = (struct cdt_entry64 *)((char *)p + head->cdtr_len);
	printf("NUM_ENTRIES_RAS(cp) = %d\n", NUM_ENTRIES_RAS(p));
	num_entries = last - first;
    }					       

    int header_magic(void)
    {
	return head->cdtr_magic;
    }

    char *header_name(void)
    {
	return head->cdtr_pathname;
    }
	
    int header_name_size(void)
    {
	return head->cdtr_pathlen;
    }

    char *header_type_string(void)
    {
	return "ras";
    }

    int header_num_entries(void)
    {
	return num_entries;
    }

    char *entry_name(void)
    {
	return entry->d_name;
    }

    int entry_name_size(void)
    {
	return sizeof(entry->d_name);
    }

    unsigned long long entry_len(void)
    {
	return entry->d_len;
    }

    __ptr64 entry_addr(void)
    {
	return (__ptr64) entry->d_ptr;
    }

    int entry_addr_size(void)
    {
	return sizeof(entry->d_ptr);
    }

    unsigned long long entry_segval(void)
    {
	return entry->__d_segval;
    }

    int entry_segval_size(void)
    {
	return sizeof(entry->__d_segval);
    }

    int entry_isreal(void)
    {
	return entry->__d_segval == DUMP_REAL_SEGVAL64;
    }

    virtual int entry_npages(u_longlong_t addr, size_t len)
    {
	return NPAGES64(addr, len);
    }

    char *first_group(char *v)
    {
	in_first_group = 1;
#ifdef CDT_DEBUG
	printf("first_group %s returning %08x\n", header_type_string(), last);
#endif
	return (char *)last;
    }

    int more_groups(void)
    {
	return in_first_group;
    }

    char *next_group(char *v)
    {
	in_first_group = 0;
#ifdef CDT_DEBUG
	printf("next_group %s returning %08x\n", header_type_string(), v);
#endif
	return v;
    }

    char *first_entry(char *v)
    {
	entry = first;
	return v;
    }

    int more_entrys(void)
    {
	return entry < last;
    }

    char *next_entry(char *v)
    {
	++entry;
	return v;
    }
};

class CDT_uras : public CDT {
private:
    struct cdt_head_ras_u *head;
    struct cdt_entry_u *first;
    struct cdt_entry_u *entry;
    struct cdt_entry_u *last;
    int in_first_entry;
    int num_entries;

public:
    void header_setup(void *p)
    {
#ifdef CDT_DEBUG
	printf("uras setup %d\n", sizeof(*first));
#endif
	head = (struct cdt_head_ras_u *)p;
	first = (struct cdt_entry_u *)(((char *)p) + sizeof(struct cdt_head_ras_uh) + head->cdtru_pathlen);
	num_entries = head->cdtru_nentries;
	last = first + num_entries;
    }					       

    int header_magic(void)
    {
	return head->cdtru_magic;
    }

    char *header_name(void)
    {
	return head->cdtru_pathname;
    }
	
    int header_name_size(void)
    {
	return head->cdtru_pathlen;
    }

    char *header_type_string(void)
    {
	return "uras";
    }

    int header_num_entries(void)
    {
	return num_entries;
    }

    char *entry_name(void)
    {
	return entry->du_name;
    }

    int entry_name_size(void)
    {
	return sizeof(entry->du_name);
    }

    unsigned long long entry_len(void)
    {
	return entry->du_len;
    }

    __ptr64 entry_addr(void)
    {
	return entry->du_ptr;
    }

    int entry_addr_size(void)
    {
	return sizeof(entry->du_ptr);
    }

    unsigned long long entry_segval(void)
    {
	return entry->du_segval;
    }

    int entry_segval_size(void)
    {
	return sizeof(entry->du_segval);
    }

    int entry_isreal(void)
    {
	return entry->du_segval == DUMP_REAL_SEGVAL64;
    }

    virtual int entry_npages(u_longlong_t addr, size_t len)
    {
	return NPAGES64(addr, len);
    }

    char *first_group(char *v)
    {
#ifdef CDT_DEBUG
	printf("first_group %s returning %08x\n", header_type_string(),
	       first + 1);
#endif
	return (char *)(more_groups() ? (first + 1) : first);
    }

    int more_groups(void)
    {
	return first->du_magic == DMP_MAGIC_UD64;
    }

    char *next_group(char *v)
    {
	first = (struct cdt_entry_u *)v;
#ifdef CDT_DEBUG
	printf("next_group %s returning %08x\n", header_type_string(),
	       first + 1);
#endif
	return more_groups() ? (char *)(first + 1) : v;
    }

    char *first_entry(char *v)
    {
	entry = first;
	in_first_entry = 1;
	return v;
    }

    int more_entrys(void)
    {
	return in_first_entry;
    }

    char *next_entry(char *v)
    {
	in_first_entry = 0;
	return v;
    }
};
