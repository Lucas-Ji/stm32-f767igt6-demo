// --------------------------------------------------------------------------------------------------------------------
///
/// \file LibUds.h
///
/// \brief UDS layer (ISO 14229-1)
///
/// Copyright (c) 2021 Neusoft.
/// All Rights Reserved.
///
// --------------------------------------------------------------------------------------------------------------------


#ifndef LIB_UDS_H__INCLUDED
#define LIB_UDS_H__INCLUDED

// --------------------------------------------------------------------------------------------------------------------
//	Includes
// --------------------------------------------------------------------------------------------------------------------
#include "LibTypes.h"
#include "LibUdsCfg.h"
#include "DiagTask.h"
#include "LibService.h"
#include <stdint.h>

// --------------------------------------------------------------------------------------------------------------------
//	Global Definitions
// --------------------------------------------------------------------------------------------------------------------
/* type conversion and function linkage macros for C/C++ */
#if defined(__cplusplus)
	#define EXTERN_C					extern "C"
	#define STATIC_CAST(type, exp)		(static_cast<type>(exp))
	#define DYNAMIC_CAST(type, exp)		(dynamic_cast<type>(exp))
	#define REINTERPRET_CAST(type, exp)	(reinterpret_cast<type>(exp))
	#define CONST_CAST(type, exp)		(const_cast<type>(exp))
#else
	#define EXTERN_C
	#define STATIC_CAST(type, exp)		((type)(exp))
	#define DYNAMIC_CAST(type, exp)		((type)(exp))
	#define REINTERPRET_CAST(type, exp)	((type)(exp))
	#define CONST_CAST(type, exp)		((type)(exp))
#endif

// --------------------------------------------------------------------------------------------------------------------
/// \brief Type for NotifySessionTransition functions.
// --------------------------------------------------------------------------------------------------------------------
typedef void (*LibUds_NotifySessionTransType) (E_LibUds_Session_t oldSession, E_LibUds_Session_t newSession);

typedef struct
{
    uint32_t StartAddr;         /*data start address*/
    uint32_t DataLen;           /*data len*/
    uint32_t gs_RxBlockNum;
} tDowloadDataInfo;
// --------------------------------------------------------------------------------------------------------------------
/// \brief Suppress positive response bit
// --------------------------------------------------------------------------------------------------------------------
#define	LIBUDS_SPR_BIT							(0x80)

// --------------------------------------------------------------------------------------------------------------------
/// \brief Index of subfunction in request
// --------------------------------------------------------------------------------------------------------------------
#define LIBUDS_ADDR_SUBFUNCTION					(0x01)

// --------------------------------------------------------------------------------------------------------------------
/// \brief Invalid index
///
/// \details Valid index: index 0x00000000 - 0xFFFFFFFE
// --------------------------------------------------------------------------------------------------------------------
#define LIBUDS_INVALID_INDEX					(0xFFFFFFFF)

// --------------------------------------------------------------------------------------------------------------------
/// \brief Start address of DID in payload.
///
/// \details Used in services ReadDataByIdentifier, WriteDataByIdentifier.
// --------------------------------------------------------------------------------------------------------------------
#define LIBUDS_DID_START_ADDR					(1)

// --------------------------------------------------------------------------------------------------------------------
/// \brief Start address of RID in payload.
///
/// \details Used in service RoutineControl.
// --------------------------------------------------------------------------------------------------------------------
#define LIBUDS_RID_START_ADDR					(2)

// --------------------------------------------------------------------------------------------------------------------
/// \brief Length of RoutineControl data record.
///
/// \details Data record = subfunction + RID.
// --------------------------------------------------------------------------------------------------------------------
#define LIBUDS_ROUTCTL_DATA_RECORD_LENGTH		(3)

// --------------------------------------------------------------------------------------------------------------------
/// \brief Start address of routine control subfunction in payload.
///
/// \details Used in service RoutineControl.
// --------------------------------------------------------------------------------------------------------------------
#define LIBUDS_ROUTCTL_SUBFUNC_START_ADDR		(1)

