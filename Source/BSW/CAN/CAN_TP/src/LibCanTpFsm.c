// --------------------------------------------------------------------------------------------------------------------
/// \file LibCanTpFsm.c
///
/// \brief <brief description>
///
/// <detailed description>
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

#include "LibCanTpFsm.h"
#include "LibCanTpInternal.h"

//#include "LibAssert.h"
#include "LibDiagCom.h"
#include "LibFsm.h"
#include "LibTypes.h"
#include <string.h>

// --------------------------------------------------------------------------------------------------------------------
//	Local Definitions
// --------------------------------------------------------------------------------------------------------------------

#define LIBCANTP_FSM_EVENTS(DEFINE_EVENT) \
	DEFINE_EVENT(LIBCANTP_EV_INIT) \
	DEFINE_EVENT(LIBCANTP_EV_RECV) \
	DEFINE_EVENT(LIBCANTP_EV_SEND) \
	DEFINE_EVENT(LIBCANTP_EV_FINISH)

#define LIBCANTP_FSM_STATES(DEFINE_STATE) \
	DEFINE_STATE(LIBCANTP_ST_INIT, 			LibFsm_Empty) \
	DEFINE_STATE(LIBCANTP_ST_READY, 		LibFsm_Empty) \
	DEFINE_STATE(LIBCANTP_ST_RECV, 			LibFsm_Empty) \
	DEFINE_STATE(LIBCANTP_ST_SEND, 			LibFsm_Empty)


#define TRANSITIONS_FOR_STATE_LIBCANTP_ST_INIT(ENTRY) \
	ENTRY(LIBCANTP_EV_INIT, 		LIBCANTP_ST_READY, 			LibCanTpFsm_Init) \

#define TRANSITIONS_FOR_STATE_LIBCANTP_ST_READY(ENTRY) \
	ENTRY(LIBCANTP_EV_INIT, 		LIBCANTP_ST_READY, 			LibCanTpFsm_Init) \
	ENTRY(LIBCANTP_EV_RECV, 		LIBCANTP_ST_RECV, 			LibCanTpFsm_Recv) \
	ENTRY(LIBCANTP_EV_SEND,			LIBCANTP_ST_SEND,			LibCanTpFsm_Send) \

#define TRANSITIONS_FOR_STATE_LIBCANTP_ST_RECV(ENTRY) \
    ENTRY(LIBCANTP_EV_INIT, 		LIBCANTP_ST_READY, 			LibCanTpFsm_Init) \
	ENTRY(LIBCANTP_EV_RECV, 		LIBCANTP_ST_RECV, 			LibCanTpFsm_Recv) \
	ENTRY(LIBCANTP_EV_FINISH, 		LIBCANTP_ST_READY, 			LibCanTpFsm_RecvFinish) \

#define TRANSITIONS_FOR_STATE_LIBCANTP_ST_SEND(ENTRY) \
    ENTRY(LIBCANTP_EV_INIT, 		LIBCANTP_ST_READY, 			LibCanTpFsm_Init) \
	ENTRY(LIBCANTP_EV_SEND, 		LIBCANTP_ST_SEND, 			LibCanTpFsm_Send) \
	ENTRY(LIBCANTP_EV_FINISH, 		LIBCANTP_ST_READY, 			LibCanTpFsm_SendFinish) \


// --------------------------------------------------------------------------------------------------------------------
//	Local Data Types
// --------------------------------------------------------------------------------------------------------------------

LIBFSM_DECLARE_STATE_MACHINE(LibCanTpInt, LIBCANTP_FSM_EVENTS, LIBCANTP_FSM_STATES, NULL);

// --------------------------------------------------------------------------------------------------------------------
//	Global Variables
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
//	Local Variables
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
//	Local Function Prototypes
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
//	Global Functions
// --------------------------------------------------------------------------------------------------------------------

// ====================================================================================================================
// LibCanTpFsm_TriggerInit:
// ====================================================================================================================
void LibCanTpFsm_TriggerInit(S_LibCanTp_Inst_t* pInst)
{
	LibFsm_DispatchEvent(&pInst->Fsm, LIBCANTP_EV_INIT);
}

