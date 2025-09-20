// ----
// ---- file   : MIDIPipeNodeScriptProxy.cpp
// ---- author : Bastian Spiegel <bs@tkscript.de>
// ---- legal  : (c) 2014-2023 by Bastian Spiegel.
// ----          Distributed under terms of the GNU LESSER GENERAL PUBLIC LICENSE (LGPL). See
// ----          http://www.gnu.org/licenses/licenses.html#LGPL or COPYING for further information.
// ----
// ---- info   : This is part of the "syntracker" sequencer.
// ----
// ---- created: 03Sep2014
// ---- changed: 04Sep2014, 08Sep2014, 09Sep2014, 27Sep2014, 08Feb2015, 16Feb2015, 08Mar2015
// ----          13Mar2016, 29Mar2019, 25Jun2019, 09Nov2020, 03Dec2023
// ----
// ----
// ----

#include "tkmidipipe.h"


MIDIPipeNodeScriptProxy::MIDIPipeNodeScriptProxy(void) {
   sci = NULL;
   b_sci_deleteme = YAC_FALSE;
}

MIDIPipeNodeScriptProxy::~MIDIPipeNodeScriptProxy() {
   freeSCI();
}

void MIDIPipeNodeScriptProxy::init(MIDIPipe *_parentPipe) {
   MIDIPipeNode::init(_parentPipe);
}

void MIDIPipeNodeScriptProxy::setSci(YAC_Object *_sci) {

   freeSCI();

   if(YAC_VALID(_sci))
   {
      if(YAC_Is_Value(_sci))
      {
         YAC_CAST_ARG(YAC_ValueObject, vo, _sci);

         if(vo->type == YAC_TYPE_OBJECT)
         {
            sci = vo->value.object_val;

            b_sci_deleteme = vo->deleteme;
            vo->deleteme = YAC_FALSE;
         }
      }
      else
      {
         sci = _sci;

         b_sci_deleteme = YAC_FALSE;
      }
   }
}

YAC_Object *MIDIPipeNodeScriptProxy::getSci(void) {
   return sci;
}

void MIDIPipeNodeScriptProxy::finalizeSCI(void) {
   // only used by NodeScript (as of [09Nov2020])
   YAC_Value r;
   yac_host->yacEvalMethodByName(sci,
                                 "nodeFinalize",
                                 NULL/*args*/,
                                 0,/*numArgs*/
                                 &r
                                 );

   r.unsetFast();
}

void MIDIPipeNodeScriptProxy::freeSCI(void) {
   if(b_sci_deleteme)
   {
      finalizeSCI();

      YAC_DELETE_SAFE(sci);
      b_sci_deleteme = YAC_FALSE;
   }
   else
   {
      sci = NULL;
   }
}

