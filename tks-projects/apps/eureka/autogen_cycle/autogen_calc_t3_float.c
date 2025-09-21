// auto-generated on 20250612-141352

//         name: lcurve test 3 off=0 num=19278
//  sample_rate: 48000
//    note_freq: 261.63
//      #frames: 19278



static float var_x_lcurve_test_3_0_ = 0.0f;
static float var_wt_x_lcurve_test_3_0_ = 0.0f;
static float var_wt_y_lcurve_test_3_0_ = 0.0f;
static float var_wt_freq_lcurve_test_3_0_ = 0.0f;

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
static short curve_16_lcurve_test_3_0_[256] = {
0, 42, 83, 124, 165, 204, 243, 281, 319, 355, 391, 425, 459, 492, 525, 556, 586, 615, 643, 671, 697, 722, 746, 769, 791, 812, 831, 850, 867, 884, 899, 913, 
926, 939, 950, 960, 969, 977, 984, 990, 996, 1000, 1004, 1007, 1010, 1012, 1013, 1014, 1014, 1014, 1013, 1012, 1010, 1009, 1007, 1005, 1002, 998, 993, 988, 981, 973, 965, 955, 
945, 933, 921, 908, 893, 878, 862, 846, 828, 809, 790, 770, 749, 727, 704, 681, 657, 632, 606, 579, 552, 524, 496, 467, 437, 406, 375, 343, 311, 278, 245, 211, 
177, 142, 107, 71, 35, -1, -38, -75, -113, -151, -189, -227, -266, -305, -344, -383, -422, -461, -501, -541, -580, -620, -659, -699, -738, -778, -817, -856, -895, -934, -973, -1011, 
-1049, -1087, -1125, -1162, -1199, -1236, -1272, -1308, -1343, -1378, -1412, -1446, -1480, -1513, -1545, -1577, -1609, -1639, -1670, -1699, -1728, -1756, -1784, -1811, -1837, -1862, -1887, -1911, -1935, -1957, -1979, -2000, 
-2020, -2040, -2058, -2076, -2093, -2110, -2125, -2139, -2153, -2166, -2178, -2189, -2199, -2209, -2217, -2225, -2231, -2237, -2242, -2246, -2249, -2251, -2253, -2253, -2252, -2251, -2249, -2246, -2241, -2236, -2230, -2223, 
-2216, -2207, -2197, -2187, -2175, -2163, -2150, -2135, -2120, -2104, -2087, -2070, -2051, -2031, -2011, -1990, -1967, -1944, -1920, -1896, -1870, -1843, -1816, -1788, -1758, -1728, -1698, -1666, -1633, -1600, -1566, -1531, 
-1495, -1458, -1421, -1383, -1344, -1304, -1263, -1222, -1179, -1136, -1092, -1048, -1002, -956, -909, -862, -813, -764, -714, -663, -612, -560, -507, -453, -399, -344, -288, -232, -175, -117, -59, 0, 
};
static short curve_17_lcurve_test_3_0_[32] = {
0, 460, 861, 1204, 1489, 1716, 1885, 1998, 2054, 2054, 1999, 1890, 1728, 1514, 1250, 939, 584, 190, -236, -681, -1115, -1497, -1790, -1972, -2033, -1981, -1827, -1586, -1272, -897, -470, 0, 
};
static short curve_18_lcurve_test_3_0_[32] = {
0, 460, 861, 1204, 1489, 1716, 1885, 1998, 2054, 2054, 1999, 1890, 1728, 1514, 1250, 939, 584, 190, -236, -681, -1115, -1497, -1790, -1972, -2033, -1981, -1827, -1586, -1272, -897, -470, 0, 
};
static short curve_19_lcurve_test_3_0_[32] = {
0, 66, 132, 198, 264, 330, 396, 462, 528, 594, 660, 726, 792, 858, 924, 990, 1057, 1123, 1189, 1255, 1321, 1387, 1453, 1519, 1585, 1651, 1717, 1783, 1849, 1915, 1981, 2048, 
};


