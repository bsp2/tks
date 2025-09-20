// ----
// ---- file   : SampleChain.h
// ---- author : Bastian Spiegel <bs@tkscript.de>
// ---- legal  : (c) 2016 by Bastian Spiegel. 
// ----          Distributed under terms of the GNU LESSER GENERAL PUBLIC LICENSE (LGPL). See 
// ----          http://www.gnu.org/licenses/licenses.html#LGPL or COPYING for further information.
// ----
// ---- info   : This is part of the "syntracker" sequencer.
// ----
// ---- changed: 25Mar2016
// ----
// ----
// ----

#ifndef __TK_SAMPLECHAIN_H__
#define __TK_SAMPLECHAIN_H__

YG("samplechain")


/* @class SampleChain,Object
 */
YC class SampleChain : public YAC_Object {

  public:
   samplechain_algorithm_t alg;
   bool_t b_alg_valid;
   samplechain_t sc;


  public:
   SampleChain(void);
   ~SampleChain();

   YAC(SampleChain);

  protected:
   void free (void);

  public:
   YM sUI getNumAlgorithms (void);
   YM sBool selectAlgorithm (sUI _idx);
   YM void getAlgorithmName (YAC_Value *_r);

   YM sBool init (sUI _numSlices);
   YM sBool setParameteri (YAC_String *_pname, sSI _value);
   YM sBool setParameterf (YAC_String *_pname, sF32 _value);
   YM sBool add (sUI _numFrames, YAC_Object *_userData);
   YM void calc (void);
   YM sUI queryNumElements (void);
   YM sUI queryTotalSize (void);
   YM sUI queryElementOffset (sUI _elementIdx);
   YM sUI queryElementTotalSize (sUI _elementIdx);
   YM sUI queryElementOriginalSize (sUI _elementIdx);
   YM YAC_Object *queryElementUserData (sUI _elementIdx);
   YM void exit (void);
};


#endif // __TK_SAMPLECHAIN_H__
