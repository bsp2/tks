// ----
// ---- file   : MIDIPipeNode.cpp
// ---- author : Bastian Spiegel <bs@tkscript.de>
// ---- legal  : (c) 2014-2019 by Bastian Spiegel.
// ----          Distributed under terms of the GNU LESSER GENERAL PUBLIC LICENSE (LGPL). See
// ----          http://www.gnu.org/licenses/licenses.html#LGPL or COPYING for further information.
// ----
// ---- info   : This is part of the "syntracker" sequencer.
// ----
// ---- created: 01Sep2014
// ---- changed: 04Sep2014, 05Sep2014, 08Sep2014, 09Sep2014, 10Sep2014, 11Sep2014, 14Sep2014
// ----          08Feb2015, 24Feb2015, 08Mar2015, 28Apr2015, 19Jan2017, 13Mar2017, 25Jun2019
// ----          14Sep2019, 05Oct2019, 03Dec2023
// ----
// ----
// ----

#include "tkmidipipe.h"


MIDIPipeNode::MIDIPipeNode(void) {
   parent_pipe = NULL;
   prev        = NULL;
   next        = NULL;
   name        = NULL;
   remark      = NULL;
   suggested_preset_name = NULL;
   node_state  = presolo_state = MIDIPIPE_STATE_PLAY;
}

MIDIPipeNode::~MIDIPipeNode() {
   YAC_DELETE_SAFE(name);
   YAC_DELETE_SAFE(remark);
   YAC_DELETE_SAFE(suggested_preset_name);
}

void MIDIPipeNode::init(MIDIPipe *_parentPipe) {
   parent_pipe = _parentPipe;
}

void MIDIPipeNode::setName(YAC_String *_name) {
   if(NULL == name)
   {
      name = YAC_New_String();
   }
   name->yacCopy(_name);
}

YAC_String *MIDIPipeNode::getName(void) const {
   return name;
}

void MIDIPipeNode::setRemark(YAC_Object *_remark) {
   if(YAC_VALID(_remark))
   {
      if(NULL == remark)
      {
         remark = YAC_New_String();
      }
      remark->yacCopy(_remark);
   }
   else
   {
      YAC_DELETE_SAFE(remark);
   }
}

YAC_String *MIDIPipeNode::getRemark(void) const {
   return remark;
}

void MIDIPipeNode::setSuggestedPresetName(YAC_String *_name) {
   if(NULL == suggested_preset_name)
   {
      suggested_preset_name = YAC_New_String();
   }
   suggested_preset_name->yacCopy(_name);
}

YAC_String *MIDIPipeNode::getSuggestedPresetName(void) const {
   return suggested_preset_name;
}

MIDIPipe *MIDIPipeNode::getParent(void) const {
   return parent_pipe;
}

MIDIPipeNode *MIDIPipeNode::getPrev(void) const {
   return prev;
}

MIDIPipeNode *MIDIPipeNode::getNext(void) const {
   return next;
}

sUI MIDIPipeNode::getNodeIdx(void) const {
   sUI nodeIdx = 0u;

   const MIDIPipeNode *c = this;

   while(NULL != c->prev)
   {
      nodeIdx++;
      c = c->prev;
   }

   return nodeIdx;
}

sUI MIDIPipeNode::getState(void) const {
   return node_state;
}

void MIDIPipeNode::setState(sUI _state) {
   node_state = _state;
}

sUI MIDIPipeNode::getPresoloState(void) const {
   return presolo_state;
}

void MIDIPipeNode::setPresoloState(sUI _state) {
   presolo_state = _state;
}

void MIDIPipeNode::saveSoloState(void) {
   presolo_state = node_state;
}

void MIDIPipeNode::restorePreSoloState(void) {
   node_state = presolo_state;

   if(MIDIPIPE_STATE_SOLO == node_state)
   {
      parent_pipe->solo_node = this;
   }
}

sBool MIDIPipeNode::isNodeActive(void) const {
   if(MIDIPIPE_STATE_MUTE != node_state)
   {
      if(MIDIPIPE_STATE_SOLO == node_state)
      {
         return (parent_pipe->solo_node == this);
      }
      else
      {
         // Play
         return YAC_TRUE;
      }
   }
   return YAC_FALSE;
}

sBool MIDIPipeNode::isPipeMuted(void) const {
   if(NULL != parent_pipe)
   {
      return !parent_pipe->isPipeActive();
   }
   return YAC_FALSE;
}

sBool MIDIPipeNode::isNodeOrPipeMuted(void) const {
   if(NULL != parent_pipe)
   {
      return !parent_pipe->isPipeActive() || !isNodeActive();
   }
   return YAC_FALSE;
}

void MIDIPipeNode::processFrame(MIDIPipeFrame *_framePlay,
                                MIDIPipeFrame *_frameRec,
                                sBool _bForceMute,
                                sBool _bPlaySeq
                                ) {
   // implemented by derived classes
}

void MIDIPipeNode::seek(sUI _ticks) {
   // implemented by derived classes
}

void MIDIPipeNode::reset(sBool _bSoft) {
   // implemented by derived classes
   (void)_bSoft;
}

void MIDIPipeNode::songSelect(void) {
   // implemented by derived classes
}

sBool MIDIPipeNode::moveUp(void) {
   if(NULL != prev)
   {
      sSI lastEdited = parent_pipe->last_edited_node_idx;

      if(-1 != lastEdited)
      {
         sSI thisNodeIdx = (sSI)getNodeIdx();

         if(thisNodeIdx == lastEdited)
         {
            parent_pipe->last_edited_node_idx--;
         }
         else if( (thisNodeIdx - 1) == lastEdited)
         {
            parent_pipe->last_edited_node_idx++;
         }
      }

      prev->next = next;

      if(NULL != next)
      {
         next->prev = prev;
      }

      MIDIPipeNode *p = prev->prev;
      if(NULL != prev->prev)
      {
         prev->prev->next = this;
      }
      prev->prev = this;
      next = prev;
      prev = p;

      if(NULL == prev)
         parent_pipe->first_node = this;

      return YAC_TRUE;
   }
   return YAC_FALSE;
}

sBool MIDIPipeNode::moveDown(void) {
   if(NULL != next)
   {
      sSI lastEdited = parent_pipe->last_edited_node_idx;

      if(-1 != lastEdited)
      {
         sSI thisNodeIdx = (sSI)getNodeIdx();

         if(thisNodeIdx == lastEdited)
         {
            parent_pipe->last_edited_node_idx++;
         }
         else if( (thisNodeIdx + 1) == lastEdited)
         {
            parent_pipe->last_edited_node_idx--;
         }
      }

      next->prev = prev;

      if(NULL != prev)
      {
         prev->next = next;
      }

      MIDIPipeNode *n = next;
      next = next->next;
      n->next = this;
      prev = n;

      if(NULL != next)
      {
         next->prev = this;
      }

      if(parent_pipe->first_node == this)
      {
         parent_pipe->first_node = n;
      }

      return YAC_TRUE;
   }
   return YAC_FALSE;
}

void MIDIPipeNode::handleSongSpeedChanged(sBool _bPPQ, sBool _bBPM) {
}
