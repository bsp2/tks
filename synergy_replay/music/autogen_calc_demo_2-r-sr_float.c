// auto-generated on 20260522-204826

//         name: cycle_rs off=0 num=1139
//  sample_rate: 16574
//    note_freq: 449
//      #frames: 1139
//        xfade: 128



static float var_x_cycle_rs_0_rs = 0.0f;
static float var_wt_x_cycle_rs_0_rs = 0.0f;
static float var_wt_y_cycle_rs_0_rs = 0.0f;
static float var_wt_freq_cycle_rs_0_rs = 0.0f;

static float tmp_f;
static float tmp2_f;
static short tmp_i;
static short tmp2_i;
#define loop(X)  for(unsigned int i = 0u; i < (X); i++)
#define clamp(a,b,c) (((a)<(b))?(b):(((a)>(c))?(c):(a)))

static inline float mathLerpf(float _a, float _b, float _t) { return _a + (_b - _a) * _t; }
static inline float mathClampf(float a, float b, float c) { return (((a)<(b))?(b):(((a)>(c))?(c):(a))); }
static inline float mathMinf(float a, float b) { return (a<b)?a:b; }
static inline float mathMaxf(float a, float b) { return (a>b)?a:b; }
static inline float mathAbsMaxf(float _x, float _y) { return ( ( (_x<0.0f)?-_x:_x)>((_y<0.0f)?-_y:_y)?_x:_y ); }
static inline float mathAbsMinf(float _x, float _y) { return ( ((_x<0.0f)?-_x:_x)<((_y<0.0f)?-_y:_y)?_x:_y ); }
static inline float frac(float _x) { return _x - ((int)_x); }

static inline float winLinear(const float *_s, float _index) {
   int idx = (int)_index;
   float r = _index - (float)idx;
   return mathLerpf(_s[idx], _s[idx+1], r);
}

extern float cycle_sine_tbl_f[16384];

extern short cycle_sine_tbl_i[16384];
extern float cycle_xfade_buf[4096];

static float ffrac_s(float _f) { int i; if(_f >= 0.0f) { i = (int)_f; return _f - (float)i; } else { i = (int)-_f; return 1.0f - (-_f - (float)i); } }

static float mathLogf(const float _x) {
   union {
      float f;
      unsigned int u;
   } bx;
   bx.f = _x;
   const unsigned int ex = bx.u >> 23;
   const signed int t = (signed int)ex - (signed int)127;
   const unsigned int s = (t < 0) ? (-t) : t;
   bx.u = 1065353216u | (bx.u & 8388607u);
   return
      -1.7417939f + (2.8212026f + (-1.4699568f + (0.44717955f - 0.056570851f * bx.f)*bx.f)*bx.f)*bx.f
      + 0.6931471806f * t;
}
#include <math.h>
#define Dstplugin_fix_denorm_32(a) ( ((a)+10.0f) - 10.0f )

static float mathPowerf(float _x, float _y) {
   float r;
   if(_y != 0.0f)
   {
      if(_x < 0.0f)
      {
         r = (float)( -expf(_y*mathLogf(-_x)) );
      }
      else if(_x > 0.0f)
      {
         r = (float)( expf(_y*mathLogf(_x)) );
      }
      else
      {
         r = 0.0f;
      }
   }
   else
   {
      r = 1.0f;
   }
   return Dstplugin_fix_denorm_32(r);
}

static short curve_1_cycle_rs_0_rs[32] = {
2048, 2068, 1970, 1781, 1306, 716, 272, 175, 129, 97, 75, 62, 56, 55, 59, 67, 77, 89, 102, 116, 129, 141, 152, 160, 164, 164, 159, 148, 128, 98, 57, 0, 
};
static short curve_2_cycle_rs_0_rs[32] = {
2048, 1901, 1768, 1648, 1538, 1437, 1345, 1260, 1182, 1110, 1043, 981, 922, 868, 816, 767, 720, 675, 631, 588, 546, 504, 461, 418, 374, 328, 281, 231, 178, 123, 63, 0, 
};
static short curve_3_cycle_rs_0_rs[32] = {
2048, 1797, 1594, 1427, 1287, 1168, 1065, 977, 900, 833, 774, 722, 676, 635, 598, 564, 533, 504, 476, 450, 424, 397, 371, 343, 313, 281, 246, 207, 164, 116, 61, 0, 
};

float tmp2_saw_speed__cycle_rs_0_rs;
float tmp5_rmp_step__cycle_rs_0_rs;
float tmp7_pul_speed__cycle_rs_0_rs;
float tmp10_rmp_step__cycle_rs_0_rs;
float tmp14_rmp_step__cycle_rs_0_rs;
float tmp18_rmp_step__cycle_rs_0_rs;

static void calc_cycle_rs_0_rs(float *_d) {
   float tmp1_saw_phase = 0.0f;
   float tmp4_rmp_cur = 0.0f;
   float tmp6_pul_phase = 0.0f;
   float tmp9_rmp_cur = 0.0f;
   float tmp11_lpf_1rc_v0 = 0.0f;
   float tmp12_lpf_1rc_v1 = 0.0f;
   float tmp13_rmp_cur = 0.0f;
   float tmp15_lpf_1rc_v0 = 0.0f;
   float tmp16_lpf_1rc_v1 = 0.0f;
   float tmp17_rmp_cur = 0.0f;
   float tmp19_svf_lp = 0.0f;
   float tmp20_svf_hp = 0.0f;
   float tmp21_svf_bp = 0.0f;
   // start=0 end=0.99 #frames=725.112:
   // start=0 end=0.99 #frames=517.938:
   // start=0 end=0.99 #frames=517.938:
   // start=0 end=0.99 #frames=517.938:
   
   tmp2_saw_speed__cycle_rs_0_rs = 0.0270906f;
   tmp5_rmp_step__cycle_rs_0_rs = 0.00136531f;
   tmp7_pul_speed__cycle_rs_0_rs = 0.101996f;
   tmp10_rmp_step__cycle_rs_0_rs = 0.00191143f;
   tmp14_rmp_step__cycle_rs_0_rs = 0.00191143f;
   tmp18_rmp_step__cycle_rs_0_rs = 0.00191143f;
   int smpIdx = 0;
   int outIdx = 0;
   int xfadeCaptureLeft = 128;
   int xfadeReadIdx = 0;
   float xfadeCur = 0.0f;
   int xfadeCaptureIdx = 0;
   loop(1139)
   {
      float out = 0.0f;
      
      // ========
      // ======== lane "out" modIdx=0 modString=saw
      // ========
      
      // -- mod="saw" dstVar=out
      float tmp3;
      tmp3/*saw_tmp*/ = (tmp1_saw_phase);
      tmp3/*saw_tmp*/ = ffrac_s(tmp3/*saw_tmp*/);
      out = 1.0f - (tmp3/*saw_tmp*/ * 2.0f);
      tmp1_saw_phase = ffrac_s(tmp1_saw_phase + tmp2_saw_speed__cycle_rs_0_rs);
      
      // -- mod="rmp" dstVar=out
      tmp3/*seq*/ = out;
      tmp3/*seq*/ = tmp4_rmp_cur;
      tmp4_rmp_cur = mathMinf(tmp4_rmp_cur+tmp5_rmp_step__cycle_rs_0_rs, 0.99);
      
      // -- mod="lut" dstVar=tmp3/*seq*/
      tmp3/*seq*/ = curve_2_cycle_rs_0_rs[((unsigned int)(tmp3/*seq*/ * 32)) & 31] * (1.0f / 2048);
      
      // -- mod="$AMP1_RNG" dstVar=tmp3/*seq*/
      tmp3/*seq*/ *= 0.4f;
      
      // -- mod="$AMP1_MIN" dstVar=tmp3/*seq*/
      tmp3/*seq*/ += 0.0f;
      out *= tmp3/*seq*/;
      
      // -- mod="pul" dstVar=out
      tmp3/*seq*/ = out;
      float tmp8;
      tmp8/*pul_tmp*/ = (tmp6_pul_phase);
      tmp8/*pul_tmp*/ = ffrac_s(tmp8/*pul_tmp*/);
      tmp3/*seq*/ = (tmp8/*pul_tmp*/ >= 0.5) ? 1.0f : -1.0f;
      tmp6_pul_phase = ffrac_s(tmp6_pul_phase + tmp7_pul_speed__cycle_rs_0_rs);
      
      // -- mod="rmp" dstVar=tmp3/*seq*/
      tmp8/*seq*/ = tmp3/*seq*/;
      tmp8/*seq*/ = tmp9_rmp_cur;
      tmp9_rmp_cur = mathMinf(tmp9_rmp_cur+tmp10_rmp_step__cycle_rs_0_rs, 0.99);
      
      // -- mod="lut" dstVar=tmp8/*seq*/
      tmp8/*seq*/ = curve_3_cycle_rs_0_rs[((unsigned int)(tmp8/*seq*/ * 32)) & 31] * (1.0f / 2048);
      
      // -- mod="$AMP2_RNG" dstVar=tmp8/*seq*/
      tmp8/*seq*/ *= 0.4f;
      
      // -- mod="$AMP2_MIN" dstVar=tmp8/*seq*/
      tmp8/*seq*/ += 0.0f;
      tmp3/*seq*/ *= tmp8/*seq*/;
      out += tmp3/*seq*/;
      
      // -- mod="lpf" dstVar=out
      
      // ---- mod="lpf" input "freq" seq 1/1
      
      // -- mod="rmp" dstVar=tmp3/*freq*/
      tmp3/*freq*/ = tmp13_rmp_cur;
      tmp13_rmp_cur = mathMinf(tmp13_rmp_cur+tmp14_rmp_step__cycle_rs_0_rs, 0.99);
      
      // -- mod="lut" dstVar=tmp3/*freq*/
      tmp3/*freq*/ = curve_1_cycle_rs_0_rs[((unsigned int)(tmp3/*freq*/ * 32)) & 31] * (1.0f / 2048);
      
      // -- mod="$LPF_RNG" dstVar=tmp3/*freq*/
      tmp3/*freq*/ *= 0.148438f;
      
      // -- mod="$LPF_MIN" dstVar=tmp3/*freq*/
      tmp3/*freq*/ += 0.101562f;
      tmp_f = (1.0f - tmp3/*freq*/ * 0.078125);
      tmp11_lpf_1rc_v0 = tmp_f * tmp11_lpf_1rc_v0 - tmp3/*freq*/ * tmp12_lpf_1rc_v1 + out * tmp3/*freq*/;
      tmp12_lpf_1rc_v1 = tmp_f * tmp12_lpf_1rc_v1 + tmp3/*freq*/ * tmp11_lpf_1rc_v0;
      out = tmp12_lpf_1rc_v1;
      
      // -- mod="lpf" dstVar=out
      
      // ---- mod="lpf" input "freq" seq 1/1
      
      // -- mod="rmp" dstVar=tmp3/*freq*/
      tmp3/*freq*/ = tmp17_rmp_cur;
      tmp17_rmp_cur = mathMinf(tmp17_rmp_cur+tmp18_rmp_step__cycle_rs_0_rs, 0.99);
      
      // -- mod="lut" dstVar=tmp3/*freq*/
      tmp3/*freq*/ = curve_1_cycle_rs_0_rs[((unsigned int)(tmp3/*freq*/ * 32)) & 31] * (1.0f / 2048);
      
      // -- mod="$LPF_RNG" dstVar=tmp3/*freq*/
      tmp3/*freq*/ *= 0.148438f;
      
      // -- mod="$LPF_MIN" dstVar=tmp3/*freq*/
      tmp3/*freq*/ += 0.101562f;
      tmp_f = (1.0f - tmp3/*freq*/ * 0.078125);
      tmp15_lpf_1rc_v0 = tmp_f * tmp15_lpf_1rc_v0 - tmp3/*freq*/ * tmp16_lpf_1rc_v1 + out * tmp3/*freq*/;
      tmp16_lpf_1rc_v1 = tmp_f * tmp16_lpf_1rc_v1 + tmp3/*freq*/ * tmp15_lpf_1rc_v0;
      out = tmp16_lpf_1rc_v1;
      
      // -- mod="svf" dstVar=out
      tmp19_svf_lp = tmp19_svf_lp + (tmp21_svf_bp * 0.346875f);
      tmp20_svf_hp = out - tmp19_svf_lp - (tmp21_svf_bp * 0.146875f);
      tmp21_svf_bp = tmp21_svf_bp + (tmp20_svf_hp * 0.346875f);
      out = tmp20_svf_hp + tmp21_svf_bp;
      
      // -- mod="$AMP" dstVar=out
      out *= 2.33125f;
      
      // -- mod="clp" dstVar=out
      if(out > 0.999f) out = 0.999f;
      else if(out < -0.999f) out = -0.999f;
      
      // ------ xfade (128 frames) -----
      if(outIdx >= 1006)
      {
         if(xfadeCaptureLeft > 0)
         {
            cycle_xfade_buf[xfadeCaptureIdx++] = out;
            xfadeCaptureLeft--;
         }
         else if(outIdx >= 1011)
         {
            out = mathLerpf(out, cycle_xfade_buf[xfadeReadIdx++], xfadeCur);
            xfadeCur += 0.0078125;
         }
      }
      

      _d[outIdx++] = out;
   } /* end loop frames */
} /* end calc() */



