/*
 * CanNmMgr.h
 *
 *  Created on: 2021年10月28日
 *      Author: pan.sw
 */

#ifndef BSW_CAN_CAN_NM_INC_CANNMMGR_H_
#define BSW_CAN_CAN_NM_INC_CANNMMGR_H_

#include "CanNmCfg.h"
#include "LibCanIL.h"

#ifndef MSEC_PER_SEC
#define MSEC_PER_SEC 1000
#endif
// --------------------------------------------------------------------------------------------------------------------
/// \brief Indicates how long at least in [ms], the device shall wait for the first NM message
///        after wake-up and before going to back to sleep.
// --------------------------------------------------------------------------------------------------------------------
#define CANNM_POWER_ON_WAKEUP_TIME (UINT32_C(5) * MSEC_PER_SEC)

// --------------------------------------------------------------------------------------------------------------------
//  Global Function Prototypes
// --------------------------------------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------------------------------------
/// \brief Network manager started from the bus-off state.
///
/// \details The function holds the device waked-up for CANNM_POWER_ON_WAKEUP_TIME
///          in order to wait for the starting sequence.
///          Puts the CAN transceiver into normal state.
// --------------------------------------------------------------------------------------------------------------------
void CanNmMgr_PowerOnIndication(void);

// --------------------------------------------------------------------------------------------------------------------
/// \brief Network manager started from the bus-off state.
///
/// \details The function puts the CAN transceiver into STB mode.
// --------------------------------------------------------------------------------------------------------------------
void CanNmMgr_PowerOffIndication(void);

// --------------------------------------------------------------------------------------------------------------------
/// \brief Buss sleep entered.
///
/// \details The function request a shutdown of the device.
// --------------------------------------------------------------------------------------------------------------------
void CanNmMgr_BusSleepEntered(void);

// --------------------------------------------------------------------------------------------------------------------
/// \brief Network mode entered.
///
/// \details The function enables the CanIL
// --------------------------------------------------------------------------------------------------------------------
void CanNmMgr_NetworkModeEntered(void);

// --------------------------------------------------------------------------------------------------------------------
/// \brief Going off the Network mode.
///
/// \details The function disables the CanIL
// --------------------------------------------------------------------------------------------------------------------
void CanNmMgr_PrepareBusSleepEntered(void);
#endif /* BSW_CAN_CAN_NM_INC_CANNMMGR_H_ */
