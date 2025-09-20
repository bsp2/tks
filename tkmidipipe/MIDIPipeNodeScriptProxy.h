// ----
// ---- file   : MIDIPipeNodeScriptProxy.h
// ---- author : Bastian Spiegel <bs@tkscript.de>
// ---- legal  : (c) 2014-2024 by Bastian Spiegel.
// ----          Distributed under terms of the GNU LESSER GENERAL PUBLIC LICENSE (LGPL). See
// ----          http://www.gnu.org/licenses/licenses.html#LGPL or COPYING for further information.
// ----
// ---- info   : This is part of the "syntracker" sequencer.
// ----
// ---- created: 03Sep2014
// ---- changed: 04Sep2014, 08Sep2014, 09Sep2014, 11Sep2014, 27Sep2014, 08Feb2015, 16Feb2015
// ----          22Feb2015, 08Mar2015, 25Jun2019, 09Nov2020, 03Dec2023, 06Oct2024
// ----
// ----
// ----

#ifndef TK_MIDIPIPENODESCRIPTPROXY_H__
#define TK_MIDIPIPENODESCRIPTPROXY_H__

YG("midipipe")


/* @class MIDIPipeNodeScriptProxy,Object
 */
YC class MIDIPipeNodeScriptProxy : public MIDIPipeNode {
  public:
   YAC_Object *sci;
   sBool       b_sci_deleteme;

  public:
   MIDIPipeNodeScriptProxy(void);
   ~MIDIPipeNodeScriptProxy();

   YAC(MIDIPipeNodeScriptProxy);

  protected:
   void finalizeSCI (void);

  public:
   YM void        init                   (MIDIPipe *_parentPipe);
   YM void        setSci                 (YAC_Object *_sci);
   YM YAC_Object *getSci                 (void);
   YM void        freeSCI                (void);
   YM void        processFrame           (MIDIPipeFrame *_framePlay, MIDIPipeFrame *_frameRec, sBool _bForceMute, sBool _bPlaySeq);
   YM void        seek                   (sUI _ticks);
   YM void        reset                  (sBool _bSoft);
   YM void        songSelect             (void);
   YM void        handleSongSpeedChanged (sBool _bPPQ, sBool _bBPM);

};


#endif // TK_MIDIPIPENODESCRIPTPROXY_H__
