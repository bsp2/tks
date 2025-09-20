// ----
// ---- file   : MIDIPipe.h
// ---- author : Bastian Spiegel <bs@tkscript.de>
// ---- legal  : (c) 2014-2024 by Bastian Spiegel.
// ----          Distributed under terms of the GNU LESSER GENERAL PUBLIC LICENSE (LGPL). See
// ----          http://www.gnu.org/licenses/licenses.html#LGPL or COPYING for further information.
// ----
// ---- info   : This is part of the "syntracker" sequencer.
// ----
// ---- created: 01Sep2014
// ---- changed: 03Sep2014, 04Sep2014, 05Sep2014, 08Sep2014, 09Sep2014, 10Sep2014, 11Sep2014
// ----          14Sep2014, 19Sep2014, 20Sep2014, 27Sep2014, 08Feb2015, 22Feb2015, 23Feb2015
// ----          24Feb2015, 08Mar2015, 28Apr2015, 19Jan2017, 24Mar2017, 25Jun2019, 14Sep2019
// ----          19Jun2021, 20Jun2021, 03Dec2023, 06Oct2024
// ----
// ----
// ----

#ifndef TK_MIDIPIPE_H__
#define TK_MIDIPIPE_H__

YG("midipipe")


/* @class MIDIPipe,Object
 */
YC class MIDIPipe : public YAC_Object {

#define MIDIPIPE_STATE_PLAY  YCI  (0u)
#define MIDIPIPE_STATE_MUTE  YCI  (1u)
#define MIDIPIPE_STATE_SOLO  YCI  (2u)
#define NUM_MIDIPIPE_STATES  YCI  (3u)

#define MAX_PIPE_DELAY       YCI  (64u)
#define MAX_PIPE_DELAY_MASK  YCI  (63u)


  public:
   MIDIPipeRoot *root;

   sUI pipe_idx;

   MIDIPipeNode *first_node;
   sUI num_nodes;

   YAC_String *name;
   YAC_String *remark;
   YAC_String *suggested_preset_name;

   sUI pipe_state;
   MIDIPipeNode *solo_node; // never dereferenced
   sUI presolo_state; // save state when other pipe is solo'd

   sSI last_edited_node_idx;

   sBool b_final_output;  // true=route all MIDI (framePlay) output through this pipe

   MIDIPipeFrame delay_frames[MAX_PIPE_DELAY];
   sUI current_delay_frame_idx;


  public:
   MIDIPipe(void);
   ~MIDIPipe();

   YAC(MIDIPipe);

  public:
   void freeNodes (void);
   sBool unlinkNodeInt (MIDIPipeNode *_node, YAC_Value *_r);


  public:
   YM void init (MIDIPipeRoot *_root);

   YM MIDIPipeRoot *getRoot (void) const;

   YM MIDIPipe *getPrevUsedPipe (void) const;
   YM MIDIPipe *getNextUsedPipe (void) const;

   YM MIDIPipe *getPrevNonEmptyUsedPipe (void) const;
   YM MIDIPipe *getNextNonEmptyUsedPipe (void) const;

   YM sUI getPipeIdx (void) const;
   YM sUI getPipeUsedIdx (void) const;

   YM void setName (YAC_String *_name);
   YM YAC_String *getName (void) const;

   YM void setRemark (YAC_Object *_remark);
   YM YAC_String *getRemark (void) const;

   YM void setSuggestedPresetName (YAC_String *_name);
   YM YAC_String *getSuggestedPresetName (void) const;

   YM sUI getNumNodes (void) const;

   YM MIDIPipeNode *getFirst (void) const;
   YM MIDIPipeNode *getLast (void) const;

   YM MIDIPipeNode *getNodeByIdx (sUI _idx) const;

   YM void addNode (YAC_Object *_nodeVO);

   YM sBool removeNode (MIDIPipeNode *_node);

   YM void unlinkNode (MIDIPipeNode *_node, YAC_Value *_r);

   YM sUI           getState            (void) const;
   YM void          setState            (sUI _state);
   YM sUI           getPresoloState     (void) const;
   YM void          setPresoloState     (sUI _state);
   YM void          setSoloNode         (YAC_Object *_node);
   YM MIDIPipeNode *getSoloNode         (void) const;
   YM sBool         isPipeActive        (void) const;
   YM void          saveSoloState       (void);
   YM void          restorePreSoloState (void);

   YM void processFrame (MIDIPipeFrame *_framePlay, MIDIPipeFrame *_frameRec, sBool _bPlaySeq);

   YM void seek (sUI _ticks);

   YM void reset (sBool _bSoft);

   YM void songSelect (void);

   YM sBool moveUp (void);
   YM sBool moveDown (void);

   YM void handleSongSpeedChanged (sBool _bPPQ, sBool _bBPM);

   YM void setLastEditedNodeIdx (sSI _idx);

   YM sSI getLastEditedNodeIdx (void);

   YM void setFinalOutput (sBool _bEnable);
   YM sBool getFinalOutput (void);
};


#endif // TK_MIDIPIPE_H__
