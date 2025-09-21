// "Moog"-ish ladder filter 

// adapted from SyModuleFltMoog1 (2010, based on see <http://musicdsp.org/showArchiveComment.php?ArchiveID=24>)

/*
 * @cycle_mod    xldf
 * @cycle_voice  SyModuleFltMoog %cyid%;
 * @cycle_noteon voice->%cyid%.init();
 * @cycle_noteon voice->%cyid%.reset(voice->sample_rate, voice->sr_factor);
 * @cycle_input  freq
 * @cycle_input  q
 * @cycle_config mode 0
 * @cycle_emit   %cyio% = voice->%cyid%.calc(%cyio%, %cycf_mode%, %cyin_freq%, %cyin_q%);
 *
 */

class SyModuleFltMoogState {
  public:
   float x, y1, y2, y3, y4; // temporary
   float oldx, oldy1, oldy2, oldy3;
   float newy4;

   inline void reset(void) {
      x = y1 = y2 = y3 = y4 = 0.0f;
      oldx = oldy1 = oldy2 = oldy3 = 0.0f;
      newy4 = 0.0f;
   }

   inline void shuffle(void) {
      oldx  = x;
      oldy1 = y1;
      oldy2 = y2;
      oldy3 = y3;
      y4    = newy4;
   }
};

class SyModuleFltMoog {
   struct {
      float p, k, r;
   } param;

   SyModuleFltMoogState state;

   float sample_rate;
   float sr_factor;

  public:
   SyModuleFltMoog(void) {
   }

   ~SyModuleFltMoog() {
   }

   /* filter types */
   enum {
      LPF = 0, /* low pass filter */
      HPF = 1, /* high pass filter */
      BPF = 2, /* band pass filter */
   };

   void init (void) {
   }

   void reset (float _sampleRate, float _srFactor) {
      sample_rate = _sampleRate;
      sr_factor   = _srFactor;
      param.p = param.k = param.r = 0.0f;
      state.reset();
   }

   void calcCoeff (float freq,   // center frequency 0..1
                   float q       // resonance 0..1
                   ) {
      param.p = freq * (1.8f - 0.8f * freq);
      param.k = param.p + param.p - 1.0f;

      float t  = (1.0f - param.p) * 1.386249f;
      float t2 = 12.0f + t * t;
      param.r = q * (t2 + 6.0f * t) / (t2 - 6.0f * t);
   }

   float calc (float _inSmp, int _cfgType, float _inFreq, float _inQ) {

      // Filter type
      int type = _cfgType;
      if(type < 0)
         type = 0;
      else if(type > BPF)
         type = BPF;

      // Cutoff/center frequency
      float freq = _inFreq;
      if(freq < 0.0005f)
         freq = 0.0005f;
      else if(freq > 1.0f)
         freq = 1.0f;

      // Resonance
      float q = _inQ;
      if(q < 0.0f)
         q = 0.0f;
      else if(q > 1.0f)
         q = 1.0f;

      // Recalc coefficients
      calcCoeff(freq * sr_factor, q);

#define FREQ_THRESHOLD (1.0f / 0.4f)
      float t = 1.0f;
      if(freq <= FREQ_THRESHOLD)
      {
         float f = (float) (freq * FREQ_THRESHOLD * sr_factor);
         t = q * (1.0f-f);
         t = t*t * (3.02f - 2.0f*t);
         t = (1.0f + t * 1.13f);
      }

      // Iterate channels (removed, mono only)

      // Inverted feed back for corner peaking
      state.x = _inSmp - param.r * state.y4;
      state.x = Dstplugin_fix_denorm_32(state.x);

      //Four cascaded onepole filters (bilinear transform)
      state.y1    = state.x *param.p + state.oldx *param.p - state.y1*param.k;
      state.y2    = state.y1*param.p + state.oldy1*param.p - state.y2*param.k;
      state.y3    = state.y2*param.p + state.oldy2*param.p - state.y3*param.k;
      state.newy4 = state.y3*param.p + state.oldy3*param.p - state.y4*param.k;

      // Dyac_dbgflt_32(state.newy4);

      state.y1    = Dstplugin_fix_denorm_32(state.y1);
      state.y2    = Dstplugin_fix_denorm_32(state.y2);
      state.y3    = Dstplugin_fix_denorm_32(state.y3);
      state.newy4 = Dstplugin_fix_denorm_32(state.newy4);

      // Clipper band limited sigmoid
      state.newy4 = state.newy4 - ((state.newy4*state.newy4*state.newy4) * (1.0f / 6.0f));

      float out = t * state.newy4;
      state.shuffle();

      // pseudo HPF / BPF ?
      if(HPF == type)
         out = _inSmp - out;
      else if(BPF == type)
         out = out - (_inSmp - out);

      return out;
   }

};
