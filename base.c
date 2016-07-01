
/* @(#)base.c	1.3 */

#include "sym.h"
#include <sys/debug.h>

#ifndef _KERNEL
#define _KERNEL
#endif

#ifndef KERNEL
#define KERNEL
#endif

/*
 * The _system_configuration structure has no name and is declared as
 * extern in systemcfg.h.  So we #define extern as typedef and include
 * systemcfg.h.  This has the effect of creating a typedef called
 * _system_configuration which we can then manipulate in the pseudo
 * code.
 */
#define extern typedef
#define _system_configuration system_configuration_t
#include <sys/systemcfg.h>
#undef extern
#undef _system_configuration
extern system_configuration_t _system_configuration;

#include <sys/types.h>
#include <sys/malloc.h>
#include <sys/lock_def.h>
#ifndef _KERNSYS
#define _KERNSYS
#endif
#include <sys/ppda.h>
#include <sys/pnda.h>

#ifdef __64BIT__
#define __XCOFF64__
#endif

#include <a.out.h>
#include <sys/ldr/ld_data.h>
#include <pse/str_stream.h>
#ifdef ALIGN
#undef ALIGN
#endif

#include <sys/stream.h>
#ifdef reg
#undef reg
#endif

#include <sys/file.h>
#include <sys/vnode.h>
#include <sys/socketvar.h>
#include <sys/un.h>
#include <sys/unpcb.h>
#include <sys/socket.h>
#include <sys/protosw.h>
#include <net/if.h>
#include <sys/domain.h>
#include <net/route.h>
#include <net/netisr.h>
#include <sys/mbuf.h>
#include <termios.h>
#include <netinet/ip_var.h>
#include <netinet/udp.h>
#include <netinet/udp_var.h>
#include <netinet/tcp_var.h>
#include <netinet/if_ether.h>
#include <netinet/if_ether6.h>
#include <sys/unpcb.h>
#include <sys/lockname.h>
#ifdef PS_ID_LOCK
#include <sys/pollset.h>
#endif
#include <sys/selpoll.h>

#include <vmm/vmdefs.h>
#include <vmm/vmpft.h>
#include <vmm/vmscb.h>
#include <vmm/m_vmpft.h>
#include <lfs/lfs_numa.h>
#include <ras/ras_priv.h>
#include <sys/vmker.h>
#include <net/netopt.h>
#include <net/net_malloc.h>
#include <sctp_var.h>

typedef unsigned char   uint8;
typedef unsigned short  uint16;
typedef unsigned int    uint32;

typedef struct nlchash {
	struct nlcbuf	*nlc_forw;	/* head buffer in hashlist */
	struct nlcbuf	*nlc_back;	/* tail buffer in hashlist */
	Simple_lock	 nlc_lock;	/* lock per hash anchor    */
} nlchash_t;

typedef struct nlcbuf {
	struct nlcbuf	*nlc_forw;	/* head buffer in hash		    */
	struct nlcbuf	*nlc_back;	/* tail buffer in hash		    */
	struct vfs	*nlc_vfsp;	/* vfs pointer of node		    */

	void		*nlc_dp;	/* ptr to parent node of entry      */
	uint32		nlc_did;	/* parent filenode cap id ptr       */

	void		*nlc_np;	/* node ptr of cached entry	    */
	uint32		*nlc_nidp;	/* node capability id ptr           */
	uint32		nlc_nid;	/* node cap. id -- value at time
					   of entry */

	uint32		nlc_namelen;	/* name length			    */
	union {
		char	_namea[32];
					/* static fast name <=NLC_STATICLEN */
		char	*_namep;	/* alloc'd slow name >NLC_STATICLEN */
	} _name;			/* component name		    */
} nlcbuf_t;

#define MAX_ALLOC_INDEX		495
#define MAX_LEAF_INDEX		495
#define MAX_ID			(MAX_ALLOC_INDEX*MAX_LEAF_INDEX)

typedef struct {
	ushort psl_free;
	/* space for one extra bit */
	char psl_alloc[(MAX_LEAF_INDEX+NBPB)/NBPB];
	pcache_t *psl_pollcache[MAX_LEAF_INDEX];
}ps_leaf_node_t;
typedef struct {
	char psa_alloc[(MAX_ALLOC_INDEX+NBPB)/NBPB];
	ps_leaf_node_t *psa_leaf_node[MAX_ALLOC_INDEX];
}ps_alloc_node_t;


#define DEBUG_BIT BASE_C_BIT

#include <stdio.h>

/*
 * -qdbxextra causes the compiler to core dump so we force some key
 * data structures out into the object file.
 */
rpn64_t fx;
struct p64pte v0;
struct scb v0_1;
ppftfl_t v1;
pnft_t v2;
ppltfl_t v3;
pnlt_t v4;

