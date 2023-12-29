// --------------------------------------------------------------------------------------------------------------------
///
/// \file LibCanILCfg.c
///
/// \brief Configuration file for the CanIL implementation
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
#include "LibCanIL.h"
#include "LibCanILCfg.h"
#include "LibCanILCbk.h"
// --------------------------------------------------------------------------------------------------------------------
//	Local Definitions 
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
//	Local Data Types
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
//	Local Variables
// --------------------------------------------------------------------------------------------------------------------
static const S_LibCanIL_MessageDesc_t LibCanILCfg_Messages[(uint8_t)LIBCANILCFG_MESSAGE_NAME_DIMENSION] =
{   
	[LIBCANIL_MSG_COMMONTESTRX_NM] = { // common test network manage frame
		.Id				= CAN_NM_RX_ID,
		.IsExtId		= false,
		.Length			= LIBCAN_DLCSIZE_8_B,
		.IsTx			= false,
		.IsIntel		= false,
		.CycleTime		= UINT16_C(0),
		.StartDelayTime	= UINT16_C(0),
		.CanDevId		= CAN_NM_CHANNEL,
		.FirstSignal    = LIBCANIL_NMSIG_COMMONTESTRX_NODEID,
		.NSignals		= UINT8_C(8),
		.FirstMsgRecCbk	= LIBCANILCFG_CALLBACK_NAME_NO_CALLBACK,
		.NDataChCbks	= UINT8_C(0),
		.IsASWHndle     = false,
	},
	[LIBCANIL_MSG_COMMONTESTTX_NM] = { // common test network manage frame
		.Id				= (CAN_NM_TX_BASE_ID + CAN_NM_TX_NODE_ID),
		.IsExtId		= false,
		.Length			= LIBCAN_DLCSIZE_8_B,
		.IsTx			= true,
		.IsIntel		= false,
		.CycleTime		= UINT16_C(0),
		.StartDelayTime	= UINT16_C(0),
		.CanDevId		= CAN_NM_CHANNEL,
		.FirstSignal    = LIBCANIL_NMSIG_COMMONTESTTX_NODEID,
		.NSignals		= UINT8_C(8),
		.FirstMsgRecCbk	= LIBCANILCFG_CALLBACK_NAME_NO_CALLBACK,
		.NDataChCbks	= UINT8_C(0),
		.IsASWHndle     = false,
	},
};

