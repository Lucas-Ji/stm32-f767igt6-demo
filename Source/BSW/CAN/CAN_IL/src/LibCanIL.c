// --------------------------------------------------------------------------------------------------------------------
/// \file LibCanIL
///
/// \brief This module implements the CAN Interaction Layer
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
#include "LibCanIL.h"
#include "LibTimer.h"
#include "CanTask.h"

// --------------------------------------------------------------------------------------------------------------------
//	Local Definitions
// --------------------------------------------------------------------------------------------------------------------
#define LIBCANIL_MSG_REQ_FIFO_ELEMENTS	(8U)
#define LIBCANIL_MSG_IND_FIFO_ELEMENTS	(8U)
#define LIBCANIL_MSG_CON_FIFO_ELEMENTS	(8U)

#if LIBCANILCFG_NUMBER_OF_TX_EVENT_MESSAGE 
//the interval need to be 20ms, set to be 19ms because the delay
#define LIBCANIL_EVENT_MSG_SEND_INTERVAL      (TIMEBMSToTMS)
#define LIBCANIL_EVENT_MSG_SEND_TIMES         (3U)
#endif

#if LIBCANILCFG_NUMBER_OF_RX_CYCLE_MESSAGE 
#define LIBCANIL_CYCLE_MSG_RECE_INTERVAL     (5U)
#define LIBCANIL_CYCLE_MSG_RECE_TIMEOUTMAX   (5000U)
#endif

//#define LIBCANIL_CYCLETX_SENDCNT_EN
// --------------------------------------------------------------------------------------------------------------------
//	Local Data Types
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
//	Local Function Prototypes
// --------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------
/// \brief Initialized the Interaction Layer.
// --------------------------------------------------------------------------------------------------------------------
static void LibCanIL_Init(void);

// --------------------------------------------------------------------------------------------------------------------
/// \brief Read all relevant signals and writes them to the message data.
///
/// \param msgName
/// Name of message to be write
/// \param pMsg
/// Pointer to the message structure
// --------------------------------------------------------------------------------------------------------------------
static void LibCanIL_WriteMessage(E_LibCanILCfg_MessageNames_t msgName, S_LibCan_Msg_t* pMsg);

// --------------------------------------------------------------------------------------------------------------------
/// \brief Read the message data an set all new signal values of this message.
///
/// \param msgName
/// Name of message to be read
/// \param pMsg
/// Pointer to the message structure
// --------------------------------------------------------------------------------------------------------------------
static void LibCanIL_ReadMessage(E_LibCanILCfg_MessageNames_t msgName, const S_LibCan_Msg_t* pMsg);

// --------------------------------------------------------------------------------------------------------------------
/// \brief Transmit a CAN Message.
///
/// \details
/// - Read all relevant signals of this message
/// - Construct the message, data from signals, Id, Length ...
/// - Push the message to the transmit-FIFO and
/// - Set the transmit-flag of CAN-Task
///
/// \param msgName
/// Name of message to be transmit
// --------------------------------------------------------------------------------------------------------------------
static void LibCanIL_TransmitMsg(E_LibCanILCfg_MessageNames_t msgName);

// --------------------------------------------------------------------------------------------------------------------
/// \brief Returns the extracted signal from can message.
///
/// \param msgData
/// Data from can message.
/// \param msgStartBit
/// Start bit of the signal.
/// \param length
/// Length of the signal.
///
/// \return The value of the signal.
// --------------------------------------------------------------------------------------------------------------------
static uint64_t LibCanIL_ExtractSignal(uint64_t msgData, uint8_t msgStartBit, uint8_t length);

// --------------------------------------------------------------------------------------------------------------------
/// \brief Returns the can message data with the new signal value.
///
/// \param msgData
/// Old data for the can message.
/// \param sigValue
/// New value of the signal.
/// \param msgStartBit
/// Start bit of the signal.
/// \param length
/// Length of the signal.
///
/// \return The new data for the can message.
// --------------------------------------------------------------------------------------------------------------------
static uint64_t LibCanIL_ConstructSignal(uint64_t msgData, uint64_t sigValue, uint8_t msgStartBit, uint8_t length);

// --------------------------------------------------------------------------------------------------------------------
/// \brief Callback function used by the Interaction Layer tx event message timer
///
/// \param pData expected to point to the instance structure in which the timeout has occurred
// --------------------------------------------------------------------------------------------------------------------
#if LIBCANILCFG_NUMBER_OF_TX_EVENT_MESSAGE 
static void LibCanIL_TxEventMsgTimerCallback(void* pData);
#endif

// --------------------------------------------------------------------------------------------------------------------
/// \brief Callback function used by the Interaction Layer tx cycle message timer
///
/// \param pData expected to point to the instance structure in which the timeout has occurred
// --------------------------------------------------------------------------------------------------------------------
#if LIBCANILCFG_NUMBER_OF_TX_CYCLE_MESSAGE
static void LibCanIL_TxCycleMsgTimerCallback(void* pData);
#endif

// --------------------------------------------------------------------------------------------------------------------
/// \brief Callback function used by the Interaction Layer rx cycle message timer
///
/// \param pData expected to point to the instance structure in which the timeout has occurred
// --------------------------------------------------------------------------------------------------------------------
#if LIBCANILCFG_NUMBER_OF_RX_CYCLE_MESSAGE 
static void LibCanIL_RxCycleMsgTimerCallback(void* pData);
#endif

// --------------------------------------------------------------------------------------------------------------------
/// \brief Service Handler for the CAN Interaction Layer
///
/// \param pData <br> User data. Not used
// --------------------------------------------------------------------------------------------------------------------
static void LibCanIL_ServiceHndl(void* pData);

// --------------------------------------------------------------------------------------------------------------------
/// \brief Handle Service Event INIT
// --------------------------------------------------------------------------------------------------------------------
static void LibCanIL_ServiceEvInit(void);

// --------------------------------------------------------------------------------------------------------------------
/// \brief Handle Service TX Event MESSAGE_TIMER
// --------------------------------------------------------------------------------------------------------------------
#if LIBCANILCFG_NUMBER_OF_TX_EVENT_MESSAGE 
static void LibCanIL_ServiceEvTxEventMessageTimer(void);
#endif

// --------------------------------------------------------------------------------------------------------------------
/// \brief Handle Service TX CYCLE MESSAGE_TIMER
// --------------------------------------------------------------------------------------------------------------------
#if LIBCANILCFG_NUMBER_OF_TX_CYCLE_MESSAGE
static void LibCanIL_ServiceEvTxCycleMessageTimer(void);
#endif

// --------------------------------------------------------------------------------------------------------------------
/// \brief Handle Service RX CYCLE MESSAGE_TIMER
// --------------------------------------------------------------------------------------------------------------------
#if LIBCANILCFG_NUMBER_OF_RX_CYCLE_MESSAGE 
static void LibCanIL_ServiceEvRxCycleMessageTimer(void);
#endif

// --------------------------------------------------------------------------------------------------------------------
/// \brief Handle Service Event MESSAGE_INDICATION
// --------------------------------------------------------------------------------------------------------------------
static void LibCanIL_ServiceEvMessageInd(void);

// --------------------------------------------------------------------------------------------------------------------
/// \brief Handle Service Event MESSAGE_CONFIRM
// --------------------------------------------------------------------------------------------------------------------
static void LibCanIL_ServiceEvMessageCon(void);

// --------------------------------------------------------------------------------------------------------------------
/// \brief Indicates the reception of a new CAN message
///
/// \param pMsg
/// The received CAN message
/// \param devId
/// The device driver ID of the CAN interface that received the CAN message
// --------------------------------------------------------------------------------------------------------------------
static void LibCanIL_MsgIndicate(S_LibCan_Msg_t *pMsg);

// --------------------------------------------------------------------------------------------------------------------
/// \brief Confirms the successful transmission of a CAN message
///
/// \param msgId
/// The CAN ID that was sent successfully
/// \param devId
/// The device driver ID of the CAN interface that received the CAN message
// --------------------------------------------------------------------------------------------------------------------
static void LibCanIL_MsgConfirm(uint32_t msgId);

// --------------------------------------------------------------------------------------------------------------------
/// \brief Monitoring the Rx Msg can be used or not
///
/// \param msgDesc
/// The CAN Msg
/// \param msgName
/// The Msg name
// --------------------------------------------------------------------------------------------------------------------
#if LIBCANILCFG_NUMBER_OF_RX_CYCLE_MESSAGE 
static bool_t LibCanIL_RxMsgMonitor(const S_LibCanIL_MessageDesc_t* msgDesc, E_LibCanILCfg_MessageNames_t msgName);
#endif

// --------------------------------------------------------------------------------------------------------------------
/// \brief Count the CycleTx Send Num
///
/// \param msgName
/// The Msg name
// --------------------------------------------------------------------------------------------------------------------
#ifdef LIBCANIL_CYCLETX_SENDCNT_EN
static void LibCanIL_CycleTx_SendNumCnt(E_LibCanILCfg_MessageNames_t msgName);
#endif

// --------------------------------------------------------------------------------------------------------------------
//	Global Variables
// --------------------------------------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------------------------------------
/// \brief Service Instance of the CAN Interaction Layer
///
/// Initialize the service with the Service handler. No data is required.
///
/// \sa LibCanIl_ServiceHndl
// --------------------------------------------------------------------------------------------------------------------
S_LibService_Inst_t LibCanIL_Service = LIBSERVICE_INIT_SERVICE(LibCanIL_ServiceHndl, NULL);

