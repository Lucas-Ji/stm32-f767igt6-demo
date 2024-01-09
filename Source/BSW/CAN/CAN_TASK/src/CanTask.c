// --------------------------------------------------------------------------------------------------------------------
///
/// \file Can.c
///
/// \brief CAN task.
///
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
#include "CanTask.h"
#include "CanIF.h"
#include "LibCanMsg.h"
#include "LibCanDrvMsg.h"
#include "LibCanIL.h"
#include "LibCanTp.h"
#include "LibTimer.h"
#include "LibCanModule.h"
#include "LibService.h"
#include "LibServiceHost.h"
#include "LibTypes.h"
#include "CanNm.h"

#if 0
#include "xcp_can.h"
#endif /* jianggang */



// --------------------------------------------------------------------------------------------------------------------
//	Local Definitions
// --------------------------------------------------------------------------------------------------------------------
#ifdef DiagDTCManage
extern void DiagDtcMgr_DtcErrorNotification(const DiagDtc_Id_t dtcId, const DiagDtcMgr_ErrorState_t errorState);
#endif /* jianggang */
// --------------------------------------------------------------------------------------------------------------------
/// \brief Mask of all CAN events
// --------------------------------------------------------------------------------------------------------------------
#define CAN_ALL_EVENTS_MASK		(EV_CAN_ALIVE		        \
								| EV_CAN_MSG_RCVD	        \
								| EV_CAN_WAKEUP		        \
								| EV_CAN_MSG_REQ	        \
								| EV_CAN_MSG_CON	        \
								| EV_CAN_BUS_OFF	        \
								| EV_CAN_LOSTCOMM_TIMEOUT	\
								| EV_TRIGGER_SHUTDOWN)

// --------------------------------------------------------------------------------------------------------------------
/// \brief The period in milliseconds within CAN should keep amplifier alive after CAN is woken up, before first valid
/// CAN message is received.
// --------------------------------------------------------------------------------------------------------------------
#define CAN_BACK_TO_SLEEP_TIME_MS			1000U

#define CAN_MSGSENT_BUFFER_ELEMENTS			UINT8_C(16)

#define LIBCANTASK_MSG_CON_FIFO_ELEMENTS	(LIBCANTP_MSG_CON_FIFO_ELEMENTS + CANNM_NM_MSG_CON_FIFO_ELEMENTS)


// --------------------------------------------------------------------------------------------------------------------
//	Local Data Types
// --------------------------------------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------------------------------------
//	Global Variables
// --------------------------------------------------------------------------------------------------------------------
const S_LibCanModule_Module_t* Can_ModuleTable[] = {
	&LibCanIL_Module,
#ifdef LIBCANTP
	&LibCanTp_Module,
#endif /* jianggang */
	&CanNm_Module,
#ifdef XCPSERVICE
	&Xcp_Module
#endif /* jianggang */
};

const uint8_t CanCfg_NumberOfModules = sizeof(Can_ModuleTable) / sizeof(Can_ModuleTable[0]);

// --------------------------------------------------------------------------------------------------------------------
//	Local Variables
// --------------------------------------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------------------------------------
/// \brief The flag whether CAN init or not.
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
/// \brief The flag whether CAN wake up request is present.
// --------------------------------------------------------------------------------------------------------------------
//static bool_t Can_IsWakeUpRequested = true;

static S_LibService_Inst_t* const Can_ServiceTable[] = {
	&TASK_CAN,
	&LibCanIL_Service,
#ifdef LIBCANTP
	&LibCanTp_Service,
#endif /* jianggang */
	&CanNm_Service,
#ifdef XCP_USING_LIBFIFO
	&LibXcp_Service,
#endif
};

static const S_LibServiceHost_Inst_t Can_ServiceHost = {
		NULL,
		NULL,
		Can_ServiceTable,
		sizeof(Can_ServiceTable) / sizeof(Can_ServiceTable[0])
};

void TASK_CAN_ServiceHndl(void* pData);
// --------------------------------------------------------------------------------------------------------------------
//	Local Function Prototypes
// --------------------------------------------------------------------------------------------------------------------
S_LibService_Inst_t TASK_CAN = LIBSERVICE_INIT_SERVICE(TASK_CAN_ServiceHndl, NULL);

