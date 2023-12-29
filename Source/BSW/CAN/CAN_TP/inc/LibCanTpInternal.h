// --------------------------------------------------------------------------------------------------------------------
///
/// \file LibCanTpInternal.h
///
/// \brief <short descritpion>
/// <detailed description>
///
///
/// Copyright (c) 2021 Neusoft.
/// All Rights Reserved.
///
// --------------------------------------------------------------------------------------------------------------------

 

#ifndef LIBCANTPINTERNAL_H__INCLUDED
#define LIBCANTPINTERNAL_H__INCLUDED

// --------------------------------------------------------------------------------------------------------------------
//	Includes
// --------------------------------------------------------------------------------------------------------------------

#include "LibCanTp.h"
#include "LibCanTpCfg.h"

#include "LibTimer.h"

#include "LibTypes.h"

// --------------------------------------------------------------------------------------------------------------------
//	Global Definitions
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
//	Global Data Types
// --------------------------------------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------------------------------------
/// \brief Frame types defined by ISO 15765-2 (ISO TP)
// --------------------------------------------------------------------------------------------------------------------
typedef enum E_LibCanTp_FrameType_t {
	LIBCANTP_FRAMETYPE_SINGLEFRAME		= 0x00U,
	LIBCANTP_FRAMETYPE_FIRSTFRAME		= 0x01U,
	LIBCANTP_FRAMETYPE_CONSECUTIVEFRAME	= 0x02U,
	LIBCANTP_FRAMETYPE_FLOWCONTROL		= 0x03U,
} E_LibCanTp_FrameType_t;

typedef enum E_LibCanTp_AddressingScheme_t {
	LIBCANTP_ADDR_SCHEME_NORMAL_ADDRESSING,
	LIBCANTP_ADDR_SCHEME_NORMAL_FIXED_ADDRESSING,
	LIBCANTP_ADDR_SCHEME_EXTENDED_ADDRESSING,
	LIBCANTP_ADDR_SCHEME_MIXED_ADDRESSING,
} E_LibCanTp_AddressingScheme_t;

struct S_LibCanTp_Inst_t;

// --------------------------------------------------------------------------------------------------------------------
/// \brief Data Unit (N_PDU) used for services provided by the Transport Protocol
// --------------------------------------------------------------------------------------------------------------------
typedef struct S_LibCanTp_DataUnit_t {
	// ----------------------------------------------------------------------------------------------------------------
	/// \brief Source Address
	///
	/// When the message got received by the AMP this is the address of the tester. If the message is sent by the AMP
	/// this is the physical address of the AMP.
	// ----------------------------------------------------------------------------------------------------------------
	uint16_t			SourceAddress;

	// ----------------------------------------------------------------------------------------------------------------
	/// \brief Target Address
	///
	/// When the message got received this is either the physical or functional address of the AMP. If the message is
	/// is sent this is the address of the tester.
	// ----------------------------------------------------------------------------------------------------------------
	uint16_t			TargetAddress;

	// ----------------------------------------------------------------------------------------------------------------
	/// \brief Physical Addressing flag
	///
	/// If this is true the message is sent with physical addressing. 
	/// Physical Addressing: 1-to-1 conversation
	/// Functional Addressing 1-to-n conversation
	// ----------------------------------------------------------------------------------------------------------------
	bool_t				IsPhysical;

	// ----------------------------------------------------------------------------------------------------------------
	/// \brief Length of the message stored inside the buffer
	// ----------------------------------------------------------------------------------------------------------------
	uint32_t			Length;

	// ----------------------------------------------------------------------------------------------------------------
	/// \brief Buffer in which the received data or the data to be sent is stored. Its size is determined by the 
	/// configuration given by the OEM. This information can be retrieved from CDD.
	///
	/// ECU Information -> Supported Interfaces -> <Interface description>
	// ----------------------------------------------------------------------------------------------------------------
	uint8_t				Buffer[LIBCANTPCFG_MULTIFRAME_BUFFER_SIZE];

	// ----------------------------------------------------------------------------------------------------------------
	/// \brief Read/Write index for handling with the buffer
	// ----------------------------------------------------------------------------------------------------------------
	uint32_t			BufferRWIdx;

	// ----------------------------------------------------------------------------------------------------------------
	/// \brief Remainig data that has to be written/read
	/// 
	/// Should be equivalent to Length - BufferRWIdx
	// ----------------------------------------------------------------------------------------------------------------
	uint32_t			BufferDataRemaining;

	// ----------------------------------------------------------------------------------------------------------------
	/// \brief Flag for inidication of a reception/sending is finished
	///
	/// Shoud be equivalent to BufferRWIdx == Length or BufferDataRemaining == 0
	// ----------------------------------------------------------------------------------------------------------------
	bool_t				IsFinished;

	// ----------------------------------------------------------------------------------------------------------------
	/// \brief Flag for inidication of a receive msg is multi frame
	///
	/// When Get FirstFrame set "true" and When finished set "false"
	// ----------------------------------------------------------------------------------------------------------------
	bool_t				IsRxMultiFrame;

	// ----------------------------------------------------------------------------------------------------------------
	/// \brief Flag for inidication of a send msg is multi frame
	///
	/// When send FirstFrame set "true" and When finished set "false"
	// ----------------------------------------------------------------------------------------------------------------
	bool_t				IsTxMultiFrame;
} S_LibCanTp_DataUnit_t;

