
/********************************************************************************
 *
 * \file LibTypes.h
 *
 * This header file contains the definitions of the types to be used inside the
 * programs.
 *
 * Copyright (c) 2013, All Rights Reserved.
 *
 ********************************************************************************/

#ifndef LIB_TYPES_H_INCLUDED
#define LIB_TYPES_H_INCLUDED
/*******************************************************************************
	Includes File
 *******************************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
//#include "LibCanMsg.h"
#include "FreeRTOS.h"
#include "UartIF.h"
/*******************************************************************************
	Global Data Types
*******************************************************************************/
typedef enum
{
    CanChannel_1,
    CanChannel_2,
	CanChannel_All
} E_LibCan_Channel_t;

typedef uint8_t  bool_t;
// typedef unsigned char bool_t;
typedef uint16_t    Ret_t;
typedef E_LibCan_Channel_t  E_LibDrv_DevId_t;
typedef void*  pLibDrv_Hndl_t;
typedef uint32_t EventMaskType;
typedef uint8_t  StatusType;

typedef union U2BYTES
{
	uint8_t  Bytes[2];
	uint16_t HalfWord;
}u_U8_U16_t;


typedef union U4BYTES
{
	uint8_t  Bytes[4];
	uint32_t Word;
}u_U8_U32_t;


typedef union U8BYTES
{
	uint8_t  Bytes[8];
	uint64_t LongWord;
}u_U8_U64_t;


/*******************************************************************************
	Global Definitions
 *******************************************************************************/
#ifndef TRUE
#define TRUE                          (1)
#endif //TRUE

#ifndef FALSE
#define FALSE                         (0)
#endif //FALSE

#ifndef true
#define true                          (1)
#endif //true

#ifndef false
#define false                         (0)
#endif //false

/* #ifndef Enable
#define Enable                        (1)
#endif //Enable

#ifndef Disable
#define Disable                       (0)
#endif //Disable */

#ifndef NULL
#define NULL                          ((void*)0)
#endif //NULL

#ifndef UINT8_MAX
#define UINT8_MAX	                  (0xFFU)
#endif //UINT8_MAX

#ifndef UINT16_MAX
#define UINT16_MAX	                  (0xFFFFU)
#endif //UINT16_MAX

#ifndef UINT32_MAX
#define UINT32_MAX	                  (0xFFFFFFFFU)
#endif //UINT32_MAX

#ifndef UINT64_MAX
#define UINT64_MAX	                  (0xFFFFFFFFFFFFFFFFULL)
#endif //UINT64_MAX

#ifndef INT8_C
#define INT8_C(x)		              (int8_t)(x)
#endif //UINT8_C

#ifndef INT16_C
#define INT16_C(x)		              (int16_t)(x)
#endif //UINT16_C

#ifndef INT32_C
#define INT32_C(x)		              (int32_t)(x)
#endif //UINT32_C

#ifndef INT64_C
#define INT64_C(x)		              (int64_t)(x)
#endif //UINT64_C

#ifndef UINT8_C
#define UINT8_C(x)		              (uint8_t)(x ## u)
#endif //UINT8_C

#ifndef UINT16_C
#define UINT16_C(x)		              (uint16_t)(x ## u)
#endif //UINT16_C

#ifndef UINT32_C
#define UINT32_C(x)		              (uint32_t)(x ## u)
#endif //UINT32_C

#ifndef UINT64_C
#define UINT64_C(x)		              (uint64_t)(x ## u)
#endif //UINT64_C

#define STATIC_CAST(type, exp)		  ((type)(exp))

