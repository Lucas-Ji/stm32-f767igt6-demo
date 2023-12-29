// --------------------------------------------------------------------------------------------------------------------
///
/// \file Can.h
///
/// \brief CAN task.
///
///
/// Copyright (c) 2021 Neusoft.
/// All Rights Reserved.
///
// --------------------------------------------------------------------------------------------------------------------


#ifndef CANTASK_H_INCLUDED
#define CANTASK_H_INCLUDED

// --------------------------------------------------------------------------------------------------------------------
//	Includes
// --------------------------------------------------------------------------------------------------------------------
#include "CanTask_Cfg.h"
#include "LibService.h"
#include "LibTimer.h"
// --------------------------------------------------------------------------------------------------------------------
//	Global Definitions
// --------------------------------------------------------------------------------------------------------------------

#ifdef  REPORT_LOST_COMM_EN

#include "LibCanCfg_FiltTbl.h"
/* #include "DiagDtcMgr.h"
#include "DiagDtcMgrCfg.h" */

typedef struct 
{
	uint8_t  TimeCnt: 7;
	uint8_t  IsLost:  1;
	uint8_t  ReportCnt;
} ECU_LOST_S;

#define LOST_COMM_TIMER_TICK_VALUE                (200U)   //200ms
#define LOST_COMM_TIMEOUT_VALUE_MAX               (7U)    //7S
#define LOST_COMM2_TIMEOUT_VALUE_MAX              (25U)    //25S
#define LOST_COMM_RECOVERY_CNT                    (5U)     
#define LOST_COMM_REPORT_CNTMAX                   (3U)     

//should keep the order with LibCanIntLayCfg_Msgs_CAN

#define LOST_COMM_WITH_CMP1_INDEX                 (0U)
#define LOST_COMM_WITH_CMP2_INDEX                 (1U)

#if BSWDCMulti_Variant || CANDCMulti_Variant
#define LOST_COMM_WITH_PRC_INDEX                  (2U)
#define LOST_COMM_WITH_BMS_INDEX                  (3U)
#define LOST_COMM_INDEX_MAX                       (4U)
#else
#define LOST_COMM_WITH_BMS_INDEX                  (2U)
#define LOST_COMM_INDEX_MAX                       (3U)
#endif

extern S_LibTimer_Inst_t CanTask_LostComm_TickCnt_Timer;

#endif
// --------------------------------------------------------------------------------------------------------------------
// CAN events
// --------------------------------------------------------------------------------------------------------------------
#define EV_CAN_ALIVE							UINT32_C(0x00000001)	//!< Alive event
#define EV_CAN_SERVICE_HOST						UINT32_C(0x00000002)	//!< Service host event
#define EV_CAN_MSG_RCVD 						UINT32_C(0x00000004)	//!< CAN message received event
#define EV_CAN_WAKEUP	 						UINT32_C(0x00000008)	//!< CAN wake up request is received
#define EV_CAN_MSG_REQ							UINT32_C(0x00000010)	//!< CAN message request
#define EV_CAN_MSG_CON							UINT32_C(0x00000020)	//!< CAN message confirmation
#define EV_CAN_BUS_OFF							UINT32_C(0x00000040)	//!< CAN bus off
#define EV_TRIGGER_SHUTDOWN                     UINT32_C(0x00000080)	
#define EV_CAN_LOSTCOMM_TIMEOUT                 UINT32_C(0x00000100)

// --------------------------------------------------------------------------------------------------------------------
//	Global Data Types
// --------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------
/// \brief  Definitions of the possible states of the CAN transceiver which can be set or checked over
/// LIBCANTR_IOCTL_SET_STATE or LIBCANTR_IOCTL_GET_STATE commands
/// \sa #LIBCANTR_IOCTL_SET_STATE
/// \sa #LIBCANTR_IOCTL_GET_STATE
// --------------------------------------------------------------------------------------------------------------------
typedef enum
{
	LIBCANTR_STATE_SLEEP,		//!< the transceiver is in the sleep state or on the way into sleep state
	LIBCANTR_STATE_STANDBY,		//!< the CAN transceiver is in the first level power save state
	LIBCANTR_STATE_WAKEUP_REQ,	//!< the CAN transceiver is in the sleep or standby mode but the wake up request has been detected
	LIBCANTR_STATE_NORMAL,		//!< the CAN transceiver is enabled - CAN communication possible
	LIBCANTR_STATE_LISTEN_ONLY,	//!< the CAN transceiver is in listen only mode - Only receiver part is operating
	LIBCANTR_STATE_ERROR		//!< CAN bus or internal error have been detected
} E_LibCanTr_State_t;


// --------------------------------------------------------------------------------------------------------------------
/// \brief The function will be called when Receive Can Msg in CANIF.
// --------------------------------------------------------------------------------------------------------------------
extern void Can_HandleCanMsgs(pLibDrv_Hndl_t hndl);
// --------------------------------------------------------------------------------------------------------------------
//	Imported Variables
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
//	Imported Variables
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
//	Global Function Prototypes
// --------------------------------------------------------------------------------------------------------------------
extern S_LibService_Inst_t TASK_CAN;
// --------------------------------------------------------------------------------------------------------------------
/// \brief Get CAN transceiver state.
///
// --------------------------------------------------------------------------------------------------------------------
E_LibCanTr_State_t Can_GetCanTransceiverState(void);
// --------------------------------------------------------------------------------------------------------------------
/// \brief Set CAN transceiver state.
///
/// \param state
/// New state of CAN transceiver.
// --------------------------------------------------------------------------------------------------------------------
void Can_SetCanTransceiverState(E_LibCanTr_State_t state);

// /// \copydoc LibCanTr_WakeUpReqCallback_t
void Can_TrcvWakeUpCallback(void);

// --------------------------------------------------------------------------------------------------------------------
/// \brief The callback function to be invoked on CAN message received.
///
/// \param pData
/// Pointer to the user data
///
/// \note This function is called from interrupt context
// --------------------------------------------------------------------------------------------------------------------
void Can_MsgReceived(void* pData);

// --------------------------------------------------------------------------------------------------------------------
/// \brief CAN message sent callback function.
///
/// \param id
/// CAN ID of the sent message.
///
/// \note The callback function is called from interrupt context.
// --------------------------------------------------------------------------------------------------------------------
void Can_MsgSent(uint32_t id);

// --------------------------------------------------------------------------------------------------------------------
/// \brief Callback function in case of bus off error.
///
/// \param pData
/// User data
///
/// \note The callback function is called from interrupt context.
// --------------------------------------------------------------------------------------------------------------------
extern void Can_BusOff(void* pData);

extern void TASK_CAN_ServiceHostInit(void);

extern void TASK_CAN_ServiceHostMain(void);


#endif // CAN_H__INCLUDED
