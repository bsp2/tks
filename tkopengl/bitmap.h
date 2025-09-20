/// bitmap.h
///
/// (c) 1999-2025 Bastian Spiegel <bs@tkscript.de>
///     - Distributed under terms of the Lesser GNU General Public License (LGPL).
///       See COPYING and <http://www.gnu.org/licenses/licenses.html#LGPL> for further information.
///
///
#ifndef TKS_BITMAP_H__
#define TKS_BITMAP_H__

typedef struct __vector2i {
	sS32 x;
	sS32 y;
} tksvec2i;

typedef struct __selection2i {
	tksvec2i p;
	tksvec2i s;
} tkssel2i;

typedef struct __clipvec2i {
	tkssel2i qs;
	tkssel2i ps;
} tksclipvec2i;

typedef
struct sPixel24 {
  sU8 r;
  sU8 g;
  sU8 b;
} sPixel24 ;

typedef
struct sPixel128 {
  sF32 r;
  sF32 g;
  sF32 b;
  sF32 a;
} sPixel128;

typedef
/// A single memory element
union _mem {
   sS8                s8;
   sU8                u8;
   sS16               s16;
   sU16               u16;
   sS32               s32;
   sU32               u32;
   sF32               f32;
   sPixel24           z24;
   /* sPixel128          z128; */
} mem;

#define cCol38216(r,g,b) ((sU16)((((r)&0xf8)<<8)|(((g)&0xfc)<<3)|(((b)&0xf8)>>3)))
#define cCol38232(r,g,b,a) ((sU32)(((a)<<24)|((r)<<16)|((g)<<8)|(b)))
#define cCol32216(v) ((sU16)((((v)&0x0000F8)>>3)|(((v)&0x00FC00)>>5)|(((v)&0xF80000)>>8)))

// 332222222222111111111100
// 10987654321098765432109876543210
// xxxxxxxxrrrrrrrrggggggggbbbbbbbb
//                 rrrrrggggggbbbbb
#define cCol16224(v) ( (((v&0xF800)<<8)|((v&0x3800)<<5)) | (((v&0x07E0)<<5)|((v&0x0060)<<3)) | (((v&0x1F)<<3)|((v&0x7))) )
#define cCol16232(v) ( 0xFF000000 | (((v&0xF800)<<8)|((v&0x3800)<<5)) | (((v&0x07E0)<<5)|((v&0x0060)<<3)) | (((v&0x1F)<<3)|((v&0x7))) )

#define cBitSel(a,b,c) (a)->data.u8+(a)->z*(b)+(a)->stride*(c)

class Bitmap /*: public YAC_Object*/ {
public:
  sBool     owndata;
  yacmemptr data;
  sS32      stride; /// Number of bytes in one scanline.
  sU8       z; // Number of bytes per pixel. 1=8bit,3=24bit,4=32bit/32bitAlpha, 16=RGBA float
  tksvec2i  s;

public:
  Bitmap(void);
  ~Bitmap(void);

  sBool     alloc(tksvec2i, sS32 depth);
  sBool     alloc(sS32 size_x, sS32 size_y, sS32 depth);
  void      free(void);
  sS32      getDepth(void);
  yacmemptr getData(sS32 x, sS32 y);
  void      visitBuffer(sU8 *_data, sUI _w, sUI _h, sUI _z);
};


#endif  // TKS_BITMAP_H__
