
#define YAC_BIGSTRING
#define YAC_PRINTF

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <yac.h>
#include "eqxmms.h"

/* IIR filter coefficients */
#include "iir_cf.h"

typedef struct 
{
	const sChar *bands_text;
	sSI          bands;
	sBool        original_freq;
}EQBandsChoices;

static const EQBandsChoices bc[] =
{
	{ "10 bands, original XMMS frequencies", 10, true },
	{ "10 bands, ISO spaced frequencies", 10, false },
	{ "15 bands, ISO spaced frequencies", 15, false },
 	{ "25 bands, ISO spaced frequencies", 25, false },
	{ "31 bands, ISO spaced frequencies", 31, false },
	{ 0 }
};


EQXMMS::EQXMMS(void) {
	off_i=0;
	off_j=2;
	off_k=1;
	i_band_type=0;
	eq_read_config();
	init();
}

EQXMMS::~EQXMMS() {
}

void EQXMMS::init(void)
{
	setBandType(EQXMMS_BAND_ISO31);
}

void EQXMMS::setBandGain(sSI index, sF32 value, sF32 _pan)
{
	//printf("setBandGain(%i, %f, %f)\n", index, value, _pan);
	/* Map the gain and preamp values */
	if(_pan>1)_pan=1; else if(_pan<-1)_pan=-1;
   sF32 p=((1.0f-_pan)/2.0f);
   sF32 voll=value*p;
   sF32 volr=value*(1.0f-p);
	if (index >= 0)
	{
		gain[index][0]= 0.03f * voll + 0.000999999f * voll * voll;
		gain[index][1]= 0.03f * volr + 0.000999999f * volr * volr;
	}
	else
	{
      /* -12dB .. 12dB mapping */
 		preamp[0]=1 + 0.0932471f * voll + 0.00279033f * voll * voll;
 		preamp[1]=1 + 0.0932471f * volr + 0.00279033f * volr * volr;
	}
}

sF32 EQXMMS::getBandGain(sSI index, sSI _ch) {
	if(((sUI)_ch)<EQ_CHANNELS)
	{
		if( ((sUI)index) < 31 )
		{
			return gain[index][_ch];
		}
		else
		{
 			return preamp[_ch];/* -12dB .. 12dB mapping */
 		}
	}
	return 0;
}

void EQXMMS::setBandType(sSI _type) 
{
	const EQBandsChoices *pbc;
	if(((sUI)_type)<EQXMMS_BAND_NUM)
	{
		pbc = &bc[_type];
		eqcfg.band_num = pbc->bands;
		eqcfg.use_xmms_original_freqs = pbc->original_freq;
		i_band_type=_type;
		init_iir();
	}
}

sSI EQXMMS::getBandType(void) {
	return i_band_type;
}

void EQXMMS::setExtraFiltering(sSI _b) {
	eqcfg.extra_filtering=(sBool)_b;
}

sSI EQXMMS::getExtraFiltering(void) {
	return eqcfg.extra_filtering;
}

void EQXMMS::adjustISOBand(sSI _index, sF32 _fader) {
	if(_index<(eqcfg.band_num-1))
	{
		const sIIRCoefficients *cl=&iir_ccf[_index];
		const sIIRCoefficients *cr=&iir_ccf[_index+1];
		sIIRCoefficients *cd=&iir_cf[_index];
		cd->alpha=cl->alpha+(cr->alpha-cl->alpha)*_fader;
		cd->beta =cl->beta +(cr->beta -cl->beta )*_fader;
		cd->gamma=cl->gamma+(cr->gamma-cl->gamma)*_fader;
	}
}

static sF32 EQXMMS_band_freq_table[]={
   20.0f, 25.0f, 31.5f, 40.0f, 50.0f, 63.0f, 80.0f, 100.0f, 125.0f, 160.0f, 200.0f, 250.0f, 315.0f,
	400.0f, 500.0f, 630.0f, 800.0f, 1000.0f, 1250.0f, 1600.0f, 2000.0f, 2500.0f, 3150.0f, 4000.0f,
	5000.0f, 6300.0f, 8000.0f, 10000.0f, 12500.0f, 16000.0f, 20000.0f, 0
};

