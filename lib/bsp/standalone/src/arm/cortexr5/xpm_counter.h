/******************************************************************************
* Copyright (c) 2014 - 2022 Xilinx, Inc.  All rights reserved.
* Copyright (c) 2022 - 2024 Advanced Micro Devices, Inc. All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/

/*****************************************************************************/
/**
*
* @file xpm_counter.h
*
* @addtogroup r5_event_counter_apis Cortex R5 Event Counters Functions
*
* Cortex R5 event counter functions can be utilized to configure and control
* the Cortex-R5 performance monitor events.
* Cortex-R5 Performance Monitor has 3 event counters which can be used to
* count a variety of events described in Coretx-R5 TRM. The xpm_counter.h file
* defines configurations XPM_CNTRCFGx which can be used to program the event
* counters to count a set of events.
*
*
* @{
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who  Date     Changes
* ----- ---- -------- -----------------------------------------------
* 5.00  pkp  02/10/14 Initial version
* 7.1   aru  04/15/19 Updated the events correctly
* 7.2   mus  01/29/20 Added new macro Xpm_ReadCycleCounterVal, to
*                     read PMU cycle counter value
* 7.2   asa  03/18/20 Add prototypes for new APIs. Deprecate older
*                     APIs. Add new macros being used in the new APIs.
* 7.7	sk   01/10/22 Modify Xpm_GetEventCounter and Xpm_DisableEvent
* 		      functions arguments to fix misra_c_2012_rule_8_3
* 		      violation.
* 8.0	sk   03/02/22 Add endif at the end of the file to fix misra_c_2012
* 		      _directive_4_10 violation.
* 8.2   asa  02/24/23 Add macros for R52.
* 9.2   mus  09/06/24 CortexR52 integrated in VersalNet has 4 event counters,
*                     where-as RPU in ZynqMP/Versal has 3 event
*                     counters, updated macro's related to event counter
*                     related masks accordingly.
* </pre>
*
******************************************************************************/

/**
 *@cond nocomments
 */

#ifndef XPMCOUNTER_H /* prevent circular inclusions */
#define XPMCOUNTER_H /* by using protection macros */

/***************************** Include Files ********************************/

#include <stdint.h>
#include "xpseudo_asm.h"
#include "xil_types.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/************************** Constant Definitions ****************************/
#if defined (ARMR5) && !defined (ARMR52)
#define VERSAL_ZYNQMP_R5
#endif

/* Number of performance counters */
#if defined (ARMR52)
#define XPM_CTRCOUNT 4U
#else
#define XPM_CTRCOUNT 3U
#endif

/* The following constants define the Cortex-R5 Performance Monitor Events */

/*
 * Software increment. The register is incremented only on writes to the
 * Software Increment Register
 */
#define XPM_EVENT_SOFTINCR 0x00U

/*
 * Instruction cache miss
 * Each Instruction fetch from normal cacheable memory that causes a refill from
 * the level 2 memory system generates this event.Accesses that do not cause a new
 * cache refill, but are satisfied from refilling data of a previous miss are not
 * counted. Where instruction fetches consists of multiple instructions, these
 * accesses count as single events.
 */
#define XPM_EVENT_INSTRCACHEMISS 0x01

/*
 * Data cache miss
 * Each data read from or write to normal cacheable memory that causes a refill
 * from the level 2 memory system generates this event.Accesses that do not cause
 * a new cache refill, but are satisfied from refilling data of a previous miss are
 * not counted. Each access to a cache line to normal cacheable memory that causes
 * a new linefill is counted,including the multiple transactions of an LDM and STM
 */

#define XPM_EVENT_DATACACHEMISS 0x03

/*
 * Each access to a cache line is counted including the multiple transactions of
 * an LDM, STM, or other operations.
 */

#define XPM_EVENT_DATACACHEACCESS 0x04

/*
 *Data read architecturally executed
 *This evevt occurs for every every instruction that explicitly reads data,
 *including SWP.
 */

#define XPM_EVENT_DATAREAD 0x06

/*
 *Data write architecturally executed
 *This evevt occurs for every every instruction that explicitly writes data,
 *including SWP.
 */

