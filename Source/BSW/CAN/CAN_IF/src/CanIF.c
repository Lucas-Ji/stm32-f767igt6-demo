// --------------------------------------------------------------------------------------------------------------------
///
/// \file CanIF.c
///
/// \brief 
///
/// 
///
///
/// 
///
/// Copyright (c) 
/// All Rights Reserved.
///
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
//	Local Definitions to control properties inside header files
// --------------------------------------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------------------------------------
//	Includes
// --------------------------------------------------------------------------------------------------------------------
#include "CanIF.h"
#include "LibTypes.h"
#include "CanTask.h"
#include "LibCanMsg.h"
#include "CanNm.h"
// --------------------------------------------------------------------------------------------------------------------
//	Local Data Types
// --------------------------------------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------------------------------------
//	Local Variables
// --------------------------------------------------------------------------------------------------------------------
static CAN_FilterTypeDef CAN1Filter;

// -------------------------------------------------------------------------------------------------------------------- 
/// \brief FIFO buffer for CANIF Receiver
// -------------------------------------------------------------------------------------------------------------------- 
static S_LibCan_Msg_t CanIF_MsgRecvBuffer[CANIF_MSG_RECV_FIFO_ELEMENTS];

// -------------------------------------------------------------------------------------------------------------------- 
/// \brief Settings for the FIFO 
// -------------------------------------------------------------------------------------------------------------------- 
LIBFIFO_DEFINE_INST(CanIF_MsgRecvFifo,
                   (uint32_t*)(void*)CanIF_MsgRecvBuffer,
                   sizeof(S_LibCan_Msg_t),
                   (uint32_t)CANIF_MSG_RECV_FIFO_ELEMENTS,
                   false);
// -------------------------------------------------------------------------------------------------------------------- 
/// \brief Buffer configuration 
// -------------------------------------------------------------------------------------------------------------------- 


// --------------------------------------------------------------------------------------------------------------------
//	Global Variables
// --------------------------------------------------------------------------------------------------------------------
uint8_t Can1_Bus_Off_flag = false;
uint8_t Can2_Bus_Off_flag = false;
// --------------------------------------------------------------------------------------------------------------------
//	Local Function Prototypes
// --------------------------------------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------------------------------------
//	Functions
// --------------------------------------------------------------------------------------------------------------------

 /**************************************************************************************
 * FunctionName   : Can1IfDrv_Init
 * Description    : Can1 initialization
 * EntryParameter : None
 * ReturnValue    : None
 **************************************************************************************/
void Can1IfDrv_Init(void)
{
	MX_CAN1_Init();
}
/**************************************************************************************
* FunctionName   : Can2IfDrv_Init
* Description    : Can2 initialization
* EntryParameter : None
* ReturnValue    : None
**************************************************************************************/
void Can2IfDrv_Init(void)
{

}

/**************************************************************************************
* FunctionName   : Can1IfDrv_Deinit
* Description    : Can1 Deinitialization
* EntryParameter : None
* ReturnValue    : None
**************************************************************************************/
void Can1IfDrv_Deinit(void)
{
	HAL_CAN_DeInit(&hcan1);
}
/**************************************************************************************
* FunctionName   : Can2IfDrv_Deinit
* Description    : Can2 Deinitialization
* EntryParameter : None
* ReturnValue    : None
**************************************************************************************/
void Can2IfDrv_Deinit(void)
{

}

void Can1IfDrv_Start(void)
{
	HAL_CAN_StateTypeDef state = HAL_CAN_GetState(&hcan1);
	if(state == HAL_CAN_STATE_READY)
	{
		CAN1Filter.FilterIdHigh = 0x0000;
		CAN1Filter.FilterIdLow = 0x0000;
		CAN1Filter.FilterMaskIdHigh = 0x0000;
		CAN1Filter.FilterMaskIdLow = 0x0000;
		CAN1Filter.FilterFIFOAssignment = CAN_FILTER_FIFO0;
		CAN1Filter.FilterBank = 0;
		CAN1Filter.FilterMode = CAN_FILTERMODE_IDMASK;
		CAN1Filter.FilterScale = CAN_FILTERSCALE_32BIT;
		CAN1Filter.FilterActivation = ENABLE;
		CAN1Filter.SlaveStartFilterBank = 14;

		HAL_CAN_ActivateNotification(&hcan1, CAN_IT_RX_FIFO0_MSG_PENDING|CAN_IT_BUSOFF);
		HAL_CAN_ConfigFilter(&hcan1, &CAN1Filter);
		HAL_CAN_Start(&hcan1);
	}
}

void Can1IfDrv_Stop(void)
{
	HAL_CAN_StateTypeDef state = HAL_CAN_GetState(&hcan1);
	if(state == HAL_CAN_STATE_LISTENING)
	{
		HAL_CAN_DeactivateNotification(&hcan1, CAN_IT_RX_FIFO0_MSG_PENDING|CAN_IT_BUSOFF);
		HAL_CAN_Stop(&hcan1);
	}
}

