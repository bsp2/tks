// ----
// ---- file   : MIDIPipeNode.h
// ---- author : Bastian Spiegel <bs@tkscript.de>
// ---- legal  : (c) 2014-2024 by Bastian Spiegel.
// ----          Distributed under terms of the GNU LESSER GENERAL PUBLIC LICENSE (LGPL). See
// ----          http://www.gnu.org/licenses/licenses.html#LGPL or COPYING for further information.
// ----
// ---- info   : This is part of the "syntracker" sequencer.
// ----
// ---- created: 01Sep2014
// ---- changed: 04Sep2014, 05Sep2014, 08Sep2014, 09Sep2014, 10Sep2014, 11Sep2014, 27Sep2014
// ----          08Feb2015, 16Feb2015, 08Mar2015, 28Apr2015, 19Jan2017, 03Dec2018, 25Jun2019
// ----          14Sep2019, 05Oct2019, 03Dec2023, 06Oct2024
// ----
// ----
// ----

#ifndef TK_MIDIPIPENODE_H__
#define TK_MIDIPIPENODE_H__

YG("midipipe")


/* @class MIDIPipeNode,Object
 */
YC class MIDIPipeNode : public YAC_Object {
  public:
   MIDIPipe *parent_pipe;

   MIDIPipeNode *prev;
   MIDIPipeNode *next;

   YAC_String *name;
   YAC_String *remark;
   YAC_String *suggested_preset_name;

   sUI node_state; // see MIDIPIPE_STATE_xxx

   sUI presolo_state; // save state when other node is solo'd

  public:
   MIDIPipeNode(void);
   ~MIDIPipeNode();

   YAC(MIDIPipeNode);

  public:
   YM virtual void init (MIDIPipe *_parentPipe);

   YM void        setName (YAC_String *_name);
   YM YAC_String *getName (void) const;

   YM void        setRemark (YAC_Object *_remark);
   YM YAC_String *getRemark (void) const;

   YM void        setSuggestedPresetName (YAC_String *_name);
   YM YAC_String *getSuggestedPresetName (void) const;

   YM MIDIPipe     *getParent  (void) const;
   YM MIDIPipeNode *getPrev    (void) const;
   YM MIDIPipeNode *getNext    (void) const;
   YM sUI           getNodeIdx (void) const;

   YM sUI   getState            (void) const;
   YM void  setState            (sUI _state);
   YM sUI   getPresoloState     (void) const;
   YM void  setPresoloState     (sUI _state);
   YM sBool isNodeActive        (void) const;
   YM sBool isPipeMuted         (void) const;
   YM sBool isNodeOrPipeMuted   (void) const;
   YM void  saveSoloState       (void);
   YM void  restorePreSoloState (void);

   YM virtual void processFrame (MIDIPipeFrame *_framePlay, MIDIPipeFrame *_frameRec, sBool _bForceMute, sBool _bPlaySeq);

   YM virtual void seek (sUI _ticks);

   YM virtual void reset (sBool _bSoft);

   YM virtual void songSelect (void);

   YM sBool moveUp (void);

   YM sBool moveDown (void);

   YM virtual void handleSongSpeedChanged (sBool _bPPQ, sBool _bBPM);

};


#endif // TK_MIDIPIPENODE_H__