static void calc_lcurve_test_3_0_(float *_d) {
   float tmp1_pha_phase = 0.0f;
   short tmp5;
   unsigned short tmp6_lfsr_state;
   short tmp7_lfsr_state_signed;
   float tmp8_slw_last = 0.0f;
   short tmp11;
   float tmp12_pha_phase = 0.0f;
   unsigned short tmp13_lfsr_state;
   short tmp14_lfsr_state_signed;
   float tmp15_slw_last = 0.0f;
   float tmp2;
   
   int smpIdx = 0;
   int outIdx = 0;
   loop(19278)
   {
      float out = 0.0f;
      
      // ========
      // ======== lane "out" modIdx=0 modString=call _calc_myosc1
      // ========
      
      // -- mod="call _calc_myosc1" dstVar=out
      // ........ template lane "_calc_myosc1" modIdx=0 mod=pha
      
      // -- mod="pha" dstVar=out
      float tmp3;
      
      // ---- mod="pha" input "freq" seq 1/1
      
      // -- mod="1" dstVar=tmp3/*pha_freq*/
      tmp3/*pha_freq*/ = 1.0f;
      
      // -- mod="fsr" dstVar=tmp3/*pha_freq*/
      float tmp4;
      tmp4/*seq*/ = tmp3/*pha_freq*/;
      tmp5/*i2f*/ = (int)(tmp4/*seq*/ * 2048);  // IntFallback: F2I
      tmp6_lfsr_state ^= tmp6_lfsr_state >> 7;
      tmp6_lfsr_state ^= tmp6_lfsr_state << 9;
      tmp6_lfsr_state ^= tmp6_lfsr_state >> 13;
      tmp7_lfsr_state_signed = (tmp6_lfsr_state & 65520);
      tmp5/*i2f*/ = tmp7_lfsr_state_signed >> 4;
      tmp4/*seq*/ = tmp5/*i2f*/ / ((float)(2048));  // IntFallback: I2F
      
      // -- mod="slw" dstVar=tmp4/*seq*/
      float tmp9;
      
      // ---- mod="slw" input "up" seq 1/1
      
      // -- mod="$P_RNDSLW_1" dstVar=tmp9/*up*/
      tmp9/*up*/ = 0.445087f;
      float tmp10;
      
      // ---- mod="slw" input "down" seq 1/1
      
      // -- mod="$P_RNDSLW_1" dstVar=tmp10/*down*/
      tmp10/*down*/ = 0.445087f;
      if(tmp8_slw_last < tmp4/*seq*/)
         tmp4/*seq*/ = tmp8_slw_last + (tmp4/*seq*/ - tmp8_slw_last) * tmp9/*up*/;
      else
         tmp4/*seq*/ = tmp8_slw_last + (tmp4/*seq*/ - tmp8_slw_last) * tmp10/*down*/;
      tmp8_slw_last = tmp4/*seq*/;
      
      // -- mod="$P_RNDAMT_1" dstVar=tmp4/*seq*/
      tmp4/*seq*/ *= 0.0982659f;
      tmp3/*pha_freq*/ += tmp4/*seq*/;
      tmp2/*pha_speed*/ = 0.00545062f * tmp3/*pha_freq*/;
      tmp3/*pha_tmp*/ = (tmp1_pha_phase);
      out = ffrac_s(tmp3/*pha_tmp*/);
      tmp1_pha_phase = ffrac_s(tmp1_pha_phase + tmp2/*pha_speed*/);
      
      // -- mod="lut" dstVar=out
      tmp2/*lut_f*/ = (out * 256);
      tmp5/*lut_idx_a*/ = (int)tmp2/*lut_f*/;
      tmp3/*lut_frac*/ = tmp2/*lut_f*/ - (float)tmp5/*lut_idx_a*/;
      tmp4/*lut_a*/ = curve_16_lcurve_test_3_0_[( (unsigned int)tmp5/*lut_idx_a*/      ) & 255] * (1.0f / 2048);
      tmp9/*lut_b*/ = curve_16_lcurve_test_3_0_[(((unsigned int)tmp5/*lut_idx_a*/) + 1u) & 255] * (1.0f / 2048);
      out = tmp4/*lut_a*/ + (tmp9/*lut_b*/ - tmp4/*lut_a*/) * tmp3/*lut_frac*/;
      
      // -- mod="$P_AMP_1" dstVar=out
      out *= 0.387283f;
      
      // -- mod="call _calc_myosc2" dstVar=out
      tmp2/*seq*/ = out;
      // ........ template lane "_calc_myosc2" modIdx=0 mod=pha
      
      // -- mod="pha" dstVar=tmp2/*seq*/
      
      // ---- mod="pha" input "freq" seq 1/1
      
      // -- mod="1.5" dstVar=tmp4/*pha_freq*/
      tmp4/*pha_freq*/ = 1.5f;
      
      // -- mod="fsr" dstVar=tmp4/*pha_freq*/
      tmp9/*seq*/ = tmp4/*pha_freq*/;
      tmp5/*i2f*/ = (int)(tmp9/*seq*/ * 2048);  // IntFallback: F2I
      tmp13_lfsr_state ^= tmp13_lfsr_state >> 7;
      tmp13_lfsr_state ^= tmp13_lfsr_state << 9;
      tmp13_lfsr_state ^= tmp13_lfsr_state >> 13;
      tmp14_lfsr_state_signed = (tmp13_lfsr_state & 65520);
      tmp5/*i2f*/ = tmp14_lfsr_state_signed >> 4;
      tmp9/*seq*/ = tmp5/*i2f*/ / ((float)(2048));  // IntFallback: I2F
      
      // -- mod="slw" dstVar=tmp9/*seq*/
      
      // ---- mod="slw" input "up" seq 1/1
      
      // -- mod="$P_RNDSLW_2" dstVar=tmp10/*up*/
      tmp10/*up*/ = 0.190751f;
      float tmp16;
      
      // ---- mod="slw" input "down" seq 1/1
      
      // -- mod="$P_RNDSLW_2" dstVar=tmp16/*down*/
      tmp16/*down*/ = 0.190751f;
      if(tmp15_slw_last < tmp9/*seq*/)
         tmp9/*seq*/ = tmp15_slw_last + (tmp9/*seq*/ - tmp15_slw_last) * tmp10/*up*/;
      else
         tmp9/*seq*/ = tmp15_slw_last + (tmp9/*seq*/ - tmp15_slw_last) * tmp16/*down*/;
      tmp15_slw_last = tmp9/*seq*/;
      
      // -- mod="$P_RNDAMT_2" dstVar=tmp9/*seq*/
      tmp9/*seq*/ *= 0.187861f;
      tmp4/*pha_freq*/ += tmp9/*seq*/;
      tmp3/*pha_speed*/ = 0.00545062f * tmp4/*pha_freq*/;
      tmp4/*pha_tmp*/ = (tmp12_pha_phase);
      tmp2/*seq*/ = ffrac_s(tmp4/*pha_tmp*/);
      tmp12_pha_phase = ffrac_s(tmp12_pha_phase + tmp3/*pha_speed*/);
      
      // -- mod="lut" dstVar=tmp2/*seq*/
      tmp3/*lut_f*/ = (tmp2/*seq*/ * 32);
      tmp5/*lut_idx_a*/ = (int)tmp3/*lut_f*/;
      tmp4/*lut_frac*/ = tmp3/*lut_f*/ - (float)tmp5/*lut_idx_a*/;
      tmp9/*lut_a*/ = curve_18_lcurve_test_3_0_[( (unsigned int)tmp5/*lut_idx_a*/      ) & 31] * (1.0f / 2048);
      tmp10/*lut_b*/ = curve_18_lcurve_test_3_0_[(((unsigned int)tmp5/*lut_idx_a*/) + 1u) & 31] * (1.0f / 2048);
      tmp2/*seq*/ = tmp9/*lut_a*/ + (tmp10/*lut_b*/ - tmp9/*lut_a*/) * tmp4/*lut_frac*/;
      
      // -- mod="$P_AMP_2" dstVar=tmp2/*seq*/
      tmp2/*seq*/ *= 0.317919f;
      out += tmp2/*seq*/;
      
      // -- mod="clp" dstVar=out
      if(out > 0.999f) out = 0.999f;
      else if(out < -0.999f) out = -0.999f;
      

      _d[outIdx++] = out;
   } /* end loop frames */
} /* end calc() */


void cycle_calc_waveform_t3(float *_wfAddr) {
   // smp: "lcurve test 3" sz=19278 (18.82k) bProc=1
   calc_lcurve_test_3_0_                        (_wfAddr +      0);  // total= 19278 ( 75.3k)
   // procedural sample size: 77112 (75.3k)
   // static     sample size: 0 (0k)
   // total      sample size: 77112 (75.3k)
}
