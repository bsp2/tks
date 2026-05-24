// ----
// ---- file   : rand.h
// ---- author : Bastian Spiegel <bs@tkscript.de>
// ---- legal  : Distributed under terms of the MIT license (https://opensource.org/licenses/MIT)
// ----          Copyright 2020-2026 by bsp
// ----
// ----          Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
// ----          associated documentation files (the "Software"), to deal in the Software without restriction, including
// ----          without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// ----          copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to
// ----          the following conditions:
// ----
// ----          The above copyright notice and this permission notice shall be included in all copies or substantial
// ----          portions of the Software.
// ----
// ----          THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT
// ----          NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// ----          IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
// ----          WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
// ----          SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
// ----
// ---- info   : This is part of the "syntracker" midi sequencer.
// ----
// ---- created: 15Feb2020
// ---- changed: 16Feb2020, 18Feb2020, 24May2026
// ----
// ----
// ----

#ifndef TK_SAMPLER_RAND_H__
#define TK_SAMPLER_RAND_H__

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


#endif // TKSAMPLER_RAND_H__
