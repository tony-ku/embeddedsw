/******************************************************************************
* Copyright (c) 2022 Xilinx, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/

/*****************************************************************************/
/**
*
* @file xnvm_validate.c
*
* This file contains the implementation of APIs used to validate write request for
* eFUSEs.
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who  Date        Changes
* ----- ---- -------- -------------------------------------------------------
* 1.0   har  07/21/2022 Initial release
*
* </pre>
*
* @note
*
******************************************************************************/

/***************************** Include Files *********************************/
#include "xnvm_validate.h"
#include "xnvm_common_defs.h"
#include "xnvm_defs.h"
#include "xnvm_utils.h"
#include "xstatus.h"
#include "xnvm_temp.h"

/**************************** Type Definitions *******************************/

/************************** Function Prototypes ******************************/
static int XNvm_EfuseValidateIV(const u32 *Iv, u32 IvAddress);
static int XNvm_EfuseCheckZeros(u32 CacheOffset, u32 Count);

/************************** Constant Definitions *****************************/

/***************** Macros (Inline Functions) Definitions *********************/
/******************************************************************************/
/**
 * @brief	This function reads the given register.
 *
 * @param	BaseAddress is the eFuse controller base address.
 * @param	RegOffset is the register offset from the base address.
 *
 * @return	The 32-bit value of the register.
 *
 ******************************************************************************/
/*  u32 XNvm_EfuseReadReg(u32 BaseAddress, u32 RegOffset)
{
	return Xil_In32((UINTPTR)(BaseAddress + RegOffset));
} */

/******************************************************************************/
/**
 * @brief	This function writes the value into the given register.
 *
 * @param	BaseAddress is the eFuse controller base address.
 * @param	RegOffset is the register offset from the base address.
 * @param	Data is the 32-bit value to be written to the register.
 *
 * @return	None
 *
 ******************************************************************************/
/* void XNvm_EfuseWriteReg(u32 BaseAddress, u32 RegOffset, u32 Data)
{
	Xil_Out32((UINTPTR)(BaseAddress + RegOffset), Data);
} */
/************************** Function Definitions *****************************/



/******************************************************************************/
/**
 * @brief	This function validates Aes key requested for programming.
 *
 * @param	KeyType - AesKey/UserKey0/Userkey1 type of aes key request to
 * 				be validated.
 *
 * @return	- XST_SUCCESS - if validation is successful.
 *		- XNVM_EFUSE_ERR_AES_ALREADY_PRGMD       - Aes key already
 *								programmed.
 *		- XNVM_EFUSE_ERR_USER_KEY0_ALREADY_PRGMD - User key 0 is already
 *								programmed.
 *		- XNVM_EFUSE_ERR_USER_KEY1_ALREADY_PRGMD - User key 1 is already
 *							 	programmed.
 *		- XNVM_EFUSE_ERR_FUSE_PROTECTED  - Efuse is write protected.
 *		- XNVM_EFUSE_ERR_WRITE_AES_KEY   - Error in writing Aes key.
 *		- XNVM_EFUSE_ERR_WRITE_USER0_KEY - Error in writing User key 0.
 *		- XNVM_EFUSE_ERR_WRITE_USER1_KEY - Error in writing User key 1.
 *
 ******************************************************************************/
