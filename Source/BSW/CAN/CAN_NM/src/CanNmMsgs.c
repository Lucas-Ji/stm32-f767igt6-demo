/*
 * CanNmMsgs.c
 *
 *  Created on: 2021锟斤拷10锟斤拷28锟斤拷
 *      Author: pan.sw
 */

#include "CanNmMsgs.h"

// --------------------------------------------------------------------------------------------------------------------
//  Local Definitions
// --------------------------------------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------------------------------------
/// \brief A default value of an unused signal of NM PDU
// --------------------------------------------------------------------------------------------------------------------
#define NM_DUMMY_DATA UINT8_C(0)

// --------------------------------------------------------------------------------------------------------------------
//  Local Function
// --------------------------------------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------------------------------------
/// \brief Returns a CAN message's data with a new signal value.
///
/// \param msgData
/// Current data of CAN message.
/// \param sigValue
/// New value of the signal.
/// \param msgStartBit
/// Start bit of the signal.
/// \param length
/// Length of the signal.
///
/// \return The new data for the can message.
// --------------------------------------------------------------------------------------------------------------------
static uint64_t CanNmMsgs_ConstructSignal(uint64_t msgData, uint64_t sigValue, uint8_t msgStartBit, uint8_t length)
{
    if (length < CANNMMSG_MSG_LENGTH)
    {
        uint64_t sigMask = ((UINT64_C(1) << length) - UINT64_C(1));

        sigValue &= sigMask;
        sigMask = sigMask << msgStartBit;
        sigValue = sigValue << msgStartBit;

        msgData &= ~sigMask;
        msgData |= sigValue;
    }
    else
    {
        msgData = sigValue;
    }

    return msgData;
}

// --------------------------------------------------------------------------------------------------------------------
/// \brief Takes a CAN message's data and extracts a signal value based on given properties.
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
static uint64_t CanNmMsgs_ExtractSignal(uint64_t msgData, uint8_t msgStartBit, uint8_t length)
{
    uint64_t retValue = UINT64_C(0);

    retValue  = msgData >> msgStartBit;
    if (length < CANNMMSG_MSG_LENGTH)
    {
        retValue &= ((UINT64_C(1) << length) - UINT64_C(1));
    }

    return retValue;
}

//=====================================================================================================================
// CanNm_MsgConfirm:
//=====================================================================================================================
uint32_t CanNmMsgs_IsNmPduId(uint32_t msgId)
{
    // Input and output NM PDUs as it is needed for message indication AND send confirmation.
	const S_LibCanIL_MessageDesc_t* const pMsgDesc =
            &LibCanILCfg_MessageTable.pMessageDesc[(uint8_t)LIBCANIL_MSG_COMMONTESTRX_NM];
    return (
               (pMsgDesc -> Id) == msgId
           );

}

//=====================================================================================================================
// CanNmMsgs_WriteMessage:
//=====================================================================================================================
static void CanNmMsgs_WriteMessage(E_LibCanILCfg_MessageNames_t msgName, S_LibCan_Msg_t* pMsg)
{
	if((uint8_t)msgName < LibCanILCfg_MessageTable.NumOfMessages)
	{
		// Pointer to the message description of the message to be write
		const S_LibCanIL_MessageDesc_t* pMsgDesc = &LibCanILCfg_MessageTable.pMessageDesc[(uint8_t)msgName];

		if (pMsgDesc->IsTx == false)
		{
			LibLog_Info("LibCanNm: Remote Message\n");
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
					msgData = CanNmMsgs_ConstructSignal(msgData, sigValue, pSignalDesc->MsgStartBit, pSignalDesc->Length);
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
			pMsg->Id = pMsgDesc->Id;
			pMsg->IsExtId = pMsgDesc->IsExtId;
			pMsg->IsCanFd = false;
			pMsg->IsBrs = false;
			pMsg->Length = pMsgDesc->Length;
		}
	}
}

//=====================================================================================================================
// CanNm_MsgConfirm:
//=====================================================================================================================
void CanNmMsgs_BuildNmPDU(S_LibCan_Msg_t* pduMsg)
{
    Lib_Assert(pduMsg != NULL);
    uint8_t nmDataTemp = 0;

    /* Byte0: Source node ID 	 									            */
    nmDataTemp = (uint8_t)(CAN_NM_TX_NODE_ID & 0xFF);
    LibCanIL_SetSignal(LIBCANIL_NMSIG_COMMONTESTRX_NODEID, nmDataTemp);

    CanNmMsgs_WriteMessage(LIBCANIL_MSG_COMMONTESTTX_NM, pduMsg);
}

//=====================================================================================================================
// CanNmMsgs_IsRSRSet:
//=====================================================================================================================
bool_t CanNmMsgs_IsRSRSet(const S_LibCan_Msg_t* const pduMsg)
{
    Lib_Assert(pduMsg != NULL);
    
    const bool_t sigValue = LibCanIL_GetSignal(LIBCANIL_NMSIG_COMMONTESTRX_CBV_REPEAT_MSG_REQ);
    return sigValue;
}
