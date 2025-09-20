// ----
// ---- file   : MIDIPipeFrameValueArray.h
// ---- author : Bastian Spiegel <bs@tkscript.de>
// ---- legal  : (c) 2014-2025 by Bastian Spiegel.
// ----          Distributed under terms of the GNU LESSER GENERAL PUBLIC LICENSE (LGPL). See
// ----          http://www.gnu.org/licenses/licenses.html#LGPL or COPYING for further information.
// ----
// ---- info   : This is part of the "syntracker" sequencer.
// ----
// ---- created: 01Sep2014
// ---- changed: 03Sep2014, 09Sep2014, 26Sep2014, 11Jan2015, 14Feb2015, 16Feb2015, 21Mar2015
// ----          14Apr2015, 15May2015, 30Jun2015, 02Nov2015, 27Feb2016, 28Feb2016, 03Mar2017
// ----          18Aug2017, 21Aug2017, 15Apr2018, 02Jun2018, 29Mar2019, 01May2019, 25May2019
// ----          26May2019, 30May2019, 24Jun2019, 08Nov2020, 07Jan2022, 06Oct2024, 09Jun2025
// ----          13Jun2025
// ----
// ----
// ----

#ifndef TK_MIDIPIPEFRAMEVALUEARRAY_H__
#define TK_MIDIPIPEFRAMEVALUEARRAY_H__


template <class T>
class MIDIPipeFrameValueArray {
  public:
   T *values;
   sUI num_values;
   sUI max_values;

   MIDIPipeFrameValueArray(void) {
      values     = NULL;
      num_values = 0u;
      max_values = 0u;
   }

   ~MIDIPipeFrameValueArray() {
      free();
   }

   // Init or de-init elements
   void initElements(T *_v, sUI _num, sBool _bAllowDelete);

   void free(void) {

      /* yac_host->printf("xxx MIDIPipeFrameValueArray::free\n"); */

      if(NULL != values)
      {
         initElements(values, max_values, YAC_TRUE/*bAllowDelete*/);
         delete [] values;
         values     = NULL;
         num_values = 0u;
         max_values = 0u;
      }
   }

   sBool alloc(sUI _maxValues) {
      /* yac_host->printf("xxx MIDIPipeFrameValueArray::alloc(max=%u)\n", _maxValues); */

      free();

      values = new T[_maxValues];

      if(NULL != values)
      {
         max_values = _maxValues;
         num_values = 0u;

         initElements(values, max_values, YAC_FALSE/*bAllowDelete*/);

         return YAC_TRUE;
      }
      else
      {
         // Out of memory
         return YAC_FALSE;
      }
   }

   sBool realloc(sUI _maxValues) {

      /* yac_host->printf("xxx MIDIPipeFrameValueArray::realloc(cMax=%u, nMax=%u)\n", max_values, _maxValues); */

      T *nv = new T[_maxValues];

      if(NULL != nv)
      {
         // Copy old elements
         if(NULL != values)
         {
            sUI copyNum = (_maxValues > max_values) ? max_values : _maxValues;
            memcpy(nv, values, sizeof(T) * copyNum);
         }

         if(_maxValues > max_values)
         {
            // Init new elements
            initElements(nv + max_values, (_maxValues - max_values)/*num*/, YAC_FALSE/*bAllowDelete*/);
         }
         else if(_maxValues < max_values)
         {
            // De-init old elements (unused now)
            initElements(values + _maxValues, (max_values - _maxValues)/*num*/, YAC_TRUE/*bAllowDelete*/);
         }

         if(NULL != values)
         {
            delete [] values;
         }

         values     = nv;
         max_values = _maxValues;

         return YAC_TRUE;
      }
      else
      {
         // Out of memory
         return YAC_FALSE;
      }
   }

   T *findUnused(void) const {
      if(num_values < max_values)
      {
         for(sUI i = 0u; i < max_values; i++)
         {
            if(-1 == values[i].dev_idx)
            {
               return &values[i];
            }
         }
      }
      return NULL;
   }

   T *findByType(const T *_value) const {
      sUI numLeft = num_values;
      /* yac_host->printf("xxx findByType numLeft=%u\n", numLeft); */
      for(sUI i = 0u; (i < max_values) && (numLeft > 0u); i++)
      {
         if(-1 != values[i].dev_idx)
         {
            if(values[i].compareType(_value))
            {
               return &values[i];
            }
            numLeft--;
         }
      }
      return NULL;
   }

