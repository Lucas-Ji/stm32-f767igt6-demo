// --------------------------------------------------------------------------------------------------------------------
///
/// \file LibFsm.h
///
/// \brief Finite State Machine Template
/// This file contains macros to be used for generation of the Finite State Machine instance within
/// *.c file. Please refer to LIBFSM_DECLARE_STATE_MACHINE macro description for more information
///
///
///
/// All Rights Reserved.
///
// --------------------------------------------------------------------------------------------------------------------


#ifndef LIB_FSM_H__INCLUDED
#define LIB_FSM_H__INCLUDED

// --------------------------------------------------------------------------------------------------------------------
//	Includes
// --------------------------------------------------------------------------------------------------------------------

#include "LibTypes.h"
#include "LibFsmInternals.h"
#include "LibFsmCfg.h"


// --------------------------------------------------------------------------------------------------------------------
//	Global Definitions
// --------------------------------------------------------------------------------------------------------------------

//lint -estring(123,*_FSM_EVENTS)
//lint -estring(123,*_FSM_STATES)

#ifdef LIBFSMCFG_DEBUG
// --------------------------------------------------------------------------------------------------------------------
/// \brief Define string from state name. Use to print state name in debug messages.
///
/// \param name
/// State name.
// --------------------------------------------------------------------------------------------------------------------
#define LIBFSM_EXPAND_STATES_NAME_DEF(name, ...) static const char_t stateName_##name[] = #name;

// --------------------------------------------------------------------------------------------------------------------
/// \brief Get string name which contain state name from states table.
///
/// \param name
/// State name.
// --------------------------------------------------------------------------------------------------------------------
#define LIBFSM_EXPAND_STATE_NAME(name, ...) stateName_##name,

// --------------------------------------------------------------------------------------------------------------------
/// \brief Define string from event name. Use to print event name in debug messages.
///
/// \param name
/// Event name.
// --------------------------------------------------------------------------------------------------------------------
#define LIBFSM_EXPAND_EVENTS_NAME_DEF(name, ...) static const char_t eventName_##name[] = #name;

// --------------------------------------------------------------------------------------------------------------------
/// \brief Get string name which contain event name from events table.
///
/// \param name
/// State name.
// --------------------------------------------------------------------------------------------------------------------
#define LIBFSM_EXPAND_EVENT_NAME(name, ...) eventName_##name,
#endif

#ifdef LIBFSMCFG_DEBUG

// --------------------------------------------------------------------------------------------------------------------
/// \brief Macro to initialize a fsm instance structure.
// --------------------------------------------------------------------------------------------------------------------
#define LIBFSM_INIT_FSM()							\
	{												\
		.CurrentState = 0U,							\
		.LastState = 0U,							\
		.LastEvent = 0U,							\
		.IsReentered = false,						\
		.pUserData = NULL,							\
		.pConsts = NULL,							\
		.DebugLevel = LIBFSM_DEBUGLEVEL_DISABLED	\
	}

#else

// --------------------------------------------------------------------------------------------------------------------
/// \brief Macro to initialize a fsm instance structure.
// --------------------------------------------------------------------------------------------------------------------
#define LIBFSM_INIT_FSM()							\
	{												\
		.CurrentState = 0U,							\
		.LastState = 0U,							\
		.LastEvent = 0U,							\
		.IsReentered = false,						\
		.pUserData = NULL,							\
		.pConsts = NULL								\
	}

#endif

// --------------------------------------------------------------------------------------------------------------------
/// \brief Expand state functions into structure of state functions from list of state declaration.
// --------------------------------------------------------------------------------------------------------------------
#define LIBFSM_EXPAND_STATE_FUNCS(...)\
{\
	.RunState	= LIBFSM_EXPAND_STATES_FUNC(__VA_ARGS__),\
	.Entry		= LIBFSM_EXPAND_STATE_TO_ENTRY_FUNC(__VA_ARGS__),\
	.Exit		= LIBFSM_EXPAND_STATE_TO_EXIT_FUNC(__VA_ARGS__)\
},

// --------------------------------------------------------------------------------------------------------------------
/// \brief  Defines module / instance specific enumeration type for the states of the FSM
/// The defined enumeration type has always following name E_xxx_State_t where xxx if the name (prefix) of the FSM
/// instance.
/// The parameter entries shall be X-macro like list of the states definitions
/// Example:
/// \code
/// #define TSTFSM_STATES(DEFINE_STATE)\<EOL>
///		DEFINE_STATE(TST_ST_UNKNOWN,	Tst_Empty		)\<EOL>
///		DEFINE_STATE(TST_ST_INIT,		Tst_InitState	)
/// \endcode
///
/// \param NAME prefix of the generated state machine
/// \param entries X-macro like list of the FSM states definitions
// --------------------------------------------------------------------------------------------------------------------
#define LIBFSM_DECLARE_STATES_ENUM(NAME, entries)\
typedef enum\
{\
	entries(LIBFSM_EXPAND_STATES_DEC) \
	NAME##_ST_NUM\
} E_##NAME##_State_t;

