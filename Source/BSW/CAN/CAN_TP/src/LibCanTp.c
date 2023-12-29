// --------------------------------------------------------------------------------------------------------------------
/// \file LibCanTp
///
/// \brief This module is the implementation of the CAN Transport Protocol
///
/// It implements the ISO 15765-2 (IsoTp) standard
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
#include "LibCanTp.h"

#include "LibCanTpFsm.h"
#include "LibCanTpInternal.h"

//#include "LibNtg7Diag.h"
//#include "LibNtg7DiagIFace.h"

//#include "LibAssert.h"
#include "LibDiagCom.h"
#include "LibDiagComInt.h"
//#include "LibDriver.h"
#include "LibFifoQueue.h"
//#include "LibLog.h"
#include <string.h>

// --------------------------------------------------------------------------------------------------------------------
//	Asserts for not yet implemented features
// --------------------------------------------------------------------------------------------------------------------

// Checks for features that are not implemented yet
//Lib_AssertStatic(LIBCANTPCFG_FC_PARAM_BS == 0U);
//Lib_AssertStatic(LIBCANTPCFG_FC_PARAM_WFTMAX == 0U);
//Lib_AssertStatic(LIBCANTPCFG_FC_PARAM_STMIN == 0U);
//Lib_AssertStatic(LIBCANTPCFG_ADDRESSING_SCHEME == LIBCANTP_ADDR_SCHEME_NORMAL_FIXED_ADDRESSING);

// --------------------------------------------------------------------------------------------------------------------
//	Local Definitions
// --------------------------------------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------------------------------------
/// \brief Declare the local instance for CanTP data unit (used by the CanTp_Inst)
// --------------------------------------------------------------------------------------------------------------------
#define LIBCANTP_DECL_DATA_UNIT(name, ...) static S_LibCanTp_DataUnit_t LibCanTp_DataUnit_##name;

// --------------------------------------------------------------------------------------------------------------------
/// \brief Declare the local instance for the CAN Frame (used by the CanTp_Inst)
// --------------------------------------------------------------------------------------------------------------------
#define LIBCANTP_DECL_CAN_MESSAGE(name, ...) static S_LibCan_Msg_t LibCanTp_CanMsg_##name;

