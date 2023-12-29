// -------------------------------------------------------------------------------------------------------------------- 
/// 
/// \file LibTimer.h
/// 
/// \brief LibTimer define 
/// 
/// 
/// All Rights Reserved. 
/// 
// -------------------------------------------------------------------------------------------------------------------- 
#ifndef _LIBTIMER_H_INCLUDED
#define _LIBTIMER_H_INCLUDED
// -------------------------------------------------------------------------------------------------------------------- 
//  Includes 
// --------------------------------------------------------------------------------------------------------------------
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
/// \brief Macro to initialize a timer instance structure.
///
/// The following example shows how to create and initialize a timer instance structure:
/// \code
/// static S_LibTimer_Inst_t LibExample_Timer = LIBTIMER_INIT_TIMER(LibExample_TimerCallback, NULL);
/// \endcode
///
/// The macro can also be used to initialize a timer instance structure part of another structure:
/// \code
/// // type definition of the structure containing the timer instance structure
/// typedef struct 
/// {
///		uint32_t SomeMember;
///		S_LibTimer_Inst_t Timer;
///		uint32_t YetAnotherMember;
/// } S_LibExample_Inst_t;
///
/// // initialization of the structure including the timer instance structure
/// static S_LibExample_Inst_t LibExample_Inst = {0, LIBTIMER_INIT_TIMER(LibExample_TimerCallback, NULL), 0};
/// \endcode
///
/// \param callback
/// The callback function to be invoked from ISR context when the timer expires.
/// \param pData
/// Pointer to user data passed back to the callback function.
// --------------------------------------------------------------------------------------------------------------------
#define LIBTIMER_INIT_TIMER(callback, pData)		{																\
														NULL,			/* pNext */									\
														NULL,			/* pPrev */									\
														UINT32_C(0),	/* Timeout_ms */							\
														UINT32_C(0),	/* Period_ms */								\
														callback,		/* Callback */								\
														pData			/* pData */									\
													}


// --------------------------------------------------------------------------------------------------------------------
//  Global Data Types
// --------------------------------------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------------------------------------
/// \brief Data type for a timer.
///
/// \attention
/// This structure shall not be used directly but only via the functions and macros provided by this module.
// --------------------------------------------------------------------------------------------------------------------
typedef struct S_LibTimer_Inst
{
	// ----------------------------------------------------------------------------------------------------------------
	/// \brief The pointer to the next entry in the linked list.
	// ----------------------------------------------------------------------------------------------------------------
	struct S_LibTimer_Inst* pNext;

	// ----------------------------------------------------------------------------------------------------------------
	/// \brief The pointer to the previous entry in the linked list.
	// ----------------------------------------------------------------------------------------------------------------
	struct S_LibTimer_Inst* pPrev;

	// ----------------------------------------------------------------------------------------------------------------
	/// \brief The absolute timeout of the timer in milliseconds.
	// ----------------------------------------------------------------------------------------------------------------
	uint32_t Timeout_ms;

	// ----------------------------------------------------------------------------------------------------------------
	/// \brief The period of the timer in milliseconds (for periodic timers).
	// ----------------------------------------------------------------------------------------------------------------
	uint32_t Period_ms;

	// ----------------------------------------------------------------------------------------------------------------
	/// \brief The callback function to be invoked from ISR context when the timer expires.
	// ----------------------------------------------------------------------------------------------------------------
	void (*Callback)(void* pData);

	// ----------------------------------------------------------------------------------------------------------------
	/// \brief Pointer to user data passed back to the callback function.
	///
	/// Each timer can have a pointer to user data that can be for example used to store a reference to an instance
	/// structure.
	// ----------------------------------------------------------------------------------------------------------------
	void* pData;

} S_LibTimer_Inst_t;


// --------------------------------------------------------------------------------------------------------------------
//  Imported variables
// --------------------------------------------------------------------------------------------------------------------

void LibTimer_Init(void);
// --------------------------------------------------------------------------------------------------------------------
//  Global Function Prototypes
// --------------------------------------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------------------------------------
/// \brief Get the number of milliseconds that have elapsed since the system was started.
///
/// \return
/// The number of milliseconds that have elapsed since the system was started.
// --------------------------------------------------------------------------------------------------------------------
uint32_t LibTimer_GetUpTime_ms(void);

// --------------------------------------------------------------------------------------------------------------------
/// \brief Perform a tick of the timer management. 
///
/// This function has to be called every millisecond in order to handle expired timers.
///
/// \attention
/// This function has to be called from ISR context or with interrupts disabled!
// --------------------------------------------------------------------------------------------------------------------
void LibTimer_Tick(void);

// --------------------------------------------------------------------------------------------------------------------
/// \brief Start a timer using the given timeout and period in milliseconds.
///
/// \note
/// The callback function will be invoked between timeout_ms and timeout_ms + 1 for the first time. Periodic timers 
/// will be invoked every period_ms.
/// \attention 
/// The timer is only started if the timer is not running.
/// \attention
/// This function will assert if timeout_ms is 0.
/// \attention
/// This function will assert if the timer callback is NULL.
/// \param pTimer
/// Pointer to the timer.
/// \param timeout_ms
/// The timeout of the timer in milliseconds.
/// \param period_ms
/// The period of the timer in milliseconds in case of periodic timers. In case of one-shot timers, period must be
/// zero.
/// \retval true
/// The timer has been successfully started.
/// \retval false
/// An error has occurred starting the timer.
// --------------------------------------------------------------------------------------------------------------------
bool_t LibTimer_Start(S_LibTimer_Inst_t* const pTimer, const uint32_t timeout_ms, const uint32_t period_ms);

// --------------------------------------------------------------------------------------------------------------------
/// \brief Stop a timer.
/// 
/// \param pTimer
/// Pointer to the timer.
// --------------------------------------------------------------------------------------------------------------------
void LibTimer_Stop(S_LibTimer_Inst_t* const pTimer);

// --------------------------------------------------------------------------------------------------------------------
/// \brief Get the time of a timer in milliseconds left to run.
///
/// \param pTimer
/// Pointer to the timer.
/// \return
/// The number of milliseconds left to run (0 if the timer is not running or has finished the run).
// --------------------------------------------------------------------------------------------------------------------
uint32_t LibTimer_GetTimeLeft_ms(const S_LibTimer_Inst_t* const pTimer);

// --------------------------------------------------------------------------------------------------------------------
//	Functions
// --------------------------------------------------------------------------------------------------------------------
#endif
