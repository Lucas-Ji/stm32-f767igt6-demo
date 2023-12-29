// --------------------------------------------------------------------------------------------------------------------
/// \file LibDiagCom.h
///
/// \brief Interface for the Diagnostic / UDS Layer
///
///
///
/// Copyright (c) 2021 Neusoft.
/// All Rights Reserved.
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
//	Local Definitions to control properties inside header files
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
//	Includes
// --------------------------------------------------------------------------------------------------------------------
#include "LibDiagCom.h"
#include "LibDiagComInt.h"

#include "LibCanTp.h"
#include "LibUdsAssist.h"
#include "LibUds.h"
#include "string.h"

// --------------------------------------------------------------------------------------------------------------------
//	Local Definitions
// --------------------------------------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------------------------------------
//	Local Data Types
// --------------------------------------------------------------------------------------------------------------------

typedef enum E_LibDiagCom_State_t {
	LIBDIAGCOM_STATE_UNUSED,			//!< DiagCom Interface currently unused
	LIBDIAGCOM_STATE_PROCESSING,		//!< currently processing
	LIBDIAGCOM_STATE_SENDING,			//!< currently sending
	LIBDIAGCOM_STATE_SENDING_CONFIRM	//!< currently waiting for confirmation
} E_LibDiagCom_State_t;

typedef struct S_LibDiagCom_Tracker_t {
	E_LibDiagCom_State_t 	State;		//!< Current State of DiagCom Interface
	S_LibUds_IfaceCfg_t* 	pUdsIface;	//!< Pointer to UDS Iface structure
	S_LibDiagCom_Msg_t* 	pComMsg;	//!< Pointer to DiagCom message structure
} S_LibDiagCom_Tracker_t;

// --------------------------------------------------------------------------------------------------------------------
//	Global Variables
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
//	Local Variables
// --------------------------------------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------------------------------------
/// \brief Global tracker state of the one instance
// --------------------------------------------------------------------------------------------------------------------
static S_LibDiagCom_Tracker_t LibDiagCom_Tracker = {
	.State 		= LIBDIAGCOM_STATE_UNUSED,
	.pUdsIface 	= NULL,
	.pComMsg 	= NULL
};

// --------------------------------------------------------------------------------------------------------------------
/// \brief Global instance of the UDS iface
// --------------------------------------------------------------------------------------------------------------------
static S_LibUds_IfaceCfg_t LibDiagCom_UdsIface;

// --------------------------------------------------------------------------------------------------------------------
/// \brief Global DiagCom message instance
// --------------------------------------------------------------------------------------------------------------------
static S_LibDiagCom_Msg_t LibDiagCom_ComMsg;

// --------------------------------------------------------------------------------------------------------------------
//	Local Function Prototypes
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
//	Global Functions
// --------------------------------------------------------------------------------------------------------------------

// ====================================================================================================================
// LibDiagCom_IsReady:
// ====================================================================================================================
bool_t LibDiagCom_IsReady(void)
{
	bool_t ret = true;

	if (LIBDIAGCOM_STATE_UNUSED != LibDiagCom_Tracker.State)
	{
		LibLog_Warning("LibDiagCom_IsReady: DiagCom is busy...\n");
		ret = false;
	}

	return ret;
}