#define XPM_EVENT_DATAWRITE 0x07

/*
 *Instruction architecturally executed
 */

#define XPM_EVENT_INSTR 0x08

/*
 * Exception taken
 * This event occurs on each exception taken
 */

#define XPM_EVENT_EXCEPTION 0x09

/*
 * Exception return architecturally executed.
 * This event occurs on every exception return
 * for example:
 * RFE,MOVS PC,LDM Rn, {..,PC}^
 */

#define XPM_EVENT_EXCEPTIONRET 0x0A

/*
 * Dual-issued pair of instructions architecturally executed
 */

#define XPM_EVENT_DUALINSTR 0x5E




/*
 * Change to Context ID Executed
 */

#define XPM_EVENT_CHANGETOCONTEXID 0x0B

/*
 * Software Change of PC, except by an exception,
 * architecturally executed
 */

#define XPM_EVENT_SWCHANGE 0x0C

/*
 * B immediate, BL immediate or BLX immediate instruction
 * architecturally executed
 *
 */

#define XPM_EVENT_IMMEDIATEINSTR 0x0D

/*
 * Procedure return architecturally executed, other than exception returns
 * For example:
 * BZ Rm, "LDM Rn, {..,PC}"
 */

#define XPM_EVENT_PROCEDURERET 0x0E

/*
 * Unaligned access architecturally executed
 * This event occurs for each instruction that was to an unaligned
 * address that either triggered an alignment fault, or would have done
 * so if the SCTLR A-bit had been set.
 */

#define XPM_EVENT_UNALIGNACCESS 0x0F

/*
 * Branch mispredicted or not predicted
 * This event ocurs for every pipeline flush
 * caused by a branch
 */

#define XPM_EVENT_BRANCHMISPREDICT 0x10

/*
 * Counts clock cycles when the Cortex-r5 processor is not in WFE/WFI. This
 * event is not exported on the EVENT bus
 */

#define XPM_EVENT_CLOCKCYCLES 0x11U

/*
 * Branches or other change in program flow that could have
 * been predicted by the branch prediction resources of the processor.
 */

#define XPM_EVENT_BRANCHPREDICT 0x12

/*
 * Stall because instruction buffer cannot deliver an instruction
 * This can indicate a cache miss. This event occurs every cycle where
 * the conditions is present.
 */


#if defined (ARMR52)
#define XPM_EVENT_DATAMEM_ACCESS				0x13U
#define XPM_EVENT_L1INSRCACHE_ACCESS			0x14U
#define XPM_EVENT_BUS_ACCESS					0x19U
#define XPM_EVENT_MEM_ERROR						0x1AU
#define XPM_EVENT_INSTR_SPECULATED				0x1BU
#define XPM_EVENT_BUS_CYCLES					0x1DU
#define XPM_EVENT_CHAINED_EVENTS				0x1EU
#define XPM_EVENT_BR_RETIRED					0x21U
#define XPM_EVENT_BR_MISPRED_RETIRED			0x22U
#define XPM_EVENT_STALL_FRONTEND				0x23U
#define XPM_EVENT_STALL_BACKEND					0x24U
#endif

#if defined (VERSAL_ZYNQMP_R5)
#define XPM_EVENT_INSTRSTALL 0x40

/*
 * Stall because of data dependency between instructions.
 * This event occurs every cycle where the condition is present.
 */

#define XPM_EVENT_DATASTALL 0x41

/*
 * Data cache write-back
 * This event occurs once for each line that is written back from the cache.
 */

#define XPM_EVENT_DATACACHEWRITE 0x42

/*
 * External memory request
 * Examples of this are cache refill, Non-cacheable accesses, write through
 * writes, cache line evictions(write-back)
 */

#define XPM_EVENT_EXTERNALMEMREQ 0x43U

/*
 * Stall because of LSU being busy
 * This event takes place each clock cycle where the condition is met.
 * A high incidence of this event indicates the pipeline is often waiting
 * for transactions to complete on the external bus.
 */

#define XPM_EVENT_LSUSTALL 0x44

