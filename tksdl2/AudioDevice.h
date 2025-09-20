/// AudioDevice.h
///
/// (c) 2001-2014 by Bastian Spiegel <bs@tkscript.de>
///     - Distributed under terms of the Lesser GNU General Public License (LGPL).
///       See COPYING and <http://www.gnu.org/licenses/licenses.html#LGPL> for further information.
///

#ifndef __TKSDL_AUDIODEVICE_H__
#define __TKSDL_AUDIODEVICE_H__



#define TKS_AMIX_BUFSIZE 8192*2

YG("audio");


/* @function MIDINoteToFrequency,float note:float
 Convert MIDI note to frequency
*/
YF sF32 YAC_CALL _MIDINoteToFrequency(sF32);



/* @class AudioDevice,Object

Static class that represents a wrapper audio output via SDL.

This class emits the following signals:

onAudioFrame() - called whenever a new frame (according to PPQ and BPM) starts

onAudioPostProcess(FloatArray buf) - Called when a block of samples is about to be sent to the output device

onAudioRender(FloatArray buf) - Called when a new sample frame needs to be rendered

onAudioBeginBlock(FloatArray buf) - Called when a new block of samples needs to be rendered. The application may decide to call processTimeFrames() to trigger the onAudioFrame() and onAudioRender() callbacks. Note: This function runs in a separate thread.

 */
YCS class _AudioDevice : public YAC_Object {
public:
   static sSI cbid_SDL_FillAudioBuffer;

public:

    SDL_AudioSpec *dsp_spec;

    YAC_String    filename; 
    void         *ofs;
    sBool         b_file;
    sBool         b_enabled;
    sUI           def_buf_size;

    sF32  volume;
    sBool swap_stereo;
    sF32  bpm; // beats per minute
    sUI   ppq; // precision per quarter

    sF32  dsp_frequency;
    sU16  dsp_block_samples;
    sUI   dsp_block_size;
    sU8   dsp_bits;     
    sU8   dsp_channels;
    sBool dsp_active;
    sBool running;
    
    // number of samples to process in current time frame
    // calculated by bpm, ppq and dsp_frequency
    sU32 timeframe_size; 

    // number of samples calculated for current time frame
    // if timeframe_samples_done==timeframe_size the mixer settings have to be updated (sequencer)
    sUI  timeframe_samples_done;

    sUI  timeframe_next_offset;
    sUI  timeframe_last_offset;
    sF32 timeframe_real_size;
    sU16 timeframe_count;

    sF32 mix_buffer[TKS_AMIX_BUFSIZE];
    sS16*dsp_buffer;

    sUI  req_size; // number of stereo samples to process during buffer fill
    sUI  req_samples_done; /// number of samples processed for current buffer fill

    YAC_FunctionHandle sig_time;
    YAC_FunctionHandle sig_postprocess;
    YAC_FunctionHandle sig_render;
    YAC_FunctionHandle sig_block;

    void*mtx_beginblock;
    
    void*sc_audiocontext;

    // anonymous pointer to FloatArray for onAudioPostProcess callback
    void *tmp_fa;

public:
    _AudioDevice(void);
    ~_AudioDevice();

    YAC(_AudioDevice);

    void YAC_VCALL yacGetSignalStringList(YAC_String *);
    void YAC_VCALL yacRegisterSignal(sUI _sigid, YAC_FunctionHandle _fun);

    void beginBlock(sU32 _req_size, sS16 *_stereobuf16);
    void calcTimeframeSize(void);
    void beginTimeframe(void);
    void buildFrequencyTable(void);

    void YAC_VCALL yacOperator(sSI, YAC_Object *, YAC_Value *);

    /* @method setVolume,float f
       Set master volume for sample mixer.
     */
    YM void _setVolume            (sF32);

    
    /* @method getVolume:float
       Query master volume
     */
    YM sF32 _getVolume            (void);


    /* @method setSwapstereo,boolean b
       Enable stereo channel swapping
     */
    YM void _setSwapstereo        (sSI);


    /* @method getSwapstereo:boolean
       Query whether stereo channels are swapped
     */
    YM sSI  _getSwapstereo        (void);


    /* @method openDSP,float freq,int bufSize:boolean
       Open sound device with the given parameters
     */
    YM sSI  _openDSP              (sF32 _freq, sSI _bufsize);


    /* @method start
       Start sound output
     */
    YM void _start                (void);


    /* @method stop
       Stop sound output
     */
    YM void _stop                 (void);


    /* @method closeDSP
       Close sound device
     */
    YM void _closeDSP             (void);

    
    /* @method getBpm:float
       Query beats per minute
     */
    YM sF32 _getBpm               (void);


    /* @method setBpm,float bpm
       Set beats per minute
     */
    YM void _setBpm               (sF32);


    /* @method setPpq,int ppq
       Set precision per quarter
     */
    YM void _setPpq               (sSI _ppq);


    /* @method getPpq:int
       Query precision per quarter
    */
    YM sSI  _getPpq               (void);


    /* @method processTimeFrames
       Trigger onAudioFrame(), onAudioRender() signal callbacks
     */
    YM void _processTimeFrames    (void);

    
    /* @method finishBlock
       Finally send sample block to audio device
    */
    YM void _finishBlock          (void);

    
    /* @method setFileName,String fileName
       Where to write the sample data when file output is enabled
    */
    YM void _setFileName          (YAC_String *_s);


    /* @method enableFileOutput,boolean bEnabled
       Enable/disable file output
    */
    YM void _enableFileOutput     (sSI);


    YM void _enable               (sSI);


    /* @method setDefaultBufferSize,int bufSize
       Set output buffer size (and thus audio latency). Warning: Not all drivers can handle small buffer sizes. The default buffer size is 8192 samples
     */
    YM void _setDefaultBufferSize (sSI);
};



#endif
