// --------------------------------------------------------------------------------------------------------------------
///
/// \file LibDiagComInt.h
///
/// \brief Interface for the Diagnostic / UDS Layer
///
///
///
/// Copyright (c) 2021 Neusoft.
/// All Rights Reserved.
///
// --------------------------------------------------------------------------------------------------------------------

 

#ifndef LIBDIAGCOMINT_H__INCLUDED
#define LIBDIAGCOMINT_H__INCLUDED

// --------------------------------------------------------------------------------------------------------------------
//	Includes
// --------------------------------------------------------------------------------------------------------------------

#include "LibTypes.h"
#include "LibDiagCom.h"

// --------------------------------------------------------------------------------------------------------------------
//	Global Definitions
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
//	Global Data Types
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
//	Imported Variables
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
//	Global Function Prototypes
// --------------------------------------------------------------------------------------------------------------------

extern void LibDiagCom_MsgSendInt(S_LibDiagCom_Msg_t* pMsg);
extern void LibDiagCom_AbortRequest(void);

#endif // LIBDIAGCOMINT_H__INCLUDED

