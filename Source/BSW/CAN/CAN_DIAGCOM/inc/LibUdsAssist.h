// --------------------------------------------------------------------------------------------------------------------
///
/// \file LibUdsAssist.h
///
/// \brief Helping functions for the Diagnostic services
///
///
///
/// Copyright (c) 2021 Neusoft.
/// All Rights Reserved.
///
// --------------------------------------------------------------------------------------------------------------------

 

#ifndef LIBUDSASSIST_H__INCLUDED
#define LIBUDSASSIST_H__INCLUDED

// --------------------------------------------------------------------------------------------------------------------
//	Includes
// --------------------------------------------------------------------------------------------------------------------
#include "LibTypes.h"
#include "LibService.h"

// --------------------------------------------------------------------------------------------------------------------
//	Global Definitions
// --------------------------------------------------------------------------------------------------------------------
#define LIBUDSASSIST_P2SERVER_MS		UINT32_C(50)
#define LIBUDSASSIST_P2EX_SERVER_MS		UINT32_C(5000)
#define LIBUDSASSIST_P4SERVER_MS		UINT32_C(90000)

// --------------------------------------------------------------------------------------------------------------------
//	Global Data Types
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
//	Imported Variables
// --------------------------------------------------------------------------------------------------------------------
extern S_LibService_Inst_t LibUdsAssist_Service;

// --------------------------------------------------------------------------------------------------------------------
//	Global Function Prototypes
// --------------------------------------------------------------------------------------------------------------------

extern void LibUdsAssist_StartRespPending(void);
extern void LibUdsAssist_StopRespPending(void);

#endif // LIBUDSASSIST_H__INCLUDED

