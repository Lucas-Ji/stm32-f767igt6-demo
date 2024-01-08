/*
 * CanNmMgr.c
 *
 *  Created on: 2021年10月28日
 *      Author: pan.sw
 */


#include "CanNmMgr.h"
#include "CanNm.h"
#include "CanTask.h"
//  Local Function
// --------------------------------------------------------------------------------------------------------------------

//=====================================================================================================================
// CanNmMgr_PowerOnIndication:
//=====================================================================================================================
void CanNmMgr_PowerOnIndication(void)
{
	//pan.sw
    Can_SetCanTransceiverState(LIBCANTR_STATE_NORMAL);
    //LibSystem_WakeUpRequest(LIBSYSTEM_REQFORRUN_CAN, CANNM_POWER_ON_WAKEUP_TIME);
    LibSystem_WakeUpRequest(CANNM_POWER_ON_WAKEUP_TIME);
}

//=====================================================================================================================
// CanNmMgr_PowerOffIndication:
//=====================================================================================================================
void CanNmMgr_PowerOffIndication(void)
{
	//pan.sw
	Can_SetCanTransceiverState(LIBCANTR_STATE_STANDBY);
}

//=====================================================================================================================
// CanNmMgr_BusSleepEntered:
//=====================================================================================================================
void CanNmMgr_BusSleepEntered(void)
{
	//system shutdown - ECU low power
    // LibSystem_ShutdownRequest(LIBSYSTEM_REQFORRUN_CAN, 0U, LIBAPP_SHUTDOWN_REASON_SHUTDOWN);
    //SleepToShutDownCount = 2000;//go to sleep delay 2s
    LibSystem_WakeUpRequest(CANNM_WAIT_BUS_SLEEP_TIME);
}

//=====================================================================================================================
// CanNmMgr_NetworkModeEntered:
//=====================================================================================================================
void CanNmMgr_NetworkModeEntered(void)
{
	//Restart CAN IL
    LibCanIL_TxStart();
    LibCanIL_RxStart();
    
    //Enable CAN TP
    #if 0
    LibCanTP_TxEnable();
    LibCanTP_RxEnable();
    #endif /* jianggang */
}

//=====================================================================================================================
// CanNmMgr_PrepareBusSleepEntered:
//=====================================================================================================================
void CanNmMgr_PrepareBusSleepEntered(void)
{
//    LibCanIL_TxStop();
//    LibCanIL_RxStop();

	LibCanIL_TxDisable();
    LibCanIL_RxStop(); 
    
    #if 0
    LibCanTP_TxDisable();
	LibCanTP_RxStop();
    #endif /* jianggang */
	
}
