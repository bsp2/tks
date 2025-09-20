// ----
// ---- file   : tksampleedit.tks
// ---- author : Bastian Spiegel <bs@tkscript.de>
// ---- legal  : (c) 2009-2025 by Bastian Spiegel.
// ----          Distributed under terms of the GNU LESSER GENERAL PUBLIC LICENSE (LGPL). See
// ----          http://www.gnu.org/licenses/licenses.html#LGPL or COPYING for further information.
// ----
// ---- info   :
// ----
// ---- created: 25Oct2009
// ---- changed: 11May2010, 20Jul2010, 25Feb2011, 04May2013, 12Feb2014, 23Feb2018, 24Feb2018
// ----          30Mar2018, 09Apr2018, 22Jun2018, 18Dec2018, 30Dec2018, 04May2019, 06May2019
// ----          09May2019, 08Feb2020, 09Feb2020, 14Feb2020, 15May2020, 06Feb2021, 22Dec2021
// ----          15Sep2023, 10Nov2023, 24Feb2024, 21Sep2024, 10Jan2025
// ----
// ----
// ----

#ifndef __TKSAMPLEEDIT_H__
#define __TKSAMPLEEDIT_H__


#include <math.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define YAC_PRINTF defined
#define YAC_BIGSTRING
#include <yac.h>

#include "tks-opengl.h"


YG("tksampleedit");


// called by plugin init
extern void tksampleedit_rms_init(void);

#define TKMINNIE_MAX_POINTS  4096


YC class PeakAvgTracker : public YAC_Object {
   sSI peak_count;
   sSI peak_timeout;
   sF32 peak_decay;
   sF32 peak;
   sF32 avg;

  public:
   PeakAvgTracker(void);
   ~PeakAvgTracker();

   YAC(PeakAvgTracker);

   YM void reset (void);
   YM void process (sF32 _curPeak);
   YM sF32 getPeak (void);
   YM sF32 getAvg (void);
};


/* @function tksampleedit_render_sample,FloatArray samples,int numCh,int sampleOffset,int numSamples,float w,float h,int renderStyle,int interpolStyle,int c32Skip,int c32Rep
Render a floating point waveform

@arg samples
@arg numCh
@arg frameOffset
@arg numFrames
@arg w
@arg h
@arg renderStyle
@arg interpolStyle
@arg c32Skip
@arg c32Rep
*/
YF void YAC_CALL tksampleedit_render_sample(YAC_Object *_data, sUI _numCh, sUI _frameOffset, sUI _numFrames, sF32 _w, sF32 _h, sF32 _min, sF32 _max, sSI _renderStyle, sSI _interpolStyle, sUI _c32Skip, sUI _c32Rep);


/* @function tksampleedit_render_rms,FloatArray samples,int numCh,int sampleOffset,int numSamples,float w,float h
Render root mean square (RMS) of a floating point waveform

*/
YF void YAC_CALL tksampleedit_render_rms(YAC_Object *_data, sUI _numCh, sUI _frameOffset, sUI _numFrames, sF32 _w, sF32 _h, sF32 _min, sF32 _max);


YF void YAC_CALL tksampleedit_render_mono_sample_1d_8bit(YAC_Object *_byteArray, sUI _width, YAC_Object *_fa, sUI _frameOffset, sUI _numFrames, sF32 _lumScl);


/* @function tksampleedit_render_fft,FloatArray magnitudes,int num,float w,float h,float zoomX,float max,int baseColor
Render FFT magnitudes using a logarithmic scale

@arg samples
@arg numFrames
@arg w
@arg h
@arg zoomX (0..1)
@arg max (0.02..64)
@arg baseColor
*/
YF void YAC_CALL tksampleedit_render_fft(YAC_Object *_data, sUI _num, sF32 _w, sF32 _h, sF32 _zoomX, sF32 _max, sUI _baseColor);


/* @function tksampleedit_calc_rms,FloatArray samples,int numCh:float
Calculate root mean square (RMS) of sample array.

@arg samples
@arg numCh
@return Root mean square
*/
YF sF32 YAC_CALL tksampleedit_calc_rms(YAC_Object *_fa, sUI _numCh);


