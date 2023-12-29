// --------------------------------------------------------------------------------------------------------------------
///
/// \file LibUdsIFace.h
///
/// \brief Interface for communication between MOST and UDS
///
/// Copyright (c) 2021 Neusoft.
/// All Rights Reserved.
///
// --------------------------------------------------------------------------------------------------------------------


#ifndef LIB_UDS_IFACE_H__INCLUDED
#define LIB_UDS_IFACE_H__INCLUDED

// --------------------------------------------------------------------------------------------------------------------
//	Includes
// --------------------------------------------------------------------------------------------------------------------
#include "LibUdsIFaceCfg.h"


// --------------------------------------------------------------------------------------------------------------------
//	Global Definitions
// --------------------------------------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------------------------------------
/// \brief Negative response service identifier
// --------------------------------------------------------------------------------------------------------------------
#define LIBUDS_RESPONSE_NEGATIVE					(0x7F)

// --------------------------------------------------------------------------------------------------------------------
/// \brief Negative response code for ResponsePending
// --------------------------------------------------------------------------------------------------------------------
#define LIBUDS_NRC_RESPONSE_PENDING					(0x78)

// --------------------------------------------------------------------------------------------------------------------
/// \brief Increase for a positive reponse
// --------------------------------------------------------------------------------------------------------------------
#define LIBUDS_RESPONSE_POSITIVE_INCREASE			(0x40)

// --------------------------------------------------------------------------------------------------------------------
/// \brief Max length of service to which the confirmation should be routed.
// --------------------------------------------------------------------------------------------------------------------
#define LIBUDS_CONFIRMED_SERVICE_MAX_LENGTH			(5)

// --------------------------------------------------------------------------------------------------------------------
/// \brief UDS service identifier
/// \details Used in LibUdsCfg.c, LibNtg6DiagCom.c.
// --------------------------------------------------------------------------------------------------------------------
#define LIBUDS_SID_SESSION_CONTROL				(0x10)	//!< UDS service DiagnosticSessionControl
#define LIBUDS_SID_ECU_RESET					(0x11)	//!< UDS service ECUReset
#define LIBUDS_SID_CLEAR_DIAG_INFO				(0x14)	//!< UDS service ClearDiagnosticInformation
#define LIBUDS_SID_READ_DTC_INFORMATION			(0x19)	//!< UDS service ReadDTCInformation
#define LIBUDS_SID_READ_DATA_BY_ID				(0x22)	//!< UDS service ReadDataByIdentifier
#define LIBUDS_SID_READ_MEMORY_BY_ADDRESS		(0x23)	//!< UDS service ReadMemoryByAddress
#define LIBUDS_SID_READ_SCALING_DATA_BY_ID		(0x24)	//!< UDS service ReadScalingDataByIdentifier
#define LIBUDS_SID_SECURITY_ACCESS				(0x27)	//!< UDS service SecurityAccess
#define LIBUDS_SID_COMMUNICATION_CONTROL		(0x28)	//!< UDS service CommunicationControl
#define LIBUDS_SID_READ_DATA_BY_PERIODIC_ID		(0x2A)	//!< UDS service ReadDataByPeriodicIdentifier
#define LIBUDS_SID_DYN_DEFINE_DATA_ID			(0x2C)	//!< UDS service DynamicallyDefineDataIdentifier
#define LIBUDS_SID_WRITE_DATA_BY_ID				(0x2E)	//!< UDS service WriteDataByIdentifier
#define LIBUDS_SID_IO_CONTROL_BY_ID				(0x2F)	//!< UDS service InputOutputControlByIdentifier
#define LIBUDS_SID_ROUTINE_CONTROL				(0x31)	//!< UDS service RoutineControl
#define LIBUDS_SID_REQUEST_DOWNLOAD				(0x34)	//!< UDS service RequestDownload
#define LIBUDS_SID_REQUEST_UPLOAD				(0x35)	//!< UDS service RequestUpload
#define LIBUDS_SID_TRANSFER_DATA				(0x36)	//!< UDS service TransferData
#define LIBUDS_SID_REQUEST_TRANSFER_EXIT		(0x37)	//!< UDS service RequestTransferExit
#define LIBUDS_SID_REQUEST_FILE_TRANSFER		(0x38)	//!< UDS service RequestFileTransfer
#define LIBUDS_SID_WRITE_MEMORY_BY_ADDRESS		(0x3D)	//!< UDS service WriteMemoryByAddress
#define LIBUDS_SID_TESTER_PRESENT				(0x3E)	//!< UDS service TesterPresent
#define LIBUDS_SID_ACCESS_TIMING_PARAMETER		(0x83)	//!< UDS service AccessTimingParameter
#define LIBUDS_SID_SECURED_DATA_TRANSMISSION	(0x84)	//!< UDS service SecuredDataTransmission
#define LIBUDS_SID_CONTROL_DTC_SETTING			(0x85)	//!< UDS service ControlDTCSetting
#define LIBUDS_SID_RESPONSE_ON_EVENT			(0x86)	//!< UDS service ResponseOnEvent
#define LIBUDS_SID_LINK_CONTROL					(0x87)	//!< UDS service LinkControl


