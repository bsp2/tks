/// image.cpp
///
/// author: (c) 1998-2024 Bastian Spiegel <bs@tkscript.de>
///     - Distributed under terms of the Lesser GNU General Public License (LGPL).
///       See COPYING and <http://www.gnu.org/licenses/licenses.html#LGPL> for further information.
///
///

#include <stdlib.h>
#include <string.h>

#ifndef YAC_PRINTF
  #define YAC_PRINTF defined
#endif
#include <yac.h>
extern YAC_Host *yac_host;

extern "C" {
#ifdef DX_GIF
#include <gif_lib.h>
#endif

#ifdef DX_PNG
extern "C" {
#include <png.h>
#ifdef YAC_WIN32
// statically linked libpng (using private png headers)
#include <pnglibconf.h>
#include <pngstruct.h>
#include <pnginfo.h>
#else
// shared libpng
#include <zlib.h>
#endif // YAC_WIN32
}
#endif

#ifdef DX_TIFF
#include <tiffio.h>
#endif

#ifdef DX_JPEG
#include <jpeglib.h>
#include <setjmp.h>
#endif
}


#include "bitmap.h"

#include "tks-list.h"

#include "image.h"

typedef 
union _imgmemptr {
  void      *any;
  sChar     *c;
  sS8       *s8;
  sU8       *u8;
  sS16      *s16;
  sU16      *u16;
  sS32      *s32;
  sU32      *u32;
  sF32      *f32;
  sSI       *si;
  sUI       *ui;
  sPixel24  *z24;
  sPixel128 *z128;
} imgmemptr;

typedef 
/// Structure for acceltab list entry. 
struct __acceltab {
	sS8  *data;
	sS32  handle;
	sS32  size;
} acceltab;

/// Typecast helper for acceltab structure 
#define tAcceltab(a) ((acceltab *)a)

Image::Image(void) {
	screen       = NULL;
	colormap     = NULL;
	acceltabs    = NULL;
	ownscreen    = YAC_FALSE;
	owncolormap  = YAC_FALSE;
	ownacceltabs = YAC_FALSE;
	type         = Image::VIRTUAL;
	sel.p.x = sel.p.y = sel.s.x = sel.s.y = 0;
}

Image::~Image(void) {
	deleteScreen();
	deleteColormap();
	freeAcceltabs();
}

void Image::deleteScreen(void) {
	if(ownscreen&&screen) delete screen;
	ownscreen = YAC_FALSE;
	screen = NULL;
}

void Image::deleteColormap(void) {
	if(owncolormap&&colormap) delete colormap;
	owncolormap = YAC_FALSE;
	colormap = NULL;
}

void Image::setScreen(Bitmap *scr) {
	deleteScreen();
	screen = scr;
	ownscreen = YAC_FALSE;
}

void Image::setScreenArea(Bitmap *scr, sS32 _ltx, sS32 _lty, sS32 _ltsx, sS32 _ltsy) {
	if(!scr)
		return;
	deleteScreen();
	Bitmap *nscreen = new(std::nothrow)Bitmap();
	if(!nscreen) 
	{
		yac_host->printf("setScreenArea: PANIC: cannot allocate screen bitmap.\n");
		return;
	}
	screen = nscreen;
	ownscreen = YAC_TRUE;
	sel.p.x = 0;
	sel.p.y = 0;
	sel.s.x = 0;
	sel.s.y = 0;
	
	if( scr->data.any  )
	{
		if( 
			( ((sU32)scr->s.x)>=( ((sU32)_ltx)+((sU32)_ltsx) ) )&&
			( ((sU32)scr->s.y)>=( ((sU32)_lty)+((sU32)_ltsy) ) )
			)
		{
			// ---- area fits into screen ----
			screen->owndata=0;
			screen->stride=scr->stride;//+(scr->s.x-_ltsx)*scr->z;
			screen->s.x=_ltsx;
			screen->s.y=_ltsy;
			screen->z=scr->z;
			screen->data.u8=scr->data.u8+_lty*scr->stride+_ltx*scr->z;
			sel.s.x=_ltsx;
			sel.s.x=_ltsy;
		}
	}
	
}

void Image::setColormap(Bitmap *cmap) {
	if(owncolormap&&colormap) delete colormap;
	colormap = cmap;
	owncolormap=0;
}

void Image::setAcceltabs(StaticList::Node *sc) {
	freeAcceltabs();
	ownacceltabs=0;
	acceltabs=sc;
}

sBool Image::allocScreen(tksvec2i size, sS32 depth) {
	deleteScreen();
	
	Bitmap *nscreen = new(std::nothrow)Bitmap();
	if(!nscreen) 
	{
		yac_host->printf("allocScreen: PANIC: cannot allocate screen bitmap.\n");
		//      throw new OutOfMemoryException();
		return 0;
	}
	
	if(!(nscreen->alloc(size,depth))) 
	{
		delete nscreen;
		//      throw new OutOfMemoryException();
		return 0;
	}
	
	screen=nscreen;
	ownscreen=1;
	type=Image::RAW;
	sel.p.x=0;
	sel.p.y=0;
	sel.s=size;
	return 1;
}

sBool Image::allocColormap(sS32 nsize, sS32 depth) {
	tksvec2i size;
	size.x=nsize;
	size.y=1;
	
	// Delete previous colormap
	deleteColormap();
	
	// Allocate new color bitmap
	Bitmap *ncolormap= new(std::nothrow)Bitmap();
	if(!ncolormap)
	{
		yac_host->printf("\n[...] Image::allocColormap: PANIC: cannot allocate color bitmap.\n");
		//      throw new OutOfMemoryException();
		return 0;
	}
	
	// Allocate colormap data array
	if(!(ncolormap->alloc(size,depth))) 
	{
		delete ncolormap;
		//      throw new OutOfMemoryException();
		return 0;
	}
	colormap=ncolormap;
	owncolormap=1;
	return 1;
	
}

void Image::clip(tksclipvec2i *c) {
	if(!screen)
		return;
	
	// ---- Check if pselection fits into screen
	// ---- adjust pselection (and qselection) if necessary 
	
	// ---- Psel. Left clipping
	if(c->ps.p.x<0) 
	{
		c->ps.s.x+=c->ps.p.x; /* shrink size */
		// Adjust qselection
		c->qs.p.x-=c->ps.p.x;
		c->ps.p.x=0;
	}
	// ---- Psel. Top clipping
	if(c->ps.p.y<0) 
	{
		c->ps.s.y+=c->ps.p.y; /* shrink size */
		// Adjust qselection
		c->qs.p.y-=c->ps.p.y;
		c->ps.p.y=0;
	}
	
	// ---- Psel. Right clipping
	if( (c->ps.p.x + c->ps.s.x) > screen->s.x ) 
	{
		c->ps.s.x-=(c->ps.p.x + c->ps.s.x)-screen->s.x;
	}
	
	// ---- Psel. Bottom clipping
	if( (c->ps.p.y + c->ps.s.y) > screen->s.y ) 
	{
		c->ps.s.y-=(c->ps.p.y + c->ps.s.y)-screen->s.y;
	}
	
	// Check if qselection fits into screen
	// adjust qselection (and pselection) if necessary 
	
	// ---- Qsel. Left clipping
	if(c->qs.p.x<0) 
	{
		c->qs.s.x+=c->qs.p.x; /* shrink size */
		// Adjust pselection
		c->ps.p.x-=c->qs.p.x;
		c->qs.p.x=0;
	}
	// ---- Qsel. Top clipping
	if(c->qs.p.y<0) 
	{
		c->qs.s.y+=c->qs.p.y; /* shrink size */
		// ---- Adjust pselection
		c->ps.p.y-=c->qs.p.y;
		c->qs.p.y=0;
	}
	
	// ---- Qsel. Right clipping
	if( (c->qs.p.x + c->qs.s.x) > screen->s.x ) 
	{
		c->qs.s.x-=(c->qs.p.x + c->qs.s.x)-screen->s.x;
	}
	
	// ---- Qsel. Bottom clipping
	if( (c->qs.p.y + c->qs.s.y) > screen->s.y ) 
	{
		c->qs.s.y-=(c->qs.p.y + c->qs.s.y)-screen->s.y;
	}
	
	// ---- Check for invalid values and set pselection width (c->ps.s.x) to zero if necessary
	if( c->ps.s.x<=0 || c->qs.s.x<=0 ) 
	{
		c->ps.s.x=0;
		return;
	}
	if ( c->ps.s.y<=0 || c->qs.s.y<=0 ) 
	{
		c->ps.s.x=0;
		return;
	}
}

sS8 *Image::getAcceltab(sS32 handle, sS32 size) {
	acceltab *t;
	StaticList::Node *c=acceltabs;
	StaticList::Node *p=0;
	
	if(!size)
		return 0;
	
	// Create list?
	if(!c) 
	{
		StaticList::CreateEmpty(&acceltabs);
		c=acceltabs;
		ownacceltabs=1;
	}
	
	while(c) 
	{
		if((t=tAcceltab(c->data))) 
		{
			if(t->handle==size)
				if(t->size==size) 
				{
					/// Found existing acceltab. return data 
					return t->data;
				}
		}
		p=c;
		c=c->next;
	}
	
	/// Allocate new acceltable 
	t = new(std::nothrow) acceltab;
	if(!t)
	{
		yac_host->printf("\n[---] Image::getAcceltab: cannot allocate acceleration table structure.\n"); 
		//throw new OutOfMemoryException();
		return (sS8 *)0;
	}
	t->data = new(std::nothrow) sS8[size];
	t->handle = handle;
	t->size = size;
	if(!t->data) 
	{
		yac_host->printf("\n[---] Image::getAcceltab: cannot allocate acceleration table.\n");
		delete t;
		//throw new OutOfMemoryException();
		return (sS8 *)NULL;
	}
	
	if(p)
		StaticList::Append(&p, (void*)t);
	else
		c->data=(void*)t;
	
	imgmemptr d;
	d.s8 = t->data;
	sS32 x;
	sS32 y;
	
	/// Generate table?
	switch(handle) 
	{
      case SCT_STENCIL8:
         // color 0 is transparent.
         for(x=0; x<256; x++) 
         {
            *d.u8++=x;
         }
         for(y=1; y<256; y++) 
         {
            *d.u8++=y;
         }
         break;

      case SCT_STENCIL16:
         // Color 0x7e0 is transparent
         for(x=0; x<0x7e0; x++)
         {
            *d.u16++=0xffff;
         }
         *d.u16++=0;
         for(x=0x7e1; x<65536; x++)
         {
            *d.u16++=0xffff;
         }
         break;

      case SCT_STENCIL32:
         // Alpha component 0 is transparent
         *d.u32++=0x00000000;
         for(x=1; x<256; x++)
         {
            *d.u32++=0x00ffffff;
         }
         break;

      case SCT_COLORKEY5:
         for (y=0; y<32; y++)
         {
            for(x=0; x<32; x++)
            {
               *d.u8++=(x==y) ? 1 : 0;
            }
         }
         break;
      case SCT_COLORKEY8_16_RGBA:
         for( y=0; y<256; y++)
         {
            for(x=0; x<256; x++)
            {
               *d.u16++=((x==y) ? 128 : 0);
            }
         }
         break;
      case SCT_FILLEDPOLYINITIALIZER:
         for(x=0; x<(size>>2); x++)
         {
            *d.u16++=65535; // min
            *d.u16++=0; // max
         }
         break;
	}
	return t->data;
}

