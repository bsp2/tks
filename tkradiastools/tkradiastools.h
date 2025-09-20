
// 01Aug2008, 12Apr2010, 09Jul2021, 11Jul2021

#ifndef __TKRADIASTOOLS_H__
#define __TKRADIASTOOLS_H__


YG("tkradiastools")


#define RADIAS_DEVICE_NR 0

#define RADIAS_TIMBREDATASIZE 228 // docs say 226 but it is 228 !

#define RADIAS_PROGRAMDATASIZE_DEC 0x6FE // decoded size
#define RADIAS_DRUMKITDATASIZE_DEC 0x6B4 // decoded size

#define RADIAS_FUN_PARAMETERCHANGE        0x41
#define RADIAS_FUN_DRUMKITPARAMETERCHANGE 0x53



YF void YAC_CALL tkradiastools_update_program_timbre(YAC_Object *_progState, YAC_Object *_progData, sSI _timbreNr, sSI _bSendFX, YAC_Object *_out, YAC_Object *_cc, sBool _bForce);

YF void YAC_CALL tkradiastools_update_drumkit_timbre(YAC_Object *_drumkitState, YAC_Object *_drumkitData, sSI _timbreNr, YAC_Object *_out, sBool _bForce);

YF void YAC_CALL tkradiastools_copy_program_timbre(YAC_Object *_progState, YAC_Object *_progData, sSI _timbreNr);

YF void YAC_CALL tkradiastools_copy_drumkit_timbre(YAC_Object *_drumkitState, sSI _dstTimbreNr, YAC_Object *_drumkitData, sSI _srcTimbreNr);

YF void YAC_CALL tkradiastools_update_program(YAC_Object *_progState, YAC_Object *_progData, sBool _bSendFX, YAC_Object *_out, YAC_Object *_cc, sBool _bForce);



#endif