// --------------------------------------------------------------------------------------------------------------------
/// \brief Start address of routine control subfunction in RoutineControl service identification data record.
/// (= 1 byte subfunction (routine control type) and 2 bytes RID)
///
/// \details Used in service RoutineControl.
// --------------------------------------------------------------------------------------------------------------------
#define LIBUDS_ROUTCTL_SUBFUNC_START_ADDR_IN		(1)

// --------------------------------------------------------------------------------------------------------------------
/// \brief RoutineInfo parameter, used by responses for RoutineControl
// --------------------------------------------------------------------------------------------------------------------
#define LIBUDS_ROUTCTL_ROUTINE_SUCCESSFULLY_COMPLETED	(0)
#define LIBUDS_ROUTCTL_ROUTINE_IN_PROGRESS				(1)
#define LIBUDS_ROUTCTL_ROUTINE_STOPPED_WITHOUT_RESULTS	(2)

// --------------------------------------------------------------------------------------------------------------------
/// \brief Start address of data record of UDS service WriteDataByIdentifier in payload.
// --------------------------------------------------------------------------------------------------------------------
#define LIBUDS_WRITE_DATARECORD_START_ADDR		UINT8_C(3)

// --------------------------------------------------------------------------------------------------------------------
/// \brief Negative response codes
// --------------------------------------------------------------------------------------------------------------------
#define LIBUDS_PRC_DATA_AVAILABLE_SERVICE_FINISHED					(0x00)
#define LIBUDS_NRC_GENERAL_REJECT									(0x10)
#define LIBUDS_NRC_SERVICE_NOT_SUPPORTED							(0x11)
#define LIBUDS_NRC_SUB_FUNCTION_NOT_SUPPORTED						(0x12)
#define LIBUDS_NRC_INCORRECT_MESSAGE_LENGTH_OR_INVALID_FORMAT		(0x13)
#define LIBUDS_NRC_RESPONSE_TOO_LONG								(0x14)
#define LIBUDS_NRC_NRC_BUSY_REPEAT_REQUEST							(0x21)
#define LIBUDS_NRC_CONDITIONS_NOT_CORRECT							(0x22)
#define LIBUDS_NRC_REQUEST_SEQUENCE_ERROR							(0x24)
#define LIBUDS_NRC_NO_RESPONSE_FROM_SUBNET_COMPONENT				(0x25)
#define LIBUDS_NRC_FAILURE_PREVENTS_EXECUTION_OF_REQUESTED_ACTION	(0x26)
#define LIBUDS_NRC_REQUEST_OUT_OF_RANGE								(0x31)
#define LIBUDS_NRC_SECURITY_ACCESS_DENIED							(0x33)
#define LIBUDS_NRC_INVALID_KEY										(0x35)
#define LIBUDS_NRC_EXCEED_NUMBER_OF_ATTEMPTS						(0x36)
#define LIBUDS_NRC_REQUIRED_TIME_DELAY_NOT_EXPIRED					(0x37)
#define LIBUDS_NRC_UPLOAD_DOWNLOAD_NOT_ACCEPTED						(0x70)
#define LIBUDS_NRC_TRANSFER_DATA_SUSPENDED							(0x71)
#define LIBUDS_NRC_GENERAL_PROGRAMMING_FAILURE						(0x72)
#define LIBUDS_NRC_WRONG_BLOCK_SEQUENCE_COUNTER						(0x73)
//#define LIBUDS_NRC_RESPONSE_PENDING					(0x78)	=> defined in LibUdsIFace.h
#define LIBUDS_NRC_SUB_FUNCTION_NOT_SUPPORTED_IN_ACTIVE_SESSION		(0x7E)
#define LIBUDS_NRC_SERVICE_NOT_SUPPORTED_IN_ACTIVE_SESSION			(0x7F)
#define LIBUDS_NRC_RPM_TOO_HIGH										(0x81)
#define LIBUDS_NRC_RPM_TOO_LOW										(0x82)
#define LIBUDS_NRC_ENGINE_IS_RUNNING								(0x83)
#define LIBUDS_NRC_ENGINE_IS_NOT_RUNNING							(0x84)
#define LIBUDS_NRC_ENGINE_RUN_TIME_TOO_LOW							(0x85)
#define LIBUDS_NRC_TEMPERATURE_TOO_HIGH								(0x86)
#define LIBUDS_NRC_TEMPERATURE_TOO_LOW								(0x87)
#define LIBUDS_NRC_VEHICLE_SPEED_TOO_HIGH							(0x88)
#define LIBUDS_NRC_VEHICLE_SPEED_TOO_LOW							(0x89)
#define LIBUDS_NRC_THROTTLE_SPEED_TOO_HIGH							(0x8A)
#define LIBUDS_NRC_THROTTLE_SPEED_TOO_LOW							(0x8B)
#define LIBUDS_NRC_TRANSMISSION_RANGE_NOT_IN_NEUTRAL				(0x8C)
#define LIBUDS_NRC_TRANSMISSION_RANGE_NOT_IN_GEARS					(0x8D)
#define LIBUDS_NRC_TBRAKE_SWITCH_NOT_CLOSED							(0x8F)
#define LIBUDS_NRC_SHIFTER_LEVER_NOT_IN_PARK						(0x90)
#define LIBUDS_NRC_TORQUE_CONVERTER_CLUTCH_LOCKED					(0x91)
#define LIBUDS_NRC_VOLTAGE_TOO_HIGH									(0x92)
#define LIBUDS_NRC_VOLTAGE_TOO_LOW									(0x93)


