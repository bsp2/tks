// ----
// ---- file   : MIDIPipeFrameValueArray.cpp
// ---- author : Bastian Spiegel <bs@tkscript.de>
// ---- legal  : (c) 2014-2025 by Bastian Spiegel.
// ----          Distributed under terms of the GNU LESSER GENERAL PUBLIC LICENSE (LGPL). See
// ----          http://www.gnu.org/licenses/licenses.html#LGPL or COPYING for further information.
// ----
// ---- info   : This is part of the "syntracker" sequencer.
// ----
// ---- created: 01Sep2014
// ---- changed: 03Sep2014, 06Sep2014, 07Sep2014, 08Sep2014, 09Sep2014, 11Jan2015, 16Feb2015
// ----          21Mar2015, 27Feb2016, 28Feb2016, 18Aug2017, 29Mar2019, 25May2019, 26May2019
// ----          30May2019, 24Jun2019, 07Jan2022, 14Jun2023, 06Oct2024, 09Jun2025
// ----
// ----
// ----

#define YAC_EPSILONCOMPARE_ABS         defined
#define YAC_EPSILONCOMPARE_ABS_DEFAULT defined
#include "tkmidipipe.h"

// (note) while MSVC accepts specialized implementations in the header file,
//         GCC complains about multiple definitions during linking.
//         The solution is to add forward declarations / prototypes in the header,
//         and implementations for each possible 'T' in the .cpp file.


// initElements:
#define DinitElements(T)                           \
   template <>                                     \
   void MIDIPipeFrameValueArray<T>::initElements   \
   (T *_v, sUI _num, sBool _bAllowDelete) {        \
      (void)_bAllowDelete;                         \
      memset(_v, 0xFF, sizeof(T) * _num);          \
   }
DinitElements(midipipe_frame_base_t)
DinitElements(midipipe_frame_val_t)
DinitElements(midipipe_frame_ctl_t)
DinitElements(midipipe_frame_note_t)
DinitElements(midipipe_frame_pat_t)
DinitElements(midipipe_frame_sysrt_t)

template <>
void MIDIPipeFrameValueArray<midipipe_frame_sysex_t>::initElements
(midipipe_frame_sysex_t *_v, sUI _num, sBool _bAllowDelete) {
   // // yac_host->printf("xxx MIDIPipeFrameValueArray<midipipe_frame_sysex_t>::initElements: v=%p num=%u bAllowDelete=%i\n", _v, _num, _bAllowDelete);

   if(_bAllowDelete)
   {
      // De-Init (before free)
      for(sUI i = 0u; i < _num; i++)
      {
         _v[i].dev_idx = -1;
         // // if(NULL != _v[i].buffer)
         // //    yac_host->printf("xxx initElements: free buffer[%u] addr=%p\n", i, _v[i].buffer);
         YAC_DELETE_SAFE(_v[i].buffer);
      }
   }
   else
   {
      // Init (after alloc)
      for(sUI i = 0u; i < _num; i++)
      {
         _v[i].dev_idx = -1;
         _v[i].buffer  = NULL;
      }
   }
}

// set:
#define Dset(T)                                       \
   template <>                                        \
   sBool MIDIPipeFrameValueArray<T>::set(T *_value) { \
      T *n = findByType(_value);                      \
      if(NULL != n)                                   \
      {                                               \
         /* Replace */                                \
         *n = *_value;                                \
         return YAC_TRUE;                             \
      }                                               \
      else                                            \
      {                                               \
         /* Add new value */                          \
         if(num_values == max_values)                 \
         {                                            \
            /* Realloc */                             \
            sUI inc = (max_values >> 1);              \
            if(inc < 8u)                              \
            {                                         \
               inc = 8u;                              \
            }                                         \
            else if(inc > 128u)                       \
            {                                         \
               inc = 128u;                            \
            }                                         \
            sUI oldMax = max_values;                  \
            if(realloc(max_values + inc))             \
            {                                         \
               n = &values[oldMax];                   \
            }                                         \
         }                                            \
         else                                         \
         {                                            \
            n = findUnused();                         \
         }                                            \
         if(NULL != n)                                \
         {                                            \
            *n = *_value;                             \
            num_values++;                             \
            return YAC_TRUE;                          \
         }                                            \
      }                                               \
      /* Out of memory */                             \
      return YAC_FALSE;                               \
   }
