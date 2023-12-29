// --------------------------------------------------------------------------------------------------------------------
///
/// \file LibFsmInternals.h
///
/// \brief Internal definitions / macros needed for generation of the Finite State Machine
/// \attention DO NOT USE directly definitions and macros from this file. Instead please refer to LibFsm.h
///
///
///
/// All Rights Reserved.
///
// --------------------------------------------------------------------------------------------------------------------


#ifndef LIB_FSM_INTERNALS_H__INCLUDED
#define LIB_FSM_INTERNALS_H__INCLUDED

// --------------------------------------------------------------------------------------------------------------------
//	Includes
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
//	Global Definitions
// --------------------------------------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------------------------------------
/// \brief Helper macro, return macro name from number of used arguments
///
/// \param _1, _2, _3, _4
/// Dummy parameters, There has to be as much of them as the macro with highest number of parameters has the parameters.
/// \param name
/// According to the passed number of arguments, will pick correct macro name.
// --------------------------------------------------------------------------------------------------------------------
#define LIBFSM_GET_MACRO_FROM_ARGS_NUM(_1,_2,_3,_4, name, ...) name

// --------------------------------------------------------------------------------------------------------------------
/// \brief Provide dummy macro name for macro with unsupported arguments number.
// --------------------------------------------------------------------------------------------------------------------
#define LIBFSM_DUMMY_NAME

// --------------------------------------------------------------------------------------------------------------------
//	Global Data Types
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
//	Imported Variables
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
//	Global Function Prototypes
// --------------------------------------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------------------------------------
/// \brief Macro LIBFSM_EXPAND_EVENTS
/// Used to generate enum type for FSM events - see LIBFSM_DECLARE_EVENTS_ENUM below
///
/// \param event name of the event enumerator
// --------------------------------------------------------------------------------------------------------------------
#define LIBFSM_EXPAND_EVENTS(event) event,

// --------------------------------------------------------------------------------------------------------------------
/// \brief Macro LIBFSM_DECLARE_EVENTS_ENUM
/// Define enumeration type for the FSM events out of the X-Macro list which shall be passed as entries parameter to
/// this macro.
/// The defined enumeration type has always following name E_xxx_Event_t where xxx if the name (prefix) of the FSM
/// instance.
/// It's expected that as entries parameter following define will be given
/// \code
/// #define TSTFSM_EVENTS(DEFINE_EVENT) \<EOL>
///		DEFINE_EVENT(TST_EV_INIT)\<EOL>
///		DEFINE_EVENT(TST_EV_ON)\<EOL>
///		DEFINE_EVENT(TST_EV_OFF)\<EOL>
///		DEFINE_EVENT(TST_EV_ERR)
///
/// \endcode
/// The TSTFSM_EVENTS can be replaced by any other module specific name.
///
/// \param NAME
/// Implementation / module specific prefix for the FSM
/// \param X-Macro like list of the events declarations
// --------------------------------------------------------------------------------------------------------------------
#define LIBFSM_DECLARE_EVENTS_ENUM(NAME,entries)\
typedef enum\
{ \
	entries(LIBFSM_EXPAND_EVENTS)\
	NAME##_EV_NUM\
} E_##NAME##_Event_t;

// --------------------------------------------------------------------------------------------------------------------
/// \brief Macro LIBFSM_EXPAND_STATES_DEC
/// Used to generates state's enumerations for the FSM from the states declarations list (see LIBFSM_DECLARE_STATES_ENUM)
/// macro
/// \param state name of the state enumerator
// --------------------------------------------------------------------------------------------------------------------
#define LIBFSM_EXPAND_STATES_DEC(state, ...) state,

// --------------------------------------------------------------------------------------------------------------------
/// \brief Macro to expand states declarations into list of corresponding functions to be called by state run.
///
/// \param state name of the state enumerator
/// \param func name of the state function called by FSM during call to xxx_RunCurrentState()
// --------------------------------------------------------------------------------------------------------------------
#define LIBFSM_EXPAND_STATES_FUNC(state, func, ...) func