int XNvm_EfuseValidateAesKeyWriteReq(XNvm_AesKeyType KeyType)
{
	int Status = XST_FAILURE;
	u32 SecCtrlBits = 0U;
	u32 CrcRegOffset = 0U;
	u32 CrcDoneMask = 0U;
	u32 CrcPassMask = 0U;
	u32 WrLkMask = 0U;

	SecCtrlBits = XNvm_EfuseReadReg(XNVM_EFUSE_CACHE_BASEADDR,
				XNVM_EFUSE_CACHE_SECURITY_CTRL_OFFSET);

	if (KeyType == XNVM_EFUSE_AES_KEY) {
		CrcRegOffset = XNVM_EFUSE_AES_CRC_REG_OFFSET;
		CrcDoneMask = XNVM_EFUSE_CTRL_STATUS_AES_CRC_DONE_MASK;
		CrcPassMask = XNVM_EFUSE_CTRL_STATUS_AES_CRC_PASS_MASK;
		WrLkMask = XNVM_EFUSE_CACHE_SECURITY_CONTROL_AES_WR_LK_MASK;
	}
	else if (KeyType == XNVM_EFUSE_USER_KEY_0) {
		CrcRegOffset = XNVM_EFUSE_AES_USR_KEY0_CRC_REG_OFFSET;
		CrcDoneMask = XNVM_EFUSE_CTRL_STATUS_AES_USER_KEY_0_CRC_DONE_MASK;
		CrcPassMask = XNVM_EFUSE_CTRL_STATUS_AES_USER_KEY_0_CRC_PASS_MASK;
		WrLkMask = XNVM_EFUSE_CACHE_SECURITY_CONTROL_USR_KEY_0_WR_LK_MASK;
	}
	else if (KeyType == XNVM_EFUSE_USER_KEY_1) {
		CrcRegOffset = XNVM_EFUSE_AES_USR_KEY1_CRC_REG_OFFSET;
		CrcDoneMask = XNVM_EFUSE_CTRL_STATUS_AES_USER_KEY_1_CRC_DONE_MASK;
		CrcPassMask = XNVM_EFUSE_CTRL_STATUS_AES_USER_KEY_1_CRC_PASS_MASK;
		WrLkMask = XNVM_EFUSE_CACHE_SECURITY_CONTROL_USR_KEY_1_WR_LK_MASK;
	}
	else {
		Status = (int)XST_INVALID_PARAM;
		goto END;
	}

	Status = XNvm_EfuseCheckAesKeyCrc(CrcRegOffset, CrcDoneMask,
			CrcPassMask, XNVM_EFUSE_CRC_AES_ZEROS);
	if (Status != XST_SUCCESS) {
		Status = (int)XNVM_EFUSE_ERR_AES_ALREADY_PRGMD +
			(KeyType << XNVM_EFUSE_ERROR_NIBBLE_SHIFT);
		goto END;
	}
	if (((SecCtrlBits & XNVM_EFUSE_CACHE_SECURITY_CONTROL_AES_DIS_MASK) != 0U) ||
		((SecCtrlBits & WrLkMask) != 0U)) {
		Status = (XNVM_EFUSE_ERR_FUSE_PROTECTED |
			(XNVM_EFUSE_ERR_WRITE_AES_KEY + (KeyType << XNVM_EFUSE_ERROR_BYTE_SHIFT)));
		goto END;
	}

	Status = XST_SUCCESS;

END:
	return Status;
}

/******************************************************************************/
/**
 * @brief	This function validates PPK Hash requested for programming.
 *
 * @param	PpkType - PpkHash0/PpkHash1/PpkHash2 type of PpkHash request to
 * 				be validated
 *
 * @return	- XST_SUCCESS - if validates successfully.
 *		- XNVM_EFUSE_ERR_PPK0_HASH_ALREADY_PRGMD - Ppk0 hash already
 *							   programmed.
 *		- XNVM_EFUSE_ERR_PPK1_HASH_ALREADY_PRGMD - Ppk1 hash already
 *							   programmed.
 *		- XNVM_EFUSE_ERR_PPK2_HASH_ALREADY_PRGMD - Ppk2 hash already
 *							   programmed.
 *		- XNVM_EFUSE_ERR_FUSE_PROTECTED  - Efuse is write protected.
 *		- XNVM_EFUSE_ERR_WRITE_PPK0_HASH - Error in writing ppk0 hash.
 *		- XNVM_EFUSE_ERR_WRITE_PPK1_HASH - Error in writing ppk1 hash.
 *		- XNVM_EFUSE_ERR_WRITE_PPK2_HASH - Error in writing ppk2 hash.
 *
 ******************************************************************************/
