// --------------------------------------------------------------------------------------------------------------------
/// \file LibCanTpInternal.c
///
/// \brief <brief description>
///
/// <detailed description>
///
/// Copyright (c) 2021 Neusoft.
/// All Rights Reserved.
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
//	Local Definitions to control properties inside header files
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
//	Includes
// --------------------------------------------------------------------------------------------------------------------

#include "LibCanTpInternal.h"
#include "LibCanTp.h"
#include "LibCanTpCfg.h"
#include "LibCanTpFsm.h"

//#include "l_endian.h"
//#include "LibAssert.h"
#include "LibCanDrvMsg.h"
#include "LibCanMsg.h"
#include "LibTypes.h"
#include "LibFsm.h"
//#include "LibLog.h"
#include <string.h>

// --------------------------------------------------------------------------------------------------------------------
//	Local Definitions 
// --------------------------------------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------------------------------------
//	Local Data Types
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
//	Global Variables 
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
//	Local Variables
// --------------------------------------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------------------------------------
//	Local Function Prototypes
// --------------------------------------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------------------------------------
/// \brief Parse received address in NormalFixedAddressing scheme. <br>
/// Implements ISO 17565-2 10.3.3
///
/// 
/// |    Type    | 28-26 | 25 | 24 | 23-16 | 15-8 | 7-0  |
/// |------------|-------|----|----|-------|------|------|
/// | - | Bits ||||||
/// | Physical   | 0x6   |  0 |  0 | 0xDA  | N_TA | N_SA |
/// | Functional | 0x6   |  0 |  0 | 0xDB  | N_TA | N_SA |
/// 
/// \param pMsg 
/// Received CAN Frame
/// \param pSrc 
/// Source address
/// \param pTgt 
/// Target address
/// \param pIsPhysical
/// if true functional addressing is used
// --------------------------------------------------------------------------------------------------------------------
static void LibCanTpInt_ParseAddrRx_NormalFixed(const S_LibCan_Msg_t* pMsg,
												uint8_t* pSrc, 
												uint8_t* pTgt, 
												bool_t* const pIsPhysical);

// --------------------------------------------------------------------------------------------------------------------
/// \brief Parse received address in NormalFixedAddressing scheme
/// 
/// \param pMsg 
/// CAN Frame that will be sent
/// \param src 
/// Source address
/// \param tgt 
/// Target address
/// \param isPhysical
/// if true functional addressing is used
// --------------------------------------------------------------------------------------------------------------------
static void LibCanTpInt_ParseAddrTx_NormalFixed(S_LibCan_Msg_t* pMsg, 
												const uint8_t src, 
												const uint8_t tgt, 
												const bool_t isPhysical);

static E_LibCanTp_FrameType_t LibCanTpInt_ParseFrameTypeRx_NormalFixed(const S_LibCan_Msg_t* pMsg);


// --------------------------------------------------------------------------------------------------------------------
//	Global Functions
// --------------------------------------------------------------------------------------------------------------------

//=====================================================================================================================
// LibCanTpInt_ParseAddrRx:
//=====================================================================================================================
void LibCanTpInt_ParseAddrRx(const S_LibCan_Msg_t* pMsg, 
							 uint8_t* pSrc, 
							 uint8_t* pTgt, 
							 bool_t* const pIsPhysical)
{
	switch (LIBCANTPCFG_ADDRESSING_SCHEME)
	{
		case LIBCANTP_ADDR_SCHEME_NORMAL_FIXED_ADDRESSING:
			LibCanTpInt_ParseAddrRx_NormalFixed(pMsg, pSrc, pTgt, pIsPhysical);
		break;
		default:
			LibLog_Error("[TODO] CAN:TP Can not handle this addressing scheme");
			Lib_Assert(false);
	}
}

//=====================================================================================================================
// LibCanTpInt_ParseAddrTx:
//=====================================================================================================================
void LibCanTpInt_ParseAddrTx(S_LibCan_Msg_t* pMsg, 
							 const uint8_t src, 
							 const uint8_t tgt, 
							 const bool_t isPhysical)
{
	switch (LIBCANTPCFG_ADDRESSING_SCHEME)
	{
		case LIBCANTP_ADDR_SCHEME_NORMAL_FIXED_ADDRESSING:
			LibCanTpInt_ParseAddrTx_NormalFixed(pMsg, src, tgt, isPhysical);
		break;
		default:
			LibLog_Error("[TODO] CAN:TP Can not handle this addressing scheme");
			Lib_Assert(false);
	}
}