   sBool set(T *_value);

   sBool unset(T *_value) {
      T *n = findByType(_value);

      if(NULL != n)
      {
         // Mark as unused
         n->dev_idx = -1;

         num_values--;

         return YAC_TRUE;
      }
      else
      {
         // Not found
         return YAC_FALSE;
      }
   }

   void optimize(void) {

      /* yac_host->printf("xxx MIDIPipeFrameValueArray::optimize\n"); */

      if(NULL != values)
      {
         T *tr = values;
         T *tw = values;
         sUI numLeft = num_values;
         for(sUI i = 0u; i < max_values && (numLeft > 0u); i++, tr++)
         {
            if(-1 != tr->dev_idx)
            {
               *tw++ = *tr;
               numLeft--;
            }
         }

         initElements(values + num_values, (max_values - num_values)/*num*/, YAC_FALSE/*bAllowDelete*/);
      }
   }

   void empty(void) {
      num_values = 0u;

      initElements(values, max_values, YAC_TRUE/*bAllowDelete*/);
   }

   void updateDontSendFlags(sBool _bDontSend) {
      sUI numLeft = num_values;
      for(sUI i = 0u; (i < max_values) && (numLeft > 0u); i++)
      {
         if(-1 != values[i].dev_idx)
         {
            values[i].b_dont_send = _bDontSend;
            numLeft--;
         }
      }
   }

   sUI getNumEventsByFlt(sSI _fltDev, sSI _fltCh) const {
      sUI r = 0u;
      sUI numLeft = num_values;
      for(sUI i = 0u; (i < max_values) && (numLeft > 0u); i++)
      {
         if(-1 != values[i].dev_idx)
         {
            if( (-1 == _fltDev) || (values[i].dev_idx == _fltDev) )
            {
               if( (-1 == _fltCh) || (sSI(values[i].midi_ch) == _fltCh) )
               {
                  r++;
               }
            }
            numLeft--;
         }
      }
      return r;
   }

   void mergeArray(MIDIPipeFrameValueArray<T> *_o) {

      if(NULL != _o->values)
      {
         if(_o->num_values > 0u)
         {
            T *ev = _o->values;
            sUI numLeft = _o->num_values;

            for(sUI i = 0u; (i < _o->max_values) && (numLeft > 0u); i++, ev++)
            {
               if(-1 != ev->dev_idx)
               {
                  set(ev);
                  numLeft--;
               }
            }
         }
      }
   }

   void mergeArrayFltDst(MIDIPipeFrameValueArray<T> *_o, sSI _fltDev, sSI _fltCh, sSI _dstDev, sSI _dstCh) {

      if(NULL != _o->values)
      {
         if(_o->num_values > 0u)
         {
            sUI i;
            T *ev = _o->values;
            sUI numLeft = _o->num_values;

            for(i = 0u; (i < _o->max_values) && (numLeft > 0u); i++, ev++)
            {
               if(-1 != ev->dev_idx)
               {
                  if( (-1 == _fltDev) || (_fltDev == ev->dev_idx) )
                  {
                     if( (-1 == _fltCh) || (_fltCh == sSI(ev->midi_ch)) )
                     {
                        T evDst = *ev;

                        if(-1 != _dstDev)
                        {
                           evDst.dev_idx = _dstDev;
                        }
                        // else: keep output device

                        if(-1 != _dstCh)
                        {
                           evDst.midi_ch = _dstCh;
                        }
                        // else: keep output channel

                        set(&evDst);
                     }
                  }

                  numLeft--;
               }
            }
         }
      }
   }

   void mergeArrayFlt(MIDIPipeFrameValueArray<T> *_o, sSI _fltDev, sSI _fltCh) {

      if(NULL != _o->values)
      {
         if(_o->num_values > 0u)
         {
            sUI i;
            T *ev = _o->values;
            sUI numLeft = _o->num_values;

            for(i = 0u; (i < _o->max_values) && (numLeft > 0u); i++, ev++)
            {
               if(-1 != ev->dev_idx)
               {
                  if( (-1 == _fltDev) || (_fltDev == ev->dev_idx) )
                  {
                     if( (-1 == _fltCh) || (_fltCh == sSI(ev->midi_ch)) )
                     {
                        T evDst = *ev;

                        set(&evDst);
                     }
                  }

                  numLeft--;
               }
            }
         }
      }
   }