// --------------------------------------------------------------------------------------------------------------------
//	Global Data Types
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
/// brief To declare the TX confirmation state which is used to get a result about a previously sent message
// --------------------------------------------------------------------------------------------------------------------
typedef enum
{
	LIBUDS_TRANSPORT_PROTOCOL_MOST_AMS	= 0U,		// MOST Application Message Protocol ( = slow )
	LIBUDS_TRANSPORT_PROTOCOL_MOST_MHP,				// MOST High Protocol ( = fast )
	LIBUDS_TRANSPORT_PROTOCOL_MOST_MEP				// MOST MEP Protocol using TCP/IP and Thrift  ( = fast )
} E_LibUds_Transport_Protocol_t;


// --------------------------------------------------------------------------------------------------------------------
/// brief To declare the TX confirmation state which is used to get a result about a previously sent message
// --------------------------------------------------------------------------------------------------------------------
typedef enum
{
	LIBUDS_TX_CONFIRMATION_STATE_NO_REQUEST	= 0U,	// The previously sent message must not be confirmed.
	LIBUDS_TX_CONFIRMATION_STATE_REQUEST,			// Is used by UDS server in case it wants to get the result of a TX procedure for the msg.
	LIBUDS_TX_CONFIRMATION_STATE_RESULT				// Is used by DIAGCOM after the message has been sent out.
} E_LibUds_TxConfirmationState_t;


// --------------------------------------------------------------------------------------------------------------------
/// \brief Type for service functions.
// --------------------------------------------------------------------------------------------------------------------
typedef Ret_t (*LibUds_FuncType_t)(void);

// --------------------------------------------------------------------------------------------------------------------
/// \brief Structure for transferring the UDS request and response between UDS/Diag and DiagCom/MOST.
// --------------------------------------------------------------------------------------------------------------------
typedef struct
{
	// ----------------------------------------------------------------------------------------------------------------
	/// \brief Service identifier
	/// \details Needed by MOST task for creating RP messages during asynchronous UDS service processing; written by MOST task.
	// ----------------------------------------------------------------------------------------------------------------
	uint8_t		Sid;

	// ----------------------------------------------------------------------------------------------------------------
	/// \brief Pointer to the UDS message
	/// \details Used for request and response.
	///			 DiagCom -> UDS (request): contains RX data
	/// 		 UDS -> DiagCom (response): contains TX data
	// ----------------------------------------------------------------------------------------------------------------
	uint8_t*	pPayload;

	// ----------------------------------------------------------------------------------------------------------------
	/// \brief Length of the UDS message
	/// \details Used for request and response.
	///			 DiagCom -> UDS (request): length of RX data
	/// 		 UDS -> DiagCom (response): length of TX data
	// ----------------------------------------------------------------------------------------------------------------
	uint16_t	PayloadLen;

	// ----------------------------------------------------------------------------------------------------------------
	/// \brief Source address of the tester which has sent the request. Written by DiagCom.
	//
	// ----------------------------------------------------------------------------------------------------------------
	uint16_t	TesterSrcAddr;

	// ----------------------------------------------------------------------------------------------------------------
	/// \brief Allowed maximal length of the UDS message
	/// \details Written by DiagCom, used by UDS.
	// ----------------------------------------------------------------------------------------------------------------
	uint16_t	MaxPayloadLen;

	// ----------------------------------------------------------------------------------------------------------------
	/// \brief True if addressing mode of message is physical; false if it's functional; written by MOST task.
	// ----------------------------------------------------------------------------------------------------------------
	bool_t		IsPhysAddr;

	// ----------------------------------------------------------------------------------------------------------------
	/// \brief True if response is positive; false if it's negative; written by UDS.
	// ----------------------------------------------------------------------------------------------------------------
	bool_t		IsPosResponse;

	// ----------------------------------------------------------------------------------------------------------------
	/// \brief True if response shall be suppressed; false if not; written by UDS.
	// ----------------------------------------------------------------------------------------------------------------
	bool_t		SupprResponse;

	// ----------------------------------------------------------------------------------------------------------------
	/// \brief Is NULL in case no Tx confirmation is requested otherwise UDS server is requesting a TX confirmation
	/// and has setup the state to LIBUDS_TX_CONFIRMATION_STATE_REQUEST.
	// ----------------------------------------------------------------------------------------------------------------
	struct S_LibUds_TxConf* pTxConf;
} S_LibUds_IfaceCfg_t;

