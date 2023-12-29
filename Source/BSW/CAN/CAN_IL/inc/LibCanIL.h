// --------------------------------------------------------------------------------------------------------------------
///
/// \file LibCanIL.h
///
/// \brief This module implements the CAN Interaction Layer
///
///
///
/// Copyright (c) 2021 Neusoft.
/// All Rights Reserved.
///
// --------------------------------------------------------------------------------------------------------------------



#ifndef LIBCANIL_H__INCLUDED
#define LIBCANIL_H__INCLUDED

// --------------------------------------------------------------------------------------------------------------------
//	Includes
// --------------------------------------------------------------------------------------------------------------------

#include "LibService.h"
#include "LibCanMsg.h"
#include "LibCanModule.h"
#include "LibFifoQueue.h"
#include "LibCanILCfg.h"
#include "can_message.h"

// --------------------------------------------------------------------------------------------------------------------
//	Global Definitions
// --------------------------------------------------------------------------------------------------------------------
#define LIBCANIL_EVENT_CAN_MESSAGE_REQ					0                       //TBD EV_CAN_MSG_REQ
#define LIBCANIL_EVENT_CAN_MESSAGE_IND					UINT32_C(0x00000002)
#define LIBCANIL_EVENT_CAN_MESSAGE_CON					UINT32_C(0x00000004)
#define LIBCANIL_EVENT_TXCYCLE_MESSAGE_TIMER			UINT32_C(0x00000008)
#define LIBCANIL_EVENT_TXEVENT_MESSAGE_TIMER            UINT32_C(0x00000020)
#define LIBCANIL_EVENT_RXCYCLE_MESSAGE_TIMER            UINT32_C(0x00000040)


// --------------------------------------------------------------------------------------------------------------------
//	Global Data Types
// --------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------
/// \brief  Structure for CAN Interaction Layer transmit message buffer.
// --------------------------------------------------------------------------------------------------------------------

typedef S_LibCan_Msg_t S_LibCanIL_MsgReqBufferEntry_t;
// --------------------------------------------------------------------------------------------------------------------
/// \brief  Structure for CAN Interaction Layer receive message buffer.
// --------------------------------------------------------------------------------------------------------------------
typedef S_LibCan_Msg_t S_LibCanIL_MsgIndBufferEntry_t;

// --------------------------------------------------------------------------------------------------------------------
/// \brief  Structure for CAN Interaction Layer message confirm buffer.
// --------------------------------------------------------------------------------------------------------------------
typedef uint32_t S_LibCanIL_MsgConBufferEntry_t;

// --------------------------------------------------------------------------------------------------------------------
/// \brief Data type for a tx cycle message.
///
/// \attention
/// This structure shall not be used directly, only for internal use of this module.
// --------------------------------------------------------------------------------------------------------------------
typedef struct
{
	// ----------------------------------------------------------------------------------------------------------------
	/// \brief Absolute call time in relation to the UpTimer in milliseconds.
	// ----------------------------------------------------------------------------------------------------------------
	uint32_t NextCallTime_ms;

	/* Enable flag */
	bool bTxCycleMsgEnabled;

	// ----------------------------------------------------------------------------------------------------------------
	/// \brief Message name for the cycle message.
	// ----------------------------------------------------------------------------------------------------------------
	E_LibCanILCfg_MessageNames_t MessageNames;

} S_LibCanIL_TxCycleMsg_t;


// --------------------------------------------------------------------------------------------------------------------
/// \brief Data type for tx a event message.
///
/// \attention
/// This structure shall not be used directly, only for internal use of this module.
// --------------------------------------------------------------------------------------------------------------------
typedef struct
{
	// ----------------------------------------------------------------------------------------------------------------
	/// \brief Send times of tx msg
	// ----------------------------------------------------------------------------------------------------------------
	uint8_t SendTimes;

	// ----------------------------------------------------------------------------------------------------------------
	/// \brief Message name for the tx message.
	// ----------------------------------------------------------------------------------------------------------------
	E_LibCanILCfg_MessageNames_t MessageNames;

} S_LibCanIL_TxEventMsg_t;


