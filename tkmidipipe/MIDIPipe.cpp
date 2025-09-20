// ----
// ---- file   : MIDIPipe.cpp
// ---- author : Bastian Spiegel <bs@tkscript.de>
// ---- legal  : (c) 2014-2023 by Bastian Spiegel.
// ----          Distributed under terms of the GNU LESSER GENERAL PUBLIC LICENSE (LGPL). See
// ----          http://www.gnu.org/licenses/licenses.html#LGPL or COPYING for further information.
// ----
// ---- info   : This is part of the "syntracker" sequencer.
// ----
// ---- created: 01Sep2014
// ---- changed: 03Sep2014, 08Sep2014, 10Sep2014, 11Sep2014, 14Sep2014, 27Sep2014, 08Feb2015
// ----          23Feb2015, 24Feb2015, 08Mar2015, 28Apr2015, 19Jan2017, 24Mar2017, 25Jun2019
// ----          14Sep2019, 30Oct2020, 19Jun2021, 03Dec2023
// ----
// ----
// ----

#include "tkmidipipe.h"


MIDIPipe::MIDIPipe(void) {
   root = NULL;

   first_node = NULL;
   num_nodes = 0u;

   name = NULL;
   remark = NULL;
   suggested_preset_name = NULL;

   pipe_state = presolo_state = MIDIPIPE_STATE_PLAY;

   solo_node = NULL;

   last_edited_node_idx = -1;

   b_final_output = false;

   current_delay_frame_idx = 0u;
}

MIDIPipe::~MIDIPipe() {
   freeNodes();

   YAC_DELETE_SAFE(name);
   YAC_DELETE_SAFE(remark);
   YAC_DELETE_SAFE(suggested_preset_name);
}

void MIDIPipe::freeNodes(void) {
   MIDIPipeNode *c = first_node;

   while(NULL != c)
   {
      MIDIPipeNode *n = c->next;

      YAC_DELETE(c);

      c = n;
   }
}

void MIDIPipe::init(MIDIPipeRoot *_root) {
   root = _root;
}

MIDIPipeRoot *MIDIPipe::getRoot(void) const {
   return root;
}

sUI MIDIPipe::getPipeIdx(void) const {
   return pipe_idx;
}

sUI MIDIPipe::getPipeUsedIdx(void) const {

   sUI usedIdx = 0u;

   if(NULL != root) // should never be NULL
   {
      for(sUI idx = 0u; idx < root->max_pipes; idx++)
      {
         if(NULL != root->pipes[idx])
         {
            if(root->pipes[idx] == this)
               return usedIdx;
            usedIdx++;
         }
      }
   }
   return usedIdx;
}

void MIDIPipe::setName(YAC_String *_name) {
   if(NULL == name)
   {
      name = YAC_New_String();
   }

   name->yacCopy(_name);
}

YAC_String *MIDIPipe::getName(void) const {
   return name;
}

void MIDIPipe::setRemark(YAC_Object *_remark) {
   if(YAC_VALID(_remark) && YAC_Is_String(_remark))
   {
      YAC_CAST_ARG(YAC_String, newRemark, _remark);

      if(NULL == remark)
      {
         remark = YAC_New_String();
      }

      remark->yacCopy(newRemark);
   }
   else
   {
      YAC_DELETE_SAFE(remark);
   }
}

YAC_String *MIDIPipe::getRemark(void) const {
   return remark;
}

void MIDIPipe::setSuggestedPresetName(YAC_String *_name) {
   if(NULL == suggested_preset_name)
   {
      suggested_preset_name = YAC_New_String();
   }

   suggested_preset_name->yacCopy(_name);
}

YAC_String *MIDIPipe::getSuggestedPresetName(void) const {
   return suggested_preset_name;
}

MIDIPipe *MIDIPipe::getPrevUsedPipe(void) const {
   MIDIPipe *prev = NULL;

   if(NULL != root)
   {
      for(sUI i = 0u; i < root->max_pipes; i++)
      {
         MIDIPipe *c = root->pipes[i];
         if(NULL != c)
         {
            if(c != this)
               prev = c;
            else
               break;
         }
      }
   }

   return prev;
}

MIDIPipe *MIDIPipe::getNextUsedPipe(void) const {
   MIDIPipe *next = NULL;

   if(NULL != root)
   {
      for(sUI i = 0u; i < root->max_pipes; i++)
      {
         MIDIPipe *c = root->pipes[i];
         if(NULL != c)
         {
            if(c == this)
            {
               next = c;
            }
            else if(NULL != next)
            {
               next = c;
               break;
            }
         }
      }
   }

   if(next == this)
      next = NULL;

   return next;
}

