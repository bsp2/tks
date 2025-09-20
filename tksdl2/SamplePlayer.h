/// SamplePlayer.h
///
/// (c) 2002-2008 by Bastian Spiegel <bs@tkscript.de>
///     - Distributed under terms of the Lesser GNU General Public License (LGPL).
///       See COPYING and <http://www.gnu.org/licenses/licenses.html#LGPL> for further information.
///
///

#ifndef __TKSDL_SAMPLEPLAYER_H__
#define __TKSDL_SAMPLEPLAYER_H__


YG("audio");

#define TKS_MAX_AUDIOVOICES 16


/* @class SamplePlayer,Object
A very basic sample player class.

It's probably only good enough for simple game sound effects. Never used it for anything else, anyway ;)

Maximum polyphony is 16 voices.
*/
YC class _SamplePlayer : public YAC_Object {
public:
    sUI    polyphony; // max. # of voices
    sF32   volume;    // default channel volume
    sF32   pan;       // default channel panning
    sF32   finetune;  // default channel finetune

public:
    AudioSampleVoice voices[TKS_MAX_AUDIOVOICES];
    _AudioSample     *default_sample;


public:
    _SamplePlayer(void);
    ~_SamplePlayer();

	YAC(_SamplePlayer);

    void processSamples(sU32 _num, sF32 *_destbuf);

    void YAC_VCALL yacOperator(sSI, YAC_Object*, YAC_Value *);


    /* @method setPolyphony,int numVoices
    Set polyphony

    @arg numVoices 1..16
    */
    YM void        _setPolyphony   (sSI);


    /* @method getPolyphony:int
    Query polyphony
    */
    YM sSI         _getPolyphony   (void);


    /* @method setVolume,float v
    Set global volume

    @arg v 0..1
    */
    YM void        _setVolume      (sF32);


    /* @method getVolume:float
    Query global volume
    */
    YM sF32        _getVolume      (void);


    /* @method noteOn,int note,float v
    Trigger note using the currently active sample
    */
    YM void        _noteOn         (sSI _midinote, sF32 _volume);


    /* @method noteOff,int note
    Stop sample that is has been playing the given note for the longest time
    */
    YM void        _noteOff        (sSI _note);


    /* @method getSample:AudioSample
    Query currently active sample
    */
    YM YAC_Object *_getSample      (void);


    /* @method setSample,AudioSample sample
    Set active sample
    */
    YM void        _setSample      (YAC_Object *_sample);


    /* @method processSamples,FloatArray fa
    Render/mix samples to the given output stream
    */
    YM void        _processSamples (YAC_Object *_fa);


    /* @method setPan,float v
    Set panning offset for next noteOn

    TODO: why is this not a parameter to the §noteOn method
    */
    YM void        _setPan         (sF32 _v);


    /* @method getPan:float
    Query panning offset for next noteOn calls
    */
    YM sF32        _getPan         (void);
};

#endif
