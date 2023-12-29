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
	//pan.sw
    LibSystem_ShutdownRequest(LIBSYSTEM_REQFORRUN_CAN, 0U, LIBAPP_SHUTDOWN_REASON_SHUTDOWN);
    SleepToShutDownCount = 2000;//go to sleep delay 2s
}

//=====================================================================================================================
// CanNmMgr_NetworkModeEntered:
//=====================================================================================================================
extern void LibSystem_WakeUpRequest(const uint32_t runTime_ms);
void CanNmMgr_NetworkModeEntered(void)
{
    LibSystem_WakeUpRequest(0xFFFFFFFF);
//    LibCanIL_TxStart();
//    LibCanIL_RxStart();
	
	 //Restart CAN IL
    LibCanIL_TxStart();
    LibCanIL_RxStart();
    
    //Enable CAN TP
    LibCanTP_TxEnable();
    LibCanTP_RxEnable();
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
    
    LibCanTP_TxDisable();
	LibCanTP_RxStop();
	
}
