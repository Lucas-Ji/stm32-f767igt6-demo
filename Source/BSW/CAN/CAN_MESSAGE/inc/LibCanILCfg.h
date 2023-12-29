// --------------------------------------------------------------------------------------------------------------------
///
/// \file LibCanILCfg.h
///
/// \brief This module implements the CAN Interaction Layer
///
/// This file is automatically by cantool
///
///
/// Copyright (c) 2021 Neusoft.
/// All Rights Reserved.
///
// --------------------------------------------------------------------------------------------------------------------

 

#ifndef LIBCANILCFG_H__INCLUDED
#define LIBCANILCFG_H__INCLUDED

// --------------------------------------------------------------------------------------------------------------------
//	Includes
// --------------------------------------------------------------------------------------------------------------------
#include "LibTypes.h"
#include "CanNmCfg.h"
// --------------------------------------------------------------------------------------------------------------------
//	Global Definitions
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
//	Global Definitions
// --------------------------------------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------------------------------------
/// \brief Length of internal signal storage in byte. Sum of all bits of all signals.
// --------------------------------------------------------------------------------------------------------------------
#define LIBCANILCFG_SIGNAL_STORAGE_LENGTH		UINT16_C(15)

// --------------------------------------------------------------------------------------------------------------------
/// \brief number of all tx messages of this module.
// --------------------------------------------------------------------------------------------------------------------
#define LIBCANILCFG_NUMBER_OF_TX_EVENT_MESSAGE	UINT16_C(0)

// --------------------------------------------------------------------------------------------------------------------
/// \brief number of all tx cycle messages of this module.
// --------------------------------------------------------------------------------------------------------------------
#define LIBCANILCFG_NUMBER_OF_TX_CYCLE_MESSAGE	UINT16_C(0)

// --------------------------------------------------------------------------------------------------------------------
/// \brief number of all rx cycle messages of this module.
// --------------------------------------------------------------------------------------------------------------------
#define LIBCANILCFG_NUMBER_OF_RX_CYCLE_MESSAGE	UINT16_C(0)

// --------------------------------------------------------------------------------------------------------------------
//	Global Data Types
// --------------------------------------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------------------------------------
/// \brief  Enumeration used for CAN message configuration.
// --------------------------------------------------------------------------------------------------------------------
typedef enum 
{
	LIBCANIL_MSG_COMMONTESTRX_NM,
	LIBCANIL_MSG_COMMONTESTTX_NM,
	
	//-----------------------------------------------------------------------------------------------------------------
	LIBCANILCFG_MESSAGE_NAME_DIMENSION,
	LIBCANILCFG_MESSAGE_NAME_NO_MESSAGE
} E_LibCanILCfg_MessageNames_t;



// --------------------------------------------------------------------------------------------------------------------
/// \brief  Enumeration used for CAN signal configuration.
// --------------------------------------------------------------------------------------------------------------------
typedef enum 
{	LIBCANIL_NMSIG_COMMONTESTRX_NODEID,
	LIBCANIL_NMSIG_COMMONTESTRX_CBV_REPEAT_MSG_REQ,
	LIBCANIL_NMSIG_COMMONTESTRX_CBV_COORDINATOR_SLEEP,
	LIBCANIL_NMSIG_COMMONTESTRX_CBV_ACTIVE_WAKEUP,
	LIBCANIL_NMSIG_COMMONTESTRX_CBV_PARTIAL_NETWORK,
	LIBCANIL_NMSIG_COMMONTESTRX_USERDATA0,
	LIBCANIL_NMSIG_COMMONTESTRX_USERDATA1,
	LIBCANIL_NMSIG_COMMONTESTRX_USERDATA2,
	LIBCANIL_NMSIG_COMMONTESTRX_USERDATA3,
	LIBCANIL_NMSIG_COMMONTESTRX_USERDATA4,
	LIBCANIL_NMSIG_COMMONTESTRX_USERDATA5,
/* ----------------------------------------------------- */
	LIBCANIL_NMSIG_COMMONTESTTX_NODEID,
	LIBCANIL_NMSIG_COMMONTESTTX_CBV_REPEAT_MSG_REQ,
	LIBCANIL_NMSIG_COMMONTESTTX_CBV_COORDINATOR_SLEEP,
	LIBCANIL_NMSIG_COMMONTESTTX_CBV_ACTIVE_WAKEUP,
	LIBCANIL_NMSIG_COMMONTESTTX_CBV_PARTIAL_NETWORK,
	LIBCANIL_NMSIG_COMMONTESTTX_USERDATA0,
	LIBCANIL_NMSIG_COMMONTESTTX_USERDATA1,
	LIBCANIL_NMSIG_COMMONTESTTX_USERDATA2,
	LIBCANIL_NMSIG_COMMONTESTTX_USERDATA3,
	LIBCANIL_NMSIG_COMMONTESTTX_USERDATA4,
	LIBCANIL_NMSIG_COMMONTESTTX_USERDATA5,
/* ----------------------------------------------------- */
	LIBCANILCFG_SIGNAL_NAME_DIMENSION,
	LIBCANILCFG_SIGNAL_NAME_NO_SIGNAL
} E_LibCanILCfg_SignalNames_t;

// --------------------------------------------------------------------------------------------------------------------
/// \brief  Enumeration used for change callback configuration.
// --------------------------------------------------------------------------------------------------------------------
typedef enum 
{

//---------------------------------------------------------------------------------------------------------------------
	LIBCANILCFG_CALLBACK_NAME_DIMENSION,
	LIBCANILCFG_CALLBACK_NAME_NO_CALLBACK
} E_LibCanILCfg_CallbackNames_t;


// --------------------------------------------------------------------------------------------------------------------
//	Imported Variables
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
//	Global Function Prototypes
// --------------------------------------------------------------------------------------------------------------------


#endif // LIBCANILCFG_H__INCLUDED