void Image::freeAcceltabs(void) {
	sS32 i=0;
	if(ownacceltabs) 
	{
		StaticList::Node *c=acceltabs;
		acceltab *t=0;
		while(c) 
		{
			if( (t=tAcceltab(c->data)) ) 
			{
				delete [] t->data;
				delete t;
				i++;
			}
			c=c->next;
		}
		StaticList::Free(&acceltabs);
		ownacceltabs=0;
	}
	acceltabs=0;
	if(i) 
	{ 
		if(yac_host->yacGetDebugLevel()>1)
			yac_host->printf("[...] Image::freeAcceltabs: %i tables freed.\n", i); 
	}
}

void Image::scaleFrom(Image *other) {
	sS32 x_ds=sel.s.x;
	sS32 y_ds=sel.s.y;
	sS32 x,y,sy,ly;
	
	imgmemptr s;
	imgmemptr d;
	imgmemptr scm;
	mem    t;
	
	if(!other) 
	{
		yac_host->printf("\n[---] Image::scaleFrom: missing other image.\n");
		return;
	}
	if(!other->screen) 
	{
		yac_host->printf("\n[---] Image::scaleFrom: missing other->screen.\n");
		return;
	}
	if(!other->screen->data.any) 
	{
		yac_host->printf("\n[---] Image::scaleFrom: missing other->screen->data.\n");   
		return;
	}
	
	if(other->screen==screen)
		return;
	
	//      CLOG("screen->z is " 
	//           << (int) screen->z 
	//           << " other->screen->z is " 
	//           << (int) other->screen->z 
	//           << endl);
	
	sU32 addx=0x00000000;
	sU32 curx=0x00000000;
	addx=(sU32)(( (sF32) other->sel.s.x / (sF32) sel.s.x ) * 65536 );
   ///yac_host->printf("addx:%i sel.s.x=%i other->sel.s.x=%i\n", addx, sel.s.x, other->sel.s.x);
	
	sU32 cury;
	sU32 curx16;
	sU32 addy=0x00000000;
	addy=(sU32)(( (sF32) other->sel.s.y / (sF32) sel.s.y ) * 65536 );
   ///yac_host->printf("addy:%i sel.s.y=%i other->sel.s.y=%i\n", addy, sel.s.y, other->sel.s.y);
	
	switch(screen->z) 
	{
		// Destination color depth 8
      case 1:
         // Source color depth
         switch(other->screen->z) 
         {
            default:
               break;
            case 1: // Convert from 8 to 8 bit
               ly=0xDeadBeef;
               cury=0x00000000;
               for(y=0; y<y_ds; y++) 
               {
                  sy=(cury>>16);
                  s.u8= (sU8*) cAdrSelP(other,0,sy);
                  d.u8= (sU8*) cAdrSel(0,y);
                  cury+=addy;
                  if(ly!=sy || type==Image::VIDEO) 
                  {
                     // zoom <100%
                     curx=0x00000000;
                     for(x=0; x<x_ds; x++) 
                     {
                        *d.u8++= s.u8[(curx>>16)];
                        curx+=addx;
                     }
                     ly=sy;
                  }
                  else 
                  {
                     // zoom >100%
                     s.u8=(sU8*) cAdrSel(0,y-1);
                     ::memcpy( d.any, s.any, (size_t) (screen->z*x_ds) );
                  } 
               } 
               break;
            case 2: // Convert from 16 to 8 bit
               yac_host->printf("[...] Image::scaleFrom: 16->8 NOP.\n");
               break;
            case 3: // Convert from 24 to 8 bit
               ly=0xDeadBeef;
               cury=0x00000000;
               for(y=0; y<y_ds; y++) 
               {
                  sy=(cury>>16);
                  s.z24= (sPixel24 *) cAdrSelP(other,0,sy);
                  d.u8= (sU8 *) cAdrSel(0,y);
                  cury+=addy;
                  if(ly!=sy || type==Image::VIDEO) 
                  {
                     // zoom <100%
                     curx=0x00000000;
                     for(x=0; x<x_ds; x++) 
                     {
                        t.z24 = s.z24[(curx>>16)];
                        *d.u8++=(sU8)(t.z24.r*0.3+t.z24.g*0.5+t.z24.b*0.2);
                        curx+=addx;
                     } 
                     ly=sy;
                  }
                  else 
                  {
                     // zoom >100%
                     s.u8=(sU8 *) cAdrSel(0,y-1);
                     memcpy( d.any, s.any, (size_t) (screen->z*x_ds) );
                  }
               }
               break;
            case 4: // Convert from 32 to 8 bit
               ly=0xDeadBeef;
               cury=0x00000000;
               for(y=0; y<y_ds; y++) 
               {
                  sy=(cury>>16);
                  s.u32= (sU32 *) cAdrSelP(other,0,sy);
                  d.u8= (sU8 *) cAdrSel(0,y);
                  cury+=addy;
                  if(ly!=sy || type==Image::VIDEO) 
                  {
                     // zoom <100%
                     curx=0x00000000; //+ ((other->sel.s.x*0x10000-(addx*x_ds))>>1)*(y&1);
                     for(x=0; x<x_ds; x++) 
                     {
                        sU32 c=s.u32[(curx>>16)];
                        d.u8[x]=( ( (c>>16)&0xFF ) + ( (c>>8)&0xFF ) + (c&0xFF) )/3;
                        curx+=addx;
                     }
                     ly=sy;
                  }
                  else 
                  {
                     // zoom >100%
                     s.u8=(sU8 *) cAdrSel(0,y-1);
                     memcpy( d.any, s.any, (size_t) x_ds );
                  }
                  d.u8+=screen->stride;
               }
               //          yac_host->printf("\n[...] Image::scaleFrom: 32->8 NOP.\n");
               break;
         } 
         break;
      default:
         break;
      case 2: // Destination color depth 16
         switch(other->screen->z) 
         {
            default:
               break;

            case 1: // Convert from 8 to 16 bit
               if((other->colormap))
               {
                  if(other->colormap->z==2)
                  {
                     if( (scm.any=other->colormap->data.any) )
                     {
                        // Apply colormap
                        ly=0xDeadBeef;
                        cury=0x00000000;
                        for(y=0; y<y_ds; y++) 
                        {
                           sy=(cury>>16);
                           s.u16= (sU16 *) cAdrSelP(other,0,sy);
                           d.u16= (sU16 *) cAdrSel(0,y);
                           cury+=addy;
                           if(ly!=sy || type==Image::VIDEO) 
                           {
                              // zoom <100%
                              curx=0x00000000;
                              for(x=0; x<x_ds; x++) 
                              {
                                 *d.u16++= scm.u16[(s.u8[(curx>>16)])];
                                 curx+=addx;
                              } 
                              ly=sy;
                           }
                           else 
                           {
                              // zoom >100%
                              s.u16=(sU16 *) cAdrSel(0,y-1);
                              memcpy( d.any, s.any, (size_t) (screen->z*x_ds) );
                           }
                        }
                     }
                  }
               }
               break;

            case 2: // Convert from 16 to 16 bit
               ly=0xDeadBeef;
               cury=0x00000000;
               for(y=0; y<y_ds; y++) 
               {
                  sy=(cury>>16);
                  s.u16= (sU16 *) cAdrSelP(other,0,sy);
                  d.u16= (sU16 *) cAdrSel(0,y);
                  cury+=addy;
                  if(ly!=sy || type==Image::VIDEO) 
                  {
                     // zoom <100%
                     curx=0x00000000;
                     for(x=0; x<x_ds; x++) 
                     {
                        *d.u16++= s.u16[(curx>>16)];
                        curx+=addx;
                     }
                     ly=sy;
                  }
                  else 
                  {
                     // zoom >100%
                     s.u16=(sU16 *) cAdrSel(0,y-1);
                     memcpy( d.any, s.any, (size_t) (screen->z*x_ds) );
                  } 
               } 
               break;

            case 3: // Convert from 24 to 16 bit
               ly=0xDeadBeef;
               cury=0x00000000;
               for(y=0; y<y_ds; y++) 
               {
                  sy=(cury>>16);
                  s.z24= (sPixel24 *) cAdrSelP(other,0,sy);
                  d.u16= (sU16 *) cAdrSel(0,y);
                  cury+=addy;
                  if(ly!=sy || type==Image::VIDEO) 
                  {
                     // zoom <100%
                     curx=0x00000000; //+ ((other->sel.s.x*0x10000-(addx*x_ds))>>1)*(y&1);
                     for(x=0; x<x_ds; x++) 
                     {
                        curx16=(curx>>16);
                        *d.u16++=cCol38216( s.z24[curx16].r,  
                                           s.z24[curx16].g, 
                                           s.z24[curx16].b );
                        curx+=addx;
                     }
                     ly=sy;
                  }
                  else 
                  {
                     // zoom >100%
                     s.u16=(sU16 *) cAdrSel(0,y-1);
                     memcpy( d.any, s.any, (size_t) (screen->z*x_ds) );
                  } 
               } 
               break;

            case 4: // Convert from 32 to 16 bit
               ly=0xDeadBeef;
               cury=0x00000000;
               for(y=0; y<y_ds; y++) 
               {
                  sy=(cury>>16);
                  s.u32= (sU32 *) cAdrSelP(other,0,sy);
                  d.u16= (sU16 *) cAdrSel(0,y);
                  cury+=addy;
                  if(ly!=sy || type==Image::VIDEO) 
                  {
                     // zoom <100%
                     curx=0x00000000; //+ ((other->sel.s.x*0x10000-(addx*x_ds))>>1)*(y&1);
                     for(x=0; x<x_ds; x++) 
                     {
/*
          1111100000000000
            11100000000000
  111111110000000000000000

          11111100000
     1111111100000000

                11111
             11111111
 */
                        sU32 c=s.u32[(curx>>16)];
#if 0
                        sU16 c16 = cCol32216(c);
                        sU32 cRev = cCol16232(c16);
                        if(x==500){ Dyac_host_printf("xxx c32=#%08x rgb565=0x%02x cRev=#%08x\n", c, c16, cRev);}
#endif
                        *d.u16++=cCol32216(c);
                        curx+=addx;
                     }
                     ly=sy;
                  }
                  else 
                  {
                     // zoom >100%
                     s.u16=(sU16 *) cAdrSel(0,y-1);
                     memcpy( d.any, s.any, (size_t) (screen->z*x_ds) );
                  } 
               }
               break;
         } // switch(other->screen->z)..
         break;

      case 3: // Destination color depth 24
         switch(other->screen->z) 
         {
            default:
               break;
            case 1: // Convert from 8 to 24 bit
               //          yac_host->printf("[...] Image::scaleFrom: 8->24 NOP.\n");
               ly=0xDeadBeef;
               cury=0x00000000;
               if(colormap&&colormap->data.any&&(colormap->z==4))
               {
                  sU32 *cm=colormap->data.u32;
                  for(y=0; y<y_ds; y++) 
                  {
                     sy=(cury>>16);
                     s.u8= (sU8 *) cAdrSelP(other,0,sy);
                     d.z24= (sPixel24 *) cAdrSel(0,y);
                     cury+=addy;
                     if(ly!=sy || type==Image::VIDEO) 
                     {
                        // zoom <100%
                        curx=0x00000000; //+ ((other->sel.s.x*0x10000-(addx*x_ds))>>1)*(y&1);
                        for(x=0; x<x_ds; x++) 
                        {
                           sU32 c32=cm[s.u8[(curx>>16)]];
                           d.z24->r=(c32>>16)&0xFF; // swap rgb to bgr!
                           d.z24->g=(c32>> 8)&0xFF;
                           d.z24->b=(c32    )&0xFF;
                           d.z24++;
                           curx+=addx;
                        }
                        ly=sy;
                     }
                     else 
                     {
                        // zoom >100%
                        s.u32=(sU32 *) cAdrSel(0,y-1);
                        memcpy( d.any, s.any, (size_t) (screen->z*x_ds) );
                     } 
                  }
               }
               else
               {
                  for(y=0; y<y_ds; y++) 
                  {
                     sy=(cury>>16);
                     s.u8= (sU8 *) cAdrSelP(other,0,sy);
                     d.z24= (sPixel24 *) cAdrSel(0,y);
                     cury+=addy;
                     if(ly!=sy || type==Image::VIDEO) 
                     {
                        // zoom <100%
                        curx=0x00000000; //+ ((other->sel.s.x*0x10000-(addx*x_ds))>>1)*(y&1);
                        for(x=0; x<x_ds; x++) 
                        {
                           sU8 c8=s.u8[(curx>>16)];
                           d.z24->r=d.z24->g=d.z24->b=c8;
                           d.z24++;
                           curx+=addx;
                        }
                        ly=sy;
                     }
                     else 
                     {
                        // zoom >100%
                        s.u32=(sU32 *) cAdrSel(0,y-1);
                        memcpy( d.any, s.any, (size_t) (screen->z*x_ds) );
                     }
                     d.u8+=screen->stride;
                  }
               }
               break;

            case 2:
               // Convert from 16 to 24 bit
               yac_host->printf("[...] Image::scaleFrom: 16->24 NOP.\n");
               break;

            case 3:
               // Convert from 24 to 24 bit
               ly=0xDeadBeef;
               cury=0x00000000;
               for(y=0; y<y_ds; y++) 
               {
                  sy=(cury>>16);
                  s.z24= (sPixel24 *) cAdrSelP(other,0,sy);
                  d.z24= (sPixel24 *) cAdrSel(0,y);
                  cury+=addy;
                  if(ly!=sy || type==Image::VIDEO) 
                  {
                     // zoom <100%
                     curx=0x00000000; //+ ((other->sel.s.x*0x10000-(addx*x_ds))>>1)*(y&1);
                     for(x=0; x<x_ds; x++) 
                     {
                        *d.z24++=s.z24[(curx>>16)];  
                        curx+=addx;
                     }
                     ly=sy;
                  }
                  else 
                  {
                     // zoom >100%
                     s.z24=(sPixel24 *) cAdrSel(0,y-1);
                     memcpy( d.any, s.any, (size_t) (screen->z*x_ds) );
                  }
               } 
               break;

            case 4: // Convert from 32 to 24 bit
               ly=0xDeadBeef;
               cury=0x00000000;
               for(y=0; y<y_ds; y++) 
               {
                  sy=(cury>>16);
                  s.u32= (sUI *) cAdrSelP(other,0,sy);
                  d.z24= (sPixel24 *) cAdrSel(0,y);
                  cury+=addy;
                  if(ly!=sy || type==Image::VIDEO) 
                  {
                     // zoom <100%
                     curx=0x00000000; //+ ((other->sel.s.x*0x10000-(addx*x_ds))>>1)*(y&1);
                     for(x=0; x<x_ds; x++) 
                     {
                        sUI t = s.u32[(curx>>16)];
                        d.z24->r = (t>>16)&255;
                        d.z24->g = (t>> 8)&255;
                        d.z24->b = (t    )&255;
                        d.z24++;
                        curx+=addx;
                     }
                     ly=sy;
                  }
                  else 
                  {
                     // zoom >100%
                     s.z24=(sPixel24 *) cAdrSel(0,y-1);
                     memcpy( d.any, s.any, (size_t) (screen->z*x_ds) );
                  }
               } 
               break;
         }
         break;

      case 4: // Destination color depth 32
         switch(other->screen->z) 
         {
            default:
               break;

            case 1: // Convert from 8 to 32 bit
               ly=0xDeadBeef;
               cury=0x00000000;
               if(other->colormap&&other->colormap->data.any&&(other->colormap->z==4))
               {
                  sU32 *cm=other->colormap->data.u32;
                  for(y=0; y<y_ds; y++) 
                  {
                     sy=(cury>>16);
                     s.u8= (sU8 *) cAdrSelP(other,0,sy);
                     d.u32= (sU32 *) cAdrSel(0,y);
                     cury+=addy;
                     if(ly!=sy || type==Image::VIDEO) 
                     {
                        // zoom <100%
                        curx=0x00000000; //+ ((other->sel.s.x*0x10000-(addx*x_ds))>>1)*(y&1);
                        for(x=0; x<x_ds; x++) 
                        {
                           *d.u32++=cm[s.u8[(curx>>16)]]|0xFF000000; //(c<<24)|(c<<16)|(c<<8)|c;//(c<<16)|(c<<8)|c;
                           curx+=addx;
                        }
                        ly=sy;
                     }
                     else 
                     {
                        // zoom >100%
                        s.u32=(sU32 *) cAdrSel(0,y-1);
                        memcpy( d.any, s.any, (size_t) (screen->z*x_ds) );
                     } 
                  }
               }
               else
               {
                  for(y=0; y<y_ds; y++) 
                  {
                     sy=(cury>>16);
                     s.u8= (sU8 *) cAdrSelP(other,0,sy);
                     d.u32= (sU32 *) cAdrSel(0,y);
                     cury+=addy;
                     if(ly!=sy || type==Image::VIDEO) 
                     {
                        // zoom <100%
                        curx=0x00000000; //+ ((other->sel.s.x*0x10000-(addx*x_ds))>>1)*(y&1);
                        for(x=0; x<x_ds; x++) 
                        {
                           sU8 c=s.u8[(curx>>16)];
                           //d.u32[x]=(c<<24)|0xFFFFFF; //(c<<24)|(c<<16)|(c<<8)|c;//(c<<16)|(c<<8)|c;
                           d.u32[x]=(c<<24)|(c<<16)|(c<<8)|c;//(c<<16)|(c<<8)|c;
                           curx+=addx;
                        }
                        ly=sy;
                     }
                     else 
                     {
                        // zoom >100%
                        s.u32=(sU32 *) cAdrSel(0,y-1);
                        memcpy( d.any, s.any, (size_t) (screen->z*x_ds) );
                     }
                     d.u8+=screen->stride;
                  }
               }
               break;
		   
            case 2: // Destination color depth 32
               // yac_host->printf("[...] Image::scaleFrom: 16->32 NOP.\n");
               ly = 0xDeadBeef;
               cury = 0x00000000;
               for(y = 0; y < y_ds; y++) 
               {
                  sy = cury >> 16;
                  s.u16= (sU16 *) cAdrSelP(other,0,sy);
                  d.u32= (sU32 *) cAdrSel(0,y);
                  cury += addy;
                  if(ly!=sy || type==Image::VIDEO) 
                  {
                     // zoom <100%
                     curx = 0x00000000; //+ ((other->sel.s.x*0x10000-(addx*x_ds))>>1)*(y&1);
                     for(x = 0; x < x_ds; x++) 
                     {
                        curx16 = curx >> 16;
                        *d.u32++ = cCol16232( s.u16[curx16] );
                        curx += addx;
                     } 
                     ly = sy;
                  }
                  else 
                  {
                     // zoom >100%
                     s.u32 = (sU32 *) cAdrSel(0,y-1);
                     ::memcpy( d.any, s.any, (size_t) (screen->z*x_ds) );
                  }
               }
               break;

            case 3: // Convert from 24 to 32 bit
               //sy=0;
               ly=0xDeadBeef;
               cury=0x00000000;
               for(y=0; y<y_ds; y++) 
               {
                  sy=(cury>>16);
                  s.z24= (sPixel24 *) cAdrSelP(other,0,sy);
                  d.u32= (sU32 *) cAdrSel(0,y);
                  cury+=addy;
                  if(ly!=sy || type==Image::VIDEO) 
                  {
                     // zoom <100%
                     curx=0x00000000; //+ ((other->sel.s.x*0x10000-(addx*x_ds))>>1)*(y&1);
                     for(x=0; x<x_ds; x++) 
                     {
                        curx16=(curx>>16);
                        *d.u32++=cCol38232( s.z24[curx16].r,  
                                           s.z24[curx16].g, 
                                           s.z24[curx16].b,
                                           255 );
                        curx+=addx;
                     } 
                     ly=sy;
                  }
                  else 
                  {
                     // zoom >100%
                     s.u32=(sU32 *) cAdrSel(0,y-1);
                     memcpy( d.any, s.any, (size_t) (screen->z*x_ds) );
                  }
               }
               break;

            case 4: // Convert from 32 to 32 bit
               //dcon.printf("[...] Image::scaleFrom: 32->32 NOP.\n");
               sy=0;
               ly=0xDeadBeef;
               cury=0x00000000;
               for(y=0; y<y_ds; y++) 
               {
                  sy=(cury>>16);
                  s.u32= (sU32 *) cAdrSelP(other,0,sy);
                  d.u32= (sU32 *) cAdrSel(0,y);
                  cury+=addy;
                  if(ly!=sy || type==Image::VIDEO) 
                  {
                     // zoom <100%
                     curx=0x00000000; //+ ((other->sel.s.x*0x10000-(addx*x_ds))>>1)*(y&1);
                     for(x=0; x<x_ds; x++) 
                     {
                        *d.u32++=s.u32[(curx>>16)];
                        curx+=addx;
                     }
                     ly=sy;
                  }
                  else 
                  {
                     // zoom >100%
                     s.u32=(sU32 *) cAdrSel(0,y-1);
                     memcpy( d.any, s.any, (size_t) (screen->z*x_ds) );
                  } 
               }
               break;
         }
         break;
   }
}


