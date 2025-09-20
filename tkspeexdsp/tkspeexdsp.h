// ----
// ---- file   : tkspeexdsp.h
// ---- author : Bastian Spiegel <bs@tkscript.de>
// ---- legal  : (c) 2020-2021 by Bastian Spiegel. 
// ----          Distributed under terms of the GNU LESSER GENERAL PUBLIC LICENSE (LGPL). See 
// ----          http://www.gnu.org/licenses/licenses.html#LGPL or COPYING for further information.
// ----
// ---- info   : libspeexdsp interface
// ----
// ---- created: 31Oct2020
// ---- changed: 31Jul2021
// ----
// ----
// ----

#ifndef __TKSPEEXDSP_H__
#define __TKSPEEXDSP_H__


YG("speexdsp")


YC class _SpeexResampler  : public YAC_Object {
  protected:
   sUI num_channels;
   SpeexResamplerState *srs;

	public:
		YAC(_SpeexResampler);

      _SpeexResampler(void);
      ~_SpeexResampler();

  protected:

  public:
      // quality: 0..10 (default=4)
      //  (note) low-quality settings seem to introduce a delay (e.g. 32 samples @quality=4, 44100Hz=>22050Hz)
      YM sBool init (sUI _numCh, sUI _sampleRateIn, sUI _sampleRateOut, sUI _quality);

      // quality-dependent output delay
      //   -   4 frames at quality=0 (and factor=1)
      //   -   8 frames at quality=1 (and factor=1)
      //   -  16 frames at quality=2 ..
      //   -  24 frames at quality=3
      //   -  32 frames at quality=4
      //   -  40 frames at quality=5
      //   -  48 frames at quality=6
      //   -  64 frames at quality=7
      //   -  80 frames at quality=8
      //   -  96 frames at quality=9
      //   - 128 frames at quality=10
      YM sUI GetOutputDelayInFramesByQualityAndFactor(sUI _quality, sF32 _factor);

      // Resample input to output buffer
      YM void process (sUI _chIdx, YAC_Object *_faIn, sUI _numFramesIn, YAC_Object *_faOut, sUI _numFramesOut);

      // Free resampler state
      YM void exit (void);     
};


#endif // __TKSPEEXDSP_H__
