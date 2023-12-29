// --------------------------------------------------------------------------------------------------------------------
/// \file LibUdsAssist.h
///
/// \brief Helping functions for the Diagnostic services
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
#include "LibUdsAssist.h"
#include "LibDiagComInt.h"
#include "LibTimer.h"
#include "LibUds.h"

// --------------------------------------------------------------------------------------------------------------------
//	Local Definitions 
// --------------------------------------------------------------------------------------------------------------------

#define LIBUDSASSIST_SRV_EV_SEND_RCRRP	UINT32_C(0x00000001)
#define LIBUDSASSIST_SRV_EV_ABORT_UDS	UINT32_C(0x00000002)

// --------------------------------------------------------------------------------------------------------------------
//	Local Data Types
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
//	Global Variables 
// --------------------------------------------------------------------------------------------------------------------

static void LibUdsAssist_ServiceHndl(void* pData);
S_LibService_Inst_t LibUdsAssist_Service = LIBSERVICE_INIT_SERVICE(LibUdsAssist_ServiceHndl, NULL);

// --------------------------------------------------------------------------------------------------------------------
//	Local Variables
// --------------------------------------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------------------------------------
/// \brief P2 Timer Callback - P2 is responsible to send the Response Pending response out regularly.
/// 
/// \param pData 
/// 
// --------------------------------------------------------------------------------------------------------------------
static void LibUdsAssist_TimerP2Clbk(void* pData);
static S_LibTimer_Inst_t LibUdsAssist_TimerP2 = LIBTIMER_INIT_TIMER(LibUdsAssist_TimerP2Clbk, NULL);

// --------------------------------------------------------------------------------------------------------------------
/// \brief P4 Timer Callback - P4 is monitoring how long the Server is sending Response Pending responses
/// 
/// After triggering of this timer, the processing of a service took to long and the request should be canceled.
/// 
/// \param pData 
// --------------------------------------------------------------------------------------------------------------------
static void LibUdsAssist_TimerP4Clbk(void* pData);
static S_LibTimer_Inst_t LibUdsAssist_TimerP4 = LIBTIMER_INIT_TIMER(LibUdsAssist_TimerP4Clbk, NULL);

// --------------------------------------------------------------------------------------------------------------------
/// \brief Message Buffer for constructing responses
/// 
/// This is a buffer to construct responses that are sent from the UdsAssist module (for example RCR-RP response).
// --------------------------------------------------------------------------------------------------------------------
static uint8_t LibUdsAssist_MessageBuffer[3U] = {0x7F, 0x00, 0x78};

// --------------------------------------------------------------------------------------------------------------------
//	Local Function Prototypes
// --------------------------------------------------------------------------------------------------------------------

static void LibUdsAssist_SendResponsePending(void);

// --------------------------------------------------------------------------------------------------------------------
//	Global Functions
// --------------------------------------------------------------------------------------------------------------------

static void LibUdsAssist_ServiceHndl(void* pData)
{
	LIB_UNUSED(pData);
	// Init
	if (LibService_CheckClearEvent(&LibUdsAssist_Service, LIBSERVICE_EV_INIT))
	{
		LibLog_Info("UdsAssist Service INIT\n");
	}

	// Reinitialize the service
	if (LibService_CheckClearEvent(&LibUdsAssist_Service, LIBSERVICE_EV_RE_INIT))
	{
		LibLog_Info("UdsAssist Service REINIT\n");
	}

	// Reinitialize the service
	if (LibService_CheckClearEvent(&LibUdsAssist_Service, LIBUDSASSIST_SRV_EV_SEND_RCRRP))
	{
		LibLog_Info("UdsAssist Service SEND_RCRRP\n");
		LibUdsAssist_SendResponsePending();
	}

	// Reinitialize the service
	if (LibService_CheckClearEvent(&LibUdsAssist_Service, LIBUDSASSIST_SRV_EV_ABORT_UDS))
	{
		LibLog_Info("UdsAssist Service ABORT_UDS\n");
		LibUdsAssist_StopRespPending();
		LibDiagCom_AbortRequest();
	}

	// Trigger shutdown
	if (LibService_CheckClearEvent(&LibUdsAssist_Service, LIBSERVICE_EV_TRIGGER_SHUTDOWN))
	{
		LibLog_Info("UdsAssist TRIGGER_SHUTDOWN\n");
		LibService_Terminate(&LibUdsAssist_Service);
	}

}

void LibUdsAssist_StartRespPending(void)
{
	LibTimer_Start(&LibUdsAssist_TimerP2, LIBUDSASSIST_P2SERVER_MS, LIBUDSASSIST_P2EX_SERVER_MS);
	LibTimer_Start(&LibUdsAssist_TimerP4, LIBUDSASSIST_P4SERVER_MS, UINT32_C(0));
}

void LibUdsAssist_StopRespPending(void)
{
	LibTimer_Stop(&LibUdsAssist_TimerP2);
	LibTimer_Stop(&LibUdsAssist_TimerP4);
}


// --------------------------------------------------------------------------------------------------------------------
//	Functions
// --------------------------------------------------------------------------------------------------------------------


static void LibUdsAssist_TimerP2Clbk(void* pData)
{
	LIB_UNUSED(pData);
	LibService_SetEvent(&LibUdsAssist_Service, LIBUDSASSIST_SRV_EV_SEND_RCRRP);
}

static void LibUdsAssist_TimerP4Clbk(void* pData)
{
	LIB_UNUSED(pData);
	LibService_SetEvent(&LibUdsAssist_Service, LIBUDSASSIST_SRV_EV_ABORT_UDS);
}

static void LibUdsAssist_SendResponsePending(void)
{
	const bool_t isUdsProcessing = LibUds_IsProcessing();

	if (isUdsProcessing)
	{
		S_LibDiagCom_Msg_t* pComMsg = LibDiagCom_GetMsg();
		S_LibUds_Msg_t* pUdsMsg = LibUds_GetMsg();

		LibUdsAssist_MessageBuffer[1U] = pUdsMsg->pIface->Sid;

		pComMsg->pPayload = LibUdsAssist_MessageBuffer;
		pComMsg->PayloadLen = 3U;
		pComMsg->TgtAddr = pUdsMsg->pIface->TesterSrcAddr;
		pComMsg->MaxPayloadLen = pUdsMsg->pIface->MaxPayloadLen;
		pComMsg->IsPhysical = true;

		LibDiagCom_MsgSendInt(pComMsg);
	}
}
