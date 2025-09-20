
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <yac.h>
#include <yac_host.cpp>
#include "eqxmms.h"

YAC_Host *yac_host;

YG("equ");

YC class _EQXMMS : public EQXMMS {
public:
	YAC(_EQXMMS);

	void YAC_VCALL yacGetConstantStringList(YAC_String*);

  YM //void init                      (void);
  YM //void setBandType               (sSI);
  YM //sSI  getBandType               (void);
  YM //void setExtraFiltering         (sSI);
  YM //sSI  getExtraFiltering         (void);
  YM //void setBandGain               (sSI, sF32, sF32);
  YM //sF32 getBandGain               (sSI, sSI);
  YM void _iir                        (YAC_Object *);
  YM //void adjustISOBand             (sSI, sF32);
  YM //void setBandFrequency          (sSI, sF32);
  YM //sF32 getBandFrequency          (sSI);
  YM //sF32 getLinearISOFrequency     (sF32);
  YM //sF32 getLinearISOBandFrequency (sSI);
  YM //sF32 mapLinearISOFrequency     (sF32);
  YM //void loadISOFrequencies        (void);
};

//YAC_C(_TKS_EQXMMS, "EQXMMS");

void YAC_VCALL _EQXMMS::yacGetConstantStringList(YAC_String *_s) {
	_s->visit(
"EQXMMS_BAND_ORIG:0 "
"EQXMMS_BAND_XMMS:0 "
"EQXMMS_BAND_ISO10:1 "
"EQXMMS_BAND_ISO15:2 "
"EQXMMS_BAND_ISO25:3 "
"EQXMMS_BAND_ISO31:4 "
"EQXMMS_BAND_NUM:5 "
);
}


void _EQXMMS::_iir(YAC_Object *_fa) {
	if(YAC_BCHK(_fa, YAC_CLID_FLOATARRAY))
	{
		YAC_FloatArray *fa=(YAC_FloatArray*)_fa;
		iir(fa->elements, fa->num_elements);
	}
}

#include "ying_equ.h"
#include "ying_equ.cpp"
#include "ying_equ_EQXMMS.cpp"

YAC_APIC void YAC_CALL YAC_Init(YAC_Host *_host) {
	yac_host=_host;

	yac_host->printf("tkeqxmms::init\n");
	YAC_Init_equ(_host);
}

YAC_APIC void YAC_CALL YAC_Exit(YAC_Host *_host) {
	yac_host->printf("tkeqxmms::exit\n");

	YAC_Exit_equ(_host);
}

sUI YAC_CALL YAC_Version(void) {
	return 0x00080404;
}
