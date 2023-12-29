// --------------------------------------------------------------------------------------------------------------------
///
/// \file LibService.h
///
/// \brief Interface for a service (pseudo thread).
///
/// A service is designed to run within the context of an OSEK host task using an event and timer mechanism. The event
/// mechanism of a service is completely independent from OSEK events. Setting a service event causes the service
/// function to be invoked from the context of the host task. Moreover timers can be used by a service to invoke a 
/// callback function on expiration: the callback function can be used for example to set a service event.
///
/// A service event may be set from OSEK interrupt service routines of category 2 and from timer callbacks: a service
/// is able to handle for example an interrupt by setting a service event in the OSEK interrupt service routine of 
/// category 2 and performing further actions after the host task has invoked the service function.
///
/// All Rights Reserved.
///
// --------------------------------------------------------------------------------------------------------------------



#ifndef LIB_SERVICE_H__INCLUDED
#define LIB_SERVICE_H__INCLUDED

// --------------------------------------------------------------------------------------------------------------------
//  Includes
// --------------------------------------------------------------------------------------------------------------------
#include "LibTypes.h"


// --------------------------------------------------------------------------------------------------------------------
//  Global Definitions
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
/// \brief Macro to initialize a service instance structure.
///
/// The following example shows how to create and initialize a service instance structure:
/// \code
/// static S_LibService_Inst_t LibExample_Service = LIBSERVICE_INIT_SERVICE(LibExample_ServiceFunc, NULL);
/// \endcode
///
/// The macro can also be used to initialize a service instance structure part of another structure:
/// \code
/// // type definition of the structure containing the service instance structure
/// typedef struct 
/// {
///		uint32_t SomeMember;
///		S_LibService_Inst_t Service;
///		uint32_t YetAnotherMember;
/// } S_LibExample_Inst_t;
///
/// // initialization of the structure including the service instance structure
/// static S_LibExample_Inst_t LibExample_Inst = {0, LIBSERVICE_INIT_SERVICE(LibExample_ServiceFunc, NULL), 0};
/// \endcode
///
/// \param serviceFunc
/// The service function to be invoked from the context of the host task on pending events.
/// \param pData
/// Pointer to user data passed back to the service function.
// --------------------------------------------------------------------------------------------------------------------
#define LIBSERVICE_INIT_SERVICE(serviceFunc, pData)	{																\
														NULL,			/* pServiceHost */							\
														UINT32_C(0),	/* EventMask */								\
														false,			/* Terminated */							\
														(serviceFunc),	/* ServiceFunc */							\
														(pData)			/* pData */									\
													}

// --------------------------------------------------------------------------------------------------------------------
/// \brief Service event to trigger the shutdown of the service.
///
/// On reception of this event, the service must shutdown and call LibService_Terminate() after shutting down the
/// service has finished.
// --------------------------------------------------------------------------------------------------------------------
#define LIBSERVICE_EV_TRIGGER_SHUTDOWN				UINT32_C(0x80000000)

// --------------------------------------------------------------------------------------------------------------------
/// \brief Service event to re-initialize the service.
///
/// On reception of this event, the task can perform initialization tasks which have to be performed whenever the 
/// service is re-initialized.
///
/// \attention
/// This event will only be set if the service is re-initialized: the event #LIBSERVICE_EV_INIT is set if the service
/// is initialized for the first time.
// --------------------------------------------------------------------------------------------------------------------
#define LIBSERVICE_EV_RE_INIT						UINT32_C(0x40000000)

// --------------------------------------------------------------------------------------------------------------------
/// \brief Service event to initialize the service.
///
/// On reception of this event, the task can perform initialization tasks which have to be performed if the service is
/// initialized for the first time.
///
/// \attention
/// This event will only be set if the service is initialized for the first time: the event #LIBSERVICE_EV_RE_INIT is 
/// set whenever the service is re-initialized.
// --------------------------------------------------------------------------------------------------------------------
#define LIBSERVICE_EV_INIT							UINT32_C(0x20000000)


