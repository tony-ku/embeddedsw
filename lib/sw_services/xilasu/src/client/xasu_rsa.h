/**************************************************************************************************
* Copyright (c) 2024 Advanced Micro Devices, Inc. All Rights Reserved.
* SPDX-License-Identifier: MIT
**************************************************************************************************/

/*************************************************************************************************/
/**
 *
 * @file xasu_rsa.h
 * @addtogroup
 * @{
 * This file Contains the RSA client function prototypes, defines and macros for
 * the RSA hardware module.
 *
 * <pre>
 * MODIFICATION HISTORY:
 *
 * Ver   Who  Date     Changes
 * ----- ---- -------- ----------------------------------------------------------------------------
 * 1.0   ss   08/20/24 Initial release
 *
 * </pre>
 *
 *************************************************************************************************/

#ifndef XASU_RSA_H
#define XASU_RSA_H

#ifdef __cplusplus
extern "C" {
#endif

/*************************************** Include Files *******************************************/
#include "xil_types.h"
#include "xasu_rsainfo.h"
#include "xasu_client.h"

/************************************ Constant Definitions ***************************************/

/************************************** Type Definitions *****************************************/

/*************************** Macros (Inline Functions) Definitions *******************************/

/************************************ Function Prototypes ****************************************/
/**< RSA Client Operation functions */
s32 XAsu_RsaEnc(XAsu_ClientParams *ClientParamPtr, XAsu_RsaClientParams *RsaClientParamPtr);
s32 XAsu_RsaDec(XAsu_ClientParams *ClientParamPtr, XAsu_RsaClientParams *RsaClientParamPtr);
s32 XAsu_RsaCrtDec(XAsu_ClientParams *ClientParamPtr, XAsu_RsaClientParams *RsaClientParamPtr);
s32 XAsu_RsaKat(void);

/************************************ Variable Definitions ***************************************/
#ifdef __cplusplus
}
#endif

#endif  /* XASU_RSA_H */