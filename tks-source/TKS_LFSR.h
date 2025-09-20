/// TKS_LFSR.h
///
/// (c) 2020-2022 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///
#ifndef __TKS_LFSR_H__
#define __TKS_LFSR_H__


class TKS_LFSR : public YAC_Object {
public:
   sUI state;

public:
   TKS_LFSR(void);
   ~TKS_LFSR();

   void init(sUI _seed, sUI _numPre);

   sSI rands (void);
   sUI randu (void);
   sF32 randf (sF32 _max);

};


// from tksampler rand.h/.cpp:
struct tks_lfsr_t {
   sUI seed1;
   sUI seed2;
   sUI last_mod_nr;

protected:
   void lfsr (sUI &seed, sUI mask);

public:
   void initPtr (void *_init, sUI _numPre);   // seed from ptr
   void initU32 (sUI _seed, sUI _numPre); // seed from uint32
   sUI rand (void);
   sUI randModNR (sUI _num);
   sF32 randf (void);
   void shuffle (void);
};

class TKS_LFSR_NR : public YAC_Object {
  protected:
   // non-repeating indices (0..(len-1))
   tks_lfsr_t lfsr;
   sUI next_idx;
   sUI len;
   sUI *values;
   sSI last_idx;  // idx not to be repeated at start of (new) sequence (or -1)

  public:
   TKS_LFSR_NR(void);
   ~TKS_LFSR_NR();

  public:
   // seed==0: use address of 'this' (pseudo-random seed)
   void init (sUI _seed, sUI _len, sUI _numPre);
   sUI getNext (void);

protected:
   void freeValues (void);
   void genValues (void);
};


#endif // __TKS_LFSR_H__
