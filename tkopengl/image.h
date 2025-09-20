/// image.h
///
/// (c) 1998-2024 Bastian Spiegel <bs@tkscript.de> <bastian.spiegel@web.de> <fl1nk@gmx.de>, <mi7991@fh-wedel.de>
///     - Distributed under terms of the Lesser GNU General Public License (LGPL).
///       See COPYING and <http://www.gnu.org/licenses/licenses.html#LGPL> for further information.
///
///

#ifndef TKS_IMAGE_H__
#define TKS_IMAGE_H__

/// Base class for image handling. 
/// Uses bitmaps to access screen memory and colormap.
/// Uses a selection to create a view into the screen.
/// You can use some tool functions to clip images / setScreen and
/// setColormap. There are several macros available to easily calculate
/// the address of a pixel in the screen bitmap and to get a color from the
/// colormap bitmap.
/// Images always work with absolute positions in screen coordinate space:
/// Coordinates within images are NOT relative to their position.
/// The selection structure describes the position and size of an image
/// in a screen.
/// Never try to mess with any own* fields yourself - use the appropriate methods
/// for allocation and freeing screens and colormaps.
/// The Colormap::colormap variable is a valid parameter for <b>setColormap</b>;
/// You can use virtually any operator or renderer to create colormaps.
/// Please refer to the Colormap documentation for more information.
///
class Image : public YAC_Object {
  public:
/// Initialize selection2di with given values. 
#ifdef DX_CONSTRUCTOR_EXPRESSIONS
#define setsel2di(__set2dsel,a,b,c,d) __set2dsel=(selection2di) {{a,b},{c,d}}
#else
#define setsel2di(__set2dsel,a,b,c,d) __set2dsel.p.x=a; __set2dsel.p.y=b; __set2dsel.s.x=c; __set2dsel.s.y=d
#endif

/// Initialize size2d with given values. 
#ifdef DX_CONSTRUCTOR_EXPRESSIONS
#define setsize2di(__set2dsize,a,b) __set2dsize=(size2di) {a,b}
#else
#define setsize2di(__set2dsize,a,b) __set2dsize.x=a; __set2dsize.y=b
#endif

/// Initialize point2d with given values. 
#ifdef DX_CONSTRUCTOR_EXPRESSIONS
#define setpoint2di(__set2dpoint,a,b) __set2dpoint=(point2di) {a,b}
#else
#define setpoint2di(__set2dpoint,a,b) __set2dpoint.x=a; __set2dpoint.y=b
#endif

/// Calc adr of 8 bit pixel value in bitmap at given coordinates
/// @param a The horizontal pixel position
/// @param b The vertical pixel position
#define cAdr8(a,b) ((sChar *)&screen->data.u8[(a)+(b)*screen->stride])

/// Calc adr of 16 bit pixel value in bitmap at given coordinates
/// @param a The horizontal pixel position
/// @param b The vertical pixel position
#define cAdr16(a,b) ((sU16*)&screen->data.u8[((a)<<1)+(b)*screen->stride])

/// Calc adr of 24 bit pixel value in bitmap at given coordinates
/// @param a The horizontal pixel position
/// @param b The vertical pixel position
#define cAdr24(a,b) ((sPixel24*)&screen->data.u8[((a)*3)+(b)*screen->stride])

/// Calc adr of 32 bit pixel value in bitmap at given coordinates
/// @param a The horizontal pixel position
/// @param b The vertical pixel position
#define cAdr32(a,b) ((sU32*)&screen->data.u8[((a)<<2)+(b)*screen->stride])

/// Macro to convert image/selection coordinate space to graphics memory address.
/// Warning! The <b>screen</b> pointer must be set, otherwise you will encounter
/// a segmentation fault.
/// @param x The horizontal pixel position (relative to selection origin)
/// @param y The vertical pixel position (relative to selection origin)
#define cAdrSel(a,b) ( (a+sel.p.x)*screen->z + (b+sel.p.y)*screen->stride + screen->data.u8 )

/// Macro to convert other image/selection coordinate space to graphics memory address.
/// Warning! The other-&gt;<b>screen</b> pointer must be set, otherwise you will encounter
/// a segmentation fault.
/// @param o The other image.
/// @param x The horizontal pixel position (relative to selection origin)
/// @param y The vertical pixel position (relative to selection origin)
#define cAdrSelO(o,a,b) ( (a+(o)->sel.p.x)*(o)->screen->z + (b+(o)->sel.p.y)*(o)->screen->stride + (o)->screen->data.u8 )

/// Macro to convert image/selection coordinate space to graphics memory address.
/// This macro can be used if dealing with other images. (i.e. not used in own class method)
/// Warning! The <b>screen</b> pointer must be set, otherwise you will encounter
/// a segmentation fault.
/// @param x The horizontal pixel position (relative to selection origin)
/// @param y The vertical pixel position (relative to selection origin)
#define cAdrSelP(c,a,b) ( (a+(c)->sel.p.x)*(c)->screen->z + (b+(c)->sel.p.y)*(c)->screen->stride + (c)->screen->data.u8 )

/// Type enumeration for image categories. */
   enum _itypes {
      /// Virtual images represent a view into other images - the memory 
      /// area is of unknown type (which is the default).
      /// Memory occupied by VIRTUAL images will not be allocated/freed.
      VIRTUAL=0,
  
