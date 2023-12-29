// --------------------------------------------------------------------------------------------------------------------
///
/// \file LibCanTp.h
///
/// \brief This module is the implementation of the CAN Transport Protocol
///
/// It implements the ISO 15765-2 (IsoTp) standard
///
/// Copyright (c) 2021 Neusoft.
/// All Rights Reserved.
///
// --------------------------------------------------------------------------------------------------------------------

 

#ifndef LIBCANTP_H__INCLUDED
#define LIBCANTP_H__INCLUDED

// --------------------------------------------------------------------------------------------------------------------
//	Includes
// --------------------------------------------------------------------------------------------------------------------

#include "LibCanTpCfg.h"
#include "LibCanMsg.h"
#include "LibCanModule.h"
#include "LibDiagCom.h"
//#include "LibDriverDevIds.h"
#include "LibFifoQueue.h"
#include "LibFsm.h"
#include "LibTypes.h"
#include "LibService.h"

// --------------------------------------------------------------------------------------------------------------------
//	Global Definitions
// --------------------------------------------------------------------------------------------------------------------

#if LIBCANTPCFG_FIXED_FC_PARAM
	#define LIBCANTP_CF_PARAM_CONST	const
#else
	#define LIBCANTP_CF_PARAM_CONST
#endif // LIBCANTPCFG_FIXED_FC_PARAM

#define LIBCANTP_MSG_REQ_FIFO_ELEMENTS  (3U)
#define LIBCANTP_MSG_IND_FIFO_ELEMENTS  (8U)
#define LIBCANTP_MSG_CON_FIFO_ELEMENTS  (3U)

// --------------------------------------------------------------------------------------------------------------------
/// \brief Service Event - Request
///
/// Request sending of a message from a higher layer
// --------------------------------------------------------------------------------------------------------------------
#define LIBCANTP_SRV_EV_REQ		UINT32_C(0x00000001)

// --------------------------------------------------------------------------------------------------------------------
/// \brief Service Event - Confirm
///
/// Confirmation from the CAN controller that a CAN frame was sent
// --------------------------------------------------------------------------------------------------------------------
#define LIBCANTP_SRV_EV_CON		UINT32_C(0x00000002)

// --------------------------------------------------------------------------------------------------------------------
/// \brief Service Event - Indicate
///
/// Indication from the CAN controller that a CAN frame was received
// --------------------------------------------------------------------------------------------------------------------
#define LIBCANTP_SRV_EV_IND		UINT32_C(0x00000004)

// --------------------------------------------------------------------------------------------------------------------
/// \brief Service Event - CommErr
///
/// Indication from the CAN controller that DiagCom Error
// --------------------------------------------------------------------------------------------------------------------
#define LIBCANTP_SRV_EV_COMERR	UINT32_C(0x00000008)


// --------------------------------------------------------------------------------------------------------------------
//	Global Data Types
// --------------------------------------------------------------------------------------------------------------------
typedef S_LibCan_Msg_t S_LibCanTp_BufferEntry_t;

typedef S_LibCanTp_BufferEntry_t S_LibCanTp_MsgReqBufferEntry_t;
typedef S_LibCanTp_BufferEntry_t S_LibCanTp_MsgIndBufferEntry_t;
typedef uint32_t S_LibCanTp_MsgConBufferEntry_t;


// --------------------------------------------------------------------------------------------------------------------
//	Imported Variables
// --------------------------------------------------------------------------------------------------------------------
extern S_LibFifoQueue_Inst_t LibCanTp_MsgReqFifo;
extern S_LibFifoQueue_Inst_t LibCanTp_MsgIndFifo;
extern S_LibFifoQueue_Inst_t LibCanTp_MsgConFifo;

// --------------------------------------------------------------------------------------------------------------------
/// \brief Service Instance of the CAN Transport Protocol Service
// --------------------------------------------------------------------------------------------------------------------
extern S_LibService_Inst_t LibCanTp_Service;

extern const S_LibCanModule_Module_t LibCanTp_Module;

// --------------------------------------------------------------------------------------------------------------------
//	Global Function Prototypes
// --------------------------------------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------------------------------------
/// \brief Checks if the message ID is dedicated to CanTp
///
/// \param msgId 
///	CAN Message ID
/// \return 
/// if true the message is dedicated to CanTp
// --------------------------------------------------------------------------------------------------------------------
extern bool_t LibCanTp_IsMsgTp(uint32_t msgId);

// --------------------------------------------------------------------------------------------------------------------
/// \brief Indicates the reception of a new CAN frame/message
/// 
/// \param pMsg 
/// The received message
/// \param devId 
/// The Device Driver Identifier
// --------------------------------------------------------------------------------------------------------------------
extern void LibCanTp_MsgIndicate(S_LibCan_Msg_t* pMsg);

// --------------------------------------------------------------------------------------------------------------------
/// \brief Confirms the successfull transmission of a previously sent CAN frame/message
/// 
/// \param msgId 
/// CAN ID of the message that was sent
/// \param devId 
/// The Device Driver Identifier
// --------------------------------------------------------------------------------------------------------------------
extern void LibCanTp_MsgConfirm(uint32_t msgId);

// --------------------------------------------------------------------------------------------------------------------
/// \brief Request the transmission of a TP message (not CAN frame)
/// 
/// \param pMsg 
/// The message that should be sent
// --------------------------------------------------------------------------------------------------------------------
extern void LibCanTp_MsgRequest(S_LibDiagCom_Msg_t* pMsg);

// --------------------------------------------------------------------------------------------------------------------
/// \brief Stop the transmission.
// --------------------------------------------------------------------------------------------------------------------
extern void LibCanTP_TxStop(void);

// --------------------------------------------------------------------------------------------------------------------
/// \brief Stop the reception.
// --------------------------------------------------------------------------------------------------------------------
extern void LibCanTP_RxStop(void);

// --------------------------------------------------------------------------------------------------------------------
/// \brief Enable the transmission.
// --------------------------------------------------------------------------------------------------------------------
extern void LibCanTP_TxEnable(void);

// --------------------------------------------------------------------------------------------------------------------
/// \brief Enable the reception.
// --------------------------------------------------------------------------------------------------------------------
extern void LibCanTP_RxEnable(void);

// --------------------------------------------------------------------------------------------------------------------
/// \brief Disable the transmission.
// --------------------------------------------------------------------------------------------------------------------
extern void LibCanTP_TxDisable(void);

// --------------------------------------------------------------------------------------------------------------------
/// \brief Disable the reception.
// --------------------------------------------------------------------------------------------------------------------
extern void LibCanTP_RxDisable(void);


#if !LIBCANTPCFG_FIXED_FC_PARAM

struct S_LibCanTp_Inst_t;

// --------------------------------------------------------------------------------------------------------------------
/// \brief Set Flow Control Parameters
///
/// \attention This function is only defined if LIBCANTPCFG_FIXED_FC_PARAM is defined to 0
/// 
/// \param pInst 
/// \param blockSize 
/// Block Size Parameter
/// \sa LIBCANTPCFG_FC_PARAM_BS
/// \param sepTimeMin 
/// Separation Time Minimum Parameter
/// \sa LIBCANTPCFG_FC_PARAM_STMIN
// --------------------------------------------------------------------------------------------------------------------
extern void LibCanTp_SetFcParams(struct S_LibCanTp_Inst_t* pInst, const uint8_t blockSize, const uint8_t sepTimeMin);

#endif // !LIBCANTPCFG_FIXED_FC_PARAM

#endif // LIBCANTP_H__INCLUDED