#ifndef DX_GIF
sBool Image::loadGIF(sChar *, sS32) {
   if(yac_host->yacGetDebugLevel()>1)
   {
      yac_host->printf("[...] Image::load: gif loader not available.\n");
   }
	return 0;
}
#else
sBool Image::loadGIF(sChar *fname, sS32 cmapz) {
	size2di igeo;
	
	GifFileType *fh=DGifOpenFileName(fname);
	if(!fh) 
	{
		yac_host->printf("[---] Image::loadGIF: error opening \"%s\"\n", fname);
		return 0;
	}
	
	GifRecordType rec;
	if(DGifGetRecordType(fh, &rec)==GIF_ERROR) 
	{
		yac_host->printf("[---] Image::loadGIF: error DGifGetRecordType().\n");
		DGifCloseFile(fh);
		return 0;
	}
	
	if(DGifGetImageDesc(fh)==GIF_ERROR) 
	{
		yac_host->printf("[---] Image::loadGIF: error DGifGetImageDesc().\n");
		DGifCloseFile(fh);
		return 0;
	}
	
	igeo.x=fh->SWidth;
	igeo.y=fh->SHeight;
	
	//  CLOG("load: GIF geometry = " << igeo.x << "x" << igeo.y << endl);
	
	if(!allocScreen(igeo, 1)) 
	{
		DGifCloseFile(fh);
		return 0;
	}
	if(!allocColormap(256, cmapz)) 
	{
		DGifCloseFile(fh);
		return 0;
	}
	
	/// read in image 
	sS32 y;
	y=0;
	while(y<screen->s.y) 
	{
		if((DGifGetLine(fh, &screen->data.u8[y*screen->stride], screen->stride)==GIF_ERROR)) 
		{
			yac_host->printf("[---] Image::load: error DGifGetLine() (line=" << y << ").\n");
			break;
		}
		y++;
	}
	
	ColorMapObject *gcmap;
	//  printf("interlace is %i\n", fh->Image.Interlace);
	if(!(gcmap=fh->Image.ColorMap)) /* check for local colormap */
		gcmap=fh->SColorMap; /* otherwise take screen colormap */
	if(gcmap) 
	{
		yac_host->printf("\n[...] Image::loadGIF: sx:%i sy:%i z:%i colors:%i\n",
			fh->SWidth,
			fh->SHeight,
			(fh->SColorResolution>>3),
			gcmap->ColorCount);
		//CLOG("load: gif sx: "
		//<< fh->SWidth
		//<< " sy: "
		//<< fh->SHeight
		//<< " z: "
		//<< (fh->SColorResolution >>3)
		//<< " colors: "
		//<< gcmap->ColorCount
		//<< "\n"
		//);
		
		imgmemptr d=colormap->data;
		switch(cmapz) 
		{
		case 2:
			for(y=0; y<gcmap->ColorCount; y++)
				*d.u16++=cCol38216( gcmap->Colors[y].Red,
				gcmap->Colors[y].Green,
				gcmap->Colors[y].Blue );
			break;
		case 4:
			for(y=0; y<gcmap->ColorCount; y++)
				*d.u32++=cCol38232( gcmap->Colors[y].Red,
				gcmap->Colors[y].Green,
				gcmap->Colors[y].Blue,
				255 );
			break;
		default:
			yac_host->printf("[---] Image::load: unknown cmap depth. (cmapz)\n");
			break;
		}
	} 
	else 
	{
		yac_host->printf("[---] Image::loadGIF: warning: no colormap information found.\n");
	}
	DGifCloseFile(fh);
	return TRUE;
}
#endif





