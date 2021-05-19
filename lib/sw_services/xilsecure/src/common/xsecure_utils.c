/******************************************************************************
* Copyright (c) 2019 - 2021 Xilinx, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/


/*****************************************************************************/
/**
*
* @file xsecure_utils.c
* This file contains common functionalities required for xilsecure library
* like functions to read/write hardware registers, SSS switch configurations.
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who     Date     Changes
* ----- ------  -------- ------------------------------------------------------
* 4.0   vns     03/09/19 Initial release
*       psl     03/26/19 Fixed MISRA-C violation
*       psl     04/05/19 Fixed IAR warnings.
* 4.1   psl     07/31/19 Fixed MISRA-C violation
* 4.2   har     01/03/20 Added blind write check for SssCfg
*       vns     01/24/20 Added assert statements to input arguments
*       har     03/26/20 Removed code for SSS configuration
*       rpo     09/10/20 Asserts are placed under XSECDEBUG macro
* 4.3	am      09/24/20 Resolved MISRA C violations
*       har     10/12/20 Addressed security review comments
* 4.4   bm      01/13/21 Added XSecure_MemCpy64 api
*       bm      05/19/21 Fix unaligned transfers in XSecure_MemCpy64
*
* </pre>
*
******************************************************************************/

/***************************** Include Files *********************************/
#include "xsecure_utils.h"

/************************** Constant Definitions *****************************/

/************************** Function Prototypes ******************************/

/************************** Function Definitions *****************************/

/*****************************************************************************/
/**
 * @brief	This function takes the hardware core out of reset
 *
 * @param	BaseAddress	- Base address of the core
 * @param	Offset		- Offset of the reset register
 *
 * @return	None
 *
 *****************************************************************************/
void XSecure_ReleaseReset(u32 BaseAddress, u32 Offset)
{
	XSecure_WriteReg(BaseAddress, Offset, XSECURE_RESET_SET);
	XSecure_WriteReg(BaseAddress, Offset, XSECURE_RESET_UNSET);
}

/*****************************************************************************/
/**
 * @brief	This function places the hardware core into the reset
 *
 * @param	BaseAddress	- Base address of the core
 * @param	Offset		- Offset of the reset register
 *
 * @return	None
 *
 *****************************************************************************/
void XSecure_SetReset(u32 BaseAddress, u32 Offset)
{
	XSecure_WriteReg(BaseAddress, Offset, XSECURE_RESET_SET);
}

/***************************************************************************/
/**
 * @brief	This function copies data from 64 bit address Src to 64 bit
 * address Dst
 *
 * @param	DstAddr is the 64 bit destination address
 * @param	SrcAddr is the 64 bit source address
 * @param	Cnt is the number of bytes of data to be copied
 *
 ******************************************************************************/
void XSecure_MemCpy64(u64 DstAddr, u64 SrcAddr, u32 Cnt)
{
	if (((DstAddr & XSECURE_WORD_ALIGN_MASK) == 0U) &&
		((SrcAddr & XSECURE_WORD_ALIGN_MASK) == 0U)) {
		while (Cnt >= sizeof (int)) {
			XSecure_Out64(DstAddr, XSecure_In64(SrcAddr));
			DstAddr += sizeof(int);
			SrcAddr += sizeof(int);
			Cnt -= (u32)sizeof(int);
		}
	}
	while (Cnt > 0U) {
		XSecure_OutByte64(DstAddr, XSecure_InByte64(SrcAddr));
		DstAddr += 1U;
		SrcAddr += 1U;
		Cnt -= 1U;
	}
}