/*
 * Store Buffer was forced to drain completely
 * Examples of this Fir cortex-R5 are DMB, Strongly ordered memory access,
 * or similar events.
 *
 */

#define XPM_EVENT_FORCEDRAINSTORE 0x45

/*
 * Instruction cache tag RAm parity or correctable ECC error
 */

#define XPM_EVENT_INSTRTAGPARITY 0x4A

/*
 * Instruction cache data RAm parity or correctable ECC error
 */

#define XPM_EVENT_INSTRDATAPARITY 0x4B

/*
 * Data cache tag or dirty RAM parity error or correctable ECC error,
 * from data-side or ACP
 */

#define XPM_EVENT_DATATAGPARITY 0x4C

/*
 * Data cache data RAM parity error or correctable ECC error
 */

#define XPM_EVENT_DATADATAPARITY 0x4D

/*
 * TCM fatal ECC error reported from the prefetch unit
 */

#define XPM_EVENT_TCMERRORPREFETCH 0x4E

/*
 * TCM fatal ECC error reported from the load/store unit
 */

#define XPM_EVENT_TCMERRORSTORE 0x4F

/*
 * Instruction cache access
 */

#define XPM_EVENT_INSTRCACHEACCESS 0x58

/*
 * Dual issue case A(branch)
 */

#define XPM_EVENT_DUALISSUEA 0x5A

/*
 * Dual issue case B1,B2,F2(load/store), F2D.
 */

#define XPM_EVENT_DUALISSUEB 0x5B

/*
 * Dual issue other case
 */

#define XPM_EVENT_DUALISSUEOTHER 0x5C

/*
 * Double precision floating point arithmetic or
 * conversion instruction executed.
 */

#define XPM_EVENT_FPA 0x5D
#endif

#if defined (ARMR52)
#define XPM_EVENT_BUS_ACCESS_LD 				0x60U
#define XPM_EVENT_BUS_ACCESS_ST 				0x61U
#endif


#if defined (VERSAL_ZYNQMP_R5)
/*
 * Data cache data RAM fatal ECC error
 */

#define XPM_EVENT_DATACACHEDATAERROR 0x60

/*
 * Data cache tag/dirty RAM fatal ECC error, from data-side or ACP
 */

#define XPM_EVENT_DATACACHETAGERROR 0x61

/*
 * Processor livelock because of hard errors or exception vector
 */

#define XPM_EVENT_PROCESSORLIVELOCK 0x62

/*
 * ATCM Multi-bit error
 */

#define XPM_EVENT_ATCMMULTIBITERROR 0x64

/*
 * B0TCM Multi-bit error
 */

#define XPM_EVENT_B0TCMMULTIBITERROR 0x65

/*
 * B1TCM Multi-bit error
 */

#define XPM_EVENT_B1TCMMULTIBITERROR 0x66

/*
 * ATCM Single-bit error
 */

#define XPM_EVENT_ATCMSINGLEBITERROR 0x67

/*
 * B0TCM Single-bit error
 */

#define XPM_EVENT_B0TCMSINGLEBITERROR 0x68

/*
 * B1TCM Single-bit error
 */

#define XPM_EVENT_B1TCMSINGLEBITERROR 0x69

/*
 * TCM correctable ECC error reported by load/store unit
 */

#define XPM_EVENT_TCMERRORLSU 0x6A

/*
 * TCM correctable ECC error reported by prefetch unit
 */

#define XPM_EVENT_TCMERRORPFU 0x6B

/*
 * TCM fatal ECC error reported by AXI slave unit
 */

#define XPM_EVENT_TCMFATALERRORAXI 0x6C

/*
 * TCM correctable ECC error reported by AXI slave unit
 */

#define XPM_EVENT_TCMERRORAXI 0x6D

/*
 * ACP D-cache access, lookup or invalidate
 */


#define XPM_EVENT_DCACHEACCESS 0x72

/*
 * ACP D-cache invalidate
 */

#define XPM_EVENT_DCACHEINVALIDATE 0x73
#endif