int XNvm_EfuseValidatePpkHashWriteReq(XNvm_PpkType PpkType)
{
	int Status = XST_FAILURE;
	u32 SecCtrlBits = 0U;
	u32 PpkOffset = 0U;
	u32 WrLkMask = 0U;

	SecCtrlBits = XNvm_EfuseReadReg(XNVM_EFUSE_CACHE_BASEADDR,
				XNVM_EFUSE_CACHE_SECURITY_CTRL_OFFSET);
	if (PpkType == XNVM_EFUSE_PPK0) {
		PpkOffset = XNVM_EFUSE_CACHE_PPK0_HASH_OFFSET;
		WrLkMask = XNVM_EFUSE_CACHE_SECURITY_CONTROL_PPK0_WR_LK_MASK;
	}
	else if (PpkType == XNVM_EFUSE_PPK1) {
		PpkOffset = XNVM_EFUSE_CACHE_PPK1_HASH_OFFSET;
		WrLkMask = XNVM_EFUSE_CACHE_SECURITY_CONTROL_PPK1_WR_LK_MASK;
	}
	else if (PpkType == XNVM_EFUSE_PPK2) {
		PpkOffset = XNVM_EFUSE_CACHE_PPK2_HASH_OFFSET;
		WrLkMask = XNVM_EFUSE_CACHE_SECURITY_CONTROL_PPK2_WR_LK_MASK;
	}
	else {
		Status = (int)XNVM_EFUSE_ERR_INVALID_PARAM;
		goto END;
	}

	Status = XNvm_EfuseCheckZeros(PpkOffset,
			XNVM_EFUSE_PPK_HASH_NUM_OF_CACHE_ROWS);
	if (Status != XST_SUCCESS) {
		Status = (int)XNVM_EFUSE_ERR_PPK0_HASH_ALREADY_PRGMD +
			(PpkType << XNVM_EFUSE_ERROR_NIBBLE_SHIFT);
		goto END;
	}
	if ((SecCtrlBits & WrLkMask) != 0U) {
		Status = (XNVM_EFUSE_ERR_FUSE_PROTECTED |
			(XNVM_EFUSE_ERR_WRITE_PPK0_HASH +
			(PpkType << XNVM_EFUSE_ERROR_BYTE_SHIFT)));
	}

END:
	return Status;
}

/******************************************************************************/
/**
 * @brief	This function validates all IVs requested for programming.
 *
 * @param	IvType - IvType to be validated
 * @param	EfuseIv - Pointer to XNvm_EfuseIvs structure which holds IV data
 * 			to be programmed to eFuse.
 *
 * @return	- XST_SUCCESS - if validation is successful.
 *		- XNVM_EFUSE_ERR_WRITE_META_HEADER_IV_RANGE - Error in
 *							Metaheader IV range
 *							write request.
 *		- XNVM_EFUSE_ERR_BLK_OBFUS_IV_ALREADY_PRGMD - Error in Blk Obfus Iv
 *							  write request.
 *		- XNVM_EFUSE_ERR_WRITE_PLM_IV_RANGE - Error in Plm Iv range
 *							write request.
 *		- XNVM_EFUSE_ERR_WRITE_DATA_PARTITION_IV_RANGE - Error in
 *							Data Partition Iv range
 *							write request.
 *
 ******************************************************************************/