Dset(midipipe_frame_base_t)
Dset(midipipe_frame_val_t)
Dset(midipipe_frame_ctl_t)
Dset(midipipe_frame_note_t)
Dset(midipipe_frame_pat_t)
Dset(midipipe_frame_sysrt_t)

template <>
sBool MIDIPipeFrameValueArray<midipipe_frame_sysex_t>::set
(midipipe_frame_sysex_t *_value) {
   // yac_host->printf("xxx MIDIPipeFrameValueArray<midipipe_frame_sysex_t>::set: _value=%p _value->buffer=%p\n", _value, _value->buffer);

   // // midipipe_frame_sysex_t *n = findByType(_value);
   midipipe_frame_sysex_t *n = NULL;  // [30May2019] always create new event

   // yac_host->printf("xxx MIDIPipeFrameValueArray<midipipe_frame_sysex_t>::set: n=%p\n", n);

   if(NULL != n)
   {
      // // yac_host->printf("xxx MIDIPipeFrameValueArray<midipipe_frame_sysex_t>::set: n->buffer=%p\n", n->buffer);

      // Replace
      if(NULL != _value->buffer)
      {
         n->dev_idx = _value->dev_idx;
         n->midi_ch = 0;  // ignored but NodeMonitor always display midi channel

         if(NULL == n->buffer)
         {
            // // yac_host->printf("xxx MIDIPipeFrameValueArray<midipipe_frame_sysex_t>::set: new buffer 1\n");
            n->buffer = YAC_New_Buffer();
         }

         // // yac_host->printf("xxx MIDIPipeFrameValueArray<midipipe_frame_sysex_t>::set: REPL: copy value->buffer to array\n");
         // // yac_host->printf("xxx MIDIPipeFrameValueArray<midipipe_frame_sysex_t>::set: REPL: value->buffer->size=%u io_offset=%u\n", _value->buffer->size, _value->buffer->io_offset);
         n->buffer->yacOperatorAssign(_value->buffer);

         n->b_dont_send = _value->b_dont_send;
      }

      return YAC_TRUE;
   }
   else
   {
      // Add new value
      // // yac_host->printf("xxx MIDIPipeFrameValueArray<midipipe_frame_sysex_t>::set: addnew num=%u max=%u\n", num_values, max_values);
      if(num_values == max_values)
      {
         // Realloc
         sUI inc = (max_values >> 1);

         if(inc < 8u)
         {
            inc = 8u;
         }
         else if(inc > 128u)
         {
            inc = 128u;
         }

         sUI oldMax = max_values;

         // // yac_host->printf("xxx MIDIPipeFrameValueArray<midipipe_frame_sysex_t>::set: realloc(%u)\n", (max_values+inc));
         if(realloc(max_values + inc))
         {
            n = &values[oldMax];
         }
      }
      else
      {
         // // yac_host->printf("xxx MIDIPipeFrameValueArray<midipipe_frame_sysex_t>::set: find unused\n");
         n = findUnused();
      }
      // // yac_host->printf("xxx MIDIPipeFrameValueArray<midipipe_frame_sysex_t>::set: n=%p\n", n);

      if(NULL != n)
      {
         if(NULL != _value->buffer)
         {
            n->dev_idx = _value->dev_idx;
            // // yac_host->printf("xxx MIDIPipeFrameValueArray<midipipe_frame_sysex_t>::set: n->buffer=%p\n", n->buffer);
            n->midi_ch = 0;  // ignored but NodeMonitor always display midi channel

            if(NULL == n->buffer)
            {
               // // yac_host->printf("xxx MIDIPipeFrameValueArray<midipipe_frame_sysex_t>::set: new buffer 2\n");
               n->buffer = YAC_New_Buffer();
            }

            // // yac_host->printf("xxx MIDIPipeFrameValueArray<midipipe_frame_sysex_t>::set: ADD: copy value->buffer to array\n");
            // // yac_host->printf("xxx MIDIPipeFrameValueArray<midipipe_frame_sysex_t>::set: ADD: value->buffer->size=%u io_offset=%u\n", _value->buffer->size, _value->buffer->io_offset);

            n->buffer->yacOperatorAssign(_value->buffer);

            // // yac_host->printf("xxx MIDIPipeFrameValueArray<midipipe_frame_sysex_t>::set: ADD: n->buffer->size=%u io_offset=%u\n", n->buffer->size, n->buffer->io_offset);

            n->b_dont_send = _value->b_dont_send;

            num_values++;

            return YAC_TRUE;
         }

      }
   }

   // Out of memory
   return YAC_FALSE;
}


