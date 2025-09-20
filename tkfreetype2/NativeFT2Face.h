
#ifndef __NATIVEFREETYPE2_H__
#define __NATIVEFREETYPE2_H__


YG("freetype");


extern FT_Library ft_library;

/* @class NativeFT2Face,Object

Represents a freetype2 font face.

For an example on how to use this class, take a look at the "testgl.tks" example
script in the tkfreetype2 plugin directory.

The tkfreetype2 plugin is accompanied by a "freetype2.tks" support script which simplifies
font handling/rendering.

*/
YC class NativeFT2Face: public YAC_Object {
public:
   FT_Face ft_face;
   sF32    glyphs_advancex[256];
   
   sBool b_monochrome; // 0=b&w, 1=antialiased

public:
   YAC(NativeFT2Face);

   NativeFT2Face(void);

   ~NativeFT2Face();

// moved to script interface #define FONT_STYLE_ITALIC YCI 1
// moved to script interface #define FONT_STYLE_BOLD YCI 2

   void freeFace(void);
   sSI  openFace2(sF32 _w, sF32 _h);


   /* @method getHeight:float
     Return the actually selected font height.
   
    @return Font height
   */
   YM sF32 getHeight(void);


   /* @method getAscender:float
     Return the "minimal descender".
 
    @return Ascender
   */
   YM sF32 getAscender(void);


   /* @method getDescender:float
    Return the "maximal ascender".

    @return Descender
   */
   YM sF32 getDescender(void);


   /* @method getUnderlinePosition:float
     Return the y position of the underline line
    
    @return Underline position
   */
   YM sF32 getUnderlinePosition(void);


   /* @method getUnderlineThickness:float
     Return the thickness of the underline line

   @return Underline thickness
   */
   YM sF32 getUnderlineThickness(void);


   /* @method openMemoryFace,Buffer b,float w,float h:boolean
     Open a font face from the given memory buffer.

    @arg b Buffer that stores the compressed font data
    @arg w Char width
    @arg h Char height
    @return true(1) if the font has been opened, false(0) if an error occured.
   */
   YM sSI  openMemoryFace(YAC_Object *_buffer, sF32 _w, sF32 _h);


   /* @method openFileFace,String fileName,float w,float h:boolean 
     Open a font face from the given local file

    @arg fileName Font file name
    @arg w Char width
    @arg h Char height
    @return true(1) if the font has been opened, false(0) if an error occured.
   */
   YM sSI  openFileFace(YAC_Object *_filename, sF32 _w, sF32 _h);


   /* @method loadChar,Class ft2face,char c
     Load a char glyph and return its extends in "ft2face"

    @arg ft2face Receives the glyph extends (class must have float fields "width", "height", "offsetx", "offsety", "advancex" !)
    @arg c Which character to load
   */
   YM void loadChar(YAC_Object *_so_ft2face, sSI _char);


   /* @method renderGlyph,Object texture
     Render last loaded char glyph.

     The texture object must support the yacArrayGetStridE(), yacArrayGetPointer(), yacArrayGetWidth(), yacArrayGetHeight() calls.
     The bitmap data is written in 8bits per pixel format. If unsure, pass an instance of the §Texture class.

     @arg texture Receives the char bitmap data
   */
   YM void renderGlyph(YAC_Object *_texture);

   ////YM sF32 stringWidth(YAC_Object *_string);
   //Calculate the width (in pixels) for the given String.


   /* @method storeGlyphsAdvanceX,FloatArray fa
    Write the advancex for glyphs 0..255 to the given §FloatArray

    @arg fa Receives the "advancex" float values for glyphs 0..255. The array must be able to hold at least 256 elements.
   */
   YM void storeGlyphsAdvanceX(YAC_Object *_floatArray);


   /* @method storeFamilyName,String s
    Write the font family name to the given string.

    @arg s Receives the font family name 
   */
   YM void storeFamilyName(YAC_Object *_string);


   /* @method storeStyleName,String s
    Write the font style name to the given string.

    @arg s Receives the font style name
   */
   YM void storeStyleName(YAC_Object *_string);


   /* @method getStyleFlags:int
     Return the font style flags 

   @return Font style flags, bit 0=italic, bit1=bold.
   */
   YM sSI  getStyleFlags(void);

   // ---- Free up resources taken up by FT2 loader (which also caches the glyphs)
   // ---- This is automatically done when this font face is deleted but you


   YM void  setMonochrome(sBool _bEnabled);
   YM sBool getMonochrome(void);
};

#endif