const S_LibCanModule_Module_t LibCanIL_Module = {
	.IsMsg 			= LibCanIL_IsMsgIL,
	.MsgIndicate 	= LibCanIL_MsgIndicate,
	.MsgConfirm 	= LibCanIL_MsgConfirm
};

// --------------------------------------------------------------------------------------------------------------------
//	Local Variables
// --------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------
///	\brief  FIFO buffer for Interaction Layer message transfer
// --------------------------------------------------------------------------------------------------------------------
static S_LibCanIL_MsgReqBufferEntry_t LibCanIL_MsgReqBuffer[LIBCANIL_MSG_REQ_FIFO_ELEMENTS];
static S_LibCanIL_MsgIndBufferEntry_t LibCanIL_MsgIndBuffer[LIBCANIL_MSG_IND_FIFO_ELEMENTS];
static S_LibCanIL_MsgConBufferEntry_t LibCanIL_MsgConBuffer[LIBCANIL_MSG_CON_FIFO_ELEMENTS];

// --------------------------------------------------------------------------------------------------------------------
///	\brief Settings for the FIFO
// --------------------------------------------------------------------------------------------------------------------
LIBFIFO_DEFINE_INST(LibCanIL_MsgReqFifo,
						   (uint32_t*)(void*)LibCanIL_MsgReqBuffer,
						   sizeof(S_LibCanIL_MsgReqBufferEntry_t),
						   (uint32_t)LIBCANIL_MSG_REQ_FIFO_ELEMENTS,
						   false);

LIBFIFO_DEFINE_INST(LibCanIL_MsgIndFifo,
						   (uint32_t*)(void*)LibCanIL_MsgIndBuffer,
						   sizeof(S_LibCanIL_MsgIndBufferEntry_t),
						   (uint32_t)LIBCANIL_MSG_IND_FIFO_ELEMENTS,
						   false);

LIBFIFO_DEFINE_INST(LibCanIL_MsgConFifo,
						   (uint32_t*)(void*)LibCanIL_MsgConBuffer,
						   sizeof(S_LibCanIL_MsgConBufferEntry_t),
						   (uint32_t)LIBCANIL_MSG_CON_FIFO_ELEMENTS,
						   false);


// ----------------------------------------------------------------------------------------------------------------
/// \brief  Internal storage of all CAN signal.
// ----------------------------------------------------------------------------------------------------------------
static uint8_t LibCanIL_SignalStorage[LIBCANILCFG_SIGNAL_STORAGE_LENGTH];

// ----------------------------------------------------------------------------------------------------------------
/// \brief  Counter for signal changes.
// ----------------------------------------------------------------------------------------------------------------
static uint8_t LibCanIL_SignalDataChCounter[(uint8_t)LIBCANILCFG_SIGNAL_NAME_DIMENSION];

// ----------------------------------------------------------------------------------------------------------------
/// \brief	The flag is the change callback requested.
// ----------------------------------------------------------------------------------------------------------------
static bool_t LibCanIL_CallbackIsRequested[(uint8_t)LIBCANILCFG_CALLBACK_NAME_DIMENSION];

// --------------------------------------------------------------------------------------------------------------------
/// \brief  Instance of the timer used to manage envet message timings
// --------------------------------------------------------------------------------------------------------------------
#if LIBCANILCFG_NUMBER_OF_TX_EVENT_MESSAGE 
static S_LibTimer_Inst_t LibCanIL_TxEventMsgTimer = LIBTIMER_INIT_TIMER(LibCanIL_TxEventMsgTimerCallback, NULL);
#endif

// --------------------------------------------------------------------------------------------------------------------
/// \brief  Instance of the timer used to manage cycle message timings
// --------------------------------------------------------------------------------------------------------------------
#if LIBCANILCFG_NUMBER_OF_TX_CYCLE_MESSAGE
static S_LibTimer_Inst_t LibCanIL_TxCycleMsgTimer = LIBTIMER_INIT_TIMER(LibCanIL_TxCycleMsgTimerCallback, NULL);
#endif 

// --------------------------------------------------------------------------------------------------------------------
/// \brief  Instance of the timer used to manage cycle message timings
// --------------------------------------------------------------------------------------------------------------------
#if LIBCANILCFG_NUMBER_OF_RX_CYCLE_MESSAGE 
static S_LibTimer_Inst_t LibCanIL_RxCycleMsgTimer = LIBTIMER_INIT_TIMER(LibCanIL_RxCycleMsgTimerCallback, NULL);
#endif

// ----------------------------------------------------------------------------------------------------------------
/// \brief  All tx cycle/event messages of this module.
// ----------------------------------------------------------------------------------------------------------------
#if LIBCANILCFG_NUMBER_OF_TX_EVENT_MESSAGE 
static S_LibCanIL_TxEventMsg_t LibCanIL_TxEventMessages[LIBCANILCFG_NUMBER_OF_TX_EVENT_MESSAGE];
#endif

// ----------------------------------------------------------------------------------------------------------------
/// \brief  All tx cycle messages of this module.
// ----------------------------------------------------------------------------------------------------------------
#if LIBCANILCFG_NUMBER_OF_TX_CYCLE_MESSAGE
static S_LibCanIL_TxCycleMsg_t LibCanIL_TxCycleMessages[LIBCANILCFG_NUMBER_OF_TX_CYCLE_MESSAGE];
#endif

// ----------------------------------------------------------------------------------------------------------------
/// \brief  All tx cycle messages of this module.
// ----------------------------------------------------------------------------------------------------------------
#if LIBCANILCFG_NUMBER_OF_RX_CYCLE_MESSAGE 
static S_LibCanIL_RxCycleMsg_t LibCanIL_RxCycleMessages[LIBCANILCFG_NUMBER_OF_RX_CYCLE_MESSAGE];
#endif

// ----------------------------------------------------------------------------------------------------------------
/// \brief  The flag is the message receive enabled.
// ----------------------------------------------------------------------------------------------------------------
static bool_t LibCanIL_ReceiveEnabled = false;

// ----------------------------------------------------------------------------------------------------------------
/// \brief  The flag is the message transmit enabled.
// ----------------------------------------------------------------------------------------------------------------
bool_t LibCanIL_TransmitEnabled = false;

// --------------------------------------------------------------------------------------------------------------------
//	Local Functions
// --------------------------------------------------------------------------------------------------------------------

//=====================================================================================================================
// LibCanIL_ServiceHndl:
//=====================================================================================================================
static void LibCanIL_ServiceHndl(void* pData)
{
	// Initialization method
	if(LibService_CheckClearEvent(&LibCanIL_Service, LIBSERVICE_EV_INIT))
	{
		LibCanIL_ServiceEvInit();
	}

	// Reinitialize the service
	if(LibService_CheckClearEvent(&LibCanIL_Service, LIBSERVICE_EV_RE_INIT))
	{
		LibLog_Info("CAN:IL Service REINIT\n");
		LibCanIL_Init();
	}

#if LIBCANILCFG_NUMBER_OF_TX_EVENT_MESSAGE 
	// Tx Event message service
	if(LibService_CheckClearEvent(&LibCanIL_Service, LIBCANIL_EVENT_TXEVENT_MESSAGE_TIMER))
	{
		LibCanIL_ServiceEvTxEventMessageTimer();
	}
#endif

#if LIBCANILCFG_NUMBER_OF_TX_CYCLE_MESSAGE
	// Tx Cycle message service
	if(LibService_CheckClearEvent(&LibCanIL_Service, LIBCANIL_EVENT_TXCYCLE_MESSAGE_TIMER))
	{
		LibCanIL_ServiceEvTxCycleMessageTimer();
	}
#endif
    
#if LIBCANILCFG_NUMBER_OF_RX_CYCLE_MESSAGE 
	// Rx Cycle message service
	if(LibService_CheckClearEvent(&LibCanIL_Service, LIBCANIL_EVENT_RXCYCLE_MESSAGE_TIMER))
	{
		LibCanIL_ServiceEvRxCycleMessageTimer();
	}
#endif

	// receive massages from CAN transceiver
	if (LibService_CheckClearEvent(&LibCanIL_Service, LIBCANIL_EVENT_CAN_MESSAGE_IND))
	{
		LibCanIL_ServiceEvMessageInd();
	}

	// confirm transmitted massages from CAN transceiver
	if (LibService_CheckClearEvent(&LibCanIL_Service, LIBCANIL_EVENT_CAN_MESSAGE_CON))
	{
		LibCanIL_ServiceEvMessageCon();
	}

	// Shutdown/destruct the service
	if(LibService_CheckClearEvent(&LibCanIL_Service, LIBSERVICE_EV_TRIGGER_SHUTDOWN))
	{
		LibLog_Info("CAN:IL Shutdown\n");
		LibService_Terminate(&LibCanIL_Service);
	}
}