// --------------------------------------------------------------------------------------------------------------------
/// \brief  Macro for declaration of the finite state machine enums
///
/// \attention The FSM template can handle up to max. 254 events (255 is reserved) and 255 states
///
///
/// HOW TO USE:
///
/// To define the state machine enums sets of lists has to be defined which are then passed as parameters or used
/// implicitly by this macro.
///
/// Defining Events:\n
///
/// To define the events the list of events has to be defined in following pattern:
/// \code
/// #define TSTFSM_EVENTS(DEFINE_EVENT)\<EOL>
///		DEFINE_EVENT(TST_EV_INIT)\<EOL>
///		DEFINE_EVENT(TST_EV_ON)\<EOL>
///		DEFINE_EVENT(TST_EV_OFF)
/// \endcode
/// The DEFINE_EVENT macro shall get one parameter which is the name of the event.
/// Those names can be then later used as parameter for the Xxx_DispatchEvent function. They shall be with the
/// module unique prefix (here TST_).
///
///
/// Defining States:\n
///
/// To define states together with the corresponding handlers (function called from Xxx_GetCurrentState()), following macro
/// list shall be defined like in the example:
/// \code
/// #define TSTFSM_STATES(DEFINE_STATE)\<EOL>
///		DEFINE_STATE(TST_ST_UNKNOWN,	Tst_Empty		)\<EOL>
///		DEFINE_STATE(TST_ST_INIT,		Tst_InitState	)\<EOL>
///		DEFINE_STATE(TST_ST_INIT,		Tst_InitState, 	Tst_Entry, Tst_Exit)\<EOL>
///		DEFINE_STATE(TST_ST_INIT,		Tst_InitState, 	Tst_Empty, Tst_Exit)\<EOL>
///		DEFINE_STATE(TST_ST_INIT,		Tst_InitState, 	Tst_Entry, Tst_Empty)
/// \endcode
/// The DEFINE_STATE shall get two or four parameters:
/// - name of state - has to be defined with module unique prefix
/// - state handler - has to be declared with module unique prefix. It make sense (but it's not obligatory) to use the same prefix like the
/// one passed later on to the LIBFSM_DECLARE_STATE_MACHINE macro as NAME parameter. In case there shall be no state handler the special
/// method name LibFsm_Empty has to be used
/// - entry state handler - has to be declared with module unique prefix. It make sense (but it's not obligatory) to use the same prefix like the
/// one passed later on to the LIBFSM_DECLARE_STATE_MACHINE macro as NAME parameter. In case there shall be no entry state handler the special
/// method name LibFsm_Empty has to be used
/// - exit state handler - has to be declared with module unique prefix. It make sense (but it's not obligatory) to use the same prefix like the
/// one passed later on to the LIBFSM_DECLARE_STATE_MACHINE macro as NAME parameter. In case there shall be no exit handler the special
/// method name LibFsm_Empty has to be used
///
/// \attention Entry state handler and exit state must not cause changing of state of the state machine.
///
///
/// Defining Transitions
///
/// To define transitions between states for each state the list of transitions has to be defined. The list shall have a fixed
/// name : TRANSITIONS_FOR_STATE_xxxx where xxx is one of the states defined in the state list. This is the start state for
/// all transitions defined in the list. Each entry of the list shall contain 3 parameters:
/// - the event which triggers
/// - the destination state of the transitions
/// - the transition handler function
/// In case there shall be no action taken during states transition the empty handler LibFsm_Empty shall be used (same like for the
/// states without state handler).
/// Example of the transitions definitions for state TST_ST_INIT:
/// \code
/// #define TRANSITIONS_FOR_STATE_TST_ST_INIT(ENTRY) \<EOL>
///		ENTRY(TST_EV_ON,	TST_ST_ON,	Tst_InitToOn)\<EOL>
///		ENTRY(TST_EV_OFF,	TST_ST_OFF,	Tst_InitToOff)
/// \endcode
///
///
/// Defining state and transitions handlers
///
/// The transition and states handling functions shall have following definitions:
/// \code
/// void Prefix_MyStateHandler(void* pData)
/// {
///		...
/// }
/// \endcode
/// It's not needed to declare the functions (they have to be only defined). The declarations as static functions
/// is generated automatically during instantiation of the FSM. The pData parameter points to the data structure
/// given to the LIBFSM_DECLARE_STATE_MACHINE as USER_DATA parameter. This pointer is also stored in the Xxx_Fsm
/// structure of the sate machine.
///
/// \attention The optional functions which possibly will be never called are intentionally non static to avoid
///compiler warnings
///
/// The initial state of the state machine is always the first state in the states definition list.
///
/// \param NAME prefix for the state machine instances
/// \param EVENTS X-macro list of events definitions
/// \param STATES X-macro list of states definitions with corresponding states handlers
/// For X-Macros technique please refer to:
/// \sa http://www.embedded.com/design/programming-languages-and-tools/4403953/C-language-coding-errors-with-X-macros-Part-1
// --------------------------------------------------------------------------------------------------------------------
//lint --emacro({123}, LIBFSM_DECLARE_STATE_MACHINE_ENUMS)
#define LIBFSM_DECLARE_STATE_MACHINE_ENUMS(NAME,EVENTS,STATES)\
LIBFSM_DECLARE_EVENTS_ENUM(NAME,EVENTS) \
LIBFSM_DECLARE_STATES_ENUM(NAME,STATES)

