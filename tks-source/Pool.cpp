/// Pool.cpp
///
/// (c) 2001-2020 Bastian Spiegel <bs@tkscript.de>
///     - distributed under terms of the GNU general public license (GPL).
///
///

#include "tks.h"
#include "Pool.h"


Pool::Pool(void) {
   elements     = NULL; 
   max_elements = free_elements = used_elements = 0u; 
   freelist     = NULL;
   uselist      = NULL;
   usecopylist  = NULL;
   logictable   = NULL;

#ifdef TKS_POOL_ACCOUNTING
   total_num_allocs = 0u;
   peak_load        = 0u;
#endif
}

Pool::~Pool() {
   freeElements();
}

sBool Pool::allocElements(sSI _maxelements) {
   freeElements();
   elements = new void*[_maxelements];

   if(NULL != elements)
   {
      max_elements = _maxelements; // xxx +1 ??
      freelist = new(std::nothrow) sSI[max_elements + 1];

      if(NULL != freelist)
      {
         uselist = new(std::nothrow) sSI[max_elements + 1];

         if(NULL != uselist)
         {
            usecopylist = new(std::nothrow) sSI[max_elements + 1];

            if(NULL != usecopylist)
            {
               logictable = new(std::nothrow) sSI[max_elements];

               if(NULL != logictable)
               {
                  free_elements = max_elements;

                  sSI i;
                  for(i=0; i<max_elements; i++)
                  {
                     elements   [i] = 0;
                     freelist   [i] = i;
                     uselist    [i] = NO_ID;
                     logictable [i] = NO_ID;
                     usecopylist[i] = NO_ID;
                  }

                  freelist   [i] = NO_ID; // END OF LIST tag
                  uselist    [i] = NO_ID;
                  usecopylist[i] = NO_ID;
                  return 1;
               }
               else
               {
                  delete [] usecopylist;
                  usecopylist = NULL;

                  delete [] uselist;
                  uselist = NULL;

                  delete [] freelist;
                  freelist = NULL;

                  delete [] elements;
                  elements = NULL; 

                  max_elements = 0;
               }
            }
            else
            {
               delete [] uselist;
               uselist = NULL;

               delete [] freelist;
               freelist = NULL;

               delete [] elements;
               elements = NULL;

               max_elements = 0;
            }
         }
         else
         {
            delete [] freelist;
            freelist = NULL;

            delete [] elements;
            elements = NULL;

            max_elements = 0;
         }
      }
      else
      {
         delete [] elements;
         elements = NULL;

         max_elements = 0;
      }
   }

   return 0;
}

void Pool::freeElements(void) {
   if(NULL != elements)
   {
      delete [] elements; 
      elements = NULL;

      max_elements = 0;

      if(NULL != logictable) 
      {
         delete [] logictable;
         logictable = NULL;
      }

      if(NULL != freelist) 
      { 
         delete [] freelist; 
         freelist = NULL; 
      }

      if(NULL != uselist) 
      { 
         delete [] uselist; 
         uselist = NULL; 
      }

      if(NULL != usecopylist) 
      { 
         delete [] usecopylist; 
         usecopylist = NULL; 
      }

      free_elements = 0;
      used_elements = 0;
   }
}

void *Pool::operator [](PoolId _logid) {
   if( (_logid >= 0) && (_logid < max_elements) )
   {
      if(NO_ID != logictable[_logid])
      {
         return elements[_logid];
      }
   }

   return NULL;
}

PoolId Pool::qAlloc(void) {
   // Return NO_ID or element index

   if(0u != free_elements)
   {
      sSI eid = freelist[--free_elements]; // get real element index
      logictable[eid] = used_elements; // save position in uselist
      uselist[used_elements++] = eid;
      freelist[free_elements] = NO_ID;

#ifdef TKS_POOL_ACCOUNTING
      total_num_allocs++;

      if(used_elements > peak_load)
      {
         peak_load = used_elements;
      }
#endif // TKS_POOL_ACCOUNTING

      return eid;
   }

   return NO_ID;
}

void Pool::qFree(PoolId _logid) {
   if(used_elements > 0)
   {
      if( (_logid >= 0) && (_logid < max_elements) )
      {
         sSI eid = logictable[_logid]; // get uselist index

         if(NO_ID != eid)
         {
            if(--used_elements && (eid != used_elements)) // is not last element?
            {
               // replace with last used element
               uselist[eid] = uselist[used_elements];
               logictable[uselist[used_elements]] = eid;
               eid = used_elements;
            }

            uselist    [used_elements]   = NO_ID; // last element is now unused
            logictable [_logid]          = NO_ID; // element is now unused
            freelist   [free_elements++] = _logid;
         }
      }
   }
}

void Pool::copyUseList(void) {
   if(NULL != usecopylist)
   {
      sSI i = 0;
      for(; i<used_elements; i++)
      {
         usecopylist[i] = uselist[i];
      }

      usecopylist[i] = NO_ID;
   }
}

void Pool::empty(void) {
   sSI i;
   for(i=0; i<max_elements; i++)
   {
      uselist[i] = usecopylist[i] = logictable[i] = NO_ID;
      freelist[i] = i; 
   }

	used_elements = 0; 
	free_elements = max_elements;
   uselist[i] = freelist[i] = usecopylist[i] = NO_ID;
}

sBool Pool::isEmpty(void) {
   return (0u == used_elements);
}

PoolId Pool::getIDByObject(void *_o) const { 
	sSI i = 0; 
	for(; i<used_elements; i++)  
	{ 
		if(elements[uselist[i]] == _o)
      {
			return uselist[i]; 
      }
	}
   
	return NO_ID; 
} 

void Pool::printDebug(const char *_name) {
#ifdef TKS_POOL_ACCOUNTING
   tkscript->printf("[...] %s pool load=%i/%i (%3.2f pct, %i allocations, peak load=%i)\n", 
                    _name,
                    used_elements, max_elements, 
                    100.0f * (used_elements / ((sF32)max_elements)),
                    total_num_allocs,
                    peak_load
                    );
#else
   (void)_name;
#endif
}