YF void YAC_CALL tksampleedit_ringbuf_copy_samples(YAC_Object *_dst, sUI _dstNumCh, sUI _dstChOff, sUI _dstFrameOff, sUI _numFrames, sUI _numCh, YAC_Object *_src, sUI _srcNumCh, sUI _srcChOff, sUI _srcFrameOff);

YF void YAC_CALL tksampleedit_copy_looped_samples_mono(YAC_Object *_dst, sSI _dstFrameOff, YAC_Object *_src, sSI _srcFrameOff, sSI _loopStart, sSI _loopEnd, sUI _numFrames);

YF sF32 YAC_CALL tksampleedit_buf_get_max(YAC_Object *_src, sUI _numFrames, sUI _numCh, sUI _chOff);

// used by Audio.FillStreamBuffer()
YF void YAC_CALL tksampleedit_mix_stereo_input_buf(YAC_Object *_faOutput, YAC_Object *_faInput, YAC_Object *_faMixerLevels, sUI _numInputChannels, sUI _numOutputChannels, sUI _numFrames, sUI _outChOff, sUI _inChOff);

// used by Audio.ProcessSampleFragment()
YF void YAC_CALL tksampleedit_mixclip_stereo_mixbuf(YAC_Object *_faOutput, YAC_Object *_faMixBuf, sF32 _levelL, sF32 _levelR, sF32 _clipLevel, sUI _numOutputChannels, sUI _numFrames, sUI _outChOff);

// used by Eureka Replay/Track/Lane
//  (note) (outoff/inoff are #samples
//  (note) no type or bounds checks
YF void YAC_CALL tksampleedit_fill_mono(YAC_Object *_faOut, sUI _outOff, sF32 _val, sUI _numFrames);

YF void YAC_CALL tksampleedit_copy_mono_to_mono(YAC_Object *_faOut, sUI _outOff, YAC_Object *_faIn, sUI _inOff, sUI _numFrames);

YF void YAC_CALL tksampleedit_amp_mono_to_mono(YAC_Object *_faOut, sUI _outOff, YAC_Object *_faIn, sUI _inOff, sUI _numFrames, sF32 _amp);

YF void YAC_CALL tksampleedit_copy_mono_adv_to_mono(YAC_Object *_faOut, sUI _outOff, YAC_Object *_faIn, sUI _inOff, sUI _inAdv, sUI _numFrames);

YF void YAC_CALL tksampleedit_copy_mono_adv_to_mono_adv(YAC_Object *_faOut, sUI _outOff, sUI _outAdv, YAC_Object *_faIn, sUI _inOff, sUI _inAdv, sUI _numFrames);

YF void YAC_CALL tksampleedit_copy_mono_adv_to_mono_adv_ring(YAC_Object *_faOut, sUI _outOff, sUI _outAdv, sUI _outRingSize, YAC_Object *_faIn, sUI _inOff, sUI _inAdv, sUI _numFrames);

YF void YAC_CALL tksampleedit_copy_mono_adv_ring_to_mono_adv(YAC_Object *_faOut, sUI _outOff, sUI _outAdv, YAC_Object *_faIn, sUI _inOff, sUI _inAdv, sUI _inRingSize, sUI _numFrames);

YF void YAC_CALL tksampleedit_scale_mono_adv_to_mono_adv(YAC_Object *_faOut, sUI _outOff, sUI _outAdv, YAC_Object *_faIn, sUI _inOff, sUI _inAdv, sF32 _scale, sUI _numFrames);

YF void YAC_CALL tksampleedit_copy_mono_to_dualmono(YAC_Object *_faOutL, YAC_Object *_faOutR, sUI _outOff, YAC_Object *_faIn, sUI _inOff, sUI _numFrames);

YF void YAC_CALL tksampleedit_copy_mono_adv_to_dualmono(YAC_Object *_faOutL, YAC_Object *_faOutR, sUI _outOff, YAC_Object *_faIn, sUI _inOff, sUI _inAdv, sUI _numFrames);

YF void YAC_CALL tksampleedit_copy_mono_to_stereo(YAC_Object *_faOut, sUI _outOff, YAC_Object *_faIn, sUI _inOff, sUI _numFrames);

YF void YAC_CALL tksampleedit_copy_pan_mono_to_stereo(YAC_Object *_faOut, sUI _outOff, YAC_Object *_faIn, sUI _inOff, sF32 _lvlL, sF32 _lvlR, sUI _numFrames);

