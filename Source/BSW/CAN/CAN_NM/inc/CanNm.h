/*
 * CanNm.h
 *
 *  Created on: 2021锟斤拷10锟斤拷28锟斤拷
 *      Author: pan.sw
 */

#ifndef BSW_CAN_CAN_NM_INC_CANNM_H_
#define BSW_CAN_CAN_NM_INC_CANNM_H_

#include "CanNmCfg.h"
#include "LibFsm.h"
#include "LibService.h"
#include "LibCanModule.h"
#include "LibFifoQueue.h"
#include "CanNmHistory.h"
#include "CanNmMgr.h"
#include "CanNmMsgs.h"
#include "LibTimer.h"
#include "LibCanIL.h"
#if 0
#include "LibCanTp.h"
#endif /* jianggang for uds */

#define SLEEP_TO_SHUT_DOWN() 

// --------------------------------------------------------------------------------------------------------------------
/// \brief Number of messages allowed in the SEND FIFO
// --------------------------------------------------------------------------------------------------------------------
#define CANNM_NM_MSG_SEND_FIFO_ELEMENTS 8

// --------------------------------------------------------------------------------------------------------------------
/// \brief Number of messages allowed in the RECEIVE FIFO
// --------------------------------------------------------------------------------------------------------------------
#define CANNM_NM_MSG_RECV_FIFO_ELEMENTS 8

// --------------------------------------------------------------------------------------------------------------------
/// \brief Number of messages allowed in the CONFIRM FIFO
// --------------------------------------------------------------------------------------------------------------------
#define CANNM_NM_MSG_CON_FIFO_ELEMENTS 8

#define CANNM_POWERDOWN_TIMEPERIOD                       UINT32_C(10)

#define	NM_Wakeup_None		UINT32_C(0x00000000)
#define	NM_Wakeup_Local		UINT32_C(0x00000001)
#define	NM_Wakeup_Remote	UINT32_C(0x00000002)


/// \brief Can NM status
typedef enum
{
	NM_SLEEP,
	NM_NORMAL
} NM_Status;
// --------------------------------------------------------------------------------------------------------------------
/// \brief used to enable/disable CanNm fun
// --------------------------------------------------------------------------------------------------------------------
extern bool_t NM_enable_flag;
/// \brief Service Instance of the CAN Network Management
// --------------------------------------------------------------------------------------------------------------------
extern S_LibService_Inst_t CanNm_Service;

// --------------------------------------------------------------------------------------------------------------------
/// \brief Module Instance of the CAN Network Management
// --------------------------------------------------------------------------------------------------------------------
extern const S_LibCanModule_Module_t CanNm_Module;

// --------------------------------------------------------------------------------------------------------------------
/// \brief FIFO for outgoing transmit messages - NM PDUs
// --------------------------------------------------------------------------------------------------------------------
extern S_LibFifoQueue_Inst_t CanNm_NmMsgSendFifo;

// --------------------------------------------------------------------------------------------------------------------
/// \brief  The state of the app frame Rx enable or not.
// --------------------------------------------------------------------------------------------------------------------
extern bool_t CanNm_Appframe_RxEnable;

// --------------------------------------------------------------------------------------------------------------------
/// \brief used to count for sleep
// --------------------------------------------------------------------------------------------------------------------
extern uint32_t SleepToShutDownCount;

// --------------------------------------------------------------------------------------------------------------------
//  Global Function Prototypes
// --------------------------------------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------------------------------------
/// \brief Set the network to the requested state.
///
/// \details The function has the same behavior as this described in the AUTOSAR
///          Specification of CAN Network Management.
///          Called by the application, usually after being indicated about NM PDU received
///          during sleep.
// --------------------------------------------------------------------------------------------------------------------
void CanNM_RequestNetwork(void);

// --------------------------------------------------------------------------------------------------------------------
/// \brief Release the network from the requested state.
///
/// \details The function has the same behavior as this described in the AUTOSAR
///          Specification of CAN Network Management.
///          Called by the application, when it is ready to go to sleep.
// --------------------------------------------------------------------------------------------------------------------
void CanNM_ReleaseNetwork(void);
// --------------------------------------------------------------------------------------------------------------------
/// \brief  get can NM status.
// --------------------------------------------------------------------------------------------------------------------
NM_Status CanNM_GetCanNM_Status(void);
//pan.sw add
//bool_t IL_send_fifo_empty = false;

extern void CanNM_SetWakeUpReason(uint32_t reason);

extern void CanNM_ClearWakeUpReason(uint32_t reason);

#endif /* BSW_CAN_CAN_NM_INC_CANNM_H_ */
