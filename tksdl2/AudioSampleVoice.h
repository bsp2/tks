/// AudioSampleVoice.h
///
/// (c) 2002-2005 by Bastian Spiegel <bs@tkscript.de>
///     - Distributed under terms of the Lesser GNU General Public License (LGPL).
///       See COPYING and <http://www.gnu.org/licenses/licenses.html#LGPL> for further information.
///

#ifndef __TKSDL_AUDIOSAMPLEVOICE_H__
#define __TKSDL_AUDIOSAMPLEVOICE_H__


YG("audio");

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~                                   ~
//~ AudioSampleVoice                  ~
//~                                   ~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
class AudioSampleVoice {
public:
    sU32                  playtime;  // #ticks 
    sF32                  volume;    // c
    sF32                  pan;
    sF32                  voll;      // pan/volume
    sF32                  volr;
    sF32                  frequency; // frequency
    sU8                   note;      // 0= 'c-1', lowest note actually used is 21 'a-2'
    sBool                 active;

public:
    _AudioSample *sample;

    sU32            intoffset;  // current sample replay offset
    sU32            length;     // current sample length
    sU16            loop_index;
    sU32            fracoffset;
    sU32            fracstep;

public:
    AudioSampleVoice(void);
    ~AudioSampleVoice();

    void processSamples(sU32 _n, sF32 *_destbuf);

    void _setFrequency (sF32);
    void _noteOn       (sSI _note, sF32 _velocity);
    void _noteOff      (void);
    void _setSample    (YAC_Object *_sample);
    void _setVolume    (sF32);
    void _setPan       (sF32);
};

#endif