//=====================================================================================================================
// LibCanIL_ServiceEvInit:
//=====================================================================================================================
static void LibCanIL_ServiceEvInit(void)
{
	uint8_t loop;
	uint8_t txMsgCounter;
	uint8_t txcycleMsgCounter;
#if LIBCANILCFG_NUMBER_OF_RX_CYCLE_MESSAGE
	uint8_t rxcycleMsgCounter;
#endif
	LibLog_Info("CAN:IL Service INIT\n");

	// Check configuration of tables
	Lib_Assert(LibCanILCfg_MessageTable.NumOfMessages == (uint8_t)LIBCANILCFG_MESSAGE_NAME_DIMENSION);
	Lib_Assert(LibCanILCfg_SignalTable.NumOfSignals == (uint16_t)LIBCANILCFG_SIGNAL_NAME_DIMENSION);
	Lib_Assert(LibCanILCfg_CallbackTable.NumOfCallbacks == (uint8_t)LIBCANILCFG_CALLBACK_NAME_DIMENSION);

	// Fill the transmit cycle/event messages in the working storage
	txcycleMsgCounter = UINT8_C(0);
	txMsgCounter = UINT8_C(0);
#if LIBCANILCFG_NUMBER_OF_RX_CYCLE_MESSAGE
	rxcycleMsgCounter = UINT8_C(0);
#endif

	for(loop = UINT8_C(0); loop < (uint8_t)LIBCANILCFG_MESSAGE_NAME_DIMENSION; loop++)
	{
		const S_LibCanIL_MessageDesc_t* pMessageDesc = &LibCanILCfg_MessageTable.pMessageDesc[loop];
		if (pMessageDesc->IsTx)
		{
#if LIBCANILCFG_NUMBER_OF_TX_EVENT_MESSAGE 
			if(pMessageDesc->CycleTime == UINT16_C(0))
			{
				LibCanIL_TxEventMessages[txMsgCounter].MessageNames = (E_LibCanILCfg_MessageNames_t)loop;
				LibCanIL_TxEventMessages[txMsgCounter].SendTimes = 0;
				txMsgCounter ++;
			}
#endif
            
#if LIBCANILCFG_NUMBER_OF_TX_CYCLE_MESSAGE
			if(pMessageDesc->CycleTime > UINT16_C(0))
			{
				LibCanIL_TxCycleMessages[txcycleMsgCounter].MessageNames = (E_LibCanILCfg_MessageNames_t)loop;
				LibCanIL_TxCycleMessages[txcycleMsgCounter].bTxCycleMsgEnabled = false;
				txcycleMsgCounter++;
			}
#endif
		}
#if LIBCANILCFG_NUMBER_OF_RX_CYCLE_MESSAGE 
		else
		{
			if(pMessageDesc->CycleTime > UINT16_C(0))
			{
				LibCanIL_RxCycleMessages[rxcycleMsgCounter].MessageNames = (E_LibCanILCfg_MessageNames_t)loop;
				LibCanIL_RxCycleMessages[rxcycleMsgCounter].MulTimeOutValue = (uint16_t)(pMessageDesc->CycleTime * 2.5 * 1.1);
				LibCanIL_RxCycleMessages[rxcycleMsgCounter].StartTimingFlag = false;
				LibCanIL_RxCycleMessages[rxcycleMsgCounter].ActualCycleValue = 0;
				LibCanIL_RxCycleMessages[rxcycleMsgCounter].MissingCnt = 0;
				LibCanIL_RxCycleMessages[rxcycleMsgCounter].FirstValueIndication = false;
				LibCanIL_RxCycleMessages[rxcycleMsgCounter].TimeOutIndication = false;
				rxcycleMsgCounter++;
			}
		}
#endif
	}

#if LIBCANILCFG_NUMBER_OF_TX_EVENT_MESSAGE 
	Lib_Assert(txMsgCounter == LIBCANILCFG_NUMBER_OF_TX_EVENT_MESSAGE);
#endif

#if LIBCANILCFG_NUMBER_OF_TX_CYCLE_MESSAGE
	Lib_Assert(txcycleMsgCounter == LIBCANILCFG_NUMBER_OF_TX_CYCLE_MESSAGE);
#endif

#if LIBCANILCFG_NUMBER_OF_RX_CYCLE_MESSAGE 
	Lib_Assert(rxcycleMsgCounter == LIBCANILCFG_NUMBER_OF_RX_CYCLE_MESSAGE);
#endif

	// check is the signal description correct in relation of the storageStartBit
	uint16_t storageStartBit = UINT16_C(0);
	for(loop = UINT8_C(0); loop < (uint8_t)LIBCANILCFG_SIGNAL_NAME_DIMENSION; loop++)
	{
		const S_LibCanIL_SignalDesc_t* pSignalDesc = &LibCanILCfg_SignalTable.pSignalDesc[loop];
		Lib_Assert(storageStartBit == pSignalDesc->StorageStartBit);
		storageStartBit += pSignalDesc->Length;
	}
	//Lib_Assert((((storageStartBit - UINT16_C(1)) >> UINT16_C(3)) + UINT16_C(1)) == LIBCANILCFG_SIGNAL_STORAGE_LENGTH);

	LibCanIL_Init();
}

//=====================================================================================================================
// LibCanIL_ServiceEvTxEventMessageTimer:
//=====================================================================================================================
#if LIBCANILCFG_NUMBER_OF_TX_EVENT_MESSAGE 
static void LibCanIL_ServiceEvTxEventMessageTimer(void)
{

    bool_t restart_timer = false;
	bool_t ret;
	const uint32_t currentTime = LibTimer_GetUpTime_ms();
	uint32_t nextCallTime = UINT32_MAX;
	uint8_t loop = UINT8_C(0);
	// check all tx event messages
	for(loop = UINT8_C(0); loop < LIBCANILCFG_NUMBER_OF_TX_EVENT_MESSAGE; loop++)
	{
		if((int32_t)(LibCanIL_TxEventMessages[loop].NextCallTime_ms - currentTime) < INT32_C(2))
		{
			const E_LibCanILCfg_MessageNames_t msgName = LibCanIL_TxEventMessages[loop].MessageNames;
			if((uint8_t)msgName < LibCanILCfg_MessageTable.NumOfMessages)
			{
				if(LibCanIL_TxEventMessages[loop].SendTimes > 0)
				{
					const S_LibCanIL_MessageDesc_t* pMsgDesc = &LibCanILCfg_MessageTable.pMessageDesc[(uint8_t)msgName];
					for(uint8_t index=0; index < pMsgDesc->NDataChCbks; index++) 
					{
						if((((uint8_t)pMsgDesc->FirstMsgRecCbk) + index) < LibCanILCfg_CallbackTable.NumOfCallbacks)
						LibCanILCfg_CallbackTable.pCallbackDesc[(pMsgDesc->FirstMsgRecCbk) + index].Callback();
					}

					// Transmit this message
					LibCanIL_TransmitMsg(msgName);

					LibCanIL_TxEventMessages[loop].SendTimes --;
					if(LibCanIL_TxEventMessages[loop].SendTimes == 0)
					{
						LibCanIL_TxEventMessages[loop].SendInterval_ms = UINT32_C(0);
						LibCanIL_TxEventMessages[loop].NextCallTime_ms = UINT32_C(0);
						if(LibCanIL_TxEventMessages[loop].TxEventMsgFinishCallback != NULL)
						{
							LibCanIL_TxEventMessages[loop].TxEventMsgFinishCallback();
						}
					}
					else
					{
						restart_timer = true;

						uint32_t IntervalTime_Temp = LibCanIL_TxEventMessages[loop].SendInterval_ms;
						LibCanIL_TxEventMessages[loop].NextCallTime_ms = currentTime + IntervalTime_Temp;
					}
				}
			}	
		}

	}
	
	if(restart_timer == true)
	{
		for(loop = UINT8_C(0); loop < LIBCANILCFG_NUMBER_OF_TX_EVENT_MESSAGE; loop++)
		{
			uint32_t NextTime_Temp;

			NextTime_Temp = LibCanIL_TxEventMessages[loop].NextCallTime_ms;

			if(NextTime_Temp != UINT32_C(0))
			{
				// calculate the next call time
				const uint32_t msgCallTime = NextTime_Temp - currentTime;
				if(nextCallTime > msgCallTime)
				{
					nextCallTime = msgCallTime;
				}
			}
		}
		// restart the LibCanIL_TxEventMsgTimer in fixed period
		ret = LibTimer_Start(&LibCanIL_TxEventMsgTimer, nextCallTime, UINT32_C(0));
	    Lib_Assert(ret);
	}
	else
	{
		LibTimer_Stop(&LibCanIL_TxEventMsgTimer);
	}
}
#endif