      /// Video images are located in video memory (not necessaribly in the visible area..)
      /// If you want accelerated image operations , this is the way to go.
      /// The Blitter class can generate VIDEO images.
      VIDEO=1,
 
      /// RAW images are located in main memory which is allocated/freed when image is deleted.
      /// The Blitter class can generate RAW images and will if it fails to create a VIDEO image
      ////
      RAW=2
   };

   /// Enum for acceleration tables
   enum _act {
      /// 8bit (color index 0=transparent) 
      SCT_STENCIL8,
      SCT_STENCIL16,
      SCT_STENCIL32,
      SCT_COLORKEY5,
      SCT_COLORKEY8,
      /// Color curves renderer 
      SCT_COLORKEY8_16_RGBA,
      SCT_FILLEDPOLY,            // size=#lines*4
      SCT_FILLEDPOLYINITIALIZER, // size=#lines*4

      PAD
   };

   /// A pointer to a bitmap that contains address and attributes 
   /// of the screen memory area you are about to access.
   /// Please change this ptr via setScreen() so that I can notify the change.
   /// Default is NULL
   Bitmap *screen;

   /// A pointer to a bitmap structure that contains address and attributes 
   /// of the colormap memory area.
   /// This is _NOT_ an abstract Colormap class but a simple bitmap that stores
   /// x color values with z bytes/value. The height (y) is always 1 for colormaps.
   /// Please change this ptr via setColormap() so that I can notify the change.
   /// Default is NULL
   Bitmap *colormap;

   /// foreground color. useful for more complex image operations.
   /// could be used as the default texture modulation color.
	struct {
		sU8  r,g,b,a;
	} fgcolor;

	/// If set true, the image class will handle the memory chunk 
   /// described by the screen bitmap. 
   /// Please DO NOT set manually.
   /// Default is FALSE
   sUI ownscreen;

   /// If set true, the image class will handle the memory chunk 
   /// described by colormap bitmap. 
   /// Please DO NOT set manually.
   /// Default is FALSE
   sUI owncolormap;

   /// If set true, the image class will handle the acceleration tables list.
   /// Please DO NOT set manually.
   /// Default is FALSE
   sUI ownacceltabs;

   /// Image type - see enum _itypes.
   /// Default is VIRTUAL
   sUI type;

   /// The selection describes a rectangular region within an image. 
   /// This is also the default work area for graphic operations that do not
   /// take an explicit selection argument (e.g. scaleFrom)
   /// The Gadget class uses this to locate a gadget in an existing window.
   /// The selection of a top-level window describes the position and size of
   /// the window on the desktop. 
   /// Setting the selection before calling _Window::create
   /// gives you a new top-level window with the position and size as 
   /// given in sel. 
   /// Coordinate space is then relative to that selection.
   ///
   /// Note that selections are NEVER relative to other selections, they
   /// are always relative to the toplevel image (screen), which is probably an OS window,
   /// the graphics card framebuffer or an offscreen framebuffer. <br>
   ///
   /// The following macros can be used to convert coordinates to memory pointers: <br>
   /// <b>cBitSel</b> -  Calc adr of 8 bit pixel in bitmap at given coordinates <br>
   /// <b>cAdr8</b> - Calc adr of an 8 bit pixel in image at given coordinates <br>
   /// <b>cAdr16</b> - Calc adr of a 16 bit pixel in image at given coordinates <br>
   /// <b>cAdr24</b> - Calc adr of a 24 bit pixel in image at given coordinates <br>
   /// <b>cAdr32</b> - Calc adr of a 32 bit pixel in image at given coordinates <br>
   /// <b>cAdrSel</b> - Convert image/selection coordinate space to graphics memory address <br>
   /// <b>cAdrSelO</b> - Convert other image/selection coordinate space to graphics memory address <br>
   ///
   /// The default selection is {0,0,0,0}
   tkssel2i sel;