void MIDIPipeNodeScriptProxy::processFrame(MIDIPipeFrame *_framePlay, MIDIPipeFrame *_frameRec,
                                           sBool _bForceMute,
                                           sBool _bPlaySeq
                                           ) {

   // printf("xxx MIDIPipeNodeScriptProxy::processFrame: this=%p bForceMute=%d bPlaySeq=%d sci=%p\n", this, _bForceMute, _bPlaySeq, sci);

   if(NULL != sci)
   {
      YAC_Value args[4];
      YAC_Value r;

#if 0
      // (todo) 09Oct2017
      // (todo) this break first tick play in NodeTrigSeq (_bMute initially false b/c of old bMuteTemp)
      // (todo) is it safe to remove this ?
      YAC_Value rQueuedMuteCustomHandler;

      sBool bMuteTemp = false;

      sUI queuedMuteTempKey = sci->yacMetaClassMemberGetAccessKeyByName("node_queued_b_mute_temp");
      sUI defaultQueuedMuteTempHandlerKey = sci->yacMetaClassMemberGetAccessKeyByName("b_node_queued_b_mute_temp_custom_handler");
      sUI bMuteTempKey = sci->yacMetaClassMemberGetAccessKeyByName("node_b_mute_temp");

      // yac_host->printf("queuedMuteTempKey=%08x\n", queuedMuteTempKey);
      // yac_host->printf("defaultQueuedMuteTempHandlerKey=%08x\n", defaultQueuedMuteTempHandlerKey);
      // yac_host->printf("bMuteTempKey=%08x\n", bMuteTempKey);

      if( (~0u != queuedMuteTempKey)               &&
          (~0u != defaultQueuedMuteTempHandlerKey) &&
          (~0u != bMuteTempKey)
          )
      {
         sci->yacMetaClassMemberGet(defaultQueuedMuteTempHandlerKey, &rQueuedMuteCustomHandler);

         if(0 == rQueuedMuteCustomHandler.value.int_val)
         {
            // Fallback code: handle queued mutes immediately (_not_ handled in script code)
            sci->yacMetaClassMemberGet(queuedMuteTempKey, &r);

            if(-1 != r.value.int_val)
            {
               // Update current b_mute_temp state
               sci->yacMetaClassMemberSet(bMuteTempKey, &r);

               bMuteTemp = (0 != r.value.int_val);
            }
            else
            {
               // Query current b_mute_temp state
               sci->yacMetaClassMemberGet(bMuteTempKey, &r);

               bMuteTemp = (0 != r.value.int_val);
            }
         }
         else
         {
            // Node handles queued mute in script code
            bMuteTemp = false;
         }
      }

      args[0].initObject(_framePlay, 0);
      args[1].initObject(_frameRec, 0);
      args[2].initInt   (!isNodeActive() || _bForceMute || bMuteTemp);
      args[3].initInt   (_bPlaySeq);

#else
      args[0].initObject(_framePlay, 0);
      args[1].initObject(_frameRec, 0);
      args[2].initInt   (!isNodeActive() || _bForceMute);
      args[3].initInt   (_bPlaySeq);

#endif

      yac_host->yacEvalMethodByName(sci,
                                    "nodeProcessFrame",
                                    args,
                                    4u,/*numArgs*/
                                    &r
                                    );

      r.unsetFast();
   }
}

void MIDIPipeNodeScriptProxy::seek(sUI _ticks) {
   if(NULL != sci)
   {
      YAC_Value args[1];
      YAC_Value r;

      args[0].initInt((sSI)_ticks);

      yac_host->yacEvalMethodByName(sci,
                                    "nodeSeek",
                                    args,
                                    1u,/*numArgs*/
                                    &r
                                    );

      r.unsetFast();
   }
}

void MIDIPipeNodeScriptProxy::reset(sBool _bSoft) {
   if(NULL != sci)
   {
      YAC_Value args[1];
      YAC_Value r;

      args[0].initInt((sSI)_bSoft);

      yac_host->yacEvalMethodByName(sci,
                                    "nodeReset",
                                    args,
                                    1u,/*numArgs*/
                                    &r
                                    );

      r.unsetFast();
   }
}

void MIDIPipeNodeScriptProxy::songSelect(void) {
   if(NULL != sci)
   {
      YAC_Value r;

      yac_host->yacEvalMethodByName(sci,
                                    "nodeSongSelect",
                                    NULL/*args*/,
                                    0u,/*numArgs*/
                                    &r
                                    );

      r.unsetFast();
   }
}

void MIDIPipeNodeScriptProxy::handleSongSpeedChanged(sBool _bPPQ, sBool _bBPM) {
   if(NULL != sci)
   {
      YAC_Value r;
      YAC_Value args[2];
      args[0].initInt(_bPPQ);
      args[1].initInt(_bBPM);

      yac_host->yacEvalMethodByName(sci,
                                    "nodeHandleSongSpeedChanged",
                                    args,
                                    2u,/*numArgs*/
                                    &r
                                    );

      r.unsetFast();
   }
}
