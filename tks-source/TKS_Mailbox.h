/// TKS_Mailbox.h
///
/// (c) 2009-2023 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///
#ifndef __TKS_MAILBOX_H__
#define __TKS_MAILBOX_H__


class TKS_Mailbox : public YAC_Object {
public:
   YAC_Event **events;
   sUI         max_queue_size;
   sUI         num_queued_events;
   sUI         write_off;
   sUI         read_off;

   TKS_Condition cond_newevent; // used to signal arrival of new event
   TKS_Mutex     mtx_queue;     // protects access to event queue

public:
   TKS_Mailbox(void);
   ~TKS_Mailbox();

   void init (void);

   // thread-safe way to query the number of queued events
   sUI getNumQueuedEvents (void);

   // discard all events
   void freeEventQueue (void);

   // discard events and allocate new (empty) queue
   void allocEventQueue (sUI _size);

   // create a copy of _event (deref value) and add to queue
   sSI  sendEvent (YAC_Event *_event);
   sSI  _sendEvent (YAC_Object *_event);

   // return reference to oldest event without removing it from the queue
   YAC_Event *peekEvent (void);
   // return reference to oldest event that matches the given id
   YAC_Event *peekEventById (sSI _id);

   // find index of next pending event (skip events that have already been read)
   sSI findEventIdxNoMtx (void);

   // find index of pending event whose id matches _id. do not lock "mtx_queue"
   sSI findEventIdxByIdNoMtx (sSI _id);

   // wait until next event arrives or until timeout expires, return null or next event
   void waitEvent (sUI _timeoutMS, YAC_Value *_r);
   void waitEventById (sSI _id, sUI _timeoutMS, YAC_Value *_r);

};


#endif // __TKS_MAILBOX_H__
