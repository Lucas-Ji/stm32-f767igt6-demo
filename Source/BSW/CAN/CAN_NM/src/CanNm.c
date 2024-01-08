/*
 * CanNm.c
 *
 *  Created on: 2021
 *      Author: pan.sw
 */

#include "CanNm.h"
#include "CanTask.h"

// --------------------------------------------------------------------------------------------------------------------
//  Events to the local service
// --------------------------------------------------------------------------------------------------------------------
#define CANNM_SRV_EV_MSG_INDICATE                       UINT32_C(0x00000001)
#define CANNM_SRV_EV_TRANSMIT_PDU                       UINT32_C(0x00000002)
#define CANNM_SRV_EV_TRANSMIT_PDU_TIMEOUT               UINT32_C(0x00000004)
#define CANNM_SRV_EV_NORMAL_TIMEOUT                     UINT32_C(0x00000008)
#define CANNM_SRV_EV_PREPARE_SLEEP                      UINT32_C(0x00000010)
#define CANNM_SRV_EV_PREPARE_SLEEP_TXFIFO_CLEAR         UINT32_C(0x00000020)
#define CANNM_SRV_EV_GO_TO_SLEEP                        UINT32_C(0x00000040)
#define CANNM_SRV_EV_GO_TO_SLEEP_TIMEOUT                UINT32_C(0x00000080)
#define CANNM_SRV_EV_REPEAT_MSG_TIMEOUT                 UINT32_C(0x00000100)
#define CANNM_SRV_EV_TASK                               UINT32_C(0x00000200)

// ----------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
//
// CAN NETWORK MANAGEMENT STATE MACHINE
//
// --------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------
/// \brief X-macro list - defines events of the general CAN network management state machine
///
// --------------------------------------------------------------------------------------------------------------------
#define CANNM_EVENTS(DEFINE_EVENT)          \
    DEFINE_EVENT(CANNM_EV_POWER_ON)         \
    DEFINE_EVENT(CANNM_EV_START)            \
    DEFINE_EVENT(CANNM_EV_NORMAL)           \
    DEFINE_EVENT(CANNM_EV_RELEASE)          \
    DEFINE_EVENT(CANNM_EV_REPEAT_TIMEOUT)   \
    DEFINE_EVENT(CANNM_EV_KEEP_REPEAT)      \
    DEFINE_EVENT(CANNM_EV_PREPARE_SLEEP)    \
    DEFINE_EVENT(CANNM_EV_GO_TO_SLEEP)

// --------------------------------------------------------------------------------------------------------------------
/// \brief X-macro list for FSM template - defines the states of the CAN NM and corresponding state's handlers
///
// --------------------------------------------------------------------------------------------------------------------

#define CANNM_STATE(DEFINE_STATE) \
    DEFINE_STATE(CANNM_STATE_POWER_OFF,      LibFsm_Empty,                  LibFsm_Empty,               LibFsm_Empty)          \
    DEFINE_STATE(CANNM_STATE_SLEEP,          CanNm_SleepSrv,                CanNm_SleepEntry,           CanNm_SleepExit)       \
    DEFINE_STATE(CANNM_STATE_REPEAT_MESSAGE, CanNm_RepeatMsgSrv,            CanNm_RepeatMsgEntry,       CanNm_RepeatMsgExit)   \
    DEFINE_STATE(CANNM_STATE_NORMAL,         CanNm_NormalSrv,               CanNm_NormalEntry,          CanNm_NormalExit)      \
    DEFINE_STATE(CANNM_STATE_READY_SLEEP,    CanNm_ReadySleepSrv,           CanNm_ReadySleepEntry,      CanNm_ReadySleepExit)  \
    DEFINE_STATE(CANNM_STATE_PREPARE_SLEEP,  CanNm_PrepareSleepSrv,         CanNm_PrepareSleepEntry,    CanNm_PrepareSleepExit)


// --------------------------------------------------------------------------------------------------------------------
/// \brief  Transitions definitions for the CANNM_STATE_POWER_OFF state of the CAN network manager
// --------------------------------------------------------------------------------------------------------------------
#define TRANSITIONS_FOR_STATE_CANNM_STATE_POWER_OFF(ENTRY)                      \
    ENTRY(CANNM_EV_POWER_ON,        CANNM_STATE_SLEEP,          LibFsm_Empty)

// --------------------------------------------------------------------------------------------------------------------
/// \brief  Transitions definitions for the CANNM_STATE_SLEEP state of the CAN network manager
// --------------------------------------------------------------------------------------------------------------------
#define TRANSITIONS_FOR_STATE_CANNM_STATE_SLEEP(ENTRY)                                       \
    ENTRY(CANNM_EV_START,           CANNM_STATE_REPEAT_MESSAGE, CanNm_TransitionEnterNetwork)

// --------------------------------------------------------------------------------------------------------------------
/// \brief  Transitions definitions for the CANNM_STATE_REPEAT_MESSAGE state of the CAN network manager
// --------------------------------------------------------------------------------------------------------------------
#define TRANSITIONS_FOR_STATE_CANNM_STATE_REPEAT_MESSAGE(ENTRY)                 \
    ENTRY(CANNM_EV_NORMAL,          CANNM_STATE_NORMAL,         LibFsm_Empty)   \
    ENTRY(CANNM_EV_REPEAT_TIMEOUT,  CANNM_STATE_READY_SLEEP,    LibFsm_Empty)

