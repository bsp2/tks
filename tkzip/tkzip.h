// ----
// ---- file   : tkzip.h
// ---- author : Bastian Spiegel <bs@tkscript.de>
// ---- legal  : (c) 2020 by Bastian Spiegel. 
// ----          Distributed under terms of the GNU LESSER GENERAL PUBLIC LICENSE (LGPL). See 
// ----          http://www.gnu.org/licenses/licenses.html#LGPL or COPYING for further information.
// ----
// ---- info   : libzip interface
// ----
// ---- created: 04Jan2020
// ---- changed: 10Jan2020
// ----
// ----
// ----

#ifndef __TKZIP_H__
#define __TKZIP_H__


YG("zip")


struct tkzip_source_t {
   sBool           b_free_source; // zip_file_add() failed, must free source
   zip_source_t   *source;        // auto-freed by libzipfile unless an error occured
   yacmemptr       data;
   sUI             num_bytes;
   tkzip_source_t *prev;
};

YC class Zip  : public YAC_Object {
  protected:
   zip_t *zip;
   tkzip_source_t *sources;  // last added source or NULL

	public:
		YAC(Zip);

      Zip(void);
      ~Zip();

  protected:
      sBool openLocalInt (YAC_String *_pathName, int _flags);
      sS64 getSizeByIndexInt (sUI _index);
      void freeSources (void);
      tkzip_source_t *addSourceFromData (const void *_dataOrNull, sUI _numBytes);

  public:
      YM sBool openLocalReadOnly (YAC_String *_pathName);
      YM sBool openLocal (YAC_String *_pathName);
      YM sBool createLocal (YAC_String *_pathName);
      YM void  close (void);
      YM sUI   getNumEntries (void);
      YM void  getNameByIndex (sUI _index, YAC_Value *_r);
      YM sSI   getIndexByName (YAC_String *_pathName);
      YM void  getSizeByIndex (sUI _index, YAC_Value *_r);
      YM void  getCompressedSizeByIndex (sUI _index, YAC_Value *_r);
      YM void  loadStringByIndex (sUI _index, YAC_Value *_r);
      YM void  loadStringByName (YAC_String *_pathName, YAC_Value *_r);
      YM void  loadBufferByIndex (sUI _index, YAC_Value *_r);
      YM void  loadBufferByName (YAC_String *_pathName, YAC_Value *_r);

      // (note) adding a file in a subdir w/o creating the dir first results in garbage file content
      YM sBool addDir (YAC_String *_pathName);

      // (note) data can be a String, an Array, a Buffer, or a Stream
      //         (or any other Object that implements the array/stream interface(s))
      YM sBool addFile (YAC_String *_pathName, YAC_Object *_data);
};



#endif // __TKZIP_H__