// --------------------------------------------------------------------------------------------------------------------
/// \brief Data type for a rx cycle message.
///
/// \attention
/// This structure shall not be used directly, only for internal use of this module.
// --------------------------------------------------------------------------------------------------------------------
typedef struct
{
	// ----------------------------------------------------------------------------------------------------------------
	/// \brief StartTiming
	// ----------------------------------------------------------------------------------------------------------------
	bool_t StartTimingFlag;

	// ----------------------------------------------------------------------------------------------------------------
	/// \brief ActualCycleValue.
	// ----------------------------------------------------------------------------------------------------------------
	uint16_t ActualCycleValue;

	// ----------------------------------------------------------------------------------------------------------------
	/// \brief MissingCnt.
	// ----------------------------------------------------------------------------------------------------------------
	uint16_t MissingCnt;
    
	// ----------------------------------------------------------------------------------------------------------------
	/// \brief TimeOutValue.
	// ----------------------------------------------------------------------------------------------------------------
	uint16_t MulTimeOutValue;
    
	// ----------------------------------------------------------------------------------------------------------------
	/// \brief The application software shall only use the signal values read from IL when FirstValueIndication = TRUE
	// ----------------------------------------------------------------------------------------------------------------
	bool_t FirstValueIndication;

	// ----------------------------------------------------------------------------------------------------------------
	/// \brief The application software shall only use the signal values read from IL when TimeOutIndication = FALSE
	// ----------------------------------------------------------------------------------------------------------------
	bool_t TimeOutIndication;

	// ----------------------------------------------------------------------------------------------------------------
	/// \brief Message name for the cycle message.
	// ----------------------------------------------------------------------------------------------------------------
	E_LibCanILCfg_MessageNames_t MessageNames;

} S_LibCanIL_RxCycleMsg_t;



// --------------------------------------------------------------------------------------------------------------------
/// \brief Callback function will be called when a new CAN massage is received or signals change.
// --------------------------------------------------------------------------------------------------------------------
typedef	void (*LibCanIL_DataChangedCbk)(void);

// --------------------------------------------------------------------------------------------------------------------
/// \brief  Structure used for CAN message configuration. This structure should be declared as constant.
// --------------------------------------------------------------------------------------------------------------------
typedef struct
{
	// ----------------------------------------------------------------------------------------------------------------
	/// \brief The CAN message data channel.
	// ----------------------------------------------------------------------------------------------------------------
	const E_LibCan_Channel_t			CanDevId;

	// ----------------------------------------------------------------------------------------------------------------
	/// \brief  The CAN message ID.
	// ----------------------------------------------------------------------------------------------------------------
	const uint32_t						Id;

	// ----------------------------------------------------------------------------------------------------------------
	/// \brief  The flag is the message with extended ID.
	// ----------------------------------------------------------------------------------------------------------------
	const bool_t						IsExtId;

	// ----------------------------------------------------------------------------------------------------------------
	/// \brief  The CAN message data length.
	// ----------------------------------------------------------------------------------------------------------------
	const E_LibCan_DlcSize_t			Length;

	// ----------------------------------------------------------------------------------------------------------------
	/// \brief  The flag is the message a message for transmitting.
	// ----------------------------------------------------------------------------------------------------------------
	const bool_t						IsTx;

	// ----------------------------------------------------------------------------------------------------------------
	/// \brief  The flag is the message byteorder Intel.
	// ----------------------------------------------------------------------------------------------------------------
	const bool_t						IsIntel;

	// ----------------------------------------------------------------------------------------------------------------
	/// \brief  The cycle time for this message to transmit.
	// ----------------------------------------------------------------------------------------------------------------
	const uint16_t						CycleTime;

	// ----------------------------------------------------------------------------------------------------------------
	/// \brief  The start delay time for cyclically transmitted messages.
	// ----------------------------------------------------------------------------------------------------------------
	const uint16_t						StartDelayTime;

	// ----------------------------------------------------------------------------------------------------------------
	/// \brief  Pointer of the first signal of this message / next signal is (FirstSignal + 1)
	// ----------------------------------------------------------------------------------------------------------------
	const E_LibCanILCfg_SignalNames_t	FirstSignal;

	// ----------------------------------------------------------------------------------------------------------------
	/// \brief  Number of signals of this message
	// ----------------------------------------------------------------------------------------------------------------
	const uint8_t						NSignals;

	// ----------------------------------------------------------------------------------------------------------------
	/// \brief Pointer of the first message receive callback for this message / next message receive callback is (FirstMsgRecCbk + 1)
	// ----------------------------------------------------------------------------------------------------------------
	const E_LibCanILCfg_CallbackNames_t	FirstMsgRecCbk;

	// ----------------------------------------------------------------------------------------------------------------
	/// \brief  Number of message receive callbacks for this message
	// ----------------------------------------------------------------------------------------------------------------
	const uint8_t						NDataChCbks;

	/* CAN handle with asw */
	const bool_t	                    IsASWHndle;

	CAN_DATATYPE * const ASWCANFrame;

	void (*ASWHndleFunc)(void);

} S_LibCanIL_MessageDesc_t;