// ====================================================================================================================
// LibDiagCom_MsgReceived:
// ====================================================================================================================
void LibDiagCom_MsgReceived(S_LibDiagCom_Msg_t* pMsg)
{
#if 1
	Ret_t ret;

	LibDiagCom_Tracker.State = LIBDIAGCOM_STATE_PROCESSING;
	LibDiagCom_Tracker.pComMsg = pMsg;

	// fill information of UDS I/f
	LibDiagCom_UdsIface.Sid = pMsg->pPayload[0];
	LibDiagCom_UdsIface.pPayload = pMsg->pPayload;
	LibDiagCom_UdsIface.PayloadLen = pMsg->PayloadLen;
	LibDiagCom_UdsIface.IsPhysAddr = pMsg->IsPhysical;
	LibDiagCom_UdsIface.MaxPayloadLen = pMsg->MaxPayloadLen;
	LibDiagCom_UdsIface.TesterSrcAddr = pMsg->SrcAddr;

	LibDiagCom_Tracker.pUdsIface = &LibDiagCom_UdsIface;

	LibLog_Debug("DiagCom Message Received PayloadLen [%d]\n", LibDiagCom_Tracker.pComMsg->PayloadLen);

	ret = LibUds_ReqVerify(LibDiagCom_Tracker.pUdsIface);

	if (LIBRET_PENDING == ret)
	{
		// this request is running asynchronous
		// start attempting to send a RCR-RP response code
		// = NRC 0x78
		LibUdsAssist_StartRespPending();
		//SetEvent(TASK_DIAG, LN7DIAG_EV_REQ_PROCESS);
	}

#else // ORIGINAL
	const bool_t isUdsBusy = LibUds_IsProcessing();


	if ((isUdsBusy)
	 || (LIBDIAGCOM_STATE_UNUSED != LibDiagCom_Tracker.State))
	{
		if (isUdsBusy)
		{
			// send busy repeat request response
			pMsg->pPayload[0U] = LIBUDS_RESPONSE_NEGATIVE;
			pMsg->pPayload[1U] = pMsg->pPayload[0U];
			pMsg->pPayload[2U] = LIBUDS_NRC_NRC_BUSY_REPEAT_REQUEST;
			pMsg->PayloadLen = 3U;
			pMsg->TgtAddr = pMsg->SrcAddr;
			pMsg->MaxPayloadLen = 3U;
			pMsg->IsPhysical = true;

			LibDiagCom_MsgSendInt(pMsg);
		} else {
			LibDiagCom_Tracker.State = LIBDIAGCOM_STATE_UNUSED;
			LibDiagCom_MsgReceived(pMsg);
		}
	}
	else
	{
		Ret_t ret;

		LibDiagCom_Tracker.State = LIBDIAGCOM_STATE_PROCESSING;
		LibDiagCom_Tracker.pComMsg = pMsg;

		// fill information of UDS I/f
		LibDiagCom_UdsIface.Sid = pMsg->pPayload[0];
		LibDiagCom_UdsIface.pPayload = pMsg->pPayload;
		LibDiagCom_UdsIface.PayloadLen = pMsg->PayloadLen;
		LibDiagCom_UdsIface.IsPhysAddr = pMsg->IsPhysical;
		LibDiagCom_UdsIface.MaxPayloadLen = pMsg->MaxPayloadLen;
		LibDiagCom_UdsIface.TesterSrcAddr = pMsg->SrcAddr;

		LibDiagCom_Tracker.pUdsIface = &LibDiagCom_UdsIface;

		LibLog_Debug("DiagCom Message Received %[]X Phys=%b", LibDiagCom_Tracker.pComMsg->PayloadLen, LibDiagCom_Tracker.pComMsg->pPayload, pMsg->IsPhysical);

		ret = LibUds_ReqVerify(LibDiagCom_Tracker.pUdsIface);

		if (LIBRET_PENDING == ret)
		{
			// this request is running asynchronous
			// start attempting to send a RCR-RP response code
			// = NRC 0x78
			LibUdsAssist_StartRespPending();
			SetEvent(TASK_DIAG, LIBUDS_EV_REQ_PROCESS);
		}
	}
#endif
}

void LibDiagCom_StartOfMsg(void)
{
	// TODO: Data_SOM
}

void LibDiagCom_MsgConfirm(void)
{
	// TODO: confirmation... check how this is done together with the UdsIface TxConf
}