// --------------------------------------------------------------------------------------------------------------------
//  Global Data Types
// --------------------------------------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------------------------------------
/// \brief Data type for a service.
///
/// \attention
/// This structure shall not be used directly but only via the functions and macros provided by this module.
// --------------------------------------------------------------------------------------------------------------------
typedef struct S_LibService_Inst
{
	// ----------------------------------------------------------------------------------------------------------------
	/// \brief Pointer to the service host instance which hosts this service.
	///
	/// \attention
	/// The pointer is automatically initialized by LibServiceHost_Init().
	// ----------------------------------------------------------------------------------------------------------------
	const struct S_LibServiceHost_Inst* pServiceHost;

	// ----------------------------------------------------------------------------------------------------------------
	/// \brief The mask of events currently set for the service.
	// ----------------------------------------------------------------------------------------------------------------
	volatile uint32_t EventMask;

	// ----------------------------------------------------------------------------------------------------------------
	/// \brief Stores whether the service has terminated.
	// ----------------------------------------------------------------------------------------------------------------
	bool_t Terminated;
	
	// ----------------------------------------------------------------------------------------------------------------
	/// \brief The service function to be invoked from the context of the host task on pending events.
	// ----------------------------------------------------------------------------------------------------------------
	void (*ServiceFunc)(void* pData);

	// ----------------------------------------------------------------------------------------------------------------
	/// \brief Pointer to user data passed back to the service function.
	// ----------------------------------------------------------------------------------------------------------------
	void* pData;

} S_LibService_Inst_t;


// --------------------------------------------------------------------------------------------------------------------
//  Imported variables
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
//  Global Function Prototypes
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
/// \brief Set an event that should be serviced.
///
/// After setting an event, the service function will be invoked from the OSEK host task context.
///
/// \note
/// This function may be invoked from the service function, OSEK interrupt service routines of category 2 and from 
/// timer callbacks.
/// \param pService
/// Pointer to the service.
/// \param eventMask
/// The mask of the events that should be set. Be aware that the events #LIBSERVICE_EV_INIT, #LIBSERVICE_EV_RE_INIT and
/// #LIBSERVICE_EV_TRIGGER_SHUTDOWN are reserved for internal service housekeeping and may not be used.
// --------------------------------------------------------------------------------------------------------------------
void LibService_SetEvent(S_LibService_Inst_t* const pService, const uint32_t eventMask);

// --------------------------------------------------------------------------------------------------------------------
/// \brief Get the mask of all events currently set.
///
/// \note
/// This function may be invoked from the service function, OSEK interrupt service routines of category 2 and from 
/// timer callbacks.
/// \param pService
/// Pointer to the service.
/// \return
/// The mask of all events currently set.
// --------------------------------------------------------------------------------------------------------------------
uint32_t LibService_GetEvent(const S_LibService_Inst_t* const pService);

// --------------------------------------------------------------------------------------------------------------------
/// \brief Clear an event.
///
/// \note
/// This function may be invoked from the service function, OSEK interrupt service routines of category 2 and from 
/// timer callbacks.
/// \param pService
/// Pointer to the service.
/// \param eventMask
/// The mask of the events that should be cleared.
// --------------------------------------------------------------------------------------------------------------------
void LibService_ClearEvent(S_LibService_Inst_t* const pService, const uint32_t eventMask);

// --------------------------------------------------------------------------------------------------------------------
/// \brief Get whether an event is currently set and clear the event.
///
/// \note
/// This function may be invoked from the service function, OSEK interrupt service routines of category 2 and from 
/// timer callbacks.
/// \attention
/// This function shall only be used to check whether a single event has been set and to clear the event: it is not
/// recommended to check and clear multiple events as the function only returns whether one of the events has been set.
/// \param pService
/// Pointer to the service.
///	\param eventMask
/// The mask of the event that should be checked and cleared.
/// \retval true
/// At least one of the events in eventMask has been set and all events in eventMask are cleared now.
/// \retval false
/// None of the events in eventMask has been set.
// --------------------------------------------------------------------------------------------------------------------
bool_t LibService_CheckClearEvent(S_LibService_Inst_t* const pService, const uint32_t eventMask);

// --------------------------------------------------------------------------------------------------------------------
/// \brief Terminate the service.
///
/// This function must be called after the service has terminated in response to the event 
/// #LIBSERVICE_EV_TRIGGER_SHUTDOWN.
///
/// \note
/// This function may only be invoked from the service function.
/// \param pService
/// Pointer to the service.
// --------------------------------------------------------------------------------------------------------------------
void LibService_Terminate(S_LibService_Inst_t* const pService);


#endif // LIB_SERVICE_H__INCLUDED