// --------------------------------------------------------------------------------------------------------------------
/// \brief Macro to generate declarations of the state handlers functions out of list with the states declarations
///
/// \param state name of the state enumerator
/// \param func name of the state function called by FSM during call to xxx_RunCurrentState()
// --------------------------------------------------------------------------------------------------------------------
#define LIBFSM_EXPAND_STATE_TO_FUNC_DEC(state, func, ...) /*lint -e{762}*/ static void func(void* pData);

// --------------------------------------------------------------------------------------------------------------------
/// \brief Macro to expand states declarations into list of corresponding functions to be called on entering the state.
///
/// \note Macro can be called with 2 or 4 arguments. If it is called with 2 entry and exit function will be set to 
/// LibFsm_Empty. In case of 4 arguments both entry and exit functions have to be defined. LibFsm_Empty has to be used
/// for non-implemented functions.
///
/// \param state name of the state enumerator
/// \param func name of the state function called by FSM during call to xxx_RunCurrentState()
/// \param entryFunc name of the state entry function, will be called on entering the state.
/// \param exitFunc name of the state exit function, will be called on exiting the state.
// --------------------------------------------------------------------------------------------------------------------
#define LIBFSM_EXPAND_STATE_TO_ENTRY_FUNC(...) LIBFSM_GET_MACRO_FROM_ARGS_NUM(__VA_ARGS__,\
	LIBFSM_EXPAND_STATE_TO_ENTRY_FUNC4,\
	LIBFSM_DUMMY_NAME,\
	LIBFSM_EXPAND_STATE_TO_ENTRY_FUNC2)(__VA_ARGS__)

// --------------------------------------------------------------------------------------------------------------------
/// \brief Macro to expand states declarations into list of corresponding functions to be called on entering the state.
///
/// \param state name of the state enumerator
/// \param func name of the state function called by FSM during call to xxx_RunCurrentState()
/// \param entryFunc name of the state entry function, will be called on entering the state.
/// \param exitFunc name of the state exit function, will be called on exiting the state.
// --------------------------------------------------------------------------------------------------------------------
#define LIBFSM_EXPAND_STATE_TO_ENTRY_FUNC4(state, func, entryFunc, exitFunc) entryFunc

// --------------------------------------------------------------------------------------------------------------------
/// \brief Macro to expand states declarations into list of corresponding functions to be called on entering the state.
/// Set entry function to the LibFsm_Empty, as there is no entryFunc parameter passed.
///
/// \param state name of the state enumerator
/// \param func name of the state function called by FSM during call to xxx_RunCurrentState()
// --------------------------------------------------------------------------------------------------------------------
#define LIBFSM_EXPAND_STATE_TO_ENTRY_FUNC2(state, func) LibFsm_Empty

// --------------------------------------------------------------------------------------------------------------------
/// \brief Macro to generate declarations of the entry state functions out of list with the states declarations.
///
/// \note Macro can be called with 2 or 4 arguments. In case of 4 arguments both entry and exit functions have to be
/// defined. LibFsm_Empty has to be used for non-implemented functions.
///
/// \param state name of the state enumerator
/// \param func name of the state function called by FSM during call to xxx_RunCurrentState()
/// \param entryFunc name of the state entry function, will be called on entering the state.
/// \param exitFunc name of the state exit function, will be called on exiting the state.
// --------------------------------------------------------------------------------------------------------------------
#define LIBFSM_EXPAND_STATE_TO_ENTRY_FUNC_DEC(...) LIBFSM_GET_MACRO_FROM_ARGS_NUM(__VA_ARGS__,\
	LIBFSM_EXPAND_STATE_TO_ENTRY_FUNC_DEC4,\
	LIBFSM_DUMMY_NAME,\
	LIBFSM_EXPAND_STATE_TO_ENTRY_FUNC_DEC2)(__VA_ARGS__)

// --------------------------------------------------------------------------------------------------------------------
/// \brief Macro to generate declarations of the entry state functions out of list with the states declarations.
///
/// \param state name of the state enumerator
/// \param func name of the state function called by FSM during call to xxx_RunCurrentState()
/// \param entryFunc name of the state entry function, will be called on entering the state.
/// \param exitFunc name of the state exit function, will be called on exiting the state.
// --------------------------------------------------------------------------------------------------------------------
#define LIBFSM_EXPAND_STATE_TO_ENTRY_FUNC_DEC4(state, func, entryFunc, exitFunc)\
	/*lint -e{762}*/ static void entryFunc(void* pData);

