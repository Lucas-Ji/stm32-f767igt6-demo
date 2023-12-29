// --------------------------------------------------------------------------------------------------------------------
///
/// \file LibCan.c
///
/// \brief Generic CAN interface
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

#include "LibCanMsg.h"



// --------------------------------------------------------------------------------------------------------------------
//	Local Definitions
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
//	Local Data Types
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
//	Global Variables
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
//	Local Variables
// --------------------------------------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------------------------------------
/// \brief The lookup table of the CAN message length
// --------------------------------------------------------------------------------------------------------------------
static uint8_t LibCan_MsgDataLenth[] =
{
	[LIBCAN_DLCSIZE_0_B]	= 0U,
	[LIBCAN_DLCSIZE_1_B]	= 1U,
	[LIBCAN_DLCSIZE_2_B]	= 2U,
	[LIBCAN_DLCSIZE_3_B]	= 3U,
	[LIBCAN_DLCSIZE_4_B]	= 4U,
	[LIBCAN_DLCSIZE_5_B]	= 5U,
	[LIBCAN_DLCSIZE_6_B]	= 6U,
	[LIBCAN_DLCSIZE_7_B]	= 7U,
	[LIBCAN_DLCSIZE_8_B]	= 8U,
	[LIBCAN_DLCSIZE_12_B]	= 12U,
	[LIBCAN_DLCSIZE_16_B]	= 16U,
	[LIBCAN_DLCSIZE_20_B]	= 20U,
	[LIBCAN_DLCSIZE_24_B]	= 24U,
	[LIBCAN_DLCSIZE_32_B]	= 32U,
	[LIBCAN_DLCSIZE_48_B]	= 48U,
	[LIBCAN_DLCSIZE_64_B]	= 64U
};


// --------------------------------------------------------------------------------------------------------------------
//	Local Function Prototypes
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
//	Functions
// --------------------------------------------------------------------------------------------------------------------

//=====================================================================================================================
// LibCan_GetMsgDataLength:
//=====================================================================================================================
uint8_t LibCan_GetMsgDataLength(E_LibCan_DlcSize_t dlc)
{
	return LibCan_MsgDataLenth[dlc];
}