// --------------------------------------------------------------------------------------------------------------------
/// \brief  Transitions definitions for the CANNM_STATE_NORMAL state of the CAN network manager
// --------------------------------------------------------------------------------------------------------------------
#define TRANSITIONS_FOR_STATE_CANNM_STATE_NORMAL(ENTRY)                         \
    ENTRY(CANNM_EV_RELEASE,         CANNM_STATE_READY_SLEEP,    LibFsm_Empty)   \
    ENTRY(CANNM_EV_KEEP_REPEAT,     CANNM_STATE_REPEAT_MESSAGE, LibFsm_Empty)

// --------------------------------------------------------------------------------------------------------------------
/// \brief  Transitions definitions for the CANNM_STATE_READY_SLEEP state of the CAN network manager
// --------------------------------------------------------------------------------------------------------------------
#define TRANSITIONS_FOR_STATE_CANNM_STATE_READY_SLEEP(ENTRY)                    \
    ENTRY(CANNM_EV_KEEP_REPEAT,     CANNM_STATE_REPEAT_MESSAGE, LibFsm_Empty)   \
    ENTRY(CANNM_EV_START,           CANNM_STATE_NORMAL,         LibFsm_Empty)   \
    ENTRY(CANNM_EV_PREPARE_SLEEP,   CANNM_STATE_PREPARE_SLEEP,  LibFsm_Empty)

// --------------------------------------------------------------------------------------------------------------------
/// \brief  Transitions definitions for the CANNM_STATE_PREPARE_SLEEP state of the CAN network manager
// --------------------------------------------------------------------------------------------------------------------
#define TRANSITIONS_FOR_STATE_CANNM_STATE_PREPARE_SLEEP(ENTRY)                                \
    ENTRY(CANNM_EV_START,           CANNM_STATE_REPEAT_MESSAGE, CanNm_TransitionEnterNetwork) \
    ENTRY(CANNM_EV_GO_TO_SLEEP,     CANNM_STATE_SLEEP,          LibFsm_Empty)


LIBFSM_DECLARE_STATE_MACHINE(CanNm, CANNM_EVENTS, CANNM_STATE, NULL);

// --------------------------------------------------------------------------------------------------------------------
//  Global Function Prototypes
// --------------------------------------------------------------------------------------------------------------------
extern void CanNmHistory_PushState( E_CanNmHistory_States_t newState );
extern void CanNMHistory_Save( void );
extern void CanNMHistory_Load( void );
// --------------------------------------------------------------------------------------------------------------------
//  Local Function Prototypes
// --------------------------------------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------------------------------------
/// \brief Service Handler for the CAN Network Management
///
/// \param pData <br> User data. Not used
// --------------------------------------------------------------------------------------------------------------------
static void CanNm_ServiceHndl(void* pData);

// --------------------------------------------------------------------------------------------------------------------
/// \brief Returns whether this CAN message is part of the network management.
///
/// \details Called by the CAN driver
///
/// \param msgId
/// CAN message identifier
///
/// \retval FALSE
/// This message is not a network management message.
/// \retval TRUE
/// This message is a network management message.
// --------------------------------------------------------------------------------------------------------------------
static bool_t CanNm_IsMsgNm(uint32_t msgId);

// --------------------------------------------------------------------------------------------------------------------
/// \brief Indicates the reception of a new CAN message.
///
/// \param pMsg
/// The received CAN message.
/// \param devId
/// The device driver ID of the CAN interface that received the CAN message.
// --------------------------------------------------------------------------------------------------------------------
static void CanNm_MsgIndicate(S_LibCan_Msg_t *pMsg);

// --------------------------------------------------------------------------------------------------------------------
/// \brief Confirms the successful transmission of a CAN message.
///
/// \param msgId
/// The CAN ID that was sent successfully.
/// \param devId
/// The device driver ID of the CAN interface that received the CAN message.
// --------------------------------------------------------------------------------------------------------------------
static void CanNm_MsgConfirm(uint32_t msgId);

// --------------------------------------------------------------------------------------------------------------------
/// \brief A callback function used to indicate that the time to transmit a new NM PDU has come.
///
/// \param pData <br> User data. Not used.
// --------------------------------------------------------------------------------------------------------------------
static void CanNm_PduTransmitTimerCb(void* pData);

// --------------------------------------------------------------------------------------------------------------------
/// \brief A timer callback that indicates the PDU transmission timeout.
///
/// \param pData <br> User data. Not used.
// --------------------------------------------------------------------------------------------------------------------
static void CanNm_PduPeriodicTxTimeoutCb(void* pData);

// --------------------------------------------------------------------------------------------------------------------
/// \brief A timer callback which indicates that the CANNM_REPEAT_MESSAGE_STATE_TIME_MS period passed.
///
/// \param pData <br> User data. Not used.
// --------------------------------------------------------------------------------------------------------------------
static void CanNm_RepeatedMsgStateTimerCb(void* pData);


// --------------------------------------------------------------------------------------------------------------------
/// \brief A timer callback which indicates that the CANNM_NORMAL_STATE_TIME_MS period passed.
///
/// \param pData <br> User data. Not used.
// --------------------------------------------------------------------------------------------------------------------
static void CanNm_NormalStateTimerCb(void* pData);