//=====================================================================================================================
// LibCanIL_ServiceEvTxCycleMessageTimer:
//=====================================================================================================================
#if LIBCANILCFG_NUMBER_OF_TX_CYCLE_MESSAGE
static void LibCanIL_ServiceEvTxCycleMessageTimer(void)
{
	bool_t ret;
	uint8_t loop;
	const uint32_t currentTime = LibTimer_GetUpTime_ms();
	uint32_t nextCallTime = UINT32_MAX;

	// check all cycle messages
	for(loop = UINT8_C(0); loop < LIBCANILCFG_NUMBER_OF_TX_CYCLE_MESSAGE; loop++)
	{
		
		if (true == LibCanIL_TxCycleMessages[loop].bTxCycleMsgEnabled)
		{
			// Messages that should be sent at the current time are now sent
			if ((int32_t)(LibCanIL_TxCycleMessages[loop].NextCallTime_ms - currentTime) < INT32_C(2))
			{
				const E_LibCanILCfg_MessageNames_t msgName = LibCanIL_TxCycleMessages[loop].MessageNames;
				if((uint8_t)msgName < LibCanILCfg_MessageTable.NumOfMessages)
				{
					// set the next sending time
					const S_LibCanIL_MessageDesc_t* pMsgDesc = &LibCanILCfg_MessageTable.pMessageDesc[(uint8_t)msgName];
					LibCanIL_TxCycleMessages[loop].NextCallTime_ms += (uint32_t)pMsgDesc->CycleTime;
					for(uint8_t index=0; index < pMsgDesc->NDataChCbks; index++) 
					{
						if((((uint8_t)pMsgDesc->FirstMsgRecCbk) + index) < LibCanILCfg_CallbackTable.NumOfCallbacks)
						LibCanILCfg_CallbackTable.pCallbackDesc[(pMsgDesc->FirstMsgRecCbk) + index].Callback();
					}
					// Transmit this message
					LibCanIL_TransmitMsg(msgName);//zdj
				}
			}
		}
	}
	for(loop = UINT8_C(0); loop < LIBCANILCFG_NUMBER_OF_TX_CYCLE_MESSAGE; loop++)
	{
		uint32_t CycleTime_Temp;
        if(false == LibCanIL_TxCycleMessages[loop].bTxCycleMsgEnabled)
		{
			if(LibCanILCfg_MessageTable.pMessageDesc[LibCanIL_TxCycleMessages[loop].MessageNames].StartDelayTime != UINT16_C(0))
			{
				CycleTime_Temp = LibCanILCfg_MessageTable.pMessageDesc[LibCanIL_TxCycleMessages[loop].MessageNames].StartDelayTime;
			}
			else
			{
				CycleTime_Temp = LibCanILCfg_MessageTable.pMessageDesc[LibCanIL_TxCycleMessages[loop].MessageNames].CycleTime;
			}
			LibCanIL_TxCycleMessages[loop].NextCallTime_ms = currentTime + CycleTime_Temp;
		}
		else if (LibCanIL_TxCycleMessages[loop].NextCallTime_ms < currentTime)
		{
			CycleTime_Temp = LibCanILCfg_MessageTable.pMessageDesc[LibCanIL_TxCycleMessages[loop].MessageNames].CycleTime;
			LibCanIL_TxCycleMessages[loop].NextCallTime_ms = currentTime + CycleTime_Temp;
		}

		// calculate the next call time
		const uint32_t msgCallTime = (LibCanIL_TxCycleMessages[loop].NextCallTime_ms - currentTime);
		if(nextCallTime > msgCallTime)
		{
			nextCallTime = msgCallTime;
		}
	}
	
	ret = LibTimer_Start(&LibCanIL_TxCycleMsgTimer, nextCallTime, UINT32_C(0));
	Lib_Assert(ret);
}
#endif

//=====================================================================================================================
// LibCanIL_ServiceEvRxCycleMessageTimer:
//=====================================================================================================================
#if LIBCANILCFG_NUMBER_OF_RX_CYCLE_MESSAGE 
static void LibCanIL_ServiceEvRxCycleMessageTimer(void)
{
    uint8_t loop;
	for(loop = UINT8_C(0); loop < LIBCANILCFG_NUMBER_OF_RX_CYCLE_MESSAGE; loop++)
	{
		if(LibCanIL_RxCycleMessages[loop].StartTimingFlag == true)
		{
			if(LibCanIL_RxCycleMessages[loop].ActualCycleValue < LIBCANIL_CYCLE_MSG_RECE_TIMEOUTMAX)
			{
                LibCanIL_RxCycleMessages[loop].ActualCycleValue += LIBCANIL_CYCLE_MSG_RECE_INTERVAL;
			}
			else if(LibCanIL_RxCycleMessages[loop].ActualCycleValue == LIBCANIL_CYCLE_MSG_RECE_TIMEOUTMAX)
			{
				LibCanIL_RxCycleMessages[loop].ActualCycleValue += LIBCANIL_CYCLE_MSG_RECE_INTERVAL;
                LibCanIL_RxCycleMessages[loop].TimeOutIndication = true;
				LibLog_Info("CANIL: Rx Cycle Msg [%d] is missing\n",loop);
			}

			LibCanIL_RxCycleMessages[loop].MissingCnt = 0;
		}
		else
		{
            if(LibCanIL_RxCycleMessages[loop].MissingCnt < LIBCANIL_CYCLE_MSG_RECE_TIMEOUTMAX)
			{
                LibCanIL_RxCycleMessages[loop].MissingCnt += LIBCANIL_CYCLE_MSG_RECE_INTERVAL;
			}
			else if(LibCanIL_RxCycleMessages[loop].MissingCnt == LIBCANIL_CYCLE_MSG_RECE_TIMEOUTMAX)
			{
				LibCanIL_RxCycleMessages[loop].MissingCnt += LIBCANIL_CYCLE_MSG_RECE_INTERVAL;
                LibCanIL_RxCycleMessages[loop].TimeOutIndication = true;
				LibLog_Info("CANIL: Rx Cycle Msg [%d] is missing\n",loop);
			}
		}
	}
}
#endif

//=====================================================================================================================
// LibCanIL_ServiceEvMessageInd:
//=====================================================================================================================
uint16_t Log_IL_count_temp;
static void LibCanIL_ServiceEvMessageInd(void)
{
	do
	{
		const S_LibCanIL_MsgIndBufferEntry_t* pMsg;
		pMsg = (S_LibCanIL_MsgIndBufferEntry_t*)LibFifoQueue_GetItem(&LibCanIL_MsgIndFifo, UINT32_C(0));
		
		// no message in queue
		if (pMsg == NULL)
		{
			break;
		}
		// read the current message in queue
		uint8_t loop;
		for (loop = 0; loop < LibCanILCfg_MessageTable.NumOfMessages; loop++)
		{
			const S_LibCanIL_MessageDesc_t* msgDesc = &LibCanILCfg_MessageTable.pMessageDesc[loop];
			if ((pMsg->Id == msgDesc->Id) && (msgDesc -> IsTx == false))
			{
				Log_IL_count_temp++;
				if( Log_IL_count_temp>1200 )
				{
					Log_IL_count_temp = 0;
					LibLog_Debug("LibCanIL_ReadMessage\n");
				}
#if LIBCANILCFG_NUMBER_OF_RX_CYCLE_MESSAGE 
				if(LibCanIL_RxMsgMonitor(msgDesc,(E_LibCanILCfg_MessageNames_t)loop) == true)
#endif
				{
					if (msgDesc -> IsASWHndle == true)
					{
						CAN_DATATYPE * const ASWCANFrameData = msgDesc -> ASWCANFrame;;
						ASWCANFrameData -> Extended = (uint8_T)(pMsg -> IsExtId);
						ASWCANFrameData -> Length = (uint8_T)(pMsg -> Length);
						ASWCANFrameData -> ID = (uint32_T)(pMsg -> Id);
						memcpy((void*)ASWCANFrameData -> Data, (void*)(pMsg -> Data), (size_t)(pMsg -> Length));
						msgDesc -> ASWHndleFunc();

						// Set all relevant message callbacks to Requested.
	/* 					const S_LibCanIL_MessageDesc_t* pMsgDesc = &LibCanILCfg_MessageTable.pMessageDesc[(uint8_t)msgName]; */
						const E_LibCanILCfg_CallbackNames_t firstDataChCbk = msgDesc->FirstMsgRecCbk;
						for(uint8_t cbkLoop = UINT8_C(0); cbkLoop < msgDesc->NDataChCbks; cbkLoop++)
						{
							if ((((uint8_t)firstDataChCbk) + cbkLoop) < LibCanILCfg_CallbackTable.NumOfCallbacks)
							{
								LibCanIL_CallbackIsRequested[((uint8_t)firstDataChCbk) + cbkLoop] = true;
							}
						}
					}
					else
					{
						LibCanIL_ReadMessage((E_LibCanILCfg_MessageNames_t)loop, pMsg);	
					}
				}
				
				break;
			}
		}

		LibFifoQueue_Pop(&LibCanIL_MsgIndFifo);

	}while(true);
}

//=====================================================================================================================
// LibCanIL_ServiceEvMessageCon:
//=====================================================================================================================
static void LibCanIL_ServiceEvMessageCon(void)
{
	do
	{
		const S_LibCanIL_MsgConBufferEntry_t* pMsg;
		pMsg = (S_LibCanIL_MsgConBufferEntry_t*)LibFifoQueue_GetItem(&LibCanIL_MsgConFifo, UINT32_C(0));
		// no confirm in queue
		if (pMsg == NULL)
		{
			break;
		}

		LibFifoQueue_Pop(&LibCanIL_MsgConFifo);
	}while(true);
}


//=====================================================================================================================
// LibCanIL_Init:
//=====================================================================================================================
static void LibCanIL_Init(void)
{
	uint8_t loop;

	// set the initial signal value and reset the data change counter
	for(loop = UINT8_C(0); loop < (uint8_t)LIBCANILCFG_SIGNAL_NAME_DIMENSION; loop++)
	{
		const S_LibCanIL_SignalDesc_t* pSignalDesc = &LibCanILCfg_SignalTable.pSignalDesc[loop];
		const uint64_t startValue =  pSignalDesc->StartValue;

		LibCanIL_SetSignal((E_LibCanILCfg_SignalNames_t)loop, startValue);
		LibCanIL_SignalDataChCounter[loop] = UINT8_C(0);
	}

	// reset the data change callback
	for(loop = UINT8_C(0); loop < (uint8_t)LIBCANILCFG_CALLBACK_NAME_DIMENSION; loop++)
	{
		LibCanIL_CallbackIsRequested[loop] = false;
	}

	// clear all fifos
	LibFifoQueue_Clear(&LibCanIL_MsgReqFifo);
	LibFifoQueue_Clear(&LibCanIL_MsgIndFifo);
	LibFifoQueue_Clear(&LibCanIL_MsgConFifo);
}

