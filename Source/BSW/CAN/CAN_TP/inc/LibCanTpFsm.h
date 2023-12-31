// --------------------------------------------------------------------------------------------------------------------
///
/// \file LibCanTpFsm.h
///
/// \brief <short descritpion>
/// <detailed description>
///
///
/// Copyright (c) 2021 Neusoft.
/// All Rights Reserved.
///
// --------------------------------------------------------------------------------------------------------------------

 

#ifndef LIBCANTPFSM_H__INCLUDED
#define LIBCANTPFSM_H__INCLUDED

// --------------------------------------------------------------------------------------------------------------------
//	Includes
// --------------------------------------------------------------------------------------------------------------------

#include "LibCanTp.h"
#include "LibCanTpInternal.h"

#include "LibFsm.h"

// --------------------------------------------------------------------------------------------------------------------
//	Global Definitions
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
//	Global Data Types
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
//	Imported Variables
// --------------------------------------------------------------------------------------------------------------------

extern S_LibFsm_t LibCanTpInt_Fsm;

// --------------------------------------------------------------------------------------------------------------------
//	Global Function Prototypes
// --------------------------------------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------------------------------------
/// \brief Trigger INIT event
/// 
/// \param pInst 
/// Instance for which the INIT event should be triggered
// --------------------------------------------------------------------------------------------------------------------
extern void LibCanTpFsm_TriggerInit(S_LibCanTp_Inst_t* pInst);

// --------------------------------------------------------------------------------------------------------------------
/// \brief 
/// 
/// \param pInst 
// --------------------------------------------------------------------------------------------------------------------
extern void LibCanTpFsm_TriggerRecv(S_LibCanTp_Inst_t* pInst);

// --------------------------------------------------------------------------------------------------------------------
/// \brief 
/// 
/// \param pInst 
// --------------------------------------------------------------------------------------------------------------------
extern void LibCanTpFsm_TriggerSend(S_LibCanTp_Inst_t* pInst);

// --------------------------------------------------------------------------------------------------------------------
/// \brief 
/// 
/// \param pInst 
// --------------------------------------------------------------------------------------------------------------------
extern void LibCanTpFsm_TriggerFinish(S_LibCanTp_Inst_t* pInst);

// --------------------------------------------------------------------------------------------------------------------
/// \brief 
/// 
/// \param pInst 
// --------------------------------------------------------------------------------------------------------------------
extern bool_t LibCanTpFsm_IsReady(S_LibCanTp_Inst_t* pInst);

// --------------------------------------------------------------------------------------------------------------------
/// \brief 
/// 
/// \param pInst 
// --------------------------------------------------------------------------------------------------------------------
extern bool_t LibCanTpFsm_IsSend(S_LibCanTp_Inst_t* pInst);

// --------------------------------------------------------------------------------------------------------------------
/// \brief 
/// 
/// \param pInst 
// --------------------------------------------------------------------------------------------------------------------
extern bool_t LibCanTpFsm_IsRecv(S_LibCanTp_Inst_t* pInst);

#endif // LIBCANTPFSM_H__INCLUDED