// copyFrom:
#define DcopyFrom(T)                                                    \
   template <>                                                          \
   void MIDIPipeFrameValueArray<T>::copyFrom                            \
   (MIDIPipeFrameValueArray<T> *_o) {                                   \
      if(NULL != _o->values)                                            \
      {                                                                 \
         if(_o->num_values > 0u)                                        \
         {                                                              \
            if(max_values < _o->num_values)                             \
            {                                                           \
               realloc(_o->num_values);                                 \
            }                                                           \
            T *ev = _o->values;                                         \
            T *dev = values;                                            \
            sUI numLeft = _o->num_values;                               \
            for(sUI i = 0u; (i < _o->max_values) && (numLeft > 0u); i++, ev++) \
            {                                                           \
               if(-1 != ev->dev_idx)                                    \
               {                                                        \
                  *dev++ = *ev;                                         \
                  numLeft--;                                            \
               }                                                        \
            }                                                           \
            num_values = _o->num_values;                                \
            initElements(values + num_values, (max_values - num_values)/*num*/, YAC_TRUE/*bAllowDelete*/); \
            return;                                                     \
         }                                                              \
      }                                                                 \
      free();                                                           \
   }

DcopyFrom(midipipe_frame_base_t)
DcopyFrom(midipipe_frame_val_t)
DcopyFrom(midipipe_frame_ctl_t)
DcopyFrom(midipipe_frame_note_t)
DcopyFrom(midipipe_frame_pat_t)
DcopyFrom(midipipe_frame_sysrt_t)

template<> void MIDIPipeFrameValueArray<midipipe_frame_sysex_t>::copyFrom
(MIDIPipeFrameValueArray<midipipe_frame_sysex_t> *_o) {

   // // yac_host->printf("xxx MIDIPipeFrameValueArray<midipipe_frame_sysex_t>::copyFrom\n");

   if(NULL != _o->values)
   {
      if(_o->num_values > 0u)
      {
         if(max_values < _o->num_values)
         {
            realloc(_o->num_values);
         }

         sUI i;
         midipipe_frame_sysex_t *ev = _o->values;
         midipipe_frame_sysex_t *dev = values;
         sUI numLeft = _o->num_values;

         for(i = 0u; (i < _o->max_values) && (numLeft > 0u); i++, ev++)
         {
            if(-1 != ev->dev_idx)
            {
               YAC_Buffer *oldDstBuf = dev->buffer;
               *dev = *ev;
               dev->buffer = oldDstBuf;

               if(NULL == dev->buffer)
               {
                  // // yac_host->printf("xxx MIDIPipeFrameValueArray<midipipe_frame_sysex_t>::copyFrom: new buffer\n");
                  dev->buffer = YAC_New_Buffer();
               }

               dev->buffer->yacOperatorAssign(ev->buffer);

               dev++;
               numLeft--;
            }
         }

         num_values = _o->num_values;

         if(num_values < max_values)
         {
            initElements(values + num_values, (max_values - num_values), YAC_TRUE/*bAllowDelete*/);
         }

         return;
      }
   }

   free();
}