// ----------------------------------------------------------------------------------------------------------------
/// \brief Configuration of sent flow control frames
// ----------------------------------------------------------------------------------------------------------------
typedef struct S_LibCanTp_FlowCtrlCfg_t {
	// --------------------------------------------------------------------------------------------------------------------
	/// \brief Number of frames inbetween two Flow Control frames (BS)
	// --------------------------------------------------------------------------------------------------------------------
	LIBCANTP_CF_PARAM_CONST uint8_t BlockSize;

	// --------------------------------------------------------------------------------------------------------------------
	/// \brief Amount of time between SendConfirm and Start of sending new frame (SeparationTime - STmin)
	// --------------------------------------------------------------------------------------------------------------------
	LIBCANTP_CF_PARAM_CONST uint8_t	SepTimeMin;
} S_LibCanTp_FlowCtrlCfg_t;

// --------------------------------------------------------------------------------------------------------------------
/// \brief Data structure holding the current state of flow
// --------------------------------------------------------------------------------------------------------------------
typedef struct S_LibCanTp_FlowCtrlSts_t {
	// ----------------------------------------------------------------------------------------------------------------
	/// \brief Current Sequence number 
	// ----------------------------------------------------------------------------------------------------------------
	uint8_t				CurSequenceNumber;

	// ----------------------------------------------------------------------------------------------------------------
	/// \brief Number of frames until the sender has to wait for next Flow Control frame
	// ----------------------------------------------------------------------------------------------------------------
	uint8_t				BlockSizeRemaining;

	// ----------------------------------------------------------------------------------------------------------------
	/// \brief Configuration of block size received by last Flow Control frame
	///
	/// 0 = not waiting for additional Flow Control frames
	/// >= 1 = Wait after BlockSize frames
	// ----------------------------------------------------------------------------------------------------------------
	uint8_t				BlockSize;

	// ----------------------------------------------------------------------------------------------------------------
	/// \brief Minimal time between two sent frames received by last Flow Control frame
	// ----------------------------------------------------------------------------------------------------------------
	uint8_t				SeparationTimeMinimum;
} S_LibCanTp_FlowCtrlSts_t;

// --------------------------------------------------------------------------------------------------------------------
/// \brief 
/// 
// --------------------------------------------------------------------------------------------------------------------
typedef struct S_LibCanTp_Timers_t {
	// ----------------------------------------------------------------------------------------------------------------
	/// \brief Timer to monitor the transmission time
	///
	/// Time for transmission of the CAN frame (any N_PDU)
	///
	/// This timer is according to ISO 17565-2 N_As (sender role) or N_Ar (receiver role)
	// ----------------------------------------------------------------------------------------------------------------
	S_LibTimer_Inst_t 		TransmissionTimer;

	// ----------------------------------------------------------------------------------------------------------------
	/// \brief Timer to monitor the timings of FlowControl messages
	///
	/// Time until reception/transmission of the next FlowControl N_PDU
	///
	/// This timer is according to ISO 17565-2 N_Bs (sender role) or N_Br (receiver role)
	// ----------------------------------------------------------------------------------------------------------------
	S_LibTimer_Inst_t		FlowControlTimer;

	// --------------------------------------------------------------------------------------------------------------------
	/// \brief 
	// --------------------------------------------------------------------------------------------------------------------
	E_LibCanTp_FrameType_t 	LastFrame;

	// ----------------------------------------------------------------------------------------------------------------
	/// \brief Timer to monitor the timings of ConsecutiveFrame messages
	///
	/// Time until reception/transmission of the next Consecutive Frame N_PDU
	///
	/// This timer is according to ISO 17565-2 N_Cs (sender role) or N_Cr (receiver role)
	// ----------------------------------------------------------------------------------------------------------------
	S_LibTimer_Inst_t		ConsecutiveTimer;

	// ----------------------------------------------------------------------------------------------------------------
	/// \brief Timer to monitor the timings of Send ConsecutiveFrame messages
	///
	/// 
	///
	/// 
	// ----------------------------------------------------------------------------------------------------------------
	S_LibTimer_Inst_t		SeparationTimeMinTimer;
} S_LibCanTp_Timers_t;