static const S_LibCanIL_SignalDesc_t LibCanILCfg_Signals[(uint8_t)(LIBCANILCFG_SIGNAL_NAME_DIMENSION)] =
{   
	[LIBCANIL_NMSIG_COMMONTESTRX_NODEID] = {
		.Length				= UINT8_C(8),
		.MsgStartBit		= UINT8_C(0),
		.StartValue			= UINT64_C(0),
		.StorageStartBit	= UINT16_C(0),
		.MsgName			= LIBCANIL_MSG_COMMONTESTRX_NM,
		.FirstDataChCbk		= LIBCANILCFG_CALLBACK_NAME_NO_CALLBACK,
		.NDataChCbks		= UINT8_C(0)
	},
	[LIBCANIL_NMSIG_COMMONTESTRX_CBV_REPEAT_MSG_REQ] = {
		.Length				= UINT8_C(1),
		.MsgStartBit		= UINT8_C(8),
		.StartValue			= UINT64_C(0),
		.StorageStartBit	= UINT16_C(8),
		.MsgName			= LIBCANIL_MSG_COMMONTESTRX_NM,
		.FirstDataChCbk		= LIBCANILCFG_CALLBACK_NAME_NO_CALLBACK,
		.NDataChCbks		= UINT8_C(0)
	},
	[LIBCANIL_NMSIG_COMMONTESTRX_CBV_COORDINATOR_SLEEP] = {
		.Length				= UINT8_C(1),
		.MsgStartBit		= UINT8_C(11),
		.StartValue			= UINT64_C(0),
		.StorageStartBit	= UINT16_C(11),
		.MsgName			= LIBCANIL_MSG_COMMONTESTRX_NM,
		.FirstDataChCbk		= LIBCANILCFG_CALLBACK_NAME_NO_CALLBACK,
		.NDataChCbks		= UINT8_C(0)
	},
	[LIBCANIL_NMSIG_COMMONTESTRX_CBV_ACTIVE_WAKEUP] = {
		.Length				= UINT8_C(1),
		.MsgStartBit		= UINT8_C(12),
		.StartValue			= UINT64_C(0),
		.StorageStartBit	= UINT16_C(12),
		.MsgName			= LIBCANIL_MSG_COMMONTESTRX_NM,
		.FirstDataChCbk		= LIBCANILCFG_CALLBACK_NAME_NO_CALLBACK,
		.NDataChCbks		= UINT8_C(0)
	},
	[LIBCANIL_NMSIG_COMMONTESTRX_CBV_PARTIAL_NETWORK] = {
		.Length				= UINT8_C(1),
		.MsgStartBit		= UINT8_C(14),
		.StartValue			= UINT64_C(0),
		.StorageStartBit	= UINT16_C(14),
		.MsgName			= LIBCANIL_MSG_COMMONTESTRX_NM,
		.FirstDataChCbk		= LIBCANILCFG_CALLBACK_NAME_NO_CALLBACK,
		.NDataChCbks		= UINT8_C(0)
	},
	[LIBCANIL_NMSIG_COMMONTESTRX_USERDATA0] = {
		.Length				= UINT8_C(8),
		.MsgStartBit		= UINT8_C(16),
		.StartValue			= UINT64_C(0),
		.StorageStartBit	= UINT16_C(16),
		.MsgName			= LIBCANIL_MSG_COMMONTESTRX_NM,
		.FirstDataChCbk		= LIBCANILCFG_CALLBACK_NAME_NO_CALLBACK,
		.NDataChCbks		= UINT8_C(0)
	},
	[LIBCANIL_NMSIG_COMMONTESTRX_USERDATA1] = {
		.Length				= UINT8_C(8),
		.MsgStartBit		= UINT8_C(24),
		.StartValue			= UINT64_C(0),
		.StorageStartBit	= UINT16_C(24),
		.MsgName			= LIBCANIL_MSG_COMMONTESTRX_NM,
		.FirstDataChCbk		= LIBCANILCFG_CALLBACK_NAME_NO_CALLBACK,
		.NDataChCbks		= UINT8_C(0)
	},
	[LIBCANIL_NMSIG_COMMONTESTRX_USERDATA2] = {
		.Length				= UINT8_C(8),
		.MsgStartBit		= UINT8_C(32),
		.StartValue			= UINT64_C(0),
		.StorageStartBit	= UINT16_C(32),
		.MsgName			= LIBCANIL_MSG_COMMONTESTRX_NM,
		.FirstDataChCbk		= LIBCANILCFG_CALLBACK_NAME_NO_CALLBACK,
		.NDataChCbks		= UINT8_C(0)
	},
	[LIBCANIL_NMSIG_COMMONTESTRX_USERDATA3] = {
		.Length				= UINT8_C(8),
		.MsgStartBit		= UINT8_C(40),
		.StartValue			= UINT64_C(0),
		.StorageStartBit	= UINT16_C(40),
		.MsgName			= LIBCANIL_MSG_COMMONTESTRX_NM,
		.FirstDataChCbk		= LIBCANILCFG_CALLBACK_NAME_NO_CALLBACK,
		.NDataChCbks		= UINT8_C(0)
	},
	[LIBCANIL_NMSIG_COMMONTESTRX_USERDATA4] = {
		.Length				= UINT8_C(8),
		.MsgStartBit		= UINT8_C(48),
		.StartValue			= UINT64_C(0),
		.StorageStartBit	= UINT16_C(48),
		.MsgName			= LIBCANIL_MSG_COMMONTESTRX_NM,
		.FirstDataChCbk		= LIBCANILCFG_CALLBACK_NAME_NO_CALLBACK,
		.NDataChCbks		= UINT8_C(0)
	},
	[LIBCANIL_NMSIG_COMMONTESTRX_USERDATA5] = {
		.Length				= UINT8_C(8),
		.MsgStartBit		= UINT8_C(56),
		.StartValue			= UINT64_C(0),
		.StorageStartBit	= UINT16_C(56),
		.MsgName			= LIBCANIL_MSG_COMMONTESTRX_NM,
		.FirstDataChCbk		= LIBCANILCFG_CALLBACK_NAME_NO_CALLBACK,
		.NDataChCbks		= UINT8_C(0)
	},
/* ----------------------------------------------------------------------------- */
	[LIBCANIL_NMSIG_COMMONTESTTX_NODEID] = {
		.Length				= UINT8_C(8),
		.MsgStartBit		= UINT8_C(0),
		.StartValue			= UINT64_C(0),
		.StorageStartBit	= UINT16_C(64),
		.MsgName			= LIBCANIL_MSG_COMMONTESTTX_NM,
		.FirstDataChCbk		= LIBCANILCFG_CALLBACK_NAME_NO_CALLBACK,
		.NDataChCbks		= UINT8_C(0)
	},
	[LIBCANIL_NMSIG_COMMONTESTTX_CBV_REPEAT_MSG_REQ] = {
		.Length				= UINT8_C(1),
		.MsgStartBit		= UINT8_C(8),
		.StartValue			= UINT64_C(0),
		.StorageStartBit	= UINT16_C(72),
		.MsgName			= LIBCANIL_MSG_COMMONTESTTX_NM,
		.FirstDataChCbk		= LIBCANILCFG_CALLBACK_NAME_NO_CALLBACK,
		.NDataChCbks		= UINT8_C(0)
	},
	[LIBCANIL_NMSIG_COMMONTESTTX_CBV_COORDINATOR_SLEEP] = {
		.Length				= UINT8_C(1),
		.MsgStartBit		= UINT8_C(11),
		.StartValue			= UINT64_C(0),
		.StorageStartBit	= UINT16_C(75),
		.MsgName			= LIBCANIL_MSG_COMMONTESTTX_NM,
		.FirstDataChCbk		= LIBCANILCFG_CALLBACK_NAME_NO_CALLBACK,
		.NDataChCbks		= UINT8_C(0)
	},
	[LIBCANIL_NMSIG_COMMONTESTTX_CBV_ACTIVE_WAKEUP] = {
		.Length				= UINT8_C(1),
		.MsgStartBit		= UINT8_C(12),
		.StartValue			= UINT64_C(0),
		.StorageStartBit	= UINT16_C(76),
		.MsgName			= LIBCANIL_MSG_COMMONTESTTX_NM,
		.FirstDataChCbk		= LIBCANILCFG_CALLBACK_NAME_NO_CALLBACK,
		.NDataChCbks		= UINT8_C(0)
	},
	[LIBCANIL_NMSIG_COMMONTESTTX_CBV_PARTIAL_NETWORK] = {
		.Length				= UINT8_C(1),
		.MsgStartBit		= UINT8_C(14),
		.StartValue			= UINT64_C(0),
		.StorageStartBit	= UINT16_C(78),
		.MsgName			= LIBCANIL_MSG_COMMONTESTTX_NM,
		.FirstDataChCbk		= LIBCANILCFG_CALLBACK_NAME_NO_CALLBACK,
		.NDataChCbks		= UINT8_C(0)
	},
	[LIBCANIL_NMSIG_COMMONTESTTX_USERDATA0] = {
		.Length				= UINT8_C(8),
		.MsgStartBit		= UINT8_C(16),
		.StartValue			= UINT64_C(0),
		.StorageStartBit	= UINT16_C(80),
		.MsgName			= LIBCANIL_MSG_COMMONTESTTX_NM,
		.FirstDataChCbk		= LIBCANILCFG_CALLBACK_NAME_NO_CALLBACK,
		.NDataChCbks		= UINT8_C(0)
	},
	[LIBCANIL_NMSIG_COMMONTESTTX_USERDATA1] = {
		.Length				= UINT8_C(8),
		.MsgStartBit		= UINT8_C(24),
		.StartValue			= UINT64_C(0),
		.StorageStartBit	= UINT16_C(88),
		.MsgName			= LIBCANIL_MSG_COMMONTESTTX_NM,
		.FirstDataChCbk		= LIBCANILCFG_CALLBACK_NAME_NO_CALLBACK,
		.NDataChCbks		= UINT8_C(0)
	},
	[LIBCANIL_NMSIG_COMMONTESTTX_USERDATA2] = {
		.Length				= UINT8_C(8),
		.MsgStartBit		= UINT8_C(32),
		.StartValue			= UINT64_C(0),
		.StorageStartBit	= UINT16_C(96),
		.MsgName			= LIBCANIL_MSG_COMMONTESTTX_NM,
		.FirstDataChCbk		= LIBCANILCFG_CALLBACK_NAME_NO_CALLBACK,
		.NDataChCbks		= UINT8_C(0)
	},
	[LIBCANIL_NMSIG_COMMONTESTTX_USERDATA3] = {
		.Length				= UINT8_C(8),
		.MsgStartBit		= UINT8_C(40),
		.StartValue			= UINT64_C(0),
		.StorageStartBit	= UINT16_C(104),
		.MsgName			= LIBCANIL_MSG_COMMONTESTTX_NM,
		.FirstDataChCbk		= LIBCANILCFG_CALLBACK_NAME_NO_CALLBACK,
		.NDataChCbks		= UINT8_C(0)
	},
	[LIBCANIL_NMSIG_COMMONTESTTX_USERDATA4] = {
		.Length				= UINT8_C(8),
		.MsgStartBit		= UINT8_C(48),
		.StartValue			= UINT64_C(0),
		.StorageStartBit	= UINT16_C(112),
		.MsgName			= LIBCANIL_MSG_COMMONTESTTX_NM,
		.FirstDataChCbk		= LIBCANILCFG_CALLBACK_NAME_NO_CALLBACK,
		.NDataChCbks		= UINT8_C(0)
	},
	[LIBCANIL_NMSIG_COMMONTESTTX_USERDATA5] = {
		.Length				= UINT8_C(8),
		.MsgStartBit		= UINT8_C(56),
		.StartValue			= UINT64_C(0),
		.StorageStartBit	= UINT16_C(120),
		.MsgName			= LIBCANIL_MSG_COMMONTESTTX_NM,
		.FirstDataChCbk		= LIBCANILCFG_CALLBACK_NAME_NO_CALLBACK,
		.NDataChCbks		= UINT8_C(0)
	},
/* --------------------------------------------------------------- */
};