// --------------------------------------------------------------------------------------------------------------------
/// \brief Get and process all available CAN messages.
///
/// \param hndl
/// The handler of CAN driver.
// --------------------------------------------------------------------------------------------------------------------
static void Can_TransmitCanMsgs(void);
static void Can_ConfirmCanMsgs(void);

static uint32_t Can_MsgSentFifoBuffer[LIBCANTASK_MSG_CON_FIFO_ELEMENTS];

LIBFIFO_DEFINE_INST(Can_MsgSentFifo, Can_MsgSentFifoBuffer, sizeof(uint32_t), LIBCANTASK_MSG_CON_FIFO_ELEMENTS, false);

// --------------------------------------------------------------------------------------------------------------------
/// \brief Can_StartNormalComm
///
/// \param void
// --------------------------------------------------------------------------------------------------------------------
static void Can_StartNormalComm(void);


// --------------------------------------------------------------------------------------------------------------------
/// \brief Lost Comm Report
///
/// \param void
// --------------------------------------------------------------------------------------------------------------------
#ifdef  REPORT_LOST_COMM_EN

#if BSWDCMulti_Variant || CANDCMulti_Variant
static ECU_LOST_S Ecu_LostComm_TimeCnt[LOST_COMM_INDEX_MAX] = { {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0} };
#else
static ECU_LOST_S Ecu_LostComm_TimeCnt[LOST_COMM_INDEX_MAX] = { {0,0,0}, {0,0,0}, {0,0,0}};
#endif

// --------------------------------------------------------------------------------------------------------------------
/// \brief A timer callback which indicates.
///
/// \param 
// --------------------------------------------------------------------------------------------------------------------
static void CanTask_LostComm_MsgCheck(uint32_t msgId);

// --------------------------------------------------------------------------------------------------------------------
/// \brief A timer callback which indicates.
///
/// \param pData <br> User data. Not used.
// --------------------------------------------------------------------------------------------------------------------
static void CanTask_LostComm_TickCnt(void);

// --------------------------------------------------------------------------------------------------------------------
/// \brief A timer callback which indicates.
///
/// \param pData <br> User data. Not used.
// --------------------------------------------------------------------------------------------------------------------
static void CanTask_LostComm_TickCnt_TimerCb(void* pData);

// --------------------------------------------------------------------------------------------------------------------
/// \brief  Instance of the timer used to lost comm
// --------------------------------------------------------------------------------------------------------------------
S_LibTimer_Inst_t CanTask_LostComm_TickCnt_Timer = LIBTIMER_INIT_TIMER(CanTask_LostComm_TickCnt_TimerCb, NULL);

#endif


// --------------------------------------------------------------------------------------------------------------------
//	Functions
// --------------------------------------------------------------------------------------------------------------------
//=====================================================================================================================
// LibSystem_WakeUpRequest:
//=====================================================================================================================
void LibSystem_WakeUpRequest(const uint32_t runTime_ms)
{
	uint32_t runTime_Temp = runTime_ms/CANNM_POWERDOWN_TIMEPERIOD;
	SleepToShutDownCount = runTime_Temp * CANNM_POWERDOWN_TIMEPERIOD;
}

//=====================================================================================================================
// Can_GetCanTransceiverState:
//=====================================================================================================================
E_LibCanTr_State_t Can_GetCanTransceiverState(void)
{
	return LIBCANTR_STATE_NORMAL;
}
//=====================================================================================================================
// Can_SetCanTransceiverState:
//=====================================================================================================================
void Can_SetCanTransceiverState(E_LibCanTr_State_t state)
{
	// TODO
}
//=====================================================================================================================
// Can_TrcvWakeUpCallback:
//=====================================================================================================================
void Can_TrcvWakeUpCallback(void)
{
	LibSystem_WakeUpRequest( CAN_BACK_TO_SLEEP_TIME_MS);
	(void)LibService_SetEvent(&TASK_CAN, EV_CAN_WAKEUP);
}
//=====================================================================================================================
// Can_MsgReceived:
//=====================================================================================================================
void Can_MsgReceived(void* pData)
{
	LIB_UNUSED(pData);
	(void)LibService_SetEvent(&TASK_CAN, EV_CAN_MSG_RCVD);
}

