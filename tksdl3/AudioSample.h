/// AudioSample.h
///
/// (c) 2001-2006 by Bastian Spiegel <bs@tkscript.de>
///     - Distributed under terms of the Lesser GNU General Public License (LGPL).
///       See COPYING and <http://www.gnu.org/licenses/licenses.html#LGPL> for further information.
///

#ifndef __TKS_AUDIOSAMPLE_H__
#define __TKS_AUDIOSAMPLE_H__

//class TKSAVIFileAudio;

YG("audio");

#define cAFloat28(a) ((sS8)((a)*127.99))
#define cAFloat216(a) ((sS16)((a)*32767.99))
#define cAFloat224(a) ((sS32)((a)*16777215.99))
#define cAFloat232(a) ((sS32)((a)*((((sF32)(1<<31))-0.01)*2.0)))


/* @class AudioSample,Object
 */

YC class _AudioSample : public YAC_Object {
public:
  /// Sample data types
  enum _stypes {
    /// Signed 8 bit samples
    Sample8=1,
    /// Signed 16 bit samples
    Sample16=2,
    /// Signed 24 bit samples
    Sample24=3,
    /// Signed 32 bit samples
    Sample32=4,
    /// 32bit Float samples
    SampleFloat=5,
    /// 64bit Double samples
    SampleDouble=6
  };

  /// Buffer types
  enum _atypes {

    /// Time domain virtual buffers represent parts of another time domain buffer. 
    /// The buffer layout is: Vertical: Subwave , Horizontal: Time (sample data)
    /// If the vertical (y) size of the buffer is >1 the buffer is considered
    /// to consist of multiple subwaves that are accessable by the y index.
    /// This can be used for wavetable/granular synthesis.
    ///
    /// The memory area is of unknown type
    ///
    TD_VIRTUAL=0,

    /// Frequency domain virtual buffer represent parts of another frequency domain
    /// buffer
    /// The buffer layout is: Vertical: time , Horizontal: Freqency spectrum
    /// You cannot convert a sample with subwaves into frequency domain.
    /// The memory area is of unknown type
    ///
    FD_VIRTUAL=1,

    /// Time domain Audio buffers are located in audio driver space. 
    /// The buffer layout is: Vertical: 1, Horizontal: Time (samples data)
    /// What you write is what you hear.
    //
    TD_AUDIO=2,

    /// Frequency domain Audio buffers are locate in audio driver space.
    /// Actually I do not know about any soundcard that is able to do
    /// an inverse DFT in hardware. 
    /// Can be used by audio drivers with integrated DSPs ?
    ///
    FD_AUDIO=3,

    /// Time domain raw buffers are located in main memory.
    /// Fast to access by CPU but they need to be copied to an AUDIO buffer 
    /// to aurealize them 8)
    ///
    TD_RAW=4,

    /// Frequency domain raw buffers are located in main memory.
    /// The buffer layout is : Vertical: time , Horizontal: freq spectrum
    ///
    FD_RAW=5
  };


public:
  /// Pointer to audio buffer
  Bitmap *buffer;

  /// True if the buffer is allocated/freed by this class
  sBool own_buffer;
  
  /// Sample-Type of audio buffer
  /// o
  int stype;

  int type;

  /// Number of interleaved channels
  /// 1=Mono, 
  /// 2=Stereo, 
  /// 3=?, 
  /// 4=Quad, 
  /// 5=Surround (seperated frontleft, frontright, rearleft, rearright, bass)
  /// >5 = multichannel :)
  /// Default=0
  int channels;

  /// Sample frequency in Hz (number of samples/sec)
  /// Default=0
  sF32 frequency;

  /// Sample base frequency in Hz
  sF32 base_frequency;

  /// Sample offset (number of samples)
  /// Default=0
  int offset;

  /// Sample length (number of samples)
  /// This must NOT match the buffer (horizontal/vertical) size
  /// Default=0
  int length;

  /// Sample repeat offset (number of samples)
  /// Default=0
  int repeat_offset;

  /// Sample repeat length (number of samples)
  /// Default=0
  int repeat_length;

   /// FFT Window size for FD_* buffers.
  int fft_m;
 
  /// reference to avi audio stream class if in use
  //TKSAVIFileAudio *avifileaudio;

public:
  _AudioSample();
  ~_AudioSample();

  YAC(_AudioSample);

  /// Allocate new audio buffer
  /// @param s Sample type (see enum _stypes). One of Sample[8,16,32,64]
  /// @param t Type (see enum _atypes). Must be TD_RAW or FD_RAW.
  /// If you allocate a frequency domain (FD_) buffer, every line in the
  /// buffer represents the frequency spectrum for the time domain sample nr y.
  /// @param l Number of samples (/subwave!)
  /// @param f Frequency. Does not affect the buffer size.
  /// @param i Number of interleaved channels
  /// @param sw Number of waves (if type != FD_*).
  /// @param fftwin FFT window size (if type==FD_*) (=>Number of frequencies/ sample)
  /// Typical values are 1,2,3,4,5,6,7,8,9,10. A window size of fftwin will
  /// result in n=((1<<fftwin)/2) frequencies 
  /// ( f= (i*s)/(1<<(fftwin+1)) ; i=1..n-1, s= 1..44100 )
  /// ( i= (f*(1<<(fftwin+1)))/s ; i=1..n-1, s= 1..44100 )
  ///
  /// @return True if buffer was allocated. False if not
  sBool allocBuffer(int s, int t, int l, sF32 f=44100.0, int i=1, int sw=1, int fftwin=0  );

  /// Delete buffer if it is owned by this object (own_buffer flag)
  void deleteBuffer();
  
  /// Use auxiliary buffer (shared with other object)
  /// @param b Pointer to audiobuffer 
  void setBuffer(Bitmap *b);

  /// Generate a sinus signal with frequency <b>freq</b> using a
  /// samplerate of <b>frate</b> (mono)
  /// @param s Sampletype
  /// @param l Number of samples
  /// @freq Sinus frequency
  /// @frate Signal sample rate
  void generateSinus(int s, int l, float freq=440, float frate=44100);

  /// Generate a 0 signal
  /// @param s Sampletype
  /// @param l Number of samples
  /// @param c Number of channels
  /// @param frate Signal sample rate
  void generateSilence(int s, int l, int c, float frate=44100.0);

  /// Generate a pulse signal with frequency <b>freq</b> using a
  /// samplerate of <b>frate</b> (mono).
  /// The generated pulse will have a pulsewidth of <b>pw</b> degrees.
  ///
  /// @param s Sampletype
  /// @param l Number of samples
  /// @param pw Pulsewidth in 0..2PI
  /// @param freq Pulse frequency
  /// @param frate Signal sample rate
  ///
  void generatePulse(int s, int l, float pw, float freq=440, float frate=44100);


  /// Normalize samples. 
  /// This will multiply all samples with MAX_RANGE/max_used_amplitude
  void normalize();

  /// Get index of maximum amplitude in time domain sample.
  /// @param sw Subwave # (0..buffer->s.y-1)
  /// @return Index ( x-position in buffer )
  int getTDMax(int sw=0);

  sBool loadAIFFFile(sBool _local, YAC_String *fname);

  void getNextLoop(sU16 *_loopindex, sU32 *_offset, sU32 *_length);

  void YAC_VCALL yacOperator(sSI, YAC_Object*, YAC_Value *_r);


  /* @method allocBuffer,int l,float f,int ch,int sw:boolean
     Allocate new audio buffer
     @arg l Number of samples
     @arg f Frequency (Hz). Does not affect the buffer size.
     @arg ch Number of interleaved channels
     @arg sw Number of waves (usually 1..)
     @return True if buffer was allocated. False if not
  */
  YM sSI  _allocBuffer      (sSI _l, sF32 _f, sSI _ch, sSI _sw);


  /* @method freeBuffer
     Free memory occupied by sample data
  */
  YM void _freeBuffer       (void);


  /* @method visitBuffer,AudioSample o
     Share sample data with other sample
  */
  YM void _visitBuffer      (YAC_Object *);

  
  /* @method normalize
     Normalize sample data
  */
  YM void _normalize        (void);


  /* @method loadAIFF,String name:boolean
     Load AIFF sample from pak file
  */
  YM sSI  _loadAIFF         (YAC_String *_name);


  /* @method loadLocalAIFF,String name:boolean
     Load AIFF sample from local file
  */
  YM sSI  _loadLocalAIFF    (YAC_String *_name);


  /* @method getChannels:int
     Query number of channels
  */
  YM sSI  _getChannels      (void);


  /* @method getFrequency:float
     Query sample frequency
  */
  YM sF32 _getFrequency     (void);


  /* @method getOffset:int
     Query sample offset
  */
  YM sSI  _getOffset        (void);

  
  /* @method setOffset,int off
     Set sample offset
  */
  YM void _setOffset        (sSI);


  /* @method getLength:int
     Query sample length
  */
  YM sSI  _getLength        (void);


  /* @method setLength,int len
     Set sample length
  */
  YM void _setLength        (sSI);


  /* @method getRepeatOffset:int
     Get sample repeat offset
  */
  YM sSI  _getRepeatOffset  (void);


  /* @method setRepeatOffset,int off
     Query sample repeat offset
  */
  YM void _setRepeatOffset  (sSI);


  /* @method getRepeatLength:int
     Query sample repeat length
  */
  YM sSI  _getRepeatLength  (void);

  
  /* @method setRepeatLength,int len
     Set sample repeat length
  */
  YM void _setRepeatLength  (sSI);


  /* @method copyBufferFrom,AudioSample o
     Copy audio sample data from other sample
  */
  YM sSI  _copyBufferFrom   (YAC_Object *_sample);

  
  /* @method getBaseFrequency:float
     Query sample base frequency
  */
  YM sF32 _getBaseFrequency (void);


  /* @method setBaseFrequency,float f
     Set sample base frequency
  */
  YM void _setBaseFrequency (sF32);


  /* @method setFrequency,float f
     Set sample frequency
  */
  YM void _setFrequency     (sF32);

  
  /* @method convertChannels,int ch
     Convert sample to given number of channels
  */
  YM sSI  _convertChannels  (sSI ch);
};


#endif // __TKS_AUDIOSAMPLE_H__