// --------------------------------------------------------------------------------------------------------------------
/// \brief Structure for transferring the Tx confirmation from the DiagCom to the Diag task.
// --------------------------------------------------------------------------------------------------------------------
typedef struct S_LibUds_TxConf
{
	// ----------------------------------------------------------------------------------------------------------------
	/// \brief Is used by UDS server and DiagCom to indentify the state of the Tx confirmation written and read by both sides
	///
	/// \details There are 3 states involved :
	///	- initialized to LIBUDS_TX_CONFIRMATION_STATE_NO_REQUEST
	///	- UDS server wants a TXConfirmation : it simply creates a message and sets TxConfirmationState =
	/// 	LIBUDS_TX_CONFIRMATION_STATE_REQUEST.
	///	- DIAGCOM sends out this msg and after it has got TX result: it fills the TxConfirmationContext with data
	/// 	and sets state to = LIBUDS_TX_CONFIRMATION_STATE_RESULT.
	// ----------------------------------------------------------------------------------------------------------------
	E_LibUds_TxConfirmationState_t		State;

	// ----------------------------------------------------------------------------------------------------------------
	/// \brief Is set to true in case the UDS server wants to shutdown the communication interface ( now Mhp or AMS )
	/// after last transmission.
	// ----------------------------------------------------------------------------------------------------------------
	bool_t		ShutdownComInterface;
	
	// Note: All following values are set by DiagCom and read by Diag task's UDS server.

	// ----------------------------------------------------------------------------------------------------------------
	/// \brief Transmission result: true if the response has been sent successfully else false.
	// ----------------------------------------------------------------------------------------------------------------
	bool_t		RespTxResult;

	// ----------------------------------------------------------------------------------------------------------------
	/// \brief Diagnostic tester address to which this message has been sent to.
	// ----------------------------------------------------------------------------------------------------------------
	uint16_t	DiagTesterAdr;

	// ----------------------------------------------------------------------------------------------------------------
	/// \brief MOST node address to which this message has been sent to.
	// ----------------------------------------------------------------------------------------------------------------
	uint16_t	MostNodeAdr;

	// ----------------------------------------------------------------------------------------------------------------
	/// \brief The diag instance of Fblock DIAG to which this message has been sent.
	// ----------------------------------------------------------------------------------------------------------------
	uint8_t		FblockDiagInst;

	// ----------------------------------------------------------------------------------------------------------------
	/// \brief The protocol used to to send this message.
	// ----------------------------------------------------------------------------------------------------------------
	E_LibUds_Transport_Protocol_t 	Protocol;

	// ----------------------------------------------------------------------------------------------------------------
	/// \brief Pointer to the last handled service
	///
	/// \details Can be NULL if not needed by the confirmation function pConfirm.
	// ----------------------------------------------------------------------------------------------------------------
	const struct S_LibUds_SvcCfg*	pSvcCfg;

	// ----------------------------------------------------------------------------------------------------------------
	/// \brief Pointer to the last handled subservice
	///
	/// \details Can be NULL if not needed by the confirmation function pConfirm.
	// ----------------------------------------------------------------------------------------------------------------
	const struct S_LibUds_SubsvcCfg* pSubsvcCfg;

	// ----------------------------------------------------------------------------------------------------------------
	/// \brief Pointer to the function which must be executed after TX Confirmation from DiagCom.
	// ----------------------------------------------------------------------------------------------------------------
	LibUds_FuncType_t			pConfirm;

} S_LibUds_TxConf_t;


// --------------------------------------------------------------------------------------------------------------------
//	Imported Variables
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
//	Global Function Prototypes
// --------------------------------------------------------------------------------------------------------------------


#endif // LIB_UDS_IFACE_H__INCLUDED