// --------------------------------------------------------------------------------------------------------------------
/// \brief A timer callback which indicates that the CANNM_READY_SLEEP_STATE_TIME_MS period passed.
///
/// \param pData <br> User data. Not used.
// --------------------------------------------------------------------------------------------------------------------
static void CanNm_NmTimeoutTimerCb(void* pData);

// --------------------------------------------------------------------------------------------------------------------
/// \brief A timer callback which indicates that the CANNM_WAIT_BUS_SLEEP_TIME period passed.
///
/// \param pData <br> User data. Not used.
// --------------------------------------------------------------------------------------------------------------------
static void CanNm_WaitBusSleepTimerCb(void* pData);
static void CanNm_SleepWaitTxFifoClearTimerCb(void* pData);
// --------------------------------------------------------------------------------------------------------------------
/// \brief A timer callback of Delay Sleep.
///
/// \param pData <br> User data. Not used.
// --------------------------------------------------------------------------------------------------------------------
static void PowerDown_DelaySleepTimerCb(void* pData);

// --------------------------------------------------------------------------------------------------------------------
/// \brief A function that constructs and transmits NM PDU via CAN
// --------------------------------------------------------------------------------------------------------------------
static void CanNm_TransmitNmPdu(void);

// --------------------------------------------------------------------------------------------------------------------
//  Global Variables
// --------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------
/// \brief used to enable/disable CanNm fun
// --------------------------------------------------------------------------------------------------------------------
bool_t NM_enable_flag = true;
// --------------------------------------------------------------------------------------------------------------------
/// \brief used to count for sleep
// --------------------------------------------------------------------------------------------------------------------
uint32_t SleepToShutDownCount=CANNM_POWERDOWN_TIMEPERIOD;
// --------------------------------------------------------------------------------------------------------------------
/// \brief used to count for sleep
// --------------------------------------------------------------------------------------------------------------------

static uint32_t NM_Wakeup_Reason = NM_Wakeup_None;
// --------------------------------------------------------------------------------------------------------------------
/// \brief Service Instance of the CAN Network Management
///
/// Initialize the service with the Service handler. No data is required.
// --------------------------------------------------------------------------------------------------------------------
S_LibService_Inst_t CanNm_Service = LIBSERVICE_INIT_SERVICE(CanNm_ServiceHndl, NULL);

// --------------------------------------------------------------------------------------------------------------------
/// \brief Module Instance of the CAN Network Management
///
/// Initialize the module with the interface functions.
// --------------------------------------------------------------------------------------------------------------------
const S_LibCanModule_Module_t CanNm_Module = {
    .IsMsg          = CanNm_IsMsgNm,
    .MsgIndicate    = CanNm_MsgIndicate,
    .MsgConfirm     = CanNm_MsgConfirm
};

// --------------------------------------------------------------------------------------------------------------------
//  Local Variables
// --------------------------------------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------------------------------------
/// \brief FIFO buffer for Network Management message transfer
// --------------------------------------------------------------------------------------------------------------------
static S_LibCan_Msg_t CanNm_NmMsgSendBuffer[CANNM_NM_MSG_SEND_FIFO_ELEMENTS];
static S_LibCan_Msg_t CanNm_NmMsgRecvBuffer[CANNM_NM_MSG_RECV_FIFO_ELEMENTS];

// --------------------------------------------------------------------------------------------------------------------
/// \brief Settings for the FIFO
// --------------------------------------------------------------------------------------------------------------------
LIBFIFO_DEFINE_INST(CanNm_NmMsgSendFifo,
                   (uint32_t*)(void*)CanNm_NmMsgSendBuffer,
                   sizeof(S_LibCan_Msg_t),
                   (uint32_t)CANNM_NM_MSG_SEND_FIFO_ELEMENTS,
                   false);

LIBFIFO_DEFINE_INST(CanNm_NmMsgRecvFifo,
                   (uint32_t*)(void*)CanNm_NmMsgRecvBuffer,
                   sizeof(S_LibCan_Msg_t),
                   (uint32_t)CANNM_NM_MSG_RECV_FIFO_ELEMENTS,
                   false);


// --------------------------------------------------------------------------------------------------------------------
/// \brief  Instance of the timer used to drive the sending of NM PDUs.
// --------------------------------------------------------------------------------------------------------------------
static S_LibTimer_Inst_t CanNm_PduTransmitTimer = LIBTIMER_INIT_TIMER(CanNm_PduTransmitTimerCb, NULL);

// static S_LibTimer_Inst_t CanNm_PduImmediateTransmitTimer = LIBTIMER_INIT_TIMER(CanNm_PduImmediateTransmitTimerCb, NULL);

// --------------------------------------------------------------------------------------------------------------------
/// \brief  Instance of the timer used to guard the PDU transmission timeout.
// --------------------------------------------------------------------------------------------------------------------
static S_LibTimer_Inst_t CanNm_TxTimeout = LIBTIMER_INIT_TIMER(CanNm_PduPeriodicTxTimeoutCb, NULL);

// --------------------------------------------------------------------------------------------------------------------
/// \brief  Instance of the timer used to drive the sending of NM PDUs
// --------------------------------------------------------------------------------------------------------------------
static S_LibTimer_Inst_t CanNm_NmTimeoutTimer = LIBTIMER_INIT_TIMER(CanNm_NmTimeoutTimerCb, NULL);

