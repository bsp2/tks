/**********************************************************************
 *    c-Source:            newdict.c
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
#include "../common.h"
#include "newdict.h"


void dictClearDict (Dict * dict, void * frame, DictCompare leq)
///////////////////////////////////////////////////////////////
{
  DictNode *head;
  head         = &dict->head;
  head->key    = 0;
  head->next   = head;
  head->prev   = head;
  dict->frame  = frame;
  dict->leq    = leq;
  VGES_POOL_CLEAR (dict->m_pool);
}


Dict *dictNewDict( void *frame, DictCompare leq)
////////////////////////////////////////////////
{
  Dict *dict = (Dict *) vgtesselator_alloc( sizeof( Dict ));
  if (!dict) return 0;
  VGES_Pool_Init (&dict->m_pool, sizeof (DictNode), 64);
  dictClearDict (dict, frame, leq);
  return dict;
}

void dictDeleteDict( Dict *dict )
/////////////////////////////////
{
  VGES_Pool_Done (&dict->m_pool);
  vgtesselator_free (dict);
}

DictNode *dictInsertBefore( Dict *dict, DictNode *node, void * key )
////////////////////////////////////////////////////////////////////
{
  DictNode *newNode = VGES_Pool_Alloc(&dict->m_pool);

  // zero-key is reserved for the sentinel node:
  //assert (key);

  if (!newNode) return 0;

  do 
  {
    node = node->prev;
  } while( (node->key) && !(*dict->leq)(dict->frame, node->key, key));


  newNode->key     = key;
  newNode->next    = node->next;
  node->next->prev = newNode;
  newNode->prev    = node;
  node->next       = newNode;
  return newNode;
}


void dictDelete( Dict *dict, DictNode *node )
/////////////////////////////////////////////
{
  node->next->prev = node->prev;
  node->prev->next = node->next;
  VGES_POOL_FREE(dict->m_pool, node);
}


DictNode *dictSearch( Dict *dict, void * key )
///////////////////////////////////////////////
{
  DictNode *node = &dict->head;

  do {
    node = node->next;
  } while( node->key != NULL && ! (*dict->leq)(dict->frame, key, node->key));

  return node;
}