// --------------------------------------------------------------------------------------------------------------------
/// \brief Declare the CanTP instance
// --------------------------------------------------------------------------------------------------------------------
#define LIBCANTP_DECL_INSTANCE(name, devid)                                                                                  \
	LIBCANTP_DECL_CAN_MESSAGE(name, devid)                                                                                   \
																												             \
	LIBCANTP_DECL_DATA_UNIT(name, devid)                                                                                     \
																												             \
	static S_LibCanTp_Inst_t LibCanTp_Inst_##name = {                                                                        \
		.DevId = devid,                                                                                                      \
		.FlowCtrlCfg =                                                                                                       \
			{                                                                                                                \
				.BlockSize  = LIBCANTPCFG_FC_PARAM_BS,                                                                       \
				.SepTimeMin = LIBCANTPCFG_FC_PARAM_STMIN,                                                                    \
			},                                                                                                               \
		.FlowCtrlSts = {0U, 0U, 0U, 0U},                                                                                     \
		.pDataUnit   = &LibCanTp_DataUnit_##name,                                                                            \
		.pCanMsg     = &LibCanTp_CanMsg_##name,                                                                              \
		.Timers = {																									         \
			.TransmissionTimer = LIBTIMER_INIT_TIMER(LibCanTp_TransmissionTimerTimeout, &LibCanTp_Inst_##name), 	         \
			.FlowControlTimer  = LIBTIMER_INIT_TIMER(LibCanTp_FlowControlTimerTimeout, &LibCanTp_Inst_##name),  	         \
			.ConsecutiveTimer  = LIBTIMER_INIT_TIMER(LibCanTp_ConsecutiveTimerTimeout, &LibCanTp_Inst_##name), 		         \
			.SeparationTimeMinTimer  = LIBTIMER_INIT_TIMER(LibCanTp_SeparationTimeMinTimerTimeout, &LibCanTp_Inst_##name)    \
		} 																											         \
	};

// --------------------------------------------------------------------------------------------------------------------
/// \brief Expand the CanTP instance into an table
// --------------------------------------------------------------------------------------------------------------------
#define LIBCANTP_EXPAND_TABLE(name, ...) &LibCanTp_Inst_##name,

// --------------------------------------------------------------------------------------------------------------------
/// \brief Expand the CanTP instances into an enum
// --------------------------------------------------------------------------------------------------------------------
#define LIBCANTP_EXPAND_ENUM(name, ...) LIBCANTP_INST_##name,

// --------------------------------------------------------------------------------------------------------------------
//	Local Data Types
// --------------------------------------------------------------------------------------------------------------------

/// \brief  The flag is the message transmit enabled.
// ----------------------------------------------------------------------------------------------------------------
static bool_t LibCanTP_TransmitEnabled = false;

// --------------------------------------------------------------------------------------------------------------------
/// \brief Enumeration with names of the CanTP instances including a value representing the number of instances in the
/// table
// --------------------------------------------------------------------------------------------------------------------
typedef enum E_LibCanTp_InstName_t
{
	LIBCANTPCFG_INSTANCES(LIBCANTP_EXPAND_ENUM) 
	LIBCANTP_INST_COUNT
} E_LibCanTp_InstName_t;

// --------------------------------------------------------------------------------------------------------------------
//	Local Function Prototypes
// --------------------------------------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------------------------------------
/// \brief Service Handler for the CAN Transport Protocol
///
/// \param pData
/// User data. Not used
// --------------------------------------------------------------------------------------------------------------------
static void LibCanTp_ServiceHndl(void *pData);

// --------------------------------------------------------------------------------------------------------------------
/// \brief Handle a received CAN frame
///
/// \param pBufEntry
/// entry which is fetched from the FIFO
// --------------------------------------------------------------------------------------------------------------------
static void LibCanTp_HandleRxFrame(const S_LibCanTp_MsgIndBufferEntry_t *pBufEntry);

// --------------------------------------------------------------------------------------------------------------------
/// \brief Handle a confirm signal (successful sending)
///
/// \param msgId
/// message ID for which the confirm arrived
// --------------------------------------------------------------------------------------------------------------------
static void LibCanTp_HandleConfirm(const uint32_t msgId);

// --------------------------------------------------------------------------------------------------------------------
//	Global Variables
// --------------------------------------------------------------------------------------------------------------------

/// \brief  The flag is the message receive enabled.
// ----------------------------------------------------------------------------------------------------------------
bool_t LibCanTP_ReceiveEnabled = false;


// --------------------------------------------------------------------------------------------------------------------
/// \brief Service Instance of the CAN Transport Protocol
///
/// Initialize the service with the Service handler. No data is required.
///
/// \sa LibCanIl_ServiceHndl
// --------------------------------------------------------------------------------------------------------------------
S_LibService_Inst_t LibCanTp_Service = LIBSERVICE_INIT_SERVICE(LibCanTp_ServiceHndl, NULL);

// --------------------------------------------------------------------------------------------------------------------
/// \brief Interface description of the CAN Transport Protocol Module
///
/// This interface may be used in the future to provide a generic interface for the CAN TASK
// --------------------------------------------------------------------------------------------------------------------
const S_LibCanModule_Module_t LibCanTp_Module = {
	.IsMsg = LibCanTp_IsMsgTp, 
	.MsgIndicate = LibCanTp_MsgIndicate, 
	.MsgConfirm = LibCanTp_MsgConfirm
};

// --------------------------------------------------------------------------------------------------------------------
//	Local Variables
// --------------------------------------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------------------------------------
///	\brief  FIFO buffer for Transport Protocol message transfer
// --------------------------------------------------------------------------------------------------------------------
static S_LibCanTp_MsgReqBufferEntry_t LibCanTp_MsgReqBuffer[LIBCANTP_MSG_REQ_FIFO_ELEMENTS];
static S_LibCanTp_MsgIndBufferEntry_t LibCanTp_MsgIndBuffer[LIBCANTP_MSG_IND_FIFO_ELEMENTS];
static S_LibCanTp_MsgConBufferEntry_t LibCanTp_MsgConBuffer[LIBCANTP_MSG_CON_FIFO_ELEMENTS];

LIBCANTPCFG_INSTANCES(LIBCANTP_DECL_INSTANCE)

S_LibCanTp_Inst_t *LibCanTp_Inst_Table[] = {LIBCANTPCFG_INSTANCES(LIBCANTP_EXPAND_TABLE)};

// --------------------------------------------------------------------------------------------------------------------
///	\brief Settings for the FIFO
// --------------------------------------------------------------------------------------------------------------------
LIBFIFO_DEFINE_INST(LibCanTp_MsgReqFifo, (uint32_t *)(void *)LibCanTp_MsgReqBuffer,
					sizeof(S_LibCanTp_MsgReqBufferEntry_t), (uint32_t)LIBCANTP_MSG_REQ_FIFO_ELEMENTS, false);

LIBFIFO_DEFINE_INST(LibCanTp_MsgIndFifo, (uint32_t *)(void *)LibCanTp_MsgIndBuffer,
					sizeof(S_LibCanTp_MsgIndBufferEntry_t), (uint32_t)LIBCANTP_MSG_IND_FIFO_ELEMENTS, false);

LIBFIFO_DEFINE_INST(LibCanTp_MsgConFifo, (uint32_t *)(void *)LibCanTp_MsgConBuffer,
					sizeof(S_LibCanTp_MsgConBufferEntry_t), (uint32_t)LIBCANTP_MSG_CON_FIFO_ELEMENTS, false);

// --------------------------------------------------------------------------------------------------------------------
//	Global Function
// --------------------------------------------------------------------------------------------------------------------

//=====================================================================================================================
// LibCanTp_MsgIndicate:
//=====================================================================================================================
void LibCanTp_MsgIndicate(S_LibCan_Msg_t *pMsg)
{
	const S_LibCanTp_BufferEntry_t bufEntry =  *pMsg;
	Ret_t pushed = LibFifoQueue_Push(&LibCanTp_MsgIndFifo, (void *)&bufEntry);
	if (!pushed)
	{
		LibLog_Warning("CAN:TP push not possible");
		LibFifoQueue_Clear(&LibCanTp_MsgIndFifo);
	}

	(void)LibService_SetEvent(&LibCanTp_Service, LIBCANTP_SRV_EV_IND);
}

//=====================================================================================================================
// LibCanTp_MsgConfirm:
//=====================================================================================================================
void LibCanTp_MsgConfirm(uint32_t msgId)
{
	(void)LibFifoQueue_Push(&LibCanTp_MsgConFifo, (void *)&msgId);
	(void)LibService_SetEvent(&LibCanTp_Service, LIBCANTP_SRV_EV_CON);
}

//=====================================================================================================================
// LibCanTp_MsgRequest:
//=====================================================================================================================
void LibCanTp_MsgRequest(S_LibDiagCom_Msg_t *pMsg)
{
	uint8_t i;
	S_LibCanTp_Inst_t *pInst = NULL;

	for (i = 0; i < LIBCANTP_INST_COUNT; i++)
	{
		pInst = LibCanTp_Inst_Table[i];
		if (pInst->DevId == pMsg->DevId)
		{
			pInst->pDataUnit->SourceAddress       = LIBCANTPCFG_ECU_PHYS_ADDRESS;
			pInst->pDataUnit->TargetAddress       = pMsg->TgtAddr;
			pInst->pDataUnit->IsPhysical          = pMsg->IsPhysical;
			pInst->pDataUnit->Length              = pMsg->PayloadLen;
			pInst->pDataUnit->BufferDataRemaining = pMsg->PayloadLen;
			pInst->pDataUnit->BufferRWIdx         = 0U;
			pInst->pDataUnit->IsFinished          = false;
			memcpy(pInst->pDataUnit->Buffer, pMsg->pPayload, pMsg->PayloadLen);
			LibLog_Debug("CAN:TP MsgRequest Length [%d]\n", pInst->pDataUnit->Length);
			LibService_SetEvent(&LibCanTp_Service, LIBCANTP_SRV_EV_REQ);

			// Start timer
			// LibTimer_Start(&pInst->Timers.TransmissionTimer, LIBCANTP_TRANSMISSION_TIMEOUT_MS, UINT32_C(0));
			break;
		}
	}
}

// --------------------------------------------------------------------------------------------------------------------
//	Functions
// --------------------------------------------------------------------------------------------------------------------

//=====================================================================================================================
// LibCanTp_ServiceHndl:
//=====================================================================================================================
static void LibCanTp_ServiceHndl(void *pData)
{
	// Initialization method
	if (LibService_CheckClearEvent(&LibCanTp_Service, LIBSERVICE_EV_INIT))
	{
		uint8_t i;
		for (i = 0; i < (sizeof(LibCanTp_Inst_Table) / sizeof(LibCanTp_Inst_Table[0])); i++)
		{
			LibCanTp_Inst_Table[i]->Fsm           = LibCanTpInt_Fsm;
			LibCanTp_Inst_Table[i]->Fsm.pUserData = LibCanTp_Inst_Table[i];

			LibCanTpFsm_TriggerInit(LibCanTp_Inst_Table[i]);
		}
	}

	// Reinitialize the service
	if (LibService_CheckClearEvent(&LibCanTp_Service, LIBSERVICE_EV_RE_INIT))
	{
		LibLog_Debug("CAN:TP Service REINIT");
		for (uint8_t i = 0; i < (sizeof(LibCanTp_Inst_Table) / sizeof(LibCanTp_Inst_Table[0])); i++)
		{
			LibCanTpFsm_TriggerInit(LibCanTp_Inst_Table[i]);
		}
	}

	// LIBCANTP_SRV_EV_COMERR
	if (LibService_CheckClearEvent(&LibCanTp_Service, LIBCANTP_SRV_EV_COMERR))
	{
		LibDiagCom_AbortRequest();
	}


	if (LibService_CheckClearEvent(&LibCanTp_Service, LIBCANTP_SRV_EV_REQ))
	{
		LibCanTpFsm_TriggerSend(LibCanTp_Inst_Table[0U]);
	}

	// receive messages from CAN transceiver
	if (LibService_CheckClearEvent(&LibCanTp_Service, LIBCANTP_SRV_EV_IND))
	{
		S_LibCanTp_MsgIndBufferEntry_t *pMsg;

		do
		{
			pMsg = (S_LibCanTp_MsgIndBufferEntry_t *)LibFifoQueue_GetItem(&LibCanTp_MsgIndFifo, UINT32_C(0));

			if (pMsg != NULL)
			{
				LibCanTp_HandleRxFrame(pMsg);
			}
			LibFifoQueue_Pop(&LibCanTp_MsgIndFifo);
			
		} while (pMsg != NULL);
	}

	// confirm transmitted massages from CAN transceiver
	if (LibService_CheckClearEvent(&LibCanTp_Service, LIBCANTP_SRV_EV_CON))
	{
		S_LibCanTp_MsgConBufferEntry_t *pMsgId;

		do
		{
			pMsgId = (S_LibCanTp_MsgConBufferEntry_t *)LibFifoQueue_GetItem(&LibCanTp_MsgConFifo, 0U);
			if (pMsgId != NULL)
			{
				LibCanTp_HandleConfirm(*pMsgId);
				LibFifoQueue_Pop(&LibCanTp_MsgConFifo);
			}
		} while (pMsgId != NULL);
	}

	// Shutdown/destruct the service
	if (LibService_CheckClearEvent(&LibCanTp_Service, LIBSERVICE_EV_TRIGGER_SHUTDOWN))
	{
		LibLog_Debug("CAN:TP Shutdown\n");
		LibService_Terminate(&LibCanTp_Service);
	}
}

Ret_t LibCanTp_HandleTxMessage(S_LibCanTp_Inst_t *pInst)
{
	Ret_t retVal            = LIBRET_OK; // LibCanTp_SetupResponse(&msg, pMsg);
	if(LibCanTP_TransmitEnabled == true)
	{
	  S_LibCan_Msg_t *pCanMsg = pInst->pCanMsg;
	  // not supported yet
	  pCanMsg->IsBrs = false;
	  pCanMsg->IsCanFd = false;
	  #if defined(LIBCANTPCFG_FRAMELENGTH) && defined(LIBCANTPCFG_FRAMEPADDING)
	  // padding
	  const uint8_t length = LibCan_GetMsgDataLength(pCanMsg->Length);
	  const uint8_t neededLength = LibCan_GetMsgDataLength((E_LibCan_DlcSize_t)LIBCANTPCFG_FRAMELENGTH);
	  if (length < neededLength)
	  {
		for (uint8_t idx = length; idx < neededLength; ++idx)
		{
			pCanMsg->Data[idx] = (uint8_t)LIBCANTPCFG_FRAMEPADDING;
		}
		pCanMsg->Length = (E_LibCan_DlcSize_t)LIBCANTPCFG_FRAMELENGTH;
	  }
	  #endif

	  S_LibCanTp_MsgReqBufferEntry_t bufEntry = *pCanMsg;

	  LibFifoQueue_Push(&LibCanTp_MsgReqFifo, &bufEntry);
	  LIBCANTPCFG_SET_TASKEV_REQ();

		if (pInst->pDataUnit->BufferDataRemaining == 0U)
		{
			pInst->pDataUnit->IsFinished = true;
			pInst->pDataUnit->IsTxMultiFrame = false;
		}
		

		const E_LibCanTp_FrameType_t messageType = LibCanTpInt_ParseFrameTypeRx(pCanMsg);
		if( LIBCANTP_FRAMETYPE_FIRSTFRAME == messageType ) //Send First Frame and Wait for Receive FlowControl Frame.
		{
			//Restart timer N_Bs
			LibTimer_Stop(&pInst->Timers.FlowControlTimer);
			LibTimer_Start(&pInst->Timers.FlowControlTimer, LIBCANTP_FLOWCONTROL_TIMEOUT_MS, UINT32_C(0));
		}
		else if(LIBCANTP_FRAMETYPE_FLOWCONTROL == messageType) //Send FlowContol Frame and Wait for Consecutive Frame.
		{
			//Restart timer N_Cr
			LibTimer_Stop(&pInst->Timers.ConsecutiveTimer);
			LibTimer_Start(&pInst->Timers.ConsecutiveTimer, LIBCANTP_CONSECUTIVE_TIMEOUT_MS, UINT32_C(0));
		}

		//Restart timer N_As
		LibTimer_Stop(&pInst->Timers.TransmissionTimer);
	    LibTimer_Start(&pInst->Timers.TransmissionTimer, LIBCANTP_TRANSMISSION_TIMEOUT_MS, UINT32_C(0));

		LibLog_Debug("CAN:TP TX %[]X", (uint8_t)pCanMsg->Length, pCanMsg->Data);

	  #if defined(LIBCANTPCFG_IMMEDIATE_CONFIRM)
	  LibCanTp_HandleConfirm(pCanMsg->Id);
	  #endif
	}
	return retVal;
}

static void LibCanTp_HandleConfirm(const uint32_t msgId)
{
	LIB_UNUSED(msgId);
	S_LibCanTp_Inst_t *pInst = LibCanTp_Inst_Table[0];

	LibTimer_Stop(&pInst->Timers.TransmissionTimer); //Stop timer N_As

	if ((!pInst->pDataUnit->IsFinished)
		&& ((0U == pInst->FlowCtrlSts.BlockSize) || (0U < pInst->FlowCtrlSts.BlockSizeRemaining)))
	{
		if((pInst->FlowCtrlSts.SeparationTimeMinimum > 0)
			&& (pInst->FlowCtrlSts.SeparationTimeMinimum <= LIBCANTP_SEPARATIONTIMEMINMUM_MAX))
		{
			//Restart the STminTimer
			LibTimer_Stop(&pInst->Timers.SeparationTimeMinTimer); 
            LibTimer_Start(&pInst->Timers.SeparationTimeMinTimer,pInst->FlowCtrlSts.SeparationTimeMinimum,UINT32_C(0)); 
		}
		else
		{
            LibService_SetEvent(&LibCanTp_Service, LIBCANTP_SRV_EV_REQ);
			LibCanTp_HandleFlowCtrlStsBlockSize(pInst);
		}
	}
	else if((pInst->pDataUnit->IsFinished) && (msgId == LIBCANTPCFG_ECU_PHYS_ADDRESS))
	{
		LibTimer_Stop(&pInst->Timers.SeparationTimeMinTimer);  //Stop STminTimer 
		//S_LibUds_TxConf_t* pTxConfirm = LN7Diag_GetTxConfirm();

		//if (NULL != pTxConfirm->pConfirm)
		//{
			// Set event for diag task
			//LN7Diag_DiagComIFace(LN7DIAG_RESP_TX_CONFIRM, NULL);
			//pTxConfirm->RespTxResult = true;
		//}
		//else
		//{
			//pTxConfirm->RespTxResult = false;
		//}
	}
			
}

static void LibCanTp_HandleRxFrame(const S_LibCanTp_MsgIndBufferEntry_t *pBufEntry)
{
	S_LibCanTp_Inst_t *pInst = NULL;
	E_LibCanTp_FrameType_t messageType = LibCanTpInt_ParseFrameTypeRx(&pBufEntry);

	// Select the TP instance according to CAN Interface
	for (uint8_t i = 0; (i < LIBCANTP_INST_COUNT) && (NULL == pInst); i++)
	{
		pInst = LibCanTp_Inst_Table[i];
		if (pInst->DevId != pBufEntry->DevId)
		{
			pInst = NULL;
		}
	}
	Lib_Assert(NULL != pInst);

	if (LibDiagCom_IsReady())
	{
		
		memcpy(pInst->pCanMsg, &pBufEntry, sizeof(S_LibCan_Msg_t));
		const uint8_t message_length = LibCan_GetMsgDataLength(pInst->pCanMsg->Length);

		switch (messageType)
		{
			case LIBCANTP_FRAMETYPE_SINGLEFRAME:
			{ // SF
				LibLog_Debug("CAN:TP RX SF: %[]X", message_length, pInst->pCanMsg->Data);
				LibCanTp_HandleSingleFrame(pInst);
				break;
			}
			case LIBCANTP_FRAMETYPE_FIRSTFRAME:
			{ // FF
				LibLog_Debug("CAN:TP RX FF: %[]X", message_length, pInst->pCanMsg->Data);
				LibCanTp_HandleFirstFrame(pInst);
				break;
			}
			case LIBCANTP_FRAMETYPE_CONSECUTIVEFRAME:
			{ // CF
				LibLog_Debug("CAN:TP RX CF: %[]X", message_length, pInst->pCanMsg->Data);
				LibCanTp_HandleConsecutiveFrame(pInst);
				break;
			}
			case LIBCANTP_FRAMETYPE_FLOWCONTROL:
			{ // FC
				LibCanTp_HandleFlowControl(pInst);
				break;
			}
			default:
				LibLog_Error("CAN:TP Not possible Frame Type");
		}
	}
}
//=====================================================================================================================
// LibCanTP_TxStop:
//=====================================================================================================================
void LibCanTP_TxStop(void)
{
	LibLog_Info("CAN:TP TX Stop");
	LibCanTP_TransmitEnabled = false;
	LibFifoQueue_Clear(&LibCanTp_MsgReqFifo);
}
//=====================================================================================================================
// LibCanTP_RxStopv:
//=====================================================================================================================
void LibCanTP_RxStop(void)
{
	//LibLog_Info("CAN:TP RX Stop");
	LibCanTP_ReceiveEnabled = false;
	LibFifoQueue_Clear(&LibCanTp_MsgIndFifo);
}
//=====================================================================================================================
// LibCanTP_TxEnable:
//=====================================================================================================================
void LibCanTP_TxEnable(void)
{
	LibLog_Info("CAN:TP TX Enable");
	LibCanTP_TransmitEnabled = true;
}
//=====================================================================================================================
// LibCanTP_RxEnable:
//=====================================================================================================================
void LibCanTP_RxEnable(void)
{
	LibLog_Info("CAN:TP RX Enable");
	LibCanTP_ReceiveEnabled = true;
}
//=====================================================================================================================
// LibCanTP_TxDisable:
//=====================================================================================================================
void LibCanTP_TxDisable(void)
{
	//LibLog_Info("CAN:TP TX Disable");
	LibCanTP_TransmitEnabled = false;
}
//=====================================================================================================================
// LibCanTP_RxDisable:
//=====================================================================================================================
void LibCanTP_RxDisable(void)
{
	//LibLog_Info("CAN:TP RX Disable");
	LibCanTP_ReceiveEnabled = false;
}