//=====================================================================================================================
// Can_MsgSent:
//=====================================================================================================================
void Can_MsgSent(uint32_t msgId)
{
	LibFifoQueue_Push(&Can_MsgSentFifo, (const void*)&msgId);
	(void)LibService_SetEvent(&TASK_CAN, EV_CAN_MSG_CON);
	Lib_Assert(Can_MsgSentFifo.Count > 0);
}

//=====================================================================================================================
// Can_BusOff:
//=====================================================================================================================
void Can_BusOff(void* pData)
{
	LibLog_Info("CAN: Callback Can_BusOff");
	LIB_UNUSED(pData);
	(void)LibService_SetEvent(&TASK_CAN, EV_CAN_BUS_OFF);
}

void Can_TriggerShutdown(const S_LibServiceHost_Inst_t* const pServiceHost)
{
	LibServiceHost_TriggerShutdown(pServiceHost);
}
#if 0
//=====================================================================================================================
// CanTask_AllService_Iint:
//=====================================================================================================================
static void CanTask_AllService_Init(void)
{
	LibLog_Info("CanTask: init all service.\n");
	for (uint8_t i = 0; i < Can_ServiceHost.ServiceCount; i++)
	{
		(void)LibService_SetEvent(Can_ServiceTable[i], LIBSERVICE_EV_INIT); 
		Can_ServiceTable[i]->ServiceFunc(&i);
	}
}

//=====================================================================================================================
// CanTask_AllService_Iint:
//=====================================================================================================================
static void CanTask_AllService_Polling(void)
{
	//LibLog_Info("CanTask: polling all service.\n");
	for (uint8_t i = 0; i < Can_ServiceHost.ServiceCount; i++)
	{
		Can_ServiceTable[i]->ServiceFunc(&i);
	}
}
#endif
//=====================================================================================================================
// TASK_CAN_ServiceHndl:
//=====================================================================================================================
void TASK_CAN_ServiceHostInit(void)
{
	// initialize the service host
	LibServiceHost_Init(&Can_ServiceHost);
}

void TASK_CAN_ServiceHostMain(void)
{
	LibServiceHost_Service(&Can_ServiceHost);
	//LibCanIL_CallRequestedCallbacks();
}


void TASK_CAN_ServiceHndl(void* pData)
{
	if (LibService_CheckClearEvent(&TASK_CAN, LIBSERVICE_EV_INIT))
	{
		LibLog_Info("CANTASK Service INIT\n");
		Can1IfDrv_Init();
		Can2IfDrv_Init();
		LibMcan_IoCtl(NULL,LIBCAN_IOCTL_START);
	}

	// Reinitialize the service
	if (LibService_CheckClearEvent(&TASK_CAN, LIBSERVICE_EV_RE_INIT))
	{
		LibLog_Info("CANTASK Service REINIT\n");
		Can1IfDrv_Deinit();
		Can2IfDrv_Deinit();
		Can1IfDrv_Init();
		Can2IfDrv_Init();
		LibMcan_IoCtl(NULL,LIBCAN_IOCTL_START);
	}
	
	if (LibService_CheckClearEvent(&TASK_CAN, LIBSERVICE_EV_TRIGGER_SHUTDOWN))
	{
		LibLog_Info("CANTSAK Service Trigger shutdown\n");
		LibService_Terminate(&TASK_CAN);
		Can1IfDrv_Deinit();
		Can2IfDrv_Deinit();
	}

	if(LibService_CheckClearEvent(&TASK_CAN, EV_CAN_ALIVE))
	{

	}

	if(LibService_CheckClearEvent(&TASK_CAN, EV_CAN_SERVICE_HOST))
	{
		
	}

	if(LibService_CheckClearEvent(&TASK_CAN, EV_CAN_WAKEUP))
	{
		#if 0
		Can_StartNormalComm();
		#endif
	}

	if(LibService_CheckClearEvent(&TASK_CAN, EV_CAN_MSG_RCVD))
	{
		Can_HandleCanMsgs(NULL);
		LibCanIL_CallRequestedCallbacks();
	}

	if(LibService_CheckClearEvent(&TASK_CAN, EV_CAN_MSG_REQ))
	{
		Can_TransmitCanMsgs();
	}

	if(LibService_CheckClearEvent(&TASK_CAN, EV_CAN_MSG_CON))
	{
		Can_ConfirmCanMsgs();
	}

	if(LibService_CheckClearEvent(&TASK_CAN, EV_CAN_BUS_OFF))
	{
		if(Can1_Bus_Off_flag == true)
		{
			Can1_Bus_Off();
		}
		if(Can2_Bus_Off_flag == true)
		{
			Can2_Bus_Off();
		}
	}

#ifdef  REPORT_LOST_COMM_EN
	if(LibService_CheckClearEvent(&TASK_CAN, EV_CAN_LOSTCOMM_TIMEOUT))
	{
		CanTask_LostComm_TickCnt();
	}
#endif

	if(LibService_CheckClearEvent(&TASK_CAN, EV_TRIGGER_SHUTDOWN))
	{
		Can_TriggerShutdown(&Can_ServiceHost);
	}
}


