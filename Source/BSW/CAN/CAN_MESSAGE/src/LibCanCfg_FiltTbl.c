// --------------------------------------------------------------------------------------------------------------------
///
/// \file LibCanCfg_FiltTbl.c
///
/// \brief CAN Filter Tables
///
/// This file is automatically by cantool
///
///
/// Copyright (c) 2021 Neusoft.
/// All Rights Reserved.
///
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
//	Local Definitions to control properties inside header files
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
//	Includes
// --------------------------------------------------------------------------------------------------------------------

#include "LibCanCfg_FiltTbl.h"


// --------------------------------------------------------------------------------------------------------------------
//	Local Definitions
// --------------------------------------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------------------------------------
/// \brief 	Filter mask to mask each bit of extended ID CAN message
// --------------------------------------------------------------------------------------------------------------------
 #define LIBCANINTLAYCFG_EXT_MASK_ALL		UINT32_C(0x1FFFFFFF)

// --------------------------------------------------------------------------------------------------------------------
/// \brief Filter mask to mask each bit of standard ID CAN message
// --------------------------------------------------------------------------------------------------------------------
#define LIBCANINTLAYCFG_STD_MASK_ALL		UINT32_C(0x000007FF)

// --------------------------------------------------------------------------------------------------------------------
//	Local Data Types
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
//	Local Variables
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
/// \brief Array of CAN message filters for CAN1
// --------------------------------------------------------------------------------------------------------------------
static const S_LibMcan_MsgFilt_t LibCanIntLayCfg_Msgs_CAN1[] =
{	

};




// --------------------------------------------------------------------------------------------------------------------
//	Global Variables
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
/// \brief CAN message filter configuration structure. CAN1
// --------------------------------------------------------------------------------------------------------------------
const S_LibMcan_MsgFiltTbl_t LibCanIntLayCfg_MsgTbl_CAN1 =
{
	.NumOfMsgs	= (uint8_t)(sizeof(LibCanIntLayCfg_Msgs_CAN1) / sizeof(LibCanIntLayCfg_Msgs_CAN1[0])),
	.pMsgFilts	= LibCanIntLayCfg_Msgs_CAN1
};



// --------------------------------------------------------------------------------------------------------------------
//	Local Function Prototypes
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
//	Functions
// --------------------------------------------------------------------------------------------------------------------