// --------------------------------------------------------------------------------------------------------------------
/// \brief Macro to generate declarations of the entry state functions out of list with the states declarations.
/// Declare nothing, while no entryFunc parameter is passed.
///
/// \param state name of the state enumerator
/// \param func name of the state function called by FSM during call to xxx_RunCurrentState()
// --------------------------------------------------------------------------------------------------------------------
#define LIBFSM_EXPAND_STATE_TO_ENTRY_FUNC_DEC2(state, func)

// --------------------------------------------------------------------------------------------------------------------
/// \brief Macro to expand states declarations into list of corresponding functions to be called on exiting the state.
///
/// \note Macro can be called with 2 or 4 arguments. If it is called with 2 entry and exit function will be set to
/// LibFsm_Empty. In case of 4 arguments both entry and exit functions have to be defined. LibFsm_Empty has to be used
/// for non-implemented functions.
///
/// \param state name of the state enumerator
/// \param func name of the state function called by FSM during call to xxx_RunCurrentState()
/// \param entryFunc name of the state entry function, will be called on entering the state.
/// \param exitFunc name of the state exit function, will be called on exiting the state.
// --------------------------------------------------------------------------------------------------------------------
#define LIBFSM_EXPAND_STATE_TO_EXIT_FUNC(...) LIBFSM_GET_MACRO_FROM_ARGS_NUM(__VA_ARGS__,\
	LIBFSM_EXPAND_STATE_TO_EXIT_FUNC4,\
	LIBFSM_DUMMY_NAME,\
	LIBFSM_EXPAND_STATE_TO_EXIT_FUNC2)(__VA_ARGS__)

// --------------------------------------------------------------------------------------------------------------------
/// \brief Macro to expand states declarations into list of corresponding functions to be called on exiting the state.
///
/// \param state name of the state enumerator
/// \param func name of the state function called by FSM during call to xxx_RunCurrentState()
/// \param entryFunc name of the state entry function, will be called on entering the state.
/// \param exitFunc name of the state exit function, will be called on exiting the state.
// --------------------------------------------------------------------------------------------------------------------
#define LIBFSM_EXPAND_STATE_TO_EXIT_FUNC4(state, func, entryFunc, exitFunc) exitFunc

// --------------------------------------------------------------------------------------------------------------------
/// \brief Macro to expand states declarations into list of corresponding functions to be called on exiting the state.
/// Set exit function to the LibFsm_Empty, as there is no exitFunc parameter passed.
///
/// \param state name of the state enumerator
/// \param func name of the state function called by FSM during call to xxx_RunCurrentState()
// --------------------------------------------------------------------------------------------------------------------
#define LIBFSM_EXPAND_STATE_TO_EXIT_FUNC2(state, func) LibFsm_Empty

// --------------------------------------------------------------------------------------------------------------------
/// \brief Macro to generate declarations of the exit state functions out of list with the states declarations.
///
/// \note Macro can be called with 2 or 4 arguments. In case of 4 arguments both entry and exit functions have to be
/// defined. LibFsm_Empty has to be used for non-implemented functions.
///
/// \param state name of the state enumerator
/// \param func name of the state function called by FSM during call to xxx_RunCurrentState()
/// \param entryFunc name of the state entry function, will be called on entering the state.
/// \param exitFunc name of the state exit function, will be called on exiting the state.
// --------------------------------------------------------------------------------------------------------------------
#define LIBFSM_EXPAND_STATE_TO_EXIT_FUNC_DEC(...) LIBFSM_GET_MACRO_FROM_ARGS_NUM(__VA_ARGS__,\
	LIBFSM_EXPAND_STATE_TO_EXIT_FUNC_DEC4,\
	LIBFSM_DUMMY_NAME,\
	LIBFSM_EXPAND_STATE_TO_EXIT_FUNC_DEC2)(__VA_ARGS__)