//=====================================================================================================================
// Can_HandleCanMsgs:
//=====================================================================================================================
void Can_HandleCanMsgs(pLibDrv_Hndl_t hndl)
{
	Ret_t ret = LIBRET_NO_ENTRY;

	// fetch messages until there are no messages to fetch anymore
	do
	{
		S_LibCan_Msg_t	msg;
		ret = LibMcan_IoCtl(&msg, LIBCAN_IOCTL_GET_NEXT_MSG);
		if (LIBRET_OK == ret)
		{
#ifdef REPORT_LOST_COMM_EN 
			CanTask_LostComm_MsgCheck(msg.Id);
#endif

			if(CanNm_Appframe_RxEnable == false)
			{
				//only chcek the CAN NM module is "interested" in the confirm singal
				if(CanNm_Module.IsMsg(msg.Id))
				{
					//invoke the indicate function of CAN NM module
					CanNm_Module.MsgIndicate(&msg);
				}
			}
			else
			{
				// loop over CAN modules
				for (uint8_t i = 0U; i < CanCfg_NumberOfModules; i++)
				{
					// check if the current module is "interested" in the confirm signal
					if (Can_ModuleTable[i]->IsMsg(msg.Id))
					{
						//invoke the indicate function of that module
						Can_ModuleTable[i]->MsgIndicate(&msg);
					}
				}
			}

			//ret = LIBRET_NO_ENTRY;
		}
		else if (LIBRET_NO_ENTRY == ret)
		{
			// do nothing - all messages fetched
		}
		else
		{
			LibLog_Info("CAN: Cannot handle message: 0x%X", ret);
		}
	}
	while (LIBRET_OK == ret);
}