//=====================================================================================================================
// LibCanTpInt_ParseFrameTypeRx:
//=====================================================================================================================
E_LibCanTp_FrameType_t LibCanTpInt_ParseFrameTypeRx(const S_LibCan_Msg_t* pMsg)
{
	E_LibCanTp_FrameType_t type;

	switch (LIBCANTPCFG_ADDRESSING_SCHEME)
	{
		case LIBCANTP_ADDR_SCHEME_NORMAL_FIXED_ADDRESSING:
			type = LibCanTpInt_ParseFrameTypeRx_NormalFixed(pMsg);
		break;
		default:
			LibLog_Error("[TODO] CAN:TP Can not handle this addressing scheme");
			Lib_Assert(false);
	}

	return type;
}

// --------------------------------------------------------------------------------------------------------------------
//	Functions
// --------------------------------------------------------------------------------------------------------------------

//=====================================================================================================================
// LibCanTpInt_ParseAddrRx_NormalFixed:
//=====================================================================================================================
inline static void LibCanTpInt_ParseAddrRx_NormalFixed(const S_LibCan_Msg_t* pMsg, 
												       uint8_t* pSrc, 
												       uint8_t* pTgt, 
												       bool_t* const pIsPhysical)
{
#if(LIBCANTP_DIAG_CANID_11BIT)
	*pSrc = (uint8_t)(pMsg->Id);
    *pTgt = (uint8_t)(LIBCANTPCFG_ECU_PHYS_ADDRESS);
	*pIsPhysical = (bool_t)(pMsg->Id == LIBCANTPCFG_TESTER_PHYS_ADDRESS);
#else
	const uint8_t addressingType = (uint8_t)((pMsg->Id >> 16U) & 0xFFU);
	*pSrc = (uint8_t)(pMsg->Id & 0xFFU);
	*pTgt = (uint8_t)((pMsg->Id >> 8U) & 0xFFU);
	*pIsPhysical = (bool_t)(0xDAU == addressingType);
	Lib_Assert((*pIsPhysical) || (0xDBU == addressingType));
#endif
}

//=====================================================================================================================
// LibCanTpInt_ParseAddrTx_NormalFixed:
//=====================================================================================================================
inline static void LibCanTpInt_ParseAddrTx_NormalFixed(S_LibCan_Msg_t* pMsg, 
												       const uint8_t src, 
												       const uint8_t tgt, 
												       const bool_t isPhysical)
{
	#if(LIBCANTP_DIAG_CANID_11BIT)
	pMsg->Id = LIBCANTPCFG_ECU_PHYS_ADDRESS;
	pMsg->IsExtId = false;
	#else
	const uint8_t addressingType = isPhysical ? 218U : 219U;
	pMsg->Id = (0x18U << 24U) | (addressingType << 16U) | (tgt << 8U) | src;
	pMsg->IsExtId = true;
	#endif
}


//=====================================================================================================================
// LibCanTpInt_ParseFrameTypeRx_NormalFixed:
//=====================================================================================================================
static E_LibCanTp_FrameType_t LibCanTpInt_ParseFrameTypeRx_NormalFixed(const S_LibCan_Msg_t* pMsg)
{
	return (E_LibCanTp_FrameType_t)((pMsg->Data[0] >> 4U) & 0x0FU);
}