   void copyFrom(MIDIPipeFrameValueArray<T> *_o);

   T *getByUsedIdx(sUI _idx) const {
      if(NULL != values)
      {
         sUI numLeft = num_values;
         T *ev = values;
         sUI usedIdx = 0u;
         for(sUI i = 0u; (i < max_values) && (numLeft > 0u); i++, ev++)
         {
            if(-1 != ev->dev_idx)
            {
               if(usedIdx == _idx)
               {
                  return ev;
               }

               usedIdx++;
               numLeft--;
            }
         }
      }
      return NULL;
   }

   T *getByUsedIdxAndFlt(sUI _idx, sSI _fltDev, sSI _fltCh, sSI _globalCh, sUI &_retNum) const {
      if(NULL != values)
      {
         sUI numLeft = num_values;
         T *ev = values;
         sUI usedIdx = 0u;
         for(sUI i = 0u; (i < max_values) && (numLeft > 0u); i++, ev++)
         {
            if(-1 != ev->dev_idx)
            {
               if((-1 == _fltDev) || (_fltDev == ev->dev_idx))
               {
                  if((-1 == _fltCh) || (_fltCh == sSI(ev->midi_ch)) || (_globalCh == sSI(ev->midi_ch)))
                  {
                     if(usedIdx == _idx)
                     {
                        _retNum = usedIdx + 1; // not relevant since ev was found
                        return ev;
                     }

                     usedIdx++;
                  }
               }

               numLeft--;
            }
         }

         _retNum = usedIdx;
      }
      else
      {
         _retNum = 0u;
      }
      return NULL;
   }

   void setDevIdxAndMidiCh(sSI _devIdx, sSI _midiCh) {
      if(NULL != values)
      {
         sUI numLeft = num_values;
         T *ev = values;
         for(sUI i = 0u; (i < max_values) && (numLeft > 0u); i++, ev++)
         {
            if(-1 != ev->dev_idx)
            {
               ev->dev_idx = _devIdx;
               ev->midi_ch = _midiCh;
               numLeft--;
            }
         }
      }
   }

   void saveElement(YAC_Object *_ofs, const T *_ev) const;

   void saveState(YAC_Object *_ofs) const {

      // Write version
      //  (note) v2: save note/vel as byte, save b_pre note off flag
      //  (note) v3: save all values (including vel) as floats, save b_pre note off flag
      _ofs->yacStreamWriteI16(3);

      // Write num_values
      _ofs->yacStreamWriteI32(num_values);

      if(NULL != values)
      {
         sUI numLeft = num_values;
         T *ev = values;
         for(sUI i = 0u; (i < max_values) && (numLeft > 0u); i++, ev++)
         {
            if(-1 != ev->dev_idx)
            {
               saveElement(_ofs, ev);

               numLeft--;
            }
         }
      }

   }

   void loadElement(YAC_Object *_ifs, T *_ev, sUI _ver);

   sBool loadState(YAC_Object *_ifs) {

      free();

      sU16 ver = _ifs->yacStreamReadI16();

      if(ver >= 1)
      {
         // Read num_values
         sU32 numVal = _ifs->yacStreamReadI32();

         if(numVal > 0u)
         {
            if(alloc(numVal))
            {
               sUI i;
               T *ev = values;

               for(i = 0u; i < numVal; i++, ev++)
               {
                  loadElement(_ifs, ev, ver);
               }

               num_values = numVal;

               return YAC_TRUE;
            }
            else
            {
               return YAC_FALSE;
            }
         }

         return YAC_TRUE;
      }

      return YAC_FALSE;
   }