//=====================================================================================================================
// LibCanIL_WriteMessage:
//=====================================================================================================================
static void LibCanIL_WriteMessage(E_LibCanILCfg_MessageNames_t msgName, S_LibCan_Msg_t* pMsg)
{
	if((uint8_t)msgName < LibCanILCfg_MessageTable.NumOfMessages)
	{
		// Pointer to the message description of the message to be write
		const S_LibCanIL_MessageDesc_t* pMsgDesc = &LibCanILCfg_MessageTable.pMessageDesc[(uint8_t)msgName];

		if (pMsgDesc->IsTx == false)
		{
			LibLog_Info("LibCanIL: Remote Message\n");
			//TODO send a remote message with this message name (msgName)
		}
		else
		{
			uint8_t sigLoop;
			uint8_t dataLoop;
			uint64_t msgData = UINT64_C(0);

			// is not for CAN FD (messages > 8Byte)
			Lib_Assert(LibCan_GetMsgDataLength(pMsgDesc->Length) <= UINT8_C(8));

			// Set all relevant signals to the new message.
			const E_LibCanILCfg_SignalNames_t firstSignal = pMsgDesc->FirstSignal;
			for(sigLoop = UINT8_C(0); sigLoop < pMsgDesc->NSignals; sigLoop++)
			{
				const uint16_t curSignal = (uint8_t)firstSignal + sigLoop;
				if (curSignal < LibCanILCfg_SignalTable.NumOfSignals)
				{
					const S_LibCanIL_SignalDesc_t* pSignalDesc = &LibCanILCfg_SignalTable.pSignalDesc[curSignal];
					const uint64_t sigValue = LibCanIL_GetSignal((E_LibCanILCfg_SignalNames_t)curSignal);

					// construct message data with signal value
					msgData = LibCanIL_ConstructSignal(msgData, sigValue, pSignalDesc->MsgStartBit, pSignalDesc->Length);
				}
			}

			// copy work variable to the message data
			for (dataLoop = UINT8_C(8); dataLoop > UINT8_C(0); dataLoop--)
			{
				if(dataLoop <= LibCan_GetMsgDataLength(pMsgDesc->Length))
				{
					pMsg->Data[dataLoop - UINT8_C(1)] = (uint8_t)msgData;
				}
				msgData >>= UINT8_C(8);
			}

			// set message dependent setup
			pMsg->CanDevId = pMsgDesc->CanDevId;
			pMsg->Id = pMsgDesc->Id;
			pMsg->IsExtId = pMsgDesc->IsExtId;
			pMsg->IsCanFd = false;
			pMsg->IsBrs = false;
			pMsg->IsRemote = false;
			pMsg->Length = pMsgDesc->Length;
			pMsg->Timestamp = 0U;
		}
	}
}
/* void LibCanIL_GetMessageFree(E_LibCanILCfg_MessageNames_t msgName, S_LibCan_Msg_t* pMsg)
{
	if((uint8_t)msgName < LibCanILCfg_MessageTable.NumOfMessages)
	{
		// Pointer to the message description of the message to be write
		const S_LibCanIL_MessageDesc_t* pMsgDesc = &LibCanILCfg_MessageTable.pMessageDesc[(uint8_t)msgName];

		uint8_t sigLoop;
		uint8_t dataLoop;
		uint64_t msgData = UINT64_C(0);

		// is not for CAN FD (messages > 8Byte)
		Lib_Assert(LibCan_GetMsgDataLength(pMsgDesc->Length) <= UINT8_C(8));

		// Set all relevant signals to the new message.
		const E_LibCanILCfg_SignalNames_t firstSignal = pMsgDesc->FirstSignal;
		for(sigLoop = UINT8_C(0); sigLoop < pMsgDesc->NSignals; sigLoop++)
		{
			const uint16_t curSignal = (uint8_t)firstSignal + sigLoop;
			if (curSignal < LibCanILCfg_SignalTable.NumOfSignals)
			{
				const S_LibCanIL_SignalDesc_t* pSignalDesc = &LibCanILCfg_SignalTable.pSignalDesc[curSignal];
				const uint64_t sigValue = LibCanIL_GetSignal((E_LibCanILCfg_SignalNames_t)curSignal);

				// construct message data with signal value
				msgData = LibCanIL_ConstructSignal(msgData, sigValue, pSignalDesc->MsgStartBit, pSignalDesc->Length);
			}
		}

		// copy work variable to the message data
		for (dataLoop = UINT8_C(8); dataLoop > UINT8_C(0); dataLoop--)
		{
			if(dataLoop <= LibCan_GetMsgDataLength(pMsgDesc->Length))
			{
				pMsg->Data[dataLoop - UINT8_C(1)] = (uint8_t)msgData;
			}
			msgData >>= UINT8_C(8);
		}

//		// set message dependent setup
//		pMsg->Id = pMsgDesc->Id;
//		pMsg->IsExtId = pMsgDesc->IsExtId;
//		pMsg->IsCanFd = false;
//		pMsg->IsBrs = false;
//		pMsg->Length = pMsgDesc->Length;
	}
} */
uint8_t singal_count;
//=====================================================================================================================
// LibCanIL_ReadMessage:
//=====================================================================================================================
static void LibCanIL_ReadMessage(E_LibCanILCfg_MessageNames_t msgName, const S_LibCan_Msg_t* pMsg)
{
	if((uint8_t)msgName < LibCanILCfg_MessageTable.NumOfMessages)
	{
		// Pointer to the message description of the message to be read
		const S_LibCanIL_MessageDesc_t* pMsgDesc = &LibCanILCfg_MessageTable.pMessageDesc[(uint8_t)msgName];

		if (pMsgDesc->IsTx)
		{
			LibLog_Info("LibCanIL: Remote Message\n");
			//TODO send a message with this message name (msgName)
		}
		else
		{
			uint8_t dataLoop;
			uint8_t sigLoop;
			uint8_t cbkLoop;
			uint64_t msgData = UINT64_C(0);

			// If DLC check succeeds (and message is not for CAN FD - length > 8 Byte),
			if ((((E_LibCan_DlcSize_t)pMsg->Length) == pMsgDesc->Length)
				&& (LibCan_GetMsgDataLength(pMsg->Length) <= UINT8_C(8)))
			{


				// Copy message data to a work variable.
				for (dataLoop = UINT8_C(0); dataLoop < UINT8_C(8); dataLoop++)
				{
					msgData <<= UINT8_C(8);
					if(dataLoop < LibCan_GetMsgDataLength(pMsg->Length))
					{
						msgData += pMsg->Data[dataLoop];
					}
				}

				// Set all relevant signals to the new value.
				const uint8_t firstSignal = (uint8_t)pMsgDesc->FirstSignal;
				for(sigLoop = UINT8_C(0); sigLoop < pMsgDesc->NSignals; sigLoop++)
				{
					const uint16_t curSignal = firstSignal + sigLoop;

					if (curSignal < LibCanILCfg_SignalTable.NumOfSignals)
					{
						uint64_t newSigValue = UINT64_C(0);
						const S_LibCanIL_SignalDesc_t* pSignalDesc = &LibCanILCfg_SignalTable.pSignalDesc[curSignal];
						const uint64_t oldSigValue = LibCanIL_GetSignal((E_LibCanILCfg_SignalNames_t)curSignal);

						// extract signal value from the message data
						newSigValue = LibCanIL_ExtractSignal(msgData, pSignalDesc->MsgStartBit, pSignalDesc->Length);

						if (oldSigValue != newSigValue)
						{
							singal_count++;
							LibLog_Info("s_c %d\r\n",singal_count);
							LibCanIL_SetSignal((E_LibCanILCfg_SignalNames_t)curSignal, newSigValue);
							LibCanIL_SignalDataChCounter[curSignal]++;

							// Set all relevant data change callbacks to Requested.
							const E_LibCanILCfg_CallbackNames_t firstDataChCbk = pSignalDesc->FirstDataChCbk;
							for(cbkLoop = UINT8_C(0); cbkLoop < pSignalDesc->NDataChCbks; cbkLoop++)
							{
								if ((((uint8_t)firstDataChCbk) + cbkLoop) < LibCanILCfg_CallbackTable.NumOfCallbacks)
								{
									LibCanIL_CallbackIsRequested[((uint8_t)firstDataChCbk) + cbkLoop] = true;
								}
							}
						}
					}
				}

				// Set all relevant message callbacks to Requested.
				const E_LibCanILCfg_CallbackNames_t firstDataChCbk = pMsgDesc->FirstMsgRecCbk;
				for(cbkLoop = UINT8_C(0); cbkLoop < pMsgDesc->NDataChCbks; cbkLoop++)
				{
					if ((((uint8_t)firstDataChCbk) + cbkLoop) < LibCanILCfg_CallbackTable.NumOfCallbacks)
					{
						LibCanIL_CallbackIsRequested[((uint8_t)firstDataChCbk) + cbkLoop] = true;
					}
				}
			}
			else
			{
#ifdef CANIL_INTEGRATION_IN_PROGRESS // This is takeover from Ferrai, but at this point we need "FclMsgDtcs_CanMsgDlcFailed" to make this compile
				// Invoke DLC check failed callback.
				LIBCANIL_DLC_CHECK_FAILED_CALLBACK(msgName);
#endif
			}
		}
	}
}

