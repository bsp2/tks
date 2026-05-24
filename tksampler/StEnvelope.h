// ----
// ---- file   : StEnvelope.h
// ---- author : Bastian Spiegel <bs@tkscript.de>
// ---- legal  : Distributed under terms of the MIT license (https://opensource.org/licenses/MIT)
// ----          Copyright 2010-2026 by bsp
// ----
// ----          Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
// ----          associated documentation files (the "Software"), to deal in the Software without restriction, including
// ----          without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// ----          copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to
// ----          the following conditions:
// ----
// ----          The above copyright notice and this permission notice shall be included in all copies or substantial
// ----          portions of the Software.
// ----
// ----          THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT
// ----          NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// ----          IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
// ----          WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
// ----          SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
// ----
// ---- info   : This is part of the "syntracker" midi sequencer.
// ----
// ---- created: 14Feb2010
// ---- changed: 23Aug2021, 03Oct2024, 22Apr2026, 24May2026
// ----
// ----
// ----

#ifndef ST_ENVELOPE_H__
#define ST_ENVELOPE_H__

YG("tksampler")


// Note: 50sec @ blocklen 0.0005 sec
#define STENVELOPE_MIN_TIMESCALE 0.00001f
//#define STENVELOPE_MAX_TIMESCALE_MS ((1000.0f/STENVELOPE_MIN_TIMESCALE)*STSAMPLEVOICE_BLOCK_LEN)  // = 100000
#define STENVELOPE_MAX_TIMESCALE_MS (100000.0f)


/* @class StEnvelope
*/
YC class StEnvelope : public YAC_Object {
   //
   // Note: this is primarily used in the StADSR class
   //

  public:
   TKS_Envelope *data; // Note: envelope total time should be 1 to make timing calculations work correctly

   sF32 intensity; // influence/scaling of envelope. r=(1.0f-intensity)*lastStage + thisState*intensity, if ADSR uses relative level scaling
   sF32 timescale; // usually 1.0, can be used to speed up/slow down A,D or R envelope separately
   sF32 exponent;  // usually 1.0. adsr_out=pow(env_val, exponent)

   YAC_Object *metadata_object; // UI/editor-related metadata
   sBool       metadata_deleteme;

   YAC_String *ui_preset_name;

   sF32 last_played_position;  // UI

  protected:
   void deleteMetaData (void);
   void deleteUIPresetName (void);

  public:
   StEnvelope(void);
   ~StEnvelope();

   YAC(StEnvelope);

   YM YAC_Object *_getData (void);
   YM YAC_Object *_getOrCreateData (void);

   YM void        _setMetaData (YAC_Object *_v); // set (deletable) object
   YM YAC_Object *_getMetaData (void);           // get reference to metadata

   YM sF32 _getIntensity (void);
   YM void _setIntensity (sF32 _v);

   YM sF32 _getTimescale (void);
   YM void _setTimescale (sF32 _v);

   YM sF32 _getTimescaleMillisec (void);
   YM void _setTimescaleMillisec (sF32 _v);

   YM sF32 _getExponent (void);
   YM void _setExponent (sF32 _v);

   YM void        _setUiPresetName (YAC_Object *_s);
   YM YAC_Object *_getUiPresetName (void);

   YM sF32 _getLastPlayedPosition (void);

};


#endif // ST_ENVELOPE_H__
