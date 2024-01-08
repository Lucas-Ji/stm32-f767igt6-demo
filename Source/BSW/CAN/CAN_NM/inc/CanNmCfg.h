/*
 * CanNmCfg.h
 *
 *  Created on: 2021年10月28日
 *      Author: pan.sw
 */

#ifndef BSW_CAN_CAN_NM_CFG_CANNMCFG_H_
#define BSW_CAN_CAN_NM_CFG_CANNMCFG_H_

#include "LibTypes.h"

// --------------------------------------------------------------------------------------------------------------------
//  Global Definitions
// --------------------------------------------------------------------------------------------------------------------
#define CAN_NM_RX_ID 0x400
#define CAN_NM_TX_BASE_ID 0x400
#define CAN_NM_TX_NODE_ID 0x1
#define CAN_NM_CHANNEL CanChannel_1

// --------------------------------------------------------------------------------------------------------------------
/// \brief AUTOSAR CanNm timing parameter Remote Sleep Indication Time in [ms]
// --------------------------------------------------------------------------------------------------------------------
#define CANNM_REMOTE_SLEEP_IND_TIME (UINT32_C(3) * MSEC_PER_SEC)

// --------------------------------------------------------------------------------------------------------------------
/// \brief NM PDU's transmission timeout in [ms]
// --------------------------------------------------------------------------------------------------------------------
#define CANNM_PDU_TX_TIMEOUT_MS UINT32_C(500)

// --------------------------------------------------------------------------------------------------------------------
/// \brief The period of transmitting the NM PDUs in [ms]
// --------------------------------------------------------------------------------------------------------------------
#define CANNM_PDU_TRANSMIT_PERIOD_MS UINT32_C(500)

#define CanNmImmediateNmCycleTime   UINT32_C(20)
#define CanNmImmediateNmTransmissions   UINT32_C(10)

// --------------------------------------------------------------------------------------------------------------------
/// \brief The time period for which to stay in the REPEAT MESSAGE STATE in [ms]
/// Defined by AUTOSAR as configuration variable - NM_REPEAT_MESSAGE_TIME
// --------------------------------------------------------------------------------------------------------------------
#define CANNM_REPEAT_MESSAGE_STATE_TIME_MS UINT32_C(1500)

// --------------------------------------------------------------------------------------------------------------------
/// \brief The time period for which to stay in the NORMAL STATE in [ms]
/// Defined by AUTOSAR as configuration variable - NM_NORMAL_TIME
// --------------------------------------------------------------------------------------------------------------------
#define CANNM_NORMAL_STATE_TIME_MS UINT32_C(1500)

// --------------------------------------------------------------------------------------------------------------------
//  \brief Depends on NMTimeoutTimer in [ms]
// --------------------------------------------------------------------------------------------------------------------
#define CANNM_NM_TIMEOUT_TIME_MS (UINT32_C(2) * MSEC_PER_SEC)

// --------------------------------------------------------------------------------------------------------------------
//  \brief An AUTOSAR configuration parameter in [ms] - NM_WAIT_BUS_SLEEP_TIME
// --------------------------------------------------------------------------------------------------------------------
#define CANNM_WAIT_BUS_SLEEP_TIME (UINT32_C(2) * MSEC_PER_SEC)

#define CANNM_SLEEP_WAIT_TXFIFO_CLEAR_TIME_MS UINT32_C(10)
// --------------------------------------------------------------------------------------------------------------------
//  Module call-backs.
// --------------------------------------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------------------------------------
/// \brief Notification that the network management has been started.
// --------------------------------------------------------------------------------------------------------------------
#define Nm_PowerOnIndication() CanNmMgr_PowerOnIndication()

// --------------------------------------------------------------------------------------------------------------------
/// \brief Notification that the network management is shooting down.
// --------------------------------------------------------------------------------------------------------------------
#define Nm_PowerOffIndication() CanNmMgr_PowerOffIndication()

// --------------------------------------------------------------------------------------------------------------------
/// \brief Notification that the network management has entered Bus-Sleep Mode.
// --------------------------------------------------------------------------------------------------------------------
#define Nm_BusSleepMode() CanNmMgr_BusSleepEntered()

// --------------------------------------------------------------------------------------------------------------------
/// \brief Notification that a NM-message has been received in the Bus-Sleep Mode,
///        which indicates that some nodes in the network have already entered the Network Mode.
// --------------------------------------------------------------------------------------------------------------------
#define Nm_NetworkStartIndication() CanNM_RequestNetwork()

// --------------------------------------------------------------------------------------------------------------------
/// \brief Notification that the network management has entered Network Mode.
// --------------------------------------------------------------------------------------------------------------------
#define Nm_NetworkMode() CanNmMgr_NetworkModeEntered()

// --------------------------------------------------------------------------------------------------------------------
/// \brief Notification that the network management has entered Prepare Bus-Sleep Mode.
// --------------------------------------------------------------------------------------------------------------------
#define Nm_PrepareBusSleepMode() CanNmMgr_PrepareBusSleepEntered()

// --------------------------------------------------------------------------------------------------------------------
/// \brief Notification that the network management has detected
///        that all other nodes on the network are ready to enter Bus-Sleep Mode.
// --------------------------------------------------------------------------------------------------------------------
#define Nm_RemoteSleepIndication() CanNM_ReleaseNetwork()

// --------------------------------------------------------------------------------------------------------------------
/// \brief Notification that the network management has detected
///        that not all other nodes on the network are longer ready to enter Bus-Sleep Mode.
// --------------------------------------------------------------------------------------------------------------------
#define Nm_RemoteSleepCancellation() CanNM_RequestNetwork()

#if 0
//pan.sw add before E_LibCanILCfg_SignalNames_t add them
#define	LIBCANIL_SIG_DESTINATION_GW_NM 			0
#define	LIBCANIL_SIG_OPCODE_GW_NM 				0
#define	LIBCANIL_SIG_DESTINATION_PLG_NM 		0
#define	LIBCANIL_SIG_OPCODE_PLG_NM				0
#define	LIBCANIL_SIG_DESTINATION_ABM_NM			0
#define	LIBCANIL_SIG_OPCODE_ABM_NM				0
#define	LIBCANIL_SIG_DESTINATION_VMDR_NM		0
#define	LIBCANIL_SIG_OPCODE_VMDR_NM				0
#define	LIBCANIL_SIG_DESTINATION_AMP_NM			0
#define	LIBCANIL_SIG_OPCODE_AMP_NM				0
#define	LIBCANIL_SIG_RMS_AMP_NM					0
#define	LIBCANIL_SIG_WAKEUPREASON_AMP_NM		0
#define	LIBCANIL_SIG_STAYAWAKEREASON_AMP_NM 	0
#define	LIBCANIL_SIG_GETHUTINFOSTS 				0
#define	LIBCANIL_SIG_SYSTEMINFORMATION_AMP_NM	0
//pan.sw add before E_LibCanILCfg_MessageNames_t add them
#define LIBCANIL_MSG_GW_NM 					0
#define LIBCANIL_MSG_PLG_NM					0
#define LIBCANIL_MSG_ABM_NM					0
#define LIBCANIL_MSG_VMDR_NM				0
#define LIBCANIL_MSG_AMP_NM					0
#define LIBCANIL_MSG_AMP2					0
#endif /* jianggang commend for CANNM msg */


#endif /* BSW_CAN_CAN_NM_CFG_CANNMCFG_H_ */
