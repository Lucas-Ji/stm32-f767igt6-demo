/*
 * CanNmHistory.c
 *
 *  Created on: 2021年10月28日
 *      Author: pan.sw
 */

#include "CanNmHistory.h"

#define CANNMHISTORY_DUMMY_TIMESTAMP UINT32_C(0)

// --------------------------------------------------------------------------------------------------------------------
/// \brief The size of the state history FIFO
// --------------------------------------------------------------------------------------------------------------------
#define CANNMHISTORY_STATES_FIFO_ELEMENTS UINT8_C(1)

// --------------------------------------------------------------------------------------------------------------------
/// \brief CanNm state structure as written to the non-volatile memory
// --------------------------------------------------------------------------------------------------------------------
//STRUCT_PACK_BEGIN(1)
typedef struct
{
    uint8_t stateValue;
    uint32_t TimeStamp;
} S_CanNmHistory_NewState_t;
//STRUCT_PACK_END

// --------------------------------------------------------------------------------------------------------------------
//  Local Variables
// --------------------------------------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------------------------------------
/// \brief FIFO buffer for CanNm states
// --------------------------------------------------------------------------------------------------------------------
static S_CanNmHistory_NewState_t CanNmHistory_StatesSaveBuffer[CANNMHISTORY_STATES_FIFO_ELEMENTS];

// --------------------------------------------------------------------------------------------------------------------
/// \brief THE CanNm states history FIFO
// --------------------------------------------------------------------------------------------------------------------
LIBFIFO_DEFINE_INST(CanNmHistory_NmStatesFifo,
                   (uint32_t*)(void*)CanNmHistory_StatesSaveBuffer,
                   sizeof(S_CanNmHistory_NewState_t),
                   (uint32_t)CANNMHISTORY_STATES_FIFO_ELEMENTS,
                   true);

// --------------------------------------------------------------------------------------------------------------------
/// \brief The last state pushed to the module.
// --------------------------------------------------------------------------------------------------------------------
E_CanNmHistory_States_t CanNmHistory_PrevState = CANNM_HIST_INITIAL;

// --------------------------------------------------------------------------------------------------------------------
//  Local Function
// --------------------------------------------------------------------------------------------------------------------

//=====================================================================================================================
// CanNmHistory_PushState:
//=====================================================================================================================
void CanNmHistory_PushState( E_CanNmHistory_States_t newState )
{
    if ( CanNmHistory_PrevState != newState)
    {
        CanNmHistory_PrevState = newState;

        S_CanNmHistory_NewState_t stateWithTimestamp;
        stateWithTimestamp.stateValue = (uint8_t)newState;
        stateWithTimestamp.TimeStamp = CANNMHISTORY_DUMMY_TIMESTAMP;

        (void)LibFifoQueue_Push(&CanNmHistory_NmStatesFifo, (void*)(&stateWithTimestamp));
    }
}

//=====================================================================================================================
// CanNMHistory_Save:
//=====================================================================================================================
void CanNMHistory_Save( void )
{
    S_CanNmHistory_NewState_t *stateWithTimestampToSave = LibFifoQueue_GetPopItem(&CanNmHistory_NmStatesFifo);
    if ( stateWithTimestampToSave != NULL )
    {
        // TODO NVM save
    }
    
}

//=====================================================================================================================
// CanNMHistory_Load:
//=====================================================================================================================
void CanNMHistory_Load( void )
{
    // TODO
}
