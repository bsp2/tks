// ----
// ---- file   : rand.cpp
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
// ---- changed: 16Feb2020, 18Feb2020, 12Apr2023, 24May2026
// ----
// ----
// ----

#include "tksampler.h"

// (note) system rand() is not thread safe


void tksampler_lfsr_t::init(void *_init) {
   yacmemptr p; p.any = _init;
   seed1 = 0x4489abcdu ^ p._ui;
   seed2 = 0xdeadbeefu ^ p._ui;
   last_mod_nr = ~0u;
}

void tksampler_lfsr_t::shuffle(void) {
   seed1 = ((seed1 << 3) | (seed2 >> (32-3))) ^ 0xd89dcfbdu;
   seed2 = ((seed2 << 3) | (seed1 >> (32-3))) ^ 0x3fb8deacu;
}

void tksampler_lfsr_t::lfsr(sUI &seed, const sUI mask) {
   sUI fb = seed & 1u;
   seed >>= 1u;
   if(0u != fb)
      seed ^= mask;
}

sUI tksampler_lfsr_t::rand(void) {
   lfsr(seed1, 0xB4BCD35Cu);
   lfsr(seed2, 0x7A5BC2E3u);

   return (seed1 ^ seed2) & 0xFFFFu;
}

sUI tksampler_lfsr_t::randModNR(sUI _num) {
   sUI r;
   if(_num > 1u)
   {
      do
      {
         r = rand() % _num;
      }
      while(last_mod_nr == r);
      last_mod_nr = r;
   }
   else
   {
      r = 0u;
   }
   return r;
}

sF32 tksampler_lfsr_t::randf(void) {
   // returns rand value in 0..1 (exclusive) range
   sUI ru = rand();
   sF32 rf = (ru & 0xFFFFu) / 65536.0f;
   return rf;
}


// (todo) add max len and avoid reallocation when len is <= maxlen
tksampler_rand_nr_t::tksampler_rand_nr_t(void) {
   lfsr.init((void*)this);
   next_idx = 0u;
   len = 0u;
   values = NULL;
   last_idx = -1;
}

tksampler_rand_nr_t::~tksampler_rand_nr_t() {
   freeValues();
}

void tksampler_rand_nr_t::freeValues(void) {
   if(NULL != values)
   {
      delete [] values;
      values = NULL;
   }
}

void tksampler_rand_nr_t::init(sUI _len) {
   // Dyac_host_printf("xxx rand_nr::init: len=%u\n", _len);
   if(_len != len)
   {
      freeValues();
      values = new(std::nothrow) sUI[_len];
      len = _len;
      last_idx = -1;
   }
   else if(len > 0u)
   {
      last_idx = values[len - 1u];
   }
   else
   {
      last_idx = -1;
   }
   genValues();
}

sUI tksampler_rand_nr_t::next(void) {
   if(len > 0u)
   {
      if(next_idx >= len)
      {
         last_idx = values[len - 1u];
         genValues();
      }
      // Dyac_host_printf("xxx rand_nr::next: r=%u\n", values[next_idx]);
      return values[next_idx++];
   }
   return 0u;
}

void tksampler_rand_nr_t::genValues(void) {
   // Dyac_host_printf("xxx tksampler_rand_nr_t::genValues: ENTER\n");
   // sUI dbgTotalNumRep = 0u;
   for(sUI i = 0u; i < len; i++)
   {
      sBool bRep = YAC_TRUE;
      sUI r;
      sUI numRep = 0u;
      do
      {
         r = lfsr.rand() % len;
         // Dyac_host_printf("xxx genValues: r=%u\n", r);

         if( (0u == i) && (-1 != last_idx) && (sSI(r) == last_idx) )
         {
            numRep++;
            if(numRep > 10000u)
            {
               // should not be reachable
               r = 0u;
            }
            else if(0 == (numRep  & 7u))
            {
               lfsr.shuffle();
               bRep = YAC_TRUE;
            }
            else
            {
               bRep = YAC_TRUE;
            }
         }
         else
         {
            bRep = YAC_FALSE;
            for(sUI j=0u; j < i; j++)
            {
               numRep++;
               if(r == values[j])
               {
                  if(numRep > 10000u)
                  {
                     // should not be reachable
                     r = 0u;
                  }
                  else if(0 == (numRep  & 7u))
                  {
                     lfsr.shuffle();
                     bRep = YAC_TRUE;
                  }
                  else
                  {
                     bRep = YAC_TRUE;
                  }
                  break;
               }
            }
         }
      }
      while(bRep);
      values[i] = r;
      // dbgTotalNumRep += numRep;
   }

   // Dyac_host_printf("xxx rand_nr::genValues: numRep=%u (len=%u)\n", dbgTotalNumRep, len);
   //   => xxx rand_nr::genValues: numRep=0 (len=5)
   //      xxx rand_nr::next: r=0
   //      xxx rand_nr::genValues: numRep=7 (len=5)
   //      xxx rand_nr::next: r=4
   //      xxx rand_nr::genValues: numRep=3 (len=5)
   //      xxx rand_nr::next: r=2
   //      xxx rand_nr::genValues: numRep=8 (len=5)
   next_idx = 0u;
   // Dyac_host_printf("xxx tksampler_rand_nr_t::genValues: LEAVE\n");
}
