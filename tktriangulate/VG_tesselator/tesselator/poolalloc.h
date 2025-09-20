#ifndef __VGES_TESSELATION_ALLOCATOR_H
#define __VGES_TESSELATION_ALLOCATOR_H
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


typedef struct VGES_PoolBlock VGES_PoolBlock;

typedef struct VGES_Pool VGES_Pool;

typedef struct _linknode _linknode;


struct VGES_PoolBlock
//////////////////////
// header for a pool block:
{
  VGuint           m_size;
  VGuint           m_used;
  VGES_PoolBlock * next;
  VGuint           m_OnHeap;
};

struct _linknode
////////////////
// dummy stucture to link freed elements
{
  _linknode * next;
};


struct VGES_Pool
/////////////////
// control structure for the pool
{
  VGuint           Elements;
  VGuint           ElementSize;
  VGuint           BlockSize;
  _linknode      * FreeList;
  VGES_PoolBlock * list;
};

void   VGES_Pool_Init   (VGES_Pool * pool, VGuint ElementSize, VGuint BlockSize);
// Initialiizes a pool for allocation.

void   VGES_Pool_Done   (VGES_Pool * pool);
// free all resources

void   VGES_Pool_Clear  (VGES_Pool * pool);
// free all previously allocated objects, but keep the memory allocated.

void * VGES_Pool_Alloc  (VGES_Pool * pool);
// alloc one element 

void   VGES_Pool_Free   (VGES_Pool * pool, void * data);
// free one element 


// Clear as a macro:
#define VGES_POOL_CLEAR(P)                    \
{                                             \
  VGES_PoolBlock * run;                       \
  for (run = (P).list; run; run = run->next)  \
    run->m_used = 0;                          \
  (P).FreeList = 0;                           \
}                                             \

// Free as a macro
#define VGES_POOL_FREE(P,D)                   \
{                                             \
  _linknode * temp = (_linknode *) (D);       \
  temp->next = (P).FreeList;                  \
  (P).FreeList = temp;                        \
}

#endif