#if defined (ARMR52)
#define XPM_EVENT_EXC_SVC 						0x82U
#define XPM_EVENT_EXC_IRQ 						0x86U
#define XPM_EVENT_EXC_FIQ 						0x87U
#define XPM_EVENT_EXC_HVC 						0x8AU
#define XPM_EVENT_EXC_TRAP_IRQ 					0x8EU
#define XPM_EVENT_EXC_TRAP_FIQ 					0x8FU
#define XPM_EVENT_KITE_AXI_READ					0xC0U
#define XPM_EVENT_KITE_AXI_WRITE				0xC1U
#define XPM_EVENT_KITE_FLASH_READ				0xC2U
#define XPM_EVENT_KITE_LLPP_READ				0xC3U
#define XPM_EVENT_KITE_LLPP_WRITE				0xC4U
#define XPM_EVENT_KITE_NC_AXI_READ				0xC5U
#define XPM_EVENT_KITE_NC_AXI_WRITE				0xC6U
#define XPM_EVENT_KITE_NC_FLASH_READ			0xC7U
#define XPM_EVENT_KITE_REFILL_PF_AXI			0xC8U
#define XPM_EVENT_KITE_REFILL_LS_AXI			0xC9U
#define XPM_EVENT_KITE_REFILL_LS_FLASH			0xCAU
#define XPM_EVENT_KITE_DC_ACCESS_AXI			0xCBU
#define XPM_EVENT_KITE_DC_ACCESS_FLASH			0xCCU
#define XPM_EVENT_KITE_IC_ACCESS_AXI			0xCDU
#define XPM_EVENT_KITE_IC_ACCESS_FLASH			0xCEU
#define XPM_EVENT_KITE_NC_LS_HINTED_AXI			0xCFU
#define XPM_EVENT_KITE_NC_LS_HINTED_FLASH_READ	0xD0U
#define XPM_EVENT_KITE_REFILL_IC_AXI			0xD1U
#define XPM_EVENT_KITE_REFILL_IC_FLASH			0xD2U
#define XPM_EVENT_KITE_NC_LS_AXI_READ			0xD3U
#define XPM_EVENT_KITE_NC_LS_FLASH_READ			0xD4U
#define XPM_EVENT_KITE_COND_BR_RETIRED			0xD5U
#define XPM_EVENT_KITE_MIS_PRED_COND_BR			0xD6U
#define XPM_EVENT_KITE_BTAC_BR_RETIRED			0xD7U
#define XPM_EVENT_KITE_MIS_PRED_BTAC_BR			0xD8U
#define XPM_EVENT_KITE_VSCTLR_CHANGED			0xD9U
#define XPM_EVENT_KITE_DSB_ALL_RETIRED			0xDAU
#define XPM_EVENT_KITE_SIMULT_ACCESS_AXI		0xDBU
#define XPM_EVENT_KITE_SIMULT_ACCESS_FLASH		0xDCU
#define XPM_EVENT_KITE_EL2_ENTERED				0xDDU
#define XPM_EVENT_KITE_CRS_BR_RETIRED			0xDEU
#define XPM_EVENT_KITE_MIS_PRED_CRS_BR			0xDFU
#define XPM_EVENT_KITE_COR_ERR_MEM				0xF0U
#define XPM_EVENT_KITE_FAT_ERR_MEM				0xF1U
#define XPM_EVENT_KITE_BUS_COR_DATA				0xF2U
#define XPM_EVENT_KITE_BUS_FAT_OTHER			0xF3U
#define XPM_EVENT_KITE_BUS_PROTOCOL_ANY			0xF4U
#define XPM_EVENT_KITE_IQ_EMPTY_NO_MISS			0x100U
#define XPM_EVENT_KITE_IQ_EMPTY_AXI_MISS		0x101U
#define XPM_EVENT_KITE_IQ_EMPTY_FLASH_MISS		0x102U
#define XPM_EVENT_KITE_INTERLOCK_OTHER			0x103U
#define XPM_EVENT_KITE_INTERLOCK_AGU			0x104U
#define XPM_EVENT_KITE_INTERLOCK_FPASIMD		0x105U
#define XPM_EVENT_KITE_LOAD_STALL_AXI			0x106U
#define XPM_EVENT_KITE_LOAD_STALL_FLASH			0x107U
#define XPM_EVENT_KITE_WR_STALL_STORE			0x108U
#define XPM_EVENT_KITE_WR_STALL_AXI_STB_FULL	0x109U
#define XPM_EVENT_KITE_WR_STALL_TCM_STB_FULL	0x10AU
#define XPM_EVENT_KITE_WR_STALL_LLPP_STB_FULL	0x10BU
#define XPM_EVENT_KITE_BARRIER_STALL_BARRIER	0x10CU
#define XPM_EVENT_KITE_BARRIER_STORE_AXIWRITE	0x10DU
#define XPM_EVENT_KITE_IC_WT_HIT				0x200U
#define XPM_EVENT_KITE_DC_WT_HIT				0x201U
#define XPM_EVENT_KITE_I_UMPU_HIT				0x202U
#define XPM_EVENT_KITE_D_UMPU_HIT				0x203U
#define XPM_EVENT_KITE_IC_CACHE_HIT				0x204U
#define XPM_EVENT_KITE_IC_LFB_HIT				0x205U
#define XPM_EVENT_KITE_IC_BIU_HIT				0x206U
#define XPM_EVENT_KITE_IC_HINT_REQ				0x207U
#endif