// --------------------------------------------------------------------------------------------------------------------
/// \brief  Instance of the timer used to count the Repeated Message State period.
// --------------------------------------------------------------------------------------------------------------------
static S_LibTimer_Inst_t CanNm_RepeatedMsgStateTimer = LIBTIMER_INIT_TIMER(CanNm_RepeatedMsgStateTimerCb, NULL);

// --------------------------------------------------------------------------------------------------------------------
/// \brief  Instance of the timer used to count the Repeated Message State period.
// --------------------------------------------------------------------------------------------------------------------
static S_LibTimer_Inst_t CanNm_NormalStateTimer = LIBTIMER_INIT_TIMER(CanNm_NormalStateTimerCb, NULL);

// --------------------------------------------------------------------------------------------------------------------
/// \brief  Instance of the timer used to count the Wait Bus-Sleep Time
// --------------------------------------------------------------------------------------------------------------------
static S_LibTimer_Inst_t CanNm_WaitBusSleepTimer = LIBTIMER_INIT_TIMER(CanNm_WaitBusSleepTimerCb, NULL);

static S_LibTimer_Inst_t CanNm_SleepWaitTxFifoClearTimer = LIBTIMER_INIT_TIMER(CanNm_SleepWaitTxFifoClearTimerCb, NULL);

// --------------------------------------------------------------------------------------------------------------------
/// \brief  Instance of the timer used to delay sleep Time
// --------------------------------------------------------------------------------------------------------------------
static S_LibTimer_Inst_t PowerDown_DelaySleepTimer = LIBTIMER_INIT_TIMER(PowerDown_DelaySleepTimerCb, NULL);

// --------------------------------------------------------------------------------------------------------------------
/// \brief  The state of the network from the NM point of view.
// --------------------------------------------------------------------------------------------------------------------
static bool_t CanNm_NetworkRequested = false;

// --------------------------------------------------------------------------------------------------------------------
/// \brief  The state of the app frame Rx enable or not.
// --------------------------------------------------------------------------------------------------------------------
bool_t CanNm_Appframe_RxEnable = false;

// --------------------------------------------------------------------------------------------------------------------
//  Local Function
// --------------------------------------------------------------------------------------------------------------------

//=====================================================================================================================
// CanNm_IsMsgNm:
//=====================================================================================================================
static bool_t CanNm_IsMsgNm(uint32_t msgId)
{
	return CanNmMsgs_IsNmPduId(msgId);
}

//=====================================================================================================================
// CanNm_MsgIndicate:
//=====================================================================================================================
static void CanNm_MsgIndicate(S_LibCan_Msg_t *pMsg)
{
	bool_t rv = LibFifoQueue_Push(&CanNm_NmMsgRecvFifo, (void*)(pMsg));
    Lib_Assert(rv != false);
    LibService_SetEvent(&CanNm_Service, CANNM_SRV_EV_MSG_INDICATE);
}

//=====================================================================================================================
// CanNm_MsgConfirm:
//=====================================================================================================================
static void CanNm_MsgConfirm(uint32_t msgId)
{
    LibTimer_Stop(&CanNm_TxTimeout);
}
//=====================================================================================================================
// CanNM_OpenNmSwitchByJumpsignal:
//=====================================================================================================================

