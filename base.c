
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
#ifndef _KERNSYS
#define _KERNSYS
#endif
#include <sys/lockname.h>
#ifdef PS_ID_LOCK
#include <sys/pollset.h>
#endif
#include <sys/selpoll.h>

#ifndef PEDZ_EXTRA
#include <vmm/vmdefs.h>
#include <lfs/lfs_numa.h>
#include <ras/ras_priv.h>
ppftfl_t v1;
pnft_t v2;
ppltfl_t v3;
pnlt_t v4;
#endif /* MINE */
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
	Simple_lock	 nlc_lock;   	/* lock per hash anchor    */
} nlchash_t;

typedef struct nlcbuf {
	struct nlcbuf	*nlc_forw;  	/* head buffer in hash		    */
	struct nlcbuf	*nlc_back;  	/* tail buffer in hash		    */
	struct vfs	*nlc_vfsp;  	/* vfs pointer of node	    	    */

	void		*nlc_dp;   	/* ptr to parent node of entry      */
	uint32		nlc_did;    	/* parent filenode cap id ptr       */

	void		*nlc_np;    	/* node ptr of cached entry	    */
	uint32		*nlc_nidp;    	/* node capability id ptr           */
	uint32		nlc_nid;    	/* node cap. id -- value at time
					   of entry */

	uint32		nlc_namelen;	/* name length			    */
	union {				
		char 	_namea[32];
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

int main(void)
{
    unsigned long ul;

#ifdef AMESIDX_F
    ul = AMESIDX_F;
    printf("AMESIDX_F = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* AMESIDX_F */

#ifdef AMESIDX_L
    ul = AMESIDX_L;
    printf("AMESIDX_L = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* AMESIDX_L */

#ifdef AMESR
    ul = AMESR;
    printf("AMESR = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* AMESR */

#ifdef APTNULL
    ul = APTNULL;
    printf("APTNULL = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* APTNULL */

#ifdef APTPIN
    ul = APTPIN;
    printf("APTPIN = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* APTPIN */

#ifdef APTREG
    ul = APTREG;
    printf("APTREG = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* APTREG */

#ifdef BASE_EXTSCB
    ul = BASE_EXTSCB;
    printf("BASE_EXTSCB = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* BASE_EXTSCB */

#ifdef BKTPFAULT
    ul = BKTPFAULT;
    printf("BKTPFAULT = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* BKTPFAULT */

#if defined(CUR_PTA_no)
    ul = CUR_PTA;
    printf("CUR_PTA = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* CUR_PTA */

#ifdef DMAPSIDX
    ul = DMAPSIDX;
    printf("DMAPSIDX = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* DMAPSIDX */

#ifdef EHANDLE
    ul = EHANDLE;
    printf("EHANDLE = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* EHANDLE */

#ifdef EISR
    ul = EISR;
    printf("EISR = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* EISR */

#ifdef EORGVADDR
    ul = EORGVADDR;
    printf("EORGVADDR = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* EORGVADDR */

#ifdef EPFTYPE
    ul = EPFTYPE;
    printf("EPFTYPE = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* EPFTYPE */

#ifdef ESRVAL
    ul = ESRVAL;
    printf("ESRVAL = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* ESRVAL */

#ifdef EVADDR
    ul = EVADDR;
    printf("EVADDR = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* EVADDR */

#ifdef EXTSCB_MASK
    ul = EXTSCB_MASK;
    printf("EXTSCB_MASK = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* EXTSCB_MASK */

#ifdef FAIL_IF_HIDDEN
    ul = FAIL_IF_HIDDEN;
    printf("FAIL_IF_HIDDEN = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* FAIL_IF_HIDDEN */

#ifdef FBANCH
    ul = FBANCH;
    printf("FBANCH = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* FBANCH */

#ifdef FBLOWLIM
    ul = FBLOWLIM;
    printf("FBLOWLIM = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* FBLOWLIM */

#ifdef FBLRU
    ul = FBLRU;
    printf("FBLRU = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* FBLRU */

#ifdef FBLRU_REREF
    ul = FBLRU_REREF;
    printf("FBLRU_REREF = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* FBLRU_REREF */

#ifdef FBMAX
    ul = FBMAX;
    printf("FBMAX = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* FBMAX */

#ifdef FBUPLIM
    ul = FBUPLIM;
    printf("FBUPLIM = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* FBUPLIM */

#ifdef FIND_HIDDEN
    ul = FIND_HIDDEN;
    printf("FIND_HIDDEN = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* FIND_HIDDEN */

#ifdef FROZEN
    ul = FROZEN;
    printf("FROZEN = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* FROZEN */

#ifdef GBITSPERDW
    ul = GBITSPERDW;
    printf("GBITSPERDW = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* GBITSPERDW */

#ifdef GDWSPERMAP
    ul = GDWSPERMAP;
    printf("GDWSPERMAP = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* GDWSPERMAP */

#ifdef GL2BITSPERDW
    ul = GL2BITSPERDW;
    printf("GL2BITSPERDW = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* GL2BITSPERDW */

#ifdef GLOB_ESID_FIRST
    ul = GLOB_ESID_FIRST;
    printf("GLOB_ESID_FIRST = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* GLOB_ESID_FIRST */

#ifdef GLOB_ESID_LAST
    ul = GLOB_ESID_LAST;
    printf("GLOB_ESID_LAST = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* GLOB_ESID_LAST */

#ifdef GLOB_EXTREG_ESID_FIRST
    ul = GLOB_EXTREG_ESID_FIRST;
    printf("GLOB_EXTREG_ESID_FIRST = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* GLOB_EXTREG_ESID_FIRST */

#ifdef GLOB_EXTREG_ESID_LAST
    ul = GLOB_EXTREG_ESID_LAST;
    printf("GLOB_EXTREG_ESID_LAST = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* GLOB_EXTREG_ESID_LAST */

#ifdef GMAPSIZE
    ul = GMAPSIZE;
    printf("GMAPSIZE = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* GMAPSIZE */

#ifdef GROWPGAHEAD
    ul = GROWPGAHEAD;
    printf("GROWPGAHEAD = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* GROWPGAHEAD */

#ifdef HALT_NOMEM
    ul = HALT_NOMEM;
    printf("HALT_NOMEM = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* HALT_NOMEM */

#ifdef HIGH_USER_ESID_FIRST
    ul = HIGH_USER_ESID_FIRST;
    printf("HIGH_USER_ESID_FIRST = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* HIGH_USER_ESID_FIRST */

#ifdef HIGH_USER_ESID_LAST
    ul = HIGH_USER_ESID_LAST;
    printf("HIGH_USER_ESID_LAST = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* HIGH_USER_ESID_LAST */

#ifdef INSPFT
    ul = INSPFT;
    printf("INSPFT = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* INSPFT */

#if defined(INVALID_HANDLE) && !defined(__64BIT__)
    ul = INVALID_HANDLE;
    printf("INVALID_HANDLE = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* INVALID_HANDLE */

#ifdef IODONE
    ul = IODONE;
    printf("IODONE = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* IODONE */

#ifdef IOFORCESYNC
    ul = IOFORCESYNC;
    printf("IOFORCESYNC = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* IOFORCESYNC */

#ifdef IOSEGMENT
    ul = IOSEGMENT;
    printf("IOSEGMENT = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* IOSEGMENT */

#ifdef IOSIDBIT
    ul = IOSIDBIT;
    printf("IOSIDBIT = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* IOSIDBIT */

#ifdef IOSIDMASK
    ul = IOSIDMASK;
    printf("IOSIDMASK = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* IOSIDMASK */

#ifdef IOSYNC
    ul = IOSYNC;
    printf("IOSYNC = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* IOSYNC */

#ifdef JFS_LKW_ESID_FIRST
    ul = JFS_LKW_ESID_FIRST;
    printf("JFS_LKW_ESID_FIRST = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* JFS_LKW_ESID_FIRST */

#ifdef JFS_LKW_ESID_LAST
    ul = JFS_LKW_ESID_LAST;
    printf("JFS_LKW_ESID_LAST = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* JFS_LKW_ESID_LAST */

#ifdef JFS_SEG_ESID_FIRST
    ul = JFS_SEG_ESID_FIRST;
    printf("JFS_SEG_ESID_FIRST = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* JFS_SEG_ESID_FIRST */

#ifdef JFS_SEG_ESID_LAST
    ul = JFS_SEG_ESID_LAST;
    printf("JFS_SEG_ESID_LAST = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* JFS_SEG_ESID_LAST */

#ifdef KERNEL_ESID
    ul = KERNEL_ESID;
    printf("KERNEL_ESID = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* KERNEL_ESID */

#ifdef KERNEL_HEAP_ESID_FIRST
    ul = KERNEL_HEAP_ESID_FIRST;
    printf("KERNEL_HEAP_ESID_FIRST = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* KERNEL_HEAP_ESID_FIRST */

#ifdef KERNEL_HEAP_ESID_LAST
    ul = KERNEL_HEAP_ESID_LAST;
    printf("KERNEL_HEAP_ESID_LAST = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* KERNEL_HEAP_ESID_LAST */

#ifdef KERNSR
    ul = KERNSR;
    printf("KERNSR = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* KERNSR */

#ifdef KERNXSR
    ul = KERNXSR;
    printf("KERNXSR = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* KERNXSR */

#ifdef KERN_THRD_ESID_FIRST
    ul = KERN_THRD_ESID_FIRST;
    printf("KERN_THRD_ESID_FIRST = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* KERN_THRD_ESID_FIRST */

#ifdef KERN_THRD_ESID_LAST
    ul = KERN_THRD_ESID_LAST;
    printf("KERN_THRD_ESID_LAST = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* KERN_THRD_ESID_LAST */

#ifdef KERSIDX
    ul = KERSIDX;
    printf("KERSIDX = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* KERSIDX */

#ifdef KEXTSIDX
    ul = KEXTSIDX;
    printf("KEXTSIDX = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* KEXTSIDX */

#ifdef KGLOB_ESID_BASE
    ul = KGLOB_ESID_BASE;
    printf("KGLOB_ESID_BASE = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* KGLOB_ESID_BASE */

#ifdef KPRIV_ESID_BASE
    ul = KPRIV_ESID_BASE;
    printf("KPRIV_ESID_BASE = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* KPRIV_ESID_BASE */

#ifdef KSP_ARCH_ESID_BASE
    ul = KSP_ARCH_ESID_BASE;
    printf("KSP_ARCH_ESID_BASE = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* KSP_ARCH_ESID_BASE */

#ifdef KSP_ARCH_ESID_END
    ul = KSP_ARCH_ESID_END;
    printf("KSP_ARCH_ESID_END = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* KSP_ARCH_ESID_END */

#ifdef KSP_ARCH_NUMSIDS
    ul = KSP_ARCH_NUMSIDS;
    printf("KSP_ARCH_NUMSIDS = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* KSP_ARCH_NUMSIDS */

#ifdef KSP_ESID_BASE
    ul = KSP_ESID_BASE;
    printf("KSP_ESID_BASE = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* KSP_ESID_BASE */

#ifdef KSP_ESID_END
    ul = KSP_ESID_END;
    printf("KSP_ESID_END = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* KSP_ESID_END */

#ifdef KSP_NUMSIDS
    ul = KSP_NUMSIDS;
    printf("KSP_NUMSIDS = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* KSP_NUMSIDS */

#ifdef KSP_SID_BASE
    ul = KSP_SID_BASE;
    printf("KSP_SID_BASE = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* KSP_SID_BASE */

#ifdef KSP_S_ARCH_ESID_BASE
    ul = KSP_S_ARCH_ESID_BASE;
    printf("KSP_S_ARCH_ESID_BASE = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* KSP_S_ARCH_ESID_BASE */

#ifdef KSP_S_ARCH_ESID_END
    ul = KSP_S_ARCH_ESID_END;
    printf("KSP_S_ARCH_ESID_END = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* KSP_S_ARCH_ESID_END */

#ifdef KSP_S_ARCH_NUMSIDS
    ul = KSP_S_ARCH_NUMSIDS;
    printf("KSP_S_ARCH_NUMSIDS = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* KSP_S_ARCH_NUMSIDS */

#ifdef KSP_S_ESID_BASE
    ul = KSP_S_ESID_BASE;
    printf("KSP_S_ESID_BASE = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* KSP_S_ESID_BASE */

#ifdef KSP_S_ESID_END
    ul = KSP_S_ESID_END;
    printf("KSP_S_ESID_END = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* KSP_S_ESID_END */

#ifdef KSP_S_NUMSIDS
    ul = KSP_S_NUMSIDS;
    printf("KSP_S_NUMSIDS = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* KSP_S_NUMSIDS */

#ifdef KSP_TOTAL_SIDS
    ul = KSP_TOTAL_SIDS;
    printf("KSP_TOTAL_SIDS = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* KSP_TOTAL_SIDS */

#ifdef KTEMP_ESID_BASE
    ul = KTEMP_ESID_BASE;
    printf("KTEMP_ESID_BASE = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* KTEMP_ESID_BASE */

#ifdef L2LINESIZE
    ul = L2LINESIZE;
    printf("L2LINESIZE = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* L2LINESIZE */

#ifdef L2PSIZE
    ul = L2PSIZE;
    printf("L2PSIZE = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* L2PSIZE */

#ifdef L2SSIZE
    ul = L2SSIZE;
    printf("L2SSIZE = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* L2SSIZE */

#ifdef LDR_LIB_ESID_FIRST
    ul = LDR_LIB_ESID_FIRST;
    printf("LDR_LIB_ESID_FIRST = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* LDR_LIB_ESID_FIRST */

#ifdef LDR_LIB_ESID_LAST
    ul = LDR_LIB_ESID_LAST;
    printf("LDR_LIB_ESID_LAST = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* LDR_LIB_ESID_LAST */

#ifdef LDR_PRIV_ADDR
    ul = LDR_PRIV_ADDR;
    printf("LDR_PRIV_ADDR = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* LDR_PRIV_ADDR */

#ifdef LDR_PRIV_ESID
    ul = LDR_PRIV_ESID;
    printf("LDR_PRIV_ESID = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* LDR_PRIV_ESID */

#ifdef LFS_ESID_RANGE
    ul = LFS_ESID_RANGE;
    printf("LFS_ESID_RANGE = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* LFS_ESID_RANGE */

#ifdef LFS_SEG_ESID_FIRST
    ul = LFS_SEG_ESID_FIRST;
    printf("LFS_SEG_ESID_FIRST = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* LFS_SEG_ESID_FIRST */

#ifdef LFS_SEG_ESID_LAST
    ul = LFS_SEG_ESID_LAST;
    printf("LFS_SEG_ESID_LAST = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* LFS_SEG_ESID_LAST */

#ifdef LINESIZE
    ul = LINESIZE;
    printf("LINESIZE = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* LINESIZE */

#ifdef LOCK_INSTR_ESID_FIRST
    ul = LOCK_INSTR_ESID_FIRST;
    printf("LOCK_INSTR_ESID_FIRST = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* LOCK_INSTR_ESID_FIRST */

#ifdef LOCK_INSTR_ESID_LAST
    ul = LOCK_INSTR_ESID_LAST;
    printf("LOCK_INSTR_ESID_LAST = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* LOCK_INSTR_ESID_LAST */

#ifdef LONG_TERM
    ul = LONG_TERM;
    printf("LONG_TERM = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* LONG_TERM */

#ifdef LOW_USER_ESID_FIRST
    ul = LOW_USER_ESID_FIRST;
    printf("LOW_USER_ESID_FIRST = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* LOW_USER_ESID_FIRST */

#ifdef LOW_USER_ESID_LAST
    ul = LOW_USER_ESID_LAST;
    printf("LOW_USER_ESID_LAST = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* LOW_USER_ESID_LAST */

#ifdef LRU_ANY
    ul = LRU_ANY;
    printf("LRU_ANY = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* LRU_ANY */

#ifdef LRU_FILE
    ul = LRU_FILE;
    printf("LRU_FILE = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* LRU_FILE */

#ifdef LRU_UNMOD
    ul = LRU_UNMOD;
    printf("LRU_UNMOD = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* LRU_UNMOD */

#if 0
#ifdef LTPIN_DEC
    ul = LTPIN_DEC;
    printf("LTPIN_DEC = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* LTPIN_DEC */

#ifdef LTPIN_INC
    ul = LTPIN_INC;
    printf("LTPIN_INC = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* LTPIN_INC */

#ifdef LTPIN_MASK
    ul = LTPIN_MASK;
    printf("LTPIN_MASK = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* LTPIN_MASK */
#endif

#ifdef LTPIN_NBITS
    ul = LTPIN_NBITS;
    printf("LTPIN_NBITS = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* LTPIN_NBITS */

#ifdef LTPIN_ONE
    ul = LTPIN_ONE;
    printf("LTPIN_ONE = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* LTPIN_ONE */

#if 0
#ifdef LTPIN_OVERFLOW
    ul = LTPIN_OVERFLOW;
    printf("LTPIN_OVERFLOW = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* LTPIN_OVERFLOW */
#endif

#ifdef MAXAPT
    ul = MAXAPT;
    printf("MAXAPT = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* MAXAPT */

#ifdef MAXLOGS
    ul = MAXLOGS;
    printf("MAXLOGS = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* MAXLOGS */

#ifdef MAXPGAHEAD
    ul = MAXPGAHEAD;
    printf("MAXPGAHEAD = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* MAXPGAHEAD */

#ifdef MAXPGSP
    ul = MAXPGSP;
    printf("MAXPGSP = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* MAXPGSP */

#if 0
#ifdef MAXPIN
    ul = MAXPIN;
    printf("MAXPIN = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* MAXPIN */
#endif

#ifdef MAXREPAGE
    ul = MAXREPAGE;
    printf("MAXREPAGE = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* MAXREPAGE */

#ifdef MAXVPN
    ul = MAXVPN;
    printf("MAXVPN = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* MAXVPN */

#ifdef MBUF_POOL_ESID_FIRST
    ul = MBUF_POOL_ESID_FIRST;
    printf("MBUF_POOL_ESID_FIRST = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* MBUF_POOL_ESID_FIRST */

#ifdef MBUF_POOL_ESID_LAST
    ul = MBUF_POOL_ESID_LAST;
    printf("MBUF_POOL_ESID_LAST = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* MBUF_POOL_ESID_LAST */

#ifdef MINPGAHEAD
    ul = MINPGAHEAD;
    printf("MINPGAHEAD = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* MINPGAHEAD */

#ifdef MPDATA_ESID_FIRST
    ul = MPDATA_ESID_FIRST;
    printf("MPDATA_ESID_FIRST = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* MPDATA_ESID_FIRST */

#ifdef MPDATA_ESID_LAST
    ul = MPDATA_ESID_LAST;
    printf("MPDATA_ESID_LAST = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* MPDATA_ESID_LAST */

#ifdef NFRAGHASH
    ul = NFRAGHASH;
    printf("NFRAGHASH = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* NFRAGHASH */

#ifdef NLOCKBITS
    ul = NLOCKBITS;
    printf("NLOCKBITS = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* NLOCKBITS */

#ifdef NOFBLRU
    ul = NOFBLRU;
    printf("NOFBLRU = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* NOFBLRU */

#ifdef NOFROZEN
    ul = NOFROZEN;
    printf("NOFROZEN = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* NOFROZEN */

#ifdef NORMAL
    ul = NORMAL;
    printf("NORMAL = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* NORMAL */

#ifdef NORMAL_PIN_PTE
    ul = NORMAL_PIN_PTE;
    printf("NORMAL_PIN_PTE = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* NORMAL_PIN_PTE */

#ifdef NOSIO
    ul = NOSIO;
    printf("NOSIO = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* NOSIO */

#ifdef NO_INSPFT
    ul = NO_INSPFT;
    printf("NO_INSPFT = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* NO_INSPFT */

#ifdef NRSVDBLKS
    ul = NRSVDBLKS;
    printf("NRSVDBLKS = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* NRSVDBLKS */

#ifdef NUMESIDS
    ul = NUMESIDS;
    printf("NUMESIDS = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* NUMESIDS */

#ifdef NUMPAGES
    ul = NUMPAGES;
    printf("NUMPAGES = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* NUMPAGES */

#ifdef NUMSAVE
    ul = NUMSAVE;
    printf("NUMSAVE = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* NUMSAVE */

#ifdef NUMSIDS
    ul = NUMSIDS;
    printf("NUMSIDS = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* NUMSIDS */

#ifdef NUMZQS
    ul = NUMZQS;
    printf("NUMZQS = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* NUMZQS */

#ifdef NUM_GLOB_ESIDS
    ul = NUM_GLOB_ESIDS;
    printf("NUM_GLOB_ESIDS = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* NUM_GLOB_ESIDS */

#ifdef NUM_RMALLOCSIDS
    ul = NUM_RMALLOCSIDS;
    printf("NUM_RMALLOCSIDS = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* NUM_RMALLOCSIDS */

#ifdef NUM_SCBSIDS
    ul = NUM_SCBSIDS;
    printf("NUM_SCBSIDS = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* NUM_SCBSIDS */

#ifdef NUM_SI_VMM_ESIDS
    ul = NUM_SI_VMM_ESIDS;
    printf("NUM_SI_VMM_ESIDS = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* NUM_SI_VMM_ESIDS */

#ifdef ONES64
    ul = ONES64;
    printf("ONES64 = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* ONES64 */

#ifdef ONES
    ul = ONES;
    printf("ONES = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* ONES */

#ifdef ORGPFAULT
    ul = ORGPFAULT;
    printf("ORGPFAULT = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* ORGPFAULT */

#ifdef PFMINPIN
    ul = PFMINPIN;
    printf("PFMINPIN = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* PFMINPIN */

#ifdef PIN_SETMOD
    ul = PIN_SETMOD;
    printf("PIN_SETMOD = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* PIN_SETMOD */

#ifdef POFFSET
    ul = POFFSET;
    printf("POFFSET = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* POFFSET */

#ifdef PORTIOSIDX
    ul = PORTIOSIDX;
    printf("PORTIOSIDX = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* PORTIOSIDX */

#ifdef PORTIOSIDX_L
    ul = PORTIOSIDX_L;
    printf("PORTIOSIDX_L = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* PORTIOSIDX_L */

#ifdef PORTIO_ESID_FIRST
    ul = PORTIO_ESID_FIRST;
    printf("PORTIO_ESID_FIRST = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* PORTIO_ESID_FIRST */

#ifdef PORTIO_ESID_LAST
    ul = PORTIO_ESID_LAST;
    printf("PORTIO_ESID_LAST = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* PORTIO_ESID_LAST */

#ifdef PORTIO_NUM_ESIDS
    ul = PORTIO_NUM_ESIDS;
    printf("PORTIO_NUM_ESIDS = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* PORTIO_NUM_ESIDS */

#ifdef PROCSR
    ul = PROCSR;
    printf("PROCSR = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* PROCSR */

#ifdef PROC_PRIV_ESID
    ul = PROC_PRIV_ESID;
    printf("PROC_PRIV_ESID = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* PROC_PRIV_ESID */

#ifdef PROC_THRD_TABLE_ESID_FIRST
    ul = PROC_THRD_TABLE_ESID_FIRST;
    printf("PROC_THRD_TABLE_ESID_FIRST = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* PROC_THRD_TABLE_ESID_FIRST */

#ifdef PROC_THRD_TABLE_ESID_LAST
    ul = PROC_THRD_TABLE_ESID_LAST;
    printf("PROC_THRD_TABLE_ESID_LAST = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* PROC_THRD_TABLE_ESID_LAST */

#ifdef PSIZE
    ul = PSIZE;
    printf("PSIZE = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* PSIZE */

#if defined(PTAORG) && !defined(__64BIT__)
    ul = PTAORG;
    printf("PTAORG = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* PTAORG */

#if defined(PTASEG_dog)
    ul = PTASEG;
    printf("PTASEG = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* PTASEG */

#if defined(PTASEG_dog)
    ul = PTASEG_REF;
    printf("PTASEG_REF = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* PTASEG_REF */

#ifdef PTASIDX
    ul = PTASIDX;
    printf("PTASIDX = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* PTASIDX */

#ifdef PTASR
    ul = PTASR;
    printf("PTASR = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* PTASR */

#ifdef PVNULL
    ul = PVNULL;
    printf("PVNULL = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* PVNULL */

#ifdef QIO
    ul = QIO;
    printf("QIO = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* QIO */

#ifdef READ_WAIT
    ul = READ_WAIT;
    printf("READ_WAIT = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* READ_WAIT */

#ifdef RELOAD
    ul = RELOAD;
    printf("RELOAD = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* RELOAD */

#ifdef RMALLOC_HEAP_ESID_FIRST
    ul = RMALLOC_HEAP_ESID_FIRST;
    printf("RMALLOC_HEAP_ESID_FIRST = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* RMALLOC_HEAP_ESID_FIRST */

#ifdef RMALLOC_HEAP_ESID_LAST
    ul = RMALLOC_HEAP_ESID_LAST;
    printf("RMALLOC_HEAP_ESID_LAST = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* RMALLOC_HEAP_ESID_LAST */

#ifdef RMALLOC_NUM_ESIDS
    ul = RMALLOC_NUM_ESIDS;
    printf("RMALLOC_NUM_ESIDS = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* RMALLOC_NUM_ESIDS */

#ifdef RMAPSIDX
    ul = RMAPSIDX;
    printf("RMAPSIDX = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* RMAPSIDX */

#ifdef RMAPSIDX_L
    ul = RMAPSIDX_L;
    printf("RMAPSIDX_L = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* RMAPSIDX_L */

#ifdef RMCOPY_NOINSERT
    ul = RMCOPY_NOINSERT;
    printf("RMCOPY_NOINSERT = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* RMCOPY_NOINSERT */

#ifdef RPCOMP
    ul = RPCOMP;
    printf("RPCOMP = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* RPCOMP */

#ifdef RPFILE
    ul = RPFILE;
    printf("RPFILE = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* RPFILE */

#ifdef RPTYPES
    ul = RPTYPES;
    printf("RPTYPES = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* RPTYPES */

#ifdef RSVDSID
    ul = RSVDSID;
    printf("RSVDSID = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* RSVDSID */

#ifdef RTFWSIDX
    ul = RTFWSIDX;
    printf("RTFWSIDX = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* RTFWSIDX */

#ifdef RTFWSIDX_L
    ul = RTFWSIDX_L;
    printf("RTFWSIDX_L = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* RTFWSIDX_L */

#ifdef RTFW_ESID_FIRST
    ul = RTFW_ESID_FIRST;
    printf("RTFW_ESID_FIRST = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* RTFW_ESID_FIRST */

#ifdef RTFW_ESID_LAST
    ul = RTFW_ESID_LAST;
    printf("RTFW_ESID_LAST = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* RTFW_ESID_LAST */

#ifdef RTFW_NUM_ESIDS
    ul = RTFW_NUM_ESIDS;
    printf("RTFW_NUM_ESIDS = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* RTFW_NUM_ESIDS */

#ifdef SHORT_TERM
    ul = SHORT_TERM;
    printf("SHORT_TERM = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* SHORT_TERM */

#ifdef SHX_ALLOCATE
    ul = SHX_ALLOCATE;
    printf("SHX_ALLOCATE = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* SHX_ALLOCATE */

#ifdef SHX_FULLOVER
    ul = SHX_FULLOVER;
    printf("SHX_FULLOVER = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* SHX_FULLOVER */

#ifdef SHX_NOALLOCATE
    ul = SHX_NOALLOCATE;
    printf("SHX_NOALLOCATE = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* SHX_NOALLOCATE */

#ifdef SIDLIMIT
    ul = SIDLIMIT;
    printf("SIDLIMIT = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* SIDLIMIT */

#ifdef SIO
    ul = SIO;
    printf("SIO = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* SIO */

#ifdef SOFFSET
    ul = SOFFSET;
    printf("SOFFSET = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* SOFFSET */

#ifdef SREGMSK
    ul = SREGMSK;
    printf("SREGMSK = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* SREGMSK */

#ifdef STARTIO
    ul = STARTIO;
    printf("STARTIO = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* STARTIO */

#ifdef STKBRK_REG
    ul = STKBRK_REG;
    printf("STKBRK_REG = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* STKBRK_REG */

#ifdef STOIBITS_32GB
    ul = STOIBITS_32GB;
    printf("STOIBITS_32GB = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* STOIBITS_32GB */

#ifdef STOIBITS_64GB
    ul = STOIBITS_64GB;
    printf("STOIBITS_64GB = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* STOIBITS_64GB */

#ifdef STOIBITS_MAX
    ul = STOIBITS_MAX;
    printf("STOIBITS_MAX = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* STOIBITS_MAX */

#if 0
#ifdef STPIN_DEC
    ul = STPIN_DEC;
    printf("STPIN_DEC = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* STPIN_DEC */

#ifdef STPIN_INC
    ul = STPIN_INC;
    printf("STPIN_INC = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* STPIN_INC */
#endif

#ifdef STPIN_MASK
    ul = STPIN_MASK;
    printf("STPIN_MASK = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* STPIN_MASK */

#ifdef STPIN_NBITS
    ul = STPIN_NBITS;
    printf("STPIN_NBITS = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* STPIN_NBITS */

#ifdef STPIN_ONE
    ul = STPIN_ONE;
    printf("STPIN_ONE = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* STPIN_ONE */

#if 0
#ifdef STPIN_OVERFLOW
    ul = STPIN_OVERFLOW;
    printf("STPIN_OVERFLOW = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* STPIN_OVERFLOW */
#endif

#ifdef SZBIT
    ul = SZBIT;
    printf("SZBIT = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* SZBIT */

#ifdef TEMPSR
    ul = TEMPSR;
    printf("TEMPSR = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* TEMPSR */

#ifdef TEXTSR
    ul = TEXTSR;
    printf("TEXTSR = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* TEXTSR */

#if defined(UNKNOWN_HANDLE) && !defined(__64BIT__)
    ul = UNKNOWN_HANDLE;
    printf("UNKNOWN_HANDLE = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* UNKNOWN_HANDLE */

#ifdef UT_THRPGIOTAIL_FLUSHP
    ul = UT_THRPGIOTAIL_FLUSHP;
    printf("UT_THRPGIOTAIL_FLUSHP = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* UT_THRPGIOTAIL_FLUSHP */

#ifdef UT_THRPGIOTAIL_INIT
    ul = UT_THRPGIOTAIL_INIT;
    printf("UT_THRPGIOTAIL_INIT = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* UT_THRPGIOTAIL_INIT */

#ifdef UT_THRPGIOTAIL_PFGET
    ul = UT_THRPGIOTAIL_PFGET;
    printf("UT_THRPGIOTAIL_PFGET = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* UT_THRPGIOTAIL_PFGET */

#ifdef UT_THRPGIOTAIL_READP
    ul = UT_THRPGIOTAIL_READP;
    printf("UT_THRPGIOTAIL_READP = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* UT_THRPGIOTAIL_READP */

#ifdef UT_THRPGIOTAIL_WRITEP
    ul = UT_THRPGIOTAIL_WRITEP;
    printf("UT_THRPGIOTAIL_WRITEP = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* UT_THRPGIOTAIL_WRITEP */

#ifdef UZBIT
    ul = UZBIT;
    printf("UZBIT = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* UZBIT */

#ifdef VMM1SWSIDX
    ul = VMM1SWSIDX;
    printf("VMM1SWSIDX = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* VMM1SWSIDX */

#ifdef VMM2SWSIDX
    ul = VMM2SWSIDX;
    printf("VMM2SWSIDX = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* VMM2SWSIDX */

#ifdef VMM2SWSR
    ul = VMM2SWSR;
    printf("VMM2SWSR = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* VMM2SWSR */

#ifdef VMM3SWSIDX
    ul = VMM3SWSIDX;
    printf("VMM3SWSIDX = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* VMM3SWSIDX */

#ifdef VMM3SWSR
    ul = VMM3SWSR;
    printf("VMM3SWSR = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* VMM3SWSR */

#ifdef VMM4SWSIDX
    ul = VMM4SWSIDX;
    printf("VMM4SWSIDX = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* VMM4SWSIDX */

#ifdef VMM4SWSR
    ul = VMM4SWSR;
    printf("VMM4SWSR = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* VMM4SWSR */

#ifdef VMM5SWSIDX
    ul = VMM5SWSIDX;
    printf("VMM5SWSIDX = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* VMM5SWSIDX */

#ifdef VMM5SWSR
    ul = VMM5SWSR;
    printf("VMM5SWSR = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* VMM5SWSR */

#ifdef VMM6SWSIDX
    ul = VMM6SWSIDX;
    printf("VMM6SWSIDX = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* VMM6SWSIDX */

#ifdef VMM6SWSR
    ul = VMM6SWSR;
    printf("VMM6SWSR = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* VMM6SWSR */

#ifdef VMMSCBSIDX_F
    ul = VMMSCBSIDX_F;
    printf("VMMSCBSIDX_F = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* VMMSCBSIDX_F */

#ifdef VMMSCBSIDX_L
    ul = VMMSCBSIDX_L;
    printf("VMMSCBSIDX_L = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* VMMSCBSIDX_L */

#ifdef VMMSIDX
    ul = VMMSIDX;
    printf("VMMSIDX = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* VMMSIDX */

#ifdef VMMSR
    ul = VMMSR;
    printf("VMMSR = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* VMMSR */

#ifdef VMMSWHATSIDX
    ul = VMMSWHATSIDX;
    printf("VMMSWHATSIDX = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* VMMSWHATSIDX */

#ifdef VMMSWHATSIDX_F
    ul = VMMSWHATSIDX_F;
    printf("VMMSWHATSIDX_F = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* VMMSWHATSIDX_F */

#ifdef VMMSWHATSIDX_L
    ul = VMMSWHATSIDX_L;
    printf("VMMSWHATSIDX_L = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* VMMSWHATSIDX_L */

#ifdef VMMSWHATSR
    ul = VMMSWHATSR;
    printf("VMMSWHATSR = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* VMMSWHATSR */

#ifdef VMMSWPFTSIDX_F
    ul = VMMSWPFTSIDX_F;
    printf("VMMSWPFTSIDX_F = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* VMMSWPFTSIDX_F */

#ifdef VMMSWPFTSIDX_L
    ul = VMMSWPFTSIDX_L;
    printf("VMMSWPFTSIDX_L = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* VMMSWPFTSIDX_L */

#ifdef VMMSWSR
    ul = VMMSWSR;
    printf("VMMSWSR = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* VMMSWSR */

#ifdef VMM_AME_ESID_FIRST
    ul = VMM_AME_ESID_FIRST;
    printf("VMM_AME_ESID_FIRST = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* VMM_AME_ESID_FIRST */

#ifdef VMM_AME_ESID_LAST
    ul = VMM_AME_ESID_LAST;
    printf("VMM_AME_ESID_LAST = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* VMM_AME_ESID_LAST */

#ifdef VMM_BASE_LEVEL
    ul = VMM_BASE_LEVEL;
    printf("VMM_BASE_LEVEL = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* VMM_BASE_LEVEL */

#ifdef VMM_COMPLEX_LOCK_AVAIL
    ul = VMM_COMPLEX_LOCK_AVAIL;
    printf("VMM_COMPLEX_LOCK_AVAIL = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* VMM_COMPLEX_LOCK_AVAIL */

#ifdef VMM_DATA_ESID_FIRST
    ul = VMM_DATA_ESID_FIRST;
    printf("VMM_DATA_ESID_FIRST = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* VMM_DATA_ESID_FIRST */

#ifdef VMM_DATA_ESID_LAST
    ul = VMM_DATA_ESID_LAST;
    printf("VMM_DATA_ESID_LAST = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* VMM_DATA_ESID_LAST */

#ifdef VMM_DMAP_ESID_FIRST
    ul = VMM_DMAP_ESID_FIRST;
    printf("VMM_DMAP_ESID_FIRST = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* VMM_DMAP_ESID_FIRST */

#ifdef VMM_DMAP_ESID_LAST
    ul = VMM_DMAP_ESID_LAST;
    printf("VMM_DMAP_ESID_LAST = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* VMM_DMAP_ESID_LAST */

#ifdef VMM_LOCK_HANDOFF
    ul = VMM_LOCK_HANDOFF;
    printf("VMM_LOCK_HANDOFF = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* VMM_LOCK_HANDOFF */

#ifdef VMM_OWNER_MASK
    ul = VMM_OWNER_MASK;
    printf("VMM_OWNER_MASK = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* VMM_OWNER_MASK */

#ifdef VMM_PTA_ESID_FIRST
    ul = VMM_PTA_ESID_FIRST;
    printf("VMM_PTA_ESID_FIRST = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* VMM_PTA_ESID_FIRST */

#ifdef VMM_PTA_ESID_LAST
    ul = VMM_PTA_ESID_LAST;
    printf("VMM_PTA_ESID_LAST = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* VMM_PTA_ESID_LAST */

#ifdef VMM_READ_MODE
    ul = VMM_READ_MODE;
    printf("VMM_READ_MODE = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* VMM_READ_MODE */

#ifdef VMM_SCB_ESID_FIRST
    ul = VMM_SCB_ESID_FIRST;
    printf("VMM_SCB_ESID_FIRST = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* VMM_SCB_ESID_FIRST */

#ifdef VMM_SCB_ESID_LAST
    ul = VMM_SCB_ESID_LAST;
    printf("VMM_SCB_ESID_LAST = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* VMM_SCB_ESID_LAST */

#ifdef VMM_SEG_ESID_FIRST
    ul = VMM_SEG_ESID_FIRST;
    printf("VMM_SEG_ESID_FIRST = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* VMM_SEG_ESID_FIRST */

#ifdef VMM_SEG_ESID_LAST
    ul = VMM_SEG_ESID_LAST;
    printf("VMM_SEG_ESID_LAST = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* VMM_SEG_ESID_LAST */

#ifdef VMM_SWHAT_ESID_FIRST
    ul = VMM_SWHAT_ESID_FIRST;
    printf("VMM_SWHAT_ESID_FIRST = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* VMM_SWHAT_ESID_FIRST */

#ifdef VMM_SWHAT_ESID_LAST
    ul = VMM_SWHAT_ESID_LAST;
    printf("VMM_SWHAT_ESID_LAST = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* VMM_SWHAT_ESID_LAST */

#ifdef VMM_SWPFT_ESID_FIRST
    ul = VMM_SWPFT_ESID_FIRST;
    printf("VMM_SWPFT_ESID_FIRST = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* VMM_SWPFT_ESID_FIRST */

#ifdef VMM_SWPFT_ESID_LAST
    ul = VMM_SWPFT_ESID_LAST;
    printf("VMM_SWPFT_ESID_LAST = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* VMM_SWPFT_ESID_LAST */

#ifdef VMM_WAITING
    ul = VMM_WAITING;
    printf("VMM_WAITING = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* VMM_WAITING */

#ifdef VMM_WANT_WRITE
    ul = VMM_WANT_WRITE;
    printf("VMM_WANT_WRITE = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* VMM_WANT_WRITE */

#ifdef VM_BACKT
    ul = VM_BACKT;
    printf("VM_BACKT = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* VM_BACKT */

#ifdef VM_INLRU
    ul = VM_INLRU;
    printf("VM_INLRU = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* VM_INLRU */

#ifdef VM_NOBACKT
    ul = VM_NOBACKT;
    printf("VM_NOBACKT = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* VM_NOBACKT */

#ifdef VM_ZQGET
    ul = VM_ZQGET;
    printf("VM_ZQGET = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* VM_ZQGET */

#ifdef VM_ZQPUT
    ul = VM_ZQPUT;
    printf("VM_ZQPUT = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* VM_ZQPUT */

#ifdef V_THRPGIO_PROT
    ul = V_THRPGIO_PROT;
    printf("V_THRPGIO_PROT = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* V_THRPGIO_PROT */

#ifdef WRITE_WAIT
    ul = WRITE_WAIT;
    printf("WRITE_WAIT = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* WRITE_WAIT */

#ifdef _64VMM
    ul = _64VMM;
    printf("_64VMM = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* _64VMM */

#ifdef _KSP_HIGHESID_BASE
    ul = _KSP_HIGHESID_BASE;
    printf("_KSP_HIGHESID_BASE = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* _KSP_HIGHESID_BASE */

#ifdef _KSP_HIGH_SIZE
    ul = _KSP_HIGH_SIZE;
    printf("_KSP_HIGH_SIZE = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* _KSP_HIGH_SIZE */

#ifdef _KSP_LOWESID_BASE
    ul = _KSP_LOWESID_BASE;
    printf("_KSP_LOWESID_BASE = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* _KSP_LOWESID_BASE */

#ifdef _KSP_LOW_SIZE
    ul = _KSP_LOW_SIZE;
    printf("_KSP_LOW_SIZE = %ld(%0*lx)\n", ul, sizeof(ul)*2, ul);
#endif /* _KSP_LOW_SIZE */

    return 0;
}