//=====================================================================================================================
// LibCanTp_HandleSingleFrame:
//=====================================================================================================================
void LibCanTp_HandleSingleFrame(S_LibCanTp_Inst_t* pInst)
{
	const bool_t isInstReady = LibCanTpFsm_IsReady(pInst);
	const bool_t isInstRecv  = LibCanTpFsm_IsRecv(pInst);

	S_LibCan_Msg_t* pMsg = pInst->pCanMsg;
	uint8_t dataOffset = 1U;
	uint16_t dataLength;
	Lib_Assert((NULL != pMsg) && (NULL != pInst));
    
	//ISO 15765-2:2016(E) 9.8.3 Unexpected arrival of N_PDU
	//An unexpected N_PDU is defined as one that has been received by a node outside the expected order of N_PDUs.
	//As a general rule, arrival of an unexpected N_PDU from any node shall be ignored, with the exception of 
    //SF N_PDUs and physically addressed FF N_PDUs; functionally addressed FirstFrames shall be ignored.
    //According Table 23 锟斤拷 Handling of unexpected arrival of N_PDU with same N_AI as currently being
	//if (isInstReady || isInstRecv) //in Recv Status allow SF interrupt and respons it
	if(isInstReady)
	{

		//Jude the DLC is correct or not
		const uint8_t SingleFrame_DLC = pMsg->Data[0];
		if(    (pMsg->Length <= SingleFrame_DLC)
			|| (SingleFrame_DLC == 0)
			|| (SingleFrame_DLC > 7)
		  )
		{

			LibLog_Warning("CAN:TP get incorrect SF DLC!");
			//Get incorrect CAN_DLC or SF_DLC and should ignored
			return;
		}

		//SF interrupt should stop N_Bs and N_Cr
		if(isInstRecv)
		{
			if(0x3E == pMsg->Data[1])
		    {
				//In FOTA process, the SF 0x3E Serveice will interrupt the download process
				//So in RecvSts not allow SF 0x3E Serveice interrupt
				return;
			}
			else
			{
				//Stop N_Bs
				LibTimer_Stop(&pInst->Timers.FlowControlTimer); 
				//Stop N_Cr
				LibTimer_Stop(&pInst->Timers.ConsecutiveTimer);

				/*Then start processing the new reception*/
			}
		}

		const uint8_t frameLength = LibCan_GetMsgDataLength(pInst->pCanMsg->Length);
		uint8_t srcAddr, tgtAddr;
		bool_t isPhysical;

		LibCanTpInt_ParseAddrRx(pMsg, &srcAddr, &tgtAddr, &isPhysical);
		pInst->pDataUnit->SourceAddress = srcAddr;
		pInst->pDataUnit->TargetAddress = tgtAddr;
		pInst->pDataUnit->IsPhysical = isPhysical;

		dataLength = pMsg->Data[0U] & 0x0FU;

		if (dataLength == 0U)
		{
			dataLength = pMsg->Data[1U];
			dataOffset++;
		}

		Lib_Assert(frameLength >= (dataLength + dataOffset));

		memcpy(pInst->pDataUnit->Buffer, &pMsg->Data[dataOffset], dataLength);
		pInst->pDataUnit->Length = dataLength;
		pInst->pDataUnit->BufferRWIdx = dataLength;
		pInst->pDataUnit->BufferDataRemaining = 0U;

		LibCanTpFsm_TriggerRecv(pInst);
	}
	else
	{
		LibLog_Warning("CAN:TP Not ready for new message yet!");
	}
}