// ====================================================================================================================
// LibCanTpFsm_TriggerRecv:
// ====================================================================================================================
void LibCanTpFsm_TriggerRecv(S_LibCanTp_Inst_t* pInst)
{
	LibFsm_DispatchEvent(&pInst->Fsm, LIBCANTP_EV_RECV);
	if (pInst->pDataUnit->IsFinished)
	{
		LibFsm_DispatchEvent(&pInst->Fsm, LIBCANTP_EV_FINISH);
	}
}

// ====================================================================================================================
// LibCanTpFsm_TriggerSend:
// ====================================================================================================================
void LibCanTpFsm_TriggerSend(S_LibCanTp_Inst_t* pInst)
{
	LibFsm_DispatchEvent(&pInst->Fsm, LIBCANTP_EV_SEND);
	if (pInst->pDataUnit->IsFinished)
	{
		LibFsm_DispatchEvent(&pInst->Fsm, LIBCANTP_EV_FINISH);
	}
}

// ====================================================================================================================
// LibCanTpFsm_IsReady:
// ====================================================================================================================
bool_t LibCanTpFsm_IsReady(S_LibCanTp_Inst_t* pInst)
{
	const E_LibCanTpInt_State_t currentState = (E_LibCanTpInt_State_t)LibFsm_GetCurrentState(&pInst->Fsm);
	return (LIBCANTP_ST_READY == currentState);
}

// ====================================================================================================================
// LibCanTpFsm_IsSend:
// ====================================================================================================================
bool_t LibCanTpFsm_IsSend(S_LibCanTp_Inst_t* pInst)
{
	const E_LibCanTpInt_State_t currentState = (E_LibCanTpInt_State_t)LibFsm_GetCurrentState(&pInst->Fsm);
	return (LIBCANTP_ST_SEND == currentState);
}

// ====================================================================================================================
// LibCanTpFsm_IsRecv:
// ====================================================================================================================
bool_t LibCanTpFsm_IsRecv(S_LibCanTp_Inst_t* pInst)
{
	const E_LibCanTpInt_State_t currentState = (E_LibCanTpInt_State_t)LibFsm_GetCurrentState(&pInst->Fsm);
	return (LIBCANTP_ST_RECV == currentState);
}


// --------------------------------------------------------------------------------------------------------------------
//	Functions
// --------------------------------------------------------------------------------------------------------------------


static void LibCanTpFsm_Init(void* pData)
{
	S_LibCanTp_Inst_t* pInst = (S_LibCanTp_Inst_t*)pData;

	//Stop all Timer
	LibTimer_Stop(&pInst->Timers.TransmissionTimer);
	LibTimer_Stop(&pInst->Timers.FlowControlTimer);
	LibTimer_Stop(&pInst->Timers.ConsecutiveTimer);

	pInst->pDataUnit->IsFinished = true; //need to be true
	pInst->pDataUnit->IsRxMultiFrame = false;
	pInst->pDataUnit->IsTxMultiFrame = false;
}

static void LibCanTpFsm_Recv(void* pData)
{
	S_LibCanTp_Inst_t* pInst = (S_LibCanTp_Inst_t*)pData;

	if (0U == pInst->pDataUnit->BufferDataRemaining)
	{
		pInst->pDataUnit->IsFinished = true;
	}
}

static void LibCanTpFsm_RecvFinish(void* pData)
{
	S_LibCanTp_Inst_t* pInst = (S_LibCanTp_Inst_t*)pData;
	// LibCanTpDiagCom_Indicate(pInst);
	// todo
	S_LibDiagCom_Msg_t* pComMsg = LibDiagCom_GetMsg();
	pComMsg->DevId = pInst->DevId;
	pComMsg->SrcAddr = pInst->pDataUnit->SourceAddress;
	pComMsg->TgtAddr = pInst->pDataUnit->TargetAddress;
	pComMsg->MaxPayloadLen = LIBCANTPCFG_MULTIFRAME_BUFFER_SIZE;
	pComMsg->pPayload = pInst->pDataUnit->Buffer;
	pComMsg->PayloadLen = pInst->pDataUnit->Length;
	pComMsg->IsPhysical = pInst->pDataUnit->IsPhysical;
	pComMsg->SendMessage = LibCanTp_MsgRequest;
	LibDiagCom_MsgReceived(pComMsg);
}