//=====================================================================================================================
// LibCanIL_TransmitMsg:
//=====================================================================================================================
static void LibCanIL_TransmitMsg(E_LibCanILCfg_MessageNames_t msgName)
{
	if (((uint8_t)msgName < LibCanILCfg_MessageTable.NumOfMessages)
	 && (LibCanIL_TransmitEnabled))
	{
		bool_t stored;
		S_LibCanIL_MsgReqBufferEntry_t msgReq;

		// Pointer to the message description of the message to be transmit
		const S_LibCanIL_MessageDesc_t* pMsgDesc = &LibCanILCfg_MessageTable.pMessageDesc[(uint8_t)msgName];

		// Set the can device id for this message
		msgReq.CanDevId = pMsgDesc->CanDevId;

		if (pMsgDesc ->IsASWHndle == true)
		{
			pMsgDesc ->ASWHndleFunc();
			msgReq.IsExtId = (bool_t)(pMsgDesc ->ASWCANFrame ->Extended);
			msgReq.IsCanFd = false;
			msgReq.IsBrs   = false;
			msgReq.Id      = pMsgDesc ->ASWCANFrame ->ID;
			msgReq.Length  = (E_LibCan_DlcSize_t)(pMsgDesc ->ASWCANFrame ->Length);
			memset((void*)(msgReq.Data), 0, (size_t)(LIBCAN_MAXDATABYTENUM));
			memcpy((void*)(msgReq.Data), (void*)(pMsgDesc ->ASWCANFrame ->Data), (size_t)(pMsgDesc ->ASWCANFrame ->Length));
		}
		else
		{
			// Generate message data
			LibCanIL_WriteMessage(msgName, &msgReq);
		}

		// store the message in the transmit queue
		stored = LibFifoQueue_Push(&LibCanIL_MsgReqFifo, (void*)(&msgReq));
		(void)LibService_SetEvent(&TASK_CAN, EV_CAN_MSG_REQ);


#ifdef LIBCANIL_CYCLETX_SENDCNT_EN
        LibCanIL_CycleTx_SendNumCnt(msgName);
#endif

		// Check if item was inserted.
		if (stored == false)
		{
			LibLog_Debug("CAN: Cannot store Interaction Layer message\n");
		}
	}
}
//=====================================================================================================================
// LibCanIL_ExtractSignal:
//=====================================================================================================================
static uint64_t LibCanIL_ExtractSignal(uint64_t msgData, uint8_t msgStartBit, uint8_t length)
{
	uint64_t retValue = 0;

	// cut msgStartBit to bit position of the byte
	const uint8_t bitPos = msgStartBit & UINT8_C(7);

	// cut msgStartBit for byte position
	const uint8_t bytePos = msgStartBit >> UINT8_C(3);

	// right shifts until msgStartBit is in position of bit 0 of message
	const uint8_t rightShift = bitPos + ((UINT8_C(7) - bytePos) << UINT8_C(3));

	retValue  = msgData >> rightShift;
	if(length < UINT8_C(64))
	{
		retValue &= ((UINT64_C(1) << length) - UINT64_C(1));
	}

	return retValue;
}

//=====================================================================================================================
// LibCanIL_ConstructSignal:
//=====================================================================================================================
static uint64_t LibCanIL_ConstructSignal(uint64_t msgData, uint64_t sigValue, uint8_t msgStartBit, uint8_t length)
{
	if(length < UINT8_C(64))
	{
		uint64_t sigMask = ((UINT64_C(1) << length) - UINT64_C(1));

		// cut msgStartBit to bit position of the byte
		const uint8_t bitPos = msgStartBit & UINT8_C(7);

		// cut msgStartBit for byte position
		const uint8_t bytePos = msgStartBit >> UINT8_C(3);

		// left shifts until bit 0 of message is in position of msgStartBit
		const uint8_t leftShift = bitPos + ((UINT8_C(7) - bytePos) << UINT8_C(3));

		sigValue &= sigMask;
		sigMask = sigMask << leftShift;
		sigValue = sigValue << leftShift;

		msgData &= ~sigMask;
		msgData |= sigValue;
	}
	else
	{
		msgData = sigValue;
	}

	return msgData;
}

//=====================================================================================================================
// LibCanIL_TxEventMsgTimerCallback:
//=====================================================================================================================
#if LIBCANILCFG_NUMBER_OF_TX_EVENT_MESSAGE 
static void LibCanIL_TxEventMsgTimerCallback(void* pData)
{
	LIB_UNUSED(pData);
	(void)LibService_SetEvent(&LibCanIL_Service, LIBCANIL_EVENT_TXEVENT_MESSAGE_TIMER);
}
#endif

//=====================================================================================================================
// LibCanIL_TxCycleMsgTimerCallback:
//=====================================================================================================================
#if LIBCANILCFG_NUMBER_OF_TX_CYCLE_MESSAGE
static void LibCanIL_TxCycleMsgTimerCallback(void* pData)
{
	LIB_UNUSED(pData);
	(void)LibService_SetEvent(&LibCanIL_Service, LIBCANIL_EVENT_TXCYCLE_MESSAGE_TIMER);
}
#endif


//=====================================================================================================================
// LibCanIL_RxCycleMsgTimerCallback:
//=====================================================================================================================
#if LIBCANILCFG_NUMBER_OF_RX_CYCLE_MESSAGE 
static void LibCanIL_RxCycleMsgTimerCallback(void* pData)
{
	LIB_UNUSED(pData);
	(void)LibService_SetEvent(&LibCanIL_Service, LIBCANIL_EVENT_RXCYCLE_MESSAGE_TIMER);
}
#endif

//=====================================================================================================================
// LibCanIL_MsgIndicate:
//=====================================================================================================================
static void LibCanIL_MsgIndicate(S_LibCan_Msg_t *pMsg)
{
	Ret_t pushed = LibFifoQueue_Push(&LibCanIL_MsgIndFifo, (void*)(pMsg));
	if (!pushed)
	{
		LibLog_Warning("CAN:IL push not possible");
		LibFifoQueue_Clear(&LibCanIL_MsgIndFifo);
	}
	(void)LibService_SetEvent(&LibCanIL_Service, LIBCANIL_EVENT_CAN_MESSAGE_IND);
}

//=====================================================================================================================
// LibCanIL_MsgConfirm:
//=====================================================================================================================
static void LibCanIL_MsgConfirm(uint32_t msgId)
{
	(void)LibFifoQueue_Push(&LibCanIL_MsgConFifo, (void*)(&msgId));
	(void)LibService_SetEvent(&LibCanIL_Service, LIBCANIL_EVENT_CAN_MESSAGE_CON);
}


// --------------------------------------------------------------------------------------------------------------------
//	Global Function
// --------------------------------------------------------------------------------------------------------------------
//=====================================================================================================================
// LibCanIL_SendEventMsgStart:
//=====================================================================================================================
#if LIBCANILCFG_NUMBER_OF_TX_EVENT_MESSAGE 
void LibCanIL_SendEventMsgStart(E_LibCanILCfg_MessageNames_t msgName, uint32_t sendTimes, uint32_t sendInterval, void (*callback)(void))
{
	for(uint8_t loop = UINT8_C(0); loop < LIBCANILCFG_NUMBER_OF_TX_EVENT_MESSAGE; loop++)
	{
		if(LibCanIL_TxEventMessages[loop].MessageNames == msgName)
		{
			if(sendTimes > 0U)
			{
				const E_LibCanILCfg_MessageNames_t msgName = LibCanIL_TxEventMessages[loop].MessageNames;
				if((uint8_t)msgName < LibCanILCfg_MessageTable.NumOfMessages)
				{
					const S_LibCanIL_MessageDesc_t* pMsgDesc = &LibCanILCfg_MessageTable.pMessageDesc[(uint8_t)msgName];
					for(uint8_t index=0; index < pMsgDesc->NDataChCbks; index++) 
					{
						if((((uint8_t)pMsgDesc->FirstMsgRecCbk) + index) < LibCanILCfg_CallbackTable.NumOfCallbacks)
						LibCanILCfg_CallbackTable.pCallbackDesc[(pMsgDesc->FirstMsgRecCbk) + index].Callback();
					}

					// Transmit this message
					LibCanIL_TransmitMsg(msgName);
				}
				sendTimes -= 1;

				if(sendTimes > 0U)
				{
					const uint32_t currentTime = LibTimer_GetUpTime_ms();
					LibCanIL_TxEventMessages[loop].SendTimes = sendTimes;
					LibCanIL_TxEventMessages[loop].SendInterval_ms = sendInterval;
					LibCanIL_TxEventMessages[loop].NextCallTime_ms = currentTime + sendInterval;
					LibCanIL_TxEventMessages[loop].TxEventMsgFinishCallback = callback;

					//Start the LibCanIL_TxEventMsgTimer only if stopped.
					uint32_t timeLeft = LibTimer_GetTimeLeft_ms(&LibCanIL_TxEventMsgTimer);
					if(timeLeft == 0U)
					{
						bool_t ret = LibTimer_Start(&LibCanIL_TxEventMsgTimer, sendInterval, UINT32_C(0));
						Lib_Assert(ret);
					}
					else if(timeLeft > sendInterval)
					{
						LibTimer_Stop(&LibCanIL_TxEventMsgTimer);
						bool_t ret = LibTimer_Start(&LibCanIL_TxEventMsgTimer, sendInterval, UINT32_C(0));
						Lib_Assert(ret);
					}
				}
				else
				{
					if(callback != NULL)
					{
						callback();
					}
				}
			}
			break;
		}
	}
}
#endif