//=====================================================================================================================
// LibCanTp_HandleFirstFrame:
//=====================================================================================================================
void LibCanTp_HandleFirstFrame(S_LibCanTp_Inst_t* pInst)
{
	const bool_t isInstReady = LibCanTpFsm_IsReady(pInst);
	const bool_t isInstRecv = LibCanTpFsm_IsRecv(pInst);

	S_LibCan_Msg_t* pCanMsg = pInst->pCanMsg;
	S_LibCanTp_DataUnit_t* pMsg = pInst->pDataUnit;
    
	//ISO 15765-2:2016(E) 9.8.3 Unexpected arrival of N_PDU
	//An unexpected N_PDU is defined as one that has been received by a node outside the expected order of N_PDUs.
	//As a general rule, arrival of an unexpected N_PDU from any node shall be ignored, with the exception of 
    //SF N_PDUs and physically addressed FF N_PDUs; functionally addressed FirstFrames shall be ignored.
    //According Table 23 锟斤拷 Handling of unexpected arrival of N_PDU with same N_AI as currently being
	//if (isInstReady || isInstRecv)
	if(isInstReady)
	{
		const uint8_t frameLength = LibCan_GetMsgDataLength(pInst->pCanMsg->Length);
		uint8_t srcAddr, tgtAddr;
		bool_t isPhysical;
		uint8_t dataOffset = 0U;
		uint32_t dataLength, dataLengthInFrame;

		//Jude the DLC is correct or not
		const uint32_t FirstFrame_DLC = 0xFFFU & LDR16_BIG(&pCanMsg->Data[0U]);
		if(    (frameLength < 8)
			|| (FirstFrame_DLC < 7)
		  )
		{
			LibLog_Warning("CAN:TP get incorrect FF DLC! frameLength[%d] FirstFrame_DLC[%d]",frameLength,FirstFrame_DLC);
			//Get incorrect CAN_DLC or SF_DLC and should ignored
			return;
		}

		LibCanTpInt_ParseAddrRx(pCanMsg, &srcAddr, &tgtAddr, &isPhysical);
		if(isPhysical) // Physical addr
		{
			//If in Recv Status
			if(isInstRecv)
			{
				//Stop N_Bs
				LibTimer_Stop(&pInst->Timers.FlowControlTimer); 
				//Stop N_Cr
				LibTimer_Stop(&pInst->Timers.ConsecutiveTimer);

				/*Then start processing the new reception*/
			}	
		}
		else // Function addr
		{
			LibLog_Warning("CAN:TP Functional Addr FF and shoulde be ignored!");
			return;
			/*Ignore the new reception*/
		}
		
		pMsg->SourceAddress = srcAddr;
		pMsg->TargetAddress = tgtAddr;
		pMsg->IsPhysical = isPhysical;
		pMsg->IsFinished = false;

		dataLength = 0xFFFU & LDR16_BIG(&pCanMsg->Data[0U]);
		dataOffset += 2U;

		// ISO 17565-2 9.6.1
		// Messages larger than 4 095 bytes shall use an escape sequence where the lower nibble of Byte #1 and all bits in 
		// Byte #2 are set to 0 (invalid length). This signifies to the network layer that the value of FF_DL is determined 
		// based on the next 32 bits in the frame (Byte #3 is the MSB and Byte #6 the LSB).
		if (0U == dataLength)
		{
			dataLength = LDR32_BIG(&pCanMsg->Data[3U]);
			dataOffset += 2U;
		}

		if (dataLength > LIBCANTPCFG_MULTIFRAME_BUFFER_SIZE)
		{
			LibCanTpInt_ParseAddrTx(pCanMsg, tgtAddr, srcAddr, isPhysical);
			pCanMsg->Data[0U] = 0x32U;		// flow control identifier | flow state = OVFLW
			pCanMsg->Data[1U] = UINT8_C(0);
			pCanMsg->Data[2U] = UINT8_C(0);
			pCanMsg->Length = LIBCAN_DLCSIZE_3_B;
			LibCanTp_HandleTxMessage(pInst);
			LibCanTpFsm_TriggerInit(pInst);
			LibLog_Warning("CAN:TP incoming message exceeded buffer");
		}
		else
		{
			dataLengthInFrame = frameLength - dataOffset;
			
			memcpy(pMsg->Buffer, &pCanMsg->Data[dataOffset], dataLengthInFrame);
			pInst->pDataUnit->Length = dataLength;
			pInst->pDataUnit->BufferRWIdx = dataLengthInFrame;
			pInst->pDataUnit->BufferDataRemaining = dataLength - dataLengthInFrame;
			pInst->pDataUnit->IsRxMultiFrame = true;

			pInst->FlowCtrlSts.BlockSizeRemaining = 0U;
			pInst->FlowCtrlSts.CurSequenceNumber = 0U;

			LibLog_Debug("CAN:TP Status - RWIdx = %u Remaining = %u", pMsg->BufferRWIdx, pMsg->BufferDataRemaining);

			LibCanTpFsm_TriggerRecv(pInst);
			LibDiagCom_StartOfMsg();

			LibCanTpInt_ParseAddrTx(pCanMsg, tgtAddr, srcAddr, isPhysical);
			pCanMsg->Data[0U] = 0x30U;		// flow control identifier | flow state = CTS
			pCanMsg->Data[1U] = pInst->FlowCtrlCfg.BlockSize;
			pCanMsg->Data[2U] = pInst->FlowCtrlCfg.SepTimeMin;
			pCanMsg->Length = LIBCAN_DLCSIZE_3_B;
			LibCanTp_HandleTxMessage(pInst);
		}
	}
	else
	{
		LibLog_Warning("CAN:TP Not ready for new message yet!");
	}
}

