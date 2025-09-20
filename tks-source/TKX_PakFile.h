/// TKX_PakFile.h
///
/// (c) 2001-2024 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///
///
#ifndef __TKX_PAKFILE_H__
#define __TKX_PAKFILE_H__


class TKX_PakFile {
public:
   
   enum __takerrorcodes {
      TKXERRUNKNOWN = 0,
      TKXERROPENFILE,
      TKXERRCREATEFILE,
      TKXERROUTOFMEM,
      TKXERRCOMPRESS,
      TKXERRINVALIDHEADER,
      TKXERRPARSELOGICNAME,
      TKXERRPARSELOCALNAME,
      TKXERROPENINCLUDEFILE,
      TKXERROPENLIB,
      TKXERROPENINPUTFILE,

      TKXNUMERRORS
   };
   
public:
   YAC_String str_tkpfilename;
   YAC_String str_takfilename;
   YAC_String str_tkpfile;
   YAC_String str_takfile;
   YAC_String str_prjname;
   YAC_String str_prjdate;
   YAC_String str_prjversion;
   YAC_String str_dataprefix;     // logic file name prefix for non-tks pakfile entries (set with [chapter] "dataprefix" attribute)
   YAC_String str_logfilename;
   YAC_String str_sourcefilename;
   YAC_String str_chaptername;
   sU32 pak_size;
   
   TKX_Chapter *chapters;
   TKX_Chapter *current_chapter;
   YAC_String   current_chapter_name;
   TKX_File    *current_file;
   char         tkx_header[1024];
   sSI          tak_error_code;

private:
   void addChapter (void);
   void addFile    (sBool _bPre = YAC_FALSE);
   
public:
   TKX_PakFile(void);
   ~TKX_PakFile();
   
   void      cleanUp                (void);
   void      scanProject            (void);
   void      writeTKX               (void);
   sBool     scanTKX                (void); /// called when the chapter index table has been downloaded.
   void      continueChapterLoading (void); /// called when new data has become available
   void      selectFirstChapter     (void);
   void      selectChapterByName    (YAC_String *_name);
   TKX_File *findLogic              (char *_name);
   TKX_File *findLogic              (YAC_String *_name);
   sBool     prepareTKXDownload     (sU8 *_firstkheader); /// when the first ~1024 byte have been downloaded, this fun is called to check file header and prepare the tkx file buffer 
   void      listContent            (void);
   void      addExtraFile           (const char *_name, sBool _bPre); // used to add additional files after a .tkp/.tkx has been loaded
   sBool     mergeLocalTSL          (YAC_String *_libName);
};


#endif // __TKX_PAKFILE_H__