YF void YAC_CALL tksampleedit_copy_dualmono_to_dualmono(YAC_Object *_faOutL, YAC_Object *_faOutR, sUI _outOff, YAC_Object *_faInL, YAC_Object *_faInR, sUI _inOff, sUI _numFrames);

YF void YAC_CALL tksampleedit_copy_dualmono_to_stereo(YAC_Object *_faOut, sUI _outOff, YAC_Object *_faInL, YAC_Object *_faInR, sUI _inOff, sUI _numFrames);

YF void YAC_CALL tksampleedit_copy_pan_dualmono_to_dualmono(YAC_Object *_faOutL, YAC_Object *_faOutR, sUI _outOff, YAC_Object *_faInL, YAC_Object *_faInR, sUI _inOff, sF32 _lvlL, sF32 _lvlR, sUI _numFrames);

YF void YAC_CALL tksampleedit_copy_pan_stereowidth_dualmono_to_dualmono(YAC_Object *_faOutL, YAC_Object *_faOutR, sUI _outOff, YAC_Object *_faInL, YAC_Object *_faInR, sUI _inOff, sF32 _lvlL, sF32 _lvlR, sF32 _stereoWidth, sUI _numFrames);

YF void YAC_CALL tksampleedit_copy_pan_dualmono_to_stereo(YAC_Object *_faOut, sUI _outOff, YAC_Object *_faInL, YAC_Object *_faInR, sUI _inOff, sF32 _lvlL, sF32 _lvlR, sUI _numFrames);

YF void YAC_CALL tksampleedit_copy_stereo_to_dualmono(YAC_Object *_faOutL, YAC_Object *_faOutR, sUI _outOff, YAC_Object *_faIn, sUI _inOff, sUI _numFrames);

YF void YAC_CALL tksampleedit_copy_stereo_to_stereo(YAC_Object *_faOut, sUI _outOff, YAC_Object *_faIn, sUI _inOff, sUI _numFrames);

YF void YAC_CALL tksampleedit_copy_pan_stereo_to_stereo(YAC_Object *_faOut, sUI _outOff, YAC_Object *_faIn, sUI _inOff, sF32 _lvlL, sF32 _lvlR, sUI _numFrames);

YF void YAC_CALL tksampleedit_copy_pan_stereo_adv_to_stereo_adv(YAC_Object *_faOut, sUI _outOff, sUI _outAdv, YAC_Object *_faIn, sUI _inOff, sUI _inAdv, sF32 _lvlL, sF32 _lvlR, sUI _numFrames);

YF void YAC_CALL tksampleedit_copy_pan_tanh_dim_stereo_adv_to_stereo_adv(YAC_Object *_faOut, sUI _outOff, sUI _outAdv, YAC_Object *_faIn, sUI _inOff, sUI _inAdv, sF32 _lvlL, sF32 _lvlR, sF32 _dim, sUI _numFrames);

YF void YAC_CALL tksampleedit_add_pan_mono_adv_to_dualmono(YAC_Object *_faOutL, YAC_Object *_faOutR, sUI _outOff, YAC_Object *_faIn, sUI _inOff, sUI _inAdv, sF32 _lvlL, sF32 _lvlR, sUI _numFrames);

YF void YAC_CALL tksampleedit_add_dualmono_to_stereo(YAC_Object *_faOut, sUI _outOff, YAC_Object *_faInL, YAC_Object *_faInR, sUI _inOff, sUI _numFrames);

YF void YAC_CALL tksampleedit_add_stereo_to_stereo(YAC_Object *_faOut, sUI _outOff, YAC_Object *_faIn, sUI _inOff, sUI _numFrames);

YF void YAC_CALL tksampleedit_add_amp_mono_adv_to_mono_adv(YAC_Object *_faOut, sUI _outOff, sUI _outAdv, YAC_Object *_faIn, sUI _inOff, sUI _inAdv, sF32 _lvl, sUI _numFrames);

YF void YAC_CALL tksampleedit_amp_ramp_mono_adv_to_mono_adv(YAC_Object *_faOut, sUI _outOff, sUI _outAdv, YAC_Object *_faIn, sUI _inOff, sUI _inAdv, sF32 _lvlStart, sF32 _lvlEnd, sUI _numFrames);

