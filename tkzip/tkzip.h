// ----
// ---- file   : tkzip.h
// ---- author : Bastian Spiegel <bs@tkscript.de>
// ---- legal  : (c) 2020-2025 by Bastian Spiegel.
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

#ifndef TKZIP_H__
#define TKZIP_H__

YG("zip")

struct tkzip_source_t {
   sBool           b_free_source; // zip_file_add() failed, must free source
   zip_source_t   *source;        // auto-freed by libzipfile unless an error occured
   yacmemptr       data;
   sUI             num_bytes;
   tkzip_source_t *prev;
};

/* @class Zip,Object
*/
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
      /* @method openLocalReadOnly,String pathName:boolean
       */
      YM sBool openLocalReadOnly (YAC_String *_pathName);

      /* @method openLocal,String pathName:boolean
       */
      YM sBool openLocal (YAC_String *_pathName);

      /* @method createLocal,String pathName:boolean
       */
      YM sBool createLocal (YAC_String *_pathName);

      /* @method close
       */
      YM void  close (void);

      /* @method getNumEntries:int
       */
      YM sUI   getNumEntries (void);

      /* @method getNameByIndex,int index:String
       */
      YM void  getNameByIndex (sUI _index, YAC_Value *_r);

      /* @method getIndexByName,String pathName:int
       */
      YM sSI   getIndexByName (YAC_String *_pathName);

      /* @method getSizeByIndex,int index:UnsignedLong
       */
      YM void  getSizeByIndex (sUI _index, YAC_Value *_r);

      /* @method getCompressedSizeByIndex,int index:UnsignedLong
       */
      YM void  getCompressedSizeByIndex (sUI _index, YAC_Value *_r);

      /* @method loadStringByIndex,int index:String
       */
      YM void  loadStringByIndex (sUI _index, YAC_Value *_r);

      /* @method loadStringByName,String pathName:String
       */
      YM void  loadStringByName (YAC_String *_pathName, YAC_Value *_r);

      /* @method loadBufferByIndex,int index:Buffer
       */
      YM void  loadBufferByIndex (sUI _index, YAC_Value *_r);

      /* @method loadBufferByName,String pathName:Buffer
       */
      YM void  loadBufferByName (YAC_String *_pathName, YAC_Value *_r);

      /* @method addDir,String pathName:boolean
         add directory to archive

         (note) adding a file in a subdir w/o creating the dir first results in garbage file content
       */
      YM sBool addDir (YAC_String *_pathName);

      /* @method addFile,String pathName,Object data:boolean
         add file to archive

         @arg pathName
         @arg data a String, an Array, a Buffer, or a Stream (or any other Object that implements the array/stream interface(s))
       */
      YM sBool addFile (YAC_String *_pathName, YAC_Object *_data);
};


#endif // TKZIP_H__