//         name: fmstack rubber drum 1 off=1171 num=15052
//  sample_rate: 48000
//    note_freq: 261.63
//      #frames: 15052



static float var_x_fmstack_rubber_drum_1_1_ = 0.0f;
static float var_wt_x_fmstack_rubber_drum_1_1_ = 0.0f;
static float var_wt_y_fmstack_rubber_drum_1_1_ = 0.0f;
static float var_wt_freq_fmstack_rubber_drum_1_1_ = 0.0f;



static void calc_fmstack_rubber_drum_1_1_(float *_d) {
   short tmp1;
   unsigned short tmp2_lfsr_state = 17545;
   short tmp3_lfsr_state_signed;
   float tmp4_hpf_z1 = 0.0f;
   float tmp5_hpf_z2 = 0.0f;
   float tmp6_lpf_1rc_v0 = 0.0f;
   float tmp7_lpf_1rc_v1 = 0.0f;
   float tmp8_boo_last = 0.0f;
   int smpIdx = 0;
   int outIdx = 0;
   loop(15052)
   {
      float out = 0.0f;
      
      // ========
      // ======== lane "out" modIdx=0 modString=fsr
      // ========
      
      // -- mod="fsr" dstVar=out
      tmp1/*i2f*/ = (int)(out * 2048);  // IntFallback: F2I
      tmp2_lfsr_state ^= tmp2_lfsr_state >> 7;
      tmp2_lfsr_state ^= tmp2_lfsr_state << 9;
      tmp2_lfsr_state ^= tmp2_lfsr_state >> 13;
      tmp3_lfsr_state_signed = (tmp2_lfsr_state & 65520);
      tmp1/*i2f*/ = tmp3_lfsr_state_signed >> 4;
      out = tmp1/*i2f*/ / ((float)(2048));  // IntFallback: I2F
      
      // -- mod="hpf" dstVar=out
      tmp_f = out;
      out = tmp_f * 0.198638 + tmp4_hpf_z1;
      tmp4_hpf_z1 = tmp_f * -0.397276 + tmp5_hpf_z2 - 0.217248 * out;
      tmp5_hpf_z2 = tmp_f * -0.397276 - 0.011799 * out;
      
      // -- mod="lpf" dstVar=out
      tmp6_lpf_1rc_v0 = 0.75 * tmp6_lpf_1rc_v0 - 0.5 * tmp7_lpf_1rc_v1 + out * 0.5;
      tmp7_lpf_1rc_v1 = 0.75 * tmp7_lpf_1rc_v1 + 0.5 * tmp6_lpf_1rc_v0;
      out = tmp7_lpf_1rc_v1;
      
      // -- mod="boo" dstVar=out
      float tmp9;
      tmp9/*boo_tmp*/ = tmp8_boo_last;
      tmp8_boo_last = out;
      out += (out - tmp9/*boo_tmp*/) * 0.8f;
      

      _d[outIdx++] = out;
   } /* end loop frames */
} /* end calc() */



//         name: fmstack rubber drum 2 off=16223 num=15052
//  sample_rate: 48000
//    note_freq: 261.63
//      #frames: 15052



static float var_x_fmstack_rubber_drum_2_1_ = 0.0f;
static float var_wt_x_fmstack_rubber_drum_2_1_ = 0.0f;
static float var_wt_y_fmstack_rubber_drum_2_1_ = 0.0f;
static float var_wt_freq_fmstack_rubber_drum_2_1_ = 0.0f;



static void calc_fmstack_rubber_drum_2_1_(float *_d) {
   short tmp1;
   unsigned short tmp2_lfsr_state = 17545;
   short tmp3_lfsr_state_signed;
   float tmp4_hpf_z1 = 0.0f;
   float tmp5_hpf_z2 = 0.0f;
   float tmp6_lpf_1rc_v0 = 0.0f;
   float tmp7_lpf_1rc_v1 = 0.0f;
   float tmp8_boo_last = 0.0f;
   int smpIdx = 0;
   int outIdx = 0;
   loop(15052)
   {
      float out = 0.0f;
      
      // ========
      // ======== lane "out" modIdx=0 modString=fsr
      // ========
      
      // -- mod="fsr" dstVar=out
      tmp1/*i2f*/ = (int)(out * 2048);  // IntFallback: F2I
      tmp2_lfsr_state ^= tmp2_lfsr_state >> 7;
      tmp2_lfsr_state ^= tmp2_lfsr_state << 9;
      tmp2_lfsr_state ^= tmp2_lfsr_state >> 13;
      tmp3_lfsr_state_signed = (tmp2_lfsr_state & 65520);
      tmp1/*i2f*/ = tmp3_lfsr_state_signed >> 4;
      out = tmp1/*i2f*/ / ((float)(2048));  // IntFallback: I2F
      
      // -- mod="hpf" dstVar=out
      tmp_f = out;
      out = tmp_f * 0.198638 + tmp4_hpf_z1;
      tmp4_hpf_z1 = tmp_f * -0.397276 + tmp5_hpf_z2 - 0.217248 * out;
      tmp5_hpf_z2 = tmp_f * -0.397276 - 0.011799 * out;
      
      // -- mod="lpf" dstVar=out
      tmp6_lpf_1rc_v0 = 0.75 * tmp6_lpf_1rc_v0 - 0.5 * tmp7_lpf_1rc_v1 + out * 0.5;
      tmp7_lpf_1rc_v1 = 0.75 * tmp7_lpf_1rc_v1 + 0.5 * tmp6_lpf_1rc_v0;
      out = tmp7_lpf_1rc_v1;
      
      // -- mod="boo" dstVar=out
      float tmp9;
      tmp9/*boo_tmp*/ = tmp8_boo_last;
      tmp8_boo_last = out;
      out += (out - tmp9/*boo_tmp*/) * 0.8f;
      

      _d[outIdx++] = out;
   } /* end loop frames */
} /* end calc() */



//         name: fmstack rubber drum 3 off=31275 num=15052
//  sample_rate: 48000
//    note_freq: 261.63
//      #frames: 15052



static float var_x_fmstack_rubber_drum_3_1_ = 0.0f;
static float var_wt_x_fmstack_rubber_drum_3_1_ = 0.0f;
static float var_wt_y_fmstack_rubber_drum_3_1_ = 0.0f;
static float var_wt_freq_fmstack_rubber_drum_3_1_ = 0.0f;



static void calc_fmstack_rubber_drum_3_1_(float *_d) {
   short tmp1;
   unsigned short tmp2_lfsr_state = 17545;
   short tmp3_lfsr_state_signed;
   float tmp4_hpf_z1 = 0.0f;
   float tmp5_hpf_z2 = 0.0f;
   float tmp6_lpf_1rc_v0 = 0.0f;
   float tmp7_lpf_1rc_v1 = 0.0f;
   float tmp8_boo_last = 0.0f;
   int smpIdx = 0;
   int outIdx = 0;
   loop(15052)
   {
      float out = 0.0f;
      
      // ========
      // ======== lane "out" modIdx=0 modString=fsr
      // ========
      
      // -- mod="fsr" dstVar=out
      tmp1/*i2f*/ = (int)(out * 2048);  // IntFallback: F2I
      tmp2_lfsr_state ^= tmp2_lfsr_state >> 7;
      tmp2_lfsr_state ^= tmp2_lfsr_state << 9;
      tmp2_lfsr_state ^= tmp2_lfsr_state >> 13;
      tmp3_lfsr_state_signed = (tmp2_lfsr_state & 65520);
      tmp1/*i2f*/ = tmp3_lfsr_state_signed >> 4;
      out = tmp1/*i2f*/ / ((float)(2048));  // IntFallback: I2F
      
      // -- mod="hpf" dstVar=out
      tmp_f = out;
      out = tmp_f * 0.198638 + tmp4_hpf_z1;
      tmp4_hpf_z1 = tmp_f * -0.397276 + tmp5_hpf_z2 - 0.217248 * out;
      tmp5_hpf_z2 = tmp_f * -0.397276 - 0.011799 * out;
      
      // -- mod="lpf" dstVar=out
      tmp6_lpf_1rc_v0 = 0.75 * tmp6_lpf_1rc_v0 - 0.5 * tmp7_lpf_1rc_v1 + out * 0.5;
      tmp7_lpf_1rc_v1 = 0.75 * tmp7_lpf_1rc_v1 + 0.5 * tmp6_lpf_1rc_v0;
      out = tmp7_lpf_1rc_v1;
      
      // -- mod="boo" dstVar=out
      float tmp9;
      tmp9/*boo_tmp*/ = tmp8_boo_last;
      tmp8_boo_last = out;
      out += (out - tmp9/*boo_tmp*/) * 0.8f;
      

      _d[outIdx++] = out;
   } /* end loop frames */
} /* end calc() */



//         name: fm 1 off=46551 num=24000
//  sample_rate: 48000
//    note_freq: 261.63
//      #frames: 24000



static float var_x_fm_1_0_ = 0.0f;
static float var_wt_x_fm_1_0_ = 0.0f;
static float var_wt_y_fm_1_0_ = 0.0f;
static float var_wt_freq_fm_1_0_ = 0.0f;

static short curve_0_fm_1_0_[64] = {
2048, 1924, 1776, 1610, 1479, 1392, 1327, 1273, 1225, 1180, 1139, 1100, 1063, 1029, 997, 966, 937, 909, 883, 858, 834, 811, 789, 767, 747, 727, 707, 689, 670, 653, 635, 618, 
601, 585, 569, 553, 537, 521, 506, 490, 474, 459, 443, 427, 411, 395, 378, 361, 344, 327, 309, 290, 271, 251, 231, 210, 188, 165, 141, 115, 89, 61, 31, 0, 
};
static short curve_1_fm_1_0_[16] = {
2048, 1377, 966, 698, 519, 398, 317, 262, 225, 197, 174, 152, 126, 94, 52, 0, };

float tmp6_tri_speed__fm_1_0_;
float tmp9_rmp_step__fm_1_0_;
float tmp17_rmp_step__fm_1_0_;