#ifndef DX_JPEG
sBool Image::loadJPEG(sU8 _open_mode, sChar *, sS32 ) {
   if(yac_host->yacGetDebugLevel()>1)
   {
      yac_host->printf("[...] Image::loadJPEG: jpeg loader not available.\n");
   }
	return 0;
}

#else
struct _jpg_error_mgr {
	struct jpeg_error_mgr pub;
	jmp_buf setjmp_buffer;
};

void _jpg_error_exit (j_common_ptr cinfo) {
	/* cinfo->err really points to a my_error_mgr struct, so coerce pointer */
	struct _jpg_error_mgr *myerr = (struct _jpg_error_mgr *) cinfo->err;
	
	/* Always display the message. */
	/* We could postpone this until after returning, if we chose. */
	(*cinfo->err->output_message) (cinfo);
	
	//jpeg_sio.close(); // ??
	
	/* Return control to the setjmp point */
	longjmp(myerr->setjmp_buffer, 1);
	
}

sBool Image::loadJPEG(sU8 _open_mode, sChar *fname, sS32 cmapz) {
	struct jpeg_decompress_struct cinfo;
	struct _jpg_error_mgr jerr;
	//FILE *fh;
	JSAMPARRAY buffer;
	sS32 jpg_bpl;
	sU8 *jimg= (sU8 *) 0;
	sS32 i;
	
	StreamIO jpeg_sio(tkscript);
	
	sBool ioret=0;    
	if(_open_mode)
	{
		ioret=jpeg_sio.openLocal(fname, 0);
	}
	else
	{
		ioret=jpeg_sio.openLogic(fname);
	}
	if(!ioret)
	{
		yac_host->printf("[---] Image::loadJPEG: error opening file \"%s\"\n", fname );
		return 0;
	}
	
	cinfo.err = jpeg_std_error(&jerr.pub);
	jerr.pub.error_exit = _jpg_error_exit;
	
	if (setjmp(jerr.setjmp_buffer)) 
	{
    /* If we get here, the JPEG code has signaled an error.
	* We need to clean up the JPEG object, close the input file, and return.
		*/
		//jpeg_destroy_decompress(&cinfo);
		jpeg_sio.close();
		yac_host->printf("[---] Image::loadJPEG: error reading JPEG.\n");
		return 0;
	}
	
	jpeg_create_decompress(&cinfo);
	jpeg_stdio_src( &cinfo, &jpeg_sio/*, fh*/);
	jpeg_read_header(&cinfo, 1);
	jpeg_start_decompress(&cinfo);
	jpg_bpl=cinfo.output_width * cinfo.output_components;
	
	if(yac_host->yacGetDebugLevel()>1)
		yac_host->printf("[...] Image::loadJPEG: sx:%i sy:%i z:%i colors:%i\n",
		cinfo.output_width,
		cinfo.output_height,
		cinfo.out_color_components,
		cinfo.desired_number_of_colors);
	//CLOG("load: jpg sx:"
	//     << cinfo.output_width
	//     << " sy:"
	//     << cinfo.output_height
	//     << " z:"
	//     << cinfo.out_color_components
	//     << " colors:"
	//     << cinfo.desired_number_of_colors
	//     << " \n";
	//     );
	
	size2di igeo;
	igeo.x=cinfo.output_width;
	igeo.y=cinfo.output_height;
	
	if(!allocScreen(igeo, (sS32) cinfo.output_components)) 
	{
		yac_host->printf("[---] Image::loadJPEG: error allocating screen.\n");
		jpeg_finish_decompress(&cinfo);
		jpeg_destroy_decompress(&cinfo);
		jpeg_sio.close();
		//delete jpeg_sio;
		return 0;
	}    
	
	jimg=screen->data.u8;
	buffer = (*cinfo.mem->alloc_sarray)
		((j_common_ptr) &cinfo, JPOOL_IMAGE, jpg_bpl, 1);
	
	sU8 *scanl= jimg;
	while(cinfo.output_scanline < cinfo.output_height) 
	{
		jpeg_read_scanlines(&cinfo, buffer, 1);
		memcpy( (void *) scanl, buffer[0], jpg_bpl); 
		scanl+=jpg_bpl;
	}
	
	/// on little endian systems we fix from rgb to bgr image data 
	/// 32 bit (+_a_lpha #) images get fixed from argb to bgra 
	
	imgmemptr d;
	switch(screen->z) 
	{
    case 1:
		/// assume grayscale 
		if(!allocColormap(256,cmapz)) 
		{
			yac_host->printf("[---] Image::loadJPEG: cannot allocate grayscale colormap\n");
			jpeg_finish_decompress(&cinfo);
			jpeg_destroy_decompress(&cinfo);
			//delete jpeg_sio;
			jpeg_sio.close();
			return 0;
		} 
		d=colormap->data;
		switch(cmapz) 
		{
        default:
			break;
        case 2:
			for(i=0; i<cinfo.desired_number_of_colors; i++)
				*d.u16++=cCol38216(i,i,i);
			break;
        case 4:
			for(i=0; i<cinfo.desired_number_of_colors; i++)
				*d.u32++=cCol38232(i,i,i,255);
			break;
		}
		break;
#ifdef YAC_LITTLE_ENDIAN
		// swap byte order
		case 3:
			break;
		case 4:
			break;
#endif
		default:
			// unknown depth!
			break;
	} 
	
	jpeg_finish_decompress(&cinfo);
	jpeg_destroy_decompress(&cinfo);
	//delete jpeg_sio;
	jpeg_sio.close();
	if(yac_host->yacGetDebugLevel()>1)
		yac_host->printf("[+++] Image::loadJPEG: \"%s\" loaded.\n", fname);
	return 1;
}

#endif


#ifndef DX_PNG
sBool Image::loadPNG(sU8 _open_mode, sChar *, sS32) {
    if(yac_host->yacGetDebugLevel()>1)
        yac_host->printf("[---] Image::loadPNG: png loader not available.\n");
	return 0;
}
sBool Image::savePNG(sChar *) {
    yac_host->printf("[---] Image::savePNG: png saver not available.\n");
    return 0;
}
#else
extern "C" {
}

//static StreamIO png_sio;

static void png_user_read_fn(png_structp _png_ptr, png_bytep _buffer, png_size_t _length) {
    YAC_Object *png_sio=(YAC_Object *)png_get_io_ptr(_png_ptr);
    if(png_sio)
    {
        sS32 r = (sS32) png_sio->yacStreamRead(_buffer, (sUI)_length);
        if(r<0)
        {
            png_error(_png_ptr, "user_read_fn() failed.\n");
        }
    }
}

sBool Image::loadPNGStream(YAC_Object *png_sio, sS32 cmapz) {
	sU8 buf[8];

	// ---- investigate png header
	if(8 != png_sio->yacStreamRead(buf, 8))
	{
		yac_host->printf("[---] Image::loadPNGStream: error reading png signature from file (short read)\n");
		return YAC_FALSE;
	}
	
	if(png_sig_cmp(buf, 0, 8)) 
	{ 
		yac_host->printf("[---] Image::loadPNGStream: not a png file.\n");
		return YAC_FALSE;
	}
	
	// Allocate png structures
	// png_structp png_ptr = png_create_read_struct(
	//				       PNG_LIBPNG_VER_STRING,
	//				       (png_voidp) /* user_error_ptr */NULL ,
	//				       /*user_error_fn*/NULL,
	//				       /*user_warning_fn*/NULL
	//				       );
	png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0);
	if(NULL == png_ptr)
	{
		yac_host->printf("[---] Image::loadPNG: error png_create_read_struct.\n");
		return YAC_FALSE;
	}
	
	png_infop info_ptr = png_create_info_struct(png_ptr);
	if(NULL == info_ptr)
	{
		yac_host->printf("[---] Image::loadPNG: error png_create_info_struct.\n");
		png_destroy_read_struct(&png_ptr, (png_infopp)NULL, (png_infopp)NULL);
		return YAC_FALSE;
	}
	
	png_infop end_info = png_create_info_struct(png_ptr);
	if(NULL == end_info)
	{
		yac_host->printf("[---] Image::loadPNG: error end_info png_create_info_struct.\n");
		png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
		return YAC_FALSE;
	}
	
	// Set longjmp error handler
	
	//    if (setjmp(png_ptr->jmpbuf))
	//      {
	//        png_destroy_read_struct(&png_ptr, &info_ptr,
	//  			      &end_info);
	//        fclose(fp);
	//        return;
	//      }
	
	// 8 o'clock, time to get busy..
	//png_init_io(png_ptr, fh);
	png_set_read_fn(png_ptr, (void*)png_sio/*user_struct*/, png_user_read_fn);
	
	png_set_sig_bytes(png_ptr, 8);
	png_read_info(png_ptr, info_ptr);
	
	png_uint_32 isx;
	png_uint_32 isy;
	sS32 ibit_depth;
	sS32 icolor_type;
	sS32 ifilter_type;
	sS32 icompression_type;
	sS32 iinterlace_type;
	sS32 irowbytes;
	sS32 iz;
	
	png_colorp ipalette;
	ipalette=0;
	sS32 inum_palette;
	
	png_set_strip_16(png_ptr); 
	
	png_get_IHDR(png_ptr,
	       info_ptr,
		   &isx,
		   &isy,
		   &ibit_depth, 
		   &icolor_type, 
		   &iinterlace_type,
		   &icompression_type,
		   &ifilter_type
		   );
	
	// Make sure that we always have 8bit/color component
	switch(ibit_depth) 
	{
      case 8:
         break;

      case 16:
         // We do not want 16bit/color component
         png_set_strip_16(png_ptr);
         break;

      default:
         // We want at least 8bit
         png_set_packing(png_ptr);
         break;
	}

   // yac_host->printf("icolor_type=%d  PNG_COLOR_TYPE_PALETTE=%d\n", icolor_type, PNG_COLOR_TYPE_PALETTE);
   // if(PNG_COLOR_TYPE_PALETTE == icolor_type)  // [26Nov2021] fix numbers-80x9.png
   // {
   //    png_set_expand(png_ptr);
   // }
	
	if (icolor_type == PNG_COLOR_TYPE_GRAY && ibit_depth < 8) 
		png_set_expand(png_ptr);
	
	//    if (icolor_type == PNG_COLOR_TYPE_PALETTE &&
	//        ibit_depth <= 8) png_set_expand(png_ptr);
	
	sBool strip_ch2 = YAC_FALSE; // for 2 channel grayscale images written by GIMP (win32) in grayscale mode..
	
	// Find out color model (1,2,3,4 bytes/pixel)
	switch(icolor_type) 
	{
      default:
      case PNG_COLOR_TYPE_PALETTE:
         iz = 1;
         png_get_PLTE(png_ptr, info_ptr, &ipalette, &inum_palette);
         break;

      case PNG_COLOR_TYPE_GRAY:
      case PNG_COLOR_TYPE_GRAY_ALPHA:
#ifdef YAC_WIN32
         if(info_ptr->channels==2) strip_ch2=1;
#else
         if(2 == png_get_channels(png_ptr, info_ptr)) 
            strip_ch2 = 1;
#endif // YAC_WIN32
         iz = 1;
         break;

         /*
           case PNG_COLOR_TYPE_GRAY_ALPHA:
           iz=2;
           break;
         */

      case PNG_COLOR_TYPE_RGB:
         iz = 3;
         break;

      case PNG_COLOR_TYPE_RGB_ALPHA:
         iz = 4;
         break;
	}
	
	irowbytes = (sS32)png_get_rowbytes(png_ptr, info_ptr);
	