int XNvm_EfuseValidateIvWriteReq(XNvm_IvType IvType, XNvm_Iv *EfuseIv)
{
	int Status = XST_FAILURE;
	u32 IvOffset = 0U;

	if (IvType == XNVM_EFUSE_BLACK_IV) {
		Status = XNvm_EfuseCheckZeros(XNVM_EFUSE_CACHE_BLACK_IV_OFFSET,
				XNVM_EFUSE_IV_NUM_OF_CACHE_ROWS);
		if (Status != XST_SUCCESS) {
			Status = (int)XNVM_EFUSE_ERR_BLK_OBFUS_IV_ALREADY_PRGMD;
			goto END;
		}
	}
	else if (IvType == XNVM_EFUSE_META_HEADER_IV_RANGE) {
		IvOffset = XNVM_EFUSE_CACHE_METAHEADER_IV_RANGE_OFFSET;
	}
	else if (IvType == XNVM_EFUSE_PLM_IV_RANGE) {
		IvOffset = XNVM_EFUSE_CACHE_PLM_IV_RANGE_OFFSET;
	}
	else if (IvType == XNVM_EFUSE_DATA_PARTITION_IV_RANGE) {
		IvOffset = XNVM_EFUSE_CACHE_DATA_PARTITION_IV_OFFSET;
	}
	else {
		Status = (int)XNVM_EFUSE_ERR_INVALID_PARAM;
		goto END;
	}

	Status = XNvm_EfuseValidateIV(EfuseIv->Iv, IvOffset);
	if (Status != XST_SUCCESS) {
		Status = (Status |
			(XNVM_EFUSE_ERR_WRITE_META_HEADER_IV_RANGE +
			(IvType << XNVM_EFUSE_ERROR_BYTE_SHIFT)));
	}

END:
	return Status;
}

/******************************************************************************/
/**
 * @brief	This function validates IV requested for programming
 *		bit by bit with cache IV to check if the request is to revert
 *		the already programmed eFuse.
 *
 * @param	Iv  - Pointer to Iv data to be programmed.
 * @param	IvAddress - Start address of the Iv to be validated.
 *
 * @return	- XST_SUCCESS - if validation is successful.
 *		- XNVM_EFUSE_ERR_INVALID_PARAM - On Invalid Parameter.
 *		- XNVM_EFUSE_ERR_BIT_CANT_REVERT - if requested to revert the
 *						   already programmed bit.
 *
 *******************************************************************************/
static int XNvm_EfuseValidateIV(const u32 *Iv, u32 IvOffset)
{
	int Status = XST_FAILURE;
	u32 IvRowsRd;
	u32 EndOffset = IvOffset + XNVM_EFUSE_IV_LEN_IN_WORDS * 4;
	u32 Offset = IvOffset;

	while(Offset < EndOffset){
		IvRowsRd = XNvm_EfuseReadReg(XNVM_EFUSE_CACHE_BASEADDR, Offset);
		if ((IvRowsRd & Iv[((Offset-IvOffset) / XNVM_WORD_LEN)]) != IvRowsRd) {
			Status = (XNVM_EFUSE_ERR_BEFORE_PROGRAMMING |
					XNVM_EFUSE_ERR_BIT_CANT_REVERT);
			goto END;
		}

		Offset = Offset + XNVM_WORD_LEN;
	}

END:
	return Status;
}

/******************************************************************************/
/**
 * @brief	This function is used verify eFUSEs for Zeros.
 *
 * @param	CacheOffset - Cache Offset from which verification has to be started.
 * @param	Count  - Number of rows till which verification has to be ended.
 *
 * @return	- XST_SUCCESS - if efuses are not programmed.
 * 		- XST_FAILURE - if efuses are already programmed.
 *
 ******************************************************************************/
static int XNvm_EfuseCheckZeros(u32 CacheOffset, u32 Count)
{
	volatile int Status = XST_FAILURE;
	int IsrStatus = XST_FAILURE;
	u32 EndOffset = CacheOffset + Count * XNVM_WORD_LEN;
	u32 Offset = CacheOffset;
	u32 CacheData = 0U;

	IsrStatus = XNvm_EfuseReadReg(XNVM_EFUSE_CTRL_BASEADDR,
					XNVM_EFUSE_ISR_REG_OFFSET);
	if ((IsrStatus & XNVM_EFUSE_ISR_CACHE_ERROR)
			== XNVM_EFUSE_ISR_CACHE_ERROR) {
		Status = (int)XNVM_EFUSE_ERR_CACHE_PARITY;
		goto END;
	}
	while (Offset < EndOffset) {
		CacheData  = XNvm_EfuseReadReg(XNVM_EFUSE_CACHE_BASEADDR, Offset);
		if (CacheData != 0x00U) {
			Status = XST_FAILURE;
			break;
		}
		Offset = Offset + XNVM_WORD_LEN;
	}

	Status = XST_SUCCESS;

END:
	return Status;
}