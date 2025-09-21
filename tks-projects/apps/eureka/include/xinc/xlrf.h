
/*
 * from https://www.musicdsp.org/en/latest/Filters/266-4th-order-linkwitz-riley-filters.html
 *
 *  "The sum of the Linkwitz-Riley (Butterworth squared) HP and LP outputs, will result an
 *    all-pass filter at Fc and flat magnitude response - close to ideal for crossovers."
 *      -- Lubomir I. Ivanov
 *
 *
 *  Implementation by moc.snoitcudorpnrec@mij
 */


/*
 * Module definition (parsed by Cycle):
 *
 *  @cycle_mod      xlrf
 *  @cycle_pre      // %cymod% pre-include code line
 *  @cycle_static   // %cymod% module-static test code line
 *  @cycle_new      // %cyid% voice_new() test code line
 *  @cycle_delete   // %cyid% voice_delete() test code line
 *  @cycle_voice    LRCrossoverFilter %cyid%;
 *  @cycle_noteon   voice->%cyid%.reset();
 *  @cycle_input    freq 0.5
 *  @cycle_config   mode 0  0=lpf 1=hpf
 *  @cycle_config   rep 1
 *  @cycle_emit     voice->%cyid%.setup(%cyin_freq%);
 *  @cycle_emit     loop(%cycf_rep%) { %cyio% = (0 == %cycf_mode%) ? voice->%cyid%.processLP(%cyio%) : voice->%cyid%.processHP(%cyio%); }
 *
 */

/* #define T float */
#define T double


class LRCrossoverFilter { // LR4 crossover filter
  private:
   struct filterCoefficents {
      T a0, a1, a2, a3, a4;
   } lpco, hpco;

   T b1co, b2co, b3co, b4co;

   struct {
      T xm1, xm2, xm3, xm4;
      T ym1, ym2, ym3, ym4;
   } lptemp, hptemp;

   T coFreqRunningAv = 100.0f;

  public:
   void reset (void) {
      lptemp.xm1 = lptemp.xm2 = lptemp.xm3 = lptemp.xm4 = 0.0f;
      lptemp.ym1 = lptemp.ym2 = lptemp.ym3 = lptemp.ym4 = 0.0f;

      hptemp.xm1 = hptemp.xm2 = hptemp.xm3 = hptemp.xm4 = 0.0f;
      hptemp.ym1 = hptemp.ym2 = hptemp.ym3 = hptemp.ym4 = 0.0f;
   }

   void setup (float crossoverFrequency/*0..1*/);
   float processHP (float _in);
   float processLP (float _in);

   /* void processBlock(float * in, float * outHP, float * outLP, int numSamples); */
   /* void dumpCoefficents(struct filterCoefficents x) { */
   /*     std::cout << "a0: " << x.a0 << "\n"; */
   /*     std::cout << "a1: " << x.a1 << "\n"; */
   /*     std::cout << "a2: " << x.a2 << "\n"; */
   /*     std::cout << "a3: " << x.a3 << "\n"; */
   /*     std::cout << "a4: " << x.a4 << "\n"; */
   /* } */
   /* void dumpInformation() { */
   /*     std::cout << "-----\nfrequency: "<< coFreqRunningAv << "\n"; */
   /*     std::cout << "lpco:\n"; */
   /*     dumpCoefficents(lpco); */
   /*     std::cout << "hpco:\n"; */
   /*     dumpCoefficents(hpco); */
   /*     std::cout << "bco:\nb1: "; */
   /*     std::cout << b1co << "\nb2: " << b2co << "\nb3: " <<  b3co << "\nb4: " << b4co << "\n"; */
   /* } */
};

void LRCrossoverFilter::setup(float crossoverFrequency) {
#if 0
   float sr = 48000.0f;
   crossoverFrequency *= 48000.0f * 0.5f;
#else
   crossoverFrequency *= 0.5f;
#endif

   const T pi = (T) ST_PLUGIN_PI;

   coFreqRunningAv = crossoverFrequency;

   T cowc  = 2 * pi * coFreqRunningAv;
   T cowc2 = cowc  * cowc;
   T cowc3 = cowc2 * cowc;
   T cowc4 = cowc2 * cowc2;

#if 0
   T cok = cowc / tan(pi * coFreqRunningAv / sr);
#else
    T cok = cowc / tan(pi * coFreqRunningAv);
#endif
   T cok2 = cok * cok;
   T cok3 = cok2 * cok;
   T cok4 = cok2 * cok2;
   T sq_tmp1 = ST_PLUGIN_SQR2 * cowc3 * cok;
   T sq_tmp2 = ST_PLUGIN_SQR2 * cowc * cok3;
   T a_tmp = 4*cowc2*cok2 + 2*sq_tmp1 + cok4 + 2*sq_tmp2+cowc4;

   b1co = (4*(cowc4+sq_tmp1-cok4-sq_tmp2))/a_tmp;
   b2co = (6*cowc4-8*cowc2*cok2+6*cok4)/a_tmp;
   b3co = (4*(cowc4-sq_tmp1+sq_tmp2-cok4))/a_tmp;
   b4co = (cok4-2*sq_tmp1+cowc4-2*sq_tmp2+4*cowc2*cok2)/a_tmp;

   //================================================
   // low-pass
   //================================================
   lpco.a0 = cowc4/a_tmp;
   lpco.a1 = 4*cowc4/a_tmp;
   lpco.a2 = 6*cowc4/a_tmp;
   lpco.a3 = lpco.a1;
   lpco.a4 = lpco.a0;

   //=====================================================
   // high-pass
   //=====================================================
   hpco.a0 = cok4/a_tmp;
   hpco.a1 = -4*cok4/a_tmp;
   hpco.a2 = 6*cok4/a_tmp;
   hpco.a3 = hpco.a1;
   hpco.a4 = hpco.a0;
}