// --------------------------------------------------------------------------------------------------------------------
/// \brief Structure used as table of CAN message configuration. This structure should be declared as constant.
// --------------------------------------------------------------------------------------------------------------------
typedef struct
{
	// ----------------------------------------------------------------------------------------------------------------
	/// \brief Number of CAN messages.
	// ----------------------------------------------------------------------------------------------------------------
	const uint8_t NumOfMessages;

	// ----------------------------------------------------------------------------------------------------------------
	/// \brief Pointer to the table of the CAN messages.
	// ----------------------------------------------------------------------------------------------------------------
	const S_LibCanIL_MessageDesc_t* const pMessageDesc;

} S_LibCanIL_MessageTable_t;

// --------------------------------------------------------------------------------------------------------------------
/// \brief  Structure used for CAN signal configuration. This structure should be declared as constant.
// --------------------------------------------------------------------------------------------------------------------
typedef struct
{
	// ----------------------------------------------------------------------------------------------------------------
	/// \brief  The CAN signal data length.
	// ----------------------------------------------------------------------------------------------------------------
	const uint8_t						Length;

	// ----------------------------------------------------------------------------------------------------------------
	/// \brief  Start bit of the signal in this message.
	// ----------------------------------------------------------------------------------------------------------------
	const uint8_t						MsgStartBit;

	// ----------------------------------------------------------------------------------------------------------------
	/// \brief  Start bit of the signal in the internal signal storage.
	// ----------------------------------------------------------------------------------------------------------------
	const uint32_t						StorageStartBit;

	// ----------------------------------------------------------------------------------------------------------------
	/// \brief  Initial start value of the signal
	// ----------------------------------------------------------------------------------------------------------------
	const uint64_t						StartValue;

	// ----------------------------------------------------------------------------------------------------------------
	/// \brief  Pointer of the message.
	// ----------------------------------------------------------------------------------------------------------------
	const E_LibCanILCfg_MessageNames_t	MsgName;

	// ----------------------------------------------------------------------------------------------------------------
	/// \brief  Pointer of the first change callback for this signal / next change callback is (FirstDataChCbk + 1).
	// ----------------------------------------------------------------------------------------------------------------
	const E_LibCanILCfg_CallbackNames_t	FirstDataChCbk;

	// ----------------------------------------------------------------------------------------------------------------
	/// \brief  Number of change callbacks for this signal.
	// ----------------------------------------------------------------------------------------------------------------
	const uint8_t						NDataChCbks;
} S_LibCanIL_SignalDesc_t;

// --------------------------------------------------------------------------------------------------------------------
/// \brief Structure used as table of CAN signal configuration. This structure should be declared as constant.
// --------------------------------------------------------------------------------------------------------------------
typedef struct
{
	// ----------------------------------------------------------------------------------------------------------------
	/// \brief Number of CAN signals.
	// ----------------------------------------------------------------------------------------------------------------
	const uint16_t NumOfSignals;

	// ----------------------------------------------------------------------------------------------------------------
	/// \brief Pointer to the table of CAN signals.
	// ----------------------------------------------------------------------------------------------------------------
	const S_LibCanIL_SignalDesc_t* const pSignalDesc;
} S_LibCanIL_SignalTable_t;

// --------------------------------------------------------------------------------------------------------------------
/// \brief  Structure used for change callback configuration. This structure should be declared as constant.
// --------------------------------------------------------------------------------------------------------------------
typedef struct
{
	// ----------------------------------------------------------------------------------------------------------------
	/// \brief	Pointer for change callback function.
	// ----------------------------------------------------------------------------------------------------------------
	const LibCanIL_DataChangedCbk	Callback;

} S_LibCanIL_CallbackDesc_t;

// --------------------------------------------------------------------------------------------------------------------
/// \brief Structure used as table of change callback configuration. This structure should be declared as constant.
// --------------------------------------------------------------------------------------------------------------------
typedef struct
{
	// ----------------------------------------------------------------------------------------------------------------
	/// \brief Number of callbacks.
	// ----------------------------------------------------------------------------------------------------------------
	const uint8_t NumOfCallbacks;

	// ----------------------------------------------------------------------------------------------------------------
	/// \brief Pointer to the table of change callbacks.
	// ----------------------------------------------------------------------------------------------------------------
	const S_LibCanIL_CallbackDesc_t* const pCallbackDesc;
} S_LibCanIL_CallbackTable_t;