   void remapDeviceIndices(const sSI *_devIdxMap, sUI _mapSize, sSI *_outDevUseCount) {

      if(NULL != values)
      {
         sUI numLeft = num_values;
         T *ev = values;
         for(sUI i = 0u; (i < max_values) && (numLeft > 0u); i++, ev++)
         {
            if(-1 != ev->dev_idx)
            {
               if((sUI)ev->dev_idx < _mapSize)
               {
                  sSI newIdx = _devIdxMap[ev->dev_idx];

                  _outDevUseCount[ev->dev_idx]++;

                  if(-1 == newIdx)
                  {
                     // Discard event
                     ev->dev_idx = -1;
                     num_values--;
                  }
                  else
                  {
                     // Remap to new MIDI out_devices index
                     ev->dev_idx = newIdx;
                  }
               }
               else
               {
                  // This should never be reached (see MIDI.SongFileReader_BuildOutDevIdxMap())

                  yac_host->printf("[---] remapDeviceIndices: oops. dev_idx=%d, mapSize=%u\n",
                                   ev->dev_idx, _mapSize
                                   );

                  if(_mapSize > 0)
                  {
                     ev->dev_idx = 0;
                  }
                  else
                  {
                     // Discard event
                     ev->dev_idx = -1;
                     num_values--;
                  }
               }

               numLeft--;
            }
         }
      }

   }

   sUI replaceDevIdxAndMidiCh(sSI _fltDevIdx, sSI _fltCh, sSI _devIdx, sSI _ch) {
      sUI r = 0u;

      if(NULL != values)
      {
         sUI numLeft = num_values;
         T *ev = values;
         for(sUI i = 0u; (i < max_values) && (numLeft > 0u); i++, ev++)
         {
            if(-1 != ev->dev_idx)
            {
               r += ev->replaceDevIdxAndMidiCh(_fltDevIdx, _fltCh, _devIdx, _ch);

               numLeft--;
            }
         }
      }

      return r;
   }

   void mergeArrayTranspose(MIDIPipeFrameValueArray<T> *_o,
                            sSI  _transpose,
                            sF32 _durationScale
                            );

   T *filter(sSI _fltDev, sSI _fltCh) {
      sUI numLeft = num_values;
      T *ev = values;
      for(sUI i = 0u; (i < max_values) && (numLeft > 0u); i++, ev++)
      {
         if(-1 != ev->dev_idx)
         {
            if((-1 == _fltDev) || (_fltDev == ev->dev_idx))
            {
               if((-1 == _fltCh) || (_fltCh == sSI(ev->midi_ch)))
               {
                  return ev;
               }
            }
            numLeft--;
         }
      }

      // Not found
      return NULL;
   }

   sUI deleteByFlt(sSI _devIdx, sSI _fltCh) {
      sUI r = 0;

      if(NULL != values)
      {
         T *ev = values;
         sUI numLeft = num_values;
         for(sUI i = 0u; (i < max_values) && (numLeft > 0u); i++, ev++)
         {
            if(-1 != ev->dev_idx)
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

               numLeft--;
            }
         }
      }

      return r;
   }

   sUI keepByFlt(sSI _devIdx, sSI _fltCh) {
      sUI r = 0u;

      if(NULL != values)
      {
         T *ev = values;
         sUI numLeft = num_values;
         for(sUI i = 0u; (i < max_values) && (numLeft > 0u); i++, ev++)
         {
            if(-1 != ev->dev_idx)
            {
               if(!(
                     ((-1 == _devIdx) || (_devIdx == ev->dev_idx)) &&
                     ((-1 == _fltCh) || (sSI(ev->midi_ch) == _fltCh))
                    )
                  )
               {
                  // Discard event
                  ev->dev_idx = -1;
                  num_values--;
                  r++;
               }

               numLeft--;
            }
         }
      }

      return r;
   }

   // Implemented in specialized classes
   T *filter (sSI _fltDev, sSI _fltCh, sUI _ctlId);

   sUI deleteByFltAndNr (sSI _devIdx, sSI _fltCh, sUI _nr);

   sBool isStateDifferentFrom (const MIDIPipeFrameValueArray<T> *_o);
};



class midipipe_frame_base_t {
  public:
   sSI   dev_idx;  // -1 = unused, or 0..out_devices.numElements-1
   sUI   midi_ch;  // 0..15
   sBool b_dont_send;

   //midipipe_frame_base_t(void) { }
   midipipe_frame_base_t() = default;

   midipipe_frame_base_t(sSI _devIdx, sUI _midiCh) {
      dev_idx = _devIdx;
      midi_ch = _midiCh;
      b_dont_send = YAC_FALSE;
   }