//=====================================================================================================================
// Can_TransmitCanMsgs:
//=====================================================================================================================
static void Can_TransmitCanMsgs(void)
{
	Ret_t    ret  = LIBRET_OK;

    //handles all transmit massages from Network Management 
     do
     {
         S_LibCan_Msg_t* pMsg;
         pMsg = (S_LibCan_Msg_t*)LibFifoQueue_GetItem(&CanNm_NmMsgSendFifo, UINT32_C(0));
         if(pMsg == NULL)
         {
             break;
         }
 
         ret = LibMcan_IoCtl(pMsg, LIBCAN_IOCTL_SEND_MSG);
         if (LIBRET_OK != ret)
         {
             LibLog_Info("CAN: Cannot handle message: %d", ret);
         }
         LibFifoQueue_Pop(&CanNm_NmMsgSendFifo);
     }
     while (true);

	//handles all transmit massages from Transport Protocol
	#if 0
	do
	{
		S_LibCanTp_MsgReqBufferEntry_t*	pMsg;
		pMsg = (S_LibCanTp_MsgReqBufferEntry_t*)LibFifoQueue_GetItem(&LibCanTp_MsgReqFifo, UINT32_C(0));
		if(pMsg == NULL)
		{
			break;
		}

		ret = LibMcan_IoCtl ((S_LibCan_Msg_t*)&pMsg, LIBCAN_IOCTL_SEND_MSG);
		if (LIBRET_OK != ret)
		{
			LibLog_Info("CAN: Cannot handle message: %d", ret);
		}
		LibFifoQueue_Pop(&LibCanTp_MsgReqFifo);
	}
	while (true);
	#endif /* jianggang */

	// handles all transmit massages from Interaction Layer
	do
	{
		S_LibCanIL_MsgReqBufferEntry_t*	pMsg;
		pMsg = (S_LibCanIL_MsgReqBufferEntry_t*)LibFifoQueue_GetItem(&LibCanIL_MsgReqFifo, UINT32_C(0));
		if(pMsg == NULL)
		{
			break;
		}
		
		ret = LibMcan_IoCtl(&pMsg, LIBCAN_IOCTL_SEND_MSG);
		if (LIBRET_OK != ret)
		{
			LibLog_Info("CAN: Cannot handle message: %d", ret);
		}
		LibFifoQueue_Pop(&LibCanIL_MsgReqFifo);
	}
	while (true);

#ifdef XCP_USING_LIBFIFO
	// handles all transmit massages from Xcp
	do
	{
		S_LibXcp_MsgReqBufferEntry_t*	pMsg;
		pMsg = (S_LibXcp_MsgReqBufferEntry_t*)LibFifoQueue_GetItem(&LibXcp_MsgReqFifo, UINT32_C(0));
		if(pMsg == NULL)
		{
			break;
		}

		ret = LibMcan_IoCtl(&pMsg, LIBCAN_IOCTL_SEND_MSG);
		if (LIBRET_OK != ret)
		{
			LibLog_Info("CAN: Cannot handle message: %d", ret);
		}
		LibFifoQueue_Pop(&LibXcp_MsgReqFifo);
	}
	while (true);
#endif
}

//=====================================================================================================================
// Can_ConfirmCanMsgs:
//=====================================================================================================================
static void Can_ConfirmCanMsgs(void)
{
	uint8_t i;

	// fetch messages until there are no messages to fetch anymore
	do
	{
		// The Can_MsgSentFifo is modified from the ISR context (see \ref Can_MsgSent).
		//DisableAllInterrupts();
		uint32_t* pId      = LibFifoQueue_GetItem(&Can_MsgSentFifo, UINT32_C(0));
		bool_t   msgFound = (pId != NULL);
		uint32_t id;
		if (msgFound)
		{
			id = *pId;
			LibFifoQueue_Pop(&Can_MsgSentFifo);
		}
		//EnableAllInterrupts();

		if(!msgFound)
		{
			break;
		}

		// loop over CAN modules
		for (i = 0U; i < CanCfg_NumberOfModules; i++)
		{
			// check if the current module is "interested" in the confirm signal
			if (Can_ModuleTable[i]->IsMsg(id))
			{
				// invoke the confirm function of that module
				Can_ModuleTable[i]->MsgConfirm(id);
			}
		}
		
	}while(true);
}


//=====================================================================================================================
// Can_StartNormalComm:
//=====================================================================================================================
static void Can_StartNormalComm(void)
{
	 //Enable the CANIL 
	 LibCanIL_RxStart();
	 LibCanIL_TxStart();
	 //Enable the CANTP
	 #if 0
	 LibCanTP_RxEnable();
	 LibCanTP_TxEnable();
	 #endif
}


#ifdef  REPORT_LOST_COMM_EN
//=====================================================================================================================
// CanTask_LostComm_TickCnt_TimerCb:
//=====================================================================================================================
static void CanTask_LostComm_TickCnt_TimerCb(void* pData)
{
	(void)LibService_SetEvent(&TASK_CAN, EV_CAN_LOSTCOMM_TIMEOUT);
}