/*
 * The following constants define the configurations for Cortex-R5 Performance
 * Monitor Events. Each configuration configures the event counters for a set
 * of events.
 * -----------------------------------------------
 * Config		PmCtr0... PmCtr5
 * -----------------------------------------------
 * XPM_CNTRCFG1		{ XPM_EVENT_SOFTINCR,
 *			  XPM_EVENT_INSTRCACHEMISS,
 *			  XPM_EVENT_DATACACHEMISS }
 *
 * XPM_CNTRCFG2		{ XPM_EVENT_DATACACHEACCESS,
 *			  XPM_EVENT_DATAREAD,
 *			  XPM_EVENT_DATAWRITE }
 *
 * XPM_CNTRCFG3		{ XPM_EVENT_INSTR,
 *			  XPM_EVENT_DUALINSTR,
 *			  XPM_EVENT_EXCEPTION }
 *
 * XPM_CNTRCFG4		{ XPM_EVENT_EXCEPTIONRET,
 *			  XPM_EVENT_CHANGETOCONTEXID,
 *			  XPM_EVENT_SWCHANGE }
 *
 * XPM_CNTRCFG5		{ XPM_EVENT_IMMEDIATEINSTR,
 *			  XPM_EVENT_PROCEDURERET,
 *			  XPM_EVENT_UNALIGNACCESS }
 *
 * XPM_CNTRCFG6		{ XPM_EVENT_BRANCHMISPREDICT,
 *			  XPM_EVENT_CLOCKCYCLES,
 *			  XPM_EVENT_BRANCHPREDICT }
 *
 * XPM_CNTRCFG7		{ XPM_EVENT_INSTRSTALL,
 *			  XPM_EVENT_DATASTALL,
 *			  XPM_EVENT_DATACACHEWRITE }
 *
 * XPM_CNTRCFG8		{ XPM_EVENT_EXTERNALMEMREQ,
 *            XPM_EVENT_LSUSTALL,
 *			  XPM_EVENT_FORCEDRAINSTORE }
 *
 * XPM_CNTRCFG9		{ XPM_EVENT_INSTRTAGPARITY,
 *            XPM_EVENT_INSTRDATAPARITY,
 *			  XPM_EVENT_DATATAGPARITY }
 *
 * XPM_CNTRCFG10	{ XPM_EVENT_DATADATAPARITY,
 *            XPM_EVENT_TCMERRORPREFETCH,
 *			  XPM_EVENT_TCMERRORSTORE }
 *
 * XPM_CNTRCFG11	{ XPM_EVENT_INSTRCACHEACCESS,
 *            XPM_EVENT_DUALISSUEA,
 *			  XPM_EVENT_DUALISSUEB }
 *
 * XPM_CNTRCFG12	{ XPM_EVENT_DUALISSUEOTHER,
 *            XPM_EVENT_FPA,
 *			  XPM_EVENT_DATACACHEDATAERROR }
 *
 * XPM_CNTRCFG13	{ XPM_EVENT_DATACACHETAGERROR,
 *            XPM_EVENT_PROCESSORLIVELOCK,
 *			  XPM_EVENT_ATCMMULTIBITERROR }
 *
 * XPM_CNTRCFG14	{ XPM_EVENT_B0TCMMULTIBITERROR,
 *            XPM_EVENT_B1TCMMULTIBITERROR,
 *			  XPM_EVENT_ATCMSINGLEBITERROR }
 *
 * XPM_CNTRCFG15	{ XPM_EVENT_B0TCMSINGLEBITERROR,
 *            XPM_EVENT_B1TCMSINGLEBITERROR,
 *			  XPM_EVENT_TCMERRORLSU }
 *
 * XPM_CNTRCFG16	{ XPM_EVENT_TCMERRORPFU,
 *            XPM_EVENT_TCMFATALERRORAXI,
 *			  XPM_EVENT_TCMERRORAXI }
 */