static void CanNM_OpenNmSwitchByJumpsignal(void)
{
	static bool_t Last_NM_enable_flag =false;
	if( Last_NM_enable_flag != NM_enable_flag )
	{
		if(NM_enable_flag != true)
		{
			Nm_NetworkMode();
		}
		else if( CanNm_GetCurrentState() == CANNM_STATE_SLEEP )
		{
			bool_t rv = LibTimer_Start(&PowerDown_DelaySleepTimer, CANNM_POWERDOWN_TIMEPERIOD, CANNM_POWERDOWN_TIMEPERIOD);
            /* call PowerDown_DelaySleepTimerCb goto shutdown when SleepToShutDownCount to 0 */
			Lib_Assert(rv != false);
		}
		Last_NM_enable_flag = NM_enable_flag;
	}
}
//=====================================================================================================================
// CanNm_ServiceHndl:
//=====================================================================================================================
static void CanNm_ServiceHndl(void* pData)
{
    static bool_t CanNm_FirstWakeup_InitFlag = false;
    CanNM_OpenNmSwitchByJumpsignal();
    // Initialization method
    if (LibService_CheckClearEvent(&CanNm_Service, LIBSERVICE_EV_INIT | LIBSERVICE_EV_RE_INIT) != false)
    {
        LibLog_Info("CanNm: Initialization");

        if(NM_enable_flag == false)
        {
        	Nm_NetworkMode();
        }
        CanNMHistory_Load();
        CanNmHistory_PushState(CANNM_HIST_OFF);
        CanNm_DispatchEvent(CANNM_EV_POWER_ON);
    }

    if (LibService_CheckClearEvent(&CanNm_Service, LIBSERVICE_EV_TRIGGER_SHUTDOWN))
    {
        LibLog_Info("CanNm: Shutdown");
        CanNMHistory_Save();
        LibService_Terminate(&CanNm_Service);
    }

    if (LibService_CheckClearEvent(&CanNm_Service, CANNM_SRV_EV_MSG_INDICATE) != false)
    {
        CanNm_RunCurrentState();
        LibFifoQueue_Pop(&CanNm_NmMsgRecvFifo);
    }

    // in repeat and normal state need send PDU period
    if (LibService_CheckClearEvent(&CanNm_Service, CANNM_SRV_EV_TRANSMIT_PDU) != false)
    {
        CanNm_TransmitNmPdu();
    }


    if (LibService_CheckClearEvent(&CanNm_Service, CANNM_SRV_EV_TRANSMIT_PDU_TIMEOUT) != false)
    {
        //LibLog_Error("CanNm - PDU transmission timed-out");
        // TODO Nm_TxTimeoutException shall be called to the CanSM module - but it doesn't exist yet
    }

    // when timerout,in network mode(include repeatmsg \ normal \ readysleep)
    // in readysleep state need jude to enter preparesleep state or not
    // in repeatmsg \ normal state, just restart the timer
    if (LibService_CheckClearEvent(&CanNm_Service, CANNM_SRV_EV_PREPARE_SLEEP) != false)
    {
        if(CanNm_GetCurrentState() == CANNM_STATE_READY_SLEEP)
        {
            CanNm_DispatchEvent(CANNM_EV_PREPARE_SLEEP);
        }
        else
        {
            //restart CanNm_NmTimeoutTimer
        	bool_t rv = LibTimer_Start(&CanNm_NmTimeoutTimer, CANNM_NM_TIMEOUT_TIME_MS, UINT32_C(0));
            Lib_Assert(rv != false);
        }
    }
    
    if (LibService_CheckClearEvent(&CanNm_Service, CANNM_SRV_EV_PREPARE_SLEEP_TXFIFO_CLEAR) != false)
    {
        LibTimer_Stop(&CanNm_SleepWaitTxFifoClearTimer);
    }

    //when timeout, in repeat msg state need to enter normal or readysleep state
    if (LibService_CheckClearEvent(&CanNm_Service, CANNM_SRV_EV_REPEAT_MSG_TIMEOUT) != false)
    {
        if(CanNm_NetworkRequested == true)
        {
            CanNm_NetworkRequested = false;
            CanNm_DispatchEvent(CANNM_EV_NORMAL);
        }
        else
        {
            CanNm_DispatchEvent(CANNM_EV_REPEAT_TIMEOUT);
        }
    }

    //when timerout, in normal state need to jude to enter readysleep state or not
    if (LibService_CheckClearEvent(&CanNm_Service, CANNM_SRV_EV_NORMAL_TIMEOUT) != false)
    {
        if(CanNm_NetworkRequested == true)
        {
            CanNm_NetworkRequested = false;
            //restart CanNm_NormalStateTimer
            bool_t rv = LibTimer_Start(&CanNm_NormalStateTimer, CANNM_NORMAL_STATE_TIME_MS, UINT32_C(0));
            Lib_Assert(rv != false);
        }
        else
        {
            Nm_RemoteSleepIndication();
        }
    }

    //when timeout, in preparesleep state need to enter sleep state
    if (LibService_CheckClearEvent(&CanNm_Service, CANNM_SRV_EV_GO_TO_SLEEP) != false)
    {
        CanNm_DispatchEvent(CANNM_EV_GO_TO_SLEEP);
    }

    if (LibService_CheckClearEvent(&CanNm_Service, CANNM_SRV_EV_GO_TO_SLEEP_TIMEOUT) != false)
    {
        LibTimer_Stop(&PowerDown_DelaySleepTimer);
    }
}

//=====================================================================================================================
// CanNm_PduTransmitTimerCb:
//=====================================================================================================================
static void CanNm_PduTransmitTimerCb(void* pData)
{
    LibService_SetEvent(&CanNm_Service, CANNM_SRV_EV_TRANSMIT_PDU);
}

// static void CanNm_PduImmediateTransmitTimerCb(void* pData)
// {
//     LibService_SetEvent(&CanNm_Service, CANNM_SRV_EV_IMMEDIATE_TRANSMIT_PDU);
// }

//=====================================================================================================================
// CanNm_PduPeriodicTxTimeoutCb:
//=====================================================================================================================
static void CanNm_PduPeriodicTxTimeoutCb(void* pData)
{
    LibService_SetEvent(&CanNm_Service, CANNM_SRV_EV_TRANSMIT_PDU_TIMEOUT);
}


//=====================================================================================================================
// CanNm_RepeatedMsgStateTimerCb:
//=====================================================================================================================
static void CanNm_RepeatedMsgStateTimerCb(void* pData)
{
    if(NM_Wakeup_Reason != NM_Wakeup_None)
    {
        CanNm_NetworkRequested = true;
    }
    LibService_SetEvent(&CanNm_Service, CANNM_SRV_EV_REPEAT_MSG_TIMEOUT);
}

//=====================================================================================================================
// CanNm_NormalStateTimerCb:
//=====================================================================================================================
static void CanNm_NormalStateTimerCb(void* pData)
{

    LibService_SetEvent(&CanNm_Service, CANNM_SRV_EV_NORMAL_TIMEOUT);

}

//=====================================================================================================================
// CanNm_NmTimeoutTimer:
//=====================================================================================================================
static void CanNm_NmTimeoutTimerCb(void* pData)
{
    if(NM_Wakeup_Reason != NM_Wakeup_None)
    {
        CanNm_NetworkRequested = true;
    }
    LibService_SetEvent(&CanNm_Service, CANNM_SRV_EV_PREPARE_SLEEP);
}

