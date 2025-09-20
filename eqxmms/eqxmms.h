/*
 *   PCM time-domain equalizer
 *
 *   Copyright (C) 2002  Felipe Rivera <liebremx at users.sourceforge.net>
 *
 *   19.08.2002 Initial release for XMMS
 *   --taken from the eq-xmms-0.5 distribution. modified for use as a tks plugin
 *     by Bastian Spiegel <bs@tkscript.de>.
 *     13-Dec-2003. Last modified: 17-Dec-2003. (added band freq. adjust)
 *
 *   
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *   $Id: eq.h,v 1.1 2003/05/21 13:59:59 liebremx Exp $
 */
#ifndef EQ_H
#define EQ_H

#include <stdio.h>
#include <string.h>

/* Max bands supported by the code
 FIXME: 31 bands processing IS HARD ON THE PROCESSOR
            On a PIII@900Mhz I got ~45% of CPU usage
            Have to improve the filter implementation
            Got it down to ~20% for 31 Bands
            Still MMX/3DNow!/SSE whatever, can bring it down */
#define EQ_MAX_BANDS 31
/* Number of channels (Stereo) */
#define EQ_CHANNELS 2

#define EQ_LEFT 0
#define EQ_RIGHT 1


enum t_eqxmms_band {
	EQXMMS_BAND_XMMS=0,
	EQXMMS_BAND_ISO10,
	EQXMMS_BAND_ISO15,
	EQXMMS_BAND_ISO25,
	EQXMMS_BAND_ISO31,

	EQXMMS_BAND_NUM,
};

// Fixed Point Fractional bits
#define FP_FRBITS 28	

// Conversions
#define EQ_REAL(x) ((gint)((x) * (1 << FP_FRBITS)))

/* Floating point */
typedef struct 
{
	double beta;
	double alpha; 
	double gamma;
}sIIRCoefficients;

/* Coefficient history for the IIR filter */
typedef struct
{
	float x[3]; /* x[n], x[n-1], x[n-2] */
	float y[3]; /* y[n], y[n-1], y[n-2] */
}sXYData;



class EQXMMS : public YAC_Object {
public:
	struct __eqconfig
	{
		sSI   band_num;
		sBool use_xmms_original_freqs,
			  use_independent_channels,
			  extra_filtering
			;
	} eqcfg;
	sSI i_band_type;

	float gain[EQ_MAX_BANDS][EQ_CHANNELS];
	float preamp[EQ_CHANNELS];
	float fa_bandfreq[EQ_MAX_BANDS];

	/* History for two filters */
	sXYData data_history  [EQ_MAX_BANDS][EQ_CHANNELS];
	sXYData data_history2 [EQ_MAX_BANDS][EQ_CHANNELS];

	/* Coefficients */
	const sIIRCoefficients *iir_ccf;
	sIIRCoefficients iir_cf[31];

	/* Indexes for the history arrays*/
	sSI off_i, off_j, off_k;	

public:
	EQXMMS(void);
	~EQXMMS();

	void init(void);
	void init_iir();

	void setBandType               (sSI _type); //selectISOBand
	sSI  getBandType               (void);
	void adjustISOBand             (sSI _index, sF32 _fader);
	void setBandFrequency          (sSI _index, sF32 _freq);
	sF32 getBandFrequency          (sSI _index);
	sF32 getLinearISOFrequency     (sF32 _freq);
	sF32 getLinearISOBandFrequency (sSI _index);
	sF32 mapLinearISOFrequency     (sF32 _freq);
	void setBandGain               (sSI index, sF32 value, sF32 _pan);
	sF32 getBandGain               (sSI index, sSI _ch);
	void loadISOFrequencies        (void);
	void setExtraFiltering         (sSI);
	sSI  getExtraFiltering         (void);

	sSI  iir(sF32 * d, sSI length);

private:
	void eq_read_config(void);
	




public:



};

#endif
