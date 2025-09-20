/// TKX_Chapter.h
///
/// (c) 2001-2019 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///
///
#ifndef __TKX_CHAPTER_H__
#define __TKX_CHAPTER_H__


#ifdef YAC_LITTLE_ENDIAN
#define TKX_get32(a) (*(sU32*)(a))
#define TKX_set32(a,b) { yacmemptr p; p.any = (void*)(a); *p.u32 = (b); }
#else
extern sUI  TKX_get32(sU32*_s);
extern void TKX_set32(sU32*_d, sU32 _b);
#endif

class TKX_PakFile;

class TKX_Chapter {
public:
   TKX_PakFile   *parent;
   TKX_Chapter   *next;
   YAC_String     chapter_name;
   TKX_File      *chapter_files;
   sUI            num_files;
   sUI            total_chapter_filesize;   // size of uncompressed file data
   sUI            total_chapter_indexsize;  // size of file index table
   unsigned char *orig_data;
   unsigned char *compr_data;
   unsigned long  compr_size;
   sUI            orig_size;
   sUI            pak_size;
   sBool          is_loaded; // true if chapter has been retrieved and is ready to be accessed
   
public:
   TKX_Chapter(TKX_PakFile *_parent);
   ~TKX_Chapter();
   
   void createCompressedChapter(void);
   void loadCompressedChapter(void);

   TKX_File *findFileByLogicName (YAC_String *_logicName) const;

   sBool mergeTSL(TKX_File *_lf, sBool _bTSL = 0);
};



#endif // __TKX_CHAPTER_H__