sF32 EQXMMS::mapLinearISOFrequency(sF32 _freq) {
	if(_freq>=0.0 && _freq<=30.0)
	{
		sSI i=(int)_freq;
		_freq-=i;
		sF32 *bft=EQXMMS_band_freq_table;
		return bft[i]+(bft[i+1]-bft[i])*_freq;
	}
	return 0;
}


sF32 EQXMMS::getLinearISOFrequency(sF32 _freq) {
	// return the position of the frequency 
	// on the linear ISO31 band. the return value is 0.0..30.0.
	sF32 *bft=EQXMMS_band_freq_table;
	sSI i=30;
	while((--i>=0)&&(_freq<bft[i]) );
	if(i>=0)
	{
		sF32 cf=(_freq-bft[i])/(bft[i+1]-bft[i]);
		cf+=i;
		if(cf>30.0)cf=30.0;
      else if (cf<0.0)cf=0.0;
		return cf;
	}
	return 0;
}

sF32 EQXMMS::getLinearISOBandFrequency(sSI _index) {
	// return where the band[_index] frequency is located
	// on the linear ISO31 band. the return value is 0.0..30.0.
	if( ((sUI)_index)<31 )
		return getLinearISOFrequency(fa_bandfreq[_index]);
	return 0;
}


void EQXMMS::setBandFrequency(sSI _index, sF32 _freq) {
	//printf("setBandFrequency(%i, %f)\n", _index, _freq);
	if((eqcfg.band_num==31)&&(((sUI)_index)<31) )
	{
		sF32 cf=getLinearISOFrequency(_freq);
		if(cf!=0.0)
		{
			sSI i= (int)cf;
			cf-=i;
			iir_cf[_index].alpha=iir_ccf[i].alpha+(iir_ccf[i+1].alpha-iir_ccf[i].alpha)*cf;
			iir_cf[_index].beta =iir_ccf[i].beta +(iir_ccf[i+1].beta -iir_ccf[i].beta )*cf;
			iir_cf[_index].gamma=iir_ccf[i].gamma+(iir_ccf[i+1].gamma-iir_ccf[i].gamma)*cf;
			fa_bandfreq[_index]=_freq;
			//yac_host->printf("band[%i].frequency=%f\n",_index, _freq);
		}

	}
}

sF32 EQXMMS::getBandFrequency(sSI _index) {
	if(((sUI)_index)<31)
		return fa_bandfreq[_index];
	else
		return 0;
}

void EQXMMS::eq_read_config(void)
{
	/* Defaults */
	eqcfg.band_num = 15;
	eqcfg.use_xmms_original_freqs = false;
	eqcfg.use_independent_channels = false;
	eqcfg.extra_filtering = true;;
}


void EQXMMS::loadISOFrequencies(void) {
	/* Select the coefficients based on the number of bands */
	if (eqcfg.band_num == 15)
	{
		iir_ccf = iir_cf15;
	}
	else if (eqcfg.band_num == 31)
	{
		iir_ccf = iir_cf31;
	}
 	else if (eqcfg.band_num == 25)
 	{
 		iir_ccf = iir_cf25;
 	}
	else if (eqcfg.band_num == 10 && eqcfg.use_xmms_original_freqs)
	{
		iir_ccf = iir_cforiginal10;
	}
	else
	{
		iir_ccf = iir_cf10;
	}
	sSI i;
	for(i=0; i<EQ_MAX_BANDS; i++)
	{
		iir_cf[i].alpha=iir_ccf[i].alpha;
		iir_cf[i].beta =iir_ccf[i].beta;
		iir_cf[i].gamma=iir_ccf[i].gamma;
		fa_bandfreq[i]=EQXMMS_band_freq_table[i];
	}
}