//=====================================================================================================================
// LibCanTp_HandleConsecutiveFrame:
//=====================================================================================================================
void LibCanTp_HandleConsecutiveFrame(S_LibCanTp_Inst_t* pInst)
{
	const bool_t isInstRecv = LibCanTpFsm_IsRecv(pInst);

	if(isInstRecv)
	{
		const uint8_t frameLength = LibCan_GetMsgDataLength(pInst->pCanMsg->Length);

		S_LibCan_Msg_t* pCanMsg = pInst->pCanMsg;
		S_LibCanTp_DataUnit_t* pMsg = pInst->pDataUnit;
		uint8_t dataOffset = 0U;

		uint8_t sequenceNumber;
		uint8_t srcAddr, tgtAddr;
		bool_t isPhysical;
		uint8_t dataLengthInFrame;

		//IF not Receive FirstFrame, just abort this msg
		if(pMsg->IsRxMultiFrame == false)
		{
			LibService_SetEvent(&LibCanTp_Service, LIBSERVICE_EV_RE_INIT);
			return;
		}

		LibCanTpInt_ParseAddrRx(pCanMsg, &srcAddr, &tgtAddr, &isPhysical);
		if(!isPhysical) // Function addr
		{
			LibLog_Warning("CAN:TP Receive functional addr CF and should be ignored!");
			return;
			/*Ignore the new reception*/
		}

		LibTimer_Stop(&pInst->Timers.FlowControlTimer); //Stop Timer N_Bs


		pMsg->SourceAddress = srcAddr;
		pMsg->TargetAddress = tgtAddr;
		pMsg->IsPhysical = isPhysical;
		pMsg->IsFinished = false;

		sequenceNumber = 0x0F & pCanMsg->Data[0U];
		pInst->FlowCtrlSts.CurSequenceNumber++;
		// Sequence number is covered by 4-bits
		if  (pInst->FlowCtrlSts.CurSequenceNumber == 16U)
		{
			pInst->FlowCtrlSts.CurSequenceNumber = 0U;
		}

		if (pInst->FlowCtrlSts.CurSequenceNumber != sequenceNumber)
		{
			LibDiagCom_Error(LIBDIAGCOM_ERROR_WRONG_SEQUENCE_NUMBER);
			LibService_SetEvent(&LibCanTp_Service, LIBSERVICE_EV_RE_INIT);
			return;  //When Error occurs, the following code will not be executed
		}

		dataOffset += 1U;
		dataLengthInFrame = frameLength - dataOffset;

		// calculate the remaining data - it can happen that the frame is longer than the remaining data. So we
		// just have to read remaining bytes
		// if possible bytes in frame are greater than the number of remaining bytes take the number of remaining bytes
		dataLengthInFrame = (dataLengthInFrame > pMsg->BufferDataRemaining) ? pMsg->BufferDataRemaining : dataLengthInFrame;

		// TODO: check for padding bytes

		if (((uint32_t)dataLengthInFrame + pMsg->BufferRWIdx) > LIBCANTPCFG_MULTIFRAME_BUFFER_SIZE)
		{
			// TODO Handle Buffer overflow
			LibDiagCom_Error(LIBDIAGCOM_ERROR_BUFFER_OVERFLOW);
			LibService_SetEvent(&LibCanTp_Service, LIBSERVICE_EV_RE_INIT);
			return;  //When Error occurs, the following code will not be executed
		}

		memcpy(&pMsg->Buffer[pMsg->BufferRWIdx], &pCanMsg->Data[dataOffset], dataLengthInFrame);
		pMsg->BufferDataRemaining -= dataLengthInFrame;
		pMsg->BufferRWIdx += dataLengthInFrame;

		LibLog_Debug("CAN:TP Status - RWIdx = %u Remaining = %u", pMsg->BufferRWIdx, pMsg->BufferDataRemaining);
		if (0U == pMsg->BufferDataRemaining)
		{
			pMsg->IsFinished = true;
			pMsg->IsRxMultiFrame = false;
			// Stop timer N_Cr
			LibTimer_Stop(&pInst->Timers.ConsecutiveTimer);
		}
		else
		{
			if(dataLengthInFrame < (LIBCANTPCFG_FRAMELENGTH-1))
			{
                // Short CF DLC (not last CF)
				LibDiagCom_Error(LIBDIAGCOM_ERROR_INVALIDE_CF);
				LibService_SetEvent(&LibCanTp_Service, LIBSERVICE_EV_RE_INIT);
				return;  //When Error occurs, the following code will not be executed
			}
			else
			{
				// Restart timer N_Cr
				LibTimer_Stop(&pInst->Timers.ConsecutiveTimer);
				LibTimer_Start(&pInst->Timers.ConsecutiveTimer, LIBCANTP_CONSECUTIVE_TIMEOUT_MS, UINT32_C(0));
			}
		}

		LibCanTpFsm_TriggerRecv(pInst);
		
	}
	else
	{
        LibLog_Warning("CAN:TP get interrupt CF!");
	}
}