void LibDiagCom_MsgSend(S_LibUds_IfaceCfg_t* pMsg)
{
#ifdef LIBUDS_PAD_TO_8B
	const uint8_t lastFrameSize = (pMsg->PayloadLen <= 7U) ? pMsg->PayloadLen : ((pMsg->PayloadLen - 6) % 7);
	if (lastFrameSize != 7U)
	{
		const uint8_t paddingLength = 7U - (lastFrameSize % 7U);
		(void)memset(&pMsg->pPayload[pMsg->PayloadLen], LIBUDS_PADDED_BYTE_VALUE, paddingLength);
		pMsg->PayloadLen += paddingLength;
		//LibLog_Info("lastFrameSize != 7U: lastFrameSize=%d | paddingLength=%d | buff: %[]X", lastFrameSize, paddingLength, pMsg->PayloadLen, pMsg->pPayload);
	}


//	const uint8_t availableMessageBytes = (pMsg->PayloadLen <= 7U) ? 7U : 6U;
//	const uint8_t paddingLength = availableMessageBytes - (pMsg->PayloadLen % availableMessageBytes);
//	if (paddingLength != availableMessageBytes)
//	{
//		(void)memset(&pMsg->pPayload[pMsg->PayloadLen], LIBUDS_PADDED_BYTE_VALUE, paddingLength);
//		pMsg->PayloadLen += paddingLength;
//	}
#endif
	S_LibDiagCom_Msg_t* pComMsg = LibDiagCom_Tracker.pComMsg;

	// stop the response pending timers
	LibUdsAssist_StopRespPending();

	LibDiagCom_Tracker.State = LIBDIAGCOM_STATE_UNUSED;

	LibDiagCom_Tracker.pUdsIface = pMsg;
	// copy fields from UDS message to Com message
	pComMsg->pPayload = pMsg->pPayload;
	pComMsg->PayloadLen = pMsg->PayloadLen;
	pComMsg->TgtAddr = pMsg->TesterSrcAddr;
	pComMsg->MaxPayloadLen = pMsg->MaxPayloadLen;



	// ISO 14229-1 6.4.1.4
	// Service responses (and service confirmations) shall always use physical addressing.
	// Note: Not implemented by the UDS layer?
	pComMsg->IsPhysical = true;

	if (pMsg->SupprResponse)
	 //&& (pMsg->IsPosResponse))
	{
		LibLog_Debug("DiagCom Message Suppressed\n");
	}
	else
	{
		LibLog_Debug("DiagCom Message Send PayloadLen [%d]\n", pComMsg->PayloadLen);

		LibDiagCom_MsgSendInt(pComMsg);
	}
}

void LibDiagCom_MsgSendInt(S_LibDiagCom_Msg_t* pMsg)
{
	// The interface which was sending the request left a callback
	// which shall be called to send the response for a request
	if (NULL != pMsg->SendMessage)
	{
		pMsg->SendMessage(pMsg);
	}
}

extern void LibDiagCom_AbortRequest(void)
{
	const Ret_t isAborted = LibUds_ReqAbort();
	LibDiagCom_Tracker.State = LIBDIAGCOM_STATE_UNUSED;
	if (LIBRET_OK != isAborted)
	{
		LibLog_Warning("UdsAssist Request was not aborted\n");
	}
	else
	{
		LibLog_Debug("UdsAssist is aborted\n");
	}
	LibUds_Init();
}

S_LibDiagCom_Msg_t* LibDiagCom_GetMsg(void)
{
	return &LibDiagCom_ComMsg;
}

void LibDiagCom_RequestProcessed(void)
{
	LibDiagCom_MsgSend(LibDiagCom_Tracker.pUdsIface);
	LibDiagCom_Tracker.State = LIBDIAGCOM_STATE_UNUSED;
}

void LibDiagCom_Error(const E_LibDiagCom_Error_t error)
{
	// PAY-ATTENTION: this function is called by LibCanTpInternal.c from TimerTimeout callback.
	// LibLog is forbidden from interrupt context and should result in a failed assertion in debug
	// builds and in undefined behaviour for release builds
	// TODO manage it in different safe-way
	// LibLog_Warning("DiagCom: Received Error 0x%X", (uint32_t)error);
	// LibDiagCom_AbortRequest();
	LibService_SetEvent(&LibCanTp_Service, LIBCANTP_SRV_EV_COMERR);
}

// --------------------------------------------------------------------------------------------------------------------
//	Functions
// --------------------------------------------------------------------------------------------------------------------




