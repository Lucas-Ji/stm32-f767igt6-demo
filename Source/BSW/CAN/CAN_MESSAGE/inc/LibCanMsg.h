// --------------------------------------------------------------------------------------------------------------------
///
/// \file LibCan.h
///
/// \brief Generic CAN interface.
///
/// In case of BusOff error, a callback function has to be called. Callback function may be called from interrupt
/// context.
///
/// Copyright (c) 2021 Neusoft.
/// All Rights Reserved.
///
// --------------------------------------------------------------------------------------------------------------------

#ifndef LIB_CAN_H__INCLUDED
#define LIB_CAN_H__INCLUDED

// --------------------------------------------------------------------------------------------------------------------
//	Includes
// --------------------------------------------------------------------------------------------------------------------
#include "LibCanDrvMsg.h"

// --------------------------------------------------------------------------------------------------------------------
//	Global Definitions
// --------------------------------------------------------------------------------------------------------------------

#define LIBCAN_MAXDATABYTENUM		8		//!< Maximum number of CAN message data bytes.

// --------------------------------------------------------------------------------------------------------------------
/// \brief I/O command to start CAN communication.
///
/// No data is required for this I/O command.
///
/// This I/O command may return the following values:
/// - #LIBRET_OK
// --------------------------------------------------------------------------------------------------------------------
#define LIBCAN_IOCTL_START			 0

// --------------------------------------------------------------------------------------------------------------------
/// \brief I/O command to stop CAN communication.
///
/// No data is required for this I/O command.
///
/// This I/O command may return the following values:
/// - #LIBRET_OK
// --------------------------------------------------------------------------------------------------------------------
#define LIBCAN_IOCTL_STOP			  1

// --------------------------------------------------------------------------------------------------------------------
/// \brief Return next CAN message from the received messages.
///
/// This I/O command expects a pointer to a variable of type S_LibCan_Msg_t to store the received CAN message.
///
/// \attention CAN driver has to ensure that more than LIBCAN_MAXDATABYTENUM bytes of data will not be stored in the
/// structure member Data.
///
/// This I/O command may return the following values:
/// - #LIBRET_OK - The CAN message has been received.
/// - #LIBRET_NO_ENTRY - No CAN message available.
// --------------------------------------------------------------------------------------------------------------------
#define LIBCAN_IOCTL_GET_NEXT_MSG		2

// --------------------------------------------------------------------------------------------------------------------
/// \brief Send CAN message
///
/// This I/O command expects a pointer to a variable of type S_LibCan_Msg_t which contains CAN message to be sent.
///
/// This I/O command may return the following values:
/// - #LIBRET_OK - The CAN message has been sent successfully.
/// - #LIBRET_INV_CALL - Try to send CAN message, but driver is configured to be in listen only mode.
/// - #LIBRET_BUSY - Try to send CAN message, but CAN driver is currently unable to send it.
// --------------------------------------------------------------------------------------------------------------------
#define LIBCAN_IOCTL_SEND_MSG		   3


// --------------------------------------------------------------------------------------------------------------------
//	Global Data Types
// --------------------------------------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------------------------------------
/// \brief Enumerate all supported CAN message data size in bytes.
// --------------------------------------------------------------------------------------------------------------------
typedef enum
{
	LIBCAN_DLCSIZE_0_B,		//<! \brief Data size is 0 bytes
	LIBCAN_DLCSIZE_1_B,		//<! \brief Data size is 1 bytes
	LIBCAN_DLCSIZE_2_B,		//<! \brief Data size is 2 bytes
	LIBCAN_DLCSIZE_3_B,		//<! \brief Data size is 3 bytes
	LIBCAN_DLCSIZE_4_B,		//<! \brief Data size is 4 bytes
	LIBCAN_DLCSIZE_5_B,		//<! \brief Data size is 5 bytes
	LIBCAN_DLCSIZE_6_B,		//<! \brief Data size is 6 bytes
	LIBCAN_DLCSIZE_7_B,		//<! \brief Data size is 7 bytes
	LIBCAN_DLCSIZE_8_B,		//<! \brief Data size is 8 bytes
	LIBCAN_DLCSIZE_12_B,	//<! \brief Data size is 12 bytes
	LIBCAN_DLCSIZE_16_B,	//<! \brief Data size is 16 bytes
	LIBCAN_DLCSIZE_20_B,	//<! \brief Data size is 20 bytes
	LIBCAN_DLCSIZE_24_B,	//<! \brief Data size is 24 bytes
	LIBCAN_DLCSIZE_32_B,	//<! \brief Data size is 32 bytes
	LIBCAN_DLCSIZE_48_B,	//<! \brief Data size is 48 bytes
	LIBCAN_DLCSIZE_64_B		//<! \brief Data size is 64 bytes
	
} E_LibCan_DlcSize_t;

typedef enum
{
    CanChannel_1,
    CanChannel_2,
	CanChannel_All
} E_LibCan_Channel_t;

// --------------------------------------------------------------------------------------------------------------------
/// \brief CAN message structure.
// --------------------------------------------------------------------------------------------------------------------
typedef struct
{
	// ----------------------------------------------------------------------------------------------------------------
	/// \brief The CAN message data channel.
	// ----------------------------------------------------------------------------------------------------------------
	E_LibCan_Channel_t CanDevId;

	// ----------------------------------------------------------------------------------------------------------------
	/// \brief The flag is the message with extended ID.
	// ----------------------------------------------------------------------------------------------------------------
	bool_t IsExtId;

	// ----------------------------------------------------------------------------------------------------------------
	/// \brief The flag whether the message is CAN FD.
	// ----------------------------------------------------------------------------------------------------------------
	bool_t IsCanFd;

	// ----------------------------------------------------------------------------------------------------------------
	/// \brief The flag whether the CAN FD message is transmitted with the switching bit rate.
	// ----------------------------------------------------------------------------------------------------------------
	bool_t IsBrs;

	// ----------------------------------------------------------------------------------------------------------------
	/// \brief The flag whether the CAN message is remote.
	// ----------------------------------------------------------------------------------------------------------------
	bool_t IsRemote;

	// ----------------------------------------------------------------------------------------------------------------
	/// \brief The CAN message ID.
	// ----------------------------------------------------------------------------------------------------------------
	uint32_t Id;

	// ----------------------------------------------------------------------------------------------------------------
	/// \brief The CAN message data length.
	// ----------------------------------------------------------------------------------------------------------------
	E_LibCan_DlcSize_t Length;

	// ----------------------------------------------------------------------------------------------------------------
	/// \brief The CAN message data timestamp.
	// ----------------------------------------------------------------------------------------------------------------
	uint32_t Timestamp;

	// ----------------------------------------------------------------------------------------------------------------
	/// \brief The array of the CAN message data.
	// ----------------------------------------------------------------------------------------------------------------
	uint8_t Data[LIBCAN_MAXDATABYTENUM];

} S_LibCan_Msg_t;

// --------------------------------------------------------------------------------------------------------------------
//	Imported Variables
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
//	Global Function Prototypes
// --------------------------------------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------------------------------------
/// \brief Get CAN message data length
///
/// \param dlc
/// CAN message DLC (Data Length Code)
///
/// \return
/// CAN message data length in bytes
// --------------------------------------------------------------------------------------------------------------------
uint8_t LibCan_GetMsgDataLength(E_LibCan_DlcSize_t dlc);


#endif // LIB_CAN_H__INCLUDED

