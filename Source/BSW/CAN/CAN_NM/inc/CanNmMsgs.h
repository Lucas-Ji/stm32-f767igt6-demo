/*
 * CanNmMsgs.h
 *
 *  Created on: 2021锟斤拷10锟斤拷28锟斤拷
 *      Author: pan.sw
 */

#ifndef BSW_CAN_CAN_NM_INC_CANNMMSGS_H_
#define BSW_CAN_CAN_NM_INC_CANNMMSGS_H_

#include <stdlib.h>
#include <string.h>
#include "LibCanIL.h"
#include "LibTypes.h"
#include "LibCanILCfg.h"
#include "CanNmCfg.h"
#include "CanNm.h"

#ifndef CANNMMSG_MSG_LENGTH
#define CANNMMSG_MSG_LENGTH 64
#endif
// --------------------------------------------------------------------------------------------------------------------
//  Global Definitions
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
//  Global Data Types
// --------------------------------------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------------------------------------
//  Global Function Prototypes
// --------------------------------------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------------------------------------
/// \brief Indicates whether the given message ID belongs to the NM module.
///
/// \param msgId
/// Message ID retrieved from the CAN module.
///
/// \retval FALSE
/// This message is not a network management message.
/// \retval TRUE
/// This message is a network management message.
// --------------------------------------------------------------------------------------------------------------------
uint32_t CanNmMsgs_IsNmPduId(uint32_t msgId);

// --------------------------------------------------------------------------------------------------------------------
/// \brief Fill in the given CAN message's fields.
///
/// \param pduMsg
/// Pointer to the message to fill. Must not be NULL.
///
/// \retval none
// --------------------------------------------------------------------------------------------------------------------
void CanNmMsgs_BuildNmPDU(S_LibCan_Msg_t* pduMsg);

// --------------------------------------------------------------------------------------------------------------------
/// \brief Indicates whether the given NM message has the RSR (Repeat-request) bit set.
///
/// \param pduMsg
/// Pointer to the message to check.
///
/// \retval FALSE
/// This message has not the RSR bit set.
/// \retval TRUE
/// This message has the RSR bit set.
// --------------------------------------------------------------------------------------------------------------------
bool_t CanNmMsgs_IsRSRSet(const S_LibCan_Msg_t* const pduMsg);


#endif /* BSW_CAN_CAN_NM_INC_CANNMMSGS_H_ */
