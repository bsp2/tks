// YAC interface wrapper for the SidPlay2 player library
//
// last changed: 9-Jan-2005, 17-Apr-2009 
//
#include <math.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fstream>//.h>

#define YAC_PRINTF defined
#include <yac.h>

////#include <tks-list.h>
#include <yac_host.cpp>
YAC_Host *yac_host;

#include <sidplay/sidplay2.h>
#include <sidplay/builders/resid.h>
#include <sidplay/SidTune.h>

YG("sidplay2");

#include "ying_sidplay2.h"

// --------------------------------------------------------------------------- SidTune
YC class _SidTune : public YAC_Object, public SidTune {
public:
	YAC(_SidTune);

	_SidTune(void) : SidTune(0) { }
	~_SidTune() { }

	void YAC_VCALL yacGetConstantStringList(YAC_String *_s) {
		_s->visit("");
	}

	YM sSI _load(YAC_Object *_s) {
		if(YAC_CHK(_s, YAC_CLID_STRING))
		{
			YAC_String *s=(YAC_String*)_s;
			return load((const char*)s->chars, 0)?0:1;
		}
		return 0;
	}


	YM sSI _selectSong(sSI _nr) {
		return selectSong((const uint_least16_t) _nr);
	}

	YM sSI _getSongs(void) {
		SidTuneInfo in=getInfo();
		return in.songs;
	}

	YM sSI _getStartSong(void) {
		SidTuneInfo in=getInfo();
		return in.startSong;
	}

	YM void _getInfoStrings(YAC_Value *_r) {
		SidTuneInfo in=getInfo();
		if(in.numberOfInfoStrings)
		{
			YAC_Object *sa=yac_host->yacNewByID(YAC_CLID_STRINGARRAY);
			if(sa)
			{
				if(sa->yacArrayAlloc(in.numberOfInfoStrings, 0, 0, 0))
				{
					YAC_RETO(sa, 1);
					sUI i;
					YAC_Value v;

					for(i=0; i<in.numberOfInfoStrings; i++)
					{
						sa->yacArrayGet(NULL, i, &v);
						if(v.value.string_val)
						{
							YAC_String *t=v.value.string_val;
							t->visit(in.infoString[i]);
						}
					}
					sa->yacArraySetWidth(in.numberOfInfoStrings);
					return;
				}
			}
		}
		YAC_RETO(0, 0);
	}

	YM sSI _loadStream(YAC_Object *_s) {
		if(YAC_BCHK(_s, YAC_CLID_STREAM))
			return loadYACStream(_s, 0)?0:1;
		else
            return 0;
	}

};
#include "ying_sidplay2_SidTune.cpp"

// --------------------------------------------------------------------------- SidPlay2
YCS class _SidPlay2 : public YAC_Object, public sidplay2 {
public:
	ReSIDBuilder *resid;
	sSI           cfg_sidModel;

public:
	YAC(_SidPlay2);
	
    _SidPlay2(void) {
		debug(1, stdout);

		cfg_sidModel = SID2_MOS8580; 

		resid=new ReSIDBuilder("reSID");
		resid->create(info().maxsids);
	}

	~_SidPlay2() {
		delete resid;
		resid=0;
	}

	void YAC_VCALL yacGetConstantStringList(YAC_String *_s) {
		_s->visit(
				"SID2_MOS6581:1 "
				"SID2_MOS8580:2 ");
	}

    YM //void stop         (void);
    YM //void pause        (void);
	YM void _fastForward (sSI _pct) {
		fastForward((uint)_pct);
	}

	YM void _play(YAC_Object *_fa) {
		if(YAC_CHK(_fa, YAC_CLID_FLOATARRAY))
		{
			YAC_FloatArray *fa=(YAC_FloatArray*)_fa;
			static sS16 buf[32*1024];
			play((void*)buf, fa->num_elements);
			sUI i;
			for(i=0; i<fa->num_elements; i+=2)
				fa->elements[i]=fa->elements[i+1]=(sF32) (buf[i>>1]*(1.0/32767.0));
		}
	}

	YM void _load(YAC_Object *_o) {
		if(YAC_CHK(_o, clid_SidTune))
		{
			_SidTune *st=(_SidTune*)_o;

		sid2_config_t      m_engCfg;
		m_engCfg = config (); 

		m_engCfg.precision=16;
        m_engCfg.frequency    = 44100;
		m_engCfg.leftVolume      = 125;
		m_engCfg.rightVolume     = 125;
		m_engCfg.playback=sid2_mono;
		m_engCfg.sampleFormat=SID2_LITTLE_SIGNED;
		
		m_engCfg.clockSpeed=SID2_CLOCK_CORRECT;
		m_engCfg.clockDefault=SID2_CLOCK_PAL;//SID2_CLOCK_CORRECT;
		m_engCfg.clockForced=0;
		
		m_engCfg.environment=sid2_envR; //sid2_envPS;
		m_engCfg.sidSamples=1;
		m_engCfg.forceDualSids=0;
		m_engCfg.sidEmulation=0;
		m_engCfg.sidModel = (sid2_model_t)cfg_sidModel ;//SID2_MOS6581;//SID2_MOS8580; 
        m_engCfg.sidDefault   = (sid2_model_t)cfg_sidModel;//SID2_MOS6581; //SID2_MOS8580;//6581;
		m_engCfg.optimisation=SID2_DEFAULT_OPTIMISATION;
		
        //m_filter.enabled      = emulation.filter; 
		
		m_engCfg.sidEmulation=resid;
		m_engCfg.precision=16;
		m_engCfg.playback=sid2_mono;
		m_engCfg.frequency=44100;

		config (m_engCfg);

			load(st);
		}
	}

	YM void _setSidModel(sSI _i) {
		cfg_sidModel=_i;
	}
};
#include "ying_sidplay2_SidPlay2.cpp"


// --------------------------------------------------------------------------- 

#include "ying_sidplay2.cpp"

void YAC_CALL YAC_Init(YAC_Host *_host) {
	yac_host=_host;
	if(yac_host->yacGetDebugLevel())
		yac_host->yacPrint("tksidplay2::YAC_Init() called.\n");

    YAC_Init_sidplay2(_host);

	if(yac_host->yacGetDebugLevel())
		yac_host->yacPrint("tksidplay2::YAC_Init() finished.\n");
}

void YAC_CALL YAC_Exit(YAC_Host *_host) {
	if(yac_host->yacGetDebugLevel())
		yac_host->yacPrint("tksidplay2::YAC_Exit() called.\n");

    YAC_Exit_sidplay2(_host);

	if(yac_host->yacGetDebugLevel())
		yac_host->yacPrint("tksidplay2::YAC_Exit() finished.\n");
}

sUI YAC_CALL YAC_Version(void) {
	return 0x00020000;
}
