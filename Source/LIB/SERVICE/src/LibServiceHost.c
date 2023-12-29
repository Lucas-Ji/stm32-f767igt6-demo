// --------------------------------------------------------------------------------------------------------------------
/// \file LibServiceHost.c
///
/// \brief Service host implementation.
///
/// A service host is required by an OSEK task to host one or more services (pseudo threads). See the documentation of
/// "LibService.h" for further information about services.
///
/// All Rights Reserved.
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
//	Local Definitions to control properties inside header files
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
//	Includes
// --------------------------------------------------------------------------------------------------------------------
#include "LibServiceHost.h"
#include "LibService.h"
#include "LibTypes.h"


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
//	Functions
// --------------------------------------------------------------------------------------------------------------------

//=====================================================================================================================
// LibServiceHost_Init:
//=====================================================================================================================
void LibServiceHost_Init(const S_LibServiceHost_Inst_t* const pServiceHost)
{
	Lib_Assert((NULL != pServiceHost) && (pServiceHost->ServiceCount > 0U));

	// initialize all services hosted by the service host: be aware that this function will be also called for
	// re-initialization and therefore some clean-up is required (i.e. clearing event LIBSERVICE_EV_TRIGGER_SHUTDOWN)
	SuspendAllInterrupts();
	for (uint32_t i = UINT32_C(0); i < pServiceHost->ServiceCount; i++)
	{
		S_LibService_Inst_t* const pService = pServiceHost->pServices[i];
		Lib_Assert(NULL != pService);
		pService->pServiceHost = pServiceHost;
		pService->EventMask &= ~LIBSERVICE_EV_TRIGGER_SHUTDOWN;
		pService->EventMask |= pService->Terminated? LIBSERVICE_EV_RE_INIT : LIBSERVICE_EV_INIT;
		pService->Terminated = false;
	}
	ResumeAllInterrupts();

	// notify the service host about a new pending event from a hosted service
	LibServiceHost_Notify(pServiceHost);
}

//=====================================================================================================================
// LibServiceHost_Service:
//=====================================================================================================================
void LibServiceHost_Service(const S_LibServiceHost_Inst_t* const pServiceHost)
{
	uint32_t i = 0U;
	while (LibServiceHost_ServiceNext(pServiceHost, &i))
	{
	}
}

//=====================================================================================================================
// LibServiceHost_ServiceNext:
//=====================================================================================================================
bool_t LibServiceHost_ServiceNext(const S_LibServiceHost_Inst_t* const pServiceHost, uint32_t* const pIdx)
{
	bool_t retval = false;

	Lib_Assert((NULL != pServiceHost) && (NULL != pIdx));
	Lib_Assert(*pIdx <= pServiceHost->ServiceCount);

	// call the service function of the next service with pending events
	while (*pIdx < pServiceHost->ServiceCount)
	{
		// check for a pending event
		const S_LibService_Inst_t* const pService = pServiceHost->pServices[(*pIdx)++];
		if (!pService->Terminated)
		{
			// call the service function for the current service
			Lib_Assert(NULL != pService->ServiceFunc);
			pService->ServiceFunc(pService->pData);
			Lib_Assert(UINT32_C(0) == (pService->EventMask & (LIBSERVICE_EV_INIT | LIBSERVICE_EV_RE_INIT)));
			retval = true;
			break;
		}
	}

	return retval;
}

//=====================================================================================================================
// LibServiceHost_TriggerShutdown:
//=====================================================================================================================
void LibServiceHost_TriggerShutdown(const S_LibServiceHost_Inst_t* const pServiceHost)
{
	Lib_Assert(NULL != pServiceHost);

	SuspendAllInterrupts();
	for (uint32_t i = UINT32_C(0); i < pServiceHost->ServiceCount; i++)
	{
		pServiceHost->pServices[i]->EventMask |= LIBSERVICE_EV_TRIGGER_SHUTDOWN;
	}
	ResumeAllInterrupts();

	LibServiceHost_Notify(pServiceHost);
}

//=====================================================================================================================
// LibServiceHost_IsTerminated:
//=====================================================================================================================
bool_t LibServiceHost_IsTerminated(const S_LibServiceHost_Inst_t* const pServiceHost)
{
	bool_t terminated = true;
	Lib_Assert(NULL != pServiceHost);
	
	for (uint32_t i = UINT32_C(0); i < pServiceHost->ServiceCount; i++)
	{
		if (!pServiceHost->pServices[i]->Terminated)
		{
			terminated = false;
			break;
		}
	}

	return terminated;
}

//=====================================================================================================================
// LibServiceHost_Notify:
//=====================================================================================================================
void LibServiceHost_Notify(const S_LibServiceHost_Inst_t* const pServiceHost)
{
	Lib_Assert(NULL != pServiceHost);
	Lib_Assert(NULL != pServiceHost->Callback);
	pServiceHost->Callback(pServiceHost->pData);
}
