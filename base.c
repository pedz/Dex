
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
#include <ldr/ld_private.h>
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

#include <vmm/vmcmem.h>
#include <vmm/vmdefs.h>
#include <vmm/vmpft.h>
#include <vmm/vmscb.h>
#include <vmm/m_vmpft.h>
#include <vmm/vmlsa.h>
#include <vmm/vmsslb.h>
#include <vmm/vmpft.h>
#include <lfs/lfs_numa.h>
#include <ras/ras_priv.h>
#include <sys/vmker.h>
#include <net/netopt.h>
#include <net/net_malloc.h>
#include <sctp_var.h>
#include <sys/specnode.h>

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
struct ppda v5;
swhatx_t v6;
aptx_t v7;
struct pftsw *v8;
struct exp_index_svc *v9;
LOADER_ANCHOR *v10;
struct tbtable_short *v11;
struct tbtable_ext *v12;
FILHDR *v13;
AOUTHDR *v14;
SCNHDR *v15;
pse_queue_t *v16;
STHP v17;
struct kmembuckets *v18;
struct ostd_mem *v19;
struct ostd_mem_rec *v20;
struct sel_cb *v21;
struct fsel_cb *v22;
struct exp_index *v23;
struct termios *v24;
struct kernvars *v25;
union in_addr_6 *v26;
struct socket *v27;
struct free_sock_hash_bucket *v28;
struct unpcb *v29;
struct sockaddr_un *v30;
struct inpcb *v31;
struct tcpcb *v32;
struct inpcb_hash_table *v33;
symptr v34;
struct netkmem_srad *v35;
struct dist_tcpstat *v36;
struct sndbufpool *v37;
struct mclust_pool *v38;
struct police_event *v39;
struct promote_frag_nmdbg *v40;
ndd_t *v41;
struct arptab *v42;
struct ifnet *v43;
struct hnode *v44;