void Can2IfDrv_Start(void)
{

}

void Can2IfDrv_Stop(void)
{

}

void Can1IfDrv_SleepReq(void)
{
	HAL_CAN_StateTypeDef state = HAL_CAN_GetState(&hcan1);
	if((state == HAL_CAN_STATE_READY) || (state == HAL_CAN_STATE_LISTENING))
	{
		HAL_CAN_RequestSleep(&hcan1);
	}
}

void Can1IfDrv_WakeUp(void)
{
	uint32_t state = HAL_CAN_IsSleepActive(&hcan1);
	if(state)
	{
		HAL_CAN_WakeUp(&hcan1);
	}
}

void Can2IfDrv_SleepReq(void)
{

}

void Can2IfDrv_WakeUp(void)
{

}

/**************************************************************************************
* FunctionName   : CanIF_ErrorCallback
* Description    : Can1/2 Error interrupt callback function
* EntryParameter : instance,eventType,flexcanState
* ReturnValue    : None
**************************************************************************************/
void HAL_CAN_ErrorCallback(CAN_HandleTypeDef *hcan)
{
	uint32_t CANErrCode;

	CANErrCode = HAL_CAN_GetError(hcan);
	switch (CANErrCode)
	{
	case HAL_CAN_ERROR_BOF:
		if(hcan->Instance == CAN1)
		{
			Can1_Bus_Off_flag=true;
		}
		else
		{
			Can2_Bus_Off_flag=true;
		}
		Can_BusOff((void*)hcan);
		break;
	
	default:
		break;
	}
}

/**************************************************************************************
* FunctionName   : Can1IF_RxCallback
* Description    : Can1 receive interrupt callback function
* EntryParameter : instance,eventType,buffIdx,flexcanState
* ReturnValue    : None
**************************************************************************************/
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
	CAN_RxHeaderTypeDef CANRxHeader;
	uint32_t RxFrameNum;

	do{
		S_LibCan_Msg_t Can1IF_Receive_Msg;
		HAL_StatusTypeDef MsgState = HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &CANRxHeader, Can1IF_Receive_Msg.Data);
		if(MsgState == HAL_OK)
		{
			Can1IF_Receive_Msg.CanDevId = CanChannel_1;
			if(CANRxHeader.IDE == CAN_ID_STD)
			{
				Can1IF_Receive_Msg.IsExtId = false;
				Can1IF_Receive_Msg.Id = CANRxHeader.StdId;
			}
			else
			{
				Can1IF_Receive_Msg.IsExtId = true;
				Can1IF_Receive_Msg.Id = CANRxHeader.ExtId;
			}
			if(CANRxHeader.RTR == CAN_RTR_DATA)
			{
				Can1IF_Receive_Msg.IsRemote = false;
				Can1IF_Receive_Msg.Length = CANRxHeader.DLC;
			}
			else
			{
				Can1IF_Receive_Msg.IsRemote = true;
				Can1IF_Receive_Msg.Length = 0u;
			}

			bool_t stored = LibFifoQueue_Push(&CanIF_MsgRecvFifo, (void*)(&Can1IF_Receive_Msg));
			if (stored == false)
			{
				(void)LibFifoQueue_Clear(&CanIF_MsgRecvFifo);
				LibLog_Error("CANIF: Cannot store receive message\n");
			}
			else
			{
				/* Can_HandleCanMsgs(hcan); */
				Can_MsgReceived((void*)hcan);
			}
			RxFrameNum = HAL_CAN_GetRxFifoFillLevel(hcan,CAN_RX_FIFO0);
		}
		else
		{
			LibLog_Error("CANIF: CAN Mailbox Cannot get receive message\n");
			break;
		}

	}while(RxFrameNum != 0u)
	
}

/**************************************************************************************
* FunctionName   : Can2IF_RxCallback
* Description    : Can2 receive interrupt callback function
* EntryParameter : instance,eventType,buffIdx,flexcanState
* ReturnValue    : None
**************************************************************************************/
void HAL_CAN_RxFifo1MsgPendingCallback(CAN_HandleTypeDef *hcan)
{

}
/**************************************************************************************
* FunctionName   : Can1_Bus_Off
* Description    : Can1 bus off processing
* EntryParameter : None
* ReturnValue    : None
**************************************************************************************/
void Can1_Bus_Off(void)
{
   if(Can1_Bus_Off_flag==true)
   {
	   Can1IfDrv_Deinit();

	   Can1IfDrv_Init();
	   Can1IfDrv_Start();
	   Can1_Bus_Off_flag=false;
   }
}
/**************************************************************************************
* FunctionName   : Can2_Bus_Off
* Description    : Can2 bus off processing
* EntryParameter : None
* ReturnValue    : None
**************************************************************************************/
void Can2_Bus_Off(void)
{
   if(Can2_Bus_Off_flag==true)
   {
	   Can2IfDrv_Deinit();

	   Can2IfDrv_Init();
	   Can2IfDrv_Start();
	   Can2_Bus_Off_flag=false;
   }
}