YF void YAC_CALL tksampleedit_add_mono_to_mono(YAC_Object *_faOut, sUI _outOff, YAC_Object *_faIn, sUI _inOff, sUI _numFrames);

YF void YAC_CALL tksampleedit_add_amp_mono_adv_ring_to_mono_adv(YAC_Object *_faOut, sUI _outOff, sUI _outAdv, YAC_Object *_faIn, sUI _inOff, sUI _inAdv, sUI _inRingSz, sF32 _lvl, sUI _numFrames);

// (note) (numFrames-xfadeFrames) samples will be written to dstSmp
YF void YAC_CALL tksampleedit_xfade_mono(YAC_Object *_faOut, YAC_Object *_faIn, sUI _numFrames, sUI _xfadeFrames);

YF void YAC_CALL tksampleedit_add_pan_mono_to_stereo(YAC_Object *_faOut, sUI _outOff, YAC_Object *_faIn, sUI _inOff, sF32 _lvlL, sF32 _lvlR, sUI _numFrames);

YF void YAC_CALL tksampleedit_add_pan_mono_adv_to_stereo(YAC_Object *_faOut, sUI _outOff, YAC_Object *_faIn, sUI _inOff, sUI _inAdv, sF32 _lvlL, sF32 _lvlR, sUI _numFrames);

YF void YAC_CALL tksampleedit_add_pan_mono_adv_to_stereo_adv(YAC_Object *_faOut, sUI _outOff, sUI _outAdv, YAC_Object *_faIn, sUI _inOff, sUI _inAdv, sF32 _lvlL, sF32 _lvlR, sUI _numFrames);

YF void YAC_CALL tksampleedit_add_pan_mono_to_stereo_adv(YAC_Object *_faOut, sUI _outOff, sUI _outAdv, YAC_Object *_faIn, sUI _inOff, sF32 _lvlL, sF32 _lvlR, sUI _numFrames);

YF void YAC_CALL tksampleedit_add_pan_dualmono_to_stereo(YAC_Object *_faOut, sUI _outOff, YAC_Object *_faInL, YAC_Object *_faInR, sUI _inOff, sF32 _lvlL, sF32 _lvlR, sUI _numFrames);

YF void YAC_CALL tksampleedit_add_pan_dualmono_to_stereo_adv(YAC_Object *_faOut, sUI _outOff, sUI _outAdv, YAC_Object *_faInL, YAC_Object *_faInR, sUI _inOff, sF32 _lvlL, sF32 _lvlR, sUI _numFrames);

YF void YAC_CALL tksampleedit_add_pan_stereo_to_dualmono(YAC_Object *_faOutL, YAC_Object *_faOutR, sUI _outOff, YAC_Object *_faIn, sUI _inOff, sF32 _lvlL, sF32 _lvlR, sUI _numFrames);

YF void YAC_CALL tksampleedit_add_pan_stereo_to_stereo(YAC_Object *_faOut, sUI _outOff, YAC_Object *_faIn, sUI _inOff, sF32 _lvlL, sF32 _lvlR, sUI _numFrames);

YF void YAC_CALL tksampleedit_add_pan_stereo_adv_to_stereo_adv(YAC_Object *_faOut, sUI _outOff, sUI _outAdv, YAC_Object *_faIn, sUI _inOff, sUI _inAdv, sF32 _lvlL, sF32 _lvlR, sUI _numFrames);

YF sF32 YAC_CALL tksampleedit_calc_peak_mono_adv(YAC_Object *_faIn, sUI _inOff, sUI _inAdv, sUI _numFrames);
YF sF32 YAC_CALL tksampleedit_calc_peak_mono_adv_ring(YAC_Object *_faIn, sUI _inOff, sUI _inAdv, sUI _ringSize, sUI _numFrames);

YF sF32 YAC_CALL tksampleedit_calc_rms_mono_adv(YAC_Object *_faIn, sUI _inOff, sUI _inAdv, sUI _numFrames);
YF sF32 YAC_CALL tksampleedit_calc_rms_mono_adv_ring(YAC_Object *_faIn, sUI _inOff, sUI _inAdv, sUI _ringSize, sUI _numFrames);

