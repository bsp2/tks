/// YAC_PakFile.h
///
/// (c) 2001-2026 Bastian Spiegel <bs@tkscript.de>
///     - distributed under terms of the Lesser GNU General Public License (LGPL)
///

#ifndef YAC_PAKFILE_H__
#define YAC_PAKFILE_H__


class YAC_PakFile : public YAC_StreamBase {
public:
	TKS_StreamIO *sio;

public:
	YAC_PakFile(void);
	~YAC_PakFile();

	void  YAC_VCALL yacOperator(sSI, YAC_Object *, YAC_Value *);

	sSI   YAC_VCALL yacStreamGetErrorCode(void);
	sUI   YAC_VCALL yacStreamGetOffset(void);
	void  YAC_VCALL yacStreamSetOffset(sUI);
	void  YAC_VCALL yacStreamSeek(sSI _offset, sUI _mode);
	sU32  YAC_VCALL yacStreamGetSize(void);
	sSI   YAC_VCALL yacStreamRead(sU8 *, sU32);
	sU8   YAC_VCALL yacStreamReadI8(void);
	sBool YAC_VCALL yacStreamEOF(void);
	sBool YAC_VCALL yacStreamOpenLogic(sChar *_logic_name);
	sBool YAC_VCALL yacStreamOpenLocal(sChar *_logic_name, sSI _access);
	void  YAC_VCALL yacStreamClose(void);
	sBool YAC_VCALL yacIsStream(void);

   sBool YAC_VCALL yacStreamIsReadable (void) { return YAC_TRUE;  }
   sBool YAC_VCALL yacStreamIsWritable (void) { return YAC_FALSE; }

	sSI  _open      (YAC_String *_name);
	sSI  openLocal (YAC_String *_name, sSI _access);
	sSI  isOpen    (void);
};


#endif // YAC_PAKFILE_H__