//=====================================================================================================================
// LibCanTp_HandleFlowControl:
//=====================================================================================================================
void LibCanTp_HandleFlowControl(S_LibCanTp_Inst_t* pInst)
{
	const bool_t isInstSend = LibCanTpFsm_IsSend(pInst);

	if(isInstSend)
	{
		S_LibCan_Msg_t* pMsg = pInst->pCanMsg;

		//jude physical or functional addr and FC DLC
		uint8_t srcAddr, tgtAddr;
		bool_t isPhysical;
		LibCanTpInt_ParseAddrRx(pMsg, &srcAddr, &tgtAddr, &isPhysical);
		if((!isPhysical) || (pMsg->Length < LIBCANTP_FC_DLC_MIN)) 
		{
			LibLog_Warning("CAN:TP get invalid FC!");
			//Abort the Sending and return
			LibDiagCom_Error(LIBDIAGCOM_ERROR_INVALIDE_FC);
	        LibService_SetEvent(&LibCanTp_Service, LIBSERVICE_EV_RE_INIT);
			return; 
		}

		if (!pInst->pDataUnit->IsFinished)
		{
			const uint8_t fs = pMsg->Data[0] & 0x0FU;
			const uint8_t bs = pMsg->Data[1];
			const uint8_t stmin = pMsg->Data[2];

			LibLog_Debug("CAN:TP RX FC FS=%u BS=%u STMIN=%u", fs, bs, stmin);
			switch (fs)
			{
				case 0U: // CTS
				{
					// ISO 17565-2 9.6.5.1
					// The FlowControl ContinueToSend parameter shall be encoded by setting the lower nibble of the N_PCI 
					// byte #1 to "0". It shall cause the sender to resume the sending of ConsecutiveFrames. The meaning 
					// of this value is that the receiver is ready to receive a maximum of BS number of ConsecutiveFrames.
					pInst->FlowCtrlSts.BlockSize = bs;
					pInst->FlowCtrlSts.BlockSizeRemaining = bs;
					pInst->FlowCtrlSts.SeparationTimeMinimum = stmin;
					//Lib_Assert(0U == stmin);
					LibCanTpFsm_TriggerSend(pInst);
					// Stop timer N_Bs
					LibTimer_Stop(&pInst->Timers.FlowControlTimer);

					LibCanTp_HandleFlowCtrlStsBlockSize(pInst);

				}
				break;
				case 1U: // WAIT
					// ISO 17565-2 9.6.5.1
					// The FlowControl Wait parameter shall be encoded by setting the lower nibble of the N_ PCI byte #1 
					// to "1". It shall cause the sender to continue to wait for a new FlowControl N_PDU and to restart 
					// its N_BS timer. If FlowStatus is set to Wait, the values of BS (BlockSize) and STmin 
					// (SeparationTime minimum) in the FlowControl message are not relevant and shall be ignored.
					LibLog_Warning("[TODO] CAN:TP FC WAIT");
					// Restart timer N_Bs
					LibTimer_Stop(&pInst->Timers.FlowControlTimer);
					LibTimer_Start(&pInst->Timers.FlowControlTimer, LIBCANTP_FLOWCONTROL_TIMEOUT_MS, UINT32_C(0));
				break;
				case 2U: // OVFLW
					// ISO 17565-2 9.6.5.1
					// The FlowControl Overflow parameter shall be encoded by setting the lower nibble of the N_PCI byte 
					// #1 to "2". It shall cause the sender to abort the transmission of a segmented message (...). This 
					// N_PCI FlowStatus parameter value is only allowed to be transmitted in the FlowControl N_PDU that 
					// follows the FirstFrame N_PDU and shall only be used if the message length FF_DL of the received 
					// FirstFrame N_PDU exceeds the buffer size of the receiving entity. If FlowStatus is set to Overflow, 
					// the values of BS (BlockSize) and STmin (SeparationTime minimum) in the FlowControl message are not 
					// relevant and shall be ignored.
					LibLog_Warning("[TODO] CAN:TP FC OVFLW");
				break;
				default:
					// ISO 17565-2 9.6.5.2
					// If an FC N_PDU message is received with an invalid (reserved) FS parameter value, the message 
					// transmission shall be aborted
					LibLog_Warning("[TODO] CAN:TP Handle reserved flow status");
			}
		}
		else
		{
			LibLog_Debug("CAN:TP RX FC unexpected");
		}
	}
	else
	{
		LibLog_Warning("CAN:TP get interrupt FC!");
	}
}