// filter:
template <>
midipipe_frame_ctl_t *MIDIPipeFrameValueArray<midipipe_frame_ctl_t>::filter
(sSI _fltDev, sSI _fltCh, sUI _ctlId) {
   // Find first matching event
   sUI numLeft = num_values;
   sUI i;
   midipipe_frame_ctl_t *ev = values;

   for(i = 0u; (i < max_values) && (numLeft > 0u); i++, ev++)
   {
      if(-1 != ev->dev_idx)
      {
         if(ev->ctl_id == _ctlId)
         {
            if((-1 == _fltDev) || (_fltDev == ev->dev_idx))
            {
               if((-1 == _fltCh) || (_fltCh == sSI(ev->midi_ch)))
               {
                  return ev;
               }
            }
         }
         numLeft--;
      }
   }

   // Not found
   return NULL;
}

// saveElement:
template <>
void MIDIPipeFrameValueArray<midipipe_frame_base_t>::saveElement
(YAC_Object *_ofs,
 const midipipe_frame_base_t *_ev
 ) const {
   _ofs->yacStreamWriteI32(_ev->dev_idx);
   _ofs->yacStreamWriteI8 (_ev->midi_ch);
}

template <>
void MIDIPipeFrameValueArray<midipipe_frame_val_t>::saveElement
(YAC_Object *_ofs,
 const midipipe_frame_val_t *_ev
 ) const {
   reinterpret_cast<const MIDIPipeFrameValueArray<midipipe_frame_base_t>*>(this)->saveElement(_ofs, _ev);

   _ofs->yacStreamWriteF32(_ev->value);
}

template <>
void MIDIPipeFrameValueArray<midipipe_frame_ctl_t>::saveElement
(YAC_Object *_ofs,
 const midipipe_frame_ctl_t *_ev
 ) const {
   reinterpret_cast<const MIDIPipeFrameValueArray<midipipe_frame_val_t>*>(this)->saveElement(_ofs, _ev);

   _ofs->yacStreamWriteI32(_ev->ctl_id);
}

template <>
void MIDIPipeFrameValueArray<midipipe_frame_note_t>::saveElement
(YAC_Object *_ofs,
 const midipipe_frame_note_t *_ev
 ) const {
   reinterpret_cast<const MIDIPipeFrameValueArray<midipipe_frame_base_t>*>(this)->saveElement(_ofs, _ev);

#if 0
   // v2:
   _ofs->yacStreamWriteI8(_ev->note_nr);   // 18Aug2017 (I8 instead of I32)
   _ofs->yacStreamWriteI8(_ev->velocity);  // 18Aug2017 (I8 instead of I32)
   _ofs->yacStreamWriteI32(_ev->duration);
   _ofs->yacStreamWriteI8(_ev->b_pre);     // 18Aug2017
#else
   // v3: [24Jun2019]
   _ofs->yacStreamWriteI8(_ev->note_nr);
   _ofs->yacStreamWriteF32(_ev->velocity);
   _ofs->yacStreamWriteI32(_ev->duration);
   _ofs->yacStreamWriteI8(_ev->b_pre);
#endif
}

template <>
void MIDIPipeFrameValueArray<midipipe_frame_pat_t>::saveElement
(YAC_Object *_ofs,
 const midipipe_frame_pat_t *_ev
 ) const {
   reinterpret_cast<const MIDIPipeFrameValueArray<midipipe_frame_val_t>*>(this)->saveElement(_ofs, _ev);

   _ofs->yacStreamWriteI8(_ev->note_nr);  // 18Aug2017 (I8 instead of I32)
}