MIDIPipe *MIDIPipe::getPrevNonEmptyUsedPipe(void) const {
   MIDIPipe *prev = NULL;

   if(NULL != root)
   {
      for(sUI i = 0u; i < root->max_pipes; i++)
      {
         MIDIPipe *c = root->pipes[i];
         if(NULL != c)
         {
            if(c != this)
            {
               if(0u != c->getNumNodes())
                  prev = c;
            }
            else
            {
               break;
            }
         }
      }
   }

   return prev;
}

MIDIPipe *MIDIPipe::getNextNonEmptyUsedPipe(void) const {
   MIDIPipe *next = NULL;

   if(NULL != root)
   {
      for(sUI i = 0u; i < root->max_pipes; i++)
      {
         MIDIPipe *c = root->pipes[i];

         // yac_host->printf("xxx c=%p i=%u (max=%u) this=%p next=%p\n", c, i, root->max_pipes, this, next);

         if(NULL != c)
         {
            if(c == this)
            {
               next = c;
            }
            else if(this == next)
            {
               if(0u != c->getNumNodes())
               {
                  next = c;
                  break;
               }
            }
         }
      }
   }

   // yac_host->printf("xxx ret next=%p\n", next);

   if(next == this)
      next = NULL;

   return next;
}

sUI MIDIPipe::getNumNodes(void) const {
   return num_nodes;
}

MIDIPipeNode *MIDIPipe::getFirst(void) const {
   return first_node;
}

MIDIPipeNode *MIDIPipe::getLast(void) const {
   MIDIPipeNode *c = first_node;

   if(NULL != c)
   {
      while(NULL != c->next)
         c = c->next;
   }

   return (MIDIPipeNode*)c;
}

MIDIPipeNode *MIDIPipe::getNodeByIdx(sUI _idx) const {
   MIDIPipeNode *c = first_node;
   sUI i = 0u;

   while(NULL != c)
   {
      if(i == _idx)
         return c;

      i++;
      c = c->next;
   }

   return NULL;
}

void MIDIPipe::addNode(YAC_Object *_nodeVO) {
   if(YAC_Is_Value(_nodeVO))
   {
      YAC_ValueObject *vo = (YAC_ValueObject*)_nodeVO;

      if(vo->deleteme)
      {
         if(YAC_BCHK(vo->value.object_val, clid_MIDIPipeNode))
         {
            vo->deleteme = YAC_FALSE;
            MIDIPipeNode *n = (MIDIPipeNode*)vo->value.object_val;

            n->init(this);

            if(NULL == first_node)
            {
               first_node = n;
               n->prev = NULL;
               n->next = NULL;
            }
            else
            {
               // Append to tail
               MIDIPipeNode *c = first_node;
               while(NULL != c->next)
               {
                  c = c->next;
               }
               c->next = n;
               n->prev = c;
               n->next = NULL;
            }

            num_nodes++;
         }
         else
         {
            yac_host->printf("[---] MIDIPipe::addNode: node object is not derived from MIDIPipeNode.\n");
         }
      }
      else
      {
         yac_host->printf("[---] MIDIPipe::addNode: failed, node object is not deletable.\n");
      }
   }
   else
   {
      yac_host->printf("[---] MIDIPipe::addNode: node object is not wrapped in value object #().\n");
   }
}

sBool MIDIPipe::unlinkNodeInt(MIDIPipeNode *_node, YAC_Value *_r) {
   if(NULL != _node)
   {
      MIDIPipeNode *c = first_node;

      sSI nodeIdx = (sSI) c->getNodeIdx();

      while(NULL != c)
      {
         if(c == _node)
         {
            if(NULL != c->prev)
               c->prev->next = c->next;

            if(NULL != c->next)
            {
               if(-1 != last_edited_node_idx)
               {
                  if( (nodeIdx + 1) >= last_edited_node_idx)
                     last_edited_node_idx--;
               }

               c->next->prev = c->prev;
            }
            else
            {
               if(nodeIdx == last_edited_node_idx)
                  last_edited_node_idx = -1;
            }

            if(first_node == c)
               first_node = c->next;

            _r->initObject(c, 1);

            num_nodes--;

            return YAC_TRUE;
         }
         else
         {
            c = c->next;
         }
      }
   }

   return YAC_FALSE;
}