//=====================================================================================================================
// LibCanTp_HandleFlowCtrlStsBlockSize:
//=====================================================================================================================
void LibCanTp_HandleFlowCtrlStsBlockSize(S_LibCanTp_Inst_t* pInst)
{
	if(pInst->FlowCtrlSts.BlockSizeRemaining > 0)
	{
		pInst->FlowCtrlSts.BlockSizeRemaining --;
		// Restart timer N_Bs
		LibTimer_Stop(&pInst->Timers.FlowControlTimer);
		LibTimer_Start(&pInst->Timers.FlowControlTimer, LIBCANTP_FLOWCONTROL_TIMEOUT_MS, UINT32_C(0));
	}
}

//=====================================================================================================================
// LibCanTp_TransmissionTimerTimeout:
//=====================================================================================================================
void LibCanTp_TransmissionTimerTimeout(void* pData)
{
	//LibLog_Warning("LibCanTpInternal: TransmissionTimerTimeout.");
	LibDiagCom_Error(LIBDIAGCOM_ERROR_TRANSMISSION_TIMEOUT);
	LibService_SetEvent(&LibCanTp_Service, LIBSERVICE_EV_RE_INIT);
}

//=====================================================================================================================
// LibCanTp_FlowControlTimerTimeout:
//=====================================================================================================================
void LibCanTp_FlowControlTimerTimeout(void* pData)
{
	//LibLog_Warning("LibCanTpInternal: FlowControlTimerTimeout.");
	LibDiagCom_Error(LIBDIAGCOM_ERROR_FLOWCONTROL_TIMEOUT);
	LibService_SetEvent(&LibCanTp_Service, LIBSERVICE_EV_RE_INIT);
}

//=====================================================================================================================
// LibCanTp_ConsecutiveTimerTimeout:
//=====================================================================================================================
void LibCanTp_ConsecutiveTimerTimeout(void* pData)
{
	//LibLog_Warning("LibCanTpInternal: ConsecutiveTimerTimeout");
	LibDiagCom_Error(LIBDIAGCOM_ERROR_CONSECUTIVEFRAME_TIMEOUT);
	LibService_SetEvent(&LibCanTp_Service, LIBSERVICE_EV_RE_INIT);
}


//=====================================================================================================================
// LibCanTp_SeparationTimeMinTimerTimeout:
//=====================================================================================================================
void LibCanTp_SeparationTimeMinTimerTimeout(void* pData)
{
	S_LibCanTp_Inst_t* pInst = (S_LibCanTp_Inst_t*)pData;
	LibService_SetEvent(&LibCanTp_Service, LIBCANTP_SRV_EV_REQ);
	LibCanTp_HandleFlowCtrlStsBlockSize(pInst);
}