template <>
void MIDIPipeFrameValueArray<midipipe_frame_sysex_t>::saveElement
(YAC_Object *_ofs,
 const midipipe_frame_sysex_t *_ev
 ) const {

   _ofs->yacStreamWriteI32(_ev->dev_idx);

   if(NULL != _ev->buffer)
   {
      _ofs->yacStreamWriteI8(YAC_TRUE);
      _ofs->yacStreamWriteObject(_ev->buffer);
   }
   else
   {
      _ofs->yacStreamWriteI8(YAC_FALSE);
   }
}

template <>
void MIDIPipeFrameValueArray<midipipe_frame_sysrt_t>::saveElement
(YAC_Object *_ofs,
 const midipipe_frame_sysrt_t *_ev
 ) const {
   reinterpret_cast<const MIDIPipeFrameValueArray<midipipe_frame_base_t>*>(this)->saveElement(_ofs, _ev);

   _ofs->yacStreamWriteI8(_ev->sysrt_byte);
}


// loadElement:
template <>
void MIDIPipeFrameValueArray<midipipe_frame_base_t>::loadElement
(YAC_Object *_ifs,
 midipipe_frame_base_t *_ev,
 sUI _ver
 ) {
   (void)_ver;

   _ev->dev_idx     = _ifs->yacStreamReadI32();
   _ev->midi_ch     = _ifs->yacStreamReadI8 ();
   _ev->b_dont_send = YAC_FALSE;

   // // yac_host->printf("midipipe_frame_base_t::loadElement: dev_idx=%d, midi_ch=%d\n", _ev->dev_idx, _ev->midi_ch);
}

template <>
void MIDIPipeFrameValueArray<midipipe_frame_val_t>::loadElement
(YAC_Object *_ifs,
 midipipe_frame_val_t *_ev,
 sUI _ver
 ) {
   reinterpret_cast<MIDIPipeFrameValueArray<midipipe_frame_base_t>*>(this)->loadElement(_ifs, _ev, _ver);

   if(_ver >= 3)
   {
      // v3: [24Jun2019]
      _ev->value = _ifs->yacStreamReadF32();
   }
   else
   {
      _ev->value = (sF32)_ifs->yacStreamReadI32();
   }

   // // yac_host->printf("midipipe_frame_val_t::loadElement: value=%d\n", _ev->value);
}

template <>
void MIDIPipeFrameValueArray<midipipe_frame_ctl_t>::loadElement
(YAC_Object *_ifs,
 midipipe_frame_ctl_t *_ev,
 sUI _ver
 ) {
   reinterpret_cast<MIDIPipeFrameValueArray<midipipe_frame_val_t>*>(this)->loadElement(_ifs, _ev, _ver);

   (void)_ver;

   _ev->ctl_id = _ifs->yacStreamReadI32();

   // // yac_host->printf("midipipe_frame_ctl_t::loadElement: ctl_id=%d\n", _ev->ctl_id);
}

template <>
void MIDIPipeFrameValueArray<midipipe_frame_note_t>::loadElement
(YAC_Object *_ifs,
 midipipe_frame_note_t *_ev,
 sUI _ver
 ) {
   reinterpret_cast<MIDIPipeFrameValueArray<midipipe_frame_base_t>*>(this)->loadElement(_ifs, _ev, _ver);

   if(_ver >= 3u)
   {
      _ev->note_nr  = _ifs->yacStreamReadI8();
      _ev->velocity = _ifs->yacStreamReadF32();
      _ev->duration = _ifs->yacStreamReadI32();
      _ev->b_pre    = (sBool)_ifs->yacStreamReadI8();
   }
   else if(_ver >= 2u)
   {
      _ev->note_nr  = (sSI)_ifs->yacStreamReadI8();
      _ev->velocity = (sF32)_ifs->yacStreamReadI8();
      _ev->duration = _ifs->yacStreamReadI32();
      _ev->b_pre    = (sBool)_ifs->yacStreamReadI8();
   }
   else
   {
      _ev->note_nr  = _ifs->yacStreamReadI32();
      _ev->velocity = (sF32)_ifs->yacStreamReadI32();
      _ev->duration = _ifs->yacStreamReadI32();
      _ev->b_pre = YAC_FALSE;
   }


   // // yac_host->printf("midipipe_frame_note_t::loadElement: note_nr=%d velocity=%d duration=%d\n", _ev->note_nr, _ev->velocity, _ev->duration);
}