//=====================================================================================================================
// CanNm_WaitBusSleepTimer:
//=====================================================================================================================
static void CanNm_WaitBusSleepTimerCb(void* pData)
{
    LibService_SetEvent(&CanNm_Service, CANNM_SRV_EV_GO_TO_SLEEP);
}

static void CanNm_SleepWaitTxFifoClearTimerCb(void* pData)
{
    void *pMsg = LibFifoQueue_GetItem(&LibCanIL_MsgReqFifo, UINT32_C(0));
    if (pMsg == NULL)
    {
        LibCanIL_TxStop();
        LibService_SetEvent(&CanNm_Service, CANNM_SRV_EV_PREPARE_SLEEP_TXFIFO_CLEAR);
    }
}
//=====================================================================================================================
// PowerDown_DelaySleepTimerCb:
//=====================================================================================================================
static void PowerDown_DelaySleepTimerCb(void* pData)
{
	if(SleepToShutDownCount>=CANNM_POWERDOWN_TIMEPERIOD)
	{
		SleepToShutDownCount-=CANNM_POWERDOWN_TIMEPERIOD;
	}
    else if( (SleepToShutDownCount==0)&&(NM_enable_flag) )
	{
		LibCanIL_TxDisable();
		LibCanIL_RxStop();
#if 0
		LibCanTP_TxDisable();
		LibCanTP_RxStop();
#endif /* jianggang */

		SLEEP_TO_SHUT_DOWN();
		LibLog_Info(": SleepTimerCb go to SLEEP");
        LibService_SetEvent(&CanNm_Service, CANNM_SRV_EV_GO_TO_SLEEP_TIMEOUT);
	}
}

//=====================================================================================================================
// CanNm_SleepEntry: FSM Sleep State Entry
//=====================================================================================================================
static void CanNm_SleepEntry(void* pData)
{
    LibLog_Info("CanNm: Enter New State - SLEEP");
    CanNmHistory_PushState(CANNM_HIST_BUS_SLEEP_MODE);

    CanNm_Appframe_RxEnable = false;

    Nm_BusSleepMode();
    bool_t rv = LibTimer_Start(&PowerDown_DelaySleepTimer, CANNM_POWERDOWN_TIMEPERIOD, CANNM_POWERDOWN_TIMEPERIOD);
    Lib_Assert(rv != false);
}

//=====================================================================================================================
// CanNm_SleepSrv: FSM Sleep State Service
//=====================================================================================================================
static void CanNm_SleepSrv(void* pData)
{
    S_LibCan_Msg_t* msg = LibFifoQueue_GetItem(&CanNm_NmMsgRecvFifo, UINT32_C(0));
    if (msg != NULL)
    {
        Nm_NetworkStartIndication();
        LibFifoQueue_Pop(&CanNm_NmMsgRecvFifo);
    }
}
//=====================================================================================================================
// CanNm_SleepExit: FSM Sleep State Exit
//=====================================================================================================================
static void CanNm_SleepExit(void* pData)
{
    LibTimer_Stop(&PowerDown_DelaySleepTimer);
}
//=====================================================================================================================
// CanNm_RepeatMessageEntry: FSM Repeat Message State Entry
//=====================================================================================================================
static void CanNm_RepeatMessageImmediateTransmissionCb(void)
{
    if(LibTimer_GetTimeLeft_ms(&CanNm_PduTransmitTimer) == 0U)
    {
        bool_t rv = LibTimer_Start(&CanNm_PduTransmitTimer, CANNM_PDU_TRANSMIT_PERIOD_MS, CANNM_PDU_TRANSMIT_PERIOD_MS);
        Lib_Assert(rv != false);
    }
}

static void CanNm_RepeatMsgEntry(void* pData)
{
	LibLog_Info("CanNm: Enter New State - RepeatMsg");
    CanNmHistory_PushState(CANNM_HIST_REPEAT_MESSAGE_STATE);

    LibCanIL_SetSignal(LIBCANIL_NMSIG_COMMONTESTTX_CBV_REPEAT_MSG_REQ,0x01);//fill in the byte of RepeatMsg State : 0-in repeatmsg state
    LibCanIL_SetSignal(LIBCANIL_NMSIG_COMMONTESTTX_CBV_ACTIVE_WAKEUP,0x00);//fill in the byte of wakeup reason : bit1-1 can wake-up

    CanNm_Appframe_RxEnable = true; //enable appframe rx
    CanNm_NetworkRequested = false; //when enter repeatmsg state, need clear the CanNm_NetworkRequested

   //start CanNm_RepeatedMsgStateTimer
    bool_t rv = LibTimer_Start(&CanNm_RepeatedMsgStateTimer, CANNM_REPEAT_MESSAGE_STATE_TIME_MS, UINT32_C(0));
    Lib_Assert(rv != false);

    if(NM_Wakeup_Local & NM_Wakeup_Reason)
    {
        LibCanIL_SendEventMsgStart(LIBCANIL_MSG_COMMONTESTTX_NM, CanNmImmediateNmTransmissions, CanNmImmediateNmCycleTime, CanNm_RepeatMessageImmediateTransmissionCb);
    }
    else
    {
        // Start the transmittimer only if stopped.
        if(LibTimer_GetTimeLeft_ms(&CanNm_PduTransmitTimer) == 0U)
        {
            rv = LibTimer_Start(&CanNm_PduTransmitTimer, CANNM_PDU_TRANSMIT_PERIOD_MS, CANNM_PDU_TRANSMIT_PERIOD_MS);
            Lib_Assert(rv != false);
        }
    }

    //start CanNm_NmTimeoutTimer
    if(LibTimer_GetTimeLeft_ms(&CanNm_NmTimeoutTimer) == 0U)
    {
        rv = LibTimer_Start(&CanNm_NmTimeoutTimer, CANNM_NM_TIMEOUT_TIME_MS, UINT32_C(0));
        Lib_Assert(rv != false);
    }
}