   /// Current list of acceleration tabs. 
   /// Acceleration tabs can be used to install look-up tables that can be shared
   /// by several objects.
   /// Currently, a MAGIC number mechanism is used to get special (internal)
   /// acceleration tables.
   /// You can use the getAccelTab method in a more general way by passing
   /// a (0..MAX_INT) MAGIC number as the first argument and the desired
   /// byte-size of the acceleration tab as the second argument.
   /// The tables will be deleted
   /// when the object is destroyed (if THIS is the owner).
   StaticList::Node *acceltabs;

  public:
   Image(void);
   ~Image(void);
  
   sBool allocScreen(tksvec2i size, sS32 depth); 
   sBool allocColormap(sS32 size, sS32 depth);
   void  deleteScreen(void);
   void  deleteColormap(void);
   void  setScreen(Bitmap *screen);
   void  setScreenArea(Bitmap *screen, sS32 _ltx, sS32 _lty, sS32 _ltsx, sS32 _ltsy);
   void  setColormap(Bitmap *cmap);
   void  setPseudoColormap(Image *renderbuffer, Bitmap *hwcmap);
   void  setAcceltabs(StaticList::Node *sc);
   void  clip(tksclipvec2i *sel);

   /// Check if a given point lies within a given selection 
   /// @param s The selection 
   /// @param p The point to be tested
#define pointIn(sl,pt) ( (pt.x>=sl.p.x)&&(pt.x<=sl.p.x+sl.s.x) && \
                         (pt.y>=sl.p.y)&&(pt.y<=sl.p.y+sl.s.y) )

   sS8 * getAcceltab (sS32 handle, sS32 size);
   void  freeAcceltabs (void);

   sBool loadJPEG (sU8 _open_mode, sChar *name, sS32 cmapz);
   sBool loadPNGStream (YAC_Object *png_sio, sS32 cmapz);
   sBool loadPNG (sU8 _open_mode, sChar *name, sS32 cmapz);
   sBool loadTIFF (sChar *name, sS32 cmapz);
   sBool loadGIF (sChar *name, sS32 cmapz);
   sBool load (sU8 _open_mode, sChar *name, sS32 screenz=0, sS32 cmapz=0, sS32 newx=0, sS32 newy=0);
   sBool loadStream (YAC_Object *_ifs, sS32 screenz=0, sS32 cmapz=0, sS32 newx=0, sS32 newy=0);

   sBool savePNGStream (YAC_Object *png_io);
   sBool savePNG (sChar *name);

   void  scaleFrom (Image *other);
   void  clearScreen (void);
   sF32  countPixels (sU32 _color_key=0);
   sBool colorKeyToAlpha (sU32 _colorkey); 
   sBool grayscale (void);
   void  interleaveAlpha (Bitmap *);
   void  copyRegion (Image *_src, sS32 _px, sS32 _py, sS32 _sx, sS32 _sy, sS32 _dx, sS32 _dy);
   void  drawLine (sS16 _px, sS16 _py, sS16 _qx, sS16 _qy, sU32 _c);
   void  drawFilledRect (sS16 _px, sS16 _py, sS16 _sx, sS16 _sy, sU32 _c);
   void  drawFilledConvexPoly (sU16 _numcoords, int *_coordpairs, sU32 _c);
   void  scroll (sS16 _dx, sS16 _dy);
   sBool flipY( void);
   void  invert (void);
   void  visitBuffer (sU8 *_data, sUI _w, sUI _h, sUI _bpp);
};


#endif // TKS_IMAGE_H__
