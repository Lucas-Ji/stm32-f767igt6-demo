// --------------------------------------------------------------------------------------------------------------------
///
/// \file LibServiceHost.h
///
/// \brief Service host implementation.
///
/// A service host is required by an OSEK task to host one or more services (pseudo threads). See the documentation of
/// "LibService.h" for further information about services.
///
/// All Rights Reserved.
///
// --------------------------------------------------------------------------------------------------------------------



#ifndef LIB_SERVICE_HOST_H__INCLUDED
#define LIB_SERVICE_HOST_H__INCLUDED

// --------------------------------------------------------------------------------------------------------------------
//  Includes
// --------------------------------------------------------------------------------------------------------------------
#include "LibTypes.h"


// --------------------------------------------------------------------------------------------------------------------
//  Global Definitions
// --------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------
/// \brief Macro to initialize a service host instance structure which invokes a callback function on service requests.
///
/// The following example shows how to create and initialize a service host instance structure:
/// \code
/// #define LIBEXAMPLE_SERVICE_COUNT (sizeof(LibExample_ServiceTable) / sizeof(LibExample_ServiceTable[0]))
/// static S_LibService_Inst_t* const LibExample_ServiceTable[] =
/// {
///		&LibExample_Service
/// };
/// static S_LibServiceHost_Inst_t LibExample_ServiceHost =
///		LIBSERVICEHOST_INIT_CALLBACK_EX(LibExample_Callback, NULL, LibExample_ServiceTable, LIBEXAMPLE_SERVICE_COUNT, false);
/// \endcode
///
/// The macro can also be used to initialize a service host instance structure part of another structure:
/// \code
/// // type definition of the structure containing the service host instance structure
/// typedef struct
/// {
///		uint32_t SomeMember;
///		S_LibServiceHost_Inst_t ServiceHost;
///		uint32_t YetAnotherMember;
/// } S_LibExample_Inst_t;
///
/// #define LIBEXAMPLE_SERVICE_COUNT (sizeof(LibExample_ServiceTable) / sizeof(LibExample_ServiceTable[0]))
/// static S_LibService_Inst_t* const LibExample_ServiceTable[] =
/// {
///		&LibExample_Service
/// };
///
/// // initialization of the structure including the service host instance structure
/// static S_LibExample_Inst_t LibExample_Inst =
/// {
///		0,
///		LIBSERVICEHOST_INIT_CALLBACK_EX(LibExample_Callback, NULL, LibExample_ServiceTable, LIBEXAMPLE_SERVICE_COUNT, false),
///		0
/// };
/// \endcode
///
/// \param callback
/// The callback function to be invoked on pending events.
/// \param pUserData
/// Pointer to user data passed back to the callback function.
/// \param pSvc
/// Array containing pointers to the services hosted by this service host instance.
/// \param count
/// The number of services in pServices.
// --------------------------------------------------------------------------------------------------------------------
#define LIBSERVICEHOST_INIT_CALLBACK_EX(callback, pUserData, pSvc, count)											\
	{																												\
		.Callback		= (callback),																				\
		.pData			= (pUserData),																				\
		.pServices		= (pSvc),																					\
		.ServiceCount	= (count)																					\
	}

// --------------------------------------------------------------------------------------------------------------------
/// \brief Macro to initialize a service host instance structure which invokes a callback function on service requests.
///
/// The following example shows how to create and initialize a service host instance structure:
/// \code
/// #define LIBEXAMPLE_SERVICE_COUNT (sizeof(LibExample_ServiceTable) / sizeof(LibExample_ServiceTable[0]))
/// static S_LibService_Inst_t* const LibExample_ServiceTable[] = 
/// {
///		&LibExample_Service
/// };
/// static S_LibServiceHost_Inst_t LibExample_ServiceHost = 
///		LIBSERVICEHOST_INIT_CALLBACK(LibExample_Callback, NULL, LibExample_ServiceTable, LIBEXAMPLE_SERVICE_COUNT);
/// \endcode
///
/// The macro can also be used to initialize a service host instance structure part of another structure:
/// \code
/// // type definition of the structure containing the service host instance structure
/// typedef struct 
/// {
///		uint32_t SomeMember;
///		S_LibServiceHost_Inst_t ServiceHost;
///		uint32_t YetAnotherMember;
/// } S_LibExample_Inst_t;
///
/// #define LIBEXAMPLE_SERVICE_COUNT (sizeof(LibExample_ServiceTable) / sizeof(LibExample_ServiceTable[0]))
/// static S_LibService_Inst_t* const LibExample_ServiceTable[] = 
/// {
///		&LibExample_Service
/// };
///
/// // initialization of the structure including the service host instance structure
/// static S_LibExample_Inst_t LibExample_Inst = 
/// {
///		0, 
///		LIBSERVICEHOST_INIT_CALLBACK(LibExample_Callback, NULL, LibExample_ServiceTable, LIBEXAMPLE_SERVICE_COUNT),
///		0
/// };
/// \endcode
///
/// \param callback
/// The callback function to be invoked on pending events.
/// \param pUserData
/// Pointer to user data passed back to the callback function.
/// \param pSvc
/// Array containing pointers to the services hosted by this service host instance.
/// \param count
/// The number of services in pServices.
// --------------------------------------------------------------------------------------------------------------------
#define LIBSERVICEHOST_INIT_CALLBACK(callback, pUserData, pSvc, count)												\
	LIBSERVICEHOST_INIT_CALLBACK_EX(callback, pUserData, pSvc, count)

