/*
 * CanNmHistory.h
 *
 *  Created on: 2021年10月28日
 *      Author: pan.sw
 */

#ifndef BSW_CAN_CAN_NM_INC_CANNMHISTORY_H_
#define BSW_CAN_CAN_NM_INC_CANNMHISTORY_H_

#include "CanNmCfg.h"
#include "LibService.h"
#include "LibCanModule.h"
#include "LibFifoQueue.h"
// --------------------------------------------------------------------------------------------------------------------
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
/// \brief CanNm history state definition
// --------------------------------------------------------------------------------------------------------------------
typedef enum
{
	CANNM_HIST_INITIAL = 0,
	CANNM_HIST_OFF,
	CANNM_HIST_BUS_SLEEP_MODE,
	CANNM_HIST_REPEAT_MESSAGE_STATE,
	CANNM_HIST_NORMAL_OPERATION_STATE,
	CANNM_HIST_READY_SLEEP_STATE,
	CANNM_HIST_PREPARE_BUS_SLEEP_MODE
} E_CanNmHistory_States_t;
// --------------------------------------------------------------------------------------------------------------------
//  Global Function Prototypes
// --------------------------------------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------------------------------------
/// \brief Set the network to the requested state.
///
/// \details The function has the same behavior as this described in the AUTOSAR
///          Specification of CAN Network Management.
///          Called by the application, when it is need to save can NM history
// --------------------------------------------------------------------------------------------------------------------
extern void CanNmHistory_PushState( E_CanNmHistory_States_t newState );
// --------------------------------------------------------------------------------------------------------------------
/// \brief Set the network to the requested state.
///
/// \details The function has the same behavior as this described in the AUTOSAR
///          Specification of CAN Network Management.
///          Called by the application, when it is need to save can NM history
// --------------------------------------------------------------------------------------------------------------------
extern void CanNMHistory_Save( void );

// --------------------------------------------------------------------------------------------------------------------
/// \brief Release the network from the requested state.
///
/// \details The function has the same behavior as this described in the AUTOSAR
///          Specification of CAN Network Management.
///          Called by the application, when it is need to load can NM history.
// --------------------------------------------------------------------------------------------------------------------
extern void CanNMHistory_Load( void );

#endif /* BSW_CAN_CAN_NM_INC_CANNMHISTORY_H_ */
