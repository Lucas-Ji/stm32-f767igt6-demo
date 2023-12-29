// --------------------------------------------------------------------------------------------------------------------
///
/// \file  LibUdsIFaceCfg.h
///
/// \brief Some definitions for the UDS Interface to DiagCom
///
///
///
/// Copyright (c) 2021 Neusoft.
/// All Rights Reserved.
///
// --------------------------------------------------------------------------------------------------------------------

#ifndef LIB_UDS_IFACE_CFG_H__INCLUDED
#define LIB_UDS_IFACE_CFG_H__INCLUDED

#include "LibTypes.h"

// --------------------------------------------------------------------------------------------------------------------
/// \brief configure the buffer sizes which are used to transport the UDS message over the different protocols
///
/// \details There are currently 2 protocols supported :
///		(1) MOST AMS and
///		(2) MOST MEP protocol followed by TCP/IP  followed by THRIFT
/// The implementation is done inside DiagCom service which is running inside MOST TASK
// --------------------------------------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------------------------------------
/// \brief size of the MOST AMS message buffer
/// \detail: for transporting the UDS message the MOST AMS Protocol can be used.
///		  Note: This buffer is allocated from netservices and is used to transport the RX and TX msg. Therefore
///				it must have the size of the biggest UDS RX and/or UDS Tx msg.
///	The definition is set based on the calculation of the LN7SSADIAG_MAX_SSA_RESULT_LENGTH
// --------------------------------------------------------------------------------------------------------------------
#define LIBUDS_MOST_AMS_MAX_MSG_SIZE			UINT16_C(2080)	//!< see requirements  DRECU-III-88  (min size = 4094) and for Bootloader SwDL3-268: (min size is 4095 )

// --------------------------------------------------------------------------------------------------------------------
/// \brief size of the MOST MEP message buffer
/// \detail: for transporting the UDS message the MEP Protocol including TCP/IP header followed by THRIFT header can be used. Here the size of the message buffer is set.
///		  Note: there is only one single buffer! It is used for receiving and transmitting messages! Therefore it must have the size of the biggest UDS RX and/or UDS Tx msg.
// --------------------------------------------------------------------------------------------------------------------
#define LIBUDS_MOST_MEP_MAX_MSG_SIZE			UINT16_C(33 * 1024)	//!< see requirements  xy  )

// --------------------------------------------------------------------------------------------------------------------
/// \brief minimum size of an UDS msg
// --------------------------------------------------------------------------------------------------------------------
#define LIBUDS_MIN_MSG_SIZE						UINT16_C(1)		//!< an UDS msg must have at least 1 byte



// --------------------------------------------------------------------------------------------------------------------
/// \brief P2: Timing value for first ResponseSending [ms]
///
/// \details The requested timing value is 50 ms, but to reach this value we must send the first ResponsePending
/// after 148 ms, normally 150ms but we need about 2ms for handling this message ( details see DoTS2-626 )
// --------------------------------------------------------------------------------------------------------------------
#define LIBUDS_RESPONSE_PENDING_P2				UINT32_C(148)

// --------------------------------------------------------------------------------------------------------------------
/// \brief P2*: Timing value for following ResponseSendings [ms]
// --------------------------------------------------------------------------------------------------------------------
#define LIBUDS_RESPONSE_PENDING_P2_STAR			UINT32_C(5100)

// --------------------------------------------------------------------------------------------------------------------
/// \brief Number of possible ResponsePendings
///
/// \details Values taken from CDD
// --------------------------------------------------------------------------------------------------------------------
#define LIBUDS_MAX_NUM_RESPONSE_PENDING			UINT32_C(30)		// = UINT32_C(60000/2000)



#endif // LIB_UDS_IFACE_CFG_H__INCLUDED

