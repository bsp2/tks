/// bitmap.cpp
///
/// (c) 1999-2019 Bastian Spiegel <bs@tkscript.de>
///     - Distributed under terms of the Lesser GNU General Public License (LGPL).
///       See COPYING and <http://www.gnu.org/licenses/licenses.html#LGPL> for further information.
///
///

#include <yac.h>

#include "bitmap.h"


Bitmap::Bitmap(void) {
	data.any = NULL;
	owndata  = YAC_FALSE;
	stride   = 0;
	z        = 0;
	s.x      = 0;
	s.y      = 0;
}

Bitmap::~Bitmap(void) {
	Bitmap::free();
}

sBool Bitmap::alloc(sS32 size_x, sS32 size_y, sS32 depth) {
	tksvec2i s;
	s.x = size_x;
	s.y = size_y;
	return Bitmap::alloc(s, depth);
}

sBool Bitmap::alloc(tksvec2i size, sS32 depth) {
	// ---- delete previously allocated data array ----
	Bitmap::free();
	
   data.u8 = new(std::nothrow) sU8[ (size.x*size.y*depth) ];

	if(NULL == data.u8)
	{
		return YAC_FALSE;
	}

	stride = (size.x * depth);
	z = depth;
	s = size;
   owndata = YAC_TRUE;

	return YAC_TRUE;
}

void Bitmap::visitBuffer(sU8 *_data, sUI _w, sUI _h, sUI _z) {
   s.x     = _w;
   s.y     = _h;
   z       = _z;
   stride  = _w * _z;
   owndata = 0;
   data.u8 = _data;
}

void Bitmap::free(void) {
	if(NULL != data.any)
	{
		if(owndata) 
		{
			delete [] data.u8;
			owndata = YAC_FALSE;
		}
		data.any = NULL;
	}

	z = 0;
	s.x = 0;
	s.y = 0;
}

sS32 Bitmap::getDepth(void) {
	if(NULL != data.any)
   {
      return z;
   }
   else
   {
      return 0;
   }
}

yacmemptr Bitmap::getData(sS32 x, sS32 y) {
	yacmemptr ret;
	ret.any = NULL;

	if(0 != getDepth())
	{
		if( (x >= 0) && (x < s.x) && (y >= 0) && (y < s.y) )
      {
			ret.u8 = data.u8 + (s.x * y * z) + (x * z);
      }
	}
	return ret;
}