//=====================================================================================================================
// CanTask_LostComm_TickCnt:
//=====================================================================================================================
static void CanTask_LostComm_TickCnt(void)
{
	static uint8_t sOneSecTickCnt = 0;
	if(++ sOneSecTickCnt >= 5) // 1000ms cycle
	{
		sOneSecTickCnt = 0;
		for (uint8_t i = 0; i < LOST_COMM_INDEX_MAX; i++)
		{
			if(!Ecu_LostComm_TimeCnt[i].IsLost) 
			{
				Ecu_LostComm_TimeCnt[i].TimeCnt ++;

				if(Ecu_LostComm_TimeCnt[i].TimeCnt >= LOST_COMM_TIMEOUT_VALUE_MAX)
				{
					switch (i)
					{
						case LOST_COMM_WITH_BMS_INDEX:
							Ecu_LostComm_TimeCnt[LOST_COMM_WITH_BMS_INDEX].IsLost = true;
							Ecu_LostComm_TimeCnt[LOST_COMM_WITH_BMS_INDEX].ReportCnt = LOST_COMM_REPORT_CNTMAX;
							//DiagDtcMgr_DtcErrorNotification(DIAGDTC_ID_VCU_LOSES_COMM_WITH_BMS,DIAGDTCMGR_ERROR_STATE_FAILURE);
							break;
						

						default:
							break;
					}
				}
				if(Ecu_LostComm_TimeCnt[i].TimeCnt >= LOST_COMM2_TIMEOUT_VALUE_MAX)
				{
					switch (i)
					{

						case LOST_COMM_WITH_CMP1_INDEX:
							Ecu_LostComm_TimeCnt[LOST_COMM_WITH_CMP1_INDEX].IsLost = true;
							Ecu_LostComm_TimeCnt[LOST_COMM_WITH_CMP1_INDEX].ReportCnt = LOST_COMM_REPORT_CNTMAX;
							//DiagDtcMgr_DtcErrorNotification(DIAGDTC_ID_VCU_LOSES_COMM_WITH_COPPRESSOR_1,DIAGDTCMGR_ERROR_STATE_FAILURE);
							break;
						
						case LOST_COMM_WITH_CMP2_INDEX:
							Ecu_LostComm_TimeCnt[LOST_COMM_WITH_CMP2_INDEX].IsLost = true;
							Ecu_LostComm_TimeCnt[LOST_COMM_WITH_CMP2_INDEX].ReportCnt = LOST_COMM_REPORT_CNTMAX;
							//DiagDtcMgr_DtcErrorNotification(DIAGDTC_ID_VCU_LOSES_COMM_WITH_COPPRESSOR_2,DIAGDTCMGR_ERROR_STATE_FAILURE);
							break;

#if BSWDCMulti_Variant || CANDCMulti_Variant
						case LOST_COMM_WITH_PRC_INDEX:
							Ecu_LostComm_TimeCnt[LOST_COMM_WITH_PRC_INDEX].IsLost = true;
							Ecu_LostComm_TimeCnt[LOST_COMM_WITH_PRC_INDEX].ReportCnt = LOST_COMM_REPORT_CNTMAX;
							//DiagDtcMgr_DtcErrorNotification(DIAGDTC_ID_VCU_LOSES_COMM_WITH_HIG_PRECHARGE_1,DIAGDTCMGR_ERROR_STATE_FAILURE);
							break;
#endif
						
						default:
							break;
					}
				}
			}
		}
	}

	//report the lost communication dtc periodly 200ms
	if(Ecu_LostComm_TimeCnt[LOST_COMM_WITH_BMS_INDEX].ReportCnt > 0)
	{   
		Ecu_LostComm_TimeCnt[LOST_COMM_WITH_BMS_INDEX].ReportCnt --;
		if(Ecu_LostComm_TimeCnt[LOST_COMM_WITH_BMS_INDEX].IsLost)
		{
			DiagDtcMgr_DtcErrorNotification(DIAGDTC_ID_VCU_LOSES_COMM_WITH_BMS,DIAGDTCMGR_ERROR_STATE_FAILURE);
		}
		else
		{
			DiagDtcMgr_DtcErrorNotification(DIAGDTC_ID_VCU_LOSES_COMM_WITH_BMS,DIAGDTCMGR_ERROR_STATE_NOTFAILURE);
		}
	}
	else
	{
		if(Ecu_LostComm_TimeCnt[LOST_COMM_WITH_BMS_INDEX].IsLost == 0)
		{
			DiagDtcMgr_DtcErrorNotification(DIAGDTC_ID_VCU_LOSES_COMM_WITH_BMS,DIAGDTCMGR_ERROR_STATE_NOTFAILURE);
		}
	}
		
    if(Ecu_LostComm_TimeCnt[LOST_COMM_WITH_CMP1_INDEX].ReportCnt > 0)
	{
		Ecu_LostComm_TimeCnt[LOST_COMM_WITH_CMP1_INDEX].ReportCnt --;
		if(Ecu_LostComm_TimeCnt[LOST_COMM_WITH_CMP1_INDEX].IsLost)
		{
			DiagDtcMgr_DtcErrorNotification(DIAGDTC_ID_VCU_LOSES_COMM_WITH_COPPRESSOR_1,DIAGDTCMGR_ERROR_STATE_FAILURE);
		}
		else
		{
			DiagDtcMgr_DtcErrorNotification(DIAGDTC_ID_VCU_LOSES_COMM_WITH_COPPRESSOR_1,DIAGDTCMGR_ERROR_STATE_NOTFAILURE);
		}
	}
    else
	{
		if(Ecu_LostComm_TimeCnt[LOST_COMM_WITH_CMP1_INDEX].IsLost == 0)
		{
			DiagDtcMgr_DtcErrorNotification(DIAGDTC_ID_VCU_LOSES_COMM_WITH_COPPRESSOR_1,DIAGDTCMGR_ERROR_STATE_NOTFAILURE);
		}
	}
    
	if(Ecu_LostComm_TimeCnt[LOST_COMM_WITH_CMP2_INDEX].ReportCnt > 0)
	{
		Ecu_LostComm_TimeCnt[LOST_COMM_WITH_CMP2_INDEX].ReportCnt --;
		if(Ecu_LostComm_TimeCnt[LOST_COMM_WITH_CMP2_INDEX].IsLost)
		{
			DiagDtcMgr_DtcErrorNotification(DIAGDTC_ID_VCU_LOSES_COMM_WITH_COPPRESSOR_2,DIAGDTCMGR_ERROR_STATE_FAILURE);
		}
		else
		{
			DiagDtcMgr_DtcErrorNotification(DIAGDTC_ID_VCU_LOSES_COMM_WITH_COPPRESSOR_2,DIAGDTCMGR_ERROR_STATE_NOTFAILURE);
		}
	}
	else
	{
		if(Ecu_LostComm_TimeCnt[LOST_COMM_WITH_CMP2_INDEX].IsLost == 0)
		{
			DiagDtcMgr_DtcErrorNotification(DIAGDTC_ID_VCU_LOSES_COMM_WITH_COPPRESSOR_2,DIAGDTCMGR_ERROR_STATE_NOTFAILURE);
		}
	}
    
#if BSWDCMulti_Variant || CANDCMulti_Variant
	if(Ecu_LostComm_TimeCnt[LOST_COMM_WITH_PRC_INDEX].ReportCnt > 0)
	{
		Ecu_LostComm_TimeCnt[LOST_COMM_WITH_PRC_INDEX].ReportCnt --;
		if(Ecu_LostComm_TimeCnt[LOST_COMM_WITH_PRC_INDEX].IsLost)
		{
			DiagDtcMgr_DtcErrorNotification(DIAGDTC_ID_VCU_LOSES_COMM_WITH_HIG_PRECHARGE_1,DIAGDTCMGR_ERROR_STATE_FAILURE);
		}
		else
		{
			DiagDtcMgr_DtcErrorNotification(DIAGDTC_ID_VCU_LOSES_COMM_WITH_HIG_PRECHARGE_1,DIAGDTCMGR_ERROR_STATE_NOTFAILURE);
		}
	}
	else
	{
		if(Ecu_LostComm_TimeCnt[LOST_COMM_WITH_PRC_INDEX].IsLost == 0)
		{
			DiagDtcMgr_DtcErrorNotification(DIAGDTC_ID_VCU_LOSES_COMM_WITH_HIG_PRECHARGE_1,DIAGDTCMGR_ERROR_STATE_NOTFAILURE);
		}
	}
#endif
}