static void calc_fm_1_0_(float *_d) {
   float tmp1_tri_phase = 0.0f;
   float tmp5_tri_phase = 0.0f;
   float tmp8_rmp_cur = 0.0f;
   short tmp11;
   short tmp12;
   float tmp16_rmp_cur = 0.0f;
   float tmp2;
   // start=0 end=1 #frames=24000:
   // start=0 end=1 #frames=24000:
   
   tmp6_tri_speed__fm_1_0_ = 0.0218025f;
   tmp9_rmp_step__fm_1_0_ = 4.16667e-05f;
   tmp17_rmp_step__fm_1_0_ = 4.16667e-05f;
   int smpIdx = 0;
   int outIdx = 0;
   loop(24000)
   {
      float out = 0.0f;
      
      // ========
      // ======== lane "out" modIdx=0 modString=tri
      // ========
      
      // -- mod="tri" dstVar=out
      float tmp3;
      
      // ---- mod="tri" input "freq" seq 1/2
      
      // -- mod="$CAR_F" dstVar=tmp3/*tri_freq*/
      tmp3/*tri_freq*/ = 1.0f;
      
      // ---- mod="tri" input "freq" seq 2/2
      
      // -- mod="1" dstVar=tmp3/*tri_freq*/
      tmp3/*tri_freq*/ = 1.0f;
      
      // -- mod="tri" dstVar=tmp3/*tri_freq*/
      float tmp4;
      tmp4/*seq*/ = tmp3/*tri_freq*/;
      float tmp7;
      tmp7/*tri_tmp*/ = (tmp5_tri_phase);
      tmp7/*tri_tmp*/ = ffrac_s(tmp7/*tri_tmp*/);
      tmp4/*seq*/ = (tmp7/*tri_tmp*/ < 0.5f) ? (-1.0f + tmp7/*tri_tmp*/ * 4.0f) : (1.0f - (tmp7/*tri_tmp*/ - 0.5f)*4);
      tmp5_tri_phase = ffrac_s(tmp5_tri_phase + tmp6_tri_speed__fm_1_0_);
      
      // -- mod="$MOD_A" dstVar=tmp4/*seq*/
      tmp4/*seq*/ *= 2.20087f;
      
      // -- mod="rmp" dstVar=tmp4/*seq*/
      tmp7/*seq*/ = tmp4/*seq*/;
      tmp7/*seq*/ = tmp8_rmp_cur;
      tmp8_rmp_cur = mathMinf(tmp8_rmp_cur+tmp9_rmp_step__fm_1_0_, 1);
      
      // -- mod="lut" dstVar=tmp7/*seq*/
      float tmp10;
      float tmp13;
      float tmp14;
      float tmp15;
      tmp10/*lut_f*/ = (tmp7/*seq*/ * 64);
      tmp13/*lut_frac*/ = tmp10/*lut_f*/ - (int)tmp10/*lut_f*/;
      tmp11/*lut_idx_a*/ = (int)tmp10/*lut_f*/;
      if(tmp11/*lut_idx_a*/ < 0)
      {
         tmp11/*lut_idx_a*/ = 0;
         tmp12/*lut_idx_b*/ = 0;
         tmp13/*lut_frac*/ = 0.0f;
      }
      else if(tmp11/*lut_idx_a*/ > 63)
      {
         tmp11/*lut_idx_a*/ = 63;
         tmp12/*lut_idx_b*/ = 63;
      }
      else
      {
         tmp12/*lut_idx_b*/ = tmp11/*lut_idx_a*/ + 1;
         if(tmp12/*lut_idx_b*/ > 63)
            tmp12/*lut_idx_b*/ = 63;
      }
      tmp14/*lut_a*/ = curve_0_fm_1_0_[(unsigned int)tmp11/*lut_idx_a*/] * (1.0f / 2048);
      tmp15/*lut_b*/ = curve_0_fm_1_0_[(unsigned int)tmp12/*lut_idx_b*/] * (1.0f / 2048);
      tmp7/*seq*/ = tmp14/*lut_a*/ + (tmp15/*lut_b*/ - tmp14/*lut_a*/) * tmp13/*lut_frac*/;
      tmp4/*seq*/ *= tmp7/*seq*/;
      tmp3/*tri_freq*/ += tmp4/*seq*/;
      tmp2/*tri_speed*/ = 0.00545062f * tmp3/*tri_freq*/;
      tmp3/*tri_tmp*/ = (tmp1_tri_phase);
      tmp3/*tri_tmp*/ = ffrac_s(tmp3/*tri_tmp*/);
      out = (tmp3/*tri_tmp*/ < 0.5f) ? (-1.0f + tmp3/*tri_tmp*/ * 4.0f) : (1.0f - (tmp3/*tri_tmp*/ - 0.5f)*4);
      tmp1_tri_phase = ffrac_s(tmp1_tri_phase + tmp2/*tri_speed*/);
      
      // -- mod="$CAR_A" dstVar=out
      out *= 0.748555f;
      
      // -- mod="rmp" dstVar=out
      tmp2/*seq*/ = out;
      tmp2/*seq*/ = tmp16_rmp_cur;
      tmp16_rmp_cur = mathMinf(tmp16_rmp_cur+tmp17_rmp_step__fm_1_0_, 1);
      
      // -- mod="lut" dstVar=tmp2/*seq*/
      tmp3/*lut_f*/ = (tmp2/*seq*/ * 16);
      tmp4/*lut_frac*/ = tmp3/*lut_f*/ - (int)tmp3/*lut_f*/;
      tmp11/*lut_idx_a*/ = (int)tmp3/*lut_f*/;
      if(tmp11/*lut_idx_a*/ < 0)
      {
         tmp11/*lut_idx_a*/ = 0;
         tmp12/*lut_idx_b*/ = 0;
         tmp4/*lut_frac*/ = 0.0f;
      }
      else if(tmp11/*lut_idx_a*/ > 15)
      {
         tmp11/*lut_idx_a*/ = 15;
         tmp12/*lut_idx_b*/ = 15;
      }
      else
      {
         tmp12/*lut_idx_b*/ = tmp11/*lut_idx_a*/ + 1;
         if(tmp12/*lut_idx_b*/ > 15)
            tmp12/*lut_idx_b*/ = 15;
      }
      tmp7/*lut_a*/ = curve_1_fm_1_0_[(unsigned int)tmp11/*lut_idx_a*/] * (1.0f / 2048);
      tmp10/*lut_b*/ = curve_1_fm_1_0_[(unsigned int)tmp12/*lut_idx_b*/] * (1.0f / 2048);
      tmp2/*seq*/ = tmp7/*lut_a*/ + (tmp10/*lut_b*/ - tmp7/*lut_a*/) * tmp4/*lut_frac*/;
      out *= tmp2/*seq*/;
      
      // -- mod="clp" dstVar=out
      if(out > 0.999f) out = 0.999f;
      else if(out < -0.999f) out = -0.999f;
      

      _d[outIdx++] = out;
   } /* end loop frames */
} /* end calc() */



//         name: tr sd 1 off=70551 num=72000
//  sample_rate: 48000
//    note_freq: 261.63
//      #frames: 24000



static float var_x_tr_sd_1_0_ = 0.0f;
static float var_wt_x_tr_sd_1_0_ = 0.0f;
static float var_wt_y_tr_sd_1_0_ = 0.0f;
static float var_wt_freq_tr_sd_1_0_ = 0.0f;
static float var_out__trig_tr_sd_1_0_ = 0.0f;


float tmp2_rmp_step__tr_sd_1_0_;
int tmp12_nos_seed1;
int tmp13_nos_seed2;
float tmp16_rmp_step__tr_sd_1_0_;

static void calc_cycle_tr_sd_1_0_(float *_d) {
   float tmp1_rmp_cur = 1.0f;
   float tmp3_svf_lp = 0.0f;
   float tmp4_svf_hp = 0.0f;
   float tmp5_svf_bp = 0.0f;
   float tmp8_svf_lp = 0.0f;
   float tmp9_svf_hp = 0.0f;
   float tmp10_svf_bp = 0.0f;
   short tmp14;
   float tmp15_rmp_cur = 1.0f;
   float tmp17_svf_lp = 0.0f;
   float tmp18_svf_hp = 0.0f;
   float tmp19_svf_bp = 0.0f;
   // start=1 end=0 #frames=249.711:
   // start=1 end=0 #frames=5895.95:
   
   tmp1_rmp_cur = 1.0f;
   tmp3_svf_lp = 0.0f;
   tmp4_svf_hp = 0.0f;
   tmp5_svf_bp = 0.0f;
   tmp8_svf_lp = 0.0f;
   tmp9_svf_hp = 0.0f;
   tmp10_svf_bp = 0.0f;
   tmp15_rmp_cur = 1.0f;
   tmp17_svf_lp = 0.0f;
   tmp18_svf_hp = 0.0f;
   tmp19_svf_bp = 0.0f;
   int smpIdx = 0;
   int outIdx = 0;
   loop(24000)
   {
      float out = 0.0f;
      
      // ========
      // ======== lane "out" modIdx=0 modString=rmp
      // ========
      
      // -- mod="rmp" dstVar=out
      out = tmp1_rmp_cur;
      tmp1_rmp_cur = mathMaxf(tmp1_rmp_cur+tmp2_rmp_step__tr_sd_1_0_, 0);
      
      // -- mod="pow" dstVar=out
      out = mathPowerf(out, 0.83815f);
      
      // -- mod="set out__trig" dstVar=out
      var_out__trig_tr_sd_1_0_ = out;
      
      // -- mod="svf" dstVar=out
      float tmp6;
      
      // ---- mod="svf" input "res" seq 1/1
      
      // -- mod="$O1_RES" dstVar=tmp6/*res*/
      tmp6/*res*/ = 0.343711f;
      
      // -- mod="pow" dstVar=tmp6/*res*/
      tmp6/*res*/ = tmp6/*res*/ * tmp6/*res*/ * tmp6/*res*/;
      float tmp7;
      
      // ---- mod="svf" input "freq" seq 1/1
      
      // -- mod="$O1_FREQ" dstVar=tmp7/*freq*/
      tmp7/*freq*/ = 0.2023f;
      
      // -- mod="0.1" dstVar=tmp7/*freq*/
      tmp7/*freq*/ *= 0.1f;
      tmp3_svf_lp = tmp3_svf_lp + (tmp5_svf_bp * tmp7/*freq*/);
      tmp4_svf_hp = out - tmp3_svf_lp - (tmp5_svf_bp * tmp6/*res*/);
      tmp5_svf_bp = tmp5_svf_bp + (tmp4_svf_hp * tmp7/*freq*/);
      out = tmp3_svf_lp;
      
      // -- mod="$O1_LVL" dstVar=out
      out *= 0.722543f;
      
      // -- mod="$out__trig" dstVar=out
      tmp6/*seq*/ = out;
      tmp6/*seq*/ = var_out__trig_tr_sd_1_0_;
      
      // -- mod="svf" dstVar=tmp6/*seq*/
      
      // ---- mod="svf" input "res" seq 1/1
      
      // -- mod="$O2_RES" dstVar=tmp7/*res*/
      tmp7/*res*/ = 0.462428f;
      
      // -- mod="pow" dstVar=tmp7/*res*/
      tmp7/*res*/ = tmp7/*res*/ * tmp7/*res*/ * tmp7/*res*/;
      float tmp11;
      
      // ---- mod="svf" input "freq" seq 1/1
      
      // -- mod="$O2_FREQ" dstVar=tmp11/*freq*/
      tmp11/*freq*/ = 0.409f;
      
      // -- mod="0.1" dstVar=tmp11/*freq*/
      tmp11/*freq*/ *= 0.1f;
      tmp8_svf_lp = tmp8_svf_lp + (tmp10_svf_bp * tmp11/*freq*/);
      tmp9_svf_hp = tmp6/*seq*/ - tmp8_svf_lp - (tmp10_svf_bp * tmp7/*res*/);
      tmp10_svf_bp = tmp10_svf_bp + (tmp9_svf_hp * tmp11/*freq*/);
      tmp6/*seq*/ = tmp8_svf_lp;
      
      // -- mod="$O2_LVL" dstVar=tmp6/*seq*/
      tmp6/*seq*/ *= 0.601156f;
      out += tmp6/*seq*/;
      
      // -- mod="nos" dstVar=out
      tmp6/*seq*/ = out;
      tmp14/*nos_fb*/ = tmp12_nos_seed1 & 1;
      tmp12_nos_seed1 >>= 1;
      if(0 != tmp14/*nos_fb*/)
          tmp12_nos_seed1 ^= 0xB4BCD35C;
      tmp14/*nos_fb*/ = tmp13_nos_seed2 & 1;
      tmp13_nos_seed2 >>= 1;
      if(0 != tmp14/*nos_fb*/)
          tmp13_nos_seed2 ^= 0x7A5BC2E3;
      tmp6/*seq*/ = (((tmp12_nos_seed1 ^ tmp13_nos_seed2) & 0xFFFF) - 0x8000) * (1.0 / 32767.0);
      
      // -- mod="rmp" dstVar=tmp6/*seq*/
      tmp7/*seq*/ = tmp6/*seq*/;
      tmp7/*seq*/ = tmp15_rmp_cur;
      tmp15_rmp_cur = mathMaxf(tmp15_rmp_cur+tmp16_rmp_step__tr_sd_1_0_, 0);
      tmp6/*seq*/ *= tmp7/*seq*/;
      
      // -- mod="pow" dstVar=tmp6/*seq*/
      tmp6/*seq*/ = mathPowerf(tmp6/*seq*/, 2.71676f);
      
      // -- mod="svf" dstVar=tmp6/*seq*/
      tmp17_svf_lp = tmp17_svf_lp + (tmp19_svf_bp * 0.130058f);
      tmp18_svf_hp = tmp6/*seq*/ - tmp17_svf_lp - (tmp19_svf_bp * 0.66474f);
      tmp19_svf_bp = tmp19_svf_bp + (tmp18_svf_hp * 0.130058f);
      tmp6/*seq*/ = tmp18_svf_hp;
      
      // -- mod="$NOS_LVL" dstVar=tmp6/*seq*/
      tmp6/*seq*/ *= 0.433526f;
      out += tmp6/*seq*/;
      
      // -- mod="$P_AMP" dstVar=out
      out *= 0.80446f;
      
      // -- mod="clp" dstVar=out
      if(out > 0.999f) out = 0.999f;
      else if(out < -0.999f) out = -0.999f;
      

      _d[outIdx++] = out;
   } /* end loop frames */
} /* end calc() */

// ----- wavetable w=3 h=1 cyclelen=24000
static void calc_tr_sd_1_0_(float *_d) {
   int off = 0;
   var_wt_y_tr_sd_1_0_ = 0.0f;
   tmp2_rmp_step__tr_sd_1_0_ = -0.00400463f;
   tmp12_nos_seed1 = 1033877898; // global
   tmp13_nos_seed2 = 1033877898; // global
   tmp16_rmp_step__tr_sd_1_0_ = -0.000169608f;
   loop(1)
   {
      var_wt_x_tr_sd_1_0_ = 0.0f;
      loop(3)
      {
         float *d = _d + off;
         calc_cycle_tr_sd_1_0_(d);
         var_wt_x_tr_sd_1_0_ += 0.5;
         off += 24000;
      }
   }
}



//         name: tr sd 2 off=142551 num=72000
//  sample_rate: 48000
//    note_freq: 261.63
//      #frames: 24000



static float var_x_tr_sd_2_0_ = 0.0f;
static float var_wt_x_tr_sd_2_0_ = 0.0f;
static float var_wt_y_tr_sd_2_0_ = 0.0f;
static float var_wt_freq_tr_sd_2_0_ = 0.0f;
static float var_out__trig_tr_sd_2_0_ = 0.0f;


float tmp2_rmp_step__tr_sd_2_0_;
int tmp12_nos_seed1;
int tmp13_nos_seed2;
float tmp16_rmp_step__tr_sd_2_0_;

static void calc_cycle_tr_sd_2_0_(float *_d) {
   float tmp1_rmp_cur = 1.0f;
   float tmp3_svf_lp = 0.0f;
   float tmp4_svf_hp = 0.0f;
   float tmp5_svf_bp = 0.0f;
   float tmp8_svf_lp = 0.0f;
   float tmp9_svf_hp = 0.0f;
   float tmp10_svf_bp = 0.0f;
   short tmp14;
   float tmp15_rmp_cur = 1.0f;
   float tmp18_svf_lp = 0.0f;
   float tmp19_svf_hp = 0.0f;
   float tmp20_svf_bp = 0.0f;
   float tmp21_boo_last = 0.0f;
   // start=1 end=0 #frames=249.711:
   // start=1 end=0 #frames=7491.33:
   
   tmp1_rmp_cur = 1.0f;
   tmp3_svf_lp = 0.0f;
   tmp4_svf_hp = 0.0f;
   tmp5_svf_bp = 0.0f;
   tmp8_svf_lp = 0.0f;
   tmp9_svf_hp = 0.0f;
   tmp10_svf_bp = 0.0f;
   tmp15_rmp_cur = 1.0f;
   tmp18_svf_lp = 0.0f;
   tmp19_svf_hp = 0.0f;
   tmp20_svf_bp = 0.0f;
   tmp21_boo_last = 0.0f;
   int smpIdx = 0;
   int outIdx = 0;
   loop(24000)
   {
      float out = 0.0f;
      
      // ========
      // ======== lane "out" modIdx=0 modString=rmp
      // ========
      
      // -- mod="rmp" dstVar=out
      out = tmp1_rmp_cur;
      tmp1_rmp_cur = mathMaxf(tmp1_rmp_cur+tmp2_rmp_step__tr_sd_2_0_, 0);
      
      // -- mod="pow" dstVar=out
      out = mathPowerf(out, 0.982659f);
      
      // -- mod="set out__trig" dstVar=out
      var_out__trig_tr_sd_2_0_ = out;
      
      // -- mod="svf" dstVar=out
      float tmp6;
      
      // ---- mod="svf" input "res" seq 1/1
      
      // -- mod="$O1_RES" dstVar=tmp6/*res*/
      tmp6/*res*/ = 0.384393f;
      
      // -- mod="pow" dstVar=tmp6/*res*/
      tmp6/*res*/ = tmp6/*res*/ * tmp6/*res*/ * tmp6/*res*/;
      float tmp7;
      
      // ---- mod="svf" input "freq" seq 1/1
      
      // -- mod="$O1_FREQ" dstVar=tmp7/*freq*/
      tmp7/*freq*/ = 0.2023f;
      
      // -- mod="0.1" dstVar=tmp7/*freq*/
      tmp7/*freq*/ *= 0.1f;
      tmp3_svf_lp = tmp3_svf_lp + (tmp5_svf_bp * tmp7/*freq*/);
      tmp4_svf_hp = out - tmp3_svf_lp - (tmp5_svf_bp * tmp6/*res*/);
      tmp5_svf_bp = tmp5_svf_bp + (tmp4_svf_hp * tmp7/*freq*/);
      out = tmp3_svf_lp;
      
      // -- mod="$O1_LVL" dstVar=out
      out *= 0.768786f;
      
      // -- mod="$out__trig" dstVar=out
      tmp6/*seq*/ = out;
      tmp6/*seq*/ = var_out__trig_tr_sd_2_0_;
      
      // -- mod="svf" dstVar=tmp6/*seq*/
      
      // ---- mod="svf" input "res" seq 1/1
      
      // -- mod="$O2_RES" dstVar=tmp7/*res*/
      tmp7/*res*/ = 0.439306f;
      
      // -- mod="pow" dstVar=tmp7/*res*/
      tmp7/*res*/ = tmp7/*res*/ * tmp7/*res*/ * tmp7/*res*/;
      float tmp11;
      
      // ---- mod="svf" input "freq" seq 1/1
      
      // -- mod="$O2_FREQ" dstVar=tmp11/*freq*/
      tmp11/*freq*/ = 0.409f;
      
      // -- mod="0.1" dstVar=tmp11/*freq*/
      tmp11/*freq*/ *= 0.1f;
      tmp8_svf_lp = tmp8_svf_lp + (tmp10_svf_bp * tmp11/*freq*/);
      tmp9_svf_hp = tmp6/*seq*/ - tmp8_svf_lp - (tmp10_svf_bp * tmp7/*res*/);
      tmp10_svf_bp = tmp10_svf_bp + (tmp9_svf_hp * tmp11/*freq*/);
      tmp6/*seq*/ = tmp8_svf_lp;
      
      // -- mod="$O2_LVL" dstVar=tmp6/*seq*/
      tmp6/*seq*/ *= 0.763006f;
      out += tmp6/*seq*/;
      
      // -- mod="nos" dstVar=out
      tmp6/*seq*/ = out;
      tmp14/*nos_fb*/ = tmp12_nos_seed1 & 1;
      tmp12_nos_seed1 >>= 1;
      if(0 != tmp14/*nos_fb*/)
          tmp12_nos_seed1 ^= 0xB4BCD35C;
      tmp14/*nos_fb*/ = tmp13_nos_seed2 & 1;
      tmp13_nos_seed2 >>= 1;
      if(0 != tmp14/*nos_fb*/)
          tmp13_nos_seed2 ^= 0x7A5BC2E3;
      tmp6/*seq*/ = (((tmp12_nos_seed1 ^ tmp13_nos_seed2) & 0xFFFF) - 0x8000) * (1.0 / 32767.0);
      
      // -- mod="rmp" dstVar=tmp6/*seq*/
      tmp7/*seq*/ = tmp6/*seq*/;
      tmp7/*seq*/ = tmp15_rmp_cur;
      tmp15_rmp_cur = mathMaxf(tmp15_rmp_cur+tmp16_rmp_step__tr_sd_2_0_, 0);
      
      // -- mod="ipl" dstVar=tmp7/*seq*/
      tmp11/*a*/ = tmp7/*seq*/;
      
      // ---- mod="ipl" input "a" seq 1/1
      
      // -- mod="0" dstVar=tmp11/*a*/
      tmp11/*a*/ = 0.0f;
      float tmp17;
      tmp17/*b*/ = tmp7/*seq*/;
      
      // ---- mod="ipl" input "b" seq 1/1
      
      // -- mod="1" dstVar=tmp17/*b*/
      tmp17/*b*/ = 1.0f;
      tmp7/*seq*/ = tmp7/*seq*/ * tmp7/*seq*/ * (3.0f - 2.0f * tmp7/*seq*/);
      tmp7/*seq*/ = tmp11/*a*/ + (tmp17/*b*/ - tmp11/*a*/) * tmp7/*seq*/;
      tmp6/*seq*/ *= tmp7/*seq*/;
      
      // -- mod="pow" dstVar=tmp6/*seq*/
      tmp6/*seq*/ = mathPowerf(tmp6/*seq*/, 8.20809f);
      
      // -- mod="svf" dstVar=tmp6/*seq*/
      tmp18_svf_lp = tmp18_svf_lp + (tmp20_svf_bp * 0.0462428f);
      tmp19_svf_hp = tmp6/*seq*/ - tmp18_svf_lp - (tmp20_svf_bp * 0.66474f);
      tmp20_svf_bp = tmp20_svf_bp + (tmp19_svf_hp * 0.0462428f);
      tmp6/*seq*/ = tmp19_svf_hp;
      
      // -- mod="boo" dstVar=tmp6/*seq*/
      tmp7/*boo_tmp*/ = tmp21_boo_last;
      tmp21_boo_last = tmp6/*seq*/;
      tmp6/*seq*/ += (tmp6/*seq*/ - tmp7/*boo_tmp*/) * 0.462428f;
      
      // -- mod="$NOS_LVL" dstVar=tmp6/*seq*/
      tmp6/*seq*/ *= 0.534682f;
      out += tmp6/*seq*/;
      
      // -- mod="$P_AMP" dstVar=out
      out *= 0.80446f;
      
      // -- mod="clp" dstVar=out
      if(out > 0.999f) out = 0.999f;
      else if(out < -0.999f) out = -0.999f;
      

      _d[outIdx++] = out;
   } /* end loop frames */
} /* end calc() */

// ----- wavetable w=3 h=1 cyclelen=24000
static void calc_tr_sd_2_0_(float *_d) {
   int off = 0;
   var_wt_y_tr_sd_2_0_ = 0.0f;
   tmp2_rmp_step__tr_sd_2_0_ = -0.00400463f;
   tmp12_nos_seed1 = 1033877898; // global
   tmp13_nos_seed2 = 1033877898; // global
   tmp16_rmp_step__tr_sd_2_0_ = -0.000133488f;
   loop(1)
   {
      var_wt_x_tr_sd_2_0_ = 0.0f;
      loop(3)
      {
         float *d = _d + off;
         calc_cycle_tr_sd_2_0_(d);
         var_wt_x_tr_sd_2_0_ += 0.5;
         off += 24000;
      }
   }
}



//         name: tr bd 5 off=214551 num=36000
//  sample_rate: 48000
//    note_freq: 261.63
//      #frames: 12000



static float var_x_tr_bd_5_0_ = 0.0f;
static float var_wt_x_tr_bd_5_0_ = 0.0f;
static float var_wt_y_tr_bd_5_0_ = 0.0f;
static float var_wt_freq_tr_bd_5_0_ = 0.0f;
static float var_out__freq_env_tr_bd_5_0_ = 0.0f;
static float var_out__trig_tr_bd_5_0_ = 0.0f;


float tmp2_rmp_step__tr_bd_5_0_;
float tmp4_rmp_step__tr_bd_5_0_;
int tmp15_nos_seed1;
int tmp16_nos_seed2;
float tmp19_rmp_step__tr_bd_5_0_;

static void calc_cycle_tr_bd_5_0_(float *_d) {
   float tmp1_rmp_cur = 1.0f;
   float tmp3_rmp_cur = 1.0f;
   float tmp5_svf_lp = 0.0f;
   float tmp6_svf_hp = 0.0f;
   float tmp7_svf_bp = 0.0f;
   float tmp11_svf_lp = 0.0f;
   float tmp12_svf_hp = 0.0f;
   float tmp13_svf_bp = 0.0f;
   short tmp17;
   float tmp18_rmp_cur = 1.0f;
   float tmp20_svf_lp = 0.0f;
   float tmp21_svf_hp = 0.0f;
   float tmp22_svf_bp = 0.0f;
   float tmp23_svf_lp = 0.0f;
   float tmp24_svf_hp = 0.0f;
   float tmp25_svf_bp = 0.0f;
   // start=1 end=0 #frames=1588.44:
   // start=1 end=0 #frames=129.017:
   // start=1 end=0 #frames=1942.2:
   
   tmp1_rmp_cur = 1.0f;
   tmp3_rmp_cur = 1.0f;
   tmp5_svf_lp = 0.0f;
   tmp6_svf_hp = 0.0f;
   tmp7_svf_bp = 0.0f;
   tmp11_svf_lp = 0.0f;
   tmp12_svf_hp = 0.0f;
   tmp13_svf_bp = 0.0f;
   tmp18_rmp_cur = 1.0f;
   tmp20_svf_lp = 0.0f;
   tmp21_svf_hp = 0.0f;
   tmp22_svf_bp = 0.0f;
   tmp23_svf_lp = 0.0f;
   tmp24_svf_hp = 0.0f;
   tmp25_svf_bp = 0.0f;
   int smpIdx = 0;
   int outIdx = 0;
   loop(12000)
   {
      float out = 0.0f;
      
      // ========
      // ======== lane "out" modIdx=0 modString=rmp
      // ========
      
      // -- mod="rmp" dstVar=out
      out = tmp1_rmp_cur;
      tmp1_rmp_cur = mathMaxf(tmp1_rmp_cur+tmp2_rmp_step__tr_bd_5_0_, 0);
      
      // -- mod="pow" dstVar=out
      out = mathPowerf(out, 5.37572f);
      
      // -- mod="$O_FREQ_ENV_LVL" dstVar=out
      out *= 0.248555f;
      
      // -- mod="set out__freq_env" dstVar=out
      var_out__freq_env_tr_bd_5_0_ = out;
      
      // -- mod="rmp" dstVar=out
      out = tmp3_rmp_cur;
      tmp3_rmp_cur = mathMaxf(tmp3_rmp_cur+tmp4_rmp_step__tr_bd_5_0_, 0);
      
      // -- mod="pow" dstVar=out
      out = mathPowerf(out, 0.83815f);
      
      // -- mod="set out__trig" dstVar=out
      var_out__trig_tr_bd_5_0_ = out;
      
      // -- mod="svf" dstVar=out
      float tmp8;
      
      // ---- mod="svf" input "res" seq 1/1
      
      // -- mod="$O1_RES" dstVar=tmp8/*res*/
      tmp8/*res*/ = 0.410405f;
      
      // -- mod="pow" dstVar=tmp8/*res*/
      tmp8/*res*/ = tmp8/*res*/ * tmp8/*res*/ * tmp8/*res*/;
      float tmp9;
      
      // ---- mod="svf" input "freq" seq 1/1
      
      // -- mod="$O1_FREQ" dstVar=tmp9/*freq*/
      tmp9/*freq*/ = 0.16185f;
      
      // -- mod="$out__freq_env" dstVar=tmp9/*freq*/
      float tmp10;
      tmp10/*seq*/ = tmp9/*freq*/;
      tmp10/*seq*/ = var_out__freq_env_tr_bd_5_0_;
      tmp9/*freq*/ += tmp10/*seq*/;
      
      // -- mod="0.1" dstVar=tmp9/*freq*/
      tmp9/*freq*/ *= 0.1f;
      tmp5_svf_lp = tmp5_svf_lp + (tmp7_svf_bp * tmp9/*freq*/);
      tmp6_svf_hp = out - tmp5_svf_lp - (tmp7_svf_bp * tmp8/*res*/);
      tmp7_svf_bp = tmp7_svf_bp + (tmp6_svf_hp * tmp9/*freq*/);
      out = tmp5_svf_lp;
      
      // -- mod="$O1_LVL" dstVar=out
      out *= 0.66763f;
      
      // -- mod="$out__trig" dstVar=out
      tmp8/*seq*/ = out;
      tmp8/*seq*/ = var_out__trig_tr_bd_5_0_;
      
      // -- mod="svf" dstVar=tmp8/*seq*/
      
      // ---- mod="svf" input "res" seq 1/1
      
      // -- mod="$O2_RES" dstVar=tmp9/*res*/
      tmp9/*res*/ = 0.523121f;
      
      // -- mod="pow" dstVar=tmp9/*res*/
      tmp9/*res*/ = tmp9/*res*/ * tmp9/*res*/ * tmp9/*res*/;
      
      // ---- mod="svf" input "freq" seq 1/1
      
      // -- mod="$O2_FREQ" dstVar=tmp10/*freq*/
      tmp10/*freq*/ = 0.312139f;
      
      // -- mod="$out__freq_env" dstVar=tmp10/*freq*/
      float tmp14;
      tmp14/*seq*/ = tmp10/*freq*/;
      tmp14/*seq*/ = var_out__freq_env_tr_bd_5_0_;
      tmp10/*freq*/ += tmp14/*seq*/;
      
      // -- mod="0.1" dstVar=tmp10/*freq*/
      tmp10/*freq*/ *= 0.1f;
      tmp11_svf_lp = tmp11_svf_lp + (tmp13_svf_bp * tmp10/*freq*/);
      tmp12_svf_hp = tmp8/*seq*/ - tmp11_svf_lp - (tmp13_svf_bp * tmp9/*res*/);
      tmp13_svf_bp = tmp13_svf_bp + (tmp12_svf_hp * tmp10/*freq*/);
      tmp8/*seq*/ = tmp11_svf_lp;
      
      // -- mod="$O2_LVL" dstVar=tmp8/*seq*/
      tmp8/*seq*/ *= 0.627168f;
      out += tmp8/*seq*/;
      
      // -- mod="nos" dstVar=out
      tmp8/*seq*/ = out;
      tmp17/*nos_fb*/ = tmp15_nos_seed1 & 1;
      tmp15_nos_seed1 >>= 1;
      if(0 != tmp17/*nos_fb*/)
          tmp15_nos_seed1 ^= 0xB4BCD35C;
      tmp17/*nos_fb*/ = tmp16_nos_seed2 & 1;
      tmp16_nos_seed2 >>= 1;
      if(0 != tmp17/*nos_fb*/)
          tmp16_nos_seed2 ^= 0x7A5BC2E3;
      tmp8/*seq*/ = (((tmp15_nos_seed1 ^ tmp16_nos_seed2) & 0xFFFF) - 0x8000) * (1.0 / 32767.0);
      
      // -- mod="rmp" dstVar=tmp8/*seq*/
      tmp9/*seq*/ = tmp8/*seq*/;
      tmp9/*seq*/ = tmp18_rmp_cur;
      tmp18_rmp_cur = mathMaxf(tmp18_rmp_cur+tmp19_rmp_step__tr_bd_5_0_, 0);
      tmp8/*seq*/ *= tmp9/*seq*/;
      
      // -- mod="pow" dstVar=tmp8/*seq*/
      tmp8/*seq*/ = mathPowerf(tmp8/*seq*/, 9.79769f);
      
      // -- mod="svf" dstVar=tmp8/*seq*/
      tmp20_svf_lp = tmp20_svf_lp + (tmp22_svf_bp * 0.132948f);
      tmp21_svf_hp = tmp8/*seq*/ - tmp20_svf_lp - (tmp22_svf_bp * 0.16474f);
      tmp22_svf_bp = tmp22_svf_bp + (tmp21_svf_hp * 0.132948f);
      tmp8/*seq*/ = tmp21_svf_hp;
      
      // -- mod="$NOS_LVL" dstVar=tmp8/*seq*/
      tmp8/*seq*/ *= 0.130058f;
      out += tmp8/*seq*/;
      
      // -- mod="$P_AMP" dstVar=out
      out *= 1.31792f;
      
      // -- mod="ipl" dstVar=out
      tmp8/*a*/ = out;
      
      // ---- mod="ipl" input "a" seq 1/1
      
      // -- mod="tan" dstVar=tmp8/*a*/
      tmp8/*a*/ = tanf(tmp8/*a*/);
      
      // -- mod="svf" dstVar=tmp8/*a*/
      tmp23_svf_lp = tmp23_svf_lp + (tmp25_svf_bp * 0.234104f);
      tmp24_svf_hp = tmp8/*a*/ - tmp23_svf_lp - (tmp25_svf_bp * 1.0f);
      tmp25_svf_bp = tmp25_svf_bp + (tmp24_svf_hp * 0.234104f);
      tmp8/*a*/ = tmp23_svf_lp;
      tmp9/*t*/ = out;
      
      // ---- mod="ipl" input "t" seq 1/1
      
      // -- mod="$P_TAN_MIX" dstVar=tmp9/*t*/
      tmp9/*t*/ = 0.745665f;
      out = tmp8/*a*/ + (out - tmp8/*a*/) * tmp9/*t*/;
      
      // -- mod="clp" dstVar=out
      if(out > 0.999f) out = 0.999f;
      else if(out < -0.999f) out = -0.999f;
      

      _d[outIdx++] = out;
   } /* end loop frames */
} /* end calc() */