float LRCrossoverFilter::processHP(const float _inSmp) {
   T tempy =
      hpco.a0 * _inSmp     +
      hpco.a1 * hptemp.xm1 +
      hpco.a2 * hptemp.xm2 +
      hpco.a3 * hptemp.xm3 +
      hpco.a4 * hptemp.xm4 -
      b1co    * hptemp.ym1 -
      b2co    * hptemp.ym2 -
      b3co    * hptemp.ym3 -
      b4co    * hptemp.ym4 ;

   // shuffle
   hptemp.xm4 = hptemp.xm3;
   hptemp.xm3 = hptemp.xm2;
   hptemp.xm2 = hptemp.xm1;
   hptemp.xm1 = _inSmp;
   hptemp.ym4 = hptemp.ym3;
   hptemp.ym3 = hptemp.ym2;
   hptemp.ym2 = hptemp.ym1;
   hptemp.ym1 = tempy;

   return (float)tempy;
}

float LRCrossoverFilter::processLP(const float _inSmp) {
   T tempy =
      lpco.a0 * _inSmp     +
      lpco.a1 * lptemp.xm1 +
      lpco.a2 * lptemp.xm2 +
      lpco.a3 * lptemp.xm3 +
      lpco.a4 * lptemp.xm4 -
      b1co    * lptemp.ym1 -
      b2co    * lptemp.ym2 -
      b3co    * lptemp.ym3 -
      b4co    * lptemp.ym4 ;

   // shuffle
   lptemp.xm4 = lptemp.xm3; // these are the same as hptemp and could be optimised away
   lptemp.xm3 = lptemp.xm2;
   lptemp.xm2 = lptemp.xm1;
   lptemp.xm1 = _inSmp;
   lptemp.ym4 = lptemp.ym3;
   lptemp.ym3 = lptemp.ym2;
   lptemp.ym2 = lptemp.ym1;
   lptemp.ym1 = tempy;

   return (float)tempy;
}

/* void LRCrossoverFilter::processBlock(float * in, float * outHP, float * outLP, int numSamples) { */

/*    float tempx, tempy; */
/*    for (int i = 0; i<numSamples; i++) { */
/*       tempx=in[i]; */

/*       // High pass */

/*       tempy = hpco.a0*tempx + */
/*          hpco.a1*hptemp.xm1 + */
/*          hpco.a2*hptemp.xm2 + */
/*          hpco.a3*hptemp.xm3 + */
/*          hpco.a4*hptemp.xm4 - */
/*          b1co*hptemp.ym1 - */
/*          b2co*hptemp.ym2 - */
/*          b3co*hptemp.ym3 - */
/*          b4co*hptemp.ym4; */

/*       hptemp.xm4=hptemp.xm3; */
/*       hptemp.xm3=hptemp.xm2; */
/*       hptemp.xm2=hptemp.xm1; */
/*       hptemp.xm1=tempx; */
/*       hptemp.ym4=hptemp.ym3; */
/*       hptemp.ym3=hptemp.ym2; */
/*       hptemp.ym2=hptemp.ym1; */
/*       hptemp.ym1=tempy; */
/*       outHP[i]=tempy; */

/*       assert(tempy<10000000); */

/*       // Low pass */

/*       tempy = lpco.a0*tempx + */
/*          lpco.a1*lptemp.xm1 + */
/*          lpco.a2*lptemp.xm2 + */
/*          lpco.a3*lptemp.xm3 + */
/*          lpco.a4*lptemp.xm4 - */
/*          b1co*lptemp.ym1 - */
/*          b2co*lptemp.ym2 - */
/*          b3co*lptemp.ym3 - */
/*          b4co*lptemp.ym4; */

/*       lptemp.xm4=lptemp.xm3; // these are the same as hptemp and could be optimised away */
/*       lptemp.xm3=lptemp.xm2; */
/*       lptemp.xm2=lptemp.xm1; */
/*       lptemp.xm1=tempx; */
/*       lptemp.ym4=lptemp.ym3; */
/*       lptemp.ym3=lptemp.ym2; */
/*       lptemp.ym2=lptemp.ym1; */
/*       lptemp.ym1=tempy; */
/*       outLP[i] = tempy; */

/*       assert(!isnan(outLP[i])); */
/*    } */
/* } */