#define FactoryModeSession 			(0x01)
#define DefaultModeSession			(0x00)
// --------------------------------------------------------------------------------------------------------------------
///	\brief True if mask contains flag to respond on a functional request; false else.
// --------------------------------------------------------------------------------------------------------------------
#define LibUds_RespOnFuncRequest(mask)	\
		((STATIC_CAST(uint32_t, LIBUDS_DIAG_RESP_FUNC_MASK)	\
		== (STATIC_CAST(uint32_t, LIBUDS_DIAG_RESP_FUNC_MASK) & mask)))

// --------------------------------------------------------------------------------------------------------------------
///	\brief True if mask contains flag to respond on a physical request; false else.
// --------------------------------------------------------------------------------------------------------------------
#define LibUds_RespOnPhysRequest(mask)	\
		((STATIC_CAST(uint32_t, LIBUDS_DIAG_RESP_PHYS_MASK)	\
		== (STATIC_CAST(uint32_t, LIBUDS_DIAG_RESP_PHYS_MASK) & mask)))

// --------------------------------------------------------------------------------------------------------------------
///	\brief Returns the active session as a value.
// --------------------------------------------------------------------------------------------------------------------
#define LibUds_GetSessionValue()	(LibUds_GetSessionAsValue(LibUds_GetSession()))

// --------------------------------------------------------------------------------------------------------------------
///	\brief Returns the active session as a mask.
// --------------------------------------------------------------------------------------------------------------------
#define LibUds_GetSessionMask()	(LibUds_GetSessionAsMask(LibUds_GetSession()))

// --------------------------------------------------------------------------------------------------------------------
///	\brief Get subservice identifier with length 2, starting at startAddr in buffer buf.
// --------------------------------------------------------------------------------------------------------------------
#define LibUds_GetSubsvc2Bytes(buf, startAddr)	((buf[startAddr] << 8) | buf[startAddr+1U])