/* Init the filter */
void EQXMMS::init_iir(void)
{
   printf("xxx init_iir\n");
	loadISOFrequencies();
	sSI i;
	for(i=0; i<EQ_MAX_BANDS; i++)
	{
		gain[i][0]=gain[i][1]=1;
	}
	preamp[0]=1;
	preamp[1]=1;

	/* Zero the history arrays */
	sUI sz=sizeof(sXYData) * EQ_MAX_BANDS * EQ_CHANNELS;
	sU8*d1=(sU8*)data_history;
	sU8*d2=(sU8*)data_history2;
	while(sz--) { *d1++=*d2++=0; }
}


sSI EQXMMS::iir(sF32 * d, sSI length)
{
	sF32 *data=d;
	sSI  index, band, channel;
	sF32 tmpflt;
	sF32 out[EQ_CHANNELS], 
      pcm[EQ_CHANNELS];

	/**
	 * IIR filter equation is
	 * y[n] = 2 * (alpha*(x[n]-x[n-2]) + gamma*y[n-1] - beta*y[n-2])
	 *
	 * NOTE: The 2 factor was introduced in the coefficients to save
	 * 			a multiplication
	 *
	 * This algorithm cascades two filters to get nice filtering
	 * at the expense of extra CPU cycles
	 */
	
	/* 32bit float, stereo sample pairs, so divide by two the length of
	 * the buffer (length is in #samples)
	 */

	for (index = 0; index < length; index+=2)
	{
		/* For each channel */
		for (channel = 0; channel < EQ_CHANNELS; channel++)
		{
			/* No need to scale when processing the PCM with the filter */
			pcm[channel] = data[index+channel];
			/* Preamp gain */
			pcm[channel] *= preamp[channel];

	
			out[channel] = 0;
			/* For each band */
			for (band = 0; band < eqcfg.band_num; band++)
			{
				/* Store Xi(n) */
				data_history[band][channel].x[off_i] = pcm[channel];
				/* Calculate and store Yi(n) */
				data_history[band][channel].y[off_i] = (sF32)
               (
               	/* 		= alpha * [x(n)-x(n-2)] */
						iir_cf[band].alpha * ( data_history[band][channel].x[off_i]
                                         -  data_history[band][channel].x[off_k])
               	/* 		+ gamma * y(n-1) */
                  + iir_cf[band].gamma * data_history[band][channel].y[off_j]
               	/* 		- beta * y(n-2) */
                  - iir_cf[band].beta * data_history[band][channel].y[off_k]
                );
				/* 
				 * The multiplication by 2.0 was 'moved' into the coefficients to save
				 * CPU cycles here */
            /* Apply the gain  */
				out[channel] +=  data_history[band][channel].y[off_i]*gain[band][channel]; // * 2.0;
            //printf("xxx 2 out[channel]=%f\n", out[channel]);
			} /* For each band */

         if (eqcfg.extra_filtering)
         {
				/* Filter the sample again */
				for (band = 0; band < eqcfg.band_num; band++)
				{
					/* Store Xi(n) */
					data_history2[band][channel].x[off_i] = out[channel];
					/* Calculate and store Yi(n) */
					data_history2[band][channel].y[off_i] = (sF32)
                  (
	               	/* y(n) = alpha * [x(n)-x(n-2)] */
							iir_cf[band].alpha * (data_history2[band][channel].x[off_i]
                                           -  data_history2[band][channel].x[off_k])
               		/* 	    + gamma * y(n-1) */
                     + iir_cf[band].gamma * data_history2[band][channel].y[off_j]
                     /* 		- beta * y(n-2) */
                     - iir_cf[band].beta * data_history2[band][channel].y[off_k]
                   );
					/* Apply the gain */
					out[channel] +=  data_history2[band][channel].y[off_i]*gain[band][channel];
				} /* For each band */
         }

			/* Volume stuff
			   Scale down original PCM sample and add it to the filters 
			   output. This substitutes the multiplication by 0.25
         */

			//out[channel] += (data[index+channel]>>2);
			tmpflt=out[channel] + data[index+channel]*0.25f;
			out[channel]=data[index+channel]=tmpflt;

		} /* For each channel */

		off_i++; off_j++; off_k++;
		
		/* Wrap around the indexes */
		if (off_i == 3) off_i = 0;
		else if (off_j == 3) off_j = 0;
		else off_k = 0;

		
	}/* For each pair of samples */

	return length;
}
