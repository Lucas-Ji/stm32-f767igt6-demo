// --------------------------------------------------------------------------------------------------------------------
///
/// \file LibFsm.c
///
/// \brief Implementation of the common methods for the FSM template
///
///
///
/// All Rights Reserved.
///
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
//	Local Definitions to control properties inside header files
// --------------------------------------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------------------------------------
//	Includes
// --------------------------------------------------------------------------------------------------------------------

#include "LibFsm.h"
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
// LibFsm_DispatchEvent:
//=====================================================================================================================
void LibFsm_DispatchEvent(S_LibFsm_t* const pFsm, const uint8_t ev)
{
	uint8_t trCnt = 0;

	//Lib_Assert(pFsm->CurrentState < pFsm->pConsts->StatesNum);
	//Lib_Assert(ev < pFsm->pConsts->EvNum);
	const S_LibFsm_StateTrDesc_t* const pCurStateTr = &pFsm->pConsts->pStatesTrDesc[pFsm->CurrentState];

	if (pFsm->IsReentered)
	{
		pFsm->pConsts->pStateFunc[pFsm->CurrentState].Entry(pFsm->pUserData);
		pFsm->IsReentered = FALSE;
	}

	for (;trCnt < pCurStateTr->NumOfTransitions; trCnt++)
	{
		const S_LibFsm_TrDesc_t* const pTrDesc = &pCurStateTr->pTrTab[trCnt];
		if (pTrDesc->Event == ev)
		{
			pFsm->LastState = pFsm->CurrentState;
			pFsm->CurrentState = pTrDesc->NewState;
			pFsm->LastEvent = ev;

#ifdef LIBFSMCFG_DEBUG
			if (LIBFSM_DEBUGLEVEL_ALL == pFsm->DebugLevel)
			{
#ifdef LIBFMSCFG_USE_NAME_DECODING
				LibLog_Info("State transition from %s to %s by event %s", strlen(pFsm->pConsts->pStateNames[pFsm->LastState]), pFsm->pConsts->pStateNames[pFsm->LastState], strlen(pFsm->pConsts->pStateNames[pFsm->CurrentState]), pFsm->pConsts->pStateNames[pFsm->CurrentState], strlen(pFsm->pConsts->pEventNames[ev]), pFsm->pConsts->pEventNames[ev]);
#else
				LibLog_Info("State transition from %d to %d by event %d", pFsm->LastState, pFsm->CurrentState, ev);
#endif // LIBFMSCFG_USE_NAME_DECODING
			}
#endif // LIBFSMCFG_DEBUG

			// In case that state is not changed by event, no state entry and exit function will be called.
			if (pFsm->LastState != pFsm->CurrentState)
			{
				// If transition function dispatch event, which cause the state change. IsReentered flag catches
				// this situation and runs entry function of the state before it is changed once again.
				pFsm->IsReentered = TRUE;
				pFsm->pConsts->pStateFunc[pFsm->LastState].Exit(pFsm->pUserData);
				(*pTrDesc->pTransition)(pFsm->pUserData);
				if (pFsm->IsReentered)
				{
					pFsm->IsReentered = FALSE;
					pFsm->pConsts->pStateFunc[pFsm->CurrentState].Entry(pFsm->pUserData);
				}
			}
			else
			{
				(*pTrDesc->pTransition)(pFsm->pUserData);
			}

			break;
		}
	}

#ifdef LIBFSMCFG_DEBUG
	if ((trCnt >= pCurStateTr->NumOfTransitions) && (pFsm->DebugLevel != LIBFSM_DEBUGLEVEL_DISABLED))
	{
#ifdef LIBFMSCFG_USE_NAME_DECODING
		LibLog_Warning("Event %s is not handled in the state %s", strlen(pFsm->pConsts->pEventNames[ev]), pFsm->pConsts->pEventNames[ev], strlen(pFsm->pConsts->pStateNames[pFsm->CurrentState]), pFsm->pConsts->pStateNames[pFsm->CurrentState]);
#else
		LibLog_Warning("Event %d is not handled in the state %d", ev, pFsm->CurrentState);
#endif // LIBFMSCFG_USE_NAME_DECODING
	}
#endif // LIBFSMCFG_DEBUG
}

//=====================================================================================================================
// LibFsm_GetCurrentState:
//=====================================================================================================================
uint8_t LibFsm_GetCurrentState(const S_LibFsm_t* const pFsm)
{
	return pFsm->CurrentState;
}

//=====================================================================================================================
// LibFsm_GetLastState:
//=====================================================================================================================
uint8_t LibFsm_GetLastState(const S_LibFsm_t* const pFsm)
{
	return pFsm->LastState;
}

//=====================================================================================================================
// LibFsm_RunCurrentState:
//=====================================================================================================================
void LibFsm_RunCurrentState(const S_LibFsm_t* const pFsm)
{
	//Lib_Assert(pFsm->CurrentState < pFsm->pConsts->StatesNum);

// Logging has to be done before the running of the current state, because the state may be changed by running the
// current state.
#ifdef LIBFSMCFG_DEBUG
	if (LIBFSM_DEBUGLEVEL_ALL == pFsm->DebugLevel)
	{
#ifdef LIBFMSCFG_USE_NAME_DECODING
		LibLog_Info("Run current state %s", strlen(pFsm->pConsts->pStateNames[pFsm->CurrentState]), pFsm->pConsts->pStateNames[pFsm->CurrentState]);
#else
		LibLog_Info("Run current state %d", pFsm->CurrentState);
#endif // LIBFMSCFG_USE_NAME_DECODING
	}
#endif // LIBFSMCFG_DEBUG

	pFsm->pConsts->pStateFunc[pFsm->CurrentState].RunState(pFsm->pUserData);
}

//=====================================================================================================================
// LibFsm_GetLastEvent:
//=====================================================================================================================
uint8_t LibFsm_GetLastEvent(const S_LibFsm_t* const pFsm)
{
	return pFsm->LastEvent;
}
