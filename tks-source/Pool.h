/// pool.h 
/// 
/// (c) 2001-2019 Bastian Spiegel <bs@tkscript.de> 
///     - distributed under terms of the GNU general public license (GPL). 
/// 

#ifndef __TKS_POOLBASE_H__
#define __TKS_POOLBASE_H__


/// originally written on   : 03Jan2002
///
// a quick alloc/free pool of elements.
// the element order is changed when allocating/freeing elements.
// this container class works _without_ linear search.
 
typedef sSI PoolId;

class Pool {
public:
   enum { NO_ID = -1 };

public:
   sSI       max_elements;
   void    **elements;
   sSI      *freelist;
   sSI      *uselist;
   sSI      *usecopylist;
   sSI      *logictable; // maps logical element id to uselist index
   sSI       free_elements;
   sSI       used_elements;

#ifdef TKS_POOL_ACCOUNTING
   sSI       total_num_allocs; 
   sSI       peak_load;
#endif // TKS_POOL_ACCOUNTING

public:
   Pool(void);
   virtual ~Pool(); // Declare virtual d'tor so that virtual classes derived from this class do not trigger warnings

   void printDebug (const char *_name);

   sBool    allocElements (int _maxelements);
   void     freeElements  (void);
   void     copyUseList   (void);
   PoolId   qAlloc        (void);
   void     qFree         (PoolId _logid);
   void    *operator []   (PoolId _logid);
   void     empty         (void);
   sBool    isEmpty       (void);
   PoolId   getIDByObject (void *_o) const; 
};


#endif // __TKS_POOLBASE_H__