   sBool compareType(const midipipe_frame_base_t *_o) const {
      return
         (dev_idx == _o->dev_idx) &&
         (midi_ch == _o->midi_ch)
         ;
   }

   sUI replaceDevIdxAndMidiCh(sSI _fltDevIdx, sSI _fltCh, sSI _devIdx, sSI _ch) {
      sUI r = 0u;

      if( (-1 == _fltDevIdx) || (dev_idx == _fltDevIdx) )
      {
         if( (-1 == _fltCh) || (sSI(midi_ch) == _fltCh) )
         {
            dev_idx = _devIdx;
            if(_ch >= 0)
               midi_ch = _ch;
            r = 1u;
         }
      }

      return r;
   }
};


class midipipe_frame_val_t : public midipipe_frame_base_t {
   // prg, pressure, or pitchbend or base class or CC/NRPN
  public:
   sF32 value;  // 0..127 or 0..16383 (changed type to float in v3)

   midipipe_frame_val_t() = default;
   midipipe_frame_val_t(sSI _devIdx, sUI _midiCh) : midipipe_frame_base_t(_devIdx, _midiCh) { }

   sBool compareType(const midipipe_frame_val_t *_o) const {
      return
         (dev_idx == _o->dev_idx) &&
         (midi_ch == _o->midi_ch)
         ;
   }
};


class midipipe_frame_ctl_t : public midipipe_frame_val_t {
  public:
   sUI  ctl_id;   // CC or NRPN #

   midipipe_frame_ctl_t() = default;
   midipipe_frame_ctl_t(sSI _devIdx, sUI _midiCh) : midipipe_frame_val_t(_devIdx, _midiCh) { }
   midipipe_frame_ctl_t(sSI _devIdx, sUI _midiCh, sUI _ctlId) : midipipe_frame_val_t(_devIdx, _midiCh) {
      ctl_id = _ctlId;
   }

   sBool compareType(const midipipe_frame_ctl_t *_o) const {
      return
         (dev_idx == _o->dev_idx) &&
         (midi_ch == _o->midi_ch) &&
         (ctl_id  == _o->ctl_id)
         ;
   }

};


class midipipe_frame_note_t : public midipipe_frame_base_t {
  public:
   sSI   note_nr;  // 0..127 (changed to float in v3)
   sF32  velocity; // 0..127 (changed to float in v3)
   sUI   duration; // 0=manual noteoff, #ticks otherwise
   sBool b_pre;    // note off mode: 1=pre note on, 0=post note on

   midipipe_frame_note_t() = default;
   midipipe_frame_note_t(sSI _devIdx, sUI _midiCh) : midipipe_frame_base_t(_devIdx, _midiCh) { }
   midipipe_frame_note_t(sSI _devIdx, sUI _midiCh, sSI _noteNr) : midipipe_frame_base_t(_devIdx, _midiCh) {
      note_nr = _noteNr;
      b_pre   = 0;
   }

   sBool compareType(const midipipe_frame_note_t *_o) const {
      return
         (dev_idx == _o->dev_idx) &&
         (midi_ch == _o->midi_ch) &&
         (note_nr == _o->note_nr)
         ;
   }

};


class midipipe_frame_pat_t : public midipipe_frame_val_t {
   // Polyphonic pressure (polyat)
  public:
   sUI   note_nr;  // 0..127

   midipipe_frame_pat_t() = default;
   midipipe_frame_pat_t(sSI _devIdx, sUI _midiCh) : midipipe_frame_val_t(_devIdx, _midiCh) { }

   sBool compareType(const midipipe_frame_pat_t *_o) const {
      /* yac_host->printf("xxx pat compareType dev_idx=%d o->dev_idx=%d midi_ch=%d o->midi_ch=%d note_nr=%d o->note_nr=%d\n", */
      /*                  dev_idx, _o->dev_idx, midi_ch, _o->midi_ch, note_nr, _o->note_nr); */
      return
         (dev_idx == _o->dev_idx) &&
         (midi_ch == _o->midi_ch) &&
         (note_nr == _o->note_nr)
         ;
   }
};


class midipipe_frame_sysex_t : public midipipe_frame_base_t {
  public:
   YAC_Buffer *buffer;  // deletable object or NULL