// ----- wavetable w=3 h=1 cyclelen=12000
static void calc_tr_bd_5_0_(float *_d) {
   int off = 0;
   var_wt_y_tr_bd_5_0_ = 0.0f;
   tmp2_rmp_step__tr_bd_5_0_ = -0.000629549f;
   tmp4_rmp_step__tr_bd_5_0_ = -0.0077509f;
   tmp15_nos_seed1 = 1033877898; // global
   tmp16_nos_seed2 = 1033877898; // global
   tmp19_rmp_step__tr_bd_5_0_ = -0.000514881f;
   loop(1)
   {
      var_wt_x_tr_bd_5_0_ = 0.0f;
      loop(3)
      {
         float *d = _d + off;
         calc_cycle_tr_bd_5_0_(d);
         var_wt_x_tr_bd_5_0_ += 0.5;
         off += 12000;
      }
   }
}



//         name: tr bd 7 off=250551 num=150000
//  sample_rate: 48000
//    note_freq: 261.63
//      #frames: 50000



static float var_x_tr_bd_7_0_ = 0.0f;
static float var_wt_x_tr_bd_7_0_ = 0.0f;
static float var_wt_y_tr_bd_7_0_ = 0.0f;
static float var_wt_freq_tr_bd_7_0_ = 0.0f;
static float var_out__freq_env_tr_bd_7_0_ = 0.0f;
static float var_out__trig_tr_bd_7_0_ = 0.0f;


float tmp2_rmp_step__tr_bd_7_0_;
float tmp4_rmp_step__tr_bd_7_0_;
int tmp15_nos_seed1;
int tmp16_nos_seed2;
float tmp19_rmp_step__tr_bd_7_0_;

static void calc_cycle_tr_bd_7_0_(float *_d) {
   float tmp1_rmp_cur = 1.0f;
   float tmp3_rmp_cur = 1.0f;
   float tmp5_svf_lp = 0.0f;
   float tmp6_svf_hp = 0.0f;
   float tmp7_svf_bp = 0.0f;
   float tmp11_svf_lp = 0.0f;
   float tmp12_svf_hp = 0.0f;
   float tmp13_svf_bp = 0.0f;
   short tmp17;
   float tmp18_rmp_cur = 1.0f;
   float tmp20_svf_lp = 0.0f;
   float tmp21_svf_hp = 0.0f;
   float tmp22_svf_bp = 0.0f;
   float tmp23_svf_lp = 0.0f;
   float tmp24_svf_hp = 0.0f;
   float tmp25_svf_bp = 0.0f;
   // start=1 end=0 #frames=2351.45:
   // start=1 end=0 #frames=158.15:
   // start=1 end=0 #frames=1595.38:
   
   tmp1_rmp_cur = 1.0f;
   tmp3_rmp_cur = 1.0f;
   tmp5_svf_lp = 0.0f;
   tmp6_svf_hp = 0.0f;
   tmp7_svf_bp = 0.0f;
   tmp11_svf_lp = 0.0f;
   tmp12_svf_hp = 0.0f;
   tmp13_svf_bp = 0.0f;
   tmp18_rmp_cur = 1.0f;
   tmp20_svf_lp = 0.0f;
   tmp21_svf_hp = 0.0f;
   tmp22_svf_bp = 0.0f;
   tmp23_svf_lp = 0.0f;
   tmp24_svf_hp = 0.0f;
   tmp25_svf_bp = 0.0f;
   int smpIdx = 0;
   int outIdx = 0;
   loop(50000)
   {
      float out = 0.0f;
      
      // ========
      // ======== lane "out" modIdx=0 modString=rmp
      // ========
      
      // -- mod="rmp" dstVar=out
      out = tmp1_rmp_cur;
      tmp1_rmp_cur = mathMaxf(tmp1_rmp_cur+tmp2_rmp_step__tr_bd_7_0_, 0);
      
      // -- mod="pow" dstVar=out
      out = mathPowerf(out, 7.31214f);
      
      // -- mod="$O_FREQ_ENV_LVL" dstVar=out
      out *= 0.32948f;
      
      // -- mod="set out__freq_env" dstVar=out
      var_out__freq_env_tr_bd_7_0_ = out;
      
      // -- mod="rmp" dstVar=out
      out = tmp3_rmp_cur;
      tmp3_rmp_cur = mathMaxf(tmp3_rmp_cur+tmp4_rmp_step__tr_bd_7_0_, 0);
      
      // -- mod="pow" dstVar=out
      out = mathPowerf(out, 2.60116f);
      
      // -- mod="set out__trig" dstVar=out
      var_out__trig_tr_bd_7_0_ = out;
      
      // -- mod="svf" dstVar=out
      float tmp8;
      
      // ---- mod="svf" input "res" seq 1/1
      
      // -- mod="$O1_RES" dstVar=tmp8/*res*/
      tmp8/*res*/ = 0.234104f;
      
      // -- mod="pow" dstVar=tmp8/*res*/
      tmp8/*res*/ = tmp8/*res*/ * tmp8/*res*/ * tmp8/*res*/;
      float tmp9;
      
      // ---- mod="svf" input "freq" seq 1/1
      
      // -- mod="$O1_FREQ" dstVar=tmp9/*freq*/
      tmp9/*freq*/ = 0.15885f;
      
      // -- mod="$out__freq_env" dstVar=tmp9/*freq*/
      float tmp10;
      tmp10/*seq*/ = tmp9/*freq*/;
      tmp10/*seq*/ = var_out__freq_env_tr_bd_7_0_;
      tmp9/*freq*/ += tmp10/*seq*/;
      
      // -- mod="0.1" dstVar=tmp9/*freq*/
      tmp9/*freq*/ *= 0.1f;
      tmp5_svf_lp = tmp5_svf_lp + (tmp7_svf_bp * tmp9/*freq*/);
      tmp6_svf_hp = out - tmp5_svf_lp - (tmp7_svf_bp * tmp8/*res*/);
      tmp7_svf_bp = tmp7_svf_bp + (tmp6_svf_hp * tmp9/*freq*/);
      out = tmp5_svf_lp;
      
      // -- mod="$O1_LVL" dstVar=out
      out *= 0.66763f;
      
      // -- mod="$out__trig" dstVar=out
      tmp8/*seq*/ = out;
      tmp8/*seq*/ = var_out__trig_tr_bd_7_0_;
      
      // -- mod="svf" dstVar=tmp8/*seq*/
      
      // ---- mod="svf" input "res" seq 1/1
      
      // -- mod="$O2_RES" dstVar=tmp9/*res*/
      tmp9/*res*/ = 0.401734f;
      
      // -- mod="pow" dstVar=tmp9/*res*/
      tmp9/*res*/ = tmp9/*res*/ * tmp9/*res*/ * tmp9/*res*/;
      
      // ---- mod="svf" input "freq" seq 1/1
      
      // -- mod="$O2_FREQ" dstVar=tmp10/*freq*/
      tmp10/*freq*/ = 0.312139f;
      
      // -- mod="$out__freq_env" dstVar=tmp10/*freq*/
      float tmp14;
      tmp14/*seq*/ = tmp10/*freq*/;
      tmp14/*seq*/ = var_out__freq_env_tr_bd_7_0_;
      tmp10/*freq*/ += tmp14/*seq*/;
      
      // -- mod="0.1" dstVar=tmp10/*freq*/
      tmp10/*freq*/ *= 0.1f;
      tmp11_svf_lp = tmp11_svf_lp + (tmp13_svf_bp * tmp10/*freq*/);
      tmp12_svf_hp = tmp8/*seq*/ - tmp11_svf_lp - (tmp13_svf_bp * tmp9/*res*/);
      tmp13_svf_bp = tmp13_svf_bp + (tmp12_svf_hp * tmp10/*freq*/);
      tmp8/*seq*/ = tmp11_svf_lp;
      
      // -- mod="$O2_LVL" dstVar=tmp8/*seq*/
      tmp8/*seq*/ *= 0.627168f;
      out += tmp8/*seq*/;
      
      // -- mod="nos" dstVar=out
      tmp8/*seq*/ = out;
      tmp17/*nos_fb*/ = tmp15_nos_seed1 & 1;
      tmp15_nos_seed1 >>= 1;
      if(0 != tmp17/*nos_fb*/)
          tmp15_nos_seed1 ^= 0xB4BCD35C;
      tmp17/*nos_fb*/ = tmp16_nos_seed2 & 1;
      tmp16_nos_seed2 >>= 1;
      if(0 != tmp17/*nos_fb*/)
          tmp16_nos_seed2 ^= 0x7A5BC2E3;
      tmp8/*seq*/ = (((tmp15_nos_seed1 ^ tmp16_nos_seed2) & 0xFFFF) - 0x8000) * (1.0 / 32767.0);
      
      // -- mod="rmp" dstVar=tmp8/*seq*/
      tmp9/*seq*/ = tmp8/*seq*/;
      tmp9/*seq*/ = tmp18_rmp_cur;
      tmp18_rmp_cur = mathMaxf(tmp18_rmp_cur+tmp19_rmp_step__tr_bd_7_0_, 0);
      tmp8/*seq*/ *= tmp9/*seq*/;
      
      // -- mod="pow" dstVar=tmp8/*seq*/
      tmp8/*seq*/ = mathPowerf(tmp8/*seq*/, 5.11561f);
      
      // -- mod="svf" dstVar=tmp8/*seq*/
      tmp20_svf_lp = tmp20_svf_lp + (tmp22_svf_bp * 0.132948f);
      tmp21_svf_hp = tmp8/*seq*/ - tmp20_svf_lp - (tmp22_svf_bp * 0.193642f);
      tmp22_svf_bp = tmp22_svf_bp + (tmp21_svf_hp * 0.132948f);
      tmp8/*seq*/ = tmp21_svf_hp;
      
      // -- mod="$NOS_LVL" dstVar=tmp8/*seq*/
      tmp8/*seq*/ *= 0.00578035f;
      out += tmp8/*seq*/;
      
      // -- mod="$P_AMP" dstVar=out
      out *= 1.31792f;
      
      // -- mod="ipl" dstVar=out
      tmp8/*a*/ = out;
      
      // ---- mod="ipl" input "a" seq 1/1
      
      // -- mod="tan" dstVar=tmp8/*a*/
      tmp8/*a*/ = tanf(tmp8/*a*/);
      
      // -- mod="svf" dstVar=tmp8/*a*/
      tmp23_svf_lp = tmp23_svf_lp + (tmp25_svf_bp * 0.15202f);
      tmp24_svf_hp = tmp8/*a*/ - tmp23_svf_lp - (tmp25_svf_bp * 1.0f);
      tmp25_svf_bp = tmp25_svf_bp + (tmp24_svf_hp * 0.15202f);
      tmp8/*a*/ = tmp23_svf_lp;
      tmp9/*t*/ = out;
      
      // ---- mod="ipl" input "t" seq 1/1
      
      // -- mod="$P_TAN_MIX" dstVar=tmp9/*t*/
      tmp9/*t*/ = 0.777457f;
      out = tmp8/*a*/ + (out - tmp8/*a*/) * tmp9/*t*/;
      
      // -- mod="clp" dstVar=out
      if(out > 0.999f) out = 0.999f;
      else if(out < -0.999f) out = -0.999f;
      

      _d[outIdx++] = out;
   } /* end loop frames */
} /* end calc() */