#define XPM_CNTRCFG1	0
#define XPM_CNTRCFG2	1
#define XPM_CNTRCFG3	2
#define XPM_CNTRCFG4	3
#define XPM_CNTRCFG5	4
#define XPM_CNTRCFG6	5
#define XPM_CNTRCFG7	6
#define XPM_CNTRCFG8	7
#define XPM_CNTRCFG9	8
#define XPM_CNTRCFG10	9
#define XPM_CNTRCFG11	10
#define XPM_CNTRCFG12   11
#define XPM_CNTRCFG13   12
#define XPM_CNTRCFG14   13
#define XPM_CNTRCFG15   14
#define XPM_CNTRCFG16   15

#if defined (ARMR52)
#define XPM_NO_COUNTERS_AVAILABLE       0xFFU
#define XPM_MAX_EVENTHANDLER_ID         0x3U
#define XPM_EVENT_CNTRS_BIT_MASK        0xFU
#define XPM_ALL_EVENT_CNTRS_IN_USE      0xFU
#define XPM_EVENT_CNTRS_MASK            0xFU
#else
#define XPM_NO_COUNTERS_AVAILABLE 	0xFFU
#define XPM_MAX_EVENTHANDLER_ID		0x2U
#define XPM_EVENT_CNTRS_BIT_MASK	0x7U
#define XPM_ALL_EVENT_CNTRS_IN_USE	0x7U
#define XPM_EVENT_CNTRS_MASK		0x3U
#endif

/**************************** Type Definitions ******************************/
/**
 *@endcond
 */
/***************** Macros (Inline Functions) Definitions ********************/
#if defined(__GNUC__)
#define Xpm_ReadCycleCounterVal()	mfcp(XREG_CP15_PERF_CYCLE_COUNTER)
#elif defined (__ICCARM__)
#define Xpm_ReadCycleCounterVal(val)       mfcp(XREG_CP15_PERF_CYCLE_COUNTER,val)
#endif
/************************** Variable Definitions ****************************/

/************************** Function Prototypes *****************************/

/* Interface functions to access performance counters from abstraction layer */
#if !defined(VERSAL_NET)
#if defined(__GNUC__)
void Xpm_SetEvents(s32 PmcrCfg) __attribute__ ((deprecated));
void Xpm_GetEventCounters(u32 *PmCtrValue) __attribute__ ((deprecated));
#else
void Xpm_SetEvents(s32 PmcrCfg);
void Xpm_GetEventCounters(u32 *PmCtrValue);
#endif
#endif
u32 Xpm_DisableEvent(u32 EventCntrId);
u32 Xpm_SetUpAnEvent(u32 EventID);
u32 Xpm_GetEventCounter(u32 EventCntrId, u32 *CntVal);
void Xpm_DisableEventCounters(void);
void Xpm_EnableEventCounters (void);
void Xpm_ResetEventCounters (void);

/* This is helper function for sleep/usleep APIs */
void Xpm_SleepPerfCounter(u32 delay, u64 frequency);

#ifdef __cplusplus
}
#endif

#endif

/**
* @} End of "addtogroup r5_event_counter_apis".
*/
