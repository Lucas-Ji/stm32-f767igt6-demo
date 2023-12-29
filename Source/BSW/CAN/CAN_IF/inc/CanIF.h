#ifndef CANIF_H__INCLUDED
#define CANIF_H__INCLUDED

// --------------------------------------------------------------------------------------------------------------------
//	Includes
// --------------------------------------------------------------------------------------------------------------------
#include "can.h"
#include "can_message.h"
#include "LibFifoQueue.h"
// --------------------------------------------------------------------------------------------------------------------
//	Global Definitions
// --------------------------------------------------------------------------------------------------------------------


/* ----------------------------------------CANIF Micro--------------------------------------------------- */

/* ------------------------------------------------------------------------------------------------------- */

// -------------------------------------------------------------------------------------------------------------------- 
/// \brief Number of messages allowed in the RECEIVE FIFO 
// -------------------------------------------------------------------------------------------------------------------- 
#define CANIF_MSG_RECV_FIFO_ELEMENTS  8U

// --------------------------------------------------------------------------------------------------------------------
//	Global Data Types
// --------------------------------------------------------------------------------------------------------------------


extern S_LibFifoQueue_Inst_t CanIF_MsgRecvFifo;
// --------------------------------------------------------------------------------------------------------------------
//	Imported Variables
// --------------------------------------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------------------------------------
//	Struct Type
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
//	Global Function Prototypes
// --------------------------------------------------------------------------------------------------------------------
extern void Can1IfDrv_Init(void);
extern void Can2IfDrv_Init(void);
extern void Can1IfDrv_Deinit(void);
extern void Can2IfDrv_Deinit(void);
extern void Can1IfDrv_Start(void);
extern void Can1IfDrv_Stop(void);
extern void Can2IfDrv_Start(void);
extern void Can2IfDrv_Stop(void);
extern void Can1IfDrv_SleepReq(void);
extern void Can1IfDrv_WakeUp(void);
extern void Can2IfDrv_SleepReq(void);
extern void Can2IfDrv_WakeUp(void);
extern void Can1_Bus_Off(void);
extern void Can2_Bus_Off(void);
extern uint8_t Can1_Bus_Off_flag;
extern uint8_t Can2_Bus_Off_flag;


#endif // CANIF_H__INCLUDED