YF void YAC_CALL tksampleedit_ms_convert_dualmono(YAC_Object *_faOutL, YAC_Object *_faOutR, sUI _outOff, YAC_Object *_faInL, YAC_Object *_faInR, sUI _inOff, sF32 _lvlOut, sUI _numFrames);

YF void YAC_CALL tksampleedit_monomix_restore_dualmono(YAC_Object *_faMonoMixOut, YAC_Object *_faRestoreOut, YAC_Object *_faInL, YAC_Object *_faInR, YAC_Object *_faRestoreIn, sUI _numFrames);


YF void YAC_CALL tksampleedit_autochop(YAC_Object *_retStartOffsets, YAC_Object *_retEndOffsets, YAC_Object *_smp, sUI _numChannels, sF32 _startThreshold, sF32 _startMinThreshold, sUI _startMinNumFramesBack, sF32 _endThreshold, sUI _endNumFramesThreshold, sF32 _endThresholdRestart, sUI _restartNumFramesThreshold);

YF void YAC_CALL tksampleedit_autochop_rms(YAC_Object *_retStartOffsets, YAC_Object *_retEndOffsets, YAC_Object *_smp, sUI _numChannels, sF32 _startThreshold, sF32 _startMinThreshold, sUI _startMinNumFramesBack, sF32 _endThreshold, sUI _endNumFramesThreshold, sUI _restartNumFramesThreshold, YAC_FloatArray *_restartRatios, sUI _restartRMSRatioLongNumFrames, sF32 _restartThresholdImm, sSI _restartRewindNumFrames, sF32 _rmsAvgSlew);

YF void YAC_CALL tksampleedit_autochop_rms_windowed(YAC_Object *_retStartOffsets, YAC_Object *_retEndOffsets, YAC_Object *_smp, sUI _numChannels, sF32 _hpf, sUI _rmsWindowSize, sUI _timeout, sF32 _rmsSlew, sF32 _avgPeakRatioSlew, sF32 _avgPeakRatioThreshold, sUI _maxMarkers);

YF sBool YAC_CALL tksampleedit_is_zerocrossing (YAC_Object *_fa, sUI _chOff, sUI _numChannels, sUI _off);
YF sUI YAC_CALL tksampleedit_find_zerocrossing_near (YAC_Object *_fa, sUI _chOff, sUI _numChannels, sUI _off, sF32 _distThreshold);
YF sUI YAC_CALL tksampleedit_find_edge_near (YAC_Object *_fa, sUI _chOff, sUI _numChannels, sUI _off, sBool _bRise, sBool _bFall, sF32 _distThreshold);
YF sUI YAC_CALL tksampleedit_find_edge_near_fwd (YAC_Object *_fa, sUI _chOff, sUI _numChannels, sUI _off, sBool _bRise, sBool _bFall, sF32 _distThreshold);
YF sUI YAC_CALL tksampleedit_find_edge_near_bwd (YAC_Object *_fa, sUI _chOff, sUI _numChannels, sUI _off, sBool _bRise, sBool _bFall, sF32 _distThreshold);

YF sUI YAC_CALL tksampleedit_find_max_near (YAC_Object *_fa, sUI _chOff, sUI _numChannels, sUI _off, sUI _frameRange);
YF sUI YAC_CALL tksampleedit_find_max_near_fwd (YAC_Object *_fa, sUI _chOff, sUI _numChannels, sUI _off, sUI _frameRange);
YF sUI YAC_CALL tksampleedit_find_absmax_near_fwd (YAC_Object *_fa, sUI _chOff, sUI _numChannels, sUI _off, sUI _frameRange);
YF sUI YAC_CALL tksampleedit_find_min_near_fwd (YAC_Object *_fa, sUI _chOff, sUI _numChannels, sUI _off, sUI _frameRange);
YF sUI YAC_CALL tksampleedit_find_max_near_bwd (YAC_Object *_fa, sUI _chOff, sUI _numChannels, sUI _off, sUI _frameRange);

YF sSI YAC_CALL tksampleedit_find_min_threshold_fwd (YAC_Object *_fa, sUI _chOff, sUI _numChannels, sF32 _min);
YF sSI YAC_CALL tksampleedit_find_min_threshold_bwd (YAC_Object *_fa, sUI _chOff, sUI _numChannels, sF32 _min);


#endif // __TKSAMPLEEDIT_H__