static S_LibCanIL_CallbackDesc_t LibCanILCfg_Callbacks[LIBCANILCFG_CALLBACK_NAME_DIMENSION] =
{   

};

// --------------------------------------------------------------------------------------------------------------------
//	Global Variables
// --------------------------------------------------------------------------------------------------------------------
const S_LibCanIL_MessageTable_t LibCanILCfg_MessageTable =
{
	.NumOfMessages	= (uint8_t)LIBCANILCFG_MESSAGE_NAME_DIMENSION,
	.pMessageDesc	= LibCanILCfg_Messages
};

const S_LibCanIL_SignalTable_t LibCanILCfg_SignalTable =
{
	.NumOfSignals	= (uint8_t)(LIBCANILCFG_SIGNAL_NAME_DIMENSION),
	.pSignalDesc	= LibCanILCfg_Signals
};

const S_LibCanIL_CallbackTable_t LibCanILCfg_CallbackTable =
{
	.NumOfCallbacks	= (uint8_t)LIBCANILCFG_CALLBACK_NAME_DIMENSION,
	.pCallbackDesc	= LibCanILCfg_Callbacks
};

// --------------------------------------------------------------------------------------------------------------------
//	Local Function Prototypes
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
//	Functions
// --------------------------------------------------------------------------------------------------------------------