// ----- wavetable w=3 h=1 cyclelen=50000
static void calc_tr_bd_7_0_(float *_d) {
   int off = 0;
   var_wt_y_tr_bd_7_0_ = 0.0f;
   tmp2_rmp_step__tr_bd_7_0_ = -0.00042527f;
   tmp4_rmp_step__tr_bd_7_0_ = -0.0063231f;
   tmp15_nos_seed1 = 1033877898; // global
   tmp16_nos_seed2 = 1033877898; // global
   tmp19_rmp_step__tr_bd_7_0_ = -0.000626812f;
   loop(1)
   {
      var_wt_x_tr_bd_7_0_ = 0.0f;
      loop(3)
      {
         float *d = _d + off;
         calc_cycle_tr_bd_7_0_(d);
         var_wt_x_tr_bd_7_0_ += 0.5;
         off += 50000;
      }
   }
}



//         name: tri rndphase wt 1 off=400551 num=8192
//  sample_rate: 48000
//    note_freq: 187.5
//      #frames: 256
// #skip_frames: 256



static float var_x_tri_rndphase_wt_1_0_ = 0.0f;
static float var_wt_x_tri_rndphase_wt_1_0_ = 0.0f;
static float var_wt_y_tri_rndphase_wt_1_0_ = 0.0f;
static float var_wt_freq_tri_rndphase_wt_1_0_ = 0.0f;
static float var_v_phase_tri_rndphase_wt_1_0_ = 0.0f;


int tmp1_nos_seed1;
int tmp2_nos_seed2;
float tmp5_tri_speed__tri_rndphase_wt_1_0_;


static void wt_init_tri_rndphase_wt_1_0_(void) {
   short tmp3;
   float out = 0.0f;
   (void)out;
   // -------- lane "wt_init" modIdx=0 modString=nos
   
   // -- mod="nos" dstVar=out
   tmp3/*nos_fb*/ = tmp1_nos_seed1 & 1;
   tmp1_nos_seed1 >>= 1;
   if(0 != tmp3/*nos_fb*/)
       tmp1_nos_seed1 ^= 0xB4BCD35C;
   tmp3/*nos_fb*/ = tmp2_nos_seed2 & 1;
   tmp2_nos_seed2 >>= 1;
   if(0 != tmp3/*nos_fb*/)
       tmp2_nos_seed2 ^= 0x7A5BC2E3;
   out = (((tmp1_nos_seed1 ^ tmp2_nos_seed2) & 0xFFFF) - 0x8000) * (1.0 / 32767.0);
   
   // -- mod="fam" dstVar=out
   out = (out + 1.0f) * 0.5f;
   
   // -- mod="sto v_phase" dstVar=out
   var_v_phase_tri_rndphase_wt_1_0_ = out;
   
} /* end wt_init */
static void calc_cycle_tri_rndphase_wt_1_0_(float *_d) {
   float tmp4_tri_phase = 0.0f;
   float tmp8_spr_cur_spd = 0.0f;
   float tmp9_spr_cur_out = 0.0f;
   float tmp10_svf_lp = 0.0f;
   float tmp11_svf_hp = 0.0f;
   float tmp12_svf_bp = 0.0f;
   tmp4_tri_phase = 0.0f;
   tmp8_spr_cur_spd = 0.0f;
   tmp9_spr_cur_out = 0.0f;
   tmp10_svf_lp = 0.0f;
   tmp11_svf_hp = 0.0f;
   tmp12_svf_bp = 0.0f;

   wt_init_tri_rndphase_wt_1_0_();
   int smpIdx = 0;
   int outIdx = 0;
   loop(512)
   {
      float out = 0.0f;
      
      // ========
      // ======== lane "out" modIdx=0 modString=tri
      // ========
      
      // -- mod="tri" dstVar=out
      float tmp6;
      float tmp7;
      
      // ---- mod="tri" input "phase" seq 1/1
      
      // -- mod="$v_phase" dstVar=tmp7/*phase*/
      tmp7/*phase*/ = var_v_phase_tri_rndphase_wt_1_0_;
      
      // -- mod="0.1868" dstVar=tmp7/*phase*/
      tmp7/*phase*/ *= 0.1868f;
      tmp6/*tri_tmp*/ = ((tmp4_tri_phase + tmp7/*phase*/));
      tmp6/*tri_tmp*/ = tmp6/*tri_tmp*/ * 6.01156f;
      tmp6/*tri_tmp*/ = ffrac_s(tmp6/*tri_tmp*/);
      out = (tmp6/*tri_tmp*/ < 0.5f) ? (-1.0f + tmp6/*tri_tmp*/ * 4.0f) : (1.0f - (tmp6/*tri_tmp*/ - 0.5f)*4);
      tmp4_tri_phase = ffrac_s(tmp4_tri_phase + tmp5_tri_speed__tri_rndphase_wt_1_0_);
      
      // -- mod="spr" dstVar=out
      
      // ---- mod="spr" input "spd" seq 1/1
      
      // -- mod="$P_SPR_SPD" dstVar=tmp6/*spd*/
      tmp6/*spd*/ = 0.0627168f;
      
      // ---- mod="spr" input "dcy" seq 1/1
      
      // -- mod="$P_SPR_DCY" dstVar=tmp7/*dcy*/
      tmp7/*dcy*/ = 0.157803f;
      tmp8_spr_cur_spd += (out - tmp9_spr_cur_out) * tmp6/*spd*/;
      tmp9_spr_cur_out += tmp8_spr_cur_spd;
      tmp8_spr_cur_spd *= tmp7/*dcy*/;
      out = tmp9_spr_cur_out;
      
      // -- mod="svf" dstVar=out
      tmp10_svf_lp = tmp10_svf_lp + (tmp12_svf_bp * 0.0982659f);
      tmp11_svf_hp = out - tmp10_svf_lp - (tmp12_svf_bp * 0.950867f);
      tmp12_svf_bp = tmp12_svf_bp + (tmp11_svf_hp * 0.0982659f);
      out = tmp10_svf_lp;
      
      // -- mod="$P_AMP" dstVar=out
      out *= 1.0f;
      
      // -- mod="clp" dstVar=out
      if(out > 0.999f) out = 0.999f;
      else if(out < -0.999f) out = -0.999f;
      

      if(smpIdx++ >= 256)
         _d[outIdx++] = out;
   } /* end loop frames */
} /* end calc() */

// ----- wavetable w=32 h=1 cyclelen=256
static void calc_tri_rndphase_wt_1_0_(float *_d) {
   int off = 0;
   var_wt_y_tri_rndphase_wt_1_0_ = 0.0f;
   tmp1_nos_seed1 = 1033877898; // global
   tmp2_nos_seed2 = 1033877898; // global
   tmp5_tri_speed__tri_rndphase_wt_1_0_ = 0.00390625f;
   loop(1)
   {
      var_wt_x_tri_rndphase_wt_1_0_ = 0.0f;
      loop(32)
      {
         float *d = _d + off;
         calc_cycle_tri_rndphase_wt_1_0_(d);
         var_wt_x_tri_rndphase_wt_1_0_ += 0.0322581;
         off += 256;
      }
   }
}



//         name: tri rndphase wt 2 off=408743 num=8192
//  sample_rate: 48000
//    note_freq: 187.5
//      #frames: 256
// #skip_frames: 256



static float var_x_tri_rndphase_wt_2_0_ = 0.0f;
static float var_wt_x_tri_rndphase_wt_2_0_ = 0.0f;
static float var_wt_y_tri_rndphase_wt_2_0_ = 0.0f;
static float var_wt_freq_tri_rndphase_wt_2_0_ = 0.0f;
static float var_v_phase_tri_rndphase_wt_2_0_ = 0.0f;


int tmp1_nos_seed1;
int tmp2_nos_seed2;
float tmp5_tri_speed__tri_rndphase_wt_2_0_;


static void wt_init_tri_rndphase_wt_2_0_(void) {
   short tmp3;
   float out = 0.0f;
   (void)out;
   // -------- lane "wt_init" modIdx=0 modString=nos
   
   // -- mod="nos" dstVar=out
   tmp3/*nos_fb*/ = tmp1_nos_seed1 & 1;
   tmp1_nos_seed1 >>= 1;
   if(0 != tmp3/*nos_fb*/)
       tmp1_nos_seed1 ^= 0xB4BCD35C;
   tmp3/*nos_fb*/ = tmp2_nos_seed2 & 1;
   tmp2_nos_seed2 >>= 1;
   if(0 != tmp3/*nos_fb*/)
       tmp2_nos_seed2 ^= 0x7A5BC2E3;
   out = (((tmp1_nos_seed1 ^ tmp2_nos_seed2) & 0xFFFF) - 0x8000) * (1.0 / 32767.0);
   
   // -- mod="fam" dstVar=out
   out = (out + 1.0f) * 0.5f;
   
   // -- mod="sto v_phase" dstVar=out
   var_v_phase_tri_rndphase_wt_2_0_ = out;
   
} /* end wt_init */
static void calc_cycle_tri_rndphase_wt_2_0_(float *_d) {
   float tmp4_tri_phase = 0.0f;
   float tmp8_spr_cur_spd = 0.0f;
   float tmp9_spr_cur_out = 0.0f;
   float tmp10_svf_lp = 0.0f;
   float tmp11_svf_hp = 0.0f;
   float tmp12_svf_bp = 0.0f;
   tmp4_tri_phase = 0.0f;
   tmp8_spr_cur_spd = 0.0f;
   tmp9_spr_cur_out = 0.0f;
   tmp10_svf_lp = 0.0f;
   tmp11_svf_hp = 0.0f;
   tmp12_svf_bp = 0.0f;

   wt_init_tri_rndphase_wt_2_0_();
   int smpIdx = 0;
   int outIdx = 0;
   loop(512)
   {
      float out = 0.0f;
      
      // ========
      // ======== lane "out" modIdx=0 modString=tri
      // ========
      
      // -- mod="tri" dstVar=out
      float tmp6;
      float tmp7;
      
      // ---- mod="tri" input "phase" seq 1/1
      
      // -- mod="$v_phase" dstVar=tmp7/*phase*/
      tmp7/*phase*/ = var_v_phase_tri_rndphase_wt_2_0_;
      
      // -- mod="0.1868" dstVar=tmp7/*phase*/
      tmp7/*phase*/ *= 0.1868f;
      tmp6/*tri_tmp*/ = ((tmp4_tri_phase + tmp7/*phase*/));
      tmp6/*tri_tmp*/ = tmp6/*tri_tmp*/ * 6.01156f;
      tmp6/*tri_tmp*/ = ffrac_s(tmp6/*tri_tmp*/);
      out = (tmp6/*tri_tmp*/ < 0.5f) ? (-1.0f + tmp6/*tri_tmp*/ * 4.0f) : (1.0f - (tmp6/*tri_tmp*/ - 0.5f)*4);
      tmp4_tri_phase = ffrac_s(tmp4_tri_phase + tmp5_tri_speed__tri_rndphase_wt_2_0_);
      
      // -- mod="spr" dstVar=out
      
      // ---- mod="spr" input "spd" seq 1/1
      
      // -- mod="$P_SPR_SPD" dstVar=tmp6/*spd*/
      tmp6/*spd*/ = 0.0627168f;
      
      // ---- mod="spr" input "dcy" seq 1/1
      
      // -- mod="$P_SPR_DCY" dstVar=tmp7/*dcy*/
      tmp7/*dcy*/ = 0.157803f;
      tmp8_spr_cur_spd += (out - tmp9_spr_cur_out) * tmp6/*spd*/;
      tmp9_spr_cur_out += tmp8_spr_cur_spd;
      tmp8_spr_cur_spd *= tmp7/*dcy*/;
      out = tmp9_spr_cur_out;
      
      // -- mod="svf" dstVar=out
      tmp10_svf_lp = tmp10_svf_lp + (tmp12_svf_bp * 0.0982659f);
      tmp11_svf_hp = out - tmp10_svf_lp - (tmp12_svf_bp * 0.950867f);
      tmp12_svf_bp = tmp12_svf_bp + (tmp11_svf_hp * 0.0982659f);
      out = tmp10_svf_lp;
      
      // -- mod="$P_AMP" dstVar=out
      out *= 1.0f;
      
      // -- mod="clp" dstVar=out
      if(out > 0.999f) out = 0.999f;
      else if(out < -0.999f) out = -0.999f;
      

      if(smpIdx++ >= 256)
         _d[outIdx++] = out;
   } /* end loop frames */
} /* end calc() */