int main(int argc, char *argv[])
{
    unsigned long ul;
    int multi_1tb_ksp_segs;

#ifdef AMESIDX_F
    ul = AMESIDX_F;
    printf("unsigned long AMESIDX_F = %#lx; /* %d */\n", ul, ul);
#endif /* AMESIDX_F */

#ifdef AMESIDX_L
    ul = AMESIDX_L;
    printf("unsigned long AMESIDX_L = %#lx; /* %d */\n", ul, ul);
#endif /* AMESIDX_L */

#ifdef AMESR
    ul = AMESR;
    printf("unsigned long AMESR = %#lx; /* %d */\n", ul, ul);
#endif /* AMESR */

#ifdef APTNULL
    ul = APTNULL;
    printf("unsigned long APTNULL = %#lx; /* %d */\n", ul, ul);
#endif /* APTNULL */

#ifdef APTPIN
    ul = APTPIN;
    printf("unsigned long APTPIN = %#lx; /* %d */\n", ul, ul);
#endif /* APTPIN */

#ifdef APTREG
    ul = APTREG;
    printf("unsigned long APTREG = %#lx; /* %d */\n", ul, ul);
#endif /* APTREG */

#ifdef BASE_EXTSCB
    ul = BASE_EXTSCB;
    printf("unsigned long BASE_EXTSCB = %#lx; /* %d */\n", ul, ul);
#endif /* BASE_EXTSCB */

#ifdef BKTPFAULT
    ul = BKTPFAULT;
    printf("unsigned long BKTPFAULT = %#lx; /* %d */\n", ul, ul);
#endif /* BKTPFAULT */

#if defined(CUR_PTA_no)
    ul = CUR_PTA;
    printf("unsigned long CUR_PTA = %#lx; /* %d */\n", ul, ul);
#endif /* CUR_PTA */

#ifdef DMAPSIDX
    ul = DMAPSIDX;
    printf("unsigned long DMAPSIDX = %#lx; /* %d */\n", ul, ul);
#endif /* DMAPSIDX */

#ifdef EHANDLE
    ul = EHANDLE;
    printf("unsigned long EHANDLE = %#lx; /* %d */\n", ul, ul);
#endif /* EHANDLE */

#ifdef EISR
    ul = EISR;
    printf("unsigned long EISR = %#lx; /* %d */\n", ul, ul);
#endif /* EISR */

#ifdef EORGVADDR
    ul = EORGVADDR;
    printf("unsigned long EORGVADDR = %#lx; /* %d */\n", ul, ul);
#endif /* EORGVADDR */

#ifdef EPFTYPE
    ul = EPFTYPE;
    printf("unsigned long EPFTYPE = %#lx; /* %d */\n", ul, ul);
#endif /* EPFTYPE */

#ifdef ESRVAL
    ul = ESRVAL;
    printf("unsigned long ESRVAL = %#lx; /* %d */\n", ul, ul);
#endif /* ESRVAL */

#ifdef EVADDR
    ul = EVADDR;
    printf("unsigned long EVADDR = %#lx; /* %d */\n", ul, ul);
#endif /* EVADDR */

#ifdef EXTSCB_MASK
    ul = EXTSCB_MASK;
    printf("unsigned long EXTSCB_MASK = %#lx; /* %d */\n", ul, ul);
#endif /* EXTSCB_MASK */

#ifdef FAIL_IF_HIDDEN
    ul = FAIL_IF_HIDDEN;
    printf("unsigned long FAIL_IF_HIDDEN = %#lx; /* %d */\n", ul, ul);
#endif /* FAIL_IF_HIDDEN */

#ifdef FBANCH
    ul = FBANCH;
    printf("unsigned long FBANCH = %#lx; /* %d */\n", ul, ul);
#endif /* FBANCH */

#ifdef FBLOWLIM
    ul = FBLOWLIM;
    printf("unsigned long FBLOWLIM = %#lx; /* %d */\n", ul, ul);
#endif /* FBLOWLIM */

#ifdef FBLRU
    ul = FBLRU;
    printf("unsigned long FBLRU = %#lx; /* %d */\n", ul, ul);
#endif /* FBLRU */

#ifdef FBLRU_REREF
    ul = FBLRU_REREF;
    printf("unsigned long FBLRU_REREF = %#lx; /* %d */\n", ul, ul);
#endif /* FBLRU_REREF */

#ifdef FBMAX
    ul = FBMAX;
    printf("unsigned long FBMAX = %#lx; /* %d */\n", ul, ul);
#endif /* FBMAX */

#ifdef FBUPLIM
    ul = FBUPLIM;
    printf("unsigned long FBUPLIM = %#lx; /* %d */\n", ul, ul);
#endif /* FBUPLIM */

#ifdef FIND_HIDDEN
    ul = FIND_HIDDEN;
    printf("unsigned long FIND_HIDDEN = %#lx; /* %d */\n", ul, ul);
#endif /* FIND_HIDDEN */

#ifdef FROZEN
    ul = FROZEN;
    printf("unsigned long FROZEN = %#lx; /* %d */\n", ul, ul);
#endif /* FROZEN */

#ifdef GBITSPERDW
    ul = GBITSPERDW;
    printf("unsigned long GBITSPERDW = %#lx; /* %d */\n", ul, ul);
#endif /* GBITSPERDW */

#ifdef GDWSPERMAP
    ul = GDWSPERMAP;
    printf("unsigned long GDWSPERMAP = %#lx; /* %d */\n", ul, ul);
#endif /* GDWSPERMAP */

#ifdef GL2BITSPERDW
    ul = GL2BITSPERDW;
    printf("unsigned long GL2BITSPERDW = %#lx; /* %d */\n", ul, ul);
#endif /* GL2BITSPERDW */

#ifdef GLOB_ESID_FIRST
    ul = GLOB_ESID_FIRST;
    printf("unsigned long GLOB_ESID_FIRST = %#lx; /* %d */\n", ul, ul);
#endif /* GLOB_ESID_FIRST */

#ifdef GLOB_ESID_LAST
    ul = GLOB_ESID_LAST;
    printf("unsigned long GLOB_ESID_LAST = %#lx; /* %d */\n", ul, ul);
#endif /* GLOB_ESID_LAST */

#ifdef GLOB_EXTREG_ESID_FIRST
    ul = GLOB_EXTREG_ESID_FIRST;
    printf("unsigned long GLOB_EXTREG_ESID_FIRST = %#lx; /* %d */\n", ul, ul);
#endif /* GLOB_EXTREG_ESID_FIRST */

#ifdef GLOB_EXTREG_ESID_LAST
    ul = GLOB_EXTREG_ESID_LAST;
    printf("unsigned long GLOB_EXTREG_ESID_LAST = %#lx; /* %d */\n", ul, ul);
#endif /* GLOB_EXTREG_ESID_LAST */

#ifdef GMAPSIZE
    ul = GMAPSIZE;
    printf("unsigned long GMAPSIZE = %#lx; /* %d */\n", ul, ul);
#endif /* GMAPSIZE */

#ifdef GROWPGAHEAD
    ul = GROWPGAHEAD;
    printf("unsigned long GROWPGAHEAD = %#lx; /* %d */\n", ul, ul);
#endif /* GROWPGAHEAD */

#ifdef HALT_NOMEM
    ul = HALT_NOMEM;
    printf("unsigned long HALT_NOMEM = %#lx; /* %d */\n", ul, ul);
#endif /* HALT_NOMEM */

#ifdef HIGH_USER_ESID_FIRST
    ul = HIGH_USER_ESID_FIRST;
    printf("unsigned long HIGH_USER_ESID_FIRST = %#lx; /* %d */\n", ul, ul);
#endif /* HIGH_USER_ESID_FIRST */

#ifdef HIGH_USER_ESID_LAST
    ul = HIGH_USER_ESID_LAST;
    printf("unsigned long HIGH_USER_ESID_LAST = %#lx; /* %d */\n", ul, ul);
#endif /* HIGH_USER_ESID_LAST */

#ifdef INSPFT
    ul = INSPFT;
    printf("unsigned long INSPFT = %#lx; /* %d */\n", ul, ul);
#endif /* INSPFT */

#if defined(INVALID_HANDLE) && !defined(__64BIT__)
    ul = INVALID_HANDLE;
    printf("unsigned long INVALID_HANDLE = %#lx; /* %d */\n", ul, ul);
#endif /* INVALID_HANDLE */

#ifdef IODONE
    ul = IODONE;
    printf("unsigned long IODONE = %#lx; /* %d */\n", ul, ul);
#endif /* IODONE */

#ifdef IOFORCESYNC
    ul = IOFORCESYNC;
    printf("unsigned long IOFORCESYNC = %#lx; /* %d */\n", ul, ul);
#endif /* IOFORCESYNC */

#ifdef IOSEGMENT
    ul = IOSEGMENT;
    printf("unsigned long IOSEGMENT = %#lx; /* %d */\n", ul, ul);
#endif /* IOSEGMENT */

#ifdef IOSIDBIT
    ul = IOSIDBIT;
    printf("unsigned long IOSIDBIT = %#lx; /* %d */\n", ul, ul);
#endif /* IOSIDBIT */

#ifdef IOSIDMASK
    ul = IOSIDMASK;
    printf("unsigned long IOSIDMASK = %#lx; /* %d */\n", ul, ul);
#endif /* IOSIDMASK */

#ifdef IOSYNC
    ul = IOSYNC;
    printf("unsigned long IOSYNC = %#lx; /* %d */\n", ul, ul);
#endif /* IOSYNC */

#ifdef JFS_LKW_ESID_FIRST
    ul = JFS_LKW_ESID_FIRST;
    printf("unsigned long JFS_LKW_ESID_FIRST = %#lx; /* %d */\n", ul, ul);
#endif /* JFS_LKW_ESID_FIRST */

#ifdef JFS_LKW_ESID_LAST
    ul = JFS_LKW_ESID_LAST;
    printf("unsigned long JFS_LKW_ESID_LAST = %#lx; /* %d */\n", ul, ul);
#endif /* JFS_LKW_ESID_LAST */

#ifdef JFS_SEG_ESID_FIRST
    ul = JFS_SEG_ESID_FIRST;
    printf("unsigned long JFS_SEG_ESID_FIRST = %#lx; /* %d */\n", ul, ul);
#endif /* JFS_SEG_ESID_FIRST */

#ifdef JFS_SEG_ESID_LAST
    ul = JFS_SEG_ESID_LAST;
    printf("unsigned long JFS_SEG_ESID_LAST = %#lx; /* %d */\n", ul, ul);
#endif /* JFS_SEG_ESID_LAST */

#ifdef KERNEL_ESID
    ul = KERNEL_ESID;
    printf("unsigned long KERNEL_ESID = %#lx; /* %d */\n", ul, ul);
#endif /* KERNEL_ESID */

#ifdef KERNEL_HEAP_ESID_FIRST
    ul = KERNEL_HEAP_ESID_FIRST;
    printf("unsigned long KERNEL_HEAP_ESID_FIRST = %#lx; /* %d */\n", ul, ul);
#endif /* KERNEL_HEAP_ESID_FIRST */

#ifdef KERNEL_HEAP_ESID_LAST
    ul = KERNEL_HEAP_ESID_LAST;
    printf("unsigned long KERNEL_HEAP_ESID_LAST = %#lx; /* %d */\n", ul, ul);
#endif /* KERNEL_HEAP_ESID_LAST */

#ifdef KERNSR
    ul = KERNSR;
    printf("unsigned long KERNSR = %#lx; /* %d */\n", ul, ul);
#endif /* KERNSR */

#ifdef KERNXSR
    ul = KERNXSR;
    printf("unsigned long KERNXSR = %#lx; /* %d */\n", ul, ul);
#endif /* KERNXSR */

#ifdef KERN_THRD_ESID_FIRST
    ul = KERN_THRD_ESID_FIRST;
    printf("unsigned long KERN_THRD_ESID_FIRST = %#lx; /* %d */\n", ul, ul);
#endif /* KERN_THRD_ESID_FIRST */

#ifdef KERN_THRD_ESID_LAST
    ul = KERN_THRD_ESID_LAST;
    printf("unsigned long KERN_THRD_ESID_LAST = %#lx; /* %d */\n", ul, ul);
#endif /* KERN_THRD_ESID_LAST */

#ifdef KERSIDX
    ul = KERSIDX;
    printf("unsigned long KERSIDX = %#lx; /* %d */\n", ul, ul);
#endif /* KERSIDX */

#ifdef KEXTSIDX
    ul = KEXTSIDX;
    printf("unsigned long KEXTSIDX = %#lx; /* %d */\n", ul, ul);
#endif /* KEXTSIDX */

#ifdef KGLOB_ESID_BASE
    ul = KGLOB_ESID_BASE;
    printf("unsigned long KGLOB_ESID_BASE = %#lx; /* %d */\n", ul, ul);
#endif /* KGLOB_ESID_BASE */

#ifdef KPRIV_ESID_BASE
    ul = KPRIV_ESID_BASE;
    printf("unsigned long KPRIV_ESID_BASE = %#lx; /* %d */\n", ul, ul);
#endif /* KPRIV_ESID_BASE */

    multi_1tb_ksp_segs = -1;
#ifdef KSP_ARCH_ESID_BASE
    ul = KSP_ARCH_ESID_BASE;
    printf("unsigned long KSP_ARCH_ESID_BASE = %#lx; /* %d */\n", ul, ul);
#endif /* KSP_ARCH_ESID_BASE */

#ifdef KSP_ARCH_ESID_END
    ul = KSP_ARCH_ESID_END;
    printf("unsigned long KSP_ARCH_ESID_END = %#lx; /* %d */\n", ul, ul);
#endif /* KSP_ARCH_ESID_END */

#ifdef KSP_ARCH_NUMSIDS
    ul = KSP_ARCH_NUMSIDS;
    printf("unsigned long KSP_ARCH_NUMSIDS = %#lx; /* %d */\n", ul, ul);
#endif /* KSP_ARCH_NUMSIDS */

#ifdef KSP_ESID_BASE
    ul = KSP_ESID_BASE;
    printf("unsigned long KSP_ESID_BASE = %#lx; /* %d */\n", ul, ul);
#endif /* KSP_ESID_BASE */

#ifdef KSP_ESID_END
    ul = KSP_ESID_END;
    printf("unsigned long KSP_ESID_END = %#lx; /* %d */\n", ul, ul);
#endif /* KSP_ESID_END */

#ifdef KSP_NUMSIDS
    ul = KSP_NUMSIDS;
    printf("unsigned long KSP_NUMSIDS = %#lx; /* %d */\n", ul, ul);
#endif /* KSP_NUMSIDS */

#ifdef KSP_SID_BASE
    ul = KSP_SID_BASE;
    printf("unsigned long KSP_SID_BASE = %#lx; /* %d */\n", ul, ul);
#endif /* KSP_SID_BASE */

#ifdef KSP_S_ARCH_ESID_BASE
    ul = KSP_S_ARCH_ESID_BASE;
    printf("unsigned long KSP_S_ARCH_ESID_BASE = %#lx; /* %d */\n", ul, ul);
#endif /* KSP_S_ARCH_ESID_BASE */

#ifdef KSP_S_ARCH_ESID_END
    ul = KSP_S_ARCH_ESID_END;
    printf("unsigned long KSP_S_ARCH_ESID_END = %#lx; /* %d */\n", ul, ul);
#endif /* KSP_S_ARCH_ESID_END */

#ifdef KSP_S_ARCH_NUMSIDS
    ul = KSP_S_ARCH_NUMSIDS;
    printf("unsigned long KSP_S_ARCH_NUMSIDS = %#lx; /* %d */\n", ul, ul);
#endif /* KSP_S_ARCH_NUMSIDS */

#ifdef KSP_S_ESID_BASE
    ul = KSP_S_ESID_BASE;
    printf("unsigned long KSP_S_ESID_BASE = %#lx; /* %d */\n", ul, ul);
#endif /* KSP_S_ESID_BASE */

#ifdef KSP_S_ESID_END
    ul = KSP_S_ESID_END;
    printf("unsigned long KSP_S_ESID_END = %#lx; /* %d */\n", ul, ul);
#endif /* KSP_S_ESID_END */

#ifdef KSP_S_NUMSIDS
    ul = KSP_S_NUMSIDS;
    printf("unsigned long KSP_S_NUMSIDS = %#lx; /* %d */\n", ul, ul);
#endif /* KSP_S_NUMSIDS */

#ifdef KSP_TOTAL_SIDS
    ul = KSP_TOTAL_SIDS;
    printf("unsigned long KSP_TOTAL_SIDS = %#lx; /* %d */\n", ul, ul);
#endif /* KSP_TOTAL_SIDS */

#ifdef KTEMP_ESID_BASE
    ul = KTEMP_ESID_BASE;
    printf("unsigned long KTEMP_ESID_BASE = %#lx; /* %d */\n", ul, ul);
#endif /* KTEMP_ESID_BASE */

#ifdef L2LINESIZE
    ul = L2LINESIZE;
    printf("unsigned long L2LINESIZE = %#lx; /* %d */\n", ul, ul);
#endif /* L2LINESIZE */

#ifdef L2PSIZE
    ul = L2PSIZE;
    printf("unsigned long L2PSIZE = %#lx; /* %d */\n", ul, ul);
#endif /* L2PSIZE */

#ifdef L2SSIZE
    ul = L2SSIZE;
    printf("unsigned long L2SSIZE = %#lx; /* %d */\n", ul, ul);
#endif /* L2SSIZE */

#ifdef LDR_LIB_ESID_FIRST
    ul = LDR_LIB_ESID_FIRST;
    printf("unsigned long LDR_LIB_ESID_FIRST = %#lx; /* %d */\n", ul, ul);
#endif /* LDR_LIB_ESID_FIRST */

#ifdef LDR_LIB_ESID_LAST
    ul = LDR_LIB_ESID_LAST;
    printf("unsigned long LDR_LIB_ESID_LAST = %#lx; /* %d */\n", ul, ul);
#endif /* LDR_LIB_ESID_LAST */

#ifdef LDR_PRIV_ADDR
    ul = LDR_PRIV_ADDR;
    printf("unsigned long LDR_PRIV_ADDR = %#lx; /* %d */\n", ul, ul);
#endif /* LDR_PRIV_ADDR */

#ifdef LDR_PRIV_ESID
    ul = LDR_PRIV_ESID;
    printf("unsigned long LDR_PRIV_ESID = %#lx; /* %d */\n", ul, ul);
#endif /* LDR_PRIV_ESID */

#ifdef LFS_ESID_RANGE
    ul = LFS_ESID_RANGE;
    printf("unsigned long LFS_ESID_RANGE = %#lx; /* %d */\n", ul, ul);
#endif /* LFS_ESID_RANGE */

#ifdef LFS_SEG_ESID_FIRST
    ul = LFS_SEG_ESID_FIRST;
    printf("unsigned long LFS_SEG_ESID_FIRST = %#lx; /* %d */\n", ul, ul);
#endif /* LFS_SEG_ESID_FIRST */

#ifdef LFS_SEG_ESID_LAST
    ul = LFS_SEG_ESID_LAST;
    printf("unsigned long LFS_SEG_ESID_LAST = %#lx; /* %d */\n", ul, ul);
#endif /* LFS_SEG_ESID_LAST */

#ifdef LINESIZE
    ul = LINESIZE;
    printf("unsigned long LINESIZE = %#lx; /* %d */\n", ul, ul);
#endif /* LINESIZE */

#ifdef LOCK_INSTR_ESID_FIRST
    ul = LOCK_INSTR_ESID_FIRST;
    printf("unsigned long LOCK_INSTR_ESID_FIRST = %#lx; /* %d */\n", ul, ul);
#endif /* LOCK_INSTR_ESID_FIRST */

#ifdef LOCK_INSTR_ESID_LAST
    ul = LOCK_INSTR_ESID_LAST;
    printf("unsigned long LOCK_INSTR_ESID_LAST = %#lx; /* %d */\n", ul, ul);
#endif /* LOCK_INSTR_ESID_LAST */

#ifdef LONG_TERM
    ul = LONG_TERM;
    printf("unsigned long LONG_TERM = %#lx; /* %d */\n", ul, ul);
#endif /* LONG_TERM */

#ifdef LOW_USER_ESID_FIRST
    ul = LOW_USER_ESID_FIRST;
    printf("unsigned long LOW_USER_ESID_FIRST = %#lx; /* %d */\n", ul, ul);
#endif /* LOW_USER_ESID_FIRST */

#ifdef LOW_USER_ESID_LAST
    ul = LOW_USER_ESID_LAST;
    printf("unsigned long LOW_USER_ESID_LAST = %#lx; /* %d */\n", ul, ul);
#endif /* LOW_USER_ESID_LAST */

#ifdef LRU_ANY
    ul = LRU_ANY;
    printf("unsigned long LRU_ANY = %#lx; /* %d */\n", ul, ul);
#endif /* LRU_ANY */

#ifdef LRU_FILE
    ul = LRU_FILE;
    printf("unsigned long LRU_FILE = %#lx; /* %d */\n", ul, ul);
#endif /* LRU_FILE */

#ifdef LRU_UNMOD
    ul = LRU_UNMOD;
    printf("unsigned long LRU_UNMOD = %#lx; /* %d */\n", ul, ul);
#endif /* LRU_UNMOD */

#if 0
#ifdef LTPIN_DEC
    ul = LTPIN_DEC;
    printf("unsigned long LTPIN_DEC = %#lx; /* %d */\n", ul, ul);
#endif /* LTPIN_DEC */

#ifdef LTPIN_INC
    ul = LTPIN_INC;
    printf("unsigned long LTPIN_INC = %#lx; /* %d */\n", ul, ul);
#endif /* LTPIN_INC */

#ifdef LTPIN_MASK
    ul = LTPIN_MASK;
    printf("unsigned long LTPIN_MASK = %#lx; /* %d */\n", ul, ul);
#endif /* LTPIN_MASK */
#endif

#ifdef LTPIN_NBITS
    ul = LTPIN_NBITS;
    printf("unsigned long LTPIN_NBITS = %#lx; /* %d */\n", ul, ul);
#endif /* LTPIN_NBITS */

#ifdef LTPIN_ONE
    ul = LTPIN_ONE;
    printf("unsigned long LTPIN_ONE = %#lx; /* %d */\n", ul, ul);
#endif /* LTPIN_ONE */

#if 0
#ifdef LTPIN_OVERFLOW
    ul = LTPIN_OVERFLOW;
    printf("unsigned long LTPIN_OVERFLOW = %#lx; /* %d */\n", ul, ul);
#endif /* LTPIN_OVERFLOW */
#endif

#ifdef MAXAPT
    ul = MAXAPT;
    printf("unsigned long MAXAPT = %#lx; /* %d */\n", ul, ul);
#endif /* MAXAPT */

#ifdef MAXLOGS
    ul = MAXLOGS;
    printf("unsigned long MAXLOGS = %#lx; /* %d */\n", ul, ul);
#endif /* MAXLOGS */

#ifdef MAXPGAHEAD
    ul = MAXPGAHEAD;
    printf("unsigned long MAXPGAHEAD = %#lx; /* %d */\n", ul, ul);
#endif /* MAXPGAHEAD */

#ifdef MAXPGSP
    ul = MAXPGSP;
    printf("unsigned long MAXPGSP = %#lx; /* %d */\n", ul, ul);
#endif /* MAXPGSP */

#if 0
#ifdef MAXPIN
    ul = MAXPIN;
    printf("unsigned long MAXPIN = %#lx; /* %d */\n", ul, ul);
#endif /* MAXPIN */
#endif

#ifdef MAXREPAGE
    ul = MAXREPAGE;
    printf("unsigned long MAXREPAGE = %#lx; /* %d */\n", ul, ul);
#endif /* MAXREPAGE */

#ifdef MAXVPN
    ul = MAXVPN;
    printf("unsigned long MAXVPN = %#lx; /* %d */\n", ul, ul);
#endif /* MAXVPN */

#ifdef MBUF_POOL_ESID_FIRST
    ul = MBUF_POOL_ESID_FIRST;
    printf("unsigned long MBUF_POOL_ESID_FIRST = %#lx; /* %d */\n", ul, ul);
#endif /* MBUF_POOL_ESID_FIRST */

#ifdef MBUF_POOL_ESID_LAST
    ul = MBUF_POOL_ESID_LAST;
    printf("unsigned long MBUF_POOL_ESID_LAST = %#lx; /* %d */\n", ul, ul);
#endif /* MBUF_POOL_ESID_LAST */

#ifdef MINPGAHEAD
    ul = MINPGAHEAD;
    printf("unsigned long MINPGAHEAD = %#lx; /* %d */\n", ul, ul);
#endif /* MINPGAHEAD */

#ifdef MPDATA_ESID_FIRST
    ul = MPDATA_ESID_FIRST;
    printf("unsigned long MPDATA_ESID_FIRST = %#lx; /* %d */\n", ul, ul);
#endif /* MPDATA_ESID_FIRST */

#ifdef MPDATA_ESID_LAST
    ul = MPDATA_ESID_LAST;
    printf("unsigned long MPDATA_ESID_LAST = %#lx; /* %d */\n", ul, ul);
#endif /* MPDATA_ESID_LAST */

#ifdef NFRAGHASH
    ul = NFRAGHASH;
    printf("unsigned long NFRAGHASH = %#lx; /* %d */\n", ul, ul);
#endif /* NFRAGHASH */

#ifdef NLOCKBITS
    ul = NLOCKBITS;
    printf("unsigned long NLOCKBITS = %#lx; /* %d */\n", ul, ul);
#endif /* NLOCKBITS */

#ifdef NOFBLRU
    ul = NOFBLRU;
    printf("unsigned long NOFBLRU = %#lx; /* %d */\n", ul, ul);
#endif /* NOFBLRU */

#ifdef NOFROZEN
    ul = NOFROZEN;
    printf("unsigned long NOFROZEN = %#lx; /* %d */\n", ul, ul);
#endif /* NOFROZEN */

#ifdef NORMAL
    ul = NORMAL;
    printf("unsigned long NORMAL = %#lx; /* %d */\n", ul, ul);
#endif /* NORMAL */

#ifdef NORMAL_PIN_PTE
    ul = NORMAL_PIN_PTE;
    printf("unsigned long NORMAL_PIN_PTE = %#lx; /* %d */\n", ul, ul);
#endif /* NORMAL_PIN_PTE */

#ifdef NOSIO
    ul = NOSIO;
    printf("unsigned long NOSIO = %#lx; /* %d */\n", ul, ul);
#endif /* NOSIO */

#ifdef NO_INSPFT
    ul = NO_INSPFT;
    printf("unsigned long NO_INSPFT = %#lx; /* %d */\n", ul, ul);
#endif /* NO_INSPFT */

#ifdef NRSVDBLKS
    ul = NRSVDBLKS;
    printf("unsigned long NRSVDBLKS = %#lx; /* %d */\n", ul, ul);
#endif /* NRSVDBLKS */

#ifdef NUMESIDS
    ul = NUMESIDS;
    printf("unsigned long NUMESIDS = %#lx; /* %d */\n", ul, ul);
#endif /* NUMESIDS */

#ifdef NUMPAGES
    ul = NUMPAGES;
    printf("unsigned long NUMPAGES = %#lx; /* %d */\n", ul, ul);
#endif /* NUMPAGES */

#ifdef NUMSAVE
    ul = NUMSAVE;
    printf("unsigned long NUMSAVE = %#lx; /* %d */\n", ul, ul);
#endif /* NUMSAVE */

#ifdef NUMSIDS
    ul = NUMSIDS;
    printf("unsigned long NUMSIDS = %#lx; /* %d */\n", ul, ul);
#endif /* NUMSIDS */

#ifdef NUMZQS
    ul = NUMZQS;
    printf("unsigned long NUMZQS = %#lx; /* %d */\n", ul, ul);
#endif /* NUMZQS */

#ifdef NUM_GLOB_ESIDS
    ul = NUM_GLOB_ESIDS;
    printf("unsigned long NUM_GLOB_ESIDS = %#lx; /* %d */\n", ul, ul);
#endif /* NUM_GLOB_ESIDS */

#ifdef NUM_RMALLOCSIDS
    ul = NUM_RMALLOCSIDS;
    printf("unsigned long NUM_RMALLOCSIDS = %#lx; /* %d */\n", ul, ul);
#endif /* NUM_RMALLOCSIDS */

#ifdef NUM_SCBSIDS
    ul = NUM_SCBSIDS;
    printf("unsigned long NUM_SCBSIDS = %#lx; /* %d */\n", ul, ul);
#endif /* NUM_SCBSIDS */

#ifdef NUM_SI_VMM_ESIDS
    ul = NUM_SI_VMM_ESIDS;
    printf("unsigned long NUM_SI_VMM_ESIDS = %#lx; /* %d */\n", ul, ul);
#endif /* NUM_SI_VMM_ESIDS */

#ifdef ONES64
    ul = ONES64;
    printf("unsigned long ONES64 = %#lx; /* %d */\n", ul, ul);
#endif /* ONES64 */

#ifdef ONES
    ul = ONES;
    printf("unsigned long ONES = %#lx; /* %d */\n", ul, ul);
#endif /* ONES */

#ifdef ORGPFAULT
    ul = ORGPFAULT;
    printf("unsigned long ORGPFAULT = %#lx; /* %d */\n", ul, ul);
#endif /* ORGPFAULT */

#ifdef PFMINPIN
    ul = PFMINPIN;
    printf("unsigned long PFMINPIN = %#lx; /* %d */\n", ul, ul);
#endif /* PFMINPIN */

#ifdef PIN_SETMOD
    ul = PIN_SETMOD;
    printf("unsigned long PIN_SETMOD = %#lx; /* %d */\n", ul, ul);
#endif /* PIN_SETMOD */

#ifdef POFFSET
    ul = POFFSET;
    printf("unsigned long POFFSET = %#lx; /* %d */\n", ul, ul);
#endif /* POFFSET */

#ifdef PORTIOSIDX
    ul = PORTIOSIDX;
    printf("unsigned long PORTIOSIDX = %#lx; /* %d */\n", ul, ul);
#endif /* PORTIOSIDX */

#ifdef PORTIOSIDX_L
    ul = PORTIOSIDX_L;
    printf("unsigned long PORTIOSIDX_L = %#lx; /* %d */\n", ul, ul);
#endif /* PORTIOSIDX_L */

#ifdef PORTIO_ESID_FIRST
    ul = PORTIO_ESID_FIRST;
    printf("unsigned long PORTIO_ESID_FIRST = %#lx; /* %d */\n", ul, ul);
#endif /* PORTIO_ESID_FIRST */

#ifdef PORTIO_ESID_LAST
    ul = PORTIO_ESID_LAST;
    printf("unsigned long PORTIO_ESID_LAST = %#lx; /* %d */\n", ul, ul);
#endif /* PORTIO_ESID_LAST */

#ifdef PORTIO_NUM_ESIDS
    ul = PORTIO_NUM_ESIDS;
    printf("unsigned long PORTIO_NUM_ESIDS = %#lx; /* %d */\n", ul, ul);
#endif /* PORTIO_NUM_ESIDS */

#ifdef PROCSR
    ul = PROCSR;
    printf("unsigned long PROCSR = %#lx; /* %d */\n", ul, ul);
#endif /* PROCSR */

#ifdef PROC_PRIV_ESID
    ul = PROC_PRIV_ESID;
    printf("unsigned long PROC_PRIV_ESID = %#lx; /* %d */\n", ul, ul);
#endif /* PROC_PRIV_ESID */

#ifdef PROC_THRD_TABLE_ESID_FIRST
    ul = PROC_THRD_TABLE_ESID_FIRST;
    printf("unsigned long PROC_THRD_TABLE_ESID_FIRST = %#lx; /* %d */\n", ul, ul);
#endif /* PROC_THRD_TABLE_ESID_FIRST */

#ifdef PROC_THRD_TABLE_ESID_LAST
    ul = PROC_THRD_TABLE_ESID_LAST;
    printf("unsigned long PROC_THRD_TABLE_ESID_LAST = %#lx; /* %d */\n", ul, ul);
#endif /* PROC_THRD_TABLE_ESID_LAST */

#ifdef PSIZE
    ul = PSIZE;
    printf("unsigned long PSIZE = %#lx; /* %d */\n", ul, ul);
#endif /* PSIZE */

#if defined(PTAORG) && !defined(__64BIT__)
    ul = PTAORG;
    printf("unsigned long PTAORG = %#lx; /* %d */\n", ul, ul);
#endif /* PTAORG */

#if defined(PTASEG_dog)
    ul = PTASEG;
    printf("unsigned long PTASEG = %#lx; /* %d */\n", ul, ul);
#endif /* PTASEG */

#if defined(PTASEG_dog)
    ul = PTASEG_REF;
    printf("unsigned long PTASEG_REF = %#lx; /* %d */\n", ul, ul);
#endif /* PTASEG_REF */

#ifdef PTASIDX
    ul = PTASIDX;
    printf("unsigned long PTASIDX = %#lx; /* %d */\n", ul, ul);
#endif /* PTASIDX */

#ifdef PTASR
    ul = PTASR;
    printf("unsigned long PTASR = %#lx; /* %d */\n", ul, ul);
#endif /* PTASR */

#ifdef PVNULL
    ul = PVNULL;
    printf("unsigned long PVNULL = %#lx; /* %d */\n", ul, ul);
#endif /* PVNULL */

#ifdef QIO
    ul = QIO;
    printf("unsigned long QIO = %#lx; /* %d */\n", ul, ul);
#endif /* QIO */

#ifdef READ_WAIT
    ul = READ_WAIT;
    printf("unsigned long READ_WAIT = %#lx; /* %d */\n", ul, ul);
#endif /* READ_WAIT */

#ifdef RELOAD
    ul = RELOAD;
    printf("unsigned long RELOAD = %#lx; /* %d */\n", ul, ul);
#endif /* RELOAD */

#ifdef RMALLOC_HEAP_ESID_FIRST
    ul = RMALLOC_HEAP_ESID_FIRST;
    printf("unsigned long RMALLOC_HEAP_ESID_FIRST = %#lx; /* %d */\n", ul, ul);
#endif /* RMALLOC_HEAP_ESID_FIRST */

#ifdef RMALLOC_HEAP_ESID_LAST
    ul = RMALLOC_HEAP_ESID_LAST;
    printf("unsigned long RMALLOC_HEAP_ESID_LAST = %#lx; /* %d */\n", ul, ul);
#endif /* RMALLOC_HEAP_ESID_LAST */

#ifdef RMALLOC_NUM_ESIDS
    ul = RMALLOC_NUM_ESIDS;
    printf("unsigned long RMALLOC_NUM_ESIDS = %#lx; /* %d */\n", ul, ul);
#endif /* RMALLOC_NUM_ESIDS */

#ifdef RMAPSIDX
    ul = RMAPSIDX;
    printf("unsigned long RMAPSIDX = %#lx; /* %d */\n", ul, ul);
#endif /* RMAPSIDX */

#ifdef RMAPSIDX_L
    ul = RMAPSIDX_L;
    printf("unsigned long RMAPSIDX_L = %#lx; /* %d */\n", ul, ul);
#endif /* RMAPSIDX_L */

#ifdef RMCOPY_NOINSERT
    ul = RMCOPY_NOINSERT;
    printf("unsigned long RMCOPY_NOINSERT = %#lx; /* %d */\n", ul, ul);
#endif /* RMCOPY_NOINSERT */

#ifdef RPCOMP
    ul = RPCOMP;
    printf("unsigned long RPCOMP = %#lx; /* %d */\n", ul, ul);
#endif /* RPCOMP */

#ifdef RPFILE
    ul = RPFILE;
    printf("unsigned long RPFILE = %#lx; /* %d */\n", ul, ul);
#endif /* RPFILE */

#ifdef RPTYPES
    ul = RPTYPES;
    printf("unsigned long RPTYPES = %#lx; /* %d */\n", ul, ul);
#endif /* RPTYPES */

#ifdef RSVDSID
    ul = RSVDSID;
    printf("unsigned long RSVDSID = %#lx; /* %d */\n", ul, ul);
#endif /* RSVDSID */

#ifdef RTFWSIDX
    ul = RTFWSIDX;
    printf("unsigned long RTFWSIDX = %#lx; /* %d */\n", ul, ul);
#endif /* RTFWSIDX */

#ifdef RTFWSIDX_L
    ul = RTFWSIDX_L;
    printf("unsigned long RTFWSIDX_L = %#lx; /* %d */\n", ul, ul);
#endif /* RTFWSIDX_L */

#ifdef RTFW_ESID_FIRST
    ul = RTFW_ESID_FIRST;
    printf("unsigned long RTFW_ESID_FIRST = %#lx; /* %d */\n", ul, ul);
#endif /* RTFW_ESID_FIRST */

#ifdef RTFW_ESID_LAST
    ul = RTFW_ESID_LAST;
    printf("unsigned long RTFW_ESID_LAST = %#lx; /* %d */\n", ul, ul);
#endif /* RTFW_ESID_LAST */

#ifdef RTFW_NUM_ESIDS
    ul = RTFW_NUM_ESIDS;
    printf("unsigned long RTFW_NUM_ESIDS = %#lx; /* %d */\n", ul, ul);
#endif /* RTFW_NUM_ESIDS */

#ifdef SHORT_TERM
    ul = SHORT_TERM;
    printf("unsigned long SHORT_TERM = %#lx; /* %d */\n", ul, ul);
#endif /* SHORT_TERM */

#ifdef SHX_ALLOCATE
    ul = SHX_ALLOCATE;
    printf("unsigned long SHX_ALLOCATE = %#lx; /* %d */\n", ul, ul);
#endif /* SHX_ALLOCATE */

#ifdef SHX_FULLOVER
    ul = SHX_FULLOVER;
    printf("unsigned long SHX_FULLOVER = %#lx; /* %d */\n", ul, ul);
#endif /* SHX_FULLOVER */

#ifdef SHX_NOALLOCATE
    ul = SHX_NOALLOCATE;
    printf("unsigned long SHX_NOALLOCATE = %#lx; /* %d */\n", ul, ul);
#endif /* SHX_NOALLOCATE */

#ifdef SIDLIMIT
    ul = SIDLIMIT;
    printf("unsigned long SIDLIMIT = %#lx; /* %d */\n", ul, ul);
#endif /* SIDLIMIT */

#ifdef SIO
    ul = SIO;
    printf("unsigned long SIO = %#lx; /* %d */\n", ul, ul);
#endif /* SIO */

#ifdef SOFFSET
    ul = SOFFSET;
    printf("unsigned long SOFFSET = %#lx; /* %d */\n", ul, ul);
#endif /* SOFFSET */

#ifdef SREGMSK
    ul = SREGMSK;
    printf("unsigned long SREGMSK = %#lx; /* %d */\n", ul, ul);
#endif /* SREGMSK */

#ifdef STARTIO
    ul = STARTIO;
    printf("unsigned long STARTIO = %#lx; /* %d */\n", ul, ul);
#endif /* STARTIO */

#ifdef STKBRK_REG
    ul = STKBRK_REG;
    printf("unsigned long STKBRK_REG = %#lx; /* %d */\n", ul, ul);
#endif /* STKBRK_REG */

#ifdef STOIBITS_32GB
    ul = STOIBITS_32GB;
    printf("unsigned long STOIBITS_32GB = %#lx; /* %d */\n", ul, ul);
#endif /* STOIBITS_32GB */

#ifdef STOIBITS_64GB
    ul = STOIBITS_64GB;
    printf("unsigned long STOIBITS_64GB = %#lx; /* %d */\n", ul, ul);
#endif /* STOIBITS_64GB */

#ifdef STOIBITS_MAX
    ul = STOIBITS_MAX;
    printf("unsigned long STOIBITS_MAX = %#lx; /* %d */\n", ul, ul);
#endif /* STOIBITS_MAX */

#if 0
#ifdef STPIN_DEC
    ul = STPIN_DEC;
    printf("unsigned long STPIN_DEC = %#lx; /* %d */\n", ul, ul);
#endif /* STPIN_DEC */

#ifdef STPIN_INC
    ul = STPIN_INC;
    printf("unsigned long STPIN_INC = %#lx; /* %d */\n", ul, ul);
#endif /* STPIN_INC */
#endif

#ifdef STPIN_MASK
    ul = STPIN_MASK;
    printf("unsigned long STPIN_MASK = %#lx; /* %d */\n", ul, ul);
#endif /* STPIN_MASK */

#ifdef STPIN_NBITS
    ul = STPIN_NBITS;
    printf("unsigned long STPIN_NBITS = %#lx; /* %d */\n", ul, ul);
#endif /* STPIN_NBITS */

#ifdef STPIN_ONE
    ul = STPIN_ONE;
    printf("unsigned long STPIN_ONE = %#lx; /* %d */\n", ul, ul);
#endif /* STPIN_ONE */

#if 0
#ifdef STPIN_OVERFLOW
    ul = STPIN_OVERFLOW;
    printf("unsigned long STPIN_OVERFLOW = %#lx; /* %d */\n", ul, ul);
#endif /* STPIN_OVERFLOW */
#endif

#ifdef SZBIT
    ul = SZBIT;
    printf("unsigned long SZBIT = %#lx; /* %d */\n", ul, ul);
#endif /* SZBIT */

#ifdef TEMPSR
    ul = TEMPSR;
    printf("unsigned long TEMPSR = %#lx; /* %d */\n", ul, ul);
#endif /* TEMPSR */

#ifdef TEXTSR
    ul = TEXTSR;
    printf("unsigned long TEXTSR = %#lx; /* %d */\n", ul, ul);
#endif /* TEXTSR */

#if defined(UNKNOWN_HANDLE) && !defined(__64BIT__)
    ul = UNKNOWN_HANDLE;
    printf("unsigned long UNKNOWN_HANDLE = %#lx; /* %d */\n", ul, ul);
#endif /* UNKNOWN_HANDLE */

#ifdef UT_THRPGIOTAIL_FLUSHP
    ul = UT_THRPGIOTAIL_FLUSHP;
    printf("unsigned long UT_THRPGIOTAIL_FLUSHP = %#lx; /* %d */\n", ul, ul);
#endif /* UT_THRPGIOTAIL_FLUSHP */

#ifdef UT_THRPGIOTAIL_INIT
    ul = UT_THRPGIOTAIL_INIT;
    printf("unsigned long UT_THRPGIOTAIL_INIT = %#lx; /* %d */\n", ul, ul);
#endif /* UT_THRPGIOTAIL_INIT */

#ifdef UT_THRPGIOTAIL_PFGET
    ul = UT_THRPGIOTAIL_PFGET;
    printf("unsigned long UT_THRPGIOTAIL_PFGET = %#lx; /* %d */\n", ul, ul);
#endif /* UT_THRPGIOTAIL_PFGET */

#ifdef UT_THRPGIOTAIL_READP
    ul = UT_THRPGIOTAIL_READP;
    printf("unsigned long UT_THRPGIOTAIL_READP = %#lx; /* %d */\n", ul, ul);
#endif /* UT_THRPGIOTAIL_READP */

#ifdef UT_THRPGIOTAIL_WRITEP
    ul = UT_THRPGIOTAIL_WRITEP;
    printf("unsigned long UT_THRPGIOTAIL_WRITEP = %#lx; /* %d */\n", ul, ul);
#endif /* UT_THRPGIOTAIL_WRITEP */

#ifdef UZBIT
    ul = UZBIT;
    printf("unsigned long UZBIT = %#lx; /* %d */\n", ul, ul);
#endif /* UZBIT */

#ifdef VMM1SWSIDX
    ul = VMM1SWSIDX;
    printf("unsigned long VMM1SWSIDX = %#lx; /* %d */\n", ul, ul);
#endif /* VMM1SWSIDX */

#ifdef VMM2SWSIDX
    ul = VMM2SWSIDX;
    printf("unsigned long VMM2SWSIDX = %#lx; /* %d */\n", ul, ul);
#endif /* VMM2SWSIDX */

#ifdef VMM2SWSR
    ul = VMM2SWSR;
    printf("unsigned long VMM2SWSR = %#lx; /* %d */\n", ul, ul);
#endif /* VMM2SWSR */

#ifdef VMM3SWSIDX
    ul = VMM3SWSIDX;
    printf("unsigned long VMM3SWSIDX = %#lx; /* %d */\n", ul, ul);
#endif /* VMM3SWSIDX */

#ifdef VMM3SWSR
    ul = VMM3SWSR;
    printf("unsigned long VMM3SWSR = %#lx; /* %d */\n", ul, ul);
#endif /* VMM3SWSR */

#ifdef VMM4SWSIDX
    ul = VMM4SWSIDX;
    printf("unsigned long VMM4SWSIDX = %#lx; /* %d */\n", ul, ul);
#endif /* VMM4SWSIDX */

#ifdef VMM4SWSR
    ul = VMM4SWSR;
    printf("unsigned long VMM4SWSR = %#lx; /* %d */\n", ul, ul);
#endif /* VMM4SWSR */

#ifdef VMM5SWSIDX
    ul = VMM5SWSIDX;
    printf("unsigned long VMM5SWSIDX = %#lx; /* %d */\n", ul, ul);
#endif /* VMM5SWSIDX */

#ifdef VMM5SWSR
    ul = VMM5SWSR;
    printf("unsigned long VMM5SWSR = %#lx; /* %d */\n", ul, ul);
#endif /* VMM5SWSR */

#ifdef VMM6SWSIDX
    ul = VMM6SWSIDX;
    printf("unsigned long VMM6SWSIDX = %#lx; /* %d */\n", ul, ul);
#endif /* VMM6SWSIDX */

#ifdef VMM6SWSR
    ul = VMM6SWSR;
    printf("unsigned long VMM6SWSR = %#lx; /* %d */\n", ul, ul);
#endif /* VMM6SWSR */

#ifdef VMMSCBSIDX_F
    ul = VMMSCBSIDX_F;
    printf("unsigned long VMMSCBSIDX_F = %#lx; /* %d */\n", ul, ul);
#endif /* VMMSCBSIDX_F */

#ifdef VMMSCBSIDX_L
    ul = VMMSCBSIDX_L;
    printf("unsigned long VMMSCBSIDX_L = %#lx; /* %d */\n", ul, ul);
#endif /* VMMSCBSIDX_L */

#ifdef VMMSIDX
    ul = VMMSIDX;
    printf("unsigned long VMMSIDX = %#lx; /* %d */\n", ul, ul);
#endif /* VMMSIDX */

#ifdef VMMSR
    ul = VMMSR;
    printf("unsigned long VMMSR = %#lx; /* %d */\n", ul, ul);
#endif /* VMMSR */

#ifdef VMMSWHATSIDX
    ul = VMMSWHATSIDX;
    printf("unsigned long VMMSWHATSIDX = %#lx; /* %d */\n", ul, ul);
#endif /* VMMSWHATSIDX */

#ifdef VMMSWHATSIDX_F
    ul = VMMSWHATSIDX_F;
    printf("unsigned long VMMSWHATSIDX_F = %#lx; /* %d */\n", ul, ul);
#endif /* VMMSWHATSIDX_F */

#ifdef VMMSWHATSIDX_L
    ul = VMMSWHATSIDX_L;
    printf("unsigned long VMMSWHATSIDX_L = %#lx; /* %d */\n", ul, ul);
#endif /* VMMSWHATSIDX_L */

#ifdef VMMSWHATSR
    ul = VMMSWHATSR;
    printf("unsigned long VMMSWHATSR = %#lx; /* %d */\n", ul, ul);
#endif /* VMMSWHATSR */

#ifdef VMMSWPFTSIDX_F
    ul = VMMSWPFTSIDX_F;
    printf("unsigned long VMMSWPFTSIDX_F = %#lx; /* %d */\n", ul, ul);
#endif /* VMMSWPFTSIDX_F */

#ifdef VMMSWPFTSIDX_L
    ul = VMMSWPFTSIDX_L;
    printf("unsigned long VMMSWPFTSIDX_L = %#lx; /* %d */\n", ul, ul);
#endif /* VMMSWPFTSIDX_L */

#ifdef VMMSWSR
    ul = VMMSWSR;
    printf("unsigned long VMMSWSR = %#lx; /* %d */\n", ul, ul);
#endif /* VMMSWSR */

#ifdef VMM_AME_ESID_FIRST
    ul = VMM_AME_ESID_FIRST;
    printf("unsigned long VMM_AME_ESID_FIRST = %#lx; /* %d */\n", ul, ul);
#endif /* VMM_AME_ESID_FIRST */

#ifdef VMM_AME_ESID_LAST
    ul = VMM_AME_ESID_LAST;
    printf("unsigned long VMM_AME_ESID_LAST = %#lx; /* %d */\n", ul, ul);
#endif /* VMM_AME_ESID_LAST */

#ifdef VMM_BASE_LEVEL
    ul = VMM_BASE_LEVEL;
    printf("unsigned long VMM_BASE_LEVEL = %#lx; /* %d */\n", ul, ul);
#endif /* VMM_BASE_LEVEL */

#ifdef VMM_COMPLEX_LOCK_AVAIL
    ul = VMM_COMPLEX_LOCK_AVAIL;
    printf("unsigned long VMM_COMPLEX_LOCK_AVAIL = %#lx; /* %d */\n", ul, ul);
#endif /* VMM_COMPLEX_LOCK_AVAIL */

#ifdef VMM_DATA_ESID_FIRST
    ul = VMM_DATA_ESID_FIRST;
    printf("unsigned long VMM_DATA_ESID_FIRST = %#lx; /* %d */\n", ul, ul);
#endif /* VMM_DATA_ESID_FIRST */

#ifdef VMM_DATA_ESID_LAST
    ul = VMM_DATA_ESID_LAST;
    printf("unsigned long VMM_DATA_ESID_LAST = %#lx; /* %d */\n", ul, ul);
#endif /* VMM_DATA_ESID_LAST */

#ifdef VMM_DMAP_ESID_FIRST
    ul = VMM_DMAP_ESID_FIRST;
    printf("unsigned long VMM_DMAP_ESID_FIRST = %#lx; /* %d */\n", ul, ul);
#endif /* VMM_DMAP_ESID_FIRST */

#ifdef VMM_DMAP_ESID_LAST
    ul = VMM_DMAP_ESID_LAST;
    printf("unsigned long VMM_DMAP_ESID_LAST = %#lx; /* %d */\n", ul, ul);
#endif /* VMM_DMAP_ESID_LAST */

#ifdef VMM_LOCK_HANDOFF
    ul = VMM_LOCK_HANDOFF;
    printf("unsigned long VMM_LOCK_HANDOFF = %#lx; /* %d */\n", ul, ul);
#endif /* VMM_LOCK_HANDOFF */

#ifdef VMM_OWNER_MASK
    ul = VMM_OWNER_MASK;
    printf("unsigned long VMM_OWNER_MASK = %#lx; /* %d */\n", ul, ul);
#endif /* VMM_OWNER_MASK */

#ifdef VMM_PTA_ESID_FIRST
    ul = VMM_PTA_ESID_FIRST;
    printf("unsigned long VMM_PTA_ESID_FIRST = %#lx; /* %d */\n", ul, ul);
#endif /* VMM_PTA_ESID_FIRST */

#ifdef VMM_PTA_ESID_LAST
    ul = VMM_PTA_ESID_LAST;
    printf("unsigned long VMM_PTA_ESID_LAST = %#lx; /* %d */\n", ul, ul);
#endif /* VMM_PTA_ESID_LAST */

#ifdef VMM_READ_MODE
    ul = VMM_READ_MODE;
    printf("unsigned long VMM_READ_MODE = %#lx; /* %d */\n", ul, ul);
#endif /* VMM_READ_MODE */

#ifdef VMM_SCB_ESID_FIRST
    ul = VMM_SCB_ESID_FIRST;
    printf("unsigned long VMM_SCB_ESID_FIRST = %#lx; /* %d */\n", ul, ul);
#endif /* VMM_SCB_ESID_FIRST */

#ifdef VMM_SCB_ESID_LAST
    ul = VMM_SCB_ESID_LAST;
    printf("unsigned long VMM_SCB_ESID_LAST = %#lx; /* %d */\n", ul, ul);
#endif /* VMM_SCB_ESID_LAST */

#ifdef VMM_SEG_ESID_FIRST
    ul = VMM_SEG_ESID_FIRST;
    printf("unsigned long VMM_SEG_ESID_FIRST = %#lx; /* %d */\n", ul, ul);
#endif /* VMM_SEG_ESID_FIRST */

#ifdef VMM_SEG_ESID_LAST
    ul = VMM_SEG_ESID_LAST;
    printf("unsigned long VMM_SEG_ESID_LAST = %#lx; /* %d */\n", ul, ul);
#endif /* VMM_SEG_ESID_LAST */

#ifdef VMM_SWHAT_ESID_FIRST
    ul = VMM_SWHAT_ESID_FIRST;
    printf("unsigned long VMM_SWHAT_ESID_FIRST = %#lx; /* %d */\n", ul, ul);
#endif /* VMM_SWHAT_ESID_FIRST */

#ifdef VMM_SWHAT_ESID_LAST
    ul = VMM_SWHAT_ESID_LAST;
    printf("unsigned long VMM_SWHAT_ESID_LAST = %#lx; /* %d */\n", ul, ul);
#endif /* VMM_SWHAT_ESID_LAST */

#ifdef VMM_SWPFT_ESID_FIRST
    ul = VMM_SWPFT_ESID_FIRST;
    printf("unsigned long VMM_SWPFT_ESID_FIRST = %#lx; /* %d */\n", ul, ul);
#endif /* VMM_SWPFT_ESID_FIRST */

#ifdef VMM_SWPFT_ESID_LAST
    ul = VMM_SWPFT_ESID_LAST;
    printf("unsigned long VMM_SWPFT_ESID_LAST = %#lx; /* %d */\n", ul, ul);
#endif /* VMM_SWPFT_ESID_LAST */

#ifdef VMM_WAITING
    ul = VMM_WAITING;
    printf("unsigned long VMM_WAITING = %#lx; /* %d */\n", ul, ul);
#endif /* VMM_WAITING */

#ifdef VMM_WANT_WRITE
    ul = VMM_WANT_WRITE;
    printf("unsigned long VMM_WANT_WRITE = %#lx; /* %d */\n", ul, ul);
#endif /* VMM_WANT_WRITE */

#ifdef VM_BACKT
    ul = VM_BACKT;
    printf("unsigned long VM_BACKT = %#lx; /* %d */\n", ul, ul);
#endif /* VM_BACKT */

#ifdef VM_INLRU
    ul = VM_INLRU;
    printf("unsigned long VM_INLRU = %#lx; /* %d */\n", ul, ul);
#endif /* VM_INLRU */

#ifdef VM_NOBACKT
    ul = VM_NOBACKT;
    printf("unsigned long VM_NOBACKT = %#lx; /* %d */\n", ul, ul);
#endif /* VM_NOBACKT */

#ifdef VM_ZQGET
    ul = VM_ZQGET;
    printf("unsigned long VM_ZQGET = %#lx; /* %d */\n", ul, ul);
#endif /* VM_ZQGET */

#ifdef VM_ZQPUT
    ul = VM_ZQPUT;
    printf("unsigned long VM_ZQPUT = %#lx; /* %d */\n", ul, ul);
#endif /* VM_ZQPUT */

#ifdef V_THRPGIO_PROT
    ul = V_THRPGIO_PROT;
    printf("unsigned long V_THRPGIO_PROT = %#lx; /* %d */\n", ul, ul);
#endif /* V_THRPGIO_PROT */

#ifdef WRITE_WAIT
    ul = WRITE_WAIT;
    printf("unsigned long WRITE_WAIT = %#lx; /* %d */\n", ul, ul);
#endif /* WRITE_WAIT */

#ifdef _64VMM
    ul = _64VMM;
    printf("unsigned long _64VMM = %#lx; /* %d */\n", ul, ul);
#endif /* _64VMM */

#ifdef _KSP_HIGHESID_BASE
    ul = _KSP_HIGHESID_BASE;
    printf("unsigned long _KSP_HIGHESID_BASE = %#lx; /* %d */\n", ul, ul);
#endif /* _KSP_HIGHESID_BASE */

#ifdef _KSP_HIGH_SIZE
    ul = _KSP_HIGH_SIZE;
    printf("unsigned long _KSP_HIGH_SIZE = %#lx; /* %d */\n", ul, ul);
#endif /* _KSP_HIGH_SIZE */

#ifdef _KSP_LOWESID_BASE
    ul = _KSP_LOWESID_BASE;
    printf("unsigned long _KSP_LOWESID_BASE = %#lx; /* %d */\n", ul, ul);
#endif /* _KSP_LOWESID_BASE */

#ifdef _KSP_LOW_SIZE
    ul = _KSP_LOW_SIZE;
    printf("unsigned long _KSP_LOW_SIZE = %#lx; /* %d */\n", ul, ul);
#endif /* _KSP_LOW_SIZE */

#ifdef GALLOC_OFF_F
    ul = GALLOC_OFF_F;
    printf("unsigned long GALLOC_OFF_F = %#lx; /* %d */\n", ul, ul);
#endif

#ifdef GLOBKER_1TB_OFF_F
    ul = GLOBKER_1TB_OFF_F;
    printf("unsigned long GLOBKER_1TB_OFF_F = %#lx; /* %d */\n", ul, ul);
#endif

#ifdef GLOBKER_4K_OFF_F
    ul = GLOBKER_4K_OFF_F;
    printf("unsigned long GLOBKER_4K_OFF_F = %#lx; /* %d */\n", ul, ul);
#endif

    ul = KERNEL_OFF;
    printf("unsigned long KERNEL_OFF = %#lx; /* %d */\n", ul, ul);

#ifdef HI_GLOB_KERN_OFF_F
    ul = GA_UNUSED1;
    printf("unsigned long GA_UNUSED1 = %#lx; /* %d */\n", ul, ul);

    ul = HI_GLOB_KERN_OFF_F;
    printf("unsigned long HI_GLOB_KERN_OFF_F = %#lx; /* %d */\n", ul, ul);
#endif

    ul = GALLOC_4K_OFF_F;
    printf("unsigned long GALLOC_4K_OFF_F = %#lx; /* %d */\n", ul, ul);

    ul = GALLOC_4K_OFF_L;
    printf("unsigned long GALLOC_4K_OFF_L = %#lx; /* %d */\n", ul, ul);

#ifdef DIR_GALLOC_4K_OFF_F
    ul = DIR_GALLOC_4K_OFF_F;
    printf("unsigned long DIR_GALLOC_4K_OFF_F = %#lx; /* %d */\n", ul, ul);
#endif

#ifdef KHEAP_4K_OFF_F
    ul = KHEAP_4K_OFF_F;
    printf("unsigned long KHEAP_4K_OFF_F = %#lx; /* %d */\n", ul, ul);
#endif

#ifdef KHEAP_4K_OFF_L
    ul = KHEAP_4K_OFF_L;
    printf("unsigned long KHEAP_4K_OFF_L = %#lx; /* %d */\n", ul, ul);
#endif

#ifdef MBUF_4K_OFF_F
    ul = MBUF_4K_OFF_F;
    printf("unsigned long MBUF_4K_OFF_F = %#lx; /* %d */\n", ul, ul);
#endif

#ifdef MBUF_4K_OFF_L
    ul = MBUF_4K_OFF_L;
    printf("unsigned long MBUF_4K_OFF_L = %#lx; /* %d */\n", ul, ul);
#endif

#ifdef MTRC_4K_OFF_F
    ul = MTRC_4K_OFF_F;
    printf("unsigned long MTRC_4K_OFF_F = %#lx; /* %d */\n", ul, ul);
#endif

#ifdef MTRC_4K_OFF_L
    ul = MTRC_4K_OFF_L;
    printf("unsigned long MTRC_4K_OFF_L = %#lx; /* %d */\n", ul, ul);
#endif

#ifdef NUM_PTASEGS
    ul = NUM_PTASEGS;
    printf("unsigned long NUM_PTASEGS = %#lx; /* %d */\n", ul, ul);
#endif

#ifdef PTA_OFF_F
    ul = PTA_OFF_F;
    printf("unsigned long PTA_OFF_F = %#lx; /* %d */\n", ul, ul);
#endif

#ifdef PTA_OFF_L
    ul = PTA_OFF_L;
    printf("unsigned long PTA_OFF_L = %#lx; /* %d */\n", ul, ul);
#endif

#ifdef LDR_LIB_4K
    ul = LDR_LIB_4K;
    printf("unsigned long LDR_LIB_4K = %#lx; /* %d */\n", ul, ul);
#endif

#ifdef LDR_LIB_OFF_F
    ul = LDR_LIB_OFF_F;
    printf("unsigned long LDR_LIB_OFF_F = %#lx; /* %d */\n", ul, ul);
#endif

#ifdef LDR_LIB_4K_OFF_F
    ul = LDR_LIB_4K_OFF_F;
    printf("unsigned long LDR_LIB_4K_OFF_F = %#lx; /* %d */\n", ul, ul);
#endif

#ifdef LDR_SHLIB32_4K_OFF_F
    ul = LDR_SHLIB32_4K_OFF_F;
    printf("unsigned long LDR_SHLIB32_4K_OFF_F = %#lx; /* %d */\n", ul, ul);
#endif

#ifdef LDR_SHLIB32_4K_OFF_L
    ul = LDR_SHLIB32_4K_OFF_L;
    printf("unsigned long LDR_SHLIB32_4K_OFF_L = %#lx; /* %d */\n", ul, ul);
#endif

#ifdef LDR_SHLIB64_4K_OFF_F
    ul = LDR_SHLIB64_4K_OFF_F;
    printf("unsigned long LDR_SHLIB64_4K_OFF_F = %#lx; /* %d */\n", ul, ul);
#endif

#ifdef LDR_SHLIB64_4K_OFF_L
    ul = LDR_SHLIB64_4K_OFF_L;
    printf("unsigned long LDR_SHLIB64_4K_OFF_L = %#lx; /* %d */\n", ul, ul);
#endif

#ifdef LDR_LIB_4K_OFF_L
    ul = LDR_LIB_4K_OFF_L;
    printf("unsigned long LDR_LIB_4K_OFF_L = %#lx; /* %d */\n", ul, ul);
#endif

#ifdef LDR_LIB_OFF_L
    ul = LDR_LIB_OFF_L;
    printf("unsigned long LDR_LIB_OFF_L = %#lx; /* %d */\n", ul, ul);
#endif

#ifdef NUM_RMALLOCSIDS
    ul = NUM_RMALLOCSIDS;
    printf("unsigned long NUM_RMALLOCSIDS = %#lx; /* %d */\n", ul, ul);
#endif

#ifdef RMALLOC_OFF_F
    ul = RMALLOC_OFF_F;
    printf("unsigned long RMALLOC_OFF_F = %#lx; /* %d */\n", ul, ul);
#endif

#ifdef RMALLOC_OFF_L
    ul = RMALLOC_OFF_L;
    printf("unsigned long RMALLOC_OFF_L = %#lx; /* %d */\n", ul, ul);
#endif

#ifdef NUM_DRMGRSEGS
    ul = NUM_DRMGRSEGS;
    printf("unsigned long NUM_DRMGRSEGS = %#lx; /* %d */\n", ul, ul);
#endif

#ifdef DRMGR_OFF_F
    ul = DRMGR_OFF_F;
    printf("unsigned long DRMGR_OFF_F = %#lx; /* %d */\n", ul, ul);
#endif

#ifdef DRMGR_OFF_L
    ul = DRMGR_OFF_L;
    printf("unsigned long DRMGR_OFF_L = %#lx; /* %d */\n", ul, ul);
#endif

#ifdef NUM_ERMGRSEGS
    ul = NUM_ERMGRSEGS;
    printf("unsigned long NUM_ERMGRSEGS = %#lx; /* %d */\n", ul, ul);
#endif

#ifdef ERMGR_OFF_F
    ul = ERMGR_OFF_F;
    printf("unsigned long ERMGR_OFF_F = %#lx; /* %d */\n", ul, ul);
#endif

#ifdef ERMGR_OFF_L
    ul = ERMGR_OFF_L;
    printf("unsigned long ERMGR_OFF_L = %#lx; /* %d */\n", ul, ul);
#endif

#ifdef NUM_MSTSPACESEGS
    ul = NUM_MSTSPACESEGS;
    printf("unsigned long NUM_MSTSPACESEGS = %#lx; /* %d */\n", ul, ul);
#endif

#ifdef MSTSPACE_OFF_F
    ul = MSTSPACE_OFF_F;
    printf("unsigned long MSTSPACE_OFF_F = %#lx; /* %d */\n", ul, ul);
#endif

#ifdef MSTSPACE_OFF_L
    ul = MSTSPACE_OFF_L;
    printf("unsigned long MSTSPACE_OFF_L = %#lx; /* %d */\n", ul, ul);
#endif

#ifdef VMM_SPACEOK_4K_OFF
    ul = VMM_SPACEOK_4K_OFF;
    printf("unsigned long VMM_SPACEOK_4K_OFF = %#lx; /* %d */\n", ul, ul);
#endif

#ifdef VMM_4K_1TB_END
    ul = VMM_4K_1TB_END;
    printf("unsigned long VMM_4K_1TB_END = %#lx; /* %d */\n", ul, ul);
#endif

#ifdef DIR_GALLOC_4K_OFF_L
    ul = DIR_GALLOC_4K_OFF_L;
    printf("unsigned long DIR_GALLOC_4K_OFF_L = %#lx; /* %d */\n", ul, ul);
#endif

#ifdef GLOBKER_4K_OFF_L
    ul = GLOBKER_4K_OFF_L;
    printf("unsigned long GLOBKER_4K_OFF_L = %#lx; /* %d */\n", ul, ul);
#endif

#ifdef VMM_64K_PAD
    ul = VMM_64K_PAD;
    printf("unsigned long VMM_64K_PAD = %#lx; /* %d */\n", ul, ul);
#endif

#ifdef VMM_64K_1TB_BOUNDARY
    ul = VMM_64K_1TB_BOUNDARY;
    printf("unsigned long VMM_64K_1TB_BOUNDARY = %#lx; /* %d */\n", ul, ul);
#endif

#ifdef GLOBKER_64K_OFF_F
    ul = GLOBKER_64K_OFF_F;
    printf("unsigned long GLOBKER_64K_OFF_F = %#lx; /* %d */\n", ul, ul);
#endif

#ifdef GALLOC_64K_OFF_F
    ul = GALLOC_64K_OFF_F;
    printf("unsigned long GALLOC_64K_OFF_F = %#lx; /* %d */\n", ul, ul);
#endif

#ifdef GALLOC_64K_OFF_L
    ul = GALLOC_64K_OFF_L;
    printf("unsigned long GALLOC_64K_OFF_L = %#lx; /* %d */\n", ul, ul);
#endif

#ifdef DIR_GALLOC_64K_OFF_F
    ul = DIR_GALLOC_64K_OFF_F;
    printf("unsigned long DIR_GALLOC_64K_OFF_F = %#lx; /* %d */\n", ul, ul);
#endif

#ifdef KHEAP_64K_OFF_F
    ul = KHEAP_64K_OFF_F;
    printf("unsigned long KHEAP_64K_OFF_F = %#lx; /* %d */\n", ul, ul);
#endif

#ifdef KHEAP_64K_OFF_L
    ul = KHEAP_64K_OFF_L;
    printf("unsigned long KHEAP_64K_OFF_L = %#lx; /* %d */\n", ul, ul);
#endif

#ifdef MBUF_64K_OFF_F
    ul = MBUF_64K_OFF_F;
    printf("unsigned long MBUF_64K_OFF_F = %#lx; /* %d */\n", ul, ul);
#endif

#ifdef MBUF_64K_OFF_L
    ul = MBUF_64K_OFF_L;
    printf("unsigned long MBUF_64K_OFF_L = %#lx; /* %d */\n", ul, ul);
#endif

#ifdef VMM_OFF
    ul = VMM_OFF;
    printf("unsigned long VMM_OFF = %#lx; /* %d */\n", ul, ul);
#endif

#ifdef VMM_OFF_F
    ul = VMM_OFF_F;
    printf("unsigned long VMM_OFF_F = %#lx; /* %d */\n", ul, ul);
#endif

#ifdef VMM_OFF_L
    ul = VMM_OFF_L;
    printf("unsigned long VMM_OFF_L = %#lx; /* %d */\n", ul, ul);
#endif

#ifdef NUM_DMAPSIDS
    ul = NUM_DMAPSIDS;
    printf("unsigned long NUM_DMAPSIDS = %#lx; /* %d */\n", ul, ul);
#endif

#ifdef DMAP_OFF_F
    ul = DMAP_OFF_F;
    printf("unsigned long DMAP_OFF_F = %#lx; /* %d */\n", ul, ul);
#endif

#ifdef DMAP_OFF_L
    ul = DMAP_OFF_L;
    printf("unsigned long DMAP_OFF_L = %#lx; /* %d */\n", ul, ul);
#endif

#ifdef NUM_AMESEGS
    ul = NUM_AMESEGS;
    printf("unsigned long NUM_AMESEGS = %#lx; /* %d */\n", ul, ul);
#endif

#ifdef AME_OFF_F
    ul = AME_OFF_F;
    printf("unsigned long AME_OFF_F = %#lx; /* %d */\n", ul, ul);
#endif

#ifdef AME_OFF_L
    ul = AME_OFF_L;
    printf("unsigned long AME_OFF_L = %#lx; /* %d */\n", ul, ul);
#endif

#ifdef NUM_VMINTSIDS
    ul = NUM_VMINTSIDS;
    printf("unsigned long NUM_VMINTSIDS = %#lx; /* %d */\n", ul, ul);
#endif

#ifdef VMINT_OFF_F
    ul = VMINT_OFF_F;
    printf("unsigned long VMINT_OFF_F = %#lx; /* %d */\n", ul, ul);
#endif

#ifdef VMINT_OFF_L
    ul = VMINT_OFF_L;
    printf("unsigned long VMINT_OFF_L = %#lx; /* %d */\n", ul, ul);
#endif

#ifdef NUM_FFBMSEGS
    ul = NUM_FFBMSEGS;
    printf("unsigned long NUM_FFBMSEGS = %#lx; /* %d */\n", ul, ul);
#endif

#ifdef FF_BITMAP_OFF_F
    ul = FF_BITMAP_OFF_F;
    printf("unsigned long FF_BITMAP_OFF_F = %#lx; /* %d */\n", ul, ul);
#endif

#ifdef FF_BITMAP_OFF_L
    ul = FF_BITMAP_OFF_L;
    printf("unsigned long FF_BITMAP_OFF_L = %#lx; /* %d */\n", ul, ul);
#endif

#ifdef LDR_LIB_64K
    ul = LDR_LIB_64K;
    printf("unsigned long LDR_LIB_64K = %#lx; /* %d */\n", ul, ul);
#endif

#ifdef LDR_LIB_64K_OFF_F
    ul = LDR_LIB_64K_OFF_F;
    printf("unsigned long LDR_LIB_64K_OFF_F = %#lx; /* %d */\n", ul, ul);
#endif

#ifdef LDR_SHLIB32_64K_OFF_F
    ul = LDR_SHLIB32_64K_OFF_F;
    printf("unsigned long LDR_SHLIB32_64K_OFF_F = %#lx; /* %d */\n", ul, ul);
#endif

#ifdef LDR_SHLIB32_64K_OFF_L
    ul = LDR_SHLIB32_64K_OFF_L;
    printf("unsigned long LDR_SHLIB32_64K_OFF_L = %#lx; /* %d */\n", ul, ul);
#endif

#ifdef LDR_SHLIB64_64K_OFF_F
    ul = LDR_SHLIB64_64K_OFF_F;
    printf("unsigned long LDR_SHLIB64_64K_OFF_F = %#lx; /* %d */\n", ul, ul);
#endif

#ifdef LDR_SHLIB64_64K_OFF_L
    ul = LDR_SHLIB64_64K_OFF_L;
    printf("unsigned long LDR_SHLIB64_64K_OFF_L = %#lx; /* %d */\n", ul, ul);
#endif

#ifdef LDR_LIB_64K_OFF_L
    ul = LDR_LIB_64K_OFF_L;
    printf("unsigned long LDR_LIB_64K_OFF_L = %#lx; /* %d */\n", ul, ul);
#endif

#ifdef NUM_PROCTTSIDS
    ul = NUM_PROCTTSIDS;
    printf("unsigned long NUM_PROCTTSIDS = %#lx; /* %d */\n", ul, ul);
#endif

#ifdef PROCTT_OFF_F
    ul = PROCTT_OFF_F;
    printf("unsigned long PROCTT_OFF_F = %#lx; /* %d */\n", ul, ul);
#endif

#ifdef PROCTT_OFF_L
    ul = PROCTT_OFF_L;
    printf("unsigned long PROCTT_OFF_L = %#lx; /* %d */\n", ul, ul);
#endif

#ifdef XMDBG_RECS_OFF_F
    ul = XMDBG_RECS_OFF_F;
    printf("unsigned long XMDBG_RECS_OFF_F = %#lx; /* %d */\n", ul, ul);
#endif

#ifdef XMDBG_RECS_OFF_L
    ul = XMDBG_RECS_OFF_L;
    printf("unsigned long XMDBG_RECS_OFF_L = %#lx; /* %d */\n", ul, ul);
#endif

#ifdef MPDATA_OFF_F
    ul = MPDATA_OFF_F;
    printf("unsigned long MPDATA_OFF_F = %#lx; /* %d */\n", ul, ul);
#endif

#ifdef MPDATA_OFF_L
    ul = MPDATA_OFF_L;
    printf("unsigned long MPDATA_OFF_L = %#lx; /* %d */\n", ul, ul);
#endif

#ifdef LOCK_INSTR_OFF_F
    ul = LOCK_INSTR_OFF_F;
    printf("unsigned long LOCK_INSTR_OFF_F = %#lx; /* %d */\n", ul, ul);
#endif

#ifdef LOCK_INSTR_OFF_L
    ul = LOCK_INSTR_OFF_L;
    printf("unsigned long LOCK_INSTR_OFF_L = %#lx; /* %d */\n", ul, ul);
#endif

#ifdef JFS_SEG_OFF_F
    ul = JFS_SEG_OFF_F;
    printf("unsigned long JFS_SEG_OFF_F = %#lx; /* %d */\n", ul, ul);
#endif

#ifdef JFS_SEG_OFF_L
    ul = JFS_SEG_OFF_L;
    printf("unsigned long JFS_SEG_OFF_L = %#lx; /* %d */\n", ul, ul);
#endif

#ifdef LFS_ESID_RANGE
    ul = LFS_ESID_RANGE;
    printf("unsigned long LFS_ESID_RANGE = %#lx; /* %d */\n", ul, ul);
#endif

#ifdef LFS_SEG_OFF_F
    ul = LFS_SEG_OFF_F;
    printf("unsigned long LFS_SEG_OFF_F = %#lx; /* %d */\n", ul, ul);
#endif

#ifdef LFS_SEG_OFF_L
    ul = LFS_SEG_OFF_L;
    printf("unsigned long LFS_SEG_OFF_L = %#lx; /* %d */\n", ul, ul);
#endif

#ifdef VMM_SCBSZ
    ul = VMM_SCBSZ;
    printf("unsigned long VMM_SCBSZ = %#lx; /* %d */\n", ul, ul);
#endif

#ifdef NUM_SCBSIDS
    ul = NUM_SCBSIDS;
    printf("unsigned long NUM_SCBSIDS = %#lx; /* %d */\n", ul, ul);
#endif

#ifdef NUM_1TB_SCBSIDS
    ul = NUM_1TB_SCBSIDS;
    printf("unsigned long NUM_1TB_SCBSIDS = %#lx; /* %d */\n", ul, ul);
#endif

#ifdef VMMSCB_OFF_F
    ul = VMMSCB_OFF_F;
    printf("unsigned long VMMSCB_OFF_F = %#lx; /* %d */\n", ul, ul);
#endif

#ifdef VMMSCB_1TB_OFF_F
    ul = VMMSCB_1TB_OFF_F;
    printf("unsigned long VMMSCB_1TB_OFF_F = %#lx; /* %d */\n", ul, ul);
#endif

#ifdef VMMSCB_1TB_OFF_L
    ul = VMMSCB_1TB_OFF_L;
    printf("unsigned long VMMSCB_1TB_OFF_L = %#lx; /* %d */\n", ul, ul);
#endif

#ifdef VMMSCB_REG_OFF
    ul = VMMSCB_REG_OFF;
    printf("unsigned long VMMSCB_REG_OFF = %#lx; /* %d */\n", ul, ul);
#endif

#ifdef VMMSCB_OFF_L
    ul = VMMSCB_OFF_L;
    printf("unsigned long VMMSCB_OFF_L = %#lx; /* %d */\n", ul, ul);
#endif

#ifdef VMM_PVL_OFF
    ul = VMM_PVL_OFF;
    printf("unsigned long VMM_PVL_OFF = %#lx; /* %d */\n", ul, ul);
#endif

#ifdef VMM_SPACEOK_64K_OFF
    ul = VMM_SPACEOK_64K_OFF;
    printf("unsigned long VMM_SPACEOK_64K_OFF = %#lx; /* %d */\n", ul, ul);
#endif

#ifdef VMM_DLVA_OFF
    ul = VMM_DLVA_OFF;
    printf("unsigned long VMM_DLVA_OFF = %#lx; /* %d */\n", ul, ul);
#endif

#ifdef VMM_CMEM_SZ
    ul = VMM_CMEM_SZ;
    printf("unsigned long VMM_CMEM_SZ = %#lx; /* %d */\n", ul, ul);
#endif

#ifdef NUM_CMEM_SEGS
    ul = NUM_CMEM_SEGS;
    printf("unsigned long NUM_CMEM_SEGS = %#lx; /* %d */\n", ul, ul);
#endif

#ifdef VMM_CMEM_OFF_F
    ul = VMM_CMEM_OFF_F;
    printf("unsigned long VMM_CMEM_OFF_F = %#lx; /* %d */\n", ul, ul);
#endif

#ifdef VMM_CMEM_OFF_L
    ul = VMM_CMEM_OFF_L;
    printf("unsigned long VMM_CMEM_OFF_L = %#lx; /* %d */\n", ul, ul);
#endif

#ifdef VMM_CBLKS_SZ
    ul = VMM_CBLKS_SZ;
    printf("unsigned long VMM_CBLKS_SZ = %#lx; /* %d */\n", ul, ul);
#endif

#ifdef NUM_CBLKS_SEGS
    ul = NUM_CBLKS_SEGS;
    printf("unsigned long NUM_CBLKS_SEGS = %#lx; /* %d */\n", ul, ul);
#endif

#ifdef VMM_CBLKS_OFF_F
    ul = VMM_CBLKS_OFF_F;
    printf("unsigned long VMM_CBLKS_OFF_F = %#lx; /* %d */\n", ul, ul);
#endif

#ifdef VMM_CBLKS_OFF_L
    ul = VMM_CBLKS_OFF_L;
    printf("unsigned long VMM_CBLKS_OFF_L = %#lx; /* %d */\n", ul, ul);
#endif

#ifdef VMM_64K_1TB_END
    ul = VMM_64K_1TB_END;
    printf("unsigned long VMM_64K_1TB_END = %#lx; /* %d */\n", ul, ul);
#endif

#ifdef DIR_GALLOC_64K_OFF_L
    ul = DIR_GALLOC_64K_OFF_L;
    printf("unsigned long DIR_GALLOC_64K_OFF_L = %#lx; /* %d */\n", ul, ul);
#endif

#ifdef GLOBKER_64K_OFF_L
    ul = GLOBKER_64K_OFF_L;
    printf("unsigned long GLOBKER_64K_OFF_L = %#lx; /* %d */\n", ul, ul);
#endif

#ifdef VMM_16M_PAD
    ul = VMM_16M_PAD;
    printf("unsigned long VMM_16M_PAD = %#lx; /* %d */\n", ul, ul);
#endif

#ifdef VMM_16M_1TB_BOUNDARY
    ul = VMM_16M_1TB_BOUNDARY;
    printf("unsigned long VMM_16M_1TB_BOUNDARY = %#lx; /* %d */\n", ul, ul);
#endif

#ifdef GLOBKER_16M_OFF_F
    ul = GLOBKER_16M_OFF_F;
    printf("unsigned long GLOBKER_16M_OFF_F = %#lx; /* %d */\n", ul, ul);
#endif

#ifdef GALLOC_16M_OFF_F
    ul = GALLOC_16M_OFF_F;
    printf("unsigned long GALLOC_16M_OFF_F = %#lx; /* %d */\n", ul, ul);
#endif

#ifdef GALLOC_16M_OFF_L
    ul = GALLOC_16M_OFF_L;
    printf("unsigned long GALLOC_16M_OFF_L = %#lx; /* %d */\n", ul, ul);
#endif

#ifdef DIR_GALLOC_16M_OFF_F
    ul = DIR_GALLOC_16M_OFF_F;
    printf("unsigned long DIR_GALLOC_16M_OFF_F = %#lx; /* %d */\n", ul, ul);
#endif

#ifdef KHEAP_16M_OFF_F
    ul = KHEAP_16M_OFF_F;
    printf("unsigned long KHEAP_16M_OFF_F = %#lx; /* %d */\n", ul, ul);
#endif

#ifdef KHEAP_16M_OFF_L
    ul = KHEAP_16M_OFF_L;
    printf("unsigned long KHEAP_16M_OFF_L = %#lx; /* %d */\n", ul, ul);
#endif

#ifdef MBUF_16M_OFF_F
    ul = MBUF_16M_OFF_F;
    printf("unsigned long MBUF_16M_OFF_F = %#lx; /* %d */\n", ul, ul);
#endif

#ifdef MBUF_16M_OFF_L
    ul = MBUF_16M_OFF_L;
    printf("unsigned long MBUF_16M_OFF_L = %#lx; /* %d */\n", ul, ul);
#endif

#ifdef DIR_GALLOC_16M_OFF_L
    ul = DIR_GALLOC_16M_OFF_L;
    printf("unsigned long DIR_GALLOC_16M_OFF_L = %#lx; /* %d */\n", ul, ul);
#endif

#ifdef GLOBKER_16M_OFF_L
    ul = GLOBKER_16M_OFF_L;
    printf("unsigned long GLOBKER_16M_OFF_L = %#lx; /* %d */\n", ul, ul);
#endif

#ifdef GLOBKER_1TB_OFF_L
    ul = GLOBKER_1TB_OFF_L;
    printf("unsigned long GLOBKER_1TB_OFF_L = %#lx; /* %d */\n", ul, ul);
#endif

#ifdef GALLOC_OFF_L
    ul = GALLOC_OFF_L;
    printf("unsigned long GALLOC_OFF_L = %#lx; /* %d */\n", ul, ul);
#endif

#ifdef NONGK_1TB_OFF_F
    ul = NONGK_1TB_OFF_F;
    printf("unsigned long NONGK_1TB_OFF_F = %#lx; /* %d */\n", ul, ul);
#endif

#ifdef NONGK_1TB_OFF_L
    ul = NONGK_1TB_OFF_L;
    printf("unsigned long NONGK_1TB_OFF_L = %#lx; /* %d */\n", ul, ul);
#endif

#ifdef SCBREG_OFF_F
    ul = SCBREG_OFF_F;
    printf("unsigned long SCBREG_OFF_F = %#lx; /* %d */\n", ul, ul);
#endif

#ifdef SCBREG_OFF_L
    ul = SCBREG_OFF_L;
    printf("unsigned long SCBREG_OFF_L = %#lx; /* %d */\n", ul, ul);
#endif

#ifdef VSID_1TB_SHIFT
    ul = VSID_1TB_SHIFT;
    printf("unsigned long VSID_1TB_SHIFT = %#lx; /* %d */\n", ul, ul);
#endif
    
    ul = RMAP_UNKNOWN;
    printf("unsigned long RMAP_UNKNOWN = %#lx; /* %d */\n", ul, ul);
    
    ul = RMAP_SWPFT;
    printf("unsigned long RMAP_SWPFT = %#lx; /* %d */\n", ul, ul);
    
    ul = RMAP_PVT;
    printf("unsigned long RMAP_PVT = %#lx; /* %d */\n", ul, ul);
    
    ul = RMAP_PVLIST;
    printf("unsigned long RMAP_PVLIST = %#lx; /* %d */\n", ul, ul);
    
    ul = RMAP_PFT;
    printf("unsigned long RMAP_PFT = %#lx; /* %d */\n", ul, ul);
    
    ul = RMAP_SWHAT;
    printf("unsigned long RMAP_SWHAT = %#lx; /* %d */\n", ul, ul);
    
    ul = RMAP_AHAT;
    printf("unsigned long RMAP_AHAT = %#lx; /* %d */\n", ul, ul);
    
    ul = RMAP_FIXED_LAST;
    printf("unsigned long RMAP_FIXED_LAST = %#lx; /* %d */\n", ul, ul);
    
    ul = RMAP_KERN;
    printf("unsigned long RMAP_KERN = %#lx; /* %d */\n", ul, ul);
    
    ul = RMAP_IPLCB;
    printf("unsigned long RMAP_IPLCB = %#lx; /* %d */\n", ul, ul);
    
    ul = RMAP_MSTSPACE;
    printf("unsigned long RMAP_MSTSPACE = %#lx; /* %d */\n", ul, ul);
    
    ul = RMAP_RAMD;
    printf("unsigned long RMAP_RAMD = %#lx; /* %d */\n", ul, ul);
    
    ul = RMAP_BCFG;
    printf("unsigned long RMAP_BCFG = %#lx; /* %d */\n", ul, ul);
    
    ul = RMAP_RPT;
    printf("unsigned long RMAP_RPT = %#lx; /* %d */\n", ul, ul);
    
    ul = RMAP_RPHAT;
    printf("unsigned long RMAP_RPHAT = %#lx; /* %d */\n", ul, ul);
    
    ul = RMAP_PDT;
    printf("unsigned long RMAP_PDT = %#lx; /* %d */\n", ul, ul);
    
    ul = RMAP_PTAR;
    printf("unsigned long RMAP_PTAR = %#lx; /* %d */\n", ul, ul);
    
    ul = RMAP_PTAD;
    printf("unsigned long RMAP_PTAD = %#lx; /* %d */\n", ul, ul);
    
    ul = RMAP_PTAI;
    printf("unsigned long RMAP_PTAI = %#lx; /* %d */\n", ul, ul);
    
    ul = RMAP_DMAP;
    printf("unsigned long RMAP_DMAP = %#lx; /* %d */\n", ul, ul);
    
    ul = RMAP_BACKT;
    printf("unsigned long RMAP_BACKT = %#lx; /* %d */\n", ul, ul);
    
    ul = RMAP_MEMP_FRS;
    printf("unsigned long RMAP_MEMP_FRS = %#lx; /* %d */\n", ul, ul);
    
    ul = RMAP_PAL_STOP_SELF;
    printf("unsigned long RMAP_PAL_STOP_SELF = %#lx; /* %d */\n", ul, ul);
    
    ul = RMAP_KDB_STACK;
    printf("unsigned long RMAP_KDB_STACK = %#lx; /* %d */\n", ul, ul);
    
    ul = RMAP_FIXLMB;
    printf("unsigned long RMAP_FIXLMB = %#lx; /* %d */\n", ul, ul);
    
    ul = RMAP_SCB;
    printf("unsigned long RMAP_SCB = %#lx; /* %d */\n", ul, ul);
    
    ul = RMAP_RMALLOC;
    printf("unsigned long RMAP_RMALLOC = %#lx; /* %d */\n", ul, ul);
    
    ul = RMAP_DRMGR;
    printf("unsigned long RMAP_DRMGR = %#lx; /* %d */\n", ul, ul);
    
    ul = RMAP_VMINT;
    printf("unsigned long RMAP_VMINT = %#lx; /* %d */\n", ul, ul);
    
    ul = RMAP_KERN_HI;
    printf("unsigned long RMAP_KERN_HI = %#lx; /* %d */\n", ul, ul);
    
    ul = RMAP_SLB_TRACE;
    printf("unsigned long RMAP_SLB_TRACE = %#lx; /* %d */\n", ul, ul);
    
    ul = RMAP_LGPGKSEG;
    printf("unsigned long RMAP_LGPGKSEG = %#lx; /* %d */\n", ul, ul);
    
    ul = RMAP_FF_BITMAP;
    printf("unsigned long RMAP_FF_BITMAP = %#lx; /* %d */\n", ul, ul);
    
    ul = RMAP_FWAD_RMRSA;
    printf("unsigned long RMAP_FWAD_RMRSA = %#lx; /* %d */\n", ul, ul);
    
    ul = RMAP_FWAD_DLVA;
    printf("unsigned long RMAP_FWAD_DLVA = %#lx; /* %d */\n", ul, ul);
    
    ul = RMAP_FWAD_HRSA;
    printf("unsigned long RMAP_FWAD_HRSA = %#lx; /* %d */\n", ul, ul);
    
    ul = RMAP_KPATCH;
    printf("unsigned long RMAP_KPATCH = %#lx; /* %d */\n", ul, ul);
    
    ul = RMAP_PPDA;
    printf("unsigned long RMAP_PPDA = %#lx; /* %d */\n", ul, ul);
    
    ul = RMAP_VAR_LAST;
    printf("unsigned long RMAP_VAR_LAST = %#lx; /* %d */\n", ul, ul);
    
#ifdef L2MAXVADDR
    ul = L2MAXVADDR;
    printf("unsigned long L2MAXVADDR = %#lx; /* %d */\n", ul, ul);
#endif
    
#ifdef L2MAXVADDR_IMPL
    ul = L2MAXVADDR_IMPL;
    printf("unsigned long L2MAXVADDR_IMPL = %#lx; /* %d */\n", ul, ul);
#endif
    
#ifdef VM_L2_MAXHW_VSID
    ul = VM_L2_MAXHW_VSID;
    printf("unsigned long VM_L2_MAXHW_VSID = %#lx; /* %d */\n", ul, ul);
#endif
    
#ifdef VM_L2_MAXARCH_VSID_1TB
    ul = VM_L2_MAXARCH_VSID_1TB;
    printf("unsigned long VM_L2_MAXARCH_VSID_1TB = %#lx; /* %d */\n", ul, ul);
#endif
    
#ifdef VM_L2_MAXARCH_VSID
    ul = VM_L2_MAXARCH_VSID;
    printf("unsigned long VM_L2_MAXARCH_VSID = %#lx; /* %d */\n", ul, ul);
#endif
    
#ifdef NUMSIDS
    ul = NUMSIDS;
    printf("unsigned long NUMSIDS = %#lx; /* %d */\n", ul, ul);
#endif
    
#ifdef SIDLIMIT
    ul = SIDLIMIT;
    printf("unsigned long SIDLIMIT = %#lx; /* %d */\n", ul, ul);
#endif
    
#ifdef L2SIDLIMIT_1TB
    ul = L2SIDLIMIT_1TB;
    printf("unsigned long L2SIDLIMIT_1TB = %#lx; /* %d */\n", ul, ul);
#endif
    
#ifdef SIDMAX_1TB
    ul = SIDMAX_1TB;
    printf("unsigned long SIDMAX_1TB = %#lx; /* %d */\n", ul, ul);
#endif
    
#ifdef NUMSIDS_1TB
    ul = NUMSIDS_1TB;
    printf("unsigned long NUMSIDS_1TB = %#lx; /* %d */\n", ul, ul);
#endif
    
#ifdef SIDLIMIT_1TB
    ul = SIDLIMIT_1TB;
    printf("unsigned long SIDLIMIT_1TB = %#lx; /* %d */\n", ul, ul);
#endif
    
#ifdef IOSIDMASK
    ul = IOSIDMASK;
    printf("unsigned long IOSIDMASK = %#lx; /* %d */\n", ul, ul);
#endif
    
#ifdef VM_MAXARCH_VSID
    ul = VM_MAXARCH_VSID;
    printf("unsigned long VM_MAXARCH_VSID = %#lx; /* %d */\n", ul, ul);
#endif
    
#ifdef VM_L2_IOSID_BIT
    ul = VM_L2_IOSID_BIT;
    printf("unsigned long VM_L2_IOSID_BIT = %#lx; /* %d */\n", ul, ul);
#endif
    
#ifdef IOSIDBIT
    ul = IOSIDBIT;
    printf("unsigned long IOSIDBIT = %#lx; /* %d */\n", ul, ul);
#endif
    
#ifdef RSVDSID
    ul = RSVDSID;
    printf("unsigned long RSVDSID = %#lx; /* %d */\n", ul, ul);
#endif
    
#ifdef NUMZQS
    ul = NUMZQS;
    printf("unsigned long NUMZQS = %#lx; /* %d */\n", ul, ul);
#endif
    
#ifdef NUMSAVE
    ul = NUMSAVE;
    printf("unsigned long NUMSAVE = %#lx; /* %d */\n", ul, ul);
#endif
    
#ifdef NFRAGHASH
    ul = NFRAGHASH;
    printf("unsigned long NFRAGHASH = %#lx; /* %d */\n", ul, ul);
#endif
    
    return 0;
}
