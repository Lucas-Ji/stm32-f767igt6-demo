// --------------------------------------------------------------------------------------------------------------------
///
/// \file LibFifoQueue.h
///
/// \brief C Fifo Queue Implementation
///
///
///
///
/// All Rights Reserved.
///
// --------------------------------------------------------------------------------------------------------------------

#ifndef LIB_FIFO_QUEUE_H__INCLUDED
#define LIB_FIFO_QUEUE_H__INCLUDED

// --------------------------------------------------------------------------------------------------------------------
//	Includes
// --------------------------------------------------------------------------------------------------------------------
#include "LibTypes.h"

// --------------------------------------------------------------------------------------------------------------------
//	Global Definitions
// --------------------------------------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------------------------------------
///	\brief Define an instance of the FIFO as member
/// \param pBuffer Pointer to the FIFO buffer
/// \param itemLen The length of one item
/// \param numItems Max. number items resp. length of the buffer
/// \param overwrite Overwrite items flag
// --------------------------------------------------------------------------------------------------------------------
#define LIBFIFO_DEFINE_MEMBER(pBuffer, itemLength, numItems, overwrite)\
	{\
		.pFifoMem = (pBuffer),\
		.ItemLen = (itemLength),\
		.NumMaxItems = (numItems),\
		.OverwriteItems = (overwrite),\
		.HeadIdx = 0U,\
		.TailIdx = 0U,\
		.Count = 0U\
	}

// --------------------------------------------------------------------------------------------------------------------
///	\brief Define an instance of the FIFO
///	\param name The name of the instance
/// \param pBuffer Pointer to the FIFO buffer
/// \param itemLength The length of one item
/// \param numItems Max. number items resp. length of the buffer
/// \param overwrite Overwrite items flag
// --------------------------------------------------------------------------------------------------------------------
//lint -estring(773, LIBFIFO_DEFINE_INST) Definition is ok
#define LIBFIFO_DEFINE_INST(name, pBuffer, itemLength, numItems, overwrite)\
	S_LibFifoQueue_Inst_t (name) =\
	LIBFIFO_DEFINE_MEMBER((pBuffer), (itemLength), (numItems), (overwrite))

// --------------------------------------------------------------------------------------------------------------------
/// \brief Instance structure of the fifo queue
/// \attention headIdx, tailIdx and count must not be changed, if the fifo is in use
// --------------------------------------------------------------------------------------------------------------------
typedef struct
{
	uint32_t* const	pFifoMem;           //!< Pointer to the used memory
	const uint32_t	ItemLen;            //!< Length of one item in byte.
	const uint32_t	NumMaxItems;        //!< Maximum number of items in the queue
	const bool_t		OverwriteItems;     //!< Specifies whether items can be overwritten, if the queue is full

	// ----------------------------------------------------------------------------------------------------------------
	/// \brief index of the head element (oldest) in the queue
	/// \attention Initialize with 0, if the fifo is empty. Don´t change it, if the the fifo is in use (it is changed
	/// from the fifo itself).
	// ----------------------------------------------------------------------------------------------------------------
	uint32_t	HeadIdx;
	// ----------------------------------------------------------------------------------------------------------------
	/// \brief index of the tail element in the queue
	/// \attention Initialize with 0, if the fifo is empty. Don´t change it, if the the fifo is in use (it is changed
	/// from the fifo itself).
	// ----------------------------------------------------------------------------------------------------------------
	uint32_t	TailIdx;
	// ----------------------------------------------------------------------------------------------------------------
	/// \brief number of items currently in the queue
	/// \attention Initialize with 0, if the fifo is empty. Don´t change it, if the the fifo is in use (it is changed
	/// from the fifo itself).
	// ----------------------------------------------------------------------------------------------------------------
	uint32_t	Count;

} S_LibFifoQueue_Inst_t;

// --------------------------------------------------------------------------------------------------------------------
//	Global Data Types
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
//	Imported variables
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
//	Global Function Prototypes
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
/// \brief Init the fifo for the case that there are already some elements in the queue
/// \param pInst Pointer to the instance
/// \param headIdx The head index
/// \param count Number of elements in the queue
/// \sa S_LibFifoQueue_Inst_t
// --------------------------------------------------------------------------------------------------------------------
void LibFifoQueue_Init(S_LibFifoQueue_Inst_t* const pInst, const uint32_t headIdx, const uint32_t count);


// --------------------------------------------------------------------------------------------------------------------
/// \brief Add an item into the queue
/// \details The item will be copied into the queue using memcpy
/// \param pInst The settings of the fifo queue
/// \param pItem The item
/// \return True if item was inserted
/// \sa S_LibFifoQueue_Inst_t
// --------------------------------------------------------------------------------------------------------------------
bool_t LibFifoQueue_Push(S_LibFifoQueue_Inst_t* const pInst, const void* const pItem);

// --------------------------------------------------------------------------------------------------------------------
/// \brief Pop the first item from the queue
/// \param pInst The settings of the fifo queue
/// \sa S_LibFifoQueue_Inst_t
// --------------------------------------------------------------------------------------------------------------------
void LibFifoQueue_Pop(S_LibFifoQueue_Inst_t* const pInst);

// --------------------------------------------------------------------------------------------------------------------
/// \brief Clears all elements from the queue
/// \param pInst The settings of the fifo queue
/// \sa S_LibFifoQueue_Inst_t
// --------------------------------------------------------------------------------------------------------------------
void LibFifoQueue_Clear(S_LibFifoQueue_Inst_t* const pInst);

// --------------------------------------------------------------------------------------------------------------------
/// \brief Access queue element selected by the index
/// \param itemIndex Index of the element to be accessed,
/// counted from 0x00 for the head element
/// \param pInst The settings of the fifo queue
/// \return The element, NULL if item does not exist
/// \sa S_LibFifoQueue_Inst_t
// --------------------------------------------------------------------------------------------------------------------
void* LibFifoQueue_GetItem(const S_LibFifoQueue_Inst_t* const pInst, uint32_t itemIndex);

// --------------------------------------------------------------------------------------------------------------------
/// \brief Access the first queue element and pop it from the queue
/// \param pInst The settings of the fifo queue
/// \return The element, NULL if item does not exist
/// \sa S_LibFifoQueue_Inst_t
// --------------------------------------------------------------------------------------------------------------------
void* LibFifoQueue_GetPopItem(S_LibFifoQueue_Inst_t* const pInst);

#endif  //LIB_FIFO_QUEUE_H__INCLUDED