#ifdef YAC_LITTLE_ENDIAN
    //if((icolor_type==PNG_COLOR_TYPE_RGB_ALPHA)||(icolor_type==PNG_COLOR_TYPE_RGB))
   if(PNG_COLOR_TYPE_RGB_ALPHA == icolor_type)
   {
      //png_set_packswap(png_ptr);
      png_set_bgr(png_ptr);
   }
#endif
	
	//  #ifdef DX_LITTLE_ENDIAN
	//    // Swap byte order to little endian
	//    if (ibit_depth == 16)
	//      png_set_swap(png_ptr);
	//  #endif
	
   if(yac_host->yacGetDebugLevel() > 1)
   {
      yac_host->printf("[...] Image::loadPNG: sx:%i sy:%i z:%i\n", isx, isy, iz);
   }
	//CLOG("load: png sx:" 
	//     << isx 
	//     << " sy:" 
	//     << isy 
	//     << " z:" 
	//     << iz 
	////         << " colors:" 
	////         << (iz==1 ? inum_palette : (iz==2) ? " hicolor " : " truecolor" )
	//     << "\n" 
	//     );
	
	// Allocate screen
	
	tksvec2i igeo;
	igeo.x=isx;
	igeo.y=isy;
	
	if(!allocScreen(igeo, iz)) 
	{
		yac_host->printf("[---] Image::loadPNG: cannot allocate screen (%ix%ix%i) for png image.\n", igeo.x, igeo.y, iz);
		png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
		return YAC_FALSE;
	}
	
	// Load image data
	//png_read_update_info(png_ptr, info_ptr);
	png_start_read_image(png_ptr);
	sS32 i;
	if(strip_ch2)
	{
		// ---- transform 2x8bit (GIMP grayscale image..) to 1x8bit ----
		// ---- read into temporary line buffer and transform channels ----
		sU8 *tb;
		tb = new(std::nothrow) sU8[irowbytes];
		if(tb)
		{
			yacmemptr d = screen->data;
			for(i = 0; i < (sS32) isy; i++)
			{
				png_read_row(png_ptr, tb, NULL);
				sU16 x = 0u;
				do
				{
					d.u8[x>>1] = tb[x];
				} while((x+=2)<irowbytes);
				d.u8 += screen->stride;
			}
			delete [] tb;
		}
	}
	else
	{
		// ---- regular loader ----
		if(screen->stride >= irowbytes) // probably always true..
		{
			yacmemptr d = screen->data;
			for(i = 0; i < (sS32) isy; i++)
			{
				png_read_row(png_ptr, d.u8+screen->stride*i, NULL);
			}
		}
	}
	
	// Init colormap
	if(iz==1 && (ipalette) && cmapz ) 
	{
		if(!allocColormap(256, cmapz)) 
		{
			yac_host->printf("[---] Image::loadPNG: cannot allocate colormap for png image.\n");
			png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
			return YAC_FALSE;
		}
		
		yacmemptr d = colormap->data;
		if(yac_host->yacGetDebugLevel()>1)
			yac_host->printf("[...] Image::loadPNG: using palette, index format=%s\n", (cmapz==4)?"32bit_ARGB":"16bit_565");

		switch(cmapz) 
		{
		case 2:
			for(i = 0; i < inum_palette; i++)
				*d.u16++ = cCol38216(ipalette[i].red,
                                 ipalette[i].green,
                                 ipalette[i].blue
                                 );
			break;

		case 4:
			for(i = 0; i < inum_palette; i++)
				*d.u32++ = cCol38232(ipalette[i].red,
                                 ipalette[i].green,
                                 ipalette[i].blue,
                                 255
                                 );
			break;

		default:
			yac_host->printf("[---] Image::loadPNG: unknown cmap depth cmapz=%i.\n", cmapz);
			break;
		}
	}
	
	// Clean up..
	//  png_read_end(png_ptr, end_info);
	png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
	return YAC_TRUE;
}

sBool Image::loadPNG(sU8 _open_mode, sChar *fname, sS32 cmapz) {
	YAC_Object *png_sio = yac_host->yacNewByID(YAC_CLID_PAKFILE);
	sBool ioret = YAC_FALSE;

	switch(_open_mode)
	{
      case 0:
         ioret = png_sio->yacStreamOpenLogic(fname);
         // yac_host->printf("xxx ioret=%d fname=\"%s\"\n", ioret, fname);
         break;

      case 1:
         ioret = png_sio->yacStreamOpenLocal(fname, 0);
         break;
	}

	if(!ioret)
	{
      yac_host->printf("[---] Image::loadPNG: file \"%s\" not found. open_mode=%d\n", fname, _open_mode);
      yac_host->yacDelete(png_sio);
		return YAC_FALSE;
	}

   sBool ret = loadPNGStream(png_sio, cmapz);
   if(!ret)
   {
      yac_host->printf("[---] Image::loadPNG: failed to load file \"%s\". open_mode=%d\n", fname, _open_mode);
   }

	yac_host->yacDelete(png_sio);

   return ret;
}
	
static void png_user_write_fn(png_structp _png_ptr, png_bytep _buffer, png_size_t _length) {
   YAC_Object *png_sio=(YAC_Object*)png_get_io_ptr(_png_ptr);
   if(png_sio)
   {
      sS32 r = (sS32)png_sio->yacStreamWrite(_buffer, sUI(_length));
      if(r<0)
      {
         png_error(_png_ptr, "user_write_fn() failed.\n");
      }
   }
}

sBool Image::savePNGStream(YAC_Object *png_sio) {
   if(!screen || !screen->data.any || !screen->s.x || !screen->s.y || screen->z==2)
      return YAC_FALSE;

   png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0);
   if(NULL == png_ptr)
   {
      return YAC_FALSE;
   }

   png_infop info_ptr = png_create_info_struct(png_ptr);
   if(NULL == info_ptr)
   {
      png_destroy_write_struct(&png_ptr, (png_infopp)0);
      return YAC_FALSE;
   }

   //png_init_io(png_ptr, fp);
   png_set_write_fn(png_ptr, (png_voidp)png_sio, png_user_write_fn, 0);
   // init compressor
   png_set_filter(png_ptr, 0, PNG_FILTER_NONE);
   png_set_compression_level(png_ptr, Z_BEST_COMPRESSION);

   // determine color type
   int color_type;
   switch(screen->z)
   {
      case 4:
         color_type = PNG_COLOR_TYPE_RGB_ALPHA;
         break;

      case 3:
         color_type = PNG_COLOR_TYPE_RGB;
         break;

      case 1:
         //if(colormap)
         //    color_type=PNG_COLOR_TYPE_PALETTE;
         //else
         color_type = PNG_COLOR_TYPE_GRAY;
         break;

      default:
         // invalid colordepth
         png_destroy_write_struct(&png_ptr, &info_ptr);
         return YAC_FALSE;
   }

   // set header
   png_set_IHDR(png_ptr,
                info_ptr,
                screen->s.x, screen->s.y, 
                8, color_type,
                PNG_INTERLACE_NONE,
                PNG_COMPRESSION_TYPE_DEFAULT,
                PNG_FILTER_TYPE_DEFAULT
                );

   if(PNG_COLOR_TYPE_PALETTE == color_type)
   {
      /// xxx construct 8bit palette
      /// png_set_bKGD(png_ptr, info_ptr, bgcol)
   }

   // set remark
   png_text rem;
   rem.compression = PNG_TEXT_COMPRESSION_NONE;
   rem.key = (char*) "Software";
   // Description
   // Copyright
   // Creation Time
   // Disclaimer
   // Warning
   // Source
   // Comment
   rem.text = (char*) "created by TKS";
   png_set_text(png_ptr, info_ptr, &rem, 1);
	
   png_set_oFFs(png_ptr, info_ptr, 0, 0, PNG_OFFSET_PIXEL);
	
   png_write_info(png_ptr, info_ptr);
	
#ifdef YAC_LITTLE_ENDIAN
   //if((color_type==PNG_COLOR_TYPE_RGB_ALPHA)||(color_type==PNG_COLOR_TYPE_RGB))
   //if((color_type==PNG_COLOR_TYPE_RGB_ALPHA))
   if(PNG_COLOR_TYPE_RGB_ALPHA == color_type)
   {
      //png_set_packswap(png_ptr);
      png_set_bgr(png_ptr);
   }
#endif

   // setup row pointers
   sU8 **row_pointers = new(std::nothrow) sU8*[screen->s.y];
   int i;
   imgmemptr s;
   s.u8=screen->data.u8;
   for(i = 0; i < screen->s.y; i++)
   {
      row_pointers[i] = s.u8;
      s.u8 += screen->stride;
   }
   // write rows
   png_write_rows(png_ptr, row_pointers, screen->s.y);
   png_write_end(png_ptr, 0);

   // delete row pointers
   delete [] row_pointers;

   // free structs
   png_destroy_write_struct(&png_ptr, &info_ptr);
   
	return YAC_TRUE;
}

sBool Image::savePNG(sChar *_fname) {
   if(!screen || !screen->data.any || !screen->s.x || !screen->s.y || screen->z==2)
      return YAC_FALSE;

   YAC_Object *png_sio = yac_host->yacNewByID(YAC_CLID_FILE);
	if(!png_sio->yacStreamOpenLocal(_fname, 1))
      return YAC_FALSE;

   sBool ret = savePNGStream(png_sio);

   yac_host->yacDelete(png_sio);

   return ret;
}

