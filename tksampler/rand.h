// ----
// ---- file   : rand.h
// ---- author : Bastian Spiegel <bs@tkscript.de>
// ---- legal  : (c) 2020 by Bastian Spiegel. 
// ----          Distributed under terms of the GNU LESSER GENERAL PUBLIC LICENSE (LGPL). See 
// ----          http://www.gnu.org/licenses/licenses.html#LGPL or COPYING for further information.
// ----
// ---- info   : This is part of the "syntracker" midi sequencer.
// ----
// ---- created: 15Feb2020
// ---- changed: 16Feb2020, 18Feb2020
// ----
// ----
// ----

#ifndef __TK_SAMPLER_RAND_H__
#define __TK_SAMPLER_RAND_H__

struct tksampler_lfsr_t {
   sUI seed1;
   sUI seed2;
   sUI last_mod_nr;

protected:
   void lfsr (sUI &seed, sUI mask);

public:

   void init (void *_init);   
   sUI rand (void);
   sUI randModNR (sUI _num);
   sF32 randf (void);
   void shuffle (void);
};


struct tksampler_rand_nr_t {
   // non-repeating indices (0..(len-1))
   tksampler_lfsr_t lfsr;
   sUI next_idx;
   sUI len;
   sUI *values;
   sSI last_idx;  // idx not to be repeated at start of (new) sequence (or -1)

   tksampler_rand_nr_t(void);
   ~tksampler_rand_nr_t();

   void init (sUI _len);
   sUI next (void);

protected:
   void freeValues (void);
   void genValues (void);
};


#endif // __TKSAMPLER_RAND_H__