// --------------------------------------------------------------------------------------------------------------------
/// \brief Macro to generate declarations of the exit state functions out of list with the states declarations.
///
/// \param state name of the state enumerator
/// \param func name of the state function called by FSM during call to xxx_RunCurrentState()
/// \param entryFunc name of the state entry function, will be called on entering the state.
/// \param exitFunc name of the state exit function, will be called on exiting the state.
// --------------------------------------------------------------------------------------------------------------------
#define LIBFSM_EXPAND_STATE_TO_EXIT_FUNC_DEC4(state, func, entryFunc, exitFunc)\
	/*lint -e{762}*/ static void exitFunc(void* pData);

// --------------------------------------------------------------------------------------------------------------------
/// \brief Macro to generate declarations of the exit state functions out of list with the states declarations.
/// Declare nothing, while no exitFunc parameter is passed.
///
/// \param state name of the state enumerator
/// \param func name of the state function called by FSM during call to xxx_RunCurrentState()
// --------------------------------------------------------------------------------------------------------------------
#define LIBFSM_EXPAND_STATE_TO_EXIT_FUNC_DEC2(state, func)

// --------------------------------------------------------------------------------------------------------------------
/// \brief Macro to generate declarations of the state's transitions functions out of the transitions definitions
/// list for the FSM
///
/// \param event event which causes this transition
/// \param state new state after this transition
/// \param trFunc name of the function called during this state transition
// --------------------------------------------------------------------------------------------------------------------
#define LIBFSM_EXPAND_TRDESC_TO_TRDEC(event,state,trFunc) \
		/*lint -save -e762*/\
		static void trFunc(void* pData);\
		/*lint -restore*/

// --------------------------------------------------------------------------------------------------------------------
/// \brief  Macro to generate entries of the state's transitions tables within FSM
///
/// \param event event which causes this transition
/// \param state new state after this transition
/// \param trFunc name of the function called during this state transition
// --------------------------------------------------------------------------------------------------------------------
#define LIBFSM_EXPAND_TRDESC_ENTRY(event,state,trFunc)\
	{(uint8_t)event, (uint8_t)state, trFunc},

// --------------------------------------------------------------------------------------------------------------------
/// \brief  X-Macro used during generation of the transition tables out of the state definition lists.
/// This macro assumes that for each state defined for the FSM corresponding definition of the state transition
/// list is present and it has fixed name: TRANSITIONS_FOR_STATE_xxx where xxx is equal to the state name enumerator.
/// Example:
/// \code
/// // states definitions
/// #define MYSTATES_DEFS(DEF_STATE)\<EOL>
///			DEF_STATE(MY_STATE_1,MyState1RunFunction)\ <--- state enumerator MY_STATE_1
/// 		DEF_STATE(MY_STATE_2,MyState2RunFunction)\ <--- state enumerator MY_STATE_2
/// // transitions for MY_STATE_1
/// #define TRANSITION_FOR_STATE_MY_STATE1(DEF_TR)\ <--- The name is fixed
///			DEF_TR(EV_ON, MY_STATE2, GoFromState1ToState2)
/// \endcode
/// Out of such list the transition hander functions declarations and the transition tables are generated
/// \param state enumerator of the state
// --------------------------------------------------------------------------------------------------------------------
#define LIBFSM_DECLARE_TRDESC_TABS(state,...) \
TRANSITIONS_FOR_STATE_##state(LIBFSM_EXPAND_TRDESC_TO_TRDEC)\
static const S_LibFsm_TrDesc_t state##_TrDescTab[]={TRANSITIONS_FOR_STATE_##state(LIBFSM_EXPAND_TRDESC_ENTRY)};

// --------------------------------------------------------------------------------------------------------------------
/// \brief  Macro used to generate initialization content of the S_xxx_Stt[] table of the FSM
/// The S_xxx_Stt[] table describes all transitions for all states within generated FSM end contains for each
/// state number of transitions entries and pointer to the list of the transitions.
///
/// \param state enumerator of the state
/// \sa S_LibFsm_TrDesc_t
// --------------------------------------------------------------------------------------------------------------------
#define LIBFSM_EXPAND_TO_STT_INIT(state,...) {(uint8_t)(sizeof(state##_TrDescTab)/sizeof(S_LibFsm_TrDesc_t)),state##_TrDescTab},


#endif // LIB_FSM_INTERNALS_H__INCLUDED

