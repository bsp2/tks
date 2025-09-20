// TKS_LFSR.cpp 
/// 
/// (c) 2020-2023 by Bastian Spiegel <bs@tkscript.de> 
///     - distributed under terms of the GNU general public license (GPL). 
/// 

#include "tks.h" 

#include "TKS_LFSR.h" 


TKS_LFSR::TKS_LFSR(void) {
   init(0x4489u, 0u/*numPre*/);
}

TKS_LFSR::~TKS_LFSR() {
}

void TKS_LFSR::init(sUI _seed, sUI _numPre) {
   state = _seed;

   for(sUI i = 0u; i < _numPre; i++)
   {
      rands();
   }
}

sSI TKS_LFSR::rands(void) {
   state ^= (state >> 7);
   state ^= (state << 9);
   state ^= (state >> 13);

   return state;
}

sUI TKS_LFSR::randu(void) {
   return (sUI) (rands() & 0x7FFFffffu);
}

sF32 TKS_LFSR::randf(sF32 _max) {
   sU16 r = rands() & 65535u;
   return (_max * r) / 65535.0f;
}


//------------------------------------------------------------------------------
//
// bug-fixed version of tksampler rand.h/.cpp:
//
//  (note) see tks-examples/lfsr.tks (regression test)
//
//------------------------------------------------------------------------------
void tks_lfsr_t::initPtr(void *_init, sUI _numPre) {
   yacmemptr p; p.any = _init;
   initU32(p._ui >> 5, _numPre);
}

void tks_lfsr_t::initU32(sUI _seed, sUI _numPre) {
   // yac_host->printf("xxx lfsr seed=0x%08x\n", _seed);
   seed1 = 0x4489abcdu ^ _seed;
   seed2 = 0xdeadbeefu ^ _seed;
   // seed1 = 0xaaaaaaaau ^ _seed;
   // seed2 = 0xffffffffu ^ _seed;
   last_mod_nr = ~0u;

   // first two rand() results are (almost) always the same, regardless of seed
   for(sUI i = 0u; i < _numPre; i++)
   {
      (void)rand();
      shuffle();
   }
}

void tks_lfsr_t::shuffle(void) {
   // // seed1 = ((seed1 << 3) | (seed2 >> (32-3))) ^ 0xd89dcfbdu;
   // // seed2 = ((seed2 << 3) | (seed1 >> (32-3))) ^ 0x3fb8deacu;
   seed1 = ((seed1 >> 1) | ((seed1 & 1u) << 31)) ^ 0xaaaaaaaau;
   seed2 = ((seed2 >> 1) | ((seed2 & 1u) << 31)) ^ 0xffffffffu;
}

void tks_lfsr_t::lfsr(sUI &seed, const sUI mask) {
   sUI fb = seed & 1u;
   seed >>= 1u;
   if(0u != fb)
      seed ^= mask;
}

sUI tks_lfsr_t::rand(void) {
   lfsr(seed1, 0xB4BCD35Fu);
   lfsr(seed2, 0x7A5BC2E3u);
   // lfsr(seed1, 0xaaaaaaaau);
   // lfsr(seed2, 0xffffffffu);

   return (seed1 ^ seed2) & 0xFFFFu;
}

sUI tks_lfsr_t::randModNR(sUI _num) {
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

sF32 tks_lfsr_t::randf(void) {
   // returns rand value in 0..1 (exclusive) range
   sUI ru = rand();
   sF32 rf = (ru & 0xFFFFu) / 65536.0f;
   return rf;
}


// (todo) add max len and avoid reallocation when len is <= maxlen
TKS_LFSR_NR::TKS_LFSR_NR(void) {
   next_idx = 0u;
   len = 0u;
   values = NULL;
   last_idx = -1;
}

TKS_LFSR_NR::~TKS_LFSR_NR() {
   freeValues();
}

void TKS_LFSR_NR::freeValues(void) {
   if(NULL != values)
   {
      delete [] values;
      values = NULL;
   }
}

void TKS_LFSR_NR::init(sUI _seed, sUI _len, sUI _numPre) {
   if(0u != _seed)
      lfsr.initU32(_seed, _numPre);
   else
      lfsr.initPtr((void*)this, _numPre);

   // yac_host->printf("xxx rand_nr::init: len=%u\n", _len);
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

sUI TKS_LFSR_NR::getNext(void) {
   if(len > 0u)
   {
      if(next_idx >= len)
      {
         last_idx = values[len - 1u];
         genValues();
      }
      // yac_host->printf("xxx rand_nr::next: r=%u\n", values[next_idx]);
      return values[next_idx++];
   }
   return 0u;
}

void TKS_LFSR_NR::genValues(void) {
   // yac_host->printf("xxx TKS_LFSR_NR::genValues: ENTER\n");
   if(len > 1u)
   {
      // sUI dbgTotalNumRep = 0u;
      for(sUI i = 0u; i < len; i++)
      {
         sBool bRep = YAC_TRUE;
         sUI r;
         sUI numRep = 0u;
         do
         {
            r = lfsr.rand() % len;
            // yac_host->printf("xxx genValues: r=%u\n", r);

            if( (0u == i) && (-1 != last_idx) && (sSI(r) == last_idx) )
            {
               // Compare new first value to last value of last sequence
               numRep++;
               if(numRep > 1000u)
               {
                  // should not be reachable
                  yac_host->printf("[~~~] TKS_LFSR: error1, numRep exceeded, return 0\n");
                  r = 0u;
               }
               else if(1)////0 == (numRep  & 7u))
               {
                  lfsr.shuffle();
                  bRep = YAC_TRUE;
                  numRep = 0u;
               }
               else
               {
                  bRep = YAC_TRUE;
               }
            }
            else
            {
               // Compare value to existing sequence values
               bRep = YAC_FALSE;
               for(sUI j = 0u; j < i; j++)
               {
                  if(r == values[j])
                  {
                     numRep++;
                     if(numRep > 1000u)
                     {
                        // should not be reachable
                        yac_host->printf("[~~~] TKS_LFSR: error2, numRep exceeded, return 0\n");
                        r = 0u;
                     }
                     else if(1)//0 == (numRep % len))
                     {
                        lfsr.shuffle();
                        bRep = YAC_TRUE;
                        numRep = 0u;
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
   }
   else
   {
      if(len > 0u)
         values[0] = 0u;
   }

   // yac_host->printf("xxx rand_nr::genValues: numRep=%u (len=%u)\n", dbgTotalNumRep, len);
   //   => xxx rand_nr::genValues: numRep=0 (len=5)
   //      xxx rand_nr::next: r=0
   //      xxx rand_nr::genValues: numRep=7 (len=5)
   //      xxx rand_nr::next: r=4
   //      xxx rand_nr::genValues: numRep=3 (len=5)
   //      xxx rand_nr::next: r=2
   //      xxx rand_nr::genValues: numRep=8 (len=5)
   next_idx = 0u;
   // yac_host->printf("xxx TKS_LFSR_NR::genValues: LEAVE\n");
}