// --------------------------------------------------------------------------------------------------------------------
/// \brief  Macro for declaration of the finite state machine structure / functions within *.c files
///
/// \attention This macro generates functions definitions and variables and MUST NOT be used in any *.h file. The
/// state and event enumerations must be defined before, e.g. via LIBFSM_DECLARE_STATE_MACHINE_ENUMS
/// \attention The FSM template can handle up to max. 254 events (255 is reserved) and 255 states
/// \attention Encapsulates LIBFSM_DECLARE_STATE_MACHINE_EX !
///
/// This macro generates complete instance of the finite state machine with the following interfaces and variables:
/// (All Xxx are replaced by the NAME parameter)
///
/// S_LibFsm_t Xxx_Fsm 						- main state machine structure - see FSM_STRUCT_TYPEDEFS macro
///
/// Xxx_DispatchEvent(E_Xxx_Event_t event) 	- main method to trigger the state machine with events
///
/// Xxx_RunCurrentState(void)				- calls handler of the current state
///
/// Xxx_GetCurrentState(void)				- returns current state
///
/// Xxx_GetLastState(void)					- returns state before last transition (could be equal to current state)
///
/// Xxx_GetLastEvent(void)					- returns last event dispatched to the state machine
///
/// Xxx_Empty(void)							- method to be used for states without run handler and transitions without handler
///											  (shall be used instead NULL in the transition and states configurations macros in case
///											  there is no specific handler). Deprecated, instead of it use LibFsm_Empty.
///
/// HOW TO USE:
///
/// To define the state machine sets of lists has to be defined which are then passed as parameters or used implicitly by this macro.
///
///
/// Defining Events
///
/// To define the events the list of events has to be defined in following pattern:
/// \code
/// #define TSTFSM_EVENTS(DEFINE_EVENT)\<EOL>
///		DEFINE_EVENT(TST_EV_INIT)\<EOL>
///		DEFINE_EVENT(TST_EV_ON)\<EOL>
///		DEFINE_EVENT(TST_EV_OFF)
/// \endcode
/// The DEFINE_EVENT macro shall get one parameter which is the name of the event.
/// Those names can be then later used as parameter for the Xxx_DispatchEvent function. They shall be with the
/// module unique prefix (here TST_).
///
///
/// Defining States
///
/// To define states together with the corresponding handlers (function called from Xxx_GetCurrentState()), following macro
/// list shall be defined like in the example:
/// \code
/// #define TSTFSM_STATES(DEFINE_STATE)\<EOL>
///		DEFINE_STATE(TST_ST_UNKNOWN,	Tst_Empty		)\<EOL>
///		DEFINE_STATE(TST_ST_INIT,		Tst_InitState	)\<EOL>
///		DEFINE_STATE(TST_ST_INIT,		Tst_InitState, 	Tst_Entry, Tst_Exit)\<EOL>
///		DEFINE_STATE(TST_ST_INIT,		Tst_InitState, 	Tst_Empty, Tst_Exit)\<EOL>
///		DEFINE_STATE(TST_ST_INIT,		Tst_InitState, 	Tst_Entry, Tst_Empty)
/// \endcode
/// The DEFINE_STATE shall get two or four parameters:
/// - name of state - has to be defined with module unique prefix
/// - state handler - has to be declared with module unique prefix. It make sense (but it's not obligatory) to use the same prefix like the
/// one passed later on to the LIBFSM_DECLARE_STATE_MACHINE macro as NAME parameter. In case there shall be no state handler the special
/// method name LibFsm_Empty has to be used
/// - entry state handler - has to be declared with module unique prefix. It make sense (but it's not obligatory) to use the same prefix like the
/// one passed later on to the LIBFSM_DECLARE_STATE_MACHINE macro as NAME parameter. In case there shall be no entry state handler the special
/// method name LibFsm_Empty has to be used
/// - exit state handler - has to be declared with module unique prefix. It make sense (but it's not obligatory) to use the same prefix like the
/// one passed later on to the LIBFSM_DECLARE_STATE_MACHINE macro as NAME parameter. In case there shall be no exit handler the special
/// method name LibFsm_Empty has to be used
///
/// \attention Entry state handler and exit state must not cause changing of state of the state machine.
///
///
/// Defining Transitions
///
/// To define transitions between states for each state the list of transitions has to be defined. The list shall have a fixed
/// name : TRANSITIONS_FOR_STATE_xxxx where xxx is one of the states defined in the state list. This is the start state for
/// all transitions defined in the list. Each entry of the list shall contain 3 parameters:
/// - the event which triggers
/// - the destination state of the transitions
/// - the transition handler function
/// In case there shall be no action taken during states transition the empty handler LibFsm_Empty shall be used (same like for the
/// states without state handler).
/// Example of the transitions definitions for state TST_ST_INIT:
/// \code
/// #define TRANSITIONS_FOR_STATE_TST_ST_INIT(ENTRY) \<EOL>
///		ENTRY(TST_EV_ON,	TST_ST_ON,	Tst_InitToOn)\<EOL>
///		ENTRY(TST_EV_OFF,	TST_ST_OFF,	Tst_InitToOff)
/// \endcode
///
///
/// Defining state and transition handlers:\n
///
/// The transition and states handling functions shall have following definitions:
/// \code
/// void Prefix_MyStateHandler(void* pData)
/// {
///		...
/// }
/// \endcode
/// It's not needed to declare the functions (they have to be only defined). The declarations as static functions
/// is generated automatically during instantiation of the FSM. The pData parameter points to the data structure
/// given to the LIBFSM_DECLARE_STATE_MACHINE as USER_DATA parameter. This pointer is also stored in the Xxx_Fsm
/// structure of the sate machine.
///
///	Usage:\n
///
/// \code
/// // declare the state and event enumerations
/// LIBFSM_DECLARE_STATE_MACHINE_ENUMS(ExampleName,TSTFSM_EVENTS,TSTFSM_STATES)
/// // declare the state machine structure / functions
/// LIBFSM_DECLARE_STATE_MACHINE_STRUCTURE(ExampleName,TSTFSM_EVENTS,TSTFSM_STATES,NULL)
/// \endcode

///
///
/// \attention The optional functions which possibly will be never called are intentionaly non static to avoid
///compiler warnings
///
/// The initial state of the state machine is always the first state in the states definition list.
///
/// \param NAME prefix for the state machine instances
/// \param EVENTS X-macro list of events definitions
/// \param STATES X-macro list of states definitions with corresponding states handlers
/// For X-Macros technique please refer to:
/// \sa http://www.embedded.com/design/programming-languages-and-tools/4403953/C-language-coding-errors-with-X-macros-Part-1
// --------------------------------------------------------------------------------------------------------------------
//lint --emacro({123}, LIBFSM_DECLARE_STATE_MACHINE_STRUCTURE)
#define LIBFSM_DECLARE_STATE_MACHINE_STRUCTURE(NAME, EVENTS, STATES, USER_DATA)\
LIBFSM_DECLARE_STATE_MACHINE_EX(NAME, NAME, STATES, USER_DATA, EVENTS)