#endif



#ifndef DX_TIFF
sBool Image::loadTIFF(sChar *, sS32) {
   if(yac_host->yacGetDebugLevel()>1)
   {
      yac_host->printf("[---] Image::loadTIFF: tiff loader not available.\n");
   }
	return 0;
}
#else
extern "C" {
}
sBool Image::loadTIFF(sChar *fname, sS32) {
	// Open file
	TIFF* fh = TIFFOpen(fname, "r");
	if(fh) 
	{
		sU32 isx;
		sU32 isy;
		
		if(!TIFFGetField(fh, TIFFTAG_IMAGEWIDTH, &isx))
		{
			yac_host->printf("[---] Image::loadTIFF: error getting TIFFTAG_IMAGEWIDTH\n");
			TIFFClose(fh);
			return 0;
		}
		
		if(!TIFFGetField(fh, TIFFTAG_IMAGELENGTH, &isy))
		{
			yac_host->printf("[---] Image::loadTIFF: error getting TIFFTAG_IMAGELENGTH\n");
			TIFFClose(fh);
			return 0;
		}
		
		size2di igeo;
		igeo.x=isx;
		igeo.y=isy;
		
		if(!allocScreen(igeo, 4)) 
		{
			yac_host->printf("[---] Image::loadTIFF cannot allocate screen (%ix%ix32) for TIFF image.\n", igeo.x, igeo.y);
			TIFFClose(fh);
			return 0;
		}
		
		if (!TIFFReadRGBAImage(fh, isx, isy, (sUI *)screen->data.u32,  0)) 
		{
			yac_host->printf("[---] Image::loadTIFF: error loading TIFF image data.\n");
			TIFFClose(fh);
			return 0;
		}
		
		// Swap BGRA to ARGB, flip picture vertically
#define swaporder(a) (((0xff000000&a))|((0x00ff0000&a)>>16)|((0x0000ff00&a))|((0x000000ff&a)<<16))
		imgmemptr s;
		imgmemptr d;
		sS32 x,y;
		for(y=0; y<screen->s.y/2; y++)
		{
			s.u8=cAdrSel(0, screen->s.y-y-1);
			d.u8=cAdrSel(0, y);
			for(x=0; x<screen->s.x; x++)
			{
				sU32 cs=s.u32[x];
				sU32 cd=d.u32[x];
				s.u32[x]=swaporder(cd);
				d.u32[x]=swaporder(cs);
			}
		}
		yac_host->printf("[...] Image::loadTIFF: \"%s\" loaded.\n[...]                   sx:%i sy:%i z:32 \n", igeo.x, igeo.y);
	
		TIFFClose(fh);
		return 1;
	}
	yac_host->printf("[---] Image::loadTIFF: error opening TIFF(?) file \"%s\"\n", fname);
	return 0;
}

#endif


sBool Image::load(sU8 _open_mode, sChar *fname, sS32 screenz, sS32 cmapz, sS32 newx, sS32 newy) {
	if(!cmapz)
		cmapz=4;
	// Guess file type and try that one first
	sChar *s=fname;
	sChar sf[8];
	sS32 i=0;
	sChar a;
	while((*s++))
		i++;
	if(i>255)
		// Oops..
		return 0;
	for(;(i>0 && *--s!='.'); i--);
	for(i=0; i<7 && (a=*s++); i++)  
		sf[i]=( (a>='A'&&a<='Z') ? (a|32) : a );
	sf[i]=0;
	
	sBool loaded = 0;

	if(!strcmp(sf,".jpg"))
		loaded = loadJPEG(_open_mode, fname, cmapz);

   if(!loaded && !strcmp(sf,".jpeg"))
      loaded = loadJPEG(_open_mode, fname, cmapz);

   if(!loaded && !strcmp(sf,".png"))
      loaded = loadPNG(_open_mode, fname, cmapz);

   if(!loaded && !strcmp(sf,".gif"))
      loaded = loadGIF(fname, cmapz);

   if(!loaded && !strcmp(sf,".tif"))
      loaded = loadTIFF(fname, cmapz);

   if(!loaded && !strcmp(sf,".tiff"))
      loaded = loadTIFF(fname, cmapz);

   if(!loaded)
      loaded = loadJPEG(_open_mode, fname, cmapz);

   if(!loaded)
      loaded = loadPNG(_open_mode, fname,cmapz);

   if(!loaded)
      loaded = loadGIF(fname,cmapz);

   if(!loaded)
      loaded = loadTIFF(fname,cmapz);

   if(!loaded)
   {
      yac_host->printf("[---] Image::load: unrecognized image file \"%s\"\n", fname);
      return 0;
   } 

   if(screenz)
   {
      if( (screen->z!=screenz)||((newx)&&(newx!=screen->s.x))
          ||((newy)&&(newy!=screen->s.y)) ) 
      {
         Image tmp;
         //tmp.tkscript=tkscript;
         //if(newx==screen->s.x&&newy==screen->s.y&&screenz==screen->z)
         //    return 1;
         if(newx)
            tmp.sel.s.x=newx;
         else
            tmp.sel.s.x=screen->s.x;
         if(newy)
            tmp.sel.s.y=newy;
         else
            tmp.sel.s.y=screen->s.y;
         tmp.allocScreen(tmp.sel.s, screenz);
         if(yac_host->yacGetDebugLevel()>1)
            yac_host->printf("[...] Image::load: (convert image) sx:%i sy:%i z:%i\n",
                             tmp.sel.s.x, tmp.sel.s.y, screenz );
         //CLOG("GenericImageLoader: \"" << fname << "\" ( "
         //     << tmp.sel.s.x
         //     << "x" << tmp.sel.s.y
         //     << " z:" << screenz
         //     << " ) \n");
         tmp.setColormap(colormap);
         tmp.owncolormap=0;
         tmp.scaleFrom(this);
         setScreen(tmp.screen);
         ownscreen=1;
         tmp.ownscreen=0;
      }
   }
   return 1;
}

sBool Image::loadStream(YAC_Object *_ifs, sS32 screenz, sS32 cmapz, sS32 newx, sS32 newy) {
	if(0 == cmapz)
		cmapz = 4;

	// Guess file type and try that one first
   //  (todo) look at image header
   //  (note) assume it's a PNG (for now)
	
	sBool loaded = YAC_FALSE;

   loaded = loadPNGStream(_ifs, cmapz);

   if(!loaded)
   {
      yac_host->printf("[---] Image::loadStream: unrecognized image file\n");
      return YAC_FALSE;
   } 

   // Scale/color-convert
   if(screenz)
   {
      if( (screen->z != screenz)            || 
          ((newx) && (newx != screen->s.x)) ||
          ((newy) && (newy != screen->s.y))
          ) 
      {
         Image tmp;
         //tmp.tkscript=tkscript;
         //if(newx==screen->s.x&&newy==screen->s.y&&screenz==screen->z)
         //    return 1;
         if(newx)
            tmp.sel.s.x = newx;
         else
            tmp.sel.s.x = screen->s.x;
         if(newy)
            tmp.sel.s.y = newy;
         else
            tmp.sel.s.y = screen->s.y;
         tmp.allocScreen(tmp.sel.s, screenz);
         if(yac_host->yacGetDebugLevel()>1)
            yac_host->printf("[...] Image::load: (convert image) sx:%i sy:%i z:%i\n",
                             tmp.sel.s.x, tmp.sel.s.y, screenz );
         //CLOG("GenericImageLoader: \"" << fname << "\" ( "
         //     << tmp.sel.s.x
         //     << "x" << tmp.sel.s.y
         //     << " z:" << screenz
         //     << " ) \n");
         tmp.setColormap(colormap);
         tmp.owncolormap = YAC_FALSE;
         tmp.scaleFrom(this);
         setScreen(tmp.screen);
         ownscreen = YAC_TRUE;
         tmp.ownscreen = YAC_FALSE;
      }
   }
   return YAC_TRUE;
}

void Image::clearScreen(void) {
/*if(screen)
if(screen->data.any)
{
sS32 x,y;
imgmemptr d=screen->data;
for(y=0; y<sel.s.y; y++)
{
for(x=0; x<sel.s.x*screen->z; x++)
{
d.s8[x]=0;
}
d.s8+=screen->stride;
}
}*/
	if(screen)
		if(screen->data.any)
		{
			memset(screen->data.any, 0, sel.s.y*screen->stride);
		}
} 

sF32 Image::countPixels(sU32 _color_key) {
	sF32 ret=0.0;
	sU32 pxcount=0;
	sU32 total=sel.s.x*sel.s.y;
	if(total)
	{
      if(screen && screen->data.any)
      {
         sS32 x,y;
         imgmemptr d; d.any=screen->data.any;
         for(y=0; y<sel.s.y; y++)
         {
            switch(screen->z)
            {
					case 1:
						for(x=0; x<sel.s.x; x++)
						{
							pxcount+=(d.u8[x]==(sU8)(_color_key))?1:0;
						}
						break;
					case 2:
						for(x=0; x<sel.s.x; x++)
						{
							pxcount+=(d.u16[x]==(sU16)(_color_key))?1:0;
						}
						break;
					case 3:
						for(x=0; x<sel.s.x; x++)
						{ 
							if(d.z24[x].r==((sU8)(_color_key>>16)) &&
								d.z24[x].g==((sU8)(_color_key>>8)) &&
								d.z24[x].b==((sU8)_color_key) )
								pxcount++;
						} 
						break;
					case 4:
						for(x=0; x<sel.s.x; x++)
						{
							pxcount+=(d.u32[x]==(sU32)(_color_key))?1:0;
						}
						break;
            }
            d.s8+=screen->stride;
         }

         if(pxcount)
         { 
            ret=((sF32)pxcount)/((sF32)total);
         }
      } 
	} 
	return ret;
}


void Image::setPseudoColormap(Image *_renderbuffer, Bitmap *_hwcmap) {
	if(_renderbuffer && _hwcmap && screen ) 
	{
		if(_renderbuffer->screen)
		{
			if(screen->z==1)
				setColormap(_hwcmap);
			else
				_renderbuffer->setColormap(_hwcmap);
		}
	}
}

sBool Image::colorKeyToAlpha(sU32 _c32) {
	sU8 cr,cg,cb;
	cr=(_c32>>16)&0xFF;
	cg=(_c32>> 8)&0xFF;
	cb=(_c32    )&0xFF;
	if(screen)
	{
		switch(screen->z)
		{
		case 3:
			{
				Bitmap *newscr = new(std::nothrow)Bitmap();
				if(newscr)
				{
					if(newscr->alloc(screen->s.x, screen->s.y, 4))
					{
						imgmemptr s;
						s.any=screen->data.any;
						imgmemptr d;
						d.any=newscr->data.any;
						sU16 x,y;
						for(y=0; y<screen->s.y; y++)
						{
							for(x=0; x<screen->s.x; x++)
							{
								sU8 r,g,b;
								r=s.u8[((x<<1)+x)+0];
								g=s.u8[((x<<1)+x)+1];
								b=s.u8[((x<<1)+x)+2];
								if(r==cr&&g==cg&&b==cb)
								{
									d.u32[x]=0;
								}
								else
								{
									d.u32[x]=(0xFF<<24)|(r<<16)|(g<<8)|b;
								}
							}
							s.u8+=screen->stride;
							d.u8+=newscr->stride;
						}
						deleteScreen();
						screen=newscr;
						ownscreen=1;
						return 1;
					}
				}
			}
			break;
		case 4:
			{
				imgmemptr s;
				s.any=screen->data.any;
				sU16 x,y;
				for(y=0; y<screen->s.y; y++)
				{
					for(x=0; x<screen->s.x; x++)
					{
						sU8 r,g,b;
						r=(s.u32[x]>>16)&0xFF;
						g=(s.u32[x]>> 8)&0xFF;
						b=(s.u32[x]    )&0xFF;
						if(r==cr&&g==cg&&b==cb)
						{
							s.u32[x]=0x00000000;
						}
						else
						{
							s.u32[x]=(0xFF<<24)|(r<<16)|(g<<8)|b;
						}
					}
					s.u8+=screen->stride;
				}
				return 1;
			}
			break;
		}
	}
	return 0;
}