template <>
void MIDIPipeFrameValueArray<midipipe_frame_pat_t>::loadElement
(YAC_Object *_ifs,
 midipipe_frame_pat_t *_ev,
 sUI _ver
 ) {
   reinterpret_cast<MIDIPipeFrameValueArray<midipipe_frame_val_t>*>(this)->loadElement(_ifs, _ev, _ver);

   if(_ver >= 2u)
   {
      _ev->note_nr  = (sUI)_ifs->yacStreamReadI8();
   }
   else
   {
      _ev->note_nr  = _ifs->yacStreamReadI32();
   }

   // // yac_host->printf("midipipe_frame_pat_t::loadElement: note_nr=%d pressure=%d\n", _ev->note_nr, _ev->pressure);
}

template <>
void MIDIPipeFrameValueArray<midipipe_frame_sysex_t>::loadElement
(YAC_Object *_ifs,
 midipipe_frame_sysex_t *_ev,
 sUI _ver
 ) {
   (void)_ver;

   _ev->dev_idx     = _ifs->yacStreamReadI32();
   _ev->midi_ch     = 0u;  // ignored
   _ev->b_dont_send = YAC_FALSE;

   sBool bHaveBuffer = (YAC_FALSE != _ifs->yacStreamReadI8());
   // // yac_host->printf("xxx loadElement<sysex>: ver=%u dev_idx=%d bHaveBuffer=%d\n", _ver, _ev->dev_idx, bHaveBuffer);
   if(bHaveBuffer)
   {
      if(NULL == _ev->buffer)
      {
         // // yac_host->printf("xxx loadElement: sysex new buffer\n");
         _ev->buffer = YAC_New_Buffer();
      }

      _ifs->yacStreamReadObject(_ev->buffer);
      _ev->buffer->io_offset = _ev->buffer->size;  // emit() sends 'io_offset' bytes (F0/F7)
   }
   else
   {
      YAC_DELETE_SAFE(_ev->buffer);
   }

   // // yac_host->printf("midipipe_frame_sysex_t::loadElement: dev_idx=%d\n", _ev->dev_idx);
}

template <>
void MIDIPipeFrameValueArray<midipipe_frame_sysrt_t>::loadElement
(YAC_Object *_ifs,
 midipipe_frame_sysrt_t *_ev,
 sUI _ver
 ) {
   reinterpret_cast<MIDIPipeFrameValueArray<midipipe_frame_base_t>*>(this)->loadElement(_ifs, _ev, _ver);

   _ev->sysrt_byte = _ifs->yacStreamReadI8();
}


// mergeArrayTranspose:
template <>
void MIDIPipeFrameValueArray<midipipe_frame_note_t>::mergeArrayTranspose
(MIDIPipeFrameValueArray<midipipe_frame_note_t> *_o,
 sSI _transpose,
 sF32 _durationScale
 )
{
   if(NULL != _o->values)
   {
      if(_o->num_values > 0u)
      {
         sUI i;
         midipipe_frame_note_t *ev = _o->values;
         sUI numLeft = _o->num_values;

         for(i = 0u; (i < _o->max_values) && (numLeft > 0u); i++, ev++)
         {
            if(-1 != ev->dev_idx)
            {
               midipipe_frame_note_t trEv = *ev;

               trEv.note_nr += _transpose;

               if(trEv.note_nr < 128.0f) // also checks unsigned wrap-around
               {
                  if(0u != trEv.duration)
                  {
                     trEv.duration = (sUI) (trEv.duration * _durationScale);

                     if(trEv.duration < 1u)
                     {
                        trEv.duration = 1u;
                     }
                  }

                  set(&trEv);
               }
               numLeft--;
            }
         }
      }
   }
}