// --------------------------------------------------------------------------------------------------------------------
/// \brief Define names for FSM states and events.
///
/// \param NAME prefix for the state machine instances
/// \param PARENT_STATE_MACHINE_NAME the name of state machine which events will be used
/// \param STATES X-macro list of states definitions with corresponding states handlers
/// \param USER_DATA pointer to the user data
/// \param EVENTS X-macro list of events definitions
// --------------------------------------------------------------------------------------------------------------------
#if defined(LIBFSMCFG_DEBUG) && defined(LIBFMSCFG_USE_NAME_DECODING)
#define LIBFSM_EXPAND_NAMES(NAME, PARENT_STATE_MACHINE_NAME, STATES, USER_DATA, EVENTS)\
STATES(LIBFSM_EXPAND_STATES_NAME_DEF)\
static const char_t* const NAME##StateNames[NAME##_ST_NUM]=\
{\
	STATES(LIBFSM_EXPAND_STATE_NAME)\
};\
EVENTS(LIBFSM_EXPAND_EVENTS_NAME_DEF)\
static const char_t* const NAME##EventNames[]=\
{\
	EVENTS(LIBFSM_EXPAND_EVENT_NAME)\
};
#else
#define LIBFSM_EXPAND_NAMES(NAME, PARENT_STATE_MACHINE_NAME, STATES, USER_DATA, EVENTS)
#endif

// --------------------------------------------------------------------------------------------------------------------
/// \brief Define FSM descriptor structure.
///
/// \param NAME prefix for the state machine instances
/// \param PARENT_STATE_MACHINE_NAME the name of state machine which events will be used
/// \param STATES X-macro list of states definitions with corresponding states handlers
/// \param USER_DATA pointer to the user data
/// \param EVENTS X-macro list of events definitions
// --------------------------------------------------------------------------------------------------------------------
#if defined(LIBFSMCFG_DEBUG) && defined(LIBFMSCFG_USE_NAME_DECODING)
#define LIBFSM_INIT_FSM_CONST_STRUCT(NAME, PARENT_STATE_MACHINE_NAME, STATES, USER_DATA, EVENTS)\
static const S_LibFsm_Consts_t NAME##_FsmConsts =\
{\
	.pStateNames			= NAME##StateNames,\
	.pEventNames			= NAME##EventNames,\
	.pStatesTrDesc			= NAME##_Stt,\
	.pStateFunc				= NAME##_StateHndl,\
	.StatesNum				= (uint8_t)NAME##_ST_NUM,\
	.EvNum					= (uint8_t)PARENT_STATE_MACHINE_NAME##_EV_NUM\
};
#else
#define LIBFSM_INIT_FSM_CONST_STRUCT(NAME, PARENT_STATE_MACHINE_NAME, STATES, USER_DATA, EVENTS)\
static const S_LibFsm_Consts_t NAME##_FsmConsts =\
{\
	.pStatesTrDesc			= NAME##_Stt,\
	.pStateFunc				= NAME##_StateHndl,\
	.StatesNum				= (uint8_t)NAME##_ST_NUM,\
	.EvNum					= (uint8_t)PARENT_STATE_MACHINE_NAME##_EV_NUM\
};
#endif

// --------------------------------------------------------------------------------------------------------------------
/// \brief Define FSM instance structure.
///
/// \param NAME prefix for the state machine instances
/// \param PARENT_STATE_MACHINE_NAME the name of state machine which events will be used
/// \param STATES X-macro list of states definitions with corresponding states handlers
/// \param USER_DATA pointer to the user data
/// \param EVENTS X-macro list of events definitions
// --------------------------------------------------------------------------------------------------------------------
#if defined(LIBFSMCFG_DEBUG)
#define LIBFSM_INIT_FSM_INST_STRUCT(NAME, PARENT_STATE_MACHINE_NAME, STATES, USER_DATA, EVENTS)\
extern S_LibFsm_t NAME##_Fsm; \
S_LibFsm_t NAME##_Fsm =\
{\
	.CurrentState	= UINT8_C(0),\
	.LastState		= UINT8_C(0),\
	.LastEvent		= (uint8_t)PARENT_STATE_MACHINE_NAME##_EV_NUM,\
	.IsReentered	= false,\
	.pUserData		= USER_DATA,\
	.pConsts		= &NAME##_FsmConsts,\
	.DebugLevel		= LIBFSM_DEBUGLEVEL_DISABLED\
};
#else
#define LIBFSM_INIT_FSM_INST_STRUCT(NAME, PARENT_STATE_MACHINE_NAME, STATES, USER_DATA, EVENTS)\
extern S_LibFsm_t NAME##_Fsm; \
S_LibFsm_t NAME##_Fsm = \
{\
	.CurrentState	= 0,\
	.LastState		= 0,\
	.LastEvent		= (uint8_t)PARENT_STATE_MACHINE_NAME##_EV_NUM,\
	.IsReentered	= FALSE,\
	.pUserData		= USER_DATA,\
	.pConsts		= &NAME##_FsmConsts\
};
#endif