//=====================================================================================================================
// LibCanIL_SetSignal:
//=====================================================================================================================
void LibCanIL_SetSignal(E_LibCanILCfg_SignalNames_t sigName, uint64_t sigValue)
{
	uint8_t loop;
	uint64_t sigMask = UINT64_MAX;
	uint16_t data[UINT8_C(9)];
	uint16_t mask[UINT8_C(9)];

	if ((uint16_t)sigName < LibCanILCfg_SignalTable.NumOfSignals)
	{
		// Pointer to the signal description of the signal to be set
		const S_LibCanIL_SignalDesc_t* pSigDesc = &(LibCanILCfg_SignalTable.pSignalDesc[(uint8_t)sigName]);

		// length of the signal in bits
		const uint8_t lengthBit = pSigDesc->Length;

		// first byte in the storage with bits from signal
		const uint16_t startByte = (uint16_t)(pSigDesc->StorageStartBit >> UINT8_C(3));

		// first bit in the first byte from signal
		const uint8_t startBit = (uint8_t)(pSigDesc->StorageStartBit % UINT8_C(8));

		// last byte in the storage with bits from signal
		const uint16_t stopByte = (uint16_t)((pSigDesc->StorageStartBit + (uint32_t)lengthBit - UINT32_C(1)) >> UINT8_C(3));

		// number of bytes with bits from signal
		const uint8_t lengthByte = (uint8_t)(stopByte - startByte + UINT16_C(1));

		// right shifts until bit 0 of sigValue is in position of bit 0 of storage
		const uint8_t rightShift = UINT8_C(8) + startBit + lengthBit - (lengthByte << UINT8_C(3));

		Lib_Assert(stopByte < LIBCANILCFG_SIGNAL_STORAGE_LENGTH);
		Lib_Assert(lengthBit != UINT8_C(0));
		Lib_Assert(lengthBit <= UINT8_C(64));

		if (lengthBit < UINT8_C(64))
		{
			sigMask = ((UINT64_C(1) << lengthBit) - UINT64_C(1));
		}
		sigValue &= sigMask;

		// split the signal value and the mask to storage data
		data[lengthByte - UINT8_C(1)] = (uint16_t)(sigValue << UINT8_C(8));
		mask[lengthByte - UINT8_C(1)] = (uint16_t)(sigMask << UINT8_C(8));
		for (loop = (lengthByte - UINT8_C(1)); loop > UINT8_C(0); loop--)
		{
			data[loop - UINT8_C(1)] = (uint16_t)(sigValue);
			mask[loop - UINT8_C(1)] = (uint16_t)(sigMask);
			sigValue >>= UINT8_C(8);
			sigMask >>= UINT8_C(8);
		}

		// shift the signal and the mask to the right position
		for (loop = UINT8_C(0); loop < lengthByte; loop++)
		{
			data[loop] >>= rightShift;
			mask[loop] >>= rightShift;
		}

		// copy signal value to internal storage
		SuspendAllInterrupts();
		for (loop = UINT8_C(0); loop < lengthByte; loop++)
		{
			LibCanIL_SignalStorage[startByte + loop] &= ~((uint8_t)mask[loop]);
			LibCanIL_SignalStorage[startByte + loop] |= (uint8_t)data[loop];
		}
		ResumeAllInterrupts();
	}
}

//=====================================================================================================================
// LibCanIL_GetSignal:
//=====================================================================================================================
uint64_t LibCanIL_GetSignal(E_LibCanILCfg_SignalNames_t sigName)
{
	uint64_t retValue = UINT64_C(0);
	uint8_t loop;
	uint16_t data[UINT8_C(9)];

	if ((uint16_t)sigName < LibCanILCfg_SignalTable.NumOfSignals)
	{
		// Pointer to the signal description of the signal to be get
		const S_LibCanIL_SignalDesc_t* pSigDesc = &(LibCanILCfg_SignalTable.pSignalDesc[(uint8_t)sigName]);

		// length of the signal in bits
		const uint8_t lengthBit = pSigDesc->Length;

		// first byte in the storage with bits from signal
		const uint16_t startByte = (uint16_t)(pSigDesc->StorageStartBit >> UINT8_C(3));

		// first bit in the first byte from signal
		const uint8_t startBit = (uint8_t)(pSigDesc->StorageStartBit % UINT8_C(8));

		// last byte in the storage with bits from signal
		const uint16_t stopByte = (uint16_t)((pSigDesc->StorageStartBit + (uint32_t)lengthBit - UINT32_C(1)) >> UINT8_C(3));

		// number of bytes with bits from signal
		const uint8_t lengthByte = (uint8_t)(stopByte - startByte + UINT16_C(1));

		// left shifts until bit 0 of storage is in position of bit 0 of retValue
		const uint8_t leftShift = UINT8_C(8) + startBit + lengthBit - (lengthByte << UINT8_C(3));

		Lib_Assert(stopByte < LIBCANILCFG_SIGNAL_STORAGE_LENGTH);
		Lib_Assert(lengthBit != UINT8_C(0));
		Lib_Assert(lengthBit <= UINT8_C(64));

		// copy signal value from internal storage
		SuspendAllInterrupts();
		for (loop = UINT8_C(0); loop < lengthByte; loop++)
		{
			data[loop] = (uint16_t)LibCanIL_SignalStorage[startByte + loop];
		}
		ResumeAllInterrupts();

		// shift the signal to the right position
		for (loop = UINT8_C(0); loop < lengthByte; loop++)
		{
			data[loop] <<= leftShift;
		}

		// combine the storage data to the right signal value
		for (loop = UINT8_C(0); loop < (lengthByte - UINT8_C(1)); loop++)
		{
			retValue <<= UINT8_C(8);
			retValue += (uint64_t)data[loop];
		}
		data[(lengthByte - UINT8_C(1))] >>= UINT8_C(8);
		retValue += (uint64_t)data[(lengthByte - UINT8_C(1))];

		// cut the signal to the right length
		if (lengthBit < UINT8_C(64))
		{
			retValue &= ((UINT64_C(1) << lengthBit) - 1U);
		}
	}
	return retValue;
}

//=====================================================================================================================
// LibCanIL_TransmitMessage:
//=====================================================================================================================
void LibCanIL_TransmitMessage(E_LibCanILCfg_MessageNames_t msgName)
{
	if ((uint8_t)msgName < LibCanILCfg_MessageTable.NumOfMessages)
	{
		// Pointer to the message description of the message to be transmit
		const S_LibCanIL_MessageDesc_t* pMsgDesc = &LibCanILCfg_MessageTable.pMessageDesc[(uint8_t)msgName];

		if (pMsgDesc->IsTx)
		{
			// is this message not a cycle message
			if (pMsgDesc->CycleTime == UINT8_C(0))
			{
				LibCanIL_TransmitMsg(msgName);
			}
		}
		else
		{
			LibLog_Info("LibCanIL: Remote Message\n");
			//TODO send a remote message with this message name (msgName)
		}
	}
}
//=====================================================================================================================
// LibCanIL_CallRequestedCallbacks:
//=====================================================================================================================
void LibCanIL_CallRequestedCallbacks(void)
{
	uint8_t loop;

	for (loop = UINT8_C(0); loop < (uint8_t)LIBCANILCFG_CALLBACK_NAME_DIMENSION; loop++)
	{
		if(LibCanIL_CallbackIsRequested[loop])
		{
			// reset the request
			LibCanIL_CallbackIsRequested[loop] = false;
			// call the callback function
			LibCanILCfg_CallbackTable.pCallbackDesc[loop].Callback();
		}
	}
}

//=====================================================================================================================
// LibCanIL_TxStart:
//=====================================================================================================================
void LibCanIL_TxStart(void)
{
	LibLog_Info("CAN:IL TX Start\n");

	LibCanIL_TransmitEnabled = true;
    
#if( LIBCANILCFG_NUMBER_OF_TX_CYCLE_MESSAGE > 0)
	bool_t ret;
	uint8_t loop;
	const uint32_t currentTime = LibTimer_GetUpTime_ms();
	uint32_t nextCallTime = UINT32_MAX;

	for (loop = UINT8_C(0); loop < LIBCANILCFG_NUMBER_OF_TX_CYCLE_MESSAGE; loop++)
	{
		const E_LibCanILCfg_MessageNames_t msgName = LibCanIL_TxCycleMessages[loop].MessageNames;
		if((uint8_t)msgName < LibCanILCfg_MessageTable.NumOfMessages)
		{
			const S_LibCanIL_MessageDesc_t* pMsgDesc = &LibCanILCfg_MessageTable.pMessageDesc[(uint8_t)msgName];
			if(pMsgDesc->StartDelayTime != UINT16_C(0))
			{	// cycle messages with start time delay
				LibCanIL_TxCycleMessages[loop].NextCallTime_ms = currentTime + (uint32_t)pMsgDesc->StartDelayTime;
			}
			else
			{	// cycle messages without start time delay
				LibCanIL_TxCycleMessages[loop].NextCallTime_ms = currentTime + (uint32_t)pMsgDesc->CycleTime;
				if (true == LibCanIL_TxCycleMessages[loop].bTxCycleMsgEnabled)
				{
					// Transmit this message
					LibCanIL_TransmitMsg(msgName);
				}
				
			}
		}
	}

	for (loop = UINT8_C(0); loop < LIBCANILCFG_NUMBER_OF_TX_CYCLE_MESSAGE; loop++)
	{
		// calculate the next call time
		const uint32_t msgCallTime = (LibCanIL_TxCycleMessages[loop].NextCallTime_ms - currentTime);
		if(nextCallTime > msgCallTime)
		{
			nextCallTime = msgCallTime;
		}
	}

	ret = LibTimer_Start(&LibCanIL_TxCycleMsgTimer, nextCallTime, UINT32_C(0));
	Lib_Assert(ret);
#endif

}