// --------------------------------------------------------------------------------------------------------------------
//	Imported Variables
// --------------------------------------------------------------------------------------------------------------------
extern const S_LibCanIL_MessageTable_t	LibCanILCfg_MessageTable;
extern const S_LibCanIL_SignalTable_t	LibCanILCfg_SignalTable;
extern const S_LibCanIL_CallbackTable_t	LibCanILCfg_CallbackTable;
extern S_LibFifoQueue_Inst_t LibCanIL_MsgReqFifo;
extern S_LibFifoQueue_Inst_t LibCanIL_MsgIndFifo;
extern S_LibFifoQueue_Inst_t LibCanIL_MsgConFifo;

// --------------------------------------------------------------------------------------------------------------------
/// \brief Service Instance of the CAN Interaction Layer
// --------------------------------------------------------------------------------------------------------------------
extern S_LibService_Inst_t LibCanIL_Service;

extern const S_LibCanModule_Module_t LibCanIL_Module;

// --------------------------------------------------------------------------------------------------------------------
//	Global Function Prototypes
// --------------------------------------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------------------------------------
/// \brief Set the new value to signal.
///
/// \param sigName
/// Name of signal to be set
/// \param sigValue
/// New value of signal
// --------------------------------------------------------------------------------------------------------------------
extern void LibCanIL_SetSignal(E_LibCanILCfg_SignalNames_t sigName, uint64_t sigValue);

// --------------------------------------------------------------------------------------------------------------------
/// \brief Get the value of the signal.
///
/// \param sigName
/// Name of signal to be get
// --------------------------------------------------------------------------------------------------------------------
extern uint64_t LibCanIL_GetSignal(E_LibCanILCfg_SignalNames_t sigName);

// --------------------------------------------------------------------------------------------------------------------
/// \brief This function transmit a non-cycle CAN transmit messages.
///
/// \param msgName
/// Name of message to be transmit
// --------------------------------------------------------------------------------------------------------------------
extern void LibCanIL_TransmitMessage(E_LibCanILCfg_MessageNames_t msgName);
// --------------------------------------------------------------------------------------------------------------------
/// \brief This function calls all requested message and signal changed callback.
// --------------------------------------------------------------------------------------------------------------------
extern void LibCanIL_CallRequestedCallbacks(void);

// --------------------------------------------------------------------------------------------------------------------
/// \brief Start the transmission.
// --------------------------------------------------------------------------------------------------------------------
extern void LibCanIL_TxStart(void);

// --------------------------------------------------------------------------------------------------------------------
/// \brief Start the reception.
// --------------------------------------------------------------------------------------------------------------------
extern void LibCanIL_RxStart(void);

// --------------------------------------------------------------------------------------------------------------------
/// \brief Stop the transmission.
// --------------------------------------------------------------------------------------------------------------------
extern void LibCanIL_TxStop(void);

// --------------------------------------------------------------------------------------------------------------------
/// \brief Stop the reception.
// --------------------------------------------------------------------------------------------------------------------
extern void LibCanIL_RxStop(void);

// --------------------------------------------------------------------------------------------------------------------
/// \brief Enable the transmission.
// --------------------------------------------------------------------------------------------------------------------
extern void LibCanIL_TxEnable(void);

// --------------------------------------------------------------------------------------------------------------------
/// \brief Enable the reception.
// --------------------------------------------------------------------------------------------------------------------
extern void LibCanIL_RxEnable(void);

// --------------------------------------------------------------------------------------------------------------------
/// \brief Disable the transmission.
// --------------------------------------------------------------------------------------------------------------------
extern void LibCanIL_TxDisable(void);

// --------------------------------------------------------------------------------------------------------------------
/// \brief Disable the reception.
// --------------------------------------------------------------------------------------------------------------------
extern void LibCanIL_RxDisable(void);

// --------------------------------------------------------------------------------------------------------------------
/// \brief Is this can massage part of the Interaction Layer
// --------------------------------------------------------------------------------------------------------------------
extern bool_t LibCanIL_IsMsgIL(uint32_t msgId);

// --------------------------------------------------------------------------------------------------------------------
/// \brief Start Send Event Message
// --------------------------------------------------------------------------------------------------------------------
extern void LibCanIL_SendEventMsgStart(E_LibCanILCfg_MessageNames_t msgName);

// --------------------------------------------------------------------------------------------------------------------
/// \brief Set Cycle Tx Message Enabled
// --------------------------------------------------------------------------------------------------------------------
extern void LibCanIL_SetTxCycleMessageEnabled(E_LibCanILCfg_MessageNames_t msgName, bool enableflag);

extern bool_t LibCanIL_GetTxCycleMessageEnabled(E_LibCanILCfg_MessageNames_t msgName);
#endif // LIBCANIL_H__INCLUDED
