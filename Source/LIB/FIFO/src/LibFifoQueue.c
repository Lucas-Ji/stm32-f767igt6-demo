// --------------------------------------------------------------------------------------------------------------------
///
/// \file LibFifoQueue.c
///
/// \brief Fifo queue implementation
///
///
///
/// All Rights Reserved.
///
// --------------------------------------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------------------------------------
//	Local Definitions to control properties inside header files
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
//	Includes
// --------------------------------------------------------------------------------------------------------------------
#include <string.h>
#include "LibFifoQueue.h"

// --------------------------------------------------------------------------------------------------------------------
//	Local Definitions
// --------------------------------------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------------------------------------
//	Local Data Types
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
//	Global variables
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
//	Local variables
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
//	Local Function Prototypes
// --------------------------------------------------------------------------------------------------------------------

// ====================================================================================================================
// LibFifoQueue_Init:
// ====================================================================================================================
void LibFifoQueue_Init(S_LibFifoQueue_Inst_t* const pInst, const uint32_t headIdx, const uint32_t count)
{
	Lib_Assert((NULL != pInst) && (headIdx < pInst->NumMaxItems) && (count <= pInst->NumMaxItems));

	pInst->HeadIdx = headIdx;

	if (0U == count)
	{
		pInst->TailIdx = headIdx;
	}
	else
	{
		// Calculation of the tail position
		// Decrease by 1, the tail is the last valid element and not first free space
		uint32_t posTail = headIdx + count - 1U ;

		if (posTail >= pInst->NumMaxItems)
		{
			posTail -= pInst->NumMaxItems;
		}
		pInst->TailIdx = posTail;
	}
	
	pInst->Count = count;
}


// ====================================================================================================================
// LibFifoQueue_Push:
// ====================================================================================================================
bool_t LibFifoQueue_Push(S_LibFifoQueue_Inst_t* const pInst, const void* const pItem)
{
	bool_t ret = false;
#ifndef FIFO_QUEUE_NO_NULL_QUECKS

	if (NULL != pInst)
	{
		if ((NULL != pItem) && (NULL != pInst->pFifoMem))
		{
#endif

			if (0U == pInst->Count)
			{
				// The queue is empty and we add the first element
				// position of the tail will not be changed.
				if (pInst->HeadIdx == pInst->TailIdx)
				{
					//lint -e{9087, 9016} Cast is ok, pointer arithmetic checked
					(void)memcpy((((uint8_t*)(void*)pInst->pFifoMem) + (pInst->TailIdx * pInst->ItemLen)),
								 pItem,
								 pInst->ItemLen);
					pInst->Count++;
					ret = true;
				}
			}
			else if (pInst->Count < pInst->NumMaxItems)
			{
				pInst->TailIdx++;

				if (pInst->TailIdx >= pInst->NumMaxItems)
				{
					pInst->TailIdx = 0U;
				}
				//lint -e{9087, 9016} Cast is ok, pointer arithmetic checked
				(void)memcpy((((uint8_t*)(void*)pInst->pFifoMem) + (pInst->TailIdx * pInst->ItemLen)),
							 pItem,
							 pInst->ItemLen);
				pInst->Count++;
				ret = true;
			}
			else if (pInst->OverwriteItems)
			{
				pInst->TailIdx = pInst->HeadIdx;
				pInst->HeadIdx++;

				if (pInst->HeadIdx >= pInst->NumMaxItems)
				{
					pInst->HeadIdx = 0U;
				}
				//lint -e{9087, 9016} Cast is ok, pointer arithmetic checked
				(void)memcpy((((uint8_t*)(void*)pInst->pFifoMem) + (pInst->TailIdx * pInst->ItemLen)),
							 pItem,
							 pInst->ItemLen);
				ret = true;
			}
			else
			{
				// discard the item
			}

#ifndef FIFO_QUEUE_NO_NULL_QUECKS
		}
	}

#endif
	return ret;
}

// ====================================================================================================================
// LibFifoQueue_Pop:
// ====================================================================================================================
void LibFifoQueue_Pop(S_LibFifoQueue_Inst_t* const pInst)
{
#ifndef FIFO_QUEUE_NO_NULL_QUECKS

	if (NULL != pInst)
	{
#endif

		if (0U != pInst->Count)
		{
			pInst->Count--;

			if (0U != pInst->Count)
			{
				// move head index only if this
				// is not the last element in
				// the queue
				pInst->HeadIdx++;

				if (pInst->HeadIdx >= pInst->NumMaxItems)
				{
					pInst->HeadIdx = 0U;
				}
			}
		}

#ifndef FIFO_QUEUE_NO_NULL_QUECKS
	}

#endif
}

// ====================================================================================================================
// LibFifoQueue_Clear:
// ====================================================================================================================
void LibFifoQueue_Clear(S_LibFifoQueue_Inst_t* const pInst)
{
#ifndef FIFO_QUEUE_NO_NULL_QUECKS

	if (NULL != pInst)
	{
#endif
		pInst->HeadIdx = 0U;
		pInst->TailIdx = 0U;
		pInst->Count = 0U;
#ifndef FIFO_QUEUE_NO_NULL_QUECKS
	}

#endif
}

// ====================================================================================================================
// LibFifoQueue_GetItem:
// ====================================================================================================================
void* LibFifoQueue_GetItem(const S_LibFifoQueue_Inst_t* const pInst, const uint32_t itemIndex)
{
	void* pRet = NULL;
#ifndef FIFO_QUEUE_NO_NULL_QUECKS

	if (NULL != pInst)
	{
		if (NULL != pInst->pFifoMem)
		{
#endif

			if ((itemIndex < pInst->Count) && (0U != pInst->Count) && (NULL != pInst->pFifoMem))
			{
				uint32_t pos = (pInst->HeadIdx + itemIndex);

				if (pos >= pInst->NumMaxItems)
				{
					pos -= pInst->NumMaxItems;
				}
				//lint -e{9087, 9016} Cast is ok, pointer arithmetic checked
				pRet = ((uint8_t*)(void*)pInst->pFifoMem) + (pos * pInst->ItemLen);
			}

#ifndef FIFO_QUEUE_NO_NULL_QUECKS
		}
	}

#endif
	return pRet;
}

// ====================================================================================================================
// LibFifoQueue_GetPopItem:
// ====================================================================================================================
void* LibFifoQueue_GetPopItem(S_LibFifoQueue_Inst_t* const pInst)
{
	void* pRet = NULL;
#ifndef FIFO_QUEUE_NO_NULL_QUECKS

	if (NULL != pInst)
	{
		if (NULL != pInst->pFifoMem)
		{
#endif

			if (0U != pInst->Count)
			{
				//lint -e{9087, 9016} Cast is ok, pointer arithmetic checked
				pRet = ((uint8_t*)(void*)pInst->pFifoMem) + (pInst->HeadIdx * pInst->ItemLen);
				// now pop the item
				pInst->Count--;

				if (0U != pInst->Count)
				{
					// move head index only if this
					// is not the last element in
					// the queue
					pInst->HeadIdx++;

					if (pInst->HeadIdx >= pInst->NumMaxItems)
					{
						pInst->HeadIdx = 0U;
					}
				}
			}

#ifndef FIFO_QUEUE_NO_NULL_QUECKS
		}
	}

#endif
	return pRet;
}