//=====================================================================================================================
// LibCanIL_RxStart:
//=====================================================================================================================
void LibCanIL_RxStart(void)
{
	LibLog_Info("CAN:IL RX Start\n");
	LibCanIL_ReceiveEnabled = true;

#if LIBCANILCFG_NUMBER_OF_RX_CYCLE_MESSAGE 
	bool_t ret = LibTimer_Start(&LibCanIL_RxCycleMsgTimer, LIBCANIL_CYCLE_MSG_RECE_INTERVAL, LIBCANIL_CYCLE_MSG_RECE_INTERVAL);
	Lib_Assert(ret);
#endif

#ifdef REPORT_LOST_COMM_EN 
	bool_t ret = LibTimer_Start(&CanTask_LostComm_TickCnt_Timer, LOST_COMM_TIMER_TICK_VALUE, LOST_COMM_TIMER_TICK_VALUE);
	Lib_Assert(ret);
#endif

}

//=====================================================================================================================
// LibCanIL_TxStop:
//=====================================================================================================================
void LibCanIL_TxStop(void)
{
	LibLog_Info("CAN:IL TX Stop\n");
	LibCanIL_TransmitEnabled = false;

#if LIBCANILCFG_NUMBER_OF_TX_CYCLE_MESSAGE
	LibTimer_Stop(&LibCanIL_TxCycleMsgTimer);
#endif
	LibFifoQueue_Clear(&LibCanIL_MsgReqFifo);
}

//=====================================================================================================================
// LibCanIL_RxStop:
//=====================================================================================================================
void LibCanIL_RxStop(void)
{
	LibLog_Info("CAN:IL RX Stop\n");
	LibCanIL_ReceiveEnabled = false;

	LibFifoQueue_Clear(&LibCanIL_MsgIndFifo);

#if LIBCANILCFG_NUMBER_OF_RX_CYCLE_MESSAGE 
	LibTimer_Stop(&LibCanIL_RxCycleMsgTimer);
#endif

#ifdef REPORT_LOST_COMM_EN 
	LibTimer_Stop(&CanTask_LostComm_TickCnt_Timer);
#endif

}

//=====================================================================================================================
// LibCanIL_TxEnable:
//=====================================================================================================================
void LibCanIL_TxEnable(void)
{
	LibLog_Info("CAN:IL TX Enable\n");
	LibCanIL_TransmitEnabled = true;
}

//=====================================================================================================================
// LibCanIL_RxEnable:
//=====================================================================================================================
void LibCanIL_RxEnable(void)
{
	LibLog_Info("CAN:IL RX Enable\n");
	LibCanIL_ReceiveEnabled = true;
}

//=====================================================================================================================
// LibCanIL_TxDisable:
//=====================================================================================================================
void LibCanIL_TxDisable(void)
{
	LibLog_Info("CAN:IL TX Disable\n");
	LibCanIL_TransmitEnabled = false;
}

//=====================================================================================================================
// LibCanIL_RxDisable:
//=====================================================================================================================
void LibCanIL_RxDisable(void)
{
	LibLog_Info("CAN:IL RX Disable\n");
	LibCanIL_ReceiveEnabled = false;

#if LIBCANILCFG_NUMBER_OF_RX_CYCLE_MESSAGE 
	LibTimer_Stop(&LibCanIL_RxCycleMsgTimer);
#endif

}

//=====================================================================================================================
// LibCanIL_IsMsgIL:
//=====================================================================================================================
bool_t LibCanIL_IsMsgIL(uint32_t msgId)
{
	bool_t retValue = false;
	uint8_t loop;

	// is only correct if receive is enabled
	if (LibCanIL_ReceiveEnabled)
	{
		// search the Id in Interaction Layer message config
		for (loop = 0; loop < LibCanILCfg_MessageTable.NumOfMessages; loop++)
		{
			const S_LibCanIL_MessageDesc_t* pMsgDesc = &LibCanILCfg_MessageTable.pMessageDesc[loop];
			if (msgId == pMsgDesc->Id)
			{
				retValue = true;
				break;
			}
		}
	}
	return retValue;
}


//=====================================================================================================================
// LibCanIL_RxMsgMonitor:
//=====================================================================================================================
#if LIBCANILCFG_NUMBER_OF_RX_CYCLE_MESSAGE 
static bool_t LibCanIL_RxMsgMonitor(const S_LibCanIL_MessageDesc_t* msgDesc, E_LibCanILCfg_MessageNames_t msgName)
{
	bool_t retValue = false;
	uint8_t loop;
	//GWM RS-IL-27 The IL shall perform signal supervision each Periodic and Periodic/Event message 
	if(msgDesc->CycleTime > 0)
	{
		//find the Rx Cycle Msg index in LibCanIL_RxCycleMessages
		for(loop = UINT8_C(0); loop < LIBCANILCFG_NUMBER_OF_RX_CYCLE_MESSAGE; loop++)
		{
			if(LibCanIL_RxCycleMessages[loop].MessageNames == msgName)
			{
				break;
			}
		}
		
		//caculate the Rx Msg Cycle Time and jude timeout or not
		if(LibCanIL_RxCycleMessages[loop].StartTimingFlag == false)
		{
			LibCanIL_RxCycleMessages[loop].StartTimingFlag = true;
			LibCanIL_RxCycleMessages[loop].ActualCycleValue = 0;
		}
		else 
		{
			LibCanIL_RxCycleMessages[loop].StartTimingFlag = false;

			if(LibCanIL_RxCycleMessages[loop].ActualCycleValue > LibCanIL_RxCycleMessages[loop].MulTimeOutValue)
			{
				if(LibCanIL_RxCycleMessages[loop].TimeOutIndication == false)
				{
					LibCanIL_RxCycleMessages[loop].TimeOutIndication = true;
					LibLog_Info("CANIL: Rx Cycle Msg [%d] is TimeOut (ActualCycleValue: %dms  MulTimeOutValue: %dms)\n",loop, LibCanIL_RxCycleMessages[loop].ActualCycleValue, LibCanIL_RxCycleMessages[loop].MulTimeOutValue);
				}
			}
			else
			{
				if(LibCanIL_RxCycleMessages[loop].TimeOutIndication == true)
				{
					LibCanIL_RxCycleMessages[loop].TimeOutIndication = false;
					LibLog_Info("CANIL: Rx Cycle Msg [%d] is Normal (ActualCycleValue: %dms  MulTimeOutValue: %dms)\n",loop, LibCanIL_RxCycleMessages[loop].ActualCycleValue, LibCanIL_RxCycleMessages[loop].MulTimeOutValue);     
				}
			}
		}

		//Whether read the msg or not
		if(LibCanIL_RxCycleMessages[loop].FirstValueIndication == false)
		{
			LibCanIL_RxCycleMessages[loop].FirstValueIndication = true;
			LibCanIL_RxCycleMessages[loop].TimeOutIndication = false;
			LibLog_Info("CANIL: Rx Cycle Msg [%d] gets firstvalue and cycle default normal\n",loop);
		}
		else if(LibCanIL_RxCycleMessages[loop].TimeOutIndication == false)
		{
			retValue = true;
		}

	}
	else
	{
		//The event Rx Msg don't need MsgMonitor
		retValue = true;
	}
	
	return retValue;
}
#endif

//=====================================================================================================================
// LibCanIL_CycleTx_SendNumCnt:
//=====================================================================================================================
#ifdef LIBCANIL_CYCLETX_SENDCNT_EN
static void LibCanIL_CycleTx_SendNumCnt(E_LibCanILCfg_MessageNames_t msgName)
{
	//LIBCANIL_MSG_BPSPARASTATE_CYCLE required by Zephyer

}
#endif

//=====================================================================================================================
// LibCanIL_SetTxCycleMessageEnabled:
//=====================================================================================================================
#if LIBCANILCFG_NUMBER_OF_TX_CYCLE_MESSAGE
void LibCanIL_SetTxCycleMessageEnabled(E_LibCanILCfg_MessageNames_t msgName, bool enableflag)
{
	if ((uint8_t)msgName < LibCanILCfg_MessageTable.NumOfMessages)
	{
		for (uint8_t loop = UINT8_C(0); loop < LIBCANILCFG_NUMBER_OF_TX_CYCLE_MESSAGE; loop++)
		{
			if(msgName == LibCanIL_TxCycleMessages[loop].MessageNames)
			{
				LibCanIL_TxCycleMessages[loop].bTxCycleMsgEnabled = enableflag;
				return;
			}
		}
	}

}
#endif
//=====================================================================================================================
// LibCanIL_GetTxCycleMessageEnabled:
//=====================================================================================================================
#if LIBCANILCFG_NUMBER_OF_TX_CYCLE_MESSAGE
bool_t LibCanIL_GetTxCycleMessageEnabled(E_LibCanILCfg_MessageNames_t msgName)
{
	bool_t ret = false;
	if ((uint8_t)msgName < LibCanILCfg_MessageTable.NumOfMessages)
	{
		for (uint8_t loop = UINT8_C(0); loop < LIBCANILCFG_NUMBER_OF_TX_CYCLE_MESSAGE; loop++)
		{
			if(msgName == LibCanIL_TxCycleMessages[loop].MessageNames)
			{
				ret = LibCanIL_TxCycleMessages[loop].bTxCycleMsgEnabled;
				return ret;
			}
		}
	}

	return ret;
}
#endif