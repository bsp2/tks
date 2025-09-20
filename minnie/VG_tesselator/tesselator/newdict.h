#ifndef __NEWDICT_H
#define __NEWDICT_H
/**********************************************************************
 *    h Source:            newdict.h
 *    Author:              Nils Pipenbrinck
 *    Creation Date:       09.02.2006
 *    Project Name:        OpenVG 1.0 Implementation 
 *    Description:             
 *
 *  Implementation of a simple dictionary. Used to replace the 
 *  dictionary from the tesselator.
 *
 *  Avoids useless preallocations and is not as brute force as the 
 *  code it replaces..
 *
 **********************************************************************
 *     History report
 *     
 *     Reason of changes:
 *
 *      - initial implementation
 *
 **********************************************************************/
#include "poolalloc.h"

typedef struct DictNode      DictNode;
typedef struct Dict          Dict;


typedef int (*DictCompare) (void *frame, void *key1, void *key2);

Dict		  *dictNewDict      (void *frame, DictCompare leq);
		
void		   dictDeleteDict   (Dict *dict );

DictNode	*dictSearch       (Dict *dict, void * key);

DictNode	*dictInsertBefore (Dict *dict, DictNode *node, void *);

void		   dictDelete       (Dict *dict, DictNode *node );

void       dictClearDict    (Dict * dict, void * frame, DictCompare leq);


struct DictNode
///////////////
{
  void      *key;
  DictNode	*next;
  DictNode  *prev;
};

struct Dict
///////////
{
  DictNode	      head;              // sentinel node:
  DictCompare     leq;               // compare function pointer.
  void		      * frame;             // opaque handle
  VGES_Pool       m_pool;            // node pool:
};



#endif