//=====================================================================================================================
// CanTask_LostComm_MsgCheck:
//=====================================================================================================================
uint32_t CanMsgFilter_TimeOut(uint8_t index);
static void CanTask_LostComm_MsgCheck(uint32_t msgId)
{
	// search the Id in Interaction Layer message config
	for (uint8_t loop = 0; loop < LOST_COMM_INDEX_MAX; loop++)
	{
		uint32_t Id = CanMsgFilter_TimeOut(loop);
		if (msgId == Id)
		{
			switch (loop)
			{
				case LOST_COMM_WITH_BMS_INDEX:
				    if(Ecu_LostComm_TimeCnt[LOST_COMM_WITH_BMS_INDEX].IsLost)
					{
						Ecu_LostComm_TimeCnt[LOST_COMM_WITH_BMS_INDEX].IsLost = false;
						Ecu_LostComm_TimeCnt[LOST_COMM_WITH_BMS_INDEX].ReportCnt = LOST_COMM_REPORT_CNTMAX;
					    //DiagDtcMgr_DtcErrorNotification(DIAGDTC_ID_VCU_LOSES_COMM_WITH_BMS,DIAGDTCMGR_ERROR_STATE_NOTFAILURE);
					}

					Ecu_LostComm_TimeCnt[LOST_COMM_WITH_BMS_INDEX].TimeCnt = 0;
					break;
					
				case LOST_COMM_WITH_CMP1_INDEX:
					if(Ecu_LostComm_TimeCnt[LOST_COMM_WITH_CMP1_INDEX].IsLost)
					{
						Ecu_LostComm_TimeCnt[LOST_COMM_WITH_CMP1_INDEX].IsLost = false;
						Ecu_LostComm_TimeCnt[LOST_COMM_WITH_CMP1_INDEX].ReportCnt = LOST_COMM_REPORT_CNTMAX;
					    //DiagDtcMgr_DtcErrorNotification(DIAGDTC_ID_VCU_LOSES_COMM_WITH_COPPRESSOR_1,DIAGDTCMGR_ERROR_STATE_NOTFAILURE);
					}

					Ecu_LostComm_TimeCnt[LOST_COMM_WITH_CMP1_INDEX].TimeCnt = 0;
					break;
				
				case LOST_COMM_WITH_CMP2_INDEX:
					if(Ecu_LostComm_TimeCnt[LOST_COMM_WITH_CMP2_INDEX].IsLost)
					{
						Ecu_LostComm_TimeCnt[LOST_COMM_WITH_CMP2_INDEX].IsLost = false;
						Ecu_LostComm_TimeCnt[LOST_COMM_WITH_CMP2_INDEX].ReportCnt = LOST_COMM_REPORT_CNTMAX;
					    //DiagDtcMgr_DtcErrorNotification(DIAGDTC_ID_VCU_LOSES_COMM_WITH_COPPRESSOR_2,DIAGDTCMGR_ERROR_STATE_NOTFAILURE);
					}

					Ecu_LostComm_TimeCnt[LOST_COMM_WITH_CMP2_INDEX].TimeCnt = 0;
					break;
				
#if BSWDCMulti_Variant || CANDCMulti_Variant
				case LOST_COMM_WITH_PRC_INDEX:
					if(Ecu_LostComm_TimeCnt[LOST_COMM_WITH_PRC_INDEX].IsLost)
					{
						Ecu_LostComm_TimeCnt[LOST_COMM_WITH_PRC_INDEX].IsLost = false;
						Ecu_LostComm_TimeCnt[LOST_COMM_WITH_PRC_INDEX].ReportCnt = LOST_COMM_REPORT_CNTMAX;
					    //DiagDtcMgr_DtcErrorNotification(DIAGDTC_ID_VCU_LOSES_COMM_WITH_HIG_PRECHARGE_1,DIAGDTCMGR_ERROR_STATE_NOTFAILURE);
					}

					Ecu_LostComm_TimeCnt[LOST_COMM_WITH_PRC_INDEX].TimeCnt = 0;
					break;
#endif
				
				default:
					break;
			}
		}
	}
}

#endif