void MIDIPipe::unlinkNode(MIDIPipeNode *_node, YAC_Value *_r) {
   (void)unlinkNodeInt(_node, _r);
}

sBool MIDIPipe::removeNode(MIDIPipeNode *_node) {
   YAC_Value r;
   sBool ret = unlinkNodeInt(_node, &r);
   r.unsetFast();
   return ret;
}

sUI MIDIPipe::getState(void) const {
   return pipe_state;
}

void MIDIPipe::setState(sUI _state) {
   pipe_state = _state;
}

sUI MIDIPipe::getPresoloState(void) const {
   return presolo_state;
}

void MIDIPipe::setPresoloState(sUI _state) {
   presolo_state = _state;
}

void MIDIPipe::setSoloNode(YAC_Object *_node) {
   solo_node = (MIDIPipeNode*)_node;
}

MIDIPipeNode *MIDIPipe::getSoloNode(void) const {
   return solo_node;
}

void MIDIPipe::saveSoloState(void) {
   presolo_state = pipe_state;
}

void MIDIPipe::restorePreSoloState(void) {
   pipe_state = presolo_state;

   if(MIDIPIPE_STATE_SOLO == pipe_state)
      root->solo_pipe = this;
}

sBool MIDIPipe::isPipeActive(void) const {
   if(MIDIPIPE_STATE_MUTE != pipe_state)
   {
      if(MIDIPIPE_STATE_SOLO == pipe_state)
      {
         return (root->solo_pipe == this);
      }
      else
      {
         // Play
         return YAC_TRUE;
      }
   }
   return YAC_FALSE;
}

void MIDIPipe::processFrame(MIDIPipeFrame *_framePlay, MIDIPipeFrame *_frameRec, sBool _bPlaySeq) {

   if(NULL != root)
   {
      _framePlay->setRoot(root);

      MIDIPipeNode *c = first_node;

      while(NULL != c)
      {
         // yac_host->printf("xxx MIDIPipe::processFrame c=%p\n", c);

         c->processFrame(_framePlay,
                         _frameRec,
                         (MIDIPIPE_STATE_MUTE == pipe_state),
                         _bPlaySeq
                         );

         // yac_host->printf("xxx 2 MIDIPipe::processFrame c=%p\n", c);

         c = c->next;
      }
   }
}

void MIDIPipe::seek(sUI _ticks) {
   if(NULL != root)
   {
      MIDIPipeNode *c = first_node;

      while(NULL != c)
      {
         c->seek(_ticks);

         c = c->next;
      }
   }
}

void MIDIPipe::reset(sBool _bSoft) {

   current_delay_frame_idx = 0u;

   for(sUI i = 0u; i < MAX_PIPE_DELAY; i++)
      delay_frames[i].empty();

   if(NULL != root)
   {
      MIDIPipeNode *c = first_node;

      while(NULL != c)
      {
         c->reset(_bSoft);

         c = c->next;
      }
   }
}

void MIDIPipe::songSelect(void) {
   if(NULL != root)
   {
      MIDIPipeNode *c = first_node;

      while(NULL != c)
      {
         c->songSelect();

         c = c->next;
      }
   }
}

sBool MIDIPipe::moveUp(void) {
   if(NULL != root)
   {
      if(pipe_idx > 0u)
      {
         root->swapPipeSlots(pipe_idx, pipe_idx - 1u);
         return YAC_TRUE;
      }
   }
   return YAC_FALSE;
}

sBool MIDIPipe::moveDown(void) {
   if(NULL != root)
   {
      if((sSI)pipe_idx < (((sSI)root->max_pipes) -1))
      {
         root->swapPipeSlots(pipe_idx, pipe_idx + 1);
         return YAC_TRUE;
      }
   }
   return YAC_FALSE;
}

void MIDIPipe::handleSongSpeedChanged(sBool _bPPQ, sBool _bBPM) {
   if(NULL != root)
   {
      MIDIPipeNode *c = first_node;

      while(NULL != c)
      {
         c->handleSongSpeedChanged(_bPPQ, _bBPM);

         c = c->next;
      }
   }
}

void MIDIPipe::setLastEditedNodeIdx(sSI _idx) {
   last_edited_node_idx = _idx;
}

sSI MIDIPipe::getLastEditedNodeIdx(void) {
   return last_edited_node_idx;
}

void MIDIPipe::setFinalOutput(sBool _bEnable) {
   b_final_output = _bEnable;
}

sBool MIDIPipe::getFinalOutput(void) {
   return b_final_output;
}
