// -------------------------------------------------------------------------------------------------------------------- 
/// 
/// \file LibCanDrvMsg.h
/// 
/// \brief 
/// 
/// 
/// Copyright (c) 2021 Neusoft.
/// All Rights Reserved.
/// 
// -------------------------------------------------------------------------------------------------------------------- 
#ifndef _LibCanDrvMsg_H
#define _LibCanDrvMsg_H
// -------------------------------------------------------------------------------------------------------------------- 
//  Includes 
// --------------------------------------------------------------------------------------------------------------------
//#include "../../../../LIB/TYPE/inc/LibTypes.h"
#include "LibTypes.h"
//typedef unsigned char bool_t;

// -------------------------------------------------------------------------------------------------------------------- 
//  Local Definitions 
// -------------------------------------------------------------------------------------------------------------------- 

// --------------------------------------------------------------------------------------------------------------------
//	Local Data Types
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
//	Global Variables
// --------------------------------------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------------------------------------
/// \brief Structure used for CAN message filter configuration. This structure should be declared as constant.
// --------------------------------------------------------------------------------------------------------------------
typedef struct
{
	// ----------------------------------------------------------------------------------------------------------------
	/// \brief CAN message ID.
	// ----------------------------------------------------------------------------------------------------------------
	const uint32_t Id;

	// ----------------------------------------------------------------------------------------------------------------
	/// \brief CAN message filter mask.
	// ----------------------------------------------------------------------------------------------------------------
	const uint32_t Mask;

	// ----------------------------------------------------------------------------------------------------------------
	/// \brief Flag which is true in case that the CAN message is standard one, otherwise false.
	// ----------------------------------------------------------------------------------------------------------------
	const bool_t IsStdMsgId;
	
} S_LibMcan_MsgFilt_t;

// --------------------------------------------------------------------------------------------------------------------
/// \brief Structure used as table of CAN message filter configuration. This structure should be declared as constant.
// --------------------------------------------------------------------------------------------------------------------
typedef struct
{
	// ----------------------------------------------------------------------------------------------------------------
	/// \brief Number of CAN messages.
	// ----------------------------------------------------------------------------------------------------------------
	const uint8_t NumOfMsgs;

	// ----------------------------------------------------------------------------------------------------------------
	/// \brief Pointer to the table of CAN message filters.
	// ----------------------------------------------------------------------------------------------------------------
	const S_LibMcan_MsgFilt_t* const pMsgFilts;
	
} S_LibMcan_MsgFiltTbl_t;

// --------------------------------------------------------------------------------------------------------------------
//	Local Variables
// --------------------------------------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------------------------------------
//	Local Function Prototypes
// --------------------------------------------------------------------------------------------------------------------
extern Ret_t LibMcan_IoCtl(void* pData, uint8_t command);

// --------------------------------------------------------------------------------------------------------------------
//	Functions
// --------------------------------------------------------------------------------------------------------------------
#endif