void Image::interleaveAlpha(Bitmap *_b) {
	if(_b&&(_b->z==1))
	{
		if(screen)
		{
			if(
				(screen->s.x==_b->s.x)&&
				(screen->s.y==_b->s.y)
				)
			{
				switch(screen->z)
				{
				case 4:
					{
						imgmemptr s;
						s.any=_b->data.any;
						imgmemptr d;
						d.any=screen->data.any;
						sU16 x,y;
						for(y=0; y<screen->s.y; y++)
						{
							for(x=0; x<screen->s.x; x++)
							{
								d.u32[x]=(d.u32[x]&0x00FFFFFF)|(s.u8[x]<<24);
							}
							d.u8+=screen->stride;
							s.u8+=_b->stride;
						}
					}
					break;
				default:
					yac_host->printf("\n[---] Image::interleaveAlpha: unsupported source depth.\n");
					break;
				}
			}
		}
	}
}

sBool Image::grayscale(void) {
	Bitmap *newscr;
	if(screen)
	{
		switch(screen->z)
		{
		case 3:
			newscr = new(std::nothrow)Bitmap();
			if(newscr)
			{
				if(newscr->alloc(screen->s.x, screen->s.y, 1))
				{
					imgmemptr s;
					s.any=screen->data.any;
					imgmemptr d;
					d.any=newscr->data.any;
					sU16 x,y;
					for(y=0; y<screen->s.y; y++)
					{
						for(x=0; x<screen->s.x; x++)
						{
							sU8 r,g,b;
							r=(s.u8[((x<<1)+x)+0]    )&0xFF;
							g=(s.u8[((x<<1)+x)+1]    )&0xFF;
							b=(s.u8[((x<<1)+x)+2]    )&0xFF;
							d.u8[x]=(sU8)(r*0.3+g*0.6+b*0.1);
						}
						s.u8+=screen->stride;
						d.u8+=newscr->stride;
					}
					deleteScreen();
					screen=newscr;
					ownscreen=1;
					return 1;
				}
			}
			else
			{
				yac_host->printf("\n[---] Image::grayscale: failed to allocate new screen.\n");
			}
			break;
		case 4:
			newscr = new(std::nothrow)Bitmap();
			if(newscr)
			{
				if(newscr->alloc(screen->s.x, screen->s.y, 1))
				{
					imgmemptr s;
					s.any=screen->data.any;
					imgmemptr d;
					d.any=newscr->data.any;
					sU16 x,y;
					for(y=0; y<screen->s.y; y++)
					{
						for(x=0; x<screen->s.x; x++)
						{
							sU8 r,g,b;
							sU32 c32=s.u32[x];
							r=(c32>>16)&0xFF;
							g=(c32>> 8)&0xFF;
							b=(c32    )&0xFF;
							d.u8[x]=(sU8)(r*0.3+g*0.6+b*0.1);
						}
						s.u8+=screen->stride;
						d.u8+=newscr->stride;
					}
					deleteScreen();
					screen=newscr;
					ownscreen=1;
					return 1;
				}
			}
			else
			{
				yac_host->printf("\n[---] Image::grayscale: failed to allocate new screen.\n");
			}
			break;
		default:
			yac_host->printf("\n[---] Image::grayscale: unsupported source depth.\n");
			break;
		}
	}
	return 0;
}

void Image::copyRegion(Image *_src, sS32 _px, sS32 _py, sS32 _sx, sS32 _sy, sS32 _dx, sS32 _dy) {
// 	yac_host->printf("copyRegion(_src=%08x, px=%i, py=%i, sx=%i, sy=%i, dx=%i, dy=%i\n",
//                     (sUI)_src, _px, _py, _sx, _sy, _dx, _dy);

	if(_src && _px >=0 && _py >=0)
   {
		if(_src->screen&&screen)
      {
			if(_src->screen->z==screen->z)
         {
				if(_src->screen->data.any&&screen->data.any)
            {
               if(_dx < 0)
               {
                  _sx += _dx;
                  _px -= _dx;
                  _dx = 0;
               }

               if(_dy < 0)
               {
                  _sy += _dy;
                  _py -= _dy;
                  _dy = 0;
               }

					if(_px <  _src->screen->s.x && 
                  _py <  _src->screen->s.y &&
                  _sx > 0                  &&
                  _sy > 0                  &&
						_sx <= _src->screen->s.x &&
                  _sy <= _src->screen->s.y &&
						_dx <  screen->s.x       &&
                  _dy <  screen->s.y
                  )
					{

						if( (_px+_sx)>_src->screen->s.x ) // clamp right
							_sx=(_src->screen->s.x-_px);
						if( (_py+_sy)>_src->screen->s.y ) // clamp bottom
							_sy=(_src->screen->s.y-_py);
						if( (_dx+_sx)>screen->s.x ) // clamp d right
							_sx=(screen->s.x-_dx);
						if( (_dy+_sy)>screen->s.y ) // clamp d bottom
							_sy=(screen->s.y-_dy);
						
						imgmemptr s;
						s.u8=_src->screen->data.u8+_src->screen->stride*_py+
							_src->screen->z*_px;
						imgmemptr d;
						d.u8=screen->data.u8+screen->stride*_dy+
							_src->screen->z*_dx;
						
						sS32 x;
						sS32 y;
						switch(screen->z)
						{
						case 1: // 8 bit A
							for(y=0; y<_sy; y++)
							{
								for(x=0; x<_sx; x++)
									d.u8[x]=s.u8[x];
								s.u8+=_src->screen->stride;
								d.u8+=screen->stride;
							}
							break;
						case 2: // 16 bit 565 RGB
							break;
						case 3: // 24 bit RGB
							for(y=0; y<_sy; y++)
							{
                        sSI tsx = _sx*3;
								for(x=0; x<tsx; x+=3)
								{
									d.u8[x  ] = s.u8[x  ];
									d.u8[x+1] = s.u8[x+1];
									d.u8[x+2] = s.u8[x+2];
								}
								s.u8+=_src->screen->stride;
								d.u8+=screen->stride;
							}
							break;
						case 4: // 32 bit ARGB
							for(y=0; y<_sy; y++)
							{
								for(x=0; x<_sx; x++)
									d.u32[x]=s.u32[x];
								s.u8+=_src->screen->stride;
								d.u8+=screen->stride;
							}
							break;
						}
					}
            }
         }
      }
   }
}

void Image::drawLine(sS16 _px, sS16 _py, sS16 _qx, sS16 _qy, sU32 _c) {
    if(screen)
        if(screen->data.any)
            if(screen->s.x&&screen->s.y)
            {
                // 2d clipping
                /*
                sU8 csbitsp=
				( (_px<0)?1:0 ) |
				( (_px>=screen->s.x)?2:0 ) |
				( (_py<0)?4:0 ) |
				( (_py>=screen->s.y)?8:0 );
                sU8 csbitsq=
				( (_qx<0)?1:0 ) |
				( (_qx>=screen->s.x)?2:0 ) |
				( (_qy<0)?4:0 ) |
				( (_qy>=screen->s.y)?8:0 );
				*/
                if(_px<0||_px>=screen->s.x||_py<0||_py>=screen->s.y||
					_qx<0||_qx>=screen->s.x||_qy<0||_qy>=screen->s.y)
                    return; // only accept lines that are completely visible
				
                // prepare ~bresenham
                sS16 dx=_qx-_px;
                sS16 dy=_qy-_py;
                sS16 adx=(dx<0)?-dx:dx;
                sS16 ady=(dy<0)?-dy:dy;
                if(!adx)
                    adx=1; // catch divbyzero
                if(!ady)
                    ady=1; // catch divbyzero
                sS16 cadv;
                sU32 iadv;
                if(adx>ady)
                {
                    // constant advance x
                    double ax=(((double)ady)/((double)adx))*65536;
                    iadv=(sU32)ax;
                    cadv=(dx<0)?-1:1;
                    if(dy<0)
                    {
                        sS16 t; // swap coords
                        t=_px;
                        _px=_qx;
                        _qx=t;
                        t=_py;
                        _py=_qy;
                        _qy=t;
                        cadv=-cadv;
                    }
                    //cadv*=screen->z;
                }
                else
                {
                    // constant advance y
                    double ay=(((double)adx)/((double)ady))*65536;
                    iadv=(sU32)ay;
                    cadv=(dy<0)?-1:1;
                    if(dx<0)
                    {
                        sS16 t; // swap coords
                        t=_px;
                        _px=_qx;
                        _qx=t;
                        t=_py;
                        _py=_qy;
                        _qy=t;
                        cadv=-cadv;
                    }
                    cadv*=screen->s.x; // xxx assumes s.x*z==stride
                }
                imgmemptr d;
                d.u8=screen->data.u8+_py*screen->stride+screen->z*_px;
                sU32 count32=0;
                switch(screen->z)
                {
                case 1:
                    {
                        if(adx>ady)
                        {
                            // const advance x
                            for(; adx>=0; adx--)
                            {
                                d.u8[(count32>>16)*screen->stride]=_c;
                                d.u8+=cadv;
                                count32+=iadv;
                            }
                        }
                        else
                        {
                            // const advance y
                            for(; ady>=0; ady--)
                            {
                                d.u8[(count32>>16)]=_c;
                                d.u8+=cadv;
                                count32+=iadv;
                            }
                        }
                    }
                    break;
                case 2:
                    // line 16bit not implemented yet
                    break;
                case 3:
                    // line 24bit not implemented yet
                    break;
                case 4:
                    // line 32bit 
                    {
                        if(adx>ady)
                        {
                            // const advance x
                            for(; adx>=0; adx--)
                            {
                                d.u32[(count32>>16)*screen->s.x]=_c;
                                d.u32+=cadv;
                                count32+=iadv;
                            }
                        }
                        else
                        {
                            // const advance y
                            for(; ady>=0; ady--)
                            {
                                d.u32[(count32>>16)]=_c;
                                d.u32+=cadv;
                                count32+=iadv;
                            }
                        }
                    }
                    break;
                }
            }
}