static int print_it(int multi_1tb_ksp_segs)
{
    unsigned long ul;
    char *s;

    if (multi_1tb_ksp_segs)
	s = "unsigned long ";
    else
	s = "";

#ifdef AMESIDX_F
    ul = AMESIDX_F;
    printf("%sAMESIDX_F = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* AMESIDX_F */

#ifdef AMESIDX_L
    ul = AMESIDX_L;
    printf("%sAMESIDX_L = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* AMESIDX_L */

#ifdef AMESR
    ul = AMESR;
    printf("%sAMESR = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* AMESR */

#ifdef APTNULL
    ul = APTNULL;
    printf("%sAPTNULL = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* APTNULL */

#ifdef APTPIN
    ul = APTPIN;
    printf("%sAPTPIN = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* APTPIN */

#ifdef APTREG
    ul = APTREG;
    printf("%sAPTREG = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* APTREG */

#ifdef BASE_EXTSCB
    ul = BASE_EXTSCB;
    printf("%sBASE_EXTSCB = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* BASE_EXTSCB */

#ifdef BKTPFAULT
    ul = BKTPFAULT;
    printf("%sBKTPFAULT = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* BKTPFAULT */

#if defined(CUR_PTA_no)
    ul = CUR_PTA;
    printf("%sCUR_PTA = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* CUR_PTA */

#ifdef DMAPSIDX
    ul = DMAPSIDX;
    printf("%sDMAPSIDX = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* DMAPSIDX */

#ifdef EHANDLE
    ul = EHANDLE;
    printf("%sEHANDLE = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* EHANDLE */

#ifdef EISR
    ul = EISR;
    printf("%sEISR = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* EISR */

#ifdef EORGVADDR
    ul = EORGVADDR;
    printf("%sEORGVADDR = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* EORGVADDR */

#ifdef EPFTYPE
    ul = EPFTYPE;
    printf("%sEPFTYPE = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* EPFTYPE */

#ifdef ESRVAL
    ul = ESRVAL;
    printf("%sESRVAL = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* ESRVAL */

#ifdef EVADDR
    ul = EVADDR;
    printf("%sEVADDR = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* EVADDR */

#ifdef EXTSCB_MASK
    ul = EXTSCB_MASK;
    printf("%sEXTSCB_MASK = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* EXTSCB_MASK */

#ifdef FAIL_IF_HIDDEN
    ul = FAIL_IF_HIDDEN;
    printf("%sFAIL_IF_HIDDEN = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* FAIL_IF_HIDDEN */

#ifdef FBANCH
    ul = FBANCH;
    printf("%sFBANCH = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* FBANCH */

#ifdef FBLOWLIM
    ul = FBLOWLIM;
    printf("%sFBLOWLIM = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* FBLOWLIM */

#ifdef FBLRU
    ul = FBLRU;
    printf("%sFBLRU = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* FBLRU */

#ifdef FBLRU_REREF
    ul = FBLRU_REREF;
    printf("%sFBLRU_REREF = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* FBLRU_REREF */

#ifdef FBMAX
    ul = FBMAX;
    printf("%sFBMAX = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* FBMAX */

#ifdef FBUPLIM
    ul = FBUPLIM;
    printf("%sFBUPLIM = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* FBUPLIM */

#ifdef FIND_HIDDEN
    ul = FIND_HIDDEN;
    printf("%sFIND_HIDDEN = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* FIND_HIDDEN */

#ifdef FROZEN
    ul = FROZEN;
    printf("%sFROZEN = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* FROZEN */

#ifdef GBITSPERDW
    ul = GBITSPERDW;
    printf("%sGBITSPERDW = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* GBITSPERDW */

#ifdef GDWSPERMAP
    ul = GDWSPERMAP;
    printf("%sGDWSPERMAP = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* GDWSPERMAP */

#ifdef GL2BITSPERDW
    ul = GL2BITSPERDW;
    printf("%sGL2BITSPERDW = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* GL2BITSPERDW */

#ifdef GLOB_ESID_FIRST
    ul = GLOB_ESID_FIRST;
    printf("%sGLOB_ESID_FIRST = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* GLOB_ESID_FIRST */

#ifdef GLOB_ESID_LAST
    ul = GLOB_ESID_LAST;
    printf("%sGLOB_ESID_LAST = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* GLOB_ESID_LAST */

#ifdef GLOB_EXTREG_ESID_FIRST
    ul = GLOB_EXTREG_ESID_FIRST;
    printf("%sGLOB_EXTREG_ESID_FIRST = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* GLOB_EXTREG_ESID_FIRST */

#ifdef GLOB_EXTREG_ESID_LAST
    ul = GLOB_EXTREG_ESID_LAST;
    printf("%sGLOB_EXTREG_ESID_LAST = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* GLOB_EXTREG_ESID_LAST */

#ifdef GMAPSIZE
    ul = GMAPSIZE;
    printf("%sGMAPSIZE = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* GMAPSIZE */

#ifdef GROWPGAHEAD
    ul = GROWPGAHEAD;
    printf("%sGROWPGAHEAD = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* GROWPGAHEAD */

#ifdef HALT_NOMEM
    ul = HALT_NOMEM;
    printf("%sHALT_NOMEM = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* HALT_NOMEM */

#ifdef HIGH_USER_ESID_FIRST
    ul = HIGH_USER_ESID_FIRST;
    printf("%sHIGH_USER_ESID_FIRST = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* HIGH_USER_ESID_FIRST */

#ifdef HIGH_USER_ESID_LAST
    ul = HIGH_USER_ESID_LAST;
    printf("%sHIGH_USER_ESID_LAST = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* HIGH_USER_ESID_LAST */

#ifdef INSPFT
    ul = INSPFT;
    printf("%sINSPFT = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* INSPFT */

#if defined(INVALID_HANDLE) && !defined(__64BIT__)
    ul = INVALID_HANDLE;
    printf("%sINVALID_HANDLE = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* INVALID_HANDLE */

#ifdef IODONE
    ul = IODONE;
    printf("%sIODONE = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* IODONE */

#ifdef IOFORCESYNC
    ul = IOFORCESYNC;
    printf("%sIOFORCESYNC = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* IOFORCESYNC */

#ifdef IOSEGMENT
    ul = IOSEGMENT;
    printf("%sIOSEGMENT = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* IOSEGMENT */

#ifdef IOSIDBIT
    ul = IOSIDBIT;
    printf("%sIOSIDBIT = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* IOSIDBIT */

#ifdef IOSIDMASK
    ul = IOSIDMASK;
    printf("%sIOSIDMASK = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* IOSIDMASK */

#ifdef IOSYNC
    ul = IOSYNC;
    printf("%sIOSYNC = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* IOSYNC */

#ifdef JFS_LKW_ESID_FIRST
    ul = JFS_LKW_ESID_FIRST;
    printf("%sJFS_LKW_ESID_FIRST = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* JFS_LKW_ESID_FIRST */

#ifdef JFS_LKW_ESID_LAST
    ul = JFS_LKW_ESID_LAST;
    printf("%sJFS_LKW_ESID_LAST = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* JFS_LKW_ESID_LAST */

#ifdef JFS_SEG_ESID_FIRST
    ul = JFS_SEG_ESID_FIRST;
    printf("%sJFS_SEG_ESID_FIRST = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* JFS_SEG_ESID_FIRST */

#ifdef JFS_SEG_ESID_LAST
    ul = JFS_SEG_ESID_LAST;
    printf("%sJFS_SEG_ESID_LAST = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* JFS_SEG_ESID_LAST */

#ifdef KERNEL_ESID
    ul = KERNEL_ESID;
    printf("%sKERNEL_ESID = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* KERNEL_ESID */

#ifdef KERNEL_HEAP_ESID_FIRST
    ul = KERNEL_HEAP_ESID_FIRST;
    printf("%sKERNEL_HEAP_ESID_FIRST = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* KERNEL_HEAP_ESID_FIRST */

#ifdef KERNEL_HEAP_ESID_LAST
    ul = KERNEL_HEAP_ESID_LAST;
    printf("%sKERNEL_HEAP_ESID_LAST = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* KERNEL_HEAP_ESID_LAST */

#ifdef KERNSR
    ul = KERNSR;
    printf("%sKERNSR = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* KERNSR */

#ifdef KERNXSR
    ul = KERNXSR;
    printf("%sKERNXSR = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* KERNXSR */

#ifdef KERN_THRD_ESID_FIRST
    ul = KERN_THRD_ESID_FIRST;
    printf("%sKERN_THRD_ESID_FIRST = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* KERN_THRD_ESID_FIRST */

#ifdef KERN_THRD_ESID_LAST
    ul = KERN_THRD_ESID_LAST;
    printf("%sKERN_THRD_ESID_LAST = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* KERN_THRD_ESID_LAST */

#ifdef KERSIDX
    ul = KERSIDX;
    printf("%sKERSIDX = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* KERSIDX */

#ifdef KEXTSIDX
    ul = KEXTSIDX;
    printf("%sKEXTSIDX = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* KEXTSIDX */

#ifdef KGLOB_ESID_BASE
    ul = KGLOB_ESID_BASE;
    printf("%sKGLOB_ESID_BASE = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* KGLOB_ESID_BASE */

#ifdef KPRIV_ESID_BASE
    ul = KPRIV_ESID_BASE;
    printf("%sKPRIV_ESID_BASE = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* KPRIV_ESID_BASE */

#ifdef KSP_ARCH_ESID_BASE
    ul = KSP_ARCH_ESID_BASE;
    printf("%sKSP_ARCH_ESID_BASE = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* KSP_ARCH_ESID_BASE */

#ifdef KSP_ARCH_ESID_END
    ul = KSP_ARCH_ESID_END;
    printf("%sKSP_ARCH_ESID_END = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* KSP_ARCH_ESID_END */

#ifdef KSP_ARCH_NUMSIDS
    ul = KSP_ARCH_NUMSIDS;
    printf("%sKSP_ARCH_NUMSIDS = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* KSP_ARCH_NUMSIDS */

#ifdef KSP_ESID_BASE
    ul = KSP_ESID_BASE;
    printf("%sKSP_ESID_BASE = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* KSP_ESID_BASE */

#ifdef KSP_ESID_END
    ul = KSP_ESID_END;
    printf("%sKSP_ESID_END = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* KSP_ESID_END */

#ifdef KSP_NUMSIDS
    ul = KSP_NUMSIDS;
    printf("%sKSP_NUMSIDS = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* KSP_NUMSIDS */

#ifdef KSP_SID_BASE
    ul = KSP_SID_BASE;
    printf("%sKSP_SID_BASE = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* KSP_SID_BASE */

#ifdef KSP_S_ARCH_ESID_BASE
    ul = KSP_S_ARCH_ESID_BASE;
    printf("%sKSP_S_ARCH_ESID_BASE = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* KSP_S_ARCH_ESID_BASE */

#ifdef KSP_S_ARCH_ESID_END
    ul = KSP_S_ARCH_ESID_END;
    printf("%sKSP_S_ARCH_ESID_END = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* KSP_S_ARCH_ESID_END */

#ifdef KSP_S_ARCH_NUMSIDS
    ul = KSP_S_ARCH_NUMSIDS;
    printf("%sKSP_S_ARCH_NUMSIDS = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* KSP_S_ARCH_NUMSIDS */

#ifdef KSP_S_ESID_BASE
    ul = KSP_S_ESID_BASE;
    printf("%sKSP_S_ESID_BASE = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* KSP_S_ESID_BASE */

#ifdef KSP_S_ESID_END
    ul = KSP_S_ESID_END;
    printf("%sKSP_S_ESID_END = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* KSP_S_ESID_END */

#ifdef KSP_S_NUMSIDS
    ul = KSP_S_NUMSIDS;
    printf("%sKSP_S_NUMSIDS = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* KSP_S_NUMSIDS */

#ifdef KSP_TOTAL_SIDS
    ul = KSP_TOTAL_SIDS;
    printf("%sKSP_TOTAL_SIDS = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* KSP_TOTAL_SIDS */

#ifdef KTEMP_ESID_BASE
    ul = KTEMP_ESID_BASE;
    printf("%sKTEMP_ESID_BASE = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* KTEMP_ESID_BASE */

#ifdef L2LINESIZE
    ul = L2LINESIZE;
    printf("%sL2LINESIZE = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* L2LINESIZE */

#ifdef L2PSIZE
    ul = L2PSIZE;
    printf("%sL2PSIZE = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* L2PSIZE */

#ifdef L2SSIZE
    ul = L2SSIZE;
    printf("%sL2SSIZE = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* L2SSIZE */

#ifdef L2SSIZE_1TB
    ul = L2SSIZE_1TB;
    printf("%sL2SSIZE_1TB = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* L2SSIZE */

#ifdef LDR_LIB_ESID_FIRST
    ul = LDR_LIB_ESID_FIRST;
    printf("%sLDR_LIB_ESID_FIRST = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* LDR_LIB_ESID_FIRST */

#ifdef LDR_LIB_ESID_LAST
    ul = LDR_LIB_ESID_LAST;
    printf("%sLDR_LIB_ESID_LAST = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* LDR_LIB_ESID_LAST */

#ifdef LDR_PRIV_ADDR
    ul = LDR_PRIV_ADDR;
    printf("%sLDR_PRIV_ADDR = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* LDR_PRIV_ADDR */

#ifdef LDR_PRIV_ESID
    ul = LDR_PRIV_ESID;
    printf("%sLDR_PRIV_ESID = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* LDR_PRIV_ESID */

#ifdef LFS_ESID_RANGE
    ul = LFS_ESID_RANGE;
    printf("%sLFS_ESID_RANGE = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* LFS_ESID_RANGE */

#ifdef LFS_SEG_ESID_FIRST
    ul = LFS_SEG_ESID_FIRST;
    printf("%sLFS_SEG_ESID_FIRST = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* LFS_SEG_ESID_FIRST */

#ifdef LFS_SEG_ESID_LAST
    ul = LFS_SEG_ESID_LAST;
    printf("%sLFS_SEG_ESID_LAST = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* LFS_SEG_ESID_LAST */

#ifdef LINESIZE
    ul = LINESIZE;
    printf("%sLINESIZE = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* LINESIZE */

#ifdef LOCK_INSTR_ESID_FIRST
    ul = LOCK_INSTR_ESID_FIRST;
    printf("%sLOCK_INSTR_ESID_FIRST = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* LOCK_INSTR_ESID_FIRST */

#ifdef LOCK_INSTR_ESID_LAST
    ul = LOCK_INSTR_ESID_LAST;
    printf("%sLOCK_INSTR_ESID_LAST = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* LOCK_INSTR_ESID_LAST */

#ifdef LONG_TERM
    ul = LONG_TERM;
    printf("%sLONG_TERM = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* LONG_TERM */

#ifdef LOW_USER_ESID_FIRST
    ul = LOW_USER_ESID_FIRST;
    printf("%sLOW_USER_ESID_FIRST = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* LOW_USER_ESID_FIRST */

#ifdef LOW_USER_ESID_LAST
    ul = LOW_USER_ESID_LAST;
    printf("%sLOW_USER_ESID_LAST = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* LOW_USER_ESID_LAST */

#ifdef LRU_ANY
    ul = LRU_ANY;
    printf("%sLRU_ANY = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* LRU_ANY */

#ifdef LRU_FILE
    ul = LRU_FILE;
    printf("%sLRU_FILE = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* LRU_FILE */

#ifdef LRU_UNMOD
    ul = LRU_UNMOD;
    printf("%sLRU_UNMOD = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* LRU_UNMOD */

#if 0
#ifdef LTPIN_DEC
    ul = LTPIN_DEC;
    printf("%sLTPIN_DEC = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* LTPIN_DEC */

#ifdef LTPIN_INC
    ul = LTPIN_INC;
    printf("%sLTPIN_INC = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* LTPIN_INC */

#ifdef LTPIN_MASK
    ul = LTPIN_MASK;
    printf("%sLTPIN_MASK = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* LTPIN_MASK */
#endif

#ifdef LTPIN_NBITS
    ul = LTPIN_NBITS;
    printf("%sLTPIN_NBITS = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* LTPIN_NBITS */

#ifdef LTPIN_ONE
    ul = LTPIN_ONE;
    printf("%sLTPIN_ONE = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* LTPIN_ONE */

#if 0
#ifdef LTPIN_OVERFLOW
    ul = LTPIN_OVERFLOW;
    printf("%sLTPIN_OVERFLOW = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* LTPIN_OVERFLOW */
#endif

#ifdef MAXAPT
    ul = MAXAPT;
    printf("%sMAXAPT = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* MAXAPT */

#ifdef MAXLOGS
    ul = MAXLOGS;
    printf("%sMAXLOGS = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* MAXLOGS */

#ifdef MAXPGAHEAD
    ul = MAXPGAHEAD;
    printf("%sMAXPGAHEAD = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* MAXPGAHEAD */

#ifdef MAXPGSP
    ul = MAXPGSP;
    printf("%sMAXPGSP = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* MAXPGSP */

#if 0
#ifdef MAXPIN
    ul = MAXPIN;
    printf("%sMAXPIN = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* MAXPIN */
#endif

#ifdef MAXREPAGE
    ul = MAXREPAGE;
    printf("%sMAXREPAGE = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* MAXREPAGE */

#ifdef MAXVPN
    ul = MAXVPN;
    printf("%sMAXVPN = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* MAXVPN */

#ifdef MBUF_POOL_ESID_FIRST
    ul = MBUF_POOL_ESID_FIRST;
    printf("%sMBUF_POOL_ESID_FIRST = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* MBUF_POOL_ESID_FIRST */

#ifdef MBUF_POOL_ESID_LAST
    ul = MBUF_POOL_ESID_LAST;
    printf("%sMBUF_POOL_ESID_LAST = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* MBUF_POOL_ESID_LAST */

#ifdef MINPGAHEAD
    ul = MINPGAHEAD;
    printf("%sMINPGAHEAD = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* MINPGAHEAD */

#ifdef MPDATA_ESID_FIRST
    ul = MPDATA_ESID_FIRST;
    printf("%sMPDATA_ESID_FIRST = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* MPDATA_ESID_FIRST */

#ifdef MPDATA_ESID_LAST
    ul = MPDATA_ESID_LAST;
    printf("%sMPDATA_ESID_LAST = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* MPDATA_ESID_LAST */

#ifdef NFRAGHASH
    ul = NFRAGHASH;
    printf("%sNFRAGHASH = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* NFRAGHASH */

#ifdef NLOCKBITS
    ul = NLOCKBITS;
    printf("%sNLOCKBITS = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* NLOCKBITS */

#ifdef NOFBLRU
    ul = NOFBLRU;
    printf("%sNOFBLRU = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* NOFBLRU */

#ifdef NOFROZEN
    ul = NOFROZEN;
    printf("%sNOFROZEN = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* NOFROZEN */

#ifdef NORMAL
    ul = NORMAL;
    printf("%sNORMAL = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* NORMAL */

#ifdef NORMAL_PIN_PTE
    ul = NORMAL_PIN_PTE;
    printf("%sNORMAL_PIN_PTE = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* NORMAL_PIN_PTE */

#ifdef NOSIO
    ul = NOSIO;
    printf("%sNOSIO = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* NOSIO */

#ifdef NO_INSPFT
    ul = NO_INSPFT;
    printf("%sNO_INSPFT = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* NO_INSPFT */

#ifdef NRSVDBLKS
    ul = NRSVDBLKS;
    printf("%sNRSVDBLKS = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* NRSVDBLKS */

#ifdef NUMESIDS
    ul = NUMESIDS;
    printf("%sNUMESIDS = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* NUMESIDS */

#ifdef NUMPAGES
    ul = NUMPAGES;
    printf("%sNUMPAGES = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* NUMPAGES */

#ifdef NUMSAVE
    ul = NUMSAVE;
    printf("%sNUMSAVE = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* NUMSAVE */

#ifdef NUMSIDS
    ul = NUMSIDS;
    printf("%sNUMSIDS = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* NUMSIDS */

#ifdef NUMZQS
    ul = NUMZQS;
    printf("%sNUMZQS = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* NUMZQS */

#ifdef NUM_GLOB_ESIDS
    ul = NUM_GLOB_ESIDS;
    printf("%sNUM_GLOB_ESIDS = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* NUM_GLOB_ESIDS */

#ifdef NUM_RMALLOCSIDS
    ul = NUM_RMALLOCSIDS;
    printf("%sNUM_RMALLOCSIDS = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* NUM_RMALLOCSIDS */

#ifdef NUM_SCBSIDS
    ul = NUM_SCBSIDS;
    printf("%sNUM_SCBSIDS = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* NUM_SCBSIDS */

#ifdef NUM_SI_VMM_ESIDS
    ul = NUM_SI_VMM_ESIDS;
    printf("%sNUM_SI_VMM_ESIDS = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* NUM_SI_VMM_ESIDS */

#ifdef ONES64
    ul = ONES64;
    printf("%sONES64 = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* ONES64 */

#ifdef ONES
    ul = ONES;
    printf("%sONES = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* ONES */

#ifdef ORGPFAULT
    ul = ORGPFAULT;
    printf("%sORGPFAULT = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* ORGPFAULT */

#ifdef PFMINPIN
    ul = PFMINPIN;
    printf("%sPFMINPIN = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* PFMINPIN */

#ifdef PIN_SETMOD
    ul = PIN_SETMOD;
    printf("%sPIN_SETMOD = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* PIN_SETMOD */

#ifdef POFFSET
    ul = POFFSET;
    printf("%sPOFFSET = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* POFFSET */

#ifdef PORTIOSIDX
    ul = PORTIOSIDX;
    printf("%sPORTIOSIDX = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* PORTIOSIDX */

#ifdef PORTIOSIDX_L
    ul = PORTIOSIDX_L;
    printf("%sPORTIOSIDX_L = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* PORTIOSIDX_L */

#ifdef PORTIO_ESID_FIRST
    ul = PORTIO_ESID_FIRST;
    printf("%sPORTIO_ESID_FIRST = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* PORTIO_ESID_FIRST */

#ifdef PORTIO_ESID_LAST
    ul = PORTIO_ESID_LAST;
    printf("%sPORTIO_ESID_LAST = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* PORTIO_ESID_LAST */

#ifdef PORTIO_NUM_ESIDS
    ul = PORTIO_NUM_ESIDS;
    printf("%sPORTIO_NUM_ESIDS = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* PORTIO_NUM_ESIDS */

#ifdef PROCSR
    ul = PROCSR;
    printf("%sPROCSR = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* PROCSR */

#ifdef PROC_PRIV_ESID
    ul = PROC_PRIV_ESID;
    printf("%sPROC_PRIV_ESID = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* PROC_PRIV_ESID */

#ifdef PROC_THRD_TABLE_ESID_FIRST
    ul = PROC_THRD_TABLE_ESID_FIRST;
    printf("%sPROC_THRD_TABLE_ESID_FIRST = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* PROC_THRD_TABLE_ESID_FIRST */

#ifdef PROC_THRD_TABLE_ESID_LAST
    ul = PROC_THRD_TABLE_ESID_LAST;
    printf("%sPROC_THRD_TABLE_ESID_LAST = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* PROC_THRD_TABLE_ESID_LAST */

#ifdef PSIZE
    ul = PSIZE;
    printf("%sPSIZE = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* PSIZE */

#if defined(PTAORG) && !defined(__64BIT__)
    ul = PTAORG;
    printf("%sPTAORG = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* PTAORG */

#if defined(PTASEG_dog)
    ul = PTASEG;
    printf("%sPTASEG = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* PTASEG */

#if defined(PTASEG_dog)
    ul = PTASEG_REF;
    printf("%sPTASEG_REF = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* PTASEG_REF */

#ifdef PTASIDX
    ul = PTASIDX;
    printf("%sPTASIDX = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* PTASIDX */

#ifdef PTASR
    ul = PTASR;
    printf("%sPTASR = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* PTASR */

#ifdef PVNULL
    ul = PVNULL;
    printf("%sPVNULL = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* PVNULL */

#ifdef QIO
    ul = QIO;
    printf("%sQIO = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* QIO */

#ifdef READ_WAIT
    ul = READ_WAIT;
    printf("%sREAD_WAIT = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* READ_WAIT */

#ifdef RELOAD
    ul = RELOAD;
    printf("%sRELOAD = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* RELOAD */

#ifdef RMALLOC_HEAP_ESID_FIRST
    ul = RMALLOC_HEAP_ESID_FIRST;
    printf("%sRMALLOC_HEAP_ESID_FIRST = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* RMALLOC_HEAP_ESID_FIRST */

#ifdef RMALLOC_HEAP_ESID_LAST
    ul = RMALLOC_HEAP_ESID_LAST;
    printf("%sRMALLOC_HEAP_ESID_LAST = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* RMALLOC_HEAP_ESID_LAST */

#ifdef RMALLOC_NUM_ESIDS
    ul = RMALLOC_NUM_ESIDS;
    printf("%sRMALLOC_NUM_ESIDS = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* RMALLOC_NUM_ESIDS */

#ifdef RMAPSIDX
    ul = RMAPSIDX;
    printf("%sRMAPSIDX = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* RMAPSIDX */

#ifdef RMAPSIDX_L
    ul = RMAPSIDX_L;
    printf("%sRMAPSIDX_L = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* RMAPSIDX_L */

#ifdef RMCOPY_NOINSERT
    ul = RMCOPY_NOINSERT;
    printf("%sRMCOPY_NOINSERT = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* RMCOPY_NOINSERT */

#ifdef RPCOMP
    ul = RPCOMP;
    printf("%sRPCOMP = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* RPCOMP */

#ifdef RPFILE
    ul = RPFILE;
    printf("%sRPFILE = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* RPFILE */

#ifdef RPTYPES
    ul = RPTYPES;
    printf("%sRPTYPES = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* RPTYPES */

#ifdef RSVDSID
    ul = RSVDSID;
    printf("%sRSVDSID = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* RSVDSID */

#ifdef RTFWSIDX
    ul = RTFWSIDX;
    printf("%sRTFWSIDX = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* RTFWSIDX */

#ifdef RTFWSIDX_L
    ul = RTFWSIDX_L;
    printf("%sRTFWSIDX_L = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* RTFWSIDX_L */

#ifdef RTFW_ESID_FIRST
    ul = RTFW_ESID_FIRST;
    printf("%sRTFW_ESID_FIRST = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* RTFW_ESID_FIRST */

#ifdef RTFW_ESID_LAST
    ul = RTFW_ESID_LAST;
    printf("%sRTFW_ESID_LAST = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* RTFW_ESID_LAST */

#ifdef RTFW_NUM_ESIDS
    ul = RTFW_NUM_ESIDS;
    printf("%sRTFW_NUM_ESIDS = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* RTFW_NUM_ESIDS */

#ifdef SHORT_TERM
    ul = SHORT_TERM;
    printf("%sSHORT_TERM = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* SHORT_TERM */

#ifdef SHX_ALLOCATE
    ul = SHX_ALLOCATE;
    printf("%sSHX_ALLOCATE = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* SHX_ALLOCATE */

#ifdef SHX_FULLOVER
    ul = SHX_FULLOVER;
    printf("%sSHX_FULLOVER = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* SHX_FULLOVER */

#ifdef SHX_NOALLOCATE
    ul = SHX_NOALLOCATE;
    printf("%sSHX_NOALLOCATE = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* SHX_NOALLOCATE */

#ifdef SIDLIMIT
    ul = SIDLIMIT;
    printf("%sSIDLIMIT = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* SIDLIMIT */

#ifdef SIO
    ul = SIO;
    printf("%sSIO = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* SIO */

#ifdef SOFFSET
    ul = SOFFSET;
    printf("%sSOFFSET = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* SOFFSET */

#ifdef SOFFSET_1TB
    ul = SOFFSET_1TB;
    printf("%sSOFFSET_1TB = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* SOFFSET */

#ifdef SREGMSK
    ul = SREGMSK;
    printf("%sSREGMSK = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* SREGMSK */

#ifdef STARTIO
    ul = STARTIO;
    printf("%sSTARTIO = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* STARTIO */

#ifdef STKBRK_REG
    ul = STKBRK_REG;
    printf("%sSTKBRK_REG = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* STKBRK_REG */

#ifdef STOIBITS_32GB
    ul = STOIBITS_32GB;
    printf("%sSTOIBITS_32GB = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* STOIBITS_32GB */

#ifdef STOIBITS_64GB
    ul = STOIBITS_64GB;
    printf("%sSTOIBITS_64GB = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* STOIBITS_64GB */

#ifdef STOIBITS_MAX
    ul = STOIBITS_MAX;
    printf("%sSTOIBITS_MAX = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* STOIBITS_MAX */

#if 0
#ifdef STPIN_DEC
    ul = STPIN_DEC;
    printf("%sSTPIN_DEC = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* STPIN_DEC */

#ifdef STPIN_INC
    ul = STPIN_INC;
    printf("%sSTPIN_INC = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* STPIN_INC */
#endif

#ifdef STPIN_MASK
    ul = STPIN_MASK;
    printf("%sSTPIN_MASK = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* STPIN_MASK */

#ifdef STPIN_NBITS
    ul = STPIN_NBITS;
    printf("%sSTPIN_NBITS = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* STPIN_NBITS */

#ifdef STPIN_ONE
    ul = STPIN_ONE;
    printf("%sSTPIN_ONE = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* STPIN_ONE */

#if 0
#ifdef STPIN_OVERFLOW
    ul = STPIN_OVERFLOW;
    printf("%sSTPIN_OVERFLOW = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* STPIN_OVERFLOW */
#endif

#ifdef SZBIT
    ul = SZBIT;
    printf("%sSZBIT = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* SZBIT */

#ifdef TEMPSR
    ul = TEMPSR;
    printf("%sTEMPSR = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* TEMPSR */

#ifdef TEXTSR
    ul = TEXTSR;
    printf("%sTEXTSR = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* TEXTSR */

#if defined(UNKNOWN_HANDLE) && !defined(__64BIT__)
    ul = UNKNOWN_HANDLE;
    printf("%sUNKNOWN_HANDLE = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* UNKNOWN_HANDLE */

#ifdef UT_THRPGIOTAIL_FLUSHP
    ul = UT_THRPGIOTAIL_FLUSHP;
    printf("%sUT_THRPGIOTAIL_FLUSHP = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* UT_THRPGIOTAIL_FLUSHP */

#ifdef UT_THRPGIOTAIL_INIT
    ul = UT_THRPGIOTAIL_INIT;
    printf("%sUT_THRPGIOTAIL_INIT = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* UT_THRPGIOTAIL_INIT */

#ifdef UT_THRPGIOTAIL_PFGET
    ul = UT_THRPGIOTAIL_PFGET;
    printf("%sUT_THRPGIOTAIL_PFGET = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* UT_THRPGIOTAIL_PFGET */

#ifdef UT_THRPGIOTAIL_READP
    ul = UT_THRPGIOTAIL_READP;
    printf("%sUT_THRPGIOTAIL_READP = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* UT_THRPGIOTAIL_READP */

#ifdef UT_THRPGIOTAIL_WRITEP
    ul = UT_THRPGIOTAIL_WRITEP;
    printf("%sUT_THRPGIOTAIL_WRITEP = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* UT_THRPGIOTAIL_WRITEP */

#ifdef UZBIT
    ul = UZBIT;
    printf("%sUZBIT = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* UZBIT */

#ifdef VMM1SWSIDX
    ul = VMM1SWSIDX;
    printf("%sVMM1SWSIDX = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* VMM1SWSIDX */

#ifdef VMM2SWSIDX
    ul = VMM2SWSIDX;
    printf("%sVMM2SWSIDX = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* VMM2SWSIDX */

#ifdef VMM2SWSR
    ul = VMM2SWSR;
    printf("%sVMM2SWSR = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* VMM2SWSR */

#ifdef VMM3SWSIDX
    ul = VMM3SWSIDX;
    printf("%sVMM3SWSIDX = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* VMM3SWSIDX */

#ifdef VMM3SWSR
    ul = VMM3SWSR;
    printf("%sVMM3SWSR = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* VMM3SWSR */

#ifdef VMM4SWSIDX
    ul = VMM4SWSIDX;
    printf("%sVMM4SWSIDX = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* VMM4SWSIDX */

#ifdef VMM4SWSR
    ul = VMM4SWSR;
    printf("%sVMM4SWSR = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* VMM4SWSR */

#ifdef VMM5SWSIDX
    ul = VMM5SWSIDX;
    printf("%sVMM5SWSIDX = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* VMM5SWSIDX */

#ifdef VMM5SWSR
    ul = VMM5SWSR;
    printf("%sVMM5SWSR = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* VMM5SWSR */

#ifdef VMM6SWSIDX
    ul = VMM6SWSIDX;
    printf("%sVMM6SWSIDX = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* VMM6SWSIDX */

#ifdef VMM6SWSR
    ul = VMM6SWSR;
    printf("%sVMM6SWSR = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* VMM6SWSR */

#ifdef VMMSCBSIDX_F
    ul = VMMSCBSIDX_F;
    printf("%sVMMSCBSIDX_F = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* VMMSCBSIDX_F */

#ifdef VMMSCBSIDX_L
    ul = VMMSCBSIDX_L;
    printf("%sVMMSCBSIDX_L = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* VMMSCBSIDX_L */

#ifdef VMMSIDX
    ul = VMMSIDX;
    printf("%sVMMSIDX = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* VMMSIDX */

#ifdef VMMSR
    ul = VMMSR;
    printf("%sVMMSR = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* VMMSR */

#ifdef VMMSWHATSIDX
    ul = VMMSWHATSIDX;
    printf("%sVMMSWHATSIDX = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* VMMSWHATSIDX */

#ifdef VMMSWHATSIDX_F
    ul = VMMSWHATSIDX_F;
    printf("%sVMMSWHATSIDX_F = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* VMMSWHATSIDX_F */

#ifdef VMMSWHATSIDX_L
    ul = VMMSWHATSIDX_L;
    printf("%sVMMSWHATSIDX_L = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* VMMSWHATSIDX_L */

#ifdef VMMSWHATSR
    ul = VMMSWHATSR;
    printf("%sVMMSWHATSR = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* VMMSWHATSR */

#ifdef VMMSWPFTSIDX_F
    ul = VMMSWPFTSIDX_F;
    printf("%sVMMSWPFTSIDX_F = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* VMMSWPFTSIDX_F */

#ifdef VMMSWPFTSIDX_L
    ul = VMMSWPFTSIDX_L;
    printf("%sVMMSWPFTSIDX_L = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* VMMSWPFTSIDX_L */

#ifdef VMMSWSR
    ul = VMMSWSR;
    printf("%sVMMSWSR = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* VMMSWSR */

#ifdef VMM_AME_ESID_FIRST
    ul = VMM_AME_ESID_FIRST;
    printf("%sVMM_AME_ESID_FIRST = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* VMM_AME_ESID_FIRST */

#ifdef VMM_AME_ESID_LAST
    ul = VMM_AME_ESID_LAST;
    printf("%sVMM_AME_ESID_LAST = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* VMM_AME_ESID_LAST */

#ifdef VMM_BASE_LEVEL
    ul = VMM_BASE_LEVEL;
    printf("%sVMM_BASE_LEVEL = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* VMM_BASE_LEVEL */

#ifdef VMM_COMPLEX_LOCK_AVAIL
    ul = VMM_COMPLEX_LOCK_AVAIL;
    printf("%sVMM_COMPLEX_LOCK_AVAIL = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* VMM_COMPLEX_LOCK_AVAIL */

#ifdef VMM_DATA_ESID_FIRST
    ul = VMM_DATA_ESID_FIRST;
    printf("%sVMM_DATA_ESID_FIRST = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* VMM_DATA_ESID_FIRST */

#ifdef VMM_DATA_ESID_LAST
    ul = VMM_DATA_ESID_LAST;
    printf("%sVMM_DATA_ESID_LAST = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* VMM_DATA_ESID_LAST */

#ifdef VMM_DMAP_ESID_FIRST
    ul = VMM_DMAP_ESID_FIRST;
    printf("%sVMM_DMAP_ESID_FIRST = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* VMM_DMAP_ESID_FIRST */

#ifdef VMM_DMAP_ESID_LAST
    ul = VMM_DMAP_ESID_LAST;
    printf("%sVMM_DMAP_ESID_LAST = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* VMM_DMAP_ESID_LAST */

#ifdef VMM_LOCK_HANDOFF
    ul = VMM_LOCK_HANDOFF;
    printf("%sVMM_LOCK_HANDOFF = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* VMM_LOCK_HANDOFF */

#ifdef VMM_OWNER_MASK
    ul = VMM_OWNER_MASK;
    printf("%sVMM_OWNER_MASK = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* VMM_OWNER_MASK */

#ifdef VMM_PTA_ESID_FIRST
    ul = VMM_PTA_ESID_FIRST;
    printf("%sVMM_PTA_ESID_FIRST = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* VMM_PTA_ESID_FIRST */

#ifdef VMM_PTA_ESID_LAST
    ul = VMM_PTA_ESID_LAST;
    printf("%sVMM_PTA_ESID_LAST = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* VMM_PTA_ESID_LAST */

#ifdef VMM_READ_MODE
    ul = VMM_READ_MODE;
    printf("%sVMM_READ_MODE = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* VMM_READ_MODE */

#ifdef VMM_SCB_ESID_FIRST
    ul = VMM_SCB_ESID_FIRST;
    printf("%sVMM_SCB_ESID_FIRST = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* VMM_SCB_ESID_FIRST */

#ifdef VMM_SCB_ESID_LAST
    ul = VMM_SCB_ESID_LAST;
    printf("%sVMM_SCB_ESID_LAST = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* VMM_SCB_ESID_LAST */

#ifdef VMM_SEG_ESID_FIRST
    ul = VMM_SEG_ESID_FIRST;
    printf("%sVMM_SEG_ESID_FIRST = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* VMM_SEG_ESID_FIRST */

#ifdef VMM_SEG_ESID_LAST
    ul = VMM_SEG_ESID_LAST;
    printf("%sVMM_SEG_ESID_LAST = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* VMM_SEG_ESID_LAST */

#ifdef VMM_SWHAT_ESID_FIRST
    ul = VMM_SWHAT_ESID_FIRST;
    printf("%sVMM_SWHAT_ESID_FIRST = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* VMM_SWHAT_ESID_FIRST */

#ifdef VMM_SWHAT_ESID_LAST
    ul = VMM_SWHAT_ESID_LAST;
    printf("%sVMM_SWHAT_ESID_LAST = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* VMM_SWHAT_ESID_LAST */

#ifdef VMM_SWPFT_ESID_FIRST
    ul = VMM_SWPFT_ESID_FIRST;
    printf("%sVMM_SWPFT_ESID_FIRST = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* VMM_SWPFT_ESID_FIRST */

#ifdef VMM_SWPFT_ESID_LAST
    ul = VMM_SWPFT_ESID_LAST;
    printf("%sVMM_SWPFT_ESID_LAST = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* VMM_SWPFT_ESID_LAST */

#ifdef VMM_WAITING
    ul = VMM_WAITING;
    printf("%sVMM_WAITING = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* VMM_WAITING */

#ifdef VMM_WANT_WRITE
    ul = VMM_WANT_WRITE;
    printf("%sVMM_WANT_WRITE = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* VMM_WANT_WRITE */

#ifdef VM_BACKT
    ul = VM_BACKT;
    printf("%sVM_BACKT = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* VM_BACKT */

#ifdef VM_INLRU
    ul = VM_INLRU;
    printf("%sVM_INLRU = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* VM_INLRU */

#ifdef VM_NOBACKT
    ul = VM_NOBACKT;
    printf("%sVM_NOBACKT = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* VM_NOBACKT */

#ifdef VM_ZQGET
    ul = VM_ZQGET;
    printf("%sVM_ZQGET = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* VM_ZQGET */

#ifdef VM_ZQPUT
    ul = VM_ZQPUT;
    printf("%sVM_ZQPUT = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* VM_ZQPUT */

#ifdef V_THRPGIO_PROT
    ul = V_THRPGIO_PROT;
    printf("%sV_THRPGIO_PROT = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* V_THRPGIO_PROT */

#ifdef WRITE_WAIT
    ul = WRITE_WAIT;
    printf("%sWRITE_WAIT = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* WRITE_WAIT */

#ifdef _64VMM
    ul = _64VMM;
    printf("%s_64VMM = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* _64VMM */

#ifdef _KSP_HIGHESID_BASE
    ul = _KSP_HIGHESID_BASE;
    printf("%s_KSP_HIGHESID_BASE = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* _KSP_HIGHESID_BASE */

#ifdef _KSP_HIGH_SIZE
    ul = _KSP_HIGH_SIZE;
    printf("%s_KSP_HIGH_SIZE = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* _KSP_HIGH_SIZE */

#ifdef _KSP_LOWESID_BASE
    ul = _KSP_LOWESID_BASE;
    printf("%s_KSP_LOWESID_BASE = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* _KSP_LOWESID_BASE */

#ifdef _KSP_LOW_SIZE
    ul = _KSP_LOW_SIZE;
    printf("%s_KSP_LOW_SIZE = %#018lxul; /* %ld */\n", s, ul, ul);
#endif /* _KSP_LOW_SIZE */

#ifdef GALLOC_OFF_F
    ul = GALLOC_OFF_F;
    printf("%sGALLOC_OFF_F = %#018lxul; /* %ld */\n", s, ul, ul);
#endif

#ifdef GLOBKER_1TB_OFF_F
    ul = GLOBKER_1TB_OFF_F;
    printf("%sGLOBKER_1TB_OFF_F = %#018lxul; /* %ld */\n", s, ul, ul);
#endif

#ifdef GLOBKER_4K_OFF_F
    ul = GLOBKER_4K_OFF_F;
    printf("%sGLOBKER_4K_OFF_F = %#018lxul; /* %ld */\n", s, ul, ul);
#endif

    ul = NTHREAD;
    printf("%sNTHREAD = %#018lxul; /* %ld */\n", s, ul, ul);

    ul = NONGK_1TB_OFF_F;
    printf("%sNONGK_1TB_OFF_F = %#018lxul; /* %ld */\n", s, ul, ul);

    ul = NONGK_1TB_OFF_L;
    printf("%sNONGK_1TB_OFF_L = %#018lxul; /* %ld */\n", s, ul, ul);

    ul = KERNEL_OFF;
    printf("%sKERNEL_OFF = %#018lxul; /* %ld */\n", s, ul, ul);

#ifdef HI_GLOB_KERN_OFF_F
    ul = GA_UNUSED1;
    printf("%sGA_UNUSED1 = %#018lxul; /* %ld */\n", s, ul, ul);

    ul = HI_GLOB_KERN_OFF_F;
    printf("%sHI_GLOB_KERN_OFF_F = %#018lxul; /* %ld */\n", s, ul, ul);
#endif

    ul = GALLOC_4K_OFF_F;
    printf("%sGALLOC_4K_OFF_F = %#018lxul; /* %ld */\n", s, ul, ul);

    ul = GALLOC_4K_OFF_L;
    printf("%sGALLOC_4K_OFF_L = %#018lxul; /* %ld */\n", s, ul, ul);

#ifdef DIR_GALLOC_4K_OFF_F
    ul = DIR_GALLOC_4K_OFF_F;
    printf("%sDIR_GALLOC_4K_OFF_F = %#018lxul; /* %ld */\n", s, ul, ul);
#endif

#ifdef KHEAP_4K_OFF_F
    ul = KHEAP_4K_OFF_F;
    printf("%sKHEAP_4K_OFF_F = %#018lxul; /* %ld */\n", s, ul, ul);
#endif

#ifdef KHEAP_4K_OFF_L
    ul = KHEAP_4K_OFF_L;
    printf("%sKHEAP_4K_OFF_L = %#018lxul; /* %ld */\n", s, ul, ul);
#endif

#ifdef MBUF_4K_OFF_F
    ul = MBUF_4K_OFF_F;
    printf("%sMBUF_4K_OFF_F = %#018lxul; /* %ld */\n", s, ul, ul);
#endif

#ifdef MBUF_4K_OFF_L
    ul = MBUF_4K_OFF_L;
    printf("%sMBUF_4K_OFF_L = %#018lxul; /* %ld */\n", s, ul, ul);
#endif

#ifdef MTRC_4K_OFF_F
    ul = MTRC_4K_OFF_F;
    printf("%sMTRC_4K_OFF_F = %#018lxul; /* %ld */\n", s, ul, ul);
#endif

#ifdef MTRC_4K_OFF_L
    ul = MTRC_4K_OFF_L;
    printf("%sMTRC_4K_OFF_L = %#018lxul; /* %ld */\n", s, ul, ul);
#endif

#ifdef NUM_PTASEGS
    ul = NUM_PTASEGS;
    printf("%sNUM_PTASEGS = %#018lxul; /* %ld */\n", s, ul, ul);
#endif

#ifdef PTA_OFF_F
    ul = PTA_OFF_F;
    printf("%sPTA_OFF_F = %#018lxul; /* %ld */\n", s, ul, ul);
#endif

#ifdef PTA_OFF_L
    ul = PTA_OFF_L;
    printf("%sPTA_OFF_L = %#018lxul; /* %ld */\n", s, ul, ul);
#endif

#ifdef LDR_LIB_4K
    ul = LDR_LIB_4K;
    printf("%sLDR_LIB_4K = %#018lxul; /* %ld */\n", s, ul, ul);
#endif

#ifdef LDR_LIB_OFF_F
    ul = LDR_LIB_OFF_F;
    printf("%sLDR_LIB_OFF_F = %#018lxul; /* %ld */\n", s, ul, ul);
#endif

#ifdef LDR_LIB_4K_OFF_F
    ul = LDR_LIB_4K_OFF_F;
    printf("%sLDR_LIB_4K_OFF_F = %#018lxul; /* %ld */\n", s, ul, ul);
#endif

#ifdef LDR_SHLIB32_4K_OFF_F
    ul = LDR_SHLIB32_4K_OFF_F;
    printf("%sLDR_SHLIB32_4K_OFF_F = %#018lxul; /* %ld */\n", s, ul, ul);
#endif

#ifdef LDR_SHLIB32_4K_OFF_L
    ul = LDR_SHLIB32_4K_OFF_L;
    printf("%sLDR_SHLIB32_4K_OFF_L = %#018lxul; /* %ld */\n", s, ul, ul);
#endif

#ifdef LDR_SHLIB64_4K_OFF_F
    ul = LDR_SHLIB64_4K_OFF_F;
    printf("%sLDR_SHLIB64_4K_OFF_F = %#018lxul; /* %ld */\n", s, ul, ul);
#endif

#ifdef LDR_SHLIB64_4K_OFF_L
    ul = LDR_SHLIB64_4K_OFF_L;
    printf("%sLDR_SHLIB64_4K_OFF_L = %#018lxul; /* %ld */\n", s, ul, ul);
#endif

#ifdef LDR_LIB_4K_OFF_L
    ul = LDR_LIB_4K_OFF_L;
    printf("%sLDR_LIB_4K_OFF_L = %#018lxul; /* %ld */\n", s, ul, ul);
#endif

#ifdef LDR_LIB_OFF_L
    ul = LDR_LIB_OFF_L;
    printf("%sLDR_LIB_OFF_L = %#018lxul; /* %ld */\n", s, ul, ul);
#endif

#ifdef RMALLOC_OFF_F
    ul = RMALLOC_OFF_F;
    printf("%sRMALLOC_OFF_F = %#018lxul; /* %ld */\n", s, ul, ul);
#endif

#ifdef RMALLOC_OFF_L
    ul = RMALLOC_OFF_L;
    printf("%sRMALLOC_OFF_L = %#018lxul; /* %ld */\n", s, ul, ul);
#endif

#ifdef NUM_DRMGRSEGS
    ul = NUM_DRMGRSEGS;
    printf("%sNUM_DRMGRSEGS = %#018lxul; /* %ld */\n", s, ul, ul);
#endif

#ifdef DRMGR_OFF_F
    ul = DRMGR_OFF_F;
    printf("%sDRMGR_OFF_F = %#018lxul; /* %ld */\n", s, ul, ul);
#endif

#ifdef DRMGR_OFF_L
    ul = DRMGR_OFF_L;
    printf("%sDRMGR_OFF_L = %#018lxul; /* %ld */\n", s, ul, ul);
#endif

#ifdef NUM_ERMGRSEGS
    ul = NUM_ERMGRSEGS;
    printf("%sNUM_ERMGRSEGS = %#018lxul; /* %ld */\n", s, ul, ul);
#endif

#ifdef ERMGR_OFF_F
    ul = ERMGR_OFF_F;
    printf("%sERMGR_OFF_F = %#018lxul; /* %ld */\n", s, ul, ul);
#endif

#ifdef ERMGR_OFF_L
    ul = ERMGR_OFF_L;
    printf("%sERMGR_OFF_L = %#018lxul; /* %ld */\n", s, ul, ul);
#endif

#ifdef NUM_MSTSPACESEGS
    ul = NUM_MSTSPACESEGS;
    printf("%sNUM_MSTSPACESEGS = %#018lxul; /* %ld */\n", s, ul, ul);
#endif

#ifdef MSTSPACE_OFF_F
    ul = MSTSPACE_OFF_F;
    printf("%sMSTSPACE_OFF_F = %#018lxul; /* %ld */\n", s, ul, ul);
#endif

#ifdef MSTSPACE_OFF_L
    ul = MSTSPACE_OFF_L;
    printf("%sMSTSPACE_OFF_L = %#018lxul; /* %ld */\n", s, ul, ul);
#endif

#ifdef VMM_SPACEOK_4K_OFF
    ul = VMM_SPACEOK_4K_OFF;
    printf("%sVMM_SPACEOK_4K_OFF = %#018lxul; /* %ld */\n", s, ul, ul);
#endif

#ifdef VMM_4K_1TB_END
    ul = VMM_4K_1TB_END;
    printf("%sVMM_4K_1TB_END = %#018lxul; /* %ld */\n", s, ul, ul);
#endif

#ifdef DIR_GALLOC_4K_OFF_L
    ul = DIR_GALLOC_4K_OFF_L;
    printf("%sDIR_GALLOC_4K_OFF_L = %#018lxul; /* %ld */\n", s, ul, ul);
#endif

#ifdef GLOBKER_4K_OFF_L
    ul = GLOBKER_4K_OFF_L;
    printf("%sGLOBKER_4K_OFF_L = %#018lxul; /* %ld */\n", s, ul, ul);
#endif

#ifdef VMM_64K_PAD
    ul = VMM_64K_PAD;
    printf("%sVMM_64K_PAD = %#018lxul; /* %ld */\n", s, ul, ul);
#endif

#ifdef VMM_64K_1TB_BOUNDARY
    ul = VMM_64K_1TB_BOUNDARY;
    printf("%sVMM_64K_1TB_BOUNDARY = %#018lxul; /* %ld */\n", s, ul, ul);
#endif

#ifdef GLOBKER_64K_OFF_F
    ul = GLOBKER_64K_OFF_F;
    printf("%sGLOBKER_64K_OFF_F = %#018lxul; /* %ld */\n", s, ul, ul);
#endif

#ifdef GALLOC_64K_OFF_F
    ul = GALLOC_64K_OFF_F;
    printf("%sGALLOC_64K_OFF_F = %#018lxul; /* %ld */\n", s, ul, ul);
#endif

#ifdef GALLOC_64K_OFF_L
    ul = GALLOC_64K_OFF_L;
    printf("%sGALLOC_64K_OFF_L = %#018lxul; /* %ld */\n", s, ul, ul);
#endif

#ifdef DIR_GALLOC_64K_OFF_F
    ul = DIR_GALLOC_64K_OFF_F;
    printf("%sDIR_GALLOC_64K_OFF_F = %#018lxul; /* %ld */\n", s, ul, ul);
#endif

#ifdef KHEAP_64K_OFF_F
    ul = KHEAP_64K_OFF_F;
    printf("%sKHEAP_64K_OFF_F = %#018lxul; /* %ld */\n", s, ul, ul);
#endif

#ifdef KHEAP_64K_OFF_L
    ul = KHEAP_64K_OFF_L;
    printf("%sKHEAP_64K_OFF_L = %#018lxul; /* %ld */\n", s, ul, ul);
#endif

#ifdef MBUF_64K_OFF_F
    ul = MBUF_64K_OFF_F;
    printf("%sMBUF_64K_OFF_F = %#018lxul; /* %ld */\n", s, ul, ul);
#endif

#ifdef MBUF_64K_OFF_L
    ul = MBUF_64K_OFF_L;
    printf("%sMBUF_64K_OFF_L = %#018lxul; /* %ld */\n", s, ul, ul);
#endif

#ifdef VMM_OFF
    ul = VMM_OFF;
    printf("%sVMM_OFF = %#018lxul; /* %ld */\n", s, ul, ul);
#endif

#ifdef VMM_OFF_F
    ul = VMM_OFF_F;
    printf("%sVMM_OFF_F = %#018lxul; /* %ld */\n", s, ul, ul);
#endif

#ifdef VMM_OFF_L
    ul = VMM_OFF_L;
    printf("%sVMM_OFF_L = %#018lxul; /* %ld */\n", s, ul, ul);
#endif

#ifdef NUM_DMAPSIDS
    ul = NUM_DMAPSIDS;
    printf("%sNUM_DMAPSIDS = %#018lxul; /* %ld */\n", s, ul, ul);
#endif

#ifdef DMAP_OFF_F
    ul = DMAP_OFF_F;
    printf("%sDMAP_OFF_F = %#018lxul; /* %ld */\n", s, ul, ul);
#endif

#ifdef DMAP_OFF_L
    ul = DMAP_OFF_L;
    printf("%sDMAP_OFF_L = %#018lxul; /* %ld */\n", s, ul, ul);
#endif

#ifdef NUM_AMESEGS
    ul = NUM_AMESEGS;
    printf("%sNUM_AMESEGS = %#018lxul; /* %ld */\n", s, ul, ul);
#endif

#ifdef AME_OFF_F
    ul = AME_OFF_F;
    printf("%sAME_OFF_F = %#018lxul; /* %ld */\n", s, ul, ul);
#endif

#ifdef AME_OFF_L
    ul = AME_OFF_L;
    printf("%sAME_OFF_L = %#018lxul; /* %ld */\n", s, ul, ul);
#endif

#ifdef NUM_VMINTSIDS
    ul = NUM_VMINTSIDS;
    printf("%sNUM_VMINTSIDS = %#018lxul; /* %ld */\n", s, ul, ul);
#endif

#ifdef VMINT_OFF_F
    ul = VMINT_OFF_F;
    printf("%sVMINT_OFF_F = %#018lxul; /* %ld */\n", s, ul, ul);
#endif

#ifdef VMINT_OFF_L
    ul = VMINT_OFF_L;
    printf("%sVMINT_OFF_L = %#018lxul; /* %ld */\n", s, ul, ul);
#endif

#ifdef NUM_FFBMSEGS
    ul = NUM_FFBMSEGS;
    printf("%sNUM_FFBMSEGS = %#018lxul; /* %ld */\n", s, ul, ul);
#endif

#ifdef FF_BITMAP_OFF_F
    ul = FF_BITMAP_OFF_F;
    printf("%sFF_BITMAP_OFF_F = %#018lxul; /* %ld */\n", s, ul, ul);
#endif

#ifdef FF_BITMAP_OFF_L
    ul = FF_BITMAP_OFF_L;
    printf("%sFF_BITMAP_OFF_L = %#018lxul; /* %ld */\n", s, ul, ul);
#endif

#ifdef LDR_LIB_64K
    ul = LDR_LIB_64K;
    printf("%sLDR_LIB_64K = %#018lxul; /* %ld */\n", s, ul, ul);
#endif

#ifdef LDR_LIB_64K_OFF_F
    ul = LDR_LIB_64K_OFF_F;
    printf("%sLDR_LIB_64K_OFF_F = %#018lxul; /* %ld */\n", s, ul, ul);
#endif

#ifdef LDR_SHLIB32_64K_OFF_F
    ul = LDR_SHLIB32_64K_OFF_F;
    printf("%sLDR_SHLIB32_64K_OFF_F = %#018lxul; /* %ld */\n", s, ul, ul);
#endif

#ifdef LDR_SHLIB32_64K_OFF_L
    ul = LDR_SHLIB32_64K_OFF_L;
    printf("%sLDR_SHLIB32_64K_OFF_L = %#018lxul; /* %ld */\n", s, ul, ul);
#endif

#ifdef LDR_SHLIB64_64K_OFF_F
    ul = LDR_SHLIB64_64K_OFF_F;
    printf("%sLDR_SHLIB64_64K_OFF_F = %#018lxul; /* %ld */\n", s, ul, ul);
#endif

#ifdef LDR_SHLIB64_64K_OFF_L
    ul = LDR_SHLIB64_64K_OFF_L;
    printf("%sLDR_SHLIB64_64K_OFF_L = %#018lxul; /* %ld */\n", s, ul, ul);
#endif

#ifdef LDR_LIB_64K_OFF_L
    ul = LDR_LIB_64K_OFF_L;
    printf("%sLDR_LIB_64K_OFF_L = %#018lxul; /* %ld */\n", s, ul, ul);
#endif

#ifdef NUM_PROCTTSIDS
    ul = NUM_PROCTTSIDS;
    printf("%sNUM_PROCTTSIDS = %#018lxul; /* %ld */\n", s, ul, ul);
#endif

#ifdef PROCTT_OFF_F
    ul = PROCTT_OFF_F;
    printf("%sPROCTT_OFF_F = %#018lxul; /* %ld */\n", s, ul, ul);
#endif

#ifdef PROCTT_OFF_L
    ul = PROCTT_OFF_L;
    printf("%sPROCTT_OFF_L = %#018lxul; /* %ld */\n", s, ul, ul);
#endif

#ifdef XMDBG_RECS_OFF_F
    ul = XMDBG_RECS_OFF_F;
    printf("%sXMDBG_RECS_OFF_F = %#018lxul; /* %ld */\n", s, ul, ul);
#endif

#ifdef XMDBG_RECS_OFF_L
    ul = XMDBG_RECS_OFF_L;
    printf("%sXMDBG_RECS_OFF_L = %#018lxul; /* %ld */\n", s, ul, ul);
#endif

#ifdef MPDATA_OFF_F
    ul = MPDATA_OFF_F;
    printf("%sMPDATA_OFF_F = %#018lxul; /* %ld */\n", s, ul, ul);
#endif

#ifdef MPDATA_OFF_L
    ul = MPDATA_OFF_L;
    printf("%sMPDATA_OFF_L = %#018lxul; /* %ld */\n", s, ul, ul);
#endif

#ifdef LOCK_INSTR_OFF_F
    ul = LOCK_INSTR_OFF_F;
    printf("%sLOCK_INSTR_OFF_F = %#018lxul; /* %ld */\n", s, ul, ul);
#endif

#ifdef LOCK_INSTR_OFF_L
    ul = LOCK_INSTR_OFF_L;
    printf("%sLOCK_INSTR_OFF_L = %#018lxul; /* %ld */\n", s, ul, ul);
#endif

#ifdef JFS_SEG_OFF_F
    ul = JFS_SEG_OFF_F;
    printf("%sJFS_SEG_OFF_F = %#018lxul; /* %ld */\n", s, ul, ul);
#endif

#ifdef JFS_SEG_OFF_L
    ul = JFS_SEG_OFF_L;
    printf("%sJFS_SEG_OFF_L = %#018lxul; /* %ld */\n", s, ul, ul);
#endif

#ifdef LFS_SEG_OFF_F
    ul = LFS_SEG_OFF_F;
    printf("%sLFS_SEG_OFF_F = %#018lxul; /* %ld */\n", s, ul, ul);
#endif

#ifdef LFS_SEG_OFF_L
    ul = LFS_SEG_OFF_L;
    printf("%sLFS_SEG_OFF_L = %#018lxul; /* %ld */\n", s, ul, ul);
#endif

#ifdef VMM_SCBSZ
    ul = VMM_SCBSZ;
    printf("%sVMM_SCBSZ = %#018lxul; /* %ld */\n", s, ul, ul);
#endif

#ifdef NUM_1TB_SCBSIDS
    ul = NUM_1TB_SCBSIDS;
    printf("%sNUM_1TB_SCBSIDS = %#018lxul; /* %ld */\n", s, ul, ul);
#endif

#ifdef VMMSCB_OFF_F
    ul = VMMSCB_OFF_F;
    printf("%sVMMSCB_OFF_F = %#018lxul; /* %ld */\n", s, ul, ul);
#endif

#ifdef VMMSCB_1TB_OFF_F
    ul = VMMSCB_1TB_OFF_F;
    printf("%sVMMSCB_1TB_OFF_F = %#018lxul; /* %ld */\n", s, ul, ul);
#endif

#ifdef VMMSCB_1TB_OFF_L
    ul = VMMSCB_1TB_OFF_L;
    printf("%sVMMSCB_1TB_OFF_L = %#018lxul; /* %ld */\n", s, ul, ul);
#endif

#ifdef VMMSCB_REG_OFF
    ul = VMMSCB_REG_OFF;
    printf("%sVMMSCB_REG_OFF = %#018lxul; /* %ld */\n", s, ul, ul);
#endif

#ifdef VMMSCB_OFF_L
    ul = VMMSCB_OFF_L;
    printf("%sVMMSCB_OFF_L = %#018lxul; /* %ld */\n", s, ul, ul);
#endif

#ifdef VMM_PVL_OFF
    ul = VMM_PVL_OFF;
    printf("%sVMM_PVL_OFF = %#018lxul; /* %ld */\n", s, ul, ul);
#endif

#ifdef VMM_SPACEOK_64K_OFF
    ul = VMM_SPACEOK_64K_OFF;
    printf("%sVMM_SPACEOK_64K_OFF = %#018lxul; /* %ld */\n", s, ul, ul);
#endif

#ifdef VMM_DLVA_OFF
    ul = VMM_DLVA_OFF;
    printf("%sVMM_DLVA_OFF = %#018lxul; /* %ld */\n", s, ul, ul);
#endif

#ifdef VMM_CMEM_SZ
    ul = VMM_CMEM_SZ;
    printf("%sVMM_CMEM_SZ = %#018lxul; /* %ld */\n", s, ul, ul);
#endif

#ifdef NUM_CMEM_SEGS
    ul = NUM_CMEM_SEGS;
    printf("%sNUM_CMEM_SEGS = %#018lxul; /* %ld */\n", s, ul, ul);
#endif

#ifdef VMM_CMEM_OFF_F
    ul = VMM_CMEM_OFF_F;
    printf("%sVMM_CMEM_OFF_F = %#018lxul; /* %ld */\n", s, ul, ul);
#endif

#ifdef VMM_CMEM_OFF_L
    ul = VMM_CMEM_OFF_L;
    printf("%sVMM_CMEM_OFF_L = %#018lxul; /* %ld */\n", s, ul, ul);
#endif

#ifdef VMM_CBLKS_SZ
    ul = VMM_CBLKS_SZ;
    printf("%sVMM_CBLKS_SZ = %#018lxul; /* %ld */\n", s, ul, ul);
#endif

#ifdef NUM_CBLKS_SEGS
    ul = NUM_CBLKS_SEGS;
    printf("%sNUM_CBLKS_SEGS = %#018lxul; /* %ld */\n", s, ul, ul);
#endif

#ifdef VMM_CBLKS_OFF_F
    ul = VMM_CBLKS_OFF_F;
    printf("%sVMM_CBLKS_OFF_F = %#018lxul; /* %ld */\n", s, ul, ul);
#endif

#ifdef VMM_CBLKS_OFF_L
    ul = VMM_CBLKS_OFF_L;
    printf("%sVMM_CBLKS_OFF_L = %#018lxul; /* %ld */\n", s, ul, ul);
#endif

#ifdef VMM_64K_1TB_END
    ul = VMM_64K_1TB_END;
    printf("%sVMM_64K_1TB_END = %#018lxul; /* %ld */\n", s, ul, ul);
#endif

#ifdef DIR_GALLOC_64K_OFF_L
    ul = DIR_GALLOC_64K_OFF_L;
    printf("%sDIR_GALLOC_64K_OFF_L = %#018lxul; /* %ld */\n", s, ul, ul);
#endif

#ifdef GLOBKER_64K_OFF_L
    ul = GLOBKER_64K_OFF_L;
    printf("%sGLOBKER_64K_OFF_L = %#018lxul; /* %ld */\n", s, ul, ul);
#endif

#ifdef VMM_16M_PAD
    ul = VMM_16M_PAD;
    printf("%sVMM_16M_PAD = %#018lxul; /* %ld */\n", s, ul, ul);
#endif

#ifdef VMM_16M_1TB_BOUNDARY
    ul = VMM_16M_1TB_BOUNDARY;
    printf("%sVMM_16M_1TB_BOUNDARY = %#018lxul; /* %ld */\n", s, ul, ul);
#endif

#ifdef GLOBKER_16M_OFF_F
    ul = GLOBKER_16M_OFF_F;
    printf("%sGLOBKER_16M_OFF_F = %#018lxul; /* %ld */\n", s, ul, ul);
#endif

#ifdef GALLOC_16M_OFF_F
    ul = GALLOC_16M_OFF_F;
    printf("%sGALLOC_16M_OFF_F = %#018lxul; /* %ld */\n", s, ul, ul);
#endif

#ifdef GALLOC_16M_OFF_L
    ul = GALLOC_16M_OFF_L;
    printf("%sGALLOC_16M_OFF_L = %#018lxul; /* %ld */\n", s, ul, ul);
#endif

#ifdef DIR_GALLOC_16M_OFF_F
    ul = DIR_GALLOC_16M_OFF_F;
    printf("%sDIR_GALLOC_16M_OFF_F = %#018lxul; /* %ld */\n", s, ul, ul);
#endif

#ifdef KHEAP_16M_OFF_F
    ul = KHEAP_16M_OFF_F;
    printf("%sKHEAP_16M_OFF_F = %#018lxul; /* %ld */\n", s, ul, ul);
#endif

#ifdef KHEAP_16M_OFF_L
    ul = KHEAP_16M_OFF_L;
    printf("%sKHEAP_16M_OFF_L = %#018lxul; /* %ld */\n", s, ul, ul);
#endif

#ifdef MBUF_16M_OFF_F
    ul = MBUF_16M_OFF_F;
    printf("%sMBUF_16M_OFF_F = %#018lxul; /* %ld */\n", s, ul, ul);
#endif

#ifdef MBUF_16M_OFF_L
    ul = MBUF_16M_OFF_L;
    printf("%sMBUF_16M_OFF_L = %#018lxul; /* %ld */\n", s, ul, ul);
#endif

#ifdef DIR_GALLOC_16M_OFF_L
    ul = DIR_GALLOC_16M_OFF_L;
    printf("%sDIR_GALLOC_16M_OFF_L = %#018lxul; /* %ld */\n", s, ul, ul);
#endif

#ifdef GLOBKER_16M_OFF_L
    ul = GLOBKER_16M_OFF_L;
    printf("%sGLOBKER_16M_OFF_L = %#018lxul; /* %ld */\n", s, ul, ul);
#endif

#ifdef GLOBKER_1TB_OFF_L
    ul = GLOBKER_1TB_OFF_L;
    printf("%sGLOBKER_1TB_OFF_L = %#018lxul; /* %ld */\n", s, ul, ul);
#endif

#ifdef GALLOC_OFF_L
    ul = GALLOC_OFF_L;
    printf("%sGALLOC_OFF_L = %#018lxul; /* %ld */\n", s, ul, ul);
#endif

#ifdef NONGK_1TB_OFF_F
    ul = NONGK_1TB_OFF_F;
    printf("%sNONGK_1TB_OFF_F = %#018lxul; /* %ld */\n", s, ul, ul);
#endif

#ifdef NONGK_1TB_OFF_L
    ul = NONGK_1TB_OFF_L;
    printf("%sNONGK_1TB_OFF_L = %#018lxul; /* %ld */\n", s, ul, ul);
#endif

#ifdef SCBREG_OFF_F
    ul = SCBREG_OFF_F;
    printf("%sSCBREG_OFF_F = %#018lxul; /* %ld */\n", s, ul, ul);
#endif

#ifdef SCBREG_OFF_L
    ul = SCBREG_OFF_L;
    printf("%sSCBREG_OFF_L = %#018lxul; /* %ld */\n", s, ul, ul);
#endif

#ifdef VSID_1TB_SHIFT
    ul = VSID_1TB_SHIFT;
    printf("%sVSID_1TB_SHIFT = %#018lxul; /* %ld */\n", s, ul, ul);
#endif
    
    ul = RMAP_UNKNOWN;
    printf("%sRMAP_UNKNOWN = %#018lxul; /* %ld */\n", s, ul, ul);
    
    ul = RMAP_SWPFT;
    printf("%sRMAP_SWPFT = %#018lxul; /* %ld */\n", s, ul, ul);
    
    ul = RMAP_PVT;
    printf("%sRMAP_PVT = %#018lxul; /* %ld */\n", s, ul, ul);
    
    ul = RMAP_PVLIST;
    printf("%sRMAP_PVLIST = %#018lxul; /* %ld */\n", s, ul, ul);
    
    ul = RMAP_PFT;
    printf("%sRMAP_PFT = %#018lxul; /* %ld */\n", s, ul, ul);
    
    ul = RMAP_SWHAT;
    printf("%sRMAP_SWHAT = %#018lxul; /* %ld */\n", s, ul, ul);
    
    ul = RMAP_AHAT;
    printf("%sRMAP_AHAT = %#018lxul; /* %ld */\n", s, ul, ul);
    
    ul = RMAP_FIXED_LAST;
    printf("%sRMAP_FIXED_LAST = %#018lxul; /* %ld */\n", s, ul, ul);
    
    ul = RMAP_KERN;
    printf("%sRMAP_KERN = %#018lxul; /* %ld */\n", s, ul, ul);
    
    ul = RMAP_IPLCB;
    printf("%sRMAP_IPLCB = %#018lxul; /* %ld */\n", s, ul, ul);
    
    ul = RMAP_MSTSPACE;
    printf("%sRMAP_MSTSPACE = %#018lxul; /* %ld */\n", s, ul, ul);
    
    ul = RMAP_RAMD;
    printf("%sRMAP_RAMD = %#018lxul; /* %ld */\n", s, ul, ul);
    
    ul = RMAP_BCFG;
    printf("%sRMAP_BCFG = %#018lxul; /* %ld */\n", s, ul, ul);
    
    ul = RMAP_RPT;
    printf("%sRMAP_RPT = %#018lxul; /* %ld */\n", s, ul, ul);
    
    ul = RMAP_RPHAT;
    printf("%sRMAP_RPHAT = %#018lxul; /* %ld */\n", s, ul, ul);
    
    ul = RMAP_PDT;
    printf("%sRMAP_PDT = %#018lxul; /* %ld */\n", s, ul, ul);
    
    ul = RMAP_PTAR;
    printf("%sRMAP_PTAR = %#018lxul; /* %ld */\n", s, ul, ul);
    
    ul = RMAP_PTAD;
    printf("%sRMAP_PTAD = %#018lxul; /* %ld */\n", s, ul, ul);
    
    ul = RMAP_PTAI;
    printf("%sRMAP_PTAI = %#018lxul; /* %ld */\n", s, ul, ul);
    
    ul = RMAP_DMAP;
    printf("%sRMAP_DMAP = %#018lxul; /* %ld */\n", s, ul, ul);
    
    ul = RMAP_BACKT;
    printf("%sRMAP_BACKT = %#018lxul; /* %ld */\n", s, ul, ul);
    
    ul = RMAP_MEMP_FRS;
    printf("%sRMAP_MEMP_FRS = %#018lxul; /* %ld */\n", s, ul, ul);
    
    ul = RMAP_PAL_STOP_SELF;
    printf("%sRMAP_PAL_STOP_SELF = %#018lxul; /* %ld */\n", s, ul, ul);
    
    ul = RMAP_KDB_STACK;
    printf("%sRMAP_KDB_STACK = %#018lxul; /* %ld */\n", s, ul, ul);
    
    ul = RMAP_FIXLMB;
    printf("%sRMAP_FIXLMB = %#018lxul; /* %ld */\n", s, ul, ul);
    
    ul = RMAP_SCB;
    printf("%sRMAP_SCB = %#018lxul; /* %ld */\n", s, ul, ul);
    
    ul = RMAP_RMALLOC;
    printf("%sRMAP_RMALLOC = %#018lxul; /* %ld */\n", s, ul, ul);
    
    ul = RMAP_DRMGR;
    printf("%sRMAP_DRMGR = %#018lxul; /* %ld */\n", s, ul, ul);
    
    ul = RMAP_VMINT;
    printf("%sRMAP_VMINT = %#018lxul; /* %ld */\n", s, ul, ul);
    
    ul = RMAP_KERN_HI;
    printf("%sRMAP_KERN_HI = %#018lxul; /* %ld */\n", s, ul, ul);
    
    ul = RMAP_SLB_TRACE;
    printf("%sRMAP_SLB_TRACE = %#018lxul; /* %ld */\n", s, ul, ul);
    
    ul = RMAP_LGPGKSEG;
    printf("%sRMAP_LGPGKSEG = %#018lxul; /* %ld */\n", s, ul, ul);
    
    ul = RMAP_FF_BITMAP;
    printf("%sRMAP_FF_BITMAP = %#018lxul; /* %ld */\n", s, ul, ul);
    
    ul = RMAP_FWAD_RMRSA;
    printf("%sRMAP_FWAD_RMRSA = %#018lxul; /* %ld */\n", s, ul, ul);
    
    ul = RMAP_FWAD_DLVA;
    printf("%sRMAP_FWAD_DLVA = %#018lxul; /* %ld */\n", s, ul, ul);
    
    ul = RMAP_FWAD_HRSA;
    printf("%sRMAP_FWAD_HRSA = %#018lxul; /* %ld */\n", s, ul, ul);
    
    ul = RMAP_KPATCH;
    printf("%sRMAP_KPATCH = %#018lxul; /* %ld */\n", s, ul, ul);
    
    ul = RMAP_PPDA;
    printf("%sRMAP_PPDA = %#018lxul; /* %ld */\n", s, ul, ul);
    
    ul = RMAP_VAR_LAST;
    printf("%sRMAP_VAR_LAST = %#018lxul; /* %ld */\n", s, ul, ul);
    
#ifdef L2MAXVADDR
    ul = L2MAXVADDR;
    printf("%sL2MAXVADDR = %#018lxul; /* %ld */\n", s, ul, ul);
#endif
    
#ifdef L2MAXVADDR_IMPL
    ul = L2MAXVADDR_IMPL;
    printf("%sL2MAXVADDR_IMPL = %#018lxul; /* %ld */\n", s, ul, ul);
#endif
    
#ifdef VM_L2_MAXHW_VSID
    ul = VM_L2_MAXHW_VSID;
    printf("%sVM_L2_MAXHW_VSID = %#018lxul; /* %ld */\n", s, ul, ul);
#endif
    
#ifdef VM_L2_MAXARCH_VSID_1TB
    ul = VM_L2_MAXARCH_VSID_1TB;
    printf("%sVM_L2_MAXARCH_VSID_1TB = %#018lxul; /* %ld */\n", s, ul, ul);
#endif
    
#ifdef VM_L2_MAXARCH_VSID
    ul = VM_L2_MAXARCH_VSID;
    printf("%sVM_L2_MAXARCH_VSID = %#018lxul; /* %ld */\n", s, ul, ul);
#endif
    
#ifdef L2SIDLIMIT_1TB
    ul = L2SIDLIMIT_1TB;
    printf("%sL2SIDLIMIT_1TB = %#018lxul; /* %ld */\n", s, ul, ul);
#endif
    
#ifdef L2SIDLIMIT
    ul = L2SIDLIMIT;
    printf("%sL2SIDLIMIT = %#018lxul; /* %ld */\n", s, ul, ul);
#endif
    
#ifdef SIDMAX_1TB
    ul = SIDMAX_1TB;
    printf("%sSIDMAX_1TB = %#018lxul; /* %ld */\n", s, ul, ul);
#endif
    
#ifdef SIDMAX
    ul = SIDMAX;
    printf("%sSIDMAX = %#018lxul; /* %ld */\n", s, ul, ul);
#endif
    
#ifdef NUMSIDS_1TB
    ul = NUMSIDS_1TB;
    printf("%sNUMSIDS_1TB = %#018lxul; /* %ld */\n", s, ul, ul);
#endif
    
#ifdef SIDLIMIT_1TB
    ul = SIDLIMIT_1TB;
    printf("%sSIDLIMIT_1TB = %#018lxul; /* %ld */\n", s, ul, ul);
#endif
    
#ifdef VM_MAXARCH_VSID
    ul = VM_MAXARCH_VSID;
    printf("%sVM_MAXARCH_VSID = %#018lxul; /* %ld */\n", s, ul, ul);
#endif
    
#ifdef VM_L2_IOSID_BIT
    ul = VM_L2_IOSID_BIT;
    printf("%sVM_L2_IOSID_BIT = %#018lxul; /* %ld */\n", s, ul, ul);
#endif
    
#ifdef KSP_SID_SHIFT
    ul = KSP_SID_SHIFT;
    printf("%sKSP_SID_SHIFT = %#018lxul; /* %ld */\n", s, ul, ul);
#endif

#ifdef KSP_SIDHASH_INC
    ul = KSP_SIDHASH_INC;
    printf("%sKSP_SIDHASH_INC = %#018lxul; /* %ld */\n", s, ul, ul);
#endif

#ifdef KSP_1TB_REG_SID_SHIFT
    ul = KSP_1TB_REG_SID_SHIFT;
    printf("%sKSP_1TB_REG_SID_SHIFT = %#018lxul; /* %ld */\n", s, ul, ul);
#endif

#ifdef KSP_1TB_REG_SID_INC
    ul = KSP_1TB_REG_SID_INC;
    printf("%sKSP_1TB_REG_SID_INC = %#018lxul; /* %ld */\n", s, ul, ul);
#endif

#ifdef KSP_1TB_SUPER_SID_SHIFT
    ul = KSP_1TB_SUPER_SID_SHIFT;
    printf("%sKSP_1TB_SUPER_SID_SHIFT = %#018lxul; /* %ld */\n", s, ul, ul);
#endif

#ifdef KSP_1TB_SUPER_SID_INC
    ul = KSP_1TB_SUPER_SID_INC;
    printf("%sKSP_1TB_SUPER_SID_INC = %#018lxul; /* %ld */\n", s, ul, ul);
#endif

#ifdef KSP_BIT
    ul = KSP_BIT;
    printf("%sKSP_BIT = %#018lxul; /* %ld */\n", s, ul, ul);
#endif

#ifdef KSP_REGION_BIT
    ul = KSP_REGION_BIT;
    printf("%sKSP_REGION_BIT = %#018lxul; /* %ld */\n", s, ul, ul);
#endif

#ifdef KSP_BIT_1TB
    ul = KSP_BIT_1TB;
    printf("%sKSP_BIT_1TB = %#018lxul; /* %ld */\n", s, ul, ul);
#endif

#ifdef KSP_REGION_BIT_1TB
    ul = KSP_REGION_BIT_1TB;
    printf("%sKSP_REGION_BIT_1TB = %#018lxul; /* %ld */\n", s, ul, ul);
#endif

#ifdef KSP_HI_REGION_BIT_1TB
    ul = KSP_HI_REGION_BIT_1TB;
    printf("%sKSP_HI_REGION_BIT_1TB = %#018lxul; /* %ld */\n", s, ul, ul);
#endif

#ifdef KSP_MAX_VSID
    ul = KSP_MAX_VSID;
    printf("%sKSP_MAX_VSID = %#018lxul; /* %ld */\n", s, ul, ul);
#endif

#ifdef KSP_SID_MAGIC
    ul = KSP_SID_MAGIC;
    printf("%sKSP_SID_MAGIC = %#018lxul; /* %ld */\n", s, ul, ul);
#endif

#ifdef KSP_FIRST_SID
    ul = KSP_FIRST_SID;
    printf("%sKSP_FIRST_SID = %#018lxul; /* %ld */\n", s, ul, ul);
#endif

#ifdef KSP_FIRST_SID_1TB
    ul = KSP_FIRST_SID_1TB;
    printf("%sKSP_FIRST_SID_1TB = %#018lxul; /* %ld */\n", s, ul, ul);
#endif

#ifdef KSP_SID_BASE_256MB
    ul = KSP_SID_BASE_256MB;
    printf("%sKSP_SID_BASE_256MB = %#018lxul; /* %ld */\n", s, ul, ul);
#endif

#ifdef KSP_SID_BASE_1TB
    ul = KSP_SID_BASE_1TB;
    printf("%sKSP_SID_BASE_1TB = %#018lxul; /* %ld */\n", s, ul, ul);
#endif

#ifdef KSP_REGION_INC
    ul = KSP_REGION_INC;
    printf("%sKSP_REGION_INC = %#018lxul; /* %ld */\n", s, ul, ul);
#endif

#ifdef KSP_REGION_ESID_SHIFT
    ul = KSP_REGION_ESID_SHIFT;
    printf("%sKSP_REGION_ESID_SHIFT = %#018lxul; /* %ld */\n", s, ul, ul);
#endif

#ifdef KSP_REGION_ESID_INC_1TB
    ul = KSP_REGION_ESID_INC_1TB;
    printf("%sKSP_REGION_ESID_INC_1TB = %#018lxul; /* %ld */\n", s, ul, ul);
#endif

#ifdef KSP_REGION_ESID_INC
    ul = KSP_REGION_ESID_INC;
    printf("%sKSP_REGION_ESID_INC = %#018lxul; /* %ld */\n", s, ul, ul);
#endif

#ifdef KSP_ESID_ANCHOR
    ul = KSP_ESID_ANCHOR;
    printf("%sKSP_ESID_ANCHOR = %#018lxul; /* %ld */\n", s, ul, ul);
#endif

#ifdef KSP_ESID_BASE_256MB
    ul = KSP_ESID_BASE_256MB;
    printf("%sKSP_ESID_BASE_256MB = %#018lxul; /* %ld */\n", s, ul, ul);
#endif

#ifdef KSP_ESID_BASE_MULTI_1TB
    ul = KSP_ESID_BASE_MULTI_1TB;
    printf("%sKSP_ESID_BASE_MULTI_1TB = %#018lxul; /* %ld */\n", s, ul, ul);
#endif

#ifdef KSP_NUM_REGIONS
    ul = KSP_NUM_REGIONS;
    printf("%sKSP_NUM_REGIONS = %#018lxul; /* %ld */\n", s, ul, ul);
#endif

#ifdef KSP_ESID_END_256MB
    ul = KSP_ESID_END_256MB;
    printf("%sKSP_ESID_END_256MB = %#018lxul; /* %ld */\n", s, ul, ul);
#endif

#ifdef KSP_SID_END
    ul = KSP_SID_END;
    printf("%sKSP_SID_END = %#018lxul; /* %ld */\n", s, ul, ul);
#endif

#ifdef KSP_EXCLUSIVE_SID
    ul = KSP_EXCLUSIVE_SID;
    printf("%sKSP_EXCLUSIVE_SID = %#018lxul; /* %ld */\n", s, ul, ul);
#endif

#ifdef KSP_EXCLUSIVE_SID_1TB
    ul = KSP_EXCLUSIVE_SID_1TB;
    printf("%sKSP_EXCLUSIVE_SID_1TB = %#018lxul; /* %ld */\n", s, ul, ul);
#endif

#ifdef INIT_KSP_EADDR
    ul = INIT_KSP_EADDR;
    printf("%sINIT_KSP_EADDR = %#018lxul; /* %ld */\n", s, ul, ul);
#endif

#ifdef KSP_MIRRORBITS
    ul = KSP_MIRRORBITS;
    printf("%sKSP_MIRRORBITS = %#018lxul; /* %ld */\n", s, ul, ul);
#endif

#ifdef KSP_REGULAR_ESID_1TB
    ul = KSP_REGULAR_ESID_1TB;
    printf("%sKSP_REGULAR_ESID_1TB = %#018lxul; /* %ld */\n", s, ul, ul);
#endif

#ifdef KSP_SUPER_ESID_1TB
    ul = KSP_SUPER_ESID_1TB;
    printf("%sKSP_SUPER_ESID_1TB = %#018lxul; /* %ld */\n", s, ul, ul);
#endif

#ifdef KSP_SUPER_BITS_256MB
    ul = KSP_SUPER_BITS_256MB;
    printf("%sKSP_SUPER_BITS_256MB = %#018lxul; /* %ld */\n", s, ul, ul);
#endif

#ifdef KSP_REGULAR_MULTI_1TB_MASK
    ul = KSP_REGULAR_MULTI_1TB_MASK;
    printf("%sKSP_REGULAR_MULTI_1TB_MASK = %#018lxul; /* %ld */\n", s, ul, ul);
#endif

#ifdef KSP_SUPER_MULTI_1TB_MASK
    ul = KSP_SUPER_MULTI_1TB_MASK;
    printf("%sKSP_SUPER_MULTI_1TB_MASK = %#018lxul; /* %ld */\n", s, ul, ul);
#endif

#ifdef KSP_SIDX_BASE
    ul = KSP_SIDX_BASE;
    printf("%sKSP_SIDX_BASE = %#018lxul; /* %ld */\n", s, ul, ul);
#endif

    return 0;
}

int main(int argc, char *argv[])
{
    print_it(1);
    printf("if (multi_1tb_ksp_segs == 0) {\n");
    print_it(0);
    printf("}\n");
    return 0;
}