//=====================================================================================================================
// CanNm_RepeatMsgSrv: FSM Repeat Msg State Service
//=====================================================================================================================
static void CanNm_RepeatMsgSrv(void* pData)
{
    const S_LibCan_Msg_t* const msg = LibFifoQueue_GetItem(&CanNm_NmMsgRecvFifo, UINT32_C(0));
    if (msg != NULL)
    {
        //restart CanNm_NmTimeoutTimer
        if(LibTimer_GetTimeLeft_ms(&CanNm_NmTimeoutTimer) != 0U)
        {
            LibTimer_Stop(&CanNm_NmTimeoutTimer);
        }
        bool_t rv = LibTimer_Start(&CanNm_NmTimeoutTimer, CANNM_NM_TIMEOUT_TIME_MS, UINT32_C(0));
        Lib_Assert(rv != false);

        CanNm_NetworkRequested = true;
        LibFifoQueue_Pop(&CanNm_NmMsgRecvFifo);
    }
}

//=====================================================================================================================
// CanNm_RepeatMessageExit: FSM Repeat Message State Exit
//=====================================================================================================================
static void CanNm_RepeatMsgExit(void* pData)
{
    LibCanIL_SetSignal(LIBCANIL_NMSIG_COMMONTESTTX_CBV_REPEAT_MSG_REQ,0x00);//fill in the byte of RepeatMsg State : 1-not in repeatmsg state
	LibTimer_Stop(&CanNm_RepeatedMsgStateTimer);
}

//=====================================================================================================================
// CanNm_ReadySleepEntry: FSM Ready Sleep State Entry
//=====================================================================================================================
static void CanNm_ReadySleepEntry(void* pData)
{
    LibLog_Info("CanNm: Enter New State - ReadySleep");
    CanNmHistory_PushState(CANNM_HIST_READY_SLEEP_STATE);

    LibTimer_Stop(&CanNm_PduTransmitTimer);
}

//=====================================================================================================================
// CanNm_ReadySleepSrv: FSM Ready Sleep State Service
//=====================================================================================================================
static void CanNm_ReadySleepSrv(void* pData)
{

    const S_LibCan_Msg_t* const msg = LibFifoQueue_GetItem(&CanNm_NmMsgRecvFifo, UINT32_C(0));
    if (msg != NULL)
    {
        //restart CanNm_NmTimeoutTimer
        if(LibTimer_GetTimeLeft_ms(&CanNm_NmTimeoutTimer) != 0U)
        {
            LibTimer_Stop(&CanNm_NmTimeoutTimer);
        }
        bool_t rv = LibTimer_Start(&CanNm_NmTimeoutTimer, CANNM_NM_TIMEOUT_TIME_MS, UINT32_C(0));
        Lib_Assert(rv != false);

        if (CanNmMsgs_IsRSRSet(msg) == true)
        {
            CanNm_DispatchEvent(CANNM_EV_KEEP_REPEAT); //enter repeat msg state
        }
        else
        {
             CanNm_DispatchEvent(CANNM_EV_START); //enter normal state
        }

        CanNm_NetworkRequested = true; // network in requested

        LibFifoQueue_Pop(&CanNm_NmMsgRecvFifo);
    }
}
//=====================================================================================================================
// CanNm_NormalEntry: FSM Normal State Entry
//=====================================================================================================================
static void CanNm_NormalEntry(void* pData)
{
    LibLog_Info("CanNm: Enter New State - Normal");
    CanNmHistory_PushState(CANNM_HIST_NORMAL_OPERATION_STATE);

   // Start the timer only if stopped.
    if(LibTimer_GetTimeLeft_ms(&CanNm_PduTransmitTimer) == 0U)
    {
        bool_t rv = LibTimer_Start(&CanNm_PduTransmitTimer, CANNM_PDU_TRANSMIT_PERIOD_MS, CANNM_PDU_TRANSMIT_PERIOD_MS);
        Lib_Assert(rv != false);
    }

    // start the nomal state timer
    bool_t rv = LibTimer_Start(&CanNm_NormalStateTimer, CANNM_NORMAL_STATE_TIME_MS, UINT32_C(0));
    Lib_Assert(rv != false);
}

//=====================================================================================================================
// CanNm_NormalSrv: FSM Ready Sleep State Service
//=====================================================================================================================
static void CanNm_NormalSrv(void* pData)
{
    const S_LibCan_Msg_t* const msg = LibFifoQueue_GetItem(&CanNm_NmMsgRecvFifo, UINT32_C(0));
    if (msg != NULL)
    {
        //restart CanNm_NmTimeoutTimer
        if(LibTimer_GetTimeLeft_ms(&CanNm_NmTimeoutTimer) != 0U)
        {
            LibTimer_Stop(&CanNm_NmTimeoutTimer);
        }
        bool_t rv = LibTimer_Start(&CanNm_NmTimeoutTimer, CANNM_NM_TIMEOUT_TIME_MS, UINT32_C(0));
        Lib_Assert(rv != false);
        
        if(CanNmMsgs_IsRSRSet(msg) == true)
        {
            CanNm_DispatchEvent(CANNM_EV_KEEP_REPEAT); //enter repeat msg state
        }

        CanNm_NetworkRequested = true; // network in requested

        LibFifoQueue_Pop(&CanNm_NmMsgRecvFifo);
    }
}
//=====================================================================================================================
// CanNm_NormalExit: FSM Ready Sleep State Exit
//=====================================================================================================================
static void CanNm_NormalExit(void* pData)
{
    LibTimer_Stop(&CanNm_NormalStateTimer);
}