#define LIBRET_BEGIN	              (256U)		                // first failure return value for the library
#define LIBRET_OK				      ((Ret_t)0U)				    // success
#define LIBRET_FAILED			      ((Ret_t)(LIBRET_BEGIN + 1U))	// generic failure
#define LIBRET_PENDING			      ((Ret_t)(LIBRET_BEGIN + 2U))	// operation is pending
#define LIBRET_INV_CALL			      ((Ret_t)(LIBRET_BEGIN + 3U))	// function called in invalid state
#define LIBRET_INV_PARAM		      ((Ret_t)(LIBRET_BEGIN + 4U))	// function called with invalid parameters
#define LIBRET_NOT_SUPPORTED	      ((Ret_t)(LIBRET_BEGIN + 5U))	// operation not supported
#define LIBRET_BUFF_ERR			      ((Ret_t)(LIBRET_BEGIN + 6U))	// buffer error (e.g. currently not enough space left)
#define LIBRET_ADDR_NOT_ACK		      ((Ret_t)(LIBRET_BEGIN + 7U))	// address not acknowledged
#define LIBRET_DATA_NOT_ACK		      ((Ret_t)(LIBRET_BEGIN + 8U))	// data not acknowledged
#define LIBRET_LOST_ARB			      ((Ret_t)(LIBRET_BEGIN + 9U))	// arbitration has been lost
#define LIBRET_BUS_BUSY			      ((Ret_t)(LIBRET_BEGIN + 10U))	// no transfer can be performed as the bus is busy
#define LIBRET_TIMEOUT			      ((Ret_t)(LIBRET_BEGIN + 11U))	// a timeout has occurred
#define LIBRET_BUSY				      ((Ret_t)(LIBRET_BEGIN + 12U))	// device busy
#define LIBRET_CRC				      ((Ret_t)(LIBRET_BEGIN + 13U))	// CRC failure
#define LIBRET_INCONSISTENT		      ((Ret_t)(LIBRET_BEGIN + 14U))	// inconsistent data
#define LIBRET_NO_ENTRY			      ((Ret_t)(LIBRET_BEGIN + 15U))	// no such entry


/* General bit operation */
#ifndef SET_BIT_NUM
#define SET_BIT_NUM(x,y)                  ((x) = (x)|(0x1) << (y))
#endif

#ifndef GET_BIT_NUM
#define GET_BIT_NUM(x,y)                  ((uint32_t)((x) >> (y)) & 0x1)
#endif

#ifndef GET_LOWBYTE
#define GET_LOWBYTE(x)                ((uint8_t)(0x000000FF & (x)))
#endif

#ifndef GET_HIGHBYTE
#define GET_HIGHBYTE(x)               ((uint8_t)((0x0000FF00 & (x)) >> 8))
#endif

#ifndef GET_LOWWORD
#define GET_LOWWORD(x)                ((uint16_t)(0x0000FFFF & (x)))
#endif

#ifndef SET_LOWBYTE
#define SET_LOWBYTE(x)                (0xFF00 & ((uint32_t)(x)))
#endif

#ifndef MAX
#define MAX(a,b)                      (a>b)?(a):(b)
#endif

#ifndef MIN
#define MIN(a,b)                      (a<b)?(a):(b)
#endif

/* uint32 bit operation */
#define GET_U32_LOWBYTE(x)	          ((uint32_t)(0x000000FF & (x)))
#define GET_U32_SECBYTE(x)	          ((uint32_t)((0x0000FF00 & (x)) >> 8))
#define GET_U32_TRDBYTE(x)	          ((uint32_t)((0x00FF0000 & (x)) >> 16))
#define GET_U32_HIGHBYTE(x)	          ((uint32_t)((0xFF000000 & (x)) >> 24))

/*******************************************************************************
	Global variables / imported variables
 *******************************************************************************/


/*******************************************************************************
	Global Function Prototypes
 *******************************************************************************/
#define SuspendAllInterrupts()       __disable_irq()
#define ResumeAllInterrupts()        __enable_irq()
#define Exception()
#define Lib_Assert(condition)	    do { if (!(condition)) { Exception(); } } while(false)
#define LIB_UNUSED(x)               ((void)(x))

#define LibLog_Info(format, ...)       VirtualPrintf/* printf */
#define LibLog_Debug(format, ...)      VirtualPrintf/* printf */
#define LibLog_Warning(format, ...)    VirtualPrintf/* printf */
#define LibLog_Error(format, ...)      VirtualPrintf/* printf */

void STR16_BIG(uint8_t* pDesbuff, uint16_t Sourdata);
void STR32_BIG(uint8_t* pDesbuff, uint32_t Sourdata);
uint16_t LDR16_BIG(uint8_t* pSourcbuff);
uint32_t LDR32_BIG(uint8_t* pSourcbuff);

#endif //LIB_TYPES_H