   midipipe_frame_sysex_t() = default;
   midipipe_frame_sysex_t(sSI _devIdx, sUI _midiCh) : midipipe_frame_base_t(_devIdx, _midiCh) { buffer = NULL; }

   sBool compareType(const midipipe_frame_sysex_t *_o) const {
      return
         (dev_idx == _o->dev_idx)
         ;
   }
};


class midipipe_frame_sysrt_t : public midipipe_frame_base_t {
  public:
   sU8 sysrt_byte; // e.g. $F8=timing clock

   midipipe_frame_sysrt_t() = default;
   midipipe_frame_sysrt_t(sSI _devIdx, sUI _midiCh) : midipipe_frame_base_t(_devIdx, _midiCh) { sysrt_byte = 0xFEu; }

   sBool compareType(const midipipe_frame_sysrt_t *_o) const {
      return
         (dev_idx == _o->dev_idx)
         ;
   }
};


// filter:
template <>
midipipe_frame_ctl_t *MIDIPipeFrameValueArray<midipipe_frame_ctl_t>::filter
(sSI _fltDev, sSI _fltCh, sUI _ctlId) ;


// saveElement:
template <>
void MIDIPipeFrameValueArray<midipipe_frame_base_t>::saveElement
(YAC_Object *_ofs,
 const midipipe_frame_base_t *_ev
 ) const;

template <>
void MIDIPipeFrameValueArray<midipipe_frame_val_t>::saveElement
(YAC_Object *_ofs,
 const midipipe_frame_val_t *_ev
 ) const;

template <>
void MIDIPipeFrameValueArray<midipipe_frame_ctl_t>::saveElement
(YAC_Object *_ofs,
 const midipipe_frame_ctl_t *_ev
 ) const;

template <>
void MIDIPipeFrameValueArray<midipipe_frame_note_t>::saveElement
(YAC_Object *_ofs,
 const midipipe_frame_note_t *_ev
 ) const;

template <>
void MIDIPipeFrameValueArray<midipipe_frame_pat_t>::saveElement
(YAC_Object *_ofs,
 const midipipe_frame_pat_t *_ev
 ) const;

template <>
void MIDIPipeFrameValueArray<midipipe_frame_sysex_t>::saveElement
(YAC_Object *_ofs,
 const midipipe_frame_sysex_t *_ev
 ) const;

template <>
void MIDIPipeFrameValueArray<midipipe_frame_sysrt_t>::saveElement
(YAC_Object *_ofs,
 const midipipe_frame_sysrt_t *_ev
 ) const;


// loadElement:
template <>
void MIDIPipeFrameValueArray<midipipe_frame_base_t>::loadElement
(YAC_Object *_ifs,
 midipipe_frame_base_t *_ev,
 sUI _ver
 );

template <>
void MIDIPipeFrameValueArray<midipipe_frame_val_t>::loadElement
(YAC_Object *_ifs,
 midipipe_frame_val_t *_ev,
 sUI _ver
 );

template <>
void MIDIPipeFrameValueArray<midipipe_frame_ctl_t>::loadElement
(YAC_Object *_ifs,
 midipipe_frame_ctl_t *_ev,
 sUI _ver
 );

template <>
void MIDIPipeFrameValueArray<midipipe_frame_note_t>::loadElement
(YAC_Object *_ifs,
 midipipe_frame_note_t *_ev,
 sUI _ver
 );

template <>
void MIDIPipeFrameValueArray<midipipe_frame_pat_t>::loadElement
(YAC_Object *_ifs,
 midipipe_frame_pat_t *_ev,
 sUI _ver
 );

template <>
void MIDIPipeFrameValueArray<midipipe_frame_sysex_t>::loadElement
(YAC_Object *_ifs,
 midipipe_frame_sysex_t *_ev,
 sUI _ver
 );

template <>
void MIDIPipeFrameValueArray<midipipe_frame_sysrt_t>::loadElement
(YAC_Object *_ifs,
 midipipe_frame_sysrt_t *_ev,
 sUI _ver
 );


// deleteByFltAndNr:
template <>
sUI MIDIPipeFrameValueArray<midipipe_frame_ctl_t>::deleteByFltAndNr
(sSI _devIdx, sSI _fltCh, sUI _nr);


#endif // TK_MIDIPIPEFRAMEVALUEARRAY_H__