template <>
void MIDIPipeFrameValueArray<midipipe_frame_pat_t>::mergeArrayTranspose
(MIDIPipeFrameValueArray<midipipe_frame_pat_t> *_o,
 sSI  _transpose,
 sF32 _durationScale
 )
{
   (void)_durationScale;

   if(NULL != _o->values)
   {
      if(_o->num_values > 0u)
      {
         sUI i;
         midipipe_frame_pat_t *ev = _o->values;
         sUI numLeft = _o->num_values;

         for(i = 0u; (i < _o->max_values) && (numLeft > 0u); i++, ev++)
         {
            if(-1 != ev->dev_idx)
            {
               midipipe_frame_pat_t trEv = *ev;

               trEv.note_nr += _transpose;

               if(trEv.note_nr < 128) // also checks unsigned wrap-around
               {
                  set(&trEv);
               }
               numLeft--;
            }
         }
      }
   }
}


// deleteByFltAndNr:
template <>
sUI MIDIPipeFrameValueArray<midipipe_frame_ctl_t>::deleteByFltAndNr
(sSI _devIdx, sSI _fltCh, sUI _nr)
{
   sUI r = 0u;

   if(NULL != values)
   {
      midipipe_frame_ctl_t *ev = values;
      sUI numLeft = num_values;
      sUI i;

      for(i = 0u; (i < max_values) && (numLeft > 0u); i++, ev++)
      {
         if(-1 != ev->dev_idx)
         {
            if(ev->ctl_id == _nr)
            {
               if((-1 == _devIdx) || (_devIdx == ev->dev_idx))
               {
                  if((-1 == _fltCh) || (sSI(ev->midi_ch) == _fltCh))
                  {
                     // Discard event
                     ev->dev_idx = -1;
                     num_values--;
                     r++;
                  }
               }
            }

            numLeft--;
         }
      }
   }

   return r;
}

template <>
sBool MIDIPipeFrameValueArray<midipipe_frame_base_t>::isStateDifferentFrom
(const MIDIPipeFrameValueArray<midipipe_frame_base_t> *_o)
{
   sBool bDiffers = YAC_FALSE;

   sUI numLeft = num_values;

   if(numLeft > 0u)
   {
      midipipe_frame_base_t *ev = values;

      for(sUI i = 0u; (i < max_values) && (numLeft > 0u); i++, ev++)
      {
         if(-1 != ev->dev_idx)
         {
            midipipe_frame_base_t *otherEv = _o->values;
            sUI otherNumLeft = _o->num_values;

            sBool bFound = YAC_FALSE;

            for(sUI io = 0u; (i < _o->max_values) && (otherNumLeft > 0u); io++, otherEv++)
            {
               (void)io;
               if(-1 != otherEv->dev_idx)
               {
                  if(otherEv->dev_idx == ev->dev_idx)
                  {
                     if(otherEv->midi_ch == ev->midi_ch)
                     {
                        // Event found in other array
                        bFound = YAC_TRUE;
                        break;
                     }
                  }

                  otherNumLeft--;
               }
            }

            bDiffers |= !bFound;

            if(bDiffers)
            {
               // Event does not exist in other frame
               break;
            }
            else
            {
               numLeft--;
            }
         }
      }
   }

   return bDiffers;
}