// --------------------------------------------------------------------------------------------------------------------
///	\brief Get subservice identifier with length 3, starting at startAddr in buffer buf.
// --------------------------------------------------------------------------------------------------------------------
#define LibUds_GetSubsvc3Bytes(buf, startAddr)	\
							((STATIC_CAST(uint32_t, buf[startAddr]) << 16U)\
								| (STATIC_CAST(uint32_t, buf[startAddr+1U]) << 8U)\
								| STATIC_CAST(uint32_t, buf[startAddr+2U]))


// --------------------------------------------------------------------------------------------------------------------
///	\brief Get subservice identifier with length 4, starting at startAddr in buffer buf.
// --------------------------------------------------------------------------------------------------------------------
#define LibUds_GetSubsvc4Bytes(buf, startAddr)	\
					((STATIC_CAST(uint32_t, buf[startAddr]) << 24U)\
					  | (STATIC_CAST(uint32_t, buf[startAddr+1U]) << 16U)\
					  | (STATIC_CAST(uint32_t, buf[startAddr+2U]) << 8U)\
					  |  STATIC_CAST(uint32_t, buf[startAddr+3U]))
// --------------------------------------------------------------------------------------------------------------------
///	\brief Returns subfunc without LIBUDS_SPR_BIT if this is supported.
// --------------------------------------------------------------------------------------------------------------------
#define LibUds_GetSubfuncCheckSprBit(subfunc,supp) 	((supp) ? \
		((subfunc) & (~ ((uint32_t)LIBUDS_SPR_BIT))) : (subfunc))


// --------------------------------------------------------------------------------------------------------------------
//	Global Data Types
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
//	Imported Variables
// --------------------------------------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------------------------------------
///	\brief Service instance
// --------------------------------------------------------------------------------------------------------------------
extern S_LibService_Inst_t			LibUds_Service;



// --------------------------------------------------------------------------------------------------------------------
//	Global Function Prototypes
// --------------------------------------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------------------------------------
///	\brief Service function of the UDS module: it is running inside DIAG TASK
///
///	\param pData
/// User data; currently not used
// --------------------------------------------------------------------------------------------------------------------
void LibUds_ServiceFunc(void* pData);

// --------------------------------------------------------------------------------------------------------------------
/// \brief Initialization of the UDS layer.
///
/// \details Sets the initial session and security state and resets the message container LibUds_Msg.
// --------------------------------------------------------------------------------------------------------------------
void LibUds_Init(void);

// --------------------------------------------------------------------------------------------------------------------
/// \brief Starts the S3 timer.
// --------------------------------------------------------------------------------------------------------------------
void LibUds_StartS3Timer(void);

// --------------------------------------------------------------------------------------------------------------------
/// \brief Stops the S3 timer.
// --------------------------------------------------------------------------------------------------------------------
void LibUds_StopS3Timer(void);

// --------------------------------------------------------------------------------------------------------------------
/// \brief Returns the pointer to the current service.
///
/// \details Called by UDS services, should be not NULL.
///
/// \return Pointer to the current service.
// --------------------------------------------------------------------------------------------------------------------
const S_LibUds_SvcCfg_t* LibUds_GetSvcCfg(void);

// --------------------------------------------------------------------------------------------------------------------
/// \brief Returns the pointer to the configuration of the current subservice.
///
/// \details Called by UDS services, should be not NULL.
///
/// \return Pointer to the current subservice or NULL.
// --------------------------------------------------------------------------------------------------------------------
const S_LibUds_SubsvcCfg_t* LibUds_GetSubSvcCfg(void);

// --------------------------------------------------------------------------------------------------------------------
/// \brief Returns the index of the service with SID in the service database.
///
/// \param sid
/// The service identifier
///
/// \retval Index of the service in the service database if service is in database.
/// \retval LIBUDS_SERVICES_COUNT if service is not in database.
// --------------------------------------------------------------------------------------------------------------------
uint32_t LibUds_GetSvcIndex(uint8_t sid);