// --------------------------------------------------------------------------------------------------------------------
//  Global Data Types
// --------------------------------------------------------------------------------------------------------------------
struct S_LibService_Inst;

// --------------------------------------------------------------------------------------------------------------------
/// \brief Data type for a service host.
///
/// \attention
/// This structure shall not be used directly but only via the functions and macros provided by this module.
// --------------------------------------------------------------------------------------------------------------------
typedef struct S_LibServiceHost_Inst
{
	// ----------------------------------------------------------------------------------------------------------------
	/// \brief The callback function to be invoked on pending events.
	// ----------------------------------------------------------------------------------------------------------------
	void (*Callback)(void* pData);

	// ----------------------------------------------------------------------------------------------------------------
	/// \brief Pointer to user data passed back to the callback function.
	// ----------------------------------------------------------------------------------------------------------------
	void* pData;
	
	// ----------------------------------------------------------------------------------------------------------------
	/// \brief Pointer to the table containing all services to be hosted by the service host.
	// ----------------------------------------------------------------------------------------------------------------
	struct S_LibService_Inst* const * const pServices;

	// ----------------------------------------------------------------------------------------------------------------
	/// \brief The number of services in S_LibServiceHost_Inst_t.pServices.
	// ----------------------------------------------------------------------------------------------------------------
	uint32_t ServiceCount;

} S_LibServiceHost_Inst_t;


// --------------------------------------------------------------------------------------------------------------------
//  Imported variables
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
//  Global Function Prototypes
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
/// \brief Initialize a service host.
///
/// This function has to be called before calling any other function of the service host.
///
/// \attention
/// This function has also to be called for re-initialization, e.g. when the OSEK task hosting the services has been
/// re-activated.
/// \param pServiceHost
/// Pointer to the service host.
// --------------------------------------------------------------------------------------------------------------------
void LibServiceHost_Init(const S_LibServiceHost_Inst_t* const pServiceHost);


// --------------------------------------------------------------------------------------------------------------------
/// \brief Service all services with pending events hosted by the service host.
///
/// This function should be used with a service host instance initialized by #LIBSERVICEHOST_INIT_CALLBACK.
///
/// \param pServiceHost
/// Pointer to the service host.
// --------------------------------------------------------------------------------------------------------------------
void LibServiceHost_Service(const S_LibServiceHost_Inst_t* const pServiceHost);

// --------------------------------------------------------------------------------------------------------------------
/// \brief Service the next service with pending events hosted by the service host.
///
/// This function should be used with a service host instance initialized by #LIBSERVICEHOST_INIT_CALLBACK.
///
/// Unlike LibServiceHost_Service(), this function allows to execute code between servicing all services with pending
/// events hosted by the service host, i.e. to schedule after servicing each service:
/// \code
///	uint32_t i = 0U;
///	while (LibServiceHost_ServiceNext(pServiceHost, &i))
///	{
///		(void)Schedule();
///	}
/// \endcode
///
/// \attention
/// The parameter pIdx has to be initialized to 0 prior to the first call to LibServiceHost_ServiceNext().
/// \param pServiceHost
/// Pointer to the service host.
/// \param pIdx
/// Pointer to a variable initialized with 0 to store the index of the next service to be checked for pending events.
/// \retval true
/// A service with pending events has been found and the service has been serviced.
/// \retval false
/// No further service with pending events has been found.
// --------------------------------------------------------------------------------------------------------------------
bool_t LibServiceHost_ServiceNext(const S_LibServiceHost_Inst_t* const pServiceHost, uint32_t* const pIdx);

// --------------------------------------------------------------------------------------------------------------------
/// \brief Trigger the shutdown of all services hosted by the service host.
///
/// This function shall only used with a service host instance initialized by #LIBSERVICEHOST_INIT_CALLBACK to forward
/// a shutdown request. The function LibServiceHost_IsTerminated() has to be polled afterwards following each call to 
/// LibServiceHost_Service() to detect when all services have terminated.
///
/// \param pServiceHost
/// Pointer to the service host.
// --------------------------------------------------------------------------------------------------------------------
void LibServiceHost_TriggerShutdown(const S_LibServiceHost_Inst_t* const pServiceHost);

// --------------------------------------------------------------------------------------------------------------------
/// \brief Get whether all services hosted by the service host have terminated.
///
/// \param pServiceHost
/// Pointer to the service host.
/// \retval true
/// All services hosted by the service host have terminated.
/// \retval false
/// At least one service hosted by the service host has not terminated yet.
// --------------------------------------------------------------------------------------------------------------------
bool_t LibServiceHost_IsTerminated(const S_LibServiceHost_Inst_t* const pServiceHost);

// --------------------------------------------------------------------------------------------------------------------
/// \brief Notify the service host instance about a new pending event from a hosted service.
///
/// \attention
/// This function is intended for internal use only.
/// \param pServiceHost
/// Pointer to the service host.
// --------------------------------------------------------------------------------------------------------------------
void LibServiceHost_Notify(const S_LibServiceHost_Inst_t* const pServiceHost);


#endif // LIB_SERVICE_HOST_H__INCLUDED