//=====================================================================================================================
// CanNm_ReadySleepExit: FSM Ready Sleep State Exit
//=====================================================================================================================
static void CanNm_ReadySleepExit(void* pData)
{
    //LibTimer_Stop(&CanNm_NmTimeoutTimer);
}

//=====================================================================================================================
// CanNm_PrepareSleepEntry: FSM Prepare Sleep State Entry
//=====================================================================================================================
static void CanNm_PrepareSleepEntry(void* pData)
{
    LibLog_Info("CanNm: Enter New State - PrepareSleep");

    CanNmHistory_PushState(CANNM_HIST_PREPARE_BUS_SLEEP_MODE);

    if(NM_enable_flag != false)
    {
    	Nm_PrepareBusSleepMode();
    }
    CanNm_Appframe_RxEnable = false;

    LibTimer_Stop(&CanNm_NmTimeoutTimer);

    bool_t rv = LibTimer_Start(&CanNm_SleepWaitTxFifoClearTimer, CANNM_SLEEP_WAIT_TXFIFO_CLEAR_TIME_MS, CANNM_SLEEP_WAIT_TXFIFO_CLEAR_TIME_MS);
    Lib_Assert(rv != false);

    rv = LibTimer_Start(&CanNm_WaitBusSleepTimer, CANNM_WAIT_BUS_SLEEP_TIME, UINT32_C(0));
    Lib_Assert(rv != false);
}

//=====================================================================================================================
// CanNm_PrepareSleepSrv: FSM Ready Sleep State Service
//=====================================================================================================================
static void CanNm_PrepareSleepSrv(void* pData)
{
    const S_LibCan_Msg_t* const msg = LibFifoQueue_GetItem(&CanNm_NmMsgRecvFifo, UINT32_C(0));
    if (msg != NULL)
    {
        Nm_NetworkStartIndication();
        LibFifoQueue_Pop(&CanNm_NmMsgRecvFifo);
    }
}

//=====================================================================================================================
// CanNm_PrepareSleepExit: FSM Ready Sleep State Exit
//=====================================================================================================================
static void CanNm_PrepareSleepExit(void* pData)
{
    LibTimer_Stop(&CanNm_WaitBusSleepTimer);
}

//=====================================================================================================================
// CanNm_TransmitNmPdu
//=====================================================================================================================
static void CanNm_TransmitNmPdu(void)
{
    S_LibCan_Msg_t msg;
    CanNmMsgs_BuildNmPDU(&msg);
    bool_t rv = LibFifoQueue_Push(&CanNm_NmMsgSendFifo, (void*)(&msg));
    Lib_Assert(rv != false);
    //pan.sw delete
	LibService_SetEvent(&TASK_CAN, EV_CAN_MSG_REQ);
    rv = LibTimer_Start(&CanNm_TxTimeout, CANNM_PDU_TX_TIMEOUT_MS, UINT32_C(0));
    Lib_Assert(rv != false);
}

//=====================================================================================================================
// CanNm_TransitionEnterNetwork: FSM transition when entering Network sub-machine
//=====================================================================================================================
static void CanNm_TransitionEnterNetwork(void* pData)
{
    Nm_NetworkMode();
}

//=====================================================================================================================
// CanNM_RequestNetwork
//=====================================================================================================================
void CanNM_RequestNetwork(void)
{
    CanNm_NetworkRequested = true;
    CanNm_DispatchEvent(CANNM_EV_START);
}

//=====================================================================================================================
// CanNM_ReleaseNetwork
//=====================================================================================================================
void CanNM_ReleaseNetwork(void)
{
    CanNm_NetworkRequested = false;
    CanNm_DispatchEvent(CANNM_EV_RELEASE);
}
//=====================================================================================================================
// CanNM_GetCanNM_Status
//=====================================================================================================================
NM_Status CanNM_GetCanNM_Status(void)
{
	static NM_Status value = NM_SLEEP;
	if( (CanNm_GetCurrentState() == CANNM_STATE_NORMAL) ||
		(NM_enable_flag != true))
    {
		value = NM_NORMAL;
    }
	else if(CanNm_GetCurrentState() == CANNM_STATE_SLEEP)
	{
		value = NM_SLEEP;
	}
   return value;
}

static uint32_t CanNM_GetWakeUpReason(void)
{
    return NM_Wakeup_Reason;
}

void CanNM_SetWakeUpReason(uint32_t reason)
{
    NM_Wakeup_Reason |= reason;
    if(NM_Wakeup_None != NM_Wakeup_Reason)
    {
        Nm_NetworkStartIndication();
    }
}

void CanNM_ClearWakeUpReason(uint32_t reason)
{
    uint32_t ReasonTemp = ~reason;
    NM_Wakeup_Reason &= ReasonTemp;
}