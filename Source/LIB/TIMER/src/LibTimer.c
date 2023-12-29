// -------------------------------------------------------------------------------------------------------------------- 
/// 
/// \file LibUsart.c
/// 
/// \brief LibUsart
/// 
/// 
/// $Id$ 
/// 
/// Copyright (c) 2021, wangjun. 
/// All Rights Reserved. 
/// 
// -------------------------------------------------------------------------------------------------------------------- 

// -------------------------------------------------------------------------------------------------------------------- 
//  Includes 
// --------------------------------------------------------------------------------------------------------------------
#include "LibTimer.h"
#include "LibTypes.h"


// -------------------------------------------------------------------------------------------------------------------- 
//  Local Definitions 
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
/// \brief The number of milliseconds that have elapsed since the system was started.
// --------------------------------------------------------------------------------------------------------------------
static volatile uint32_t LibTimer_UpTime_ms = UINT32_C(0);

// --------------------------------------------------------------------------------------------------------------------
/// \brief Pointer to the first timer in the active list.
// --------------------------------------------------------------------------------------------------------------------
static S_LibTimer_Inst_t* LibTimer_pFirstActiveTimer = NULL;

// --------------------------------------------------------------------------------------------------------------------
//	Local Function Prototypes
// --------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------
/// \brief Insert a timer into the double linked timer list.
///
/// The timer list is sorted using the absolute timeout of the timers: the first timer on the list will be the timer
/// which expires first. This is also true if this list contains timers which require an overflow prior to expiration:
/// the first overflowed timer will be stored after the last non-overflowed timer.
///
/// \attention
/// This function requires S_LibTimer_Inst_t.pNext and S_LibTimer_Inst_t.pPrev to be initialized with NULL.
/// \param pTimer
/// Pointer to the timer.
// --------------------------------------------------------------------------------------------------------------------
static void LibTimer_InsertIntoList(S_LibTimer_Inst_t* const pTimer);

// --------------------------------------------------------------------------------------------------------------------
//	Functions
// --------------------------------------------------------------------------------------------------------------------
//=====================================================================================================================
// LibTimer_GetUpTime_ms:
//=====================================================================================================================
uint32_t LibTimer_GetUpTime_ms(void)
{
	uint32_t retval;
	SuspendAllInterrupts();
	retval = LibTimer_UpTime_ms;
	ResumeAllInterrupts();
	return retval;
}
//=====================================================================================================================
// LibTimer_Init:
//=====================================================================================================================
void LibTimer_Init(void)
{
    LibTimer_UpTime_ms = 0;
}

//=====================================================================================================================
// LibTimer_Tick:
//=====================================================================================================================
void LibTimer_Tick(void)
{
	S_LibTimer_Inst_t* pEntry = LibTimer_pFirstActiveTimer;
	S_LibTimer_Inst_t* pPrevEntry = NULL;
	const uint32_t upTime_ms = LibTimer_UpTime_ms + 1U;

	// increment the number of milliseconds that have elapsed since the system was started
	LibTimer_UpTime_ms = upTime_ms;
	
	// check for timers which have expired
	while ((NULL != pEntry) && (upTime_ms == pEntry->Timeout_ms))
	{
		// the timer has expired, invoke the callback
		pEntry->Callback(pEntry->pData);

		// continue with the next timer
		pPrevEntry = pEntry;
		pEntry = pEntry->pNext;
	}

	// split the list after the last expired timer
	if (NULL != pEntry)
	{
		pEntry->pPrev = NULL;
	}
	LibTimer_pFirstActiveTimer = pEntry;

	// remove the expired timers from the list and re-insert periodic timers
	while (NULL != pPrevEntry)
	{
		// remove the timer from the list
		S_LibTimer_Inst_t* const pTimer = pPrevEntry;
		pPrevEntry = pTimer->pPrev;
		pTimer->pNext = NULL;
		pTimer->pPrev = NULL;

		// re-insert periodic timers
		if (UINT32_C(0) != pTimer->Period_ms)
		{
			pTimer->Timeout_ms = upTime_ms + pTimer->Period_ms;
			LibTimer_InsertIntoList(pTimer);
		}
	}
}


//=====================================================================================================================
// LibTimer_Start:
//=====================================================================================================================
bool_t LibTimer_Start(S_LibTimer_Inst_t* const pTimer, const uint32_t timeout_ms, const uint32_t period_ms)
{
	bool_t retval = false;

	SuspendAllInterrupts();
	Lib_Assert(NULL != pTimer);
	Lib_Assert(timeout_ms > UINT32_C(0));
	Lib_Assert(NULL != pTimer->Callback);
	if (((NULL == pTimer->pPrev) && (pTimer != LibTimer_pFirstActiveTimer)))
	{
		// increment the number of milliseconds: the timer waits for the given amount of millisecond ticks to come, so 
		// if this function is called in the middle of two millisecond ticks, the time waited would be too short
		pTimer->Timeout_ms = LibTimer_UpTime_ms + timeout_ms + 1U;
		pTimer->Period_ms = period_ms;
		LibTimer_InsertIntoList(pTimer);
		retval = true;
	}
	ResumeAllInterrupts();

	return retval;
}


//=====================================================================================================================
// LibTimer_Stop:
//=====================================================================================================================
void LibTimer_Stop(S_LibTimer_Inst_t* const pTimer)
{
	SuspendAllInterrupts();
	Lib_Assert(NULL != pTimer);
	if ((NULL != pTimer->pPrev) || (pTimer == LibTimer_pFirstActiveTimer))
	{
		// remove the timer from the list: update the previous entry in the double linked list first
		if (NULL == pTimer->pPrev)
		{
			// remove the first entry from the list
			LibTimer_pFirstActiveTimer = pTimer->pNext;
		}
		else
		{
			// remove any other entry from the list
			pTimer->pPrev->pNext = pTimer->pNext;
		}

		// update the next entry in the double linked list
		if (NULL != pTimer->pNext)
		{
			pTimer->pNext->pPrev = pTimer->pPrev;
		}

		// reset the previous and next pointer of the timer
		pTimer->pPrev = NULL;
		pTimer->pNext = NULL;
	}
	ResumeAllInterrupts();
}


//=====================================================================================================================
// LibTimer_GetTimeLeft_ms:
//=====================================================================================================================
uint32_t LibTimer_GetTimeLeft_ms(const S_LibTimer_Inst_t* const pTimer)
{
	uint32_t retval = UINT32_C(0);

	SuspendAllInterrupts();
	Lib_Assert(NULL != pTimer);
	if ((NULL != pTimer->pPrev) || (pTimer == LibTimer_pFirstActiveTimer))
	{
		retval = pTimer->Timeout_ms - LibTimer_UpTime_ms;
	}
	ResumeAllInterrupts();

	return retval;
}


//=====================================================================================================================
// LibTimer_InsertIntoList:
//=====================================================================================================================
static void LibTimer_InsertIntoList(S_LibTimer_Inst_t* const pTimer)
{
	const uint32_t upTime_ms = LibTimer_UpTime_ms;
	Lib_Assert((NULL == pTimer->pPrev) && (NULL == pTimer->pNext));

	// check if the timer list is empty
	if (NULL == LibTimer_pFirstActiveTimer)
	{
		// the timer list is empty, insert as the first entry
		LibTimer_pFirstActiveTimer = pTimer;
	}
	else
	{
		// the timer list is not empty, check whether the absolute timeout of the timer results in an overflow
		S_LibTimer_Inst_t* pEntry = LibTimer_pFirstActiveTimer;
		S_LibTimer_Inst_t* pPrevEntry = NULL;
		if (upTime_ms >= pTimer->Timeout_ms)
		{
			// the absolute timeout of the timer results in an overflow: search for the first timer whose absolute
			// timeout is higher than the absolute timeout of the timer to insert and whose absolute timeout results
			// in an overflow
			while ((NULL != pEntry) && ((pTimer->Timeout_ms > pEntry->Timeout_ms) || (upTime_ms < pEntry->Timeout_ms)))
			{
				pPrevEntry = pEntry;
				pEntry = pEntry->pNext;
			}
		}
		else
		{
			// the absolute timeout of the timer results in no overflow: search for the first timer whose absolute
			// timeout is higher than the absolute timeout of the timer to insert or whose absolute timeout results in
			// an overflow
			while ((NULL != pEntry) && (pTimer->Timeout_ms > pEntry->Timeout_ms) && (upTime_ms < pEntry->Timeout_ms))
			{
				pPrevEntry = pEntry;
				pEntry = pEntry->pNext;
			}
		}

		// check whether a timer whose absolute timeout is higher than the absolute timeout of the timer to insert has
		// been found
		if (NULL == pEntry)
		{
			// no timer whose absolute timeout is higher than the absolute timeout of the timer to insert has been
			// found: insert as the last entry

			//lint -e{794} pEntry is only NULL if the loops above have executed at least once: pPrevEntry will always
			//be valid in this case
			pPrevEntry->pNext = pTimer;
			pTimer->pPrev = pPrevEntry;
		}
		else
		{
			// a timer whose absolute timeout is higher than the absolute timeout of the timer to insert has been
			// found: insert directly in front of the timer which has been found
			pTimer->pNext = pEntry;
			if (NULL != pPrevEntry)
			{
				pTimer->pPrev = pPrevEntry;
				pPrevEntry->pNext = pTimer;
			}
			else
			{
				// the timer which has been found is the first entry, insert as the new first entry
				LibTimer_pFirstActiveTimer = pTimer;
			}
			pEntry->pPrev = pTimer;
		}
	}
}


