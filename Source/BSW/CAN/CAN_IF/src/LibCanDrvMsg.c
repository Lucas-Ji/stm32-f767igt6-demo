// -------------------------------------------------------------------------------------------------------------------- 
/// 
/// \file LibUsart.c
/// 
/// \brief LibUsart
/// 
/// 
/// Copyright (c) 2021 Neusoft.
/// All Rights Reserved.
/// 
// -------------------------------------------------------------------------------------------------------------------- 

// -------------------------------------------------------------------------------------------------------------------- 
//  Includes 
// --------------------------------------------------------------------------------------------------------------------
#include "LibCanDrvMsg.h"
#include "LibCanMsg.h"
#include "CanIF.h"
#include "CanNmMsgs.h"
#include "LibFifoQueue.h"
#include "LibCanMsg.h"
#include "CanTask.h"
#include "LibCanILCfg.h"
#include "LibCanTpCfg.h"
#include <string.h>


// -------------------------------------------------------------------------------------------------------------------- 
//  Local Definitions 
// -------------------------------------------------------------------------------------------------------------------- 
#define CANDRV1_MASK     UINT8_C(0x01)
#define CANDRV2_MASK     UINT8_C(0x02)
#define CANDRVALL_MASK   (CANDRV1_MASK | CANDRV2_MASK)
// --------------------------------------------------------------------------------------------------------------------
//	Local Data Types
// --------------------------------------------------------------------------------------------------------------------

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
//	Functions
// --------------------------------------------------------------------------------------------------------------------
Ret_t LibMcan_IoCtl(void* pData, uint8_t command)
{
	Ret_t retval = LIBRET_OK;

	switch (command)
	{
		case LIBCAN_IOCTL_START:
		{
			Can1IfDrv_Start();
			Can2IfDrv_Start();
			break;
		}

		case LIBCAN_IOCTL_STOP:
		{
			Can1IfDrv_Stop();
			Can2IfDrv_Stop();
			break;
		}

		case LIBCAN_IOCTL_GET_NEXT_MSG:
		{

			S_LibCan_Msg_t* pMsg;
			pMsg = (S_LibCan_Msg_t*)LibFifoQueue_GetPopItem(&CanIF_MsgRecvFifo);
			if(pMsg == NULL)
			{
				retval = LIBRET_NO_ENTRY;
				break;
			}
			S_LibCan_Msg_t* const pRecMsg = (S_LibCan_Msg_t*)pData;
			(void)memcpy(pRecMsg, pMsg, sizeof(S_LibCan_Msg_t));
		    break;	
		} 

		
		case LIBCAN_IOCTL_SEND_MSG:
		{
			CAN_TxHeaderTypeDef TxMessageHeader;
			uint8_t TxMessage[LIBCAN_MAXDATABYTENUM];
			uint32_t txMailbox;

			S_LibCan_Msg_t* const pMsg = (S_LibCan_Msg_t*)pData;
			if(pMsg->IsExtId)
			{
				TxMessageHeader.IDE = CAN_ID_EXT;
				TxMessageHeader.ExtId = pMsg->Id;
			}
			else
			{
				TxMessageHeader.IDE = CAN_ID_STD;
				TxMessageHeader.StdId = pMsg->Id;
			}
			TxMessageHeader.DLC = pMsg->Length;
			TxMessageHeader.RTR = CAN_RTR_DATA;
			TxMessageHeader.TransmitGlobalTime = DISABLE;

			(void)memcpy(TxMessage, pMsg->Data, pMsg->Length);
            
			uint8_t CanDrvChoseBit = 0x00;
			switch (pMsg->CanDevId)
			{
				case CanChannel_1:
					CanDrvChoseBit = CANDRV1_MASK;
					break;
				
				case CanChannel_2:
					CanDrvChoseBit = CANDRV2_MASK;
					break;
				
				case CanChannel_All:
					CanDrvChoseBit = CANDRVALL_MASK;
					break;
                
				default:
					break;
			}

			if(CanDrvChoseBit & CANDRV1_MASK)
			{
				HAL_StatusTypeDef send_status1 = HAL_ERROR;
				uint32_t MailBoxNum = HAL_CAN_GetTxMailboxesFreeLevel(&hcan1);
				if(MailBoxNum > 0u)
				{
					send_status1 = HAL_CAN_AddTxMessage(&hcan1, &TxMessageHeader, TxMessage, &txMailbox);
				}

				if(HAL_OK == send_status1)
				{
					// Only CAN TP/NM need Send Confirm
					/* Can_ModuleTable[i]->IsMsg(msg.Id) */
					bool_t bSendCon = false;
#ifdef LIBCANTP
					bSendCon = LibCanTp_Module.IsMsg(pMsg->Id);
#endif
					bSendCon |= CanNm_Module.IsMsg(pMsg->Id);
					if(bSendCon)
					{
						Can_MsgSent(pMsg->Id);
					}
				}
			}
			
			if (CanDrvChoseBit & CANDRV2_MASK)
			{

			}
			
			break;
		}

		default:
		{
			// unsupported I/O control command
			retval = LIBRET_INV_CALL;
			break;
		}
	}

	return retval;
}