void Image::drawFilledRect(sS16 _px, sS16 _py, sS16 _sx, sS16 _sy, sU32 _c) {
    if(screen)
        if(screen->data.any)
            if(screen->s.x&&screen->s.y)
            {
                //if(_px<0||_py<0||(_px+_sx)>=screen->s.x||(_py+_sy)>=screen->s.y)
                if(((sU16)(_px+_sx))>screen->s.x||((sU16)(_py+_sy))>screen->s.y)
                    return; // does not fit into buffer
                sU16 ix;
                sU16 iy;
                switch(screen->z)
                {
                case 1: // 8bits
                    {
                        imgmemptr d;
                        d.u8=screen->data.u8+screen->stride*_py+_px;
                        for(iy=0; iy<_sy; iy++)
                        {
                            for(ix=0; ix<_sx; ix++)
                            {
                                d.u8[ix]=_c;
                            }
                            d.u8+=screen->stride;
                        }
                    }
                    break;
                case 2: // 16bits
                    {
                        imgmemptr d;
                        d.u8=screen->data.u8+screen->stride*_py+(_px<<1);
                        for(iy=0; iy<_sy; iy++)
                        {
                            for(ix=0; ix<_sx; ix++)
                            {
                                d.u16[ix]=_c;
                            }
                            d.u8+=screen->stride;
                        }
                    }
                    break;
                case 3: // 24bits
                    {
                        imgmemptr d;
                        d.u8=screen->data.u8+screen->stride*_py+(_px*3);
                        sU8 r=(_c>>16)&0xFF;
                        sU8 g=(_c>> 8)&0xFF;
                        sU8 b=(_c    )&0xFF;
                        sU16 rix;
                        for(iy=0; iy<_sy; iy++)
                        {
                            rix=0;
                            for(ix=0; ix<_sx; ix++)
                            {
                                d.u8[rix++]=r;
                                d.u8[rix++]=g;
                                d.u8[rix++]=b;
                            }
                            d.u8+=screen->stride;
                        }
                    }
                    break;
                case 4: // 32bits
                    {
                        imgmemptr d;
                        d.u8=screen->data.u8+screen->stride*_py+_px*4;
                        for(iy=0; iy<_sy; iy++)
                        {
                            for(ix=0; ix<_sx; ix++)
                            {
                                d.u32[ix]=_c;
                            }
                            d.u8+=screen->stride;
                        }
                    }
                    break;
                }
            }
}

void Image::drawFilledConvexPoly(sU16 _numcoords, int *_coordpairs, sU32 _c) {
    if(!screen||!screen->data.any||!screen->s.x||!screen->s.y)
        return;
    /// xxx catch _numcoords==0
    imgmemptr poi;
    imgmemptr pot;
    poi.s8=getAcceltab(SCT_FILLEDPOLYINITIALIZER, (screen->s.y+4)*4);
    pot.s8=getAcceltab(SCT_FILLEDPOLY, (screen->s.y+4)*4);
    sU16 i;
    imgmemptr tpoi;
    tpoi.any=poi.any;
    imgmemptr tpot;
    tpot.any=pot.any;
    for(i=0; i<screen->s.y+4; i++) // init/reset poly outline list
        *tpot.u32++=*tpoi.u32++;
    sU16 lx=(sU16)*_coordpairs++;
    sU16 ly=(sU16)*_coordpairs++;
    sU16 topy=65535;
    if(ly<topy)
        topy=ly;
    _numcoords--;
    for(; _numcoords; _numcoords--)
    {
        sU16 tx=(sU16)*_coordpairs++;
        sU16 ty=(sU16)*_coordpairs++;
        sU16 nlx=tx;
        sU16 nly=ty;
        if(ly<topy) 
            topy=ly; // track top scanline
        // modified line algorithm, make one entry per scanline
        sS16 dx=tx-lx;
        sS16 dy=ty-ly;
        sS16 adx=(dx<0)?-dx:dx;
        sS16 ady=(dy<0)?-dy:dy;
        if(!ady)
            ady=1;
        double advx=((double)adx)/((double)ady)*65536.0;
        sU32 iadvx32=(sU32)advx;
        if(dx<0)
        {
            // swap coords
            sU16 t=tx;
            tx=lx;
            lx=t;
            t=ty;
            ty=ly;
            ly=t;
            dy=-dy;
        }
        sU32 countx32=lx<<16;
        sU16 cy=ly<<1; // min/max pairs, hence *2
		sU16 cx;
        if(dy<0)
        {
            // begin with bottom scanline
            for(; ady>/*=*/0; ady--)
            {
                cx=(countx32>>16);
                if(cx<pot.u16[cy])
                    pot.u16[cy]=cx; // new left border
                if(cx>pot.u16[cy+1])
                    pot.u16[cy+1]=cx; // new right border
                countx32+=iadvx32;
                cy-=2;
            }
        }
        else
        {
            // begin with top scanline
			for(; ady>0; ady--)
            {
                cx=(countx32>>16);
                if(cx<pot.u16[cy])
                    pot.u16[cy]=cx; // new left border
                if(cx>pot.u16[cy+1])
                    pot.u16[cy+1]=cx; // new right border
                countx32+=iadvx32;
                cy+=2;
            }
        }
        lx=nlx;
        ly=nly;
    }
    tpot.u16=&pot.u16[(topy<<1)];
    imgmemptr d;
    d.u8=screen->data.u8+screen->stride*topy;
    switch(screen->z)
    {
    case 1:
        while(*tpot.u16!=0xFFFF)
        {
            for(i=tpot.u16[0]; i<=tpot.u16[1]; i++)
                d.u8[i]=_c;
            tpot.u16+=2;
            d.u8+=screen->stride;
        }
        break;
    case 2:
        break;
    case 3:
        break;
    case 4:
        break;
    }
}

void Image::scroll(sS16 _dx, sS16 _dy) {
    if(screen&&screen->data.any)
    {
        sU8 xfl=_dx<0?1:_dx>0?2:0;
        sU8 yfl=_dy<0?4:_dy>0?8:0;
        sU8 scrmode=(yfl<<2)|xfl;
        imgmemptr s;
        imgmemptr d;
        switch(scrmode)
        {
        case 0: 
            // no scroll
            break;
        case 1: 
            // scroll left
            {
                s.u8=screen->data.u8-_dx*screen->z;
                d.u8=screen->data.u8;
                sU16 sx=screen->s.x+_dx;
                sU16 sy=screen->s.y;
                sU16 ix,iy;
                switch(screen->z)
                {
                case 1:
                    for(iy=0; iy<sy; iy++)
                    {
                        for(ix=0; ix<sx; ix++)
                        {
                            d.u8[ix]=s.u8[ix];
                        }
                        s.u8+=screen->stride;
                        d.u8+=screen->stride;
                    }
                    // fill empty area
                    sx=-_dx;
                    d.u8=screen->data.u8+(screen->s.x+_dx);
                    for(iy=0; iy<sy; iy++)
                    {
                        for(ix=0; ix<sx; ix++)
                        {
                            d.u8[ix]=0;
                        }
                        d.u8+=screen->stride;
                    }
                    break;
					
                case 4:
                    for(iy=0; iy<sy; iy++)
                    {
                        for(ix=0; ix<sx; ix++)
                        {
                            d.u32[ix]=s.u32[ix];
                        }
                        s.u8+=screen->stride;
                        d.u8+=screen->stride;
                    }
                    sx=-_dx;
                    d.u8=screen->data.u8+((screen->s.x+_dx)<<2);
                    for(iy=0; iy<sy; iy++)
                    {
                        for(ix=0; ix<sx; ix++)
                        {
                            d.u32[ix]=0;
                        }
                        d.u8+=screen->stride;
                    }
                    break;
                }
            }
            break;
        case 2:
            // scroll right
			yac_host->printf("scroll right NOP.\n");
            break;
        case 4:
            // scroll up
			yac_host->printf("scroll up NOP.\n");
            break;
        case 8:
            // scroll down
			yac_host->printf("scroll down NOP.\n");
            break;
        case 5:
            // scroll left and up
			yac_host->printf("scroll left and up NOP.\n");
            break;
        case 9:
            // scroll left and down
			yac_host->printf("scroll left and down NOP.\n");
            break;
        case 6:
            // scroll right and up
			yac_host->printf("scroll right and up NOP.\n");
            break;
        case 10:
            // scroll right and down
			yac_host->printf("scroll right and down NOP.\n");
            break;
        }
    } // if screen
}


sBool Image::flipY(void) {
	Bitmap *newscr;
	if(screen)
	{
      if(0 != screen->z)
      {
         newscr = new(std::nothrow)Bitmap();
         if(newscr)
         {
            if(newscr->alloc(screen->s.x, screen->s.y, screen->z))
            {
               imgmemptr s;
               s.any=screen->data.any;
               imgmemptr d;
               d.any=newscr->data.any;
               sU16 y;
               s.u8 += screen->stride * (screen->s.y-1);
               for(y=0; y<screen->s.y; y++)
               {
                  memcpy(d.any, s.any, screen->stride);
                  s.u8 -= screen->stride;
                  d.u8 += newscr->stride;
               }
               deleteScreen();
               screen=newscr;
               ownscreen = YAC_TRUE;
               return YAC_TRUE;
            }
            else
            {
               yac_host->printf("\n[---] Image::flipY: failed to allocate new bitmap data.\n");
            }
         }
         else
         {
            yac_host->printf("\n[---] Image::flipY: failed to allocate new bitmap.\n");
         }
      }
	}
	return YAC_FALSE;
}

void Image::invert(void) {
	if(screen)
	{
      if(0 != screen->z)
      {
         imgmemptr s; s.any = screen->data.any;
         sSI y = 0;
         switch(screen->z)
         {
            case 1:
               for(; y < screen->s.y; y++)
               {
                  for(sSI x = 0; x < screen->s.x; x++)
                  {
                     s.u8[x] = 255u - s.u8[x];
                  }
                  s.u8 += screen->stride;
               }
               break;

            case 2:
               for(; y < screen->s.y; y++)
               {
                  for(sSI x = 0; x < screen->s.x; x++)
                  {
                     sU16 rgb565 = s.u16[x];
                     sU16 r = 31u - ( (rgb565 >> 11) & 31u );
                     sU16 g = 31u - ( (rgb565 >>  5) & 63u );
                     sU16 b = 31u - ( (rgb565      ) & 31u );
                     s.u16[x] = (r << 11) | (g << 5) | b;
                  }
                  s.u8 += screen->stride;
               }
               break;

            case 3:
               for(; y < screen->s.y; y++)
               {
                  sUI j = 0u;
                  for(sSI x = 0; x < screen->s.x; x++)
                  {
                     s.u8[j + 0u] = 255u - s.u8[j + 0u];
                     s.u8[j + 1u] = 255u - s.u8[j + 1u];
                     s.u8[j + 2u] = 255u - s.u8[j + 2u];
                     j += 3u;
                  }
                  s.u8 += screen->stride;
               }
               break;

            case 4:
               for(; y < screen->s.y; y++)
               {
                  for(sSI x = 0; x < screen->s.x; x++)
                  {
                     s.u8[ (x << 2) + 0u ] = 255u - s.u8[ (x << 2) + 0u ];
                     s.u8[ (x << 2) + 1u ] = 255u - s.u8[ (x << 2) + 1u ];
                     s.u8[ (x << 2) + 2u ] = 255u - s.u8[ (x << 2) + 2u ];
                     s.u8[ (x << 2) + 3u ] = 255u - s.u8[ (x << 2) + 3u ];
                  }
                  s.u8 += screen->stride;
               }
               break;
         }
      }
	}
}

void Image::visitBuffer(sU8 *_data, sUI _w, sUI _h, sUI _bpp) {
   deleteScreen();

   Bitmap *nscreen = new(std::nothrow)Bitmap();
   nscreen->visitBuffer(_data, _w, _h, _bpp/*z*/);
}