template <>
sBool MIDIPipeFrameValueArray<midipipe_frame_val_t>::isStateDifferentFrom
(const MIDIPipeFrameValueArray<midipipe_frame_val_t> *_o)
{
   sBool bDiffers = YAC_FALSE;

   sUI numLeft = num_values;

   if(numLeft > 0u)
   {
      midipipe_frame_val_t *ev = values;

      for(sUI i = 0u; (i < max_values) && (numLeft > 0u); i++, ev++)
      {
         if(-1 != ev->dev_idx)
         {
            midipipe_frame_val_t *otherEv = _o->values;
            sUI otherNumLeft = _o->num_values;

            sBool bFound = YAC_FALSE;

            for(sUI io = 0u; (i < _o->max_values) && (otherNumLeft > 0u); io++, otherEv++)
            {
               (void)io;
               if(-1 != otherEv->dev_idx)
               {
                  if(otherEv->dev_idx == ev->dev_idx)
                  {
                     if(otherEv->midi_ch == ev->midi_ch)
                     {
                        if(Dfltequal(otherEv->value, ev->value))
                        {
                           // Value found in other array
                           bFound = YAC_TRUE;
                           break;
                        }
                     }
                  }

                  otherNumLeft--;
               }
            }

            bDiffers |= !bFound;

            if(bDiffers)
            {
               // Ctl does not exist in other frame
               break;
            }
            else
            {
               numLeft--;
            }
         }
      }
   }

   return bDiffers;
}

template <>
sBool MIDIPipeFrameValueArray<midipipe_frame_ctl_t>::isStateDifferentFrom
(const MIDIPipeFrameValueArray<midipipe_frame_ctl_t> *_o)
{
   sBool bDiffers = YAC_FALSE;

   sUI numLeft = num_values;

   if(numLeft > 0u)
   {
      midipipe_frame_ctl_t *ev = values;

      for(sUI i = 0u; (i < max_values) && (numLeft > 0u); i++, ev++)
      {
         if(-1 != ev->dev_idx)
         {
            midipipe_frame_ctl_t *otherEv = _o->values;
            sUI otherNumLeft = _o->num_values;

            sBool bFound = YAC_FALSE;

            for(sUI io = 0u; (i < _o->max_values) && (otherNumLeft > 0u); io++, otherEv++)
            {
               (void)io;
               if(-1 != otherEv->dev_idx)
               {
                  if(otherEv->dev_idx == ev->dev_idx)
                  {
                     if(otherEv->midi_ch == ev->midi_ch)
                     {
                        if(otherEv->ctl_id == ev->ctl_id)
                        {
                           if(Dfltequal(otherEv->value, ev->value))
                           {
                              // Ctl value found in other array
                              bFound = YAC_TRUE;
                              break;
                           }
                        }
                     }
                  }

                  otherNumLeft--;
               }
            }

            bDiffers |= !bFound;

            if(bDiffers)
            {
               // Ctl does not exist in other frame
               break;
            }
            else
            {
               numLeft--;
            }
         }
      }
   }

   return bDiffers;
}

template <>
sBool MIDIPipeFrameValueArray<midipipe_frame_pat_t>::isStateDifferentFrom
(const MIDIPipeFrameValueArray<midipipe_frame_pat_t> *_o)
{
   sBool bDiffers = YAC_FALSE;

   sUI numLeft = num_values;

   if(numLeft > 0u)
   {
      midipipe_frame_pat_t *ev = values;

      for(sUI i = 0u; (i < max_values) && (numLeft > 0u); i++, ev++)
      {
         if(-1 != ev->dev_idx)
         {
            midipipe_frame_pat_t *otherEv = _o->values;
            sUI otherNumLeft = _o->num_values;

            sBool bFound = YAC_FALSE;

            for(sUI io = 0u; (i < _o->max_values) && (otherNumLeft > 0u); io++, otherEv++)
            {
               (void)io;
               if(-1 != otherEv->dev_idx)
               {
                  if(otherEv->dev_idx == ev->dev_idx)
                  {
                     if(otherEv->midi_ch == ev->midi_ch)
                     {
                        if(otherEv->note_nr == ev->note_nr)
                        {
                           if(Dfltequal(otherEv->value, ev->value))
                           {
                              // Polyphonic pressure value found in other array
                              bFound = YAC_TRUE;
                              break;
                           }
                        }
                     }
                  }

                  otherNumLeft--;
               }
            }

            bDiffers |= !bFound;

            if(bDiffers)
            {
               // Ctl does not exist in other frame
               break;
            }
            else
            {
               numLeft--;
            }
         }
      }
   }

   return bDiffers;
}
