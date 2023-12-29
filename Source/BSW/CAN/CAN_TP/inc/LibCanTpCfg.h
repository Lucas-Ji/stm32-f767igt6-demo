// --------------------------------------------------------------------------------------------------------------------
///
/// \file LibCanTpCfg.h
///
/// \brief Configuration file for the CanTp implementation
///
///
/// Copyright (c) 2021 Neusoft.
/// All Rights Reserved.
///
// --------------------------------------------------------------------------------------------------------------------



#ifndef LIBCANTPCFG_H__INCLUDED
#define LIBCANTPCFG_H__INCLUDED

// --------------------------------------------------------------------------------------------------------------------
//	Includes
// --------------------------------------------------------------------------------------------------------------------

#include "LibTypes.h"
#include "CanTask.h"

// --------------------------------------------------------------------------------------------------------------------
//	Global Definitions
// --------------------------------------------------------------------------------------------------------------------

#define LIBCANTPCFG_FIXED_FC_PARAM			0

// --------------------------------------------------------------------------------------------------------------------
/// \brief is 11-bit CAN identifier or not
// --------------------------------------------------------------------------------------------------------------------
#define LIBCANTP_DIAG_CANID_11BIT	          true


#define LIBCANTPCFG_FRAMELENGTH               (8U)
#define LIBCANTPCFG_FRAMEPADDING              (0xCCU)

// --------------------------------------------------------------------------------------------------------------------
/// \brief FC Min DLC
// --------------------------------------------------------------------------------------------------------------------
#define LIBCANTP_FC_DLC_MIN	                UINT8_C(3)

// --------------------------------------------------------------------------------------------------------------------
/// \brief Timeout time for N_As and N_Ar
// --------------------------------------------------------------------------------------------------------------------
#define LIBCANTP_TRANSMISSION_TIMEOUT_MS	UINT32_C(250)

// --------------------------------------------------------------------------------------------------------------------
/// \brief Timeout time for N_Bs
// --------------------------------------------------------------------------------------------------------------------
#define LIBCANTP_FLOWCONTROL_TIMEOUT_MS		UINT32_C(300)

// --------------------------------------------------------------------------------------------------------------------
/// \brief Timeout time for N_Cr
// --------------------------------------------------------------------------------------------------------------------
#define LIBCANTP_CONSECUTIVE_TIMEOUT_MS		UINT32_C(500)

// --------------------------------------------------------------------------------------------------------------------
/// \brief Max of SEPARATIONTIMEMINMUM and this value should less than N_Cr
// --------------------------------------------------------------------------------------------------------------------
#define LIBCANTP_SEPARATIONTIMEMINMUM_MAX   UINT32_C(127)



// --------------------------------------------------------------------------------------------------------------------
/// \brief Flow Control Configuration: Block Size
///
/// This parameter controls how many frames are sent (by the sender) inbetween flow control frames (sent by the
/// receiver)
///
/// 0x00 		=> No limit. Sender does not wait for further flow control frames
/// 0x01 - 0xFF	=> Number of frames inbetween further flow control frames
// --------------------------------------------------------------------------------------------------------------------
#define LIBCANTPCFG_FC_PARAM_BS				UINT8_C(0)

// --------------------------------------------------------------------------------------------------------------------
/// \brief Flow Control Configuration: Separation Time minimum
///
/// This parameter controls the minimum time gap inbetween two ConsecutiveFrame frames.
///
/// 0x00 - 0x7F	=> time in milliseconds (0ms - 127ms)
/// 0x80 - 0xF0	=> Reserved
/// 0xF1 - 0xF9 => time in 100 microseconds
/// 0xFA - 0xFF	=> Reserved
// --------------------------------------------------------------------------------------------------------------------
#define LIBCANTPCFG_FC_PARAM_STMIN			UINT8_C(0)

// --------------------------------------------------------------------------------------------------------------------
/// \brief Flow Control Configuration: WaitFrame Time Maximum
// --------------------------------------------------------------------------------------------------------------------
#define LIBCANTPCFG_FC_PARAM_WFTMAX			UINT8_C(0)

// --------------------------------------------------------------------------------------------------------------------
/// \brief Receiver buffer size
///
/// This is for CAN up to 4096 bytes. Higher values result in faster transfer speed (managed by UDS layer)
///
/// It is possible that in CDD are two different buffer sizes given. One for normal communication and one for
/// Flash programming. If this is given the buffer can be much smaller in Application
// --------------------------------------------------------------------------------------------------------------------
#define LIBCANTPCFG_MULTIFRAME_BUFFER_SIZE	UINT32_C(1344)

// --------------------------------------------------------------------------------------------------------------------
/// \brief Addressing scheme of diagnostic
///
/// Possible values:
/// - LIBCANTP_ADDR_SCHEME_NORMAL_ADDRESSING,
/// - LIBCANTP_ADDR_SCHEME_NORMAL_FIXED_ADDRESSING,
/// - LIBCANTP_ADDR_SCHEME_EXTENDED_ADDRESSING,
/// - LIBCANTP_ADDR_SCHEME_MIXED_ADDRESSING,
///
/// \sa E_LibCanTp_AddressingScheme_t
// --------------------------------------------------------------------------------------------------------------------
#define LIBCANTPCFG_ADDRESSING_SCHEME		LIBCANTP_ADDR_SCHEME_NORMAL_FIXED_ADDRESSING

#define LIBCANTPCFG_ECU_PHYS_ADDRESS		0x499U
#define LIBCANTPCFG_ECU_FUNC_ADDRESS		0x704U
#define LIBCANTPCFG_TESTER_PHYS_ADDRESS		0x498U

// --------------------------------------------------------------------------------------------------------------------
/// \brief Declare instances of the Transport Protocol
// --------------------------------------------------------------------------------------------------------------------
#define LIBDRV_DEVID_MCAN 1
#define LIBCANTPCFG_INSTANCES(ENTRY) \
	ENTRY(MCAN, LIBDRV_DEVID_MCAN)


// --------------------------------------------------------------------------------------------------------------------
/// \brief The method to set the event for message request
// --------------------------------------------------------------------------------------------------------------------
#define LIBCANTPCFG_SET_TASKEV_REQ()   (void)LibService_SetEvent(&TASK_CAN, EV_CAN_MSG_REQ)

// --------------------------------------------------------------------------------------------------------------------
//	Global Data Types
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
//	Imported Variables
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
//	Global Function Prototypes
// --------------------------------------------------------------------------------------------------------------------

#endif // LIBCANTPCFG_H__INCLUDED