typedef struct S_LibCanTp_Inst_t {
	// ----------------------------------------------------------------------------------------------------------------
	/// \brief Driver Device ID for which the Instance is running
	// ----------------------------------------------------------------------------------------------------------------
	const E_LibDrv_DevId_t 			DevId;

	// ----------------------------------------------------------------------------------------------------------------
	/// \brief Flow control configuration which is sent by the AMP
	// ----------------------------------------------------------------------------------------------------------------
	LIBCANTP_CF_PARAM_CONST S_LibCanTp_FlowCtrlCfg_t		FlowCtrlCfg;

	// ----------------------------------------------------------------------------------------------------------------
	/// \brief Flow control status used when sending MultiFrame messages
	// ----------------------------------------------------------------------------------------------------------------
	S_LibCanTp_FlowCtrlSts_t		FlowCtrlSts;

	// ----------------------------------------------------------------------------------------------------------------
	/// \brief TP Data Unit
	// ----------------------------------------------------------------------------------------------------------------
	S_LibCanTp_DataUnit_t*			pDataUnit;

	// ----------------------------------------------------------------------------------------------------------------
	/// \brief CAN Message/Frame buffer 
	///
	/// It's used so we don't have to setup the message in stack everytime we need to send a message
	// ----------------------------------------------------------------------------------------------------------------
	S_LibCan_Msg_t*					pCanMsg;

	// ----------------------------------------------------------------------------------------------------------------
	/// \brief State Machine used for reception and sending of messages
	// ----------------------------------------------------------------------------------------------------------------
	S_LibFsm_t						Fsm;

	// ----------------------------------------------------------------------------------------------------------------
	/// \brief Instances of timers for this Transport Protocol instance
	// ----------------------------------------------------------------------------------------------------------------
	S_LibCanTp_Timers_t				Timers;
} S_LibCanTp_Inst_t;




// --------------------------------------------------------------------------------------------------------------------
//	Imported Variables
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
//	Global Function Prototypes
// --------------------------------------------------------------------------------------------------------------------

void LibCanTpInt_ParseAddrRx(const S_LibCan_Msg_t* pMsg, 
							 uint8_t* pSrc, 
							 uint8_t* pTgt, 
							 bool_t* const pIsPhysical);


void LibCanTpInt_ParseAddrTx(S_LibCan_Msg_t* pMsg, 
							 const uint8_t src, 
							 const uint8_t tgt, 
							 const bool_t isPhysical);

extern E_LibCanTp_FrameType_t LibCanTpInt_ParseFrameTypeRx(const S_LibCan_Msg_t* pMsg);


extern void LibCanTp_HandleSingleFrame(S_LibCanTp_Inst_t* pInst);
extern void LibCanTp_HandleFirstFrame(S_LibCanTp_Inst_t* pInst);
extern void LibCanTp_HandleConsecutiveFrame(S_LibCanTp_Inst_t* pInst);
extern void LibCanTp_HandleFlowControl(S_LibCanTp_Inst_t* pInst);

extern Ret_t LibCanTp_HandleTxMessage(S_LibCanTp_Inst_t* pInst);
extern void LibCanTp_HandleFlowCtrlStsBlockSize(S_LibCanTp_Inst_t* pInst);

extern void LibCanTp_TransmissionTimerTimeout(void* pData);
extern void LibCanTp_FlowControlTimerTimeout(void* pData);
extern void LibCanTp_ConsecutiveTimerTimeout(void* pData);
extern void LibCanTp_SeparationTimeMinTimerTimeout(void* pData);




#endif // LIBCANTPINTERNAL_H__INCLUDED