// --------------------------------------------------------------------------------------------------------------------
/// \brief Returns the pointer to the message container LibUds_Msg.
///
/// \details All pointers are checked here.
/// Must only be called during UDS service processing, i.e. then pIFace must NOT be empty.
///
/// \return Pointer to the message container LibUds_Msg
// --------------------------------------------------------------------------------------------------------------------
S_LibUds_Msg_t* LibUds_GetMsg(void);

// --------------------------------------------------------------------------------------------------------------------
/// \brief Writes a negative response onto the TX buffer.
///
/// \param nrc
/// The negative response code
// --------------------------------------------------------------------------------------------------------------------
void LibUds_WriteNegativeResponse(const uint8_t nrc);

// --------------------------------------------------------------------------------------------------------------------
/// \brief Writes a positive response onto the TX buffer.
///
/// \details Writes first the SID + 0x40, then the buffer pBuf.
/// Sets interface struct and SvcResult in LibUds_Msg.
/// Use this function only with small buffers.
///
/// \param pBuf
/// Pointer to a buffer with the response data without first byte (= SID + 0x40).
/// \param len
/// Length of response data.
///
/// \retval LIBRET_OK
/// The positive response could be written.
/// \retval LIBRET_FAILED
/// The response was too long and a negative response with NRC LIBUDS_NRC_RESPONSE_TOO_LONG was written.
// --------------------------------------------------------------------------------------------------------------------
Ret_t LibUds_WritePositiveResponse(const uint8_t* pBuf, uint16_t len);

// --------------------------------------------------------------------------------------------------------------------
/// \brief Returns pointer to the subservice configuration data.
///
/// \param subservice
/// Requested subservice identifier
///
/// \retval Pointer to the subservice
/// Subservice is valid.
/// \retval NULL
/// SubService is not valid.
// --------------------------------------------------------------------------------------------------------------------
const S_LibUds_SubsvcCfg_t* LibUds_FindSubsvcCfg(const uint32_t subservice);

// --------------------------------------------------------------------------------------------------------------------
/// \brief Returns the current active session as enum.
///
/// \return Current active session as enum
// --------------------------------------------------------------------------------------------------------------------
E_LibUds_Session_t LibUds_GetSession(void);

// --------------------------------------------------------------------------------------------------------------------
/// \brief Sets the active session as enum.
///
/// \param session
/// The new session as enum
// --------------------------------------------------------------------------------------------------------------------
void LibUds_SetSession(E_LibUds_Session_t session);

// --------------------------------------------------------------------------------------------------------------------
/// \brief Returns the current session as enum from value.
///
/// \param value
/// The value of the requested session
///
/// \return Current active session as enum
// --------------------------------------------------------------------------------------------------------------------
E_LibUds_Session_t LibUds_GetSessionFromValue(uint8_t value);

// --------------------------------------------------------------------------------------------------------------------
/// \brief Returns the requested session as value.
///
/// \param session
/// Session that shall be converted to a value
///
/// \return Current requested session as value
// --------------------------------------------------------------------------------------------------------------------
uint8_t LibUds_GetSessionAsValue(E_LibUds_Session_t session);

// --------------------------------------------------------------------------------------------------------------------
/// \brief Returns the requested session as mask.
///
/// \param session
/// Session that shall be converted to a mask
///
/// \return Current requested session as mask
// --------------------------------------------------------------------------------------------------------------------
uint32_t LibUds_GetSessionAsMask(E_LibUds_Session_t session);

// --------------------------------------------------------------------------------------------------------------------
/// \brief Returns true if the active session is contained within the requested session mask.
///
/// \param mask
/// Session mask that must be checked
///
/// \return True if the active session mask is contained within mask.
// --------------------------------------------------------------------------------------------------------------------
bool_t LibUds_IsSessionMaskOk(uint32_t mask);

// --------------------------------------------------------------------------------------------------------------------
/// \brief Returns true if the active security state is contained within the requested security state mask.
///
/// \param mask
/// Security state mask that must be checked
///
/// \return True if the active security state mask is contained within mask.
// --------------------------------------------------------------------------------------------------------------------
bool_t LibUds_IsSecStateMaskOk(uint32_t mask);

