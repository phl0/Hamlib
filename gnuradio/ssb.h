/*
 *  Hamlib GNUradio backend - SSB class
 *  Copyright (c) 2003 by Stephane Fillod
 *
 *	$Id: ssb.h,v 1.1 2003-10-01 19:38:34 fillods Exp $
 *
 *   This library is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Library General Public License as
 *   published by the Free Software Foundation; either version 2 of
 *   the License, or (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU Library General Public License for more details.
 *
 *   You should have received a copy of the GNU Library General Public
 *   License along with this library; if not, write to the Free Software
 *   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

#ifndef _SSB_H
#define _SSB_H 1

#include "demod.h"

#include <GrSSBMod.h>		/* SSB */


class USBDemodChainCF : public DemodChainCF {
  private:
	GrSSBMod<d_oType> *s_demod;
	float low_cutoff;

  public:
	// float rf_gain = chan->levels[rig_setting2idx(RIG_LEVEL_RF)].f;
	USBDemodChainCF (VrSource<d_iType> *src, VrSink<d_oType> *snk, rmode_t mode, pbwidth_t width, int input_rate, freq_t centerfreq = 0, float rf_gain = 1.0) : 
		DemodChainCF(src, snk, mode, width, input_rate, centerfreq, rf_gain) { 

		low_cutoff = 300;
		// centerfreq, relative to IF_center_freq
		centerfreq += (freq_t)(low_cutoff + width/2);

		s_demod = new GrSSBMod<d_oType>(2*M_PI*(low_cutoff+width/2)/(double)input_rate,rf_gain);

		demod_in = demod_out = s_demod;
	}
	~USBDemodChainCF() { delete s_demod; }

	//void setWidth(pbwidth_t width) { }	/* TODO */
	void setFreq(freq_t centerfreq) { s_demod->set_freq(centerfreq); }
	void setRFgain(float gain) { s_demod->set_gain(gain); }
};

class LSBDemodChainCF : public DemodChainCF {
  private:
	GrSSBMod<d_oType> *s_demod;
	float low_cutoff;

  public:
	// float rf_gain = chan->levels[rig_setting2idx(RIG_LEVEL_RF)].f;
	LSBDemodChainCF (VrSource<d_iType> *src, VrSink<d_oType> *snk, rmode_t mode, pbwidth_t width, int input_rate, freq_t centerfreq = 0, float rf_gain = 1.0) : 
		DemodChainCF(src, snk, mode, width, input_rate, centerfreq, rf_gain) { 

		float low_cutoff = 300;
		// centerfreq, relative to IF_center_freq
		centerfreq += (freq_t)(-low_cutoff - width/2);

		s_demod = new GrSSBMod<d_oType>(-2*M_PI*(low_cutoff+width/2)/(double)input_rate,rf_gain);

		demod_in = demod_out = s_demod;
	}
	~LSBDemodChainCF() { delete s_demod; }

	//void setWidth(pbwidth_t width) { }	/* TODO */
	void setFreq(freq_t centerfreq) { s_demod->set_freq(centerfreq); }
	void setRFgain(float gain) { s_demod->set_gain(gain); }
};


#endif	/* _SSB_H */
