// --------------------------------------------------------------------------------------------------------------------
/// \file LibCanTpCfg.c
///
/// \brief Configuration file for the CanTp implementation
///
/// <detailed description>
///
/// Copyright (c) 2021 Neusoft.
/// All Rights Reserved.
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
//	Local Definitions to control properties inside header files
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
//	Includes
// --------------------------------------------------------------------------------------------------------------------

#include "LibCanTpCfg.h"

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

/// \brief  The flag is the message receive enabled.
// ----------------------------------------------------------------------------------------------------------------
extern bool_t LibCanTP_ReceiveEnabled;

// --------------------------------------------------------------------------------------------------------------------
//	Local Variables
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
//	Local Function Prototypes
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
//	Global Functions
// --------------------------------------------------------------------------------------------------------------------

//=====================================================================================================================
// LibCanTp_IsMsgTp:
//=====================================================================================================================
bool_t LibCanTp_IsMsgTp(uint32_t msgId)
{
	bool_t return_value = false;
    if(    (    ( msgId == LIBCANTPCFG_ECU_PHYS_ADDRESS)
	        || ( msgId == LIBCANTPCFG_ECU_FUNC_ADDRESS)
			|| ( msgId == LIBCANTPCFG_TESTER_PHYS_ADDRESS)
			)&&( LibCanTP_ReceiveEnabled == true)
	      )
    {
		return_value = true;
	}
	return return_value;
}


// --------------------------------------------------------------------------------------------------------------------
//	Functions
// --------------------------------------------------------------------------------------------------------------------