// TKS_Mailbox.cpp 
/// 
/// (c) 2009-2023 by Bastian Spiegel <bs@tkscript.de> 
///     - distributed under terms of the GNU general public license (GPL). 
/// 

#include "tks.h" 

#include "YAC_ValueObject.h"
#include "YAC_Event.h"
#include "TKS_Mutex.h"
#include "TKS_Condition.h" 
#include "TKS_Mailbox.h"

TKS_Mailbox::TKS_Mailbox(void) {
   init();
}

TKS_Mailbox::~TKS_Mailbox() {
   freeEventQueue();
}

void TKS_Mailbox::init(void) {
   events         = NULL;
   max_queue_size    = 0u;
   write_off         = 0u;
   read_off          = 0u;
   num_queued_events = 0u;

   cond_newevent.create(0/*auto-reset*/);
}

void TKS_Mailbox::freeEventQueue(void) {
   mtx_queue.lock();

   if(NULL != events)
   {
      for(sUI i = 0u; i < max_queue_size; i++)
      {
         YAC_Event *ev = events[i];
         if(NULL != ev)
         {
            YAC_DELETE(ev);
            events[i] = NULL;
         }
      }
      delete [] events;
      events            = NULL;
      max_queue_size    = 0u;
      write_off         = 0u;
      read_off          = 0u;
      num_queued_events = 0u;
   }

   mtx_queue.unlock();
}

void TKS_Mailbox::allocEventQueue(sUI _size) {
   TKS_Mailbox::freeEventQueue();

   mtx_queue.lock();

   if(_size)
   {
      events = new YAC_Event*[_size];
      max_queue_size = _size;
      for(sUI i = 0u; i < _size; i++)
         events[i] = NULL;
   }

   mtx_queue.unlock();
}

sSI TKS_Mailbox::sendEvent(YAC_Event *_event) {
   sSI r;

   mtx_queue.lock();

   if(max_queue_size)
   {
      // Create a copy of the event
      YAC_Event *ev;

      if(num_queued_events == max_queue_size)
      {
         // queue full
         if(events[write_off])
         {
            Dprintf("[~~~] TKS_Mailbox: discarding event #%d.\n", write_off);
            ev = events[write_off];
         }
         else
         {
            // Event was already unlinked by waitEventById(), fill slot with a new event object
            ev = events[write_off] = (YAC_Event*) YAC_NEW_CORE_POOLED(YAC_CLID_EVENT);
         }

         read_off = (write_off + 1u) % max_queue_size;
      }
      else
      {
         ev = (YAC_Event*) YAC_NEW_CORE_POOLED(YAC_CLID_EVENT);
         num_queued_events++;
      }

      ev->copySafe(_event);
      ev->id     = _event->id;
      ev->sub_id = _event->sub_id;

      events[write_off] = ev;

      write_off = (write_off + 1u) % max_queue_size;

      // Signal new event condition
      r = cond_newevent.raise();
   }
   else
   {
      Dprintf("[---] TKS_Mailbox::sendEvent: event queue not allocated.\n");
      r = ~0;
   }

   mtx_queue.unlock();

   return r;
}

sSI TKS_Mailbox::_sendEvent(YAC_Object *_event) {
   if(YAC_BCHK(_event, YAC_CLID_EVENT))
   {
      YAC_Event *ev = (YAC_Event*)_event;
      return sendEvent(ev);
   }
   return 0;
}


sUI TKS_Mailbox::getNumQueuedEvents(void) {
   mtx_queue.lock();

   sUI r = num_queued_events;

   mtx_queue.unlock();

   return r;
}

YAC_Event *TKS_Mailbox::peekEvent(void) {
   YAC_Event *r;

   mtx_queue.lock();

   if(num_queued_events)
   {
      // Return oldest pending event
      sSI idx = findEventIdxNoMtx();
      if(-1 != idx)
      {
         r = events[idx];
      }
      else
      {
         // all events have been already read
         r = NULL;
      }
   }
   else
   {
      // No pending events
      r = NULL;
   }

   mtx_queue.unlock();

   return r;
}

YAC_Event *TKS_Mailbox::peekEventById(sSI _id) {
   YAC_Event *r;

   mtx_queue.lock();

   if(num_queued_events)
   {
      // Find event
      sSI idx = findEventIdxByIdNoMtx(_id);
      if(-1 != idx)
      {
         // Found matching event, return it
         r = events[idx];
      }
      else
      {
         // No matching event found
         r = NULL;
      }
   }
   else
   {
      // No pending events
      r = NULL;
   }

   mtx_queue.unlock();

   return r;
}

sSI TKS_Mailbox::findEventIdxNoMtx(void) {
   sUI k = num_queued_events;

   if(k > 0u)
   {
      sUI i = read_off;
      do
      {
         YAC_Event *ev = events[i];
         if(NULL != ev)
         {
            // Found matching event
            return (sSI) i;
         }

         // Check next event, this one has already been read/unlinked
         i = (i + 1u) % max_queue_size;
      }
      while(i != write_off);
   }

   // No matching event found
   return -1;
}

sSI TKS_Mailbox::findEventIdxByIdNoMtx(sSI _id) {
   sUI k = num_queued_events;

   if(k > 0)
   {
      sUI i = read_off;
      do
      {
         YAC_Event *ev = events[i];
         if(NULL != ev) // already read/unlinked ?
         {  
            if(ev->id == _id)
            {
               // Found matching event
               return (sSI) i;
            }
            k--;
         }
   
         // Check next event
         i = (i + 1) % max_queue_size;
      }
      while( (k > 0) && (i != write_off) );
   }

   // No matching event found
   return -1;
}

void TKS_Mailbox::waitEvent(sUI _timeoutMS, YAC_Value *_r) {

   _r->initNull();

   // Check if there is a pending event / available without waiting
   mtx_queue.lock();

   for(;;)
   {
      if(num_queued_events)
      {
         // Unlink (oldest) Event object
         sSI idx = findEventIdxNoMtx();
         if(-1 != idx)
         {
            YAC_Event *ev = events[idx];
            _r->initObject(ev, 1);
            events[idx] = NULL;

            read_off = (idx + 1) % max_queue_size;

            num_queued_events--;

            mtx_queue.unlock();

            // Succeeded
            return;
         }
      }
      mtx_queue.unlock();

      // Wait for new event to arrive
      sSI r = cond_newevent.wait((sSI)_timeoutMS);
      if(r)
      {
         // Timeout or other error occured
         //Dprintf("[...] TKS_Mailbox::waitEvent: condition timed out or failed (r=%d).\n", r);
         return;
      }

      mtx_queue.lock();
   }
}

void TKS_Mailbox::waitEventById(sSI _id, sUI _timeoutMS, YAC_Value *_r) {

   _r->initNull();

   // Check if there is a pending event / available without waiting
   mtx_queue.lock();

   while(1)
   {
      if(num_queued_events)
      {
         sSI idx = findEventIdxByIdNoMtx(_id);
         if(-1 != idx)
         {
            // Unlink matching Event object
            YAC_Event *ev = events[idx];
            _r->initObject(ev, 1);
            events[idx] = NULL;

            if( ((sUI)idx) == read_off)
            {
               read_off = (read_off + 1u) % max_queue_size;
            }
            else
            {
               // Do *not* increase read_off (in order to not lose old events)
            }

            num_queued_events--;

            mtx_queue.unlock();

            // Succeeded
            return;
         }
      }
      mtx_queue.unlock();

      // Wait for new event to arrive
      sSI r = cond_newevent.wait((sSI)_timeoutMS);
      if(r)
      {
         // Timeout or other error occured
         Dprintf("[...] TKS_Mailbox: wait() timed out or failed (r=%d).\n", r);
         return;
      }

      mtx_queue.lock();
   }

}