static void LibCanTpFsm_Send(void* pData)
{
	S_LibCanTp_Inst_t* pInst = (S_LibCanTp_Inst_t*)pData;
	S_LibCanTp_DataUnit_t* pMsg = pInst->pDataUnit;
	S_LibCan_Msg_t* pCanMsg = pInst->pCanMsg;

	// Set ID and isExtId
	LibCanTpInt_ParseAddrTx(pCanMsg, (uint8_t)pMsg->SourceAddress, (uint8_t)pMsg->TargetAddress, pMsg->IsPhysical);

	if (pMsg->Length <= 7U)
	{
		// SF
		pCanMsg->Data[0U] = 0x0FU & ((uint8_t)pMsg->Length);
		pCanMsg->Length = (E_LibCan_DlcSize_t)(pMsg->Length + 1U);
		memcpy(&pCanMsg->Data[1U], pMsg->Buffer, pMsg->Length);
		pMsg->BufferRWIdx += pMsg->Length;
		//pMsg->BufferDataRemaining -= pMsg->Length;
		pMsg->BufferDataRemaining = 0; //hotfix the bug: will always send msg and never stop in some abnormal situation
		pMsg->IsTxMultiFrame = false;
	//	LibLog_Info("pCanMsg->Length: %d==%d | buff: %[]X", pCanMsg->Length, LIBCAN_DLCSIZE_8_B, 8, pCanMsg->Data);
	}
	else
	{
		uint8_t dataOffset = 1U;
		uint8_t dataWritten = 0U;
		// MultiFrame
		if (0U == pMsg->BufferRWIdx)
		{
			// FF
			pCanMsg->Data[0U] = 0x10U | (0x0FU & (uint8_t)(pMsg->Length >> 8U));
			pCanMsg->Data[1U] = (uint8_t)(0xFFU & pMsg->Length);
			dataOffset += 1U;
			pInst->FlowCtrlSts.CurSequenceNumber = 0U;
			pInst->FlowCtrlSts.BlockSizeRemaining = 0U;
			pInst->FlowCtrlSts.BlockSize = 1U;
			pMsg->IsTxMultiFrame = true;
		}
		else
		{
			if(pMsg->IsTxMultiFrame == true)
			{
				// CF
				pInst->FlowCtrlSts.CurSequenceNumber += 1U;
				pInst->FlowCtrlSts.CurSequenceNumber &= 0x0FU;	// mod 16
				pCanMsg->Data[0U] = 0x20U | (0x0FU & pInst->FlowCtrlSts.CurSequenceNumber);
			}
			else
			{
				//if not MultiFrame just abort this msg
		        LibService_SetEvent(&LibCanTp_Service, LIBSERVICE_EV_RE_INIT);
				return; 
			}
			
		}

		// the max amount that still fits into this frame
		dataWritten += (8U - dataOffset);
		// take the minimum of the max amount of bytes and the actual remaining data
		dataWritten = (dataWritten < pMsg->BufferDataRemaining) ? dataWritten : pMsg->BufferDataRemaining;

		pCanMsg->Length = (E_LibCan_DlcSize_t)(dataWritten + dataOffset);

		// so far no CAN FD supported
		pCanMsg->IsBrs = false;		// <- no Bit Rate Switch
		pCanMsg->IsCanFd = false;	// <- no CAN FD

		memcpy(&pCanMsg->Data[dataOffset], &pMsg->Buffer[pMsg->BufferRWIdx], dataWritten);
		pMsg->BufferRWIdx += dataWritten;
	//	LibLog_Info("** 8+ ** pCanMsg->Length: %d==%d | buff: %[]X", pCanMsg->Length, LIBCAN_DLCSIZE_8_B, 9, pCanMsg->Data);

		// data written must not be greater than remaining data
		Lib_Assert(pMsg->BufferDataRemaining >= dataWritten);
		pMsg->BufferDataRemaining -= dataWritten;
	}

	LibCanTp_HandleTxMessage(pInst);
}

static void LibCanTpFsm_SendFinish(void* pData)
{
	// S_LibCanTp_Inst_t* pInst = (S_LibCanTp_Inst_t*)pData;
}