// --------------------------------------------------------------------------------------------------------------------
/// \brief Returns the current active security state as enum.
///
/// \return Current active security state as enum
// --------------------------------------------------------------------------------------------------------------------
E_LibUds_SecState_t LibUds_GetSecState(void);

// --------------------------------------------------------------------------------------------------------------------
/// \brief Returns the requested security state as mask.
///
/// \param secState
/// Security state that shall be converted to a mask
///
/// \return Current requested security state as mask
// --------------------------------------------------------------------------------------------------------------------
uint32_t LibUds_GetSecStateAsMask(E_LibUds_SecState_t secState);

// --------------------------------------------------------------------------------------------------------------------
/// \brief Sets the current security state.
///
/// \param secState
/// New security state
// --------------------------------------------------------------------------------------------------------------------
void LibUds_SetSecState(E_LibUds_SecState_t secState);

// --------------------------------------------------------------------------------------------------------------------
/// \brief Returns the subservice of a RoutineControl service.
///
/// \details RoutineControl service identification consists of subfunction (= routine control type) + RID.
///
/// \param pPayload
/// Pointer to the payload
/// \param supportSprBit
/// True if service supports the SPR bit
///
/// \return RoutineControl subservice
// --------------------------------------------------------------------------------------------------------------------
uint32_t LibUds_GetRoutCtlSubservice(const uint8_t* pPayload, bool_t supportSprBit);

// --------------------------------------------------------------------------------------------------------------------
/// \brief Checks length of request and calls subfunction's ReqVerify.
///
/// \details This is a default ReqVerify function for services with subfunction (e.g. SessionControl, CommunicationControl etc).
///
/// \retval LIBRET_OK
/// Service is synchronous.
/// \retval LIBRET_PENDING
/// Subfunction is asynchronous.
/// \retval LIBRET_FAILED
/// An error occurred.
// --------------------------------------------------------------------------------------------------------------------
Ret_t LibUds_DefaultSubfunc_ReqVerify(void);

// --------------------------------------------------------------------------------------------------------------------
/// \brief Calls subfunction's ReqProcess.
///
/// \details This is a default ReqProcess function for services with subfunction (e.g. SessionControl, CommunicationControl etc).
///
/// \retval LIBRET_OK
/// Processing is finished.
/// \retval LIBRET_FAILED
/// There was an error.
/// \retval LIBRET_PENDING
/// Service is still running.
// --------------------------------------------------------------------------------------------------------------------
Ret_t LibUds_DefaultSubfunc_ReqProcess(void);

// --------------------------------------------------------------------------------------------------------------------
/// \brief Performs the pVerify function of the subservice if available, else returns the process type.
///
/// \details May only be called in other ReqVerify functions.
///
/// \param pSubsvc
/// Pointer to the subservice configuration
///
/// \retval LIBRET_OK
/// Subservice is synchronous.
/// \retval LIBRET_FAILED
/// There was an error and response has been written to TX buffer.
/// \retval LIBRET_PENDING
/// Subservice is asynchronous and must be executed later.
// --------------------------------------------------------------------------------------------------------------------
Ret_t LibUds_SubsvcReqVerify(const S_LibUds_SubsvcCfg_t* pSubsvc);

// --------------------------------------------------------------------------------------------------------------------
/// \brief Performs the pProcess function of the subservice if available, else does nothing and returns LIBRET_OK.
///
/// \param pSubsvc
/// Pointer to the subservice configuration
///
/// \retval LIBRET_OK
/// Processing is finished.
/// \retval LIBRET_FAILED
/// There was an error.
/// \retval LIBRET_PENDING
/// Service is still running.
// --------------------------------------------------------------------------------------------------------------------
Ret_t LibUds_SubsvcReqProcess(const S_LibUds_SubsvcCfg_t* pSubsvc);