// ----- wavetable w=32 h=1 cyclelen=256
static void calc_tri_rndphase_wt_2_0_(float *_d) {
   int off = 0;
   var_wt_y_tri_rndphase_wt_2_0_ = 0.0f;
   tmp1_nos_seed1 = 1033877898; // global
   tmp2_nos_seed2 = 1033877898; // global
   tmp5_tri_speed__tri_rndphase_wt_2_0_ = 0.00390625f;
   loop(1)
   {
      var_wt_x_tri_rndphase_wt_2_0_ = 0.0f;
      loop(32)
      {
         float *d = _d + off;
         calc_cycle_tri_rndphase_wt_2_0_(d);
         var_wt_x_tri_rndphase_wt_2_0_ += 0.0322581;
         off += 256;
      }
   }
}



//         name: noise atk off=416967 num=6525
//  sample_rate: 48000
//    note_freq: 261.63
//      #frames: 6525



static float var_x_noise_atk_0_ = 0.0f;
static float var_wt_x_noise_atk_0_ = 0.0f;
static float var_wt_y_noise_atk_0_ = 0.0f;
static float var_wt_freq_noise_atk_0_ = 0.0f;


int tmp5_nos_seed1__noise_atk_0_;
int tmp6_nos_seed2__noise_atk_0_;

static void calc_noise_atk_0_(float *_d) {
   short tmp1;
   unsigned short tmp2_lfsr_state = 17545;
   short tmp3_lfsr_state_signed;
   float tmp7_svf_lp = 0.0f;
   float tmp8_svf_hp = 0.0f;
   float tmp9_svf_bp = 0.0f;
   float tmp10_svf_lp = 0.0f;
   float tmp11_svf_hp = 0.0f;
   float tmp12_svf_bp = 0.0f;
   tmp5_nos_seed1__noise_atk_0_ = 15765;
   tmp6_nos_seed2__noise_atk_0_ = 15765;
   int smpIdx = 0;
   int outIdx = 0;
   loop(6525)
   {
      float out = 0.0f;
      
      // ========
      // ======== lane "out" modIdx=0 modString=fsr
      // ========
      
      // -- mod="fsr" dstVar=out
      tmp1/*i2f*/ = (int)(out * 2048);  // IntFallback: F2I
      tmp2_lfsr_state ^= tmp2_lfsr_state >> 7;
      tmp2_lfsr_state ^= tmp2_lfsr_state << 9;
      tmp2_lfsr_state ^= tmp2_lfsr_state >> 13;
      tmp3_lfsr_state_signed = (tmp2_lfsr_state & 65520);
      tmp1/*i2f*/ = tmp3_lfsr_state_signed >> 4;
      out = tmp1/*i2f*/ / ((float)(2048));  // IntFallback: I2F
      
      // -- mod="$P_FSR_LVL" dstVar=out
      out *= 1.0f;
      
      // -- mod="nos" dstVar=out
      float tmp4;
      tmp4/*seq*/ = out;
      tmp1/*nos_fb*/ = tmp5_nos_seed1__noise_atk_0_ & 1;
      tmp5_nos_seed1__noise_atk_0_ >>= 1;
      if(0 != tmp1/*nos_fb*/)
          tmp5_nos_seed1__noise_atk_0_ ^= 0xB4BCD35C;
      tmp1/*nos_fb*/ = tmp6_nos_seed2__noise_atk_0_ & 1;
      tmp6_nos_seed2__noise_atk_0_ >>= 1;
      if(0 != tmp1/*nos_fb*/)
          tmp6_nos_seed2__noise_atk_0_ ^= 0x7A5BC2E3;
      tmp4/*seq*/ = (((tmp5_nos_seed1__noise_atk_0_ ^ tmp6_nos_seed2__noise_atk_0_) & 0xFFFF) - 0x8000) * (1.0 / 32767.0);
      
      // -- mod="$P_NOS_LVL" dstVar=tmp4/*seq*/
      tmp4/*seq*/ *= 1.0f;
      out += tmp4/*seq*/;
      
      // -- mod="svf" dstVar=out
      tmp7_svf_lp = tmp7_svf_lp + (tmp9_svf_bp * 0.676301f);
      tmp8_svf_hp = out - tmp7_svf_lp - (tmp9_svf_bp * 0.5f);
      tmp9_svf_bp = tmp9_svf_bp + (tmp8_svf_hp * 0.676301f);
      out = tmp8_svf_hp;
      
      // -- mod="svf" dstVar=out
      tmp10_svf_lp = tmp10_svf_lp + (tmp12_svf_bp * 0.367052f);
      tmp11_svf_hp = out - tmp10_svf_lp - (tmp12_svf_bp * 0.5f);
      tmp12_svf_bp = tmp12_svf_bp + (tmp11_svf_hp * 0.367052f);
      out = tmp10_svf_lp;
      

      _d[outIdx++] = out;
   } /* end loop frames */
} /* end calc() */



//         name: water drop 1 off=423492 num=183
//  sample_rate: 48000
//    note_freq: 261.63
//      #frames: 183



static float var_x_water_drop_1_0_ = 0.0f;
static float var_wt_x_water_drop_1_0_ = 0.0f;
static float var_wt_y_water_drop_1_0_ = 0.0f;
static float var_wt_freq_water_drop_1_0_ = 0.0f;


float tmp2_sin_speed__water_drop_1_0_;

static void calc_water_drop_1_0_(float *_d) {
   float tmp1_sin_phase = 0.0f;
   tmp2_sin_speed__water_drop_1_0_ = 0.00545062f;
   int smpIdx = 0;
   int outIdx = 0;
   loop(183)
   {
      float out = 0.0f;
      
      // ========
      // ======== lane "out" modIdx=0 modString=sin
      // ========
      
      // -- mod="sin" dstVar=out
      float tmp3;
      tmp3/*sin_tmp*/ = (tmp1_sin_phase);
      tmp3/*sin_tmp*/ = ffrac_s(tmp3/*sin_tmp*/);
      out = cycle_sine_tbl_f[(unsigned short)(16384 * tmp3/*sin_tmp*/)&16383u];
      tmp1_sin_phase = ffrac_s(tmp1_sin_phase + tmp2_sin_speed__water_drop_1_0_);
      
      // -- mod="0.8" dstVar=out
      out *= 0.8f;
      

      _d[outIdx++] = out;
   } /* end loop frames */
} /* end calc() */


extern void sr_handle_cycle_sample_calc_finished (unsigned int _sampleIdx);
#define HSCF(a)  sr_handle_cycle_sample_calc_finished(a)

void cycle_calc_waveform_demo_2_r_sr(float *_wfAddr) {
   // smp: "cycle_rs" sz=1139 (1.11k) bProc=1
   calc_cycle_rs_0_rs                           (_wfAddr +      0);  // total=  1139 ( 4.44k)
   HSCF(0);
   // smp: "fmstack_hh_1" sz=32 (0.03k) bProc=1
   HSCF(1);
   // smp: "fmstack rubber drum 1" sz=15052 (14.69k) bProc=1
   calc_fmstack_rubber_drum_1_1_                (_wfAddr +   1171);  // total= 15052 (58.79k)
   HSCF(2);
   // smp: "fmstack rubber drum 2" sz=15052 (14.69k) bProc=1
   calc_fmstack_rubber_drum_2_1_                (_wfAddr +  16223);  // total= 15052 (58.79k)
   HSCF(3);
   // smp: "fmstack rubber drum 3" sz=15052 (14.69k) bProc=1
   calc_fmstack_rubber_drum_3_1_                (_wfAddr +  31275);  // total= 15052 (58.79k)
   HSCF(4);
   // smp: "fmstack modfm bass 1" sz=32 (0.03k) bProc=1
   HSCF(5);
   // smp: "fmstack waveshaper bass 1" sz=32 (0.03k) bProc=1
   HSCF(6);
   // smp: "fmstack pwm 3" sz=32 (0.03k) bProc=1
   HSCF(7);
   // smp: "fmstack pwm 3b" sz=32 (0.03k) bProc=1
   HSCF(8);
   // smp: "fmstack pwm 3c" sz=32 (0.03k) bProc=1
   HSCF(9);
   // smp: "buzzy pad 1" sz=32 (0.03k) bProc=1
   HSCF(10);
   // smp: "fmstack stepped ratio 2" sz=32 (0.03k) bProc=1
   HSCF(11);
   // smp: "fm 1" sz=24000 (23.43k) bProc=1
   calc_fm_1_0_                                 (_wfAddr +  46551);  // total= 24000 (93.75k)
   HSCF(12);
   // smp: "tr sd 1" sz=72000 (70.31k) bProc=1
   calc_tr_sd_1_0_                              (_wfAddr +  70551);  // total= 72000 (281.25k) wt_3x1x24000
   HSCF(13);
   // smp: "tr sd 2" sz=72000 (70.31k) bProc=1
   calc_tr_sd_2_0_                              (_wfAddr + 142551);  // total= 72000 (281.25k) wt_3x1x24000
   HSCF(14);
   // smp: "tr bd 5" sz=36000 (35.15k) bProc=1
   calc_tr_bd_5_0_                              (_wfAddr + 214551);  // total= 36000 (140.62k) wt_3x1x12000
   HSCF(15);
   // smp: "tr bd 7" sz=150000 (146.48k) bProc=1
   calc_tr_bd_7_0_                              (_wfAddr + 250551);  // total=150000 (585.93k) wt_3x1x50000
   HSCF(16);
   // smp: "tri rndphase wt 1" sz=8192 (8k) bProc=1
   calc_tri_rndphase_wt_1_0_                    (_wfAddr + 400551);  // total=  8192 (   32k) wt_32x1x256
   HSCF(17);
   // smp: "tri rndphase wt 2" sz=8192 (8k) bProc=1
   calc_tri_rndphase_wt_2_0_                    (_wfAddr + 408743);  // total=  8192 (   32k) wt_32x1x256
   HSCF(18);
   // smp: "fmstack_hh_2" sz=32 (0.03k) bProc=1
   HSCF(19);
   // smp: "noise atk" sz=6525 (6.37k) bProc=1
   calc_noise_atk_0_                            (_wfAddr + 416967);  // total=  6525 (25.48k)
   HSCF(20);
   // smp: "water drop 1" sz=183 (0.17k) bProc=1
   calc_water_drop_1_0_                         (_wfAddr + 423492);  // total=   183 ( 0.71k)
   HSCF(21);
   // smp: "rain1" sz=262144 (256k) bProc=1
   HSCF(22);
   // smp: "rain2" sz=524288 (512k) bProc=1
   HSCF(23);
   // procedural sample size: 4840428 (4726.98k)
   // static     sample size: 0 (0k)
   // total      sample size: 4840428 (4726.98k)
}