// --------------------------------------------------------------------------------------------------------------------
/// \brief  Macro for the declaration of the finite state machine structure / functions within *.c files and reusage
/// of already defined event enumeration e.g. from another state machine
///
/// \attention This macro generates functions definitions and variables and MUST NOT be used in any *.h file
/// \attention The FSM template can handle up to max. 254 events (255 is reserved) and 255 states
///
/// This macro generates complete instance of the finite state machine with the following interfaces and variables:
/// (All Xxx are replaced by the NAME parameter)
///
/// S_LibFsm_t Xxx_Fsm 						- main state machine structure - see FSM_STRUCT_TYPEDEFS macro
///
/// Xxx_DispatchEvent(E_Xxx_Event_t event) 	- main method to trigger the state machine with events
///
/// Xxx_RunCurrentState(void)				- calls handler of the current state
///
/// Xxx_GetCurrentState(void)				- returns current state
///
/// Xxx_GetLastState(void)					- returns state before last transition (could be equal to current state)
///
/// Xxx_GetLastEvent(void)					- returns last event dispatched to the state machine
///
/// Xxx_Empty(void)							- method to be used for states without run handler and transitions without handler
///											  (shall be used instead NULL in the transition and states configurations macros in case
///											  there is no specific handler). Deprecated, instead of it use LibFsm_Empty.
///
/// HOW TO USE:
///
/// To define the state machine sets of lists has to be defined which are then passed as parameters or used implicitly by this macro.
///
///
/// Defining Events
///
/// To define the events the list of events has to be defined in following pattern:
/// \code
/// #define TSTFSM_EVENTS(DEFINE_EVENT)\<EOL>
///		DEFINE_EVENT(TST_EV_INIT)\<EOL>
///		DEFINE_EVENT(TST_EV_ON)\<EOL>
///		DEFINE_EVENT(TST_EV_OFF)
/// \endcode
/// The DEFINE_EVENT macro shall get one parameter which is the name of the event.
/// Those names can be then later used as parameter for the Xxx_DispatchEvent function. They shall be with the
/// module unique prefix (here TST_).
///
///
/// Defining States
///
/// To define states together with the corresponding handlers (function called from Xxx_GetCurrentState()), following macro
/// list shall be defined like in the example:
/// \code
/// #define TSTFSM_STATES(DEFINE_STATE)\<EOL>
///		DEFINE_STATE(TST_ST_UNKNOWN,	Tst_Empty		)\<EOL>
///		DEFINE_STATE(TST_ST_INIT,		Tst_InitState	)\<EOL>
///		DEFINE_STATE(TST_ST_INIT,		Tst_InitState, 	Tst_Entry, Tst_Exit)\<EOL>
///		DEFINE_STATE(TST_ST_INIT,		Tst_InitState, 	Tst_Empty, Tst_Exit)\<EOL>
///		DEFINE_STATE(TST_ST_INIT,		Tst_InitState, 	Tst_Entry, Tst_Empty)
/// \endcode
/// The DEFINE_STATE shall get two or four parameters:
/// - name of state - has to be defined with module unique prefix
/// - state handler - has to be declared with module unique prefix. It make sense (but it's not obligatory) to use the same prefix like the
/// one passed later on to the LIBFSM_DECLARE_STATE_MACHINE macro as NAME parameter. In case there shall be no state handler the special
/// method name LibFsm_Empty has to be used
/// - entry state handler - has to be declared with module unique prefix. It make sense (but it's not obligatory) to use the same prefix like the
/// one passed later on to the LIBFSM_DECLARE_STATE_MACHINE macro as NAME parameter. In case there shall be no entry state handler the special
/// method name LibFsm_Empty has to be used
/// - exit state handler - has to be declared with module unique prefix. It make sense (but it's not obligatory) to use the same prefix like the
/// one passed later on to the LIBFSM_DECLARE_STATE_MACHINE macro as NAME parameter. In case there shall be no exit handler the special
/// method name LibFsm_Empty has to be used
///
/// \attention Entry state handler and exit state must not cause changing of state of the state machine.
///
///
/// Defining Transitions
///
/// To define transitions between states for each state the list of transitions has to be defined. The list shall have a fixed
/// name : TRANSITIONS_FOR_STATE_xxxx where xxx is one of the states defined in the state list. This is the start state for
/// all transitions defined in the list. Each entry of the list shall contain 3 parameters:
/// - the event which triggers
/// - the destination state of the transitions
/// - the transition handler function
/// In case there shall be no action taken during states transition the empty handler LibFsm_Empty shall be used (same like for the
/// states without state handler).
/// Example of the transitions definitions for state TST_ST_INIT:
/// \code
/// #define TRANSITIONS_FOR_STATE_TST_ST_INIT(ENTRY) \<EOL>
///		ENTRY(TST_EV_ON,	TST_ST_ON,	Tst_InitToOn)\<EOL>
///		ENTRY(TST_EV_OFF,	TST_ST_OFF,	Tst_InitToOff)
/// \endcode
///
///
/// Defining state and transitions handlers
///
/// The transition and states handling functions shall have following definitions:
/// \code
/// void Prefix_MyStateHandler(void* pData)
/// {
///		...
/// }
/// \endcode
///
///	Usage: \n
/// \code
/// // declare the state enumeration
/// LIBFSM_DECLARE_STATES_ENUM(ExampleName,TSTFSM_STATES)
/// // declare the state machine structure / functions and reuse the events from another state machine
/// // (ExampleParentStateMachineName)
/// LIBFSM_DECLARE_STATE_MACHINE_EX(ExampleName,ExampleParentStateMachineName,TSTFSM_STATES,NULL)
/// \endcode
///
///
/// It's not needed to declare the functions (they have to be only defined). The declarations as static functions
/// is generated automatically during instantiation of the FSM. The pData parameter points to the data structure
/// given to the LIBFSM_DECLARE_STATE_MACHINE as USER_DATA parameter. This pointer is also stored in the Xxx_Fsm
/// structure of the sate machine.
///
/// \attention The optional functions which possibly will be never called are intentionaly non static to avoid
///compiler warnings
///
/// The initial state of the state machine is always the first state in the states definition list.
///
/// \param NAME prefix for the state machine instances
/// \param PARENT_STATE_MACHINE_NAME the name of state machine which events will be used
/// \param STATES X-macro list of states definitions with corresponding states handlers
/// \param USER_DATA pointer to the user data
/// \param EVENTS X-macro list of events definitions
/// For X-Macros technique please refer to:
/// \sa http://www.embedded.com/design/programming-languages-and-tools/4403953/C-language-coding-errors-with-X-macros-Part-1
// --------------------------------------------------------------------------------------------------------------------
//lint --emacro({123}, LIBFSM_DECLARE_STATE_MACHINE_EX)
#define LIBFSM_DECLARE_STATE_MACHINE_EX(NAME, PARENT_STATE_MACHINE_NAME, STATES, USER_DATA, EVENTS)\
STATES(LIBFSM_EXPAND_STATE_TO_ENTRY_FUNC_DEC)\
STATES(LIBFSM_EXPAND_STATE_TO_EXIT_FUNC_DEC)\
STATES(LIBFSM_EXPAND_STATE_TO_FUNC_DEC)\
static const S_LibFsm_StateFunc_t NAME##_StateHndl[NAME##_ST_NUM]=\
{\
	STATES(LIBFSM_EXPAND_STATE_FUNCS)\
};\
STATES(LIBFSM_DECLARE_TRDESC_TABS)\
static const S_LibFsm_StateTrDesc_t NAME##_Stt[NAME##_ST_NUM]=\
{\
	STATES(LIBFSM_EXPAND_TO_STT_INIT)\
};\
LIBFSM_EXPAND_NAMES(NAME, PARENT_STATE_MACHINE_NAME, STATES, USER_DATA, EVENTS)\
LIBFSM_INIT_FSM_CONST_STRUCT(NAME, PARENT_STATE_MACHINE_NAME, STATES, USER_DATA, EVENTS)\
LIBFSM_INIT_FSM_INST_STRUCT(NAME, PARENT_STATE_MACHINE_NAME, STATES, USER_DATA, EVENTS)\
/*lint -esym(528, *_DispatchEvent)*/\
static void NAME##_DispatchEvent(E_##PARENT_STATE_MACHINE_NAME##_Event_t ev)\
{\
	LibFsm_DispatchEvent(&NAME##_Fsm,(uint8_t)ev);\
}\
/*lint -e{830,957}*/\
void NAME##_RunCurrentState(void);\
void NAME##_RunCurrentState(void)\
{\
	LibFsm_RunCurrentState(&NAME##_Fsm);\
}\
/*lint -e{830,957}*/\
E_##NAME##_State_t NAME##_GetCurrentState(void);\
E_##NAME##_State_t NAME##_GetCurrentState(void)\
{\
	/*lint -e{9030, 9034} */\
	return (E_##NAME##_State_t)LibFsm_GetCurrentState(&NAME##_Fsm);\
}\
/*lint -e{830,957}*/\
E_##NAME##_State_t NAME##_GetLastState(void);\
E_##NAME##_State_t NAME##_GetLastState(void)\
{\
	/*lint -e{9030, 9034} */\
	return (E_##NAME##_State_t)LibFsm_GetLastState(&NAME##_Fsm);\
}\
/*lint -e{830,957}*/\
E_##PARENT_STATE_MACHINE_NAME##_Event_t NAME##_GetLastEvent(void);\
E_##PARENT_STATE_MACHINE_NAME##_Event_t NAME##_GetLastEvent(void)\
{\
	/*lint -e{9030, 9034} */\
	return (E_##PARENT_STATE_MACHINE_NAME##_Event_t)LibFsm_GetLastEvent(&NAME##_Fsm);\
}\
/*lint -e{715,818,830,957}*/\
/*lint -esym(528, *_Empty)*/\
static void NAME##_Empty(void* pData){}

// --------------------------------------------------------------------------------------------------------------------
/// \brief  Macro for declaration of the finite state machine within *.c files
/// \attention Encapsulates LIBFSM_DECLARE_STATE_MACHINE_ENUMS and LIBFSM_DECLARE_STATE_MACHINE_STRUCTURE !
/// \attention This macro generates functions definitions and variables and MUST NOT be used in any *.h file
/// \attention The FSM template can handle up to max. 254 events (255 is reserved) and 255 states
///
/// This macro generates complete instance of the finite state machine with the following interfaces and variables:
/// (All Xxx are replaced by the NAME parameter)
///
/// S_LibFsm_t Xxx_Fsm 						- main state machine structure - see FSM_STRUCT_TYPEDEFS macro
///
/// Xxx_DispatchEvent(E_Xxx_Event_t event) 	- main method to trigger the state machine with events
///
/// Xxx_RunCurrentState(void)				- calls handler of the current state
///
/// Xxx_GetCurrentState(void)				- returns current state
///
/// Xxx_GetLastState(void)					- returns state before last transition (could be equal to current state)
///
/// Xxx_GetLastEvent(void)					- returns last event dispatched to the state machine
///
/// Xxx_Empty(void)							- method to be used for states without run handler and transitions without handler
///											  (shall be used instead NULL in the transition and states configurations macros in case
///											  there is no specific handler). Deprecated, instead of it use LibFsm_Empty.
///
/// HOW TO USE:
///
/// To define the state machine sets of lists has to be defined which are then passed as parameters or used implicitly by this macro.
///
///
/// Defining Events
///
/// To define the events the list of events has to be defined in following pattern:
/// \code
/// #define TSTFSM_EVENTS(DEFINE_EVENT)\<EOL>
///		DEFINE_EVENT(TST_EV_INIT)\<EOL>
///		DEFINE_EVENT(TST_EV_ON)\<EOL>
///		DEFINE_EVENT(TST_EV_OFF)
/// \endcode
/// The DEFINE_EVENT macro shall get one parameter which is the name of the event.
/// Those names can be then later used as parameter for the Xxx_DispatchEvent function. They shall be with the
/// module unique prefix (here TST_).
///
///
/// Defining States
///
/// To define states together with the corresponding handlers (function called from Xxx_GetCurrentState()), following macro
/// list shall be defined like in the example:
/// \code
/// #define TSTFSM_STATES(DEFINE_STATE)\<EOL>
///		DEFINE_STATE(TST_ST_UNKNOWN,	Tst_Empty		)\<EOL>
///		DEFINE_STATE(TST_ST_INIT,		Tst_InitState	)\<EOL>
///		DEFINE_STATE(TST_ST_INIT,		Tst_InitState, 	Tst_Entry, Tst_Exit)\<EOL>
///		DEFINE_STATE(TST_ST_INIT,		Tst_InitState, 	Tst_Empty, Tst_Exit)\<EOL>
///		DEFINE_STATE(TST_ST_INIT,		Tst_InitState, 	Tst_Entry, Tst_Empty)
/// \endcode
/// The DEFINE_STATE shall get two or four parameters:
/// - name of state - has to be defined with module unique prefix
/// - state handler - has to be declared with module unique prefix. It make sense (but it's not obligatory) to use the same prefix like the
/// one passed later on to the LIBFSM_DECLARE_STATE_MACHINE macro as NAME parameter. In case there shall be no state handler the special
/// method name LibFsm_Empty has to be used
/// - entry state handler - has to be declared with module unique prefix. It make sense (but it's not obligatory) to use the same prefix like the
/// one passed later on to the LIBFSM_DECLARE_STATE_MACHINE macro as NAME parameter. In case there shall be no entry state handler the special
/// method name LibFsm_Empty has to be used
/// - exit state handler - has to be declared with module unique prefix. It make sense (but it's not obligatory) to use the same prefix like the
/// one passed later on to the LIBFSM_DECLARE_STATE_MACHINE macro as NAME parameter. In case there shall be no exit handler the special
/// method name LibFsm_Empty has to be used
///
/// \attention Entry state handler and exit state must not cause changing of state of the state machine.
///
///
/// Defining Transitions
///
/// To define transitions between states for each state the list of transitions has to be defined. The list shall have a fixed
/// name : TRANSITIONS_FOR_STATE_xxxx where xxx is one of the states defined in the state list. This is the start state for
/// all transitions defined in the list. Each entry of the list shall contain 3 parameters:
/// - the event which triggers
/// - the destination state of the transitions
/// - the transition handler function
/// In case there shall be no action taken during states transition the empty handler LibFsm_Empty shall be used (same like for the
/// states without state handler).
/// Example of the transitions definitions for state TST_ST_INIT:
/// \code
/// #define TRANSITIONS_FOR_STATE_TST_ST_INIT(ENTRY) \<EOL>
///		ENTRY(TST_EV_ON,	TST_ST_ON,	Tst_InitToOn)\<EOL>
///		ENTRY(TST_EV_OFF,	TST_ST_OFF,	Tst_InitToOff)
/// \endcode
///
///
/// Defining state and transitions handlers
///
/// The transition and states handling functions shall have following definitions:
/// \code
/// void Prefix_MyStateHandler(void* pData)
/// {
///		...
/// }
/// \endcode
/// It's not needed to declare the functions (they have to be only defined). The declarations as static functions
/// is generated automatically during instantiation of the FSM. The pData parameter points to the data structure
/// given to the LIBFSM_DECLARE_STATE_MACHINE as USER_DATA parameter. This pointer is also stored in the Xxx_Fsm
/// structure of the sate machine.
///
/// \attention The optional functions which possibly will be never called are intentionally non static to avoid
///compiler warnings
///
/// The initial state of the state machine is always the first state in the states definition list.
///
/// \param NAME prefix for the state machine instances
/// \param EVENTS X-macro list of events definitions
/// \param STATES X-macro list of states definitions with corresponding states handlers
/// \param USER_DATA Pointer to the user data
/// For X-Macros technique please refer to:
/// \sa http://www.embedded.com/design/programming-languages-and-tools/4403953/C-language-coding-errors-with-X-macros-Part-1
// --------------------------------------------------------------------------------------------------------------------
//lint --emacro({123}, LIBFSM_DECLARE_STATE_MACHINE)
#define LIBFSM_DECLARE_STATE_MACHINE(NAME,EVENTS,STATES,USER_DATA)\
	LIBFSM_DECLARE_STATE_MACHINE_ENUMS(NAME,EVENTS,STATES)\
	LIBFSM_DECLARE_STATE_MACHINE_STRUCTURE(NAME,EVENTS,STATES,USER_DATA)

// --------------------------------------------------------------------------------------------------------------------
//	Global Data Types
// --------------------------------------------------------------------------------------------------------------------

#ifdef LIBFSMCFG_DEBUG
// --------------------------------------------------------------------------------------------------------------------
/// \brief Enumerate all state machine debug levels.
// --------------------------------------------------------------------------------------------------------------------
typedef enum
{
	LIBFSM_DEBUGLEVEL_DISABLED,		//!< No debug messages will be printed out.
	LIBFSM_DEBUGLEVEL_UNHANDLED,	//!< Only unhandled transition debug messages will be printed out.
	LIBFSM_DEBUGLEVEL_ALL			//!< All debug messages will be printed out.
} E_LibFsm_DebugLevel_t;
#endif // LIBFSMCFG_DEBUG

// --------------------------------------------------------------------------------------------------------------------
/// \brief State handler function type definition
// --------------------------------------------------------------------------------------------------------------------
typedef void (*LibFsm_StateFunc_t)(void* pData);

// --------------------------------------------------------------------------------------------------------------------
/// \brief Transition handler function type definition
// --------------------------------------------------------------------------------------------------------------------
typedef void (*LibFsm_Transition_t)(void* pData);

// --------------------------------------------------------------------------------------------------------------------
/// \brief Single state's transition description structure
/// Describe single transition from one to another (or the same) state within FSM
// --------------------------------------------------------------------------------------------------------------------
//STRUCT_PACK_BEGIN(1)
typedef struct
{
	uint8_t				Event;						//!< event causing the state transition
	uint8_t				NewState;					//!< new state of the FSM after transition (go to state)
	LibFsm_Transition_t	pTransition;				//!< pointer to the transition handler (function called during transition)
} S_LibFsm_TrDesc_t;

// --------------------------------------------------------------------------------------------------------------------
/// \brief State transitions description structure
/// Describe all transitions for the single state of the FSM
// --------------------------------------------------------------------------------------------------------------------
typedef struct
{
	uint8_t	 					NumOfTransitions;	//!< number of defined transitions
	const S_LibFsm_TrDesc_t*	pTrTab;				//!< pointer to the table with transitions descriptions
} S_LibFsm_StateTrDesc_t;

// --------------------------------------------------------------------------------------------------------------------
/// \brief State functions structure
// --------------------------------------------------------------------------------------------------------------------
typedef struct
{
	LibFsm_StateFunc_t		RunState;	//!< Run state handler
	LibFsm_StateFunc_t		Entry;		//!< State entry handler
	LibFsm_StateFunc_t		Exit;		//!< State exit handler
} S_LibFsm_StateFunc_t;

// --------------------------------------------------------------------------------------------------------------------
/// \brief FSM descriptor structure. This structure should declare as constant.
// --------------------------------------------------------------------------------------------------------------------
typedef struct
{
#if defined(LIBFSMCFG_DEBUG) && defined(LIBFMSCFG_USE_NAME_DECODING)
	const char_t* const* const		pStateNames;	//!< pointer to the table which contains state names
	const char_t* const* const		pEventNames;	//!< pointer to the table which contains event names
#endif
	const S_LibFsm_StateTrDesc_t* 	pStatesTrDesc;	//!< pointer to the states transitions description table
	const S_LibFsm_StateFunc_t*		pStateFunc;		//!< pointer to the table which contains the state handlers
	uint8_t							StatesNum;		//!< number of states in this instance
	uint8_t							EvNum;			//!< number of events in this instance
} S_LibFsm_Consts_t;

// --------------------------------------------------------------------------------------------------------------------
/// \brief Contains all information about single instance of the state machine
// --------------------------------------------------------------------------------------------------------------------
typedef struct
{
	uint8_t 					CurrentState;	//!< current state of the FSM
	uint8_t						LastState;		//!< state of the FSM before last state transition (could be equal CurrentState)
	uint8_t 					LastEvent;		//!< last event dispatched to the FSM
	// -----------------------------------------------------------------------------------------------------------------
	/// \brief Is transition function re-entered
	/// If transition function dispatch event, order of exit, transition and entry state function callings has to be preserved
	// -----------------------------------------------------------------------------------------------------------------
	uint8_t						IsReentered;
	void*						pUserData;		//!< pointer to the user data which will be passed to the run and transition states handlers
	const S_LibFsm_Consts_t*	pConsts;		//!< pointer to the constant parameters of the state machine
#ifdef LIBFSMCFG_DEBUG
	E_LibFsm_DebugLevel_t		DebugLevel;		//!< Level of debug messages
#endif
} S_LibFsm_t;

//STRUCT_PACK_END

// --------------------------------------------------------------------------------------------------------------------
//	Imported Variables
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
//	Global Function Prototypes
// --------------------------------------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------------------------------------
/// \brief Sends event to the state machine possibly causing state transition
/// In case there is no transition defined for the event and the current state, the event will be ignored and
/// the state of the FSM will not change
/// \param ev event to trigger the state machine
/// \param pFsm pointer to the state machine instance structure
// --------------------------------------------------------------------------------------------------------------------
//lint -ecall(641, LibFsm_DispatchEvent) converting enum '' to 'int'
void LibFsm_DispatchEvent(S_LibFsm_t* const pFsm, const uint8_t ev);

// --------------------------------------------------------------------------------------------------------------------
/// \brief Returns current state of the given state machine instance
///
/// \param pFsm pointer to state machine instane structure
/// \return uint8_t current state
// --------------------------------------------------------------------------------------------------------------------
//lint -sem(LibFsm_GetCurrentState, pure)
uint8_t LibFsm_GetCurrentState(const S_LibFsm_t* const pFsm);

// --------------------------------------------------------------------------------------------------------------------
/// \brief Return the last state before last transition
/// \attention It's possible that the last state will be equal to current state in case when the last done transition
/// was done from one state to the same state (loop transition)
///
/// \param pFsm pointer to the state machine instance structure
/// \return uint8_t last state before state transition
// --------------------------------------------------------------------------------------------------------------------
//lint -sem(LibFsm_GetLastState, pure)
uint8_t LibFsm_GetLastState(const S_LibFsm_t* const pFsm);

// --------------------------------------------------------------------------------------------------------------------
/// \brief Calls the handler function corresponding to current state of the FSM
///
/// \param pFsm pointer to the state machine instance structure
// --------------------------------------------------------------------------------------------------------------------
void LibFsm_RunCurrentState(const S_LibFsm_t* const pFsm);

// --------------------------------------------------------------------------------------------------------------------
/// \brief Returns last event which has caused state transition of the state machine
/// In case there was no event dispatched to state machine this method will return out of range event
///
/// \param pFsm pointer to the state machine instance structure
/// \return uint8_t last event which caused transition
// --------------------------------------------------------------------------------------------------------------------
//lint -sem(LibFsm_GetLastEvent, pure)
uint8_t LibFsm_GetLastEvent(const S_LibFsm_t* const pFsm);

// --------------------------------------------------------------------------------------------------------------------
/// \brief The empty function.
/// Predefined empty function is used if no entry, state and exit functions are given in state definition or if
/// empty function is needed in transition.
/// It should be used instead of empty function defined per each state machine.
///
/// \param pData
/// Pointer to the state machine instance structure.
// --------------------------------------------------------------------------------------------------------------------
//lint -e{960} Function has to be inline because all FSM function declaration are static.
static inline void LibFsm_Empty(void* pData)
{
	//LIB_UNUSED(pData);
}

#ifdef LIBFSMCFG_DEBUG
// --------------------------------------------------------------------------------------------------------------------
/// \brief Set debug level of state machine.
///
/// \param pFsm
/// Pointer to the state machine instance structure.
/// \param debugLevel
/// Debug level.
// --------------------------------------------------------------------------------------------------------------------
void LibFsm_SetDebugLevel(S_LibFsm_t* const pFsm, const E_LibFsm_DebugLevel_t debugLevel);
#endif

#endif // LIB_FSM_H__INCLUDED