// --------------------------------------------------------------------------------------------------------------------
/// \brief Checks if service request is valid, executes synchrounous services.
///
/// \details
/// Implements ISO 14229-1, p.20, Figure 5 - General server response behaviour.
/// 1. If the service request is invalid, LibUds_Verify writes the appropriate negative response to the TX buffer,
/// fills in the message container LibUds_Msg and returns LIBRET_OK.
/// 2. If the service is synchronous and valid, LibUds_Verify calls the service's Verify function directly.
/// After execution is complete, it writes the response (positive or negative), fills in the message container
/// LibUds_Msg and returns LIBRET_OK.
/// 3.If the service is asynchronous and valid, LibUds_Verify responds with LIBRET_PENDING.
/// The execution of the service is triggered later by Diag task.
///
/// \param	pIFace
/// Pointer to the message transfer structure
///
/// \retval LIBRET_OK
/// Service is synchronous; processing is complete.
/// \retval LIBRET_FAILED
/// There was an error and response has been written to TX buffer.
/// \retval LIBRET_PENDING
/// Service is asynchronous and must be executed later.
// --------------------------------------------------------------------------------------------------------------------
Ret_t LibUds_ReqVerify(S_LibUds_IfaceCfg_t* pIFace);

// --------------------------------------------------------------------------------------------------------------------
/// \brief Checks if a service request with a subfunction parameter.
///
/// \details
/// Implements ISO 14229-1, p.21, Figure 6 - General server response behaviour for request messages
/// with sub-function parameter.
///
/// \retval LIBRET_OK
/// Request is synchronous.
/// \retval LIBRET_FAILED
/// There was an error and response has been written to TX buffer.
/// \retval LIBRET_PENDING
/// Service is asynchronous and must be executed later in DiagTask.
// --------------------------------------------------------------------------------------------------------------------
Ret_t LibUds_ReqVerifySubfunc(void);

// --------------------------------------------------------------------------------------------------------------------
/// \brief Processes the UDS request.
///
/// \retval LIBRET_OK
/// Request is complete (positive), response has been written.
/// \retval LIBRET_FAILED
/// Request is complete (negative), response has been written.
/// \retval LIBRET_PENDING
/// Service processing is not ready; UDS layer will report end of execution to Diag later.
// --------------------------------------------------------------------------------------------------------------------
Ret_t LibUds_ReqProcess(void);

// --------------------------------------------------------------------------------------------------------------------
/// \brief Aborts the UDS request.
///
/// \details Called by DiagCom and therefore running in DiagCom context.
/// Aborts a running service because of a lost MOST connection etc.
/// If available the subservice's ReqAbort is being called, else the main service's ReqAbort is being called.
///
/// \retval LIBRET_OK
/// Current request has been aborted.
/// \retval LIBRET_FAILED
/// An error occurred.
/// \retval LIBRET_PENDING
/// The request will be aborted later.
// --------------------------------------------------------------------------------------------------------------------
Ret_t LibUds_ReqAbort(void);

// --------------------------------------------------------------------------------------------------------------------
/// \brief Performs some final actions.
///
/// \details
/// Sets the remaining information in the interface message container for DiagCom,
/// then resets the message container because we don't need it anymore,
/// and starts the S3 timer again if we are not in default session.
// --------------------------------------------------------------------------------------------------------------------
void LibUds_ProcessingDone(void);

// --------------------------------------------------------------------------------------------------------------------
/// \brief Returns if the UDS layer is processing a request.
///
/// \details
/// Checks if LibUds_Msg.pIface is NULL.
///
/// \retval true
/// Processing of a UDS service is ongoing.
/// \retval false
/// Currently no processing of a UDS service.
// --------------------------------------------------------------------------------------------------------------------
bool_t LibUds_IsProcessing(void);

#endif // LIB_UDS_H__INCLUDED
