/**********************************************************************
 *    c Source:            poolalloc.c
 *    Author:              Nils Pipenbrinck
 *    Creation Date:       09.02.2006
 *    Project Name:        OpenVG 1.0 Implementation 
 *    Description:             
 *
 *  Implementation of a simple pool allocator for identical sized elements.
 *
 *
 **********************************************************************
 *     History report
 *     
 *     Reason of changes:
 *
 *      - initial implementation
 *
 **********************************************************************/
#include "../common.h"
#include "poolalloc.h"

void VGES_Pool_Init   (VGES_Pool * pool, VGuint ElementSize, VGuint BlockSize)
////////////////////////////////////////////////////////////
{
  pool->Elements    = 0;
  pool->ElementSize = ElementSize;
  pool->BlockSize   = BlockSize;

  while (pool->ElementSize & 7) 
    pool->ElementSize++;

  pool->FreeList    = 0;
  pool->list        = 0;
}


void VGES_Pool_Done   (VGES_Pool * pool)
////////////////////////////////////////
{
  VGES_PoolBlock * run;
  VGES_PoolBlock * next;

  for (run = pool->list; run; run = next)
  {
    next = run->next;
    if (run->m_OnHeap)
      vgtesselator_free (run);
  }
}

void VGES_Pool_Clear (VGES_Pool * pool)
////////////////////////////////////////
{
  VGES_PoolBlock * run;     // @gcov: Currently unused, OpenVG uses a macro-version instead
  for (run = pool->list; run; run = run->next)
    run->m_used = 0;
  pool->FreeList = 0;
}


void * VGES_Pool_Alloc (VGES_Pool * pool)
/////////////////////////////////////////
{
  VGES_PoolBlock * block;

  if (pool->FreeList)
  {
    _linknode * temp = pool->FreeList;
    _linknode * next = temp->next;
    pool->FreeList = next;
    return temp;
  } else 
  {
    // allocate new node:
    for (block = pool->list; block ; block  = block ->next)
    {
      if (block->m_used < block->m_size)
      {
        VGubyte * base = (VGubyte*) (&block->m_OnHeap);
        base += sizeof (block->m_OnHeap);
        return (void *) (base + (pool->ElementSize * block->m_used++));
      }
    }
  }

  block = vgtesselator_alloc (sizeof (VGES_PoolBlock) + pool->ElementSize * pool->BlockSize);
  if (!block) return 0;

  block->m_size   = pool->BlockSize;
  block->m_used   = 0;
  block->m_OnHeap = 1;
  block->next     = pool->list;
  pool->list      = block;
  return VGES_Pool_Alloc (pool);
}


void VGES_Pool_Free  (VGES_Pool * pool, void * data)
/////////////////////////////////////////////////////
{
  _linknode * temp = (_linknode *) data;      // @gcov: Currently unused, OpenVG uses a macro-version instead
  temp->next = pool->FreeList;
  pool->FreeList = temp;
}

