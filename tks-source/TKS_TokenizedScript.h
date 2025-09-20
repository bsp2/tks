/// TKS_TokenizedScript.h
///
/// (c) 2001-2024 Bastian Spiegel <bs@tkscript.de>
///     - distributed under terms of the GNU general public license (GPL).
///

#ifndef __TKS_TOKENIZEDSCRIPT_H__
#define __TKS_TOKENIZEDSCRIPT_H__


enum tks_error_codes {
  ERRTKS=0x80000000,
  ERRTKSEOF,
  ERRTKSEOFLINECOMMENT,
  ERRTKSEOFBLOCKCOMMENT,
  ERRTKSEOSBLOCKCOMMENT,
  ERRTKSEOFQUOT,
  ERRTKSEOFQUOTR,

  ERRTKS_NUMERRORS
};


extern sUI tks_entity_tree_getIDByString(YAC_String *_s); 


class TKS_TokenizedScript { 

public:
   static void *tsl_cache; // unoptimized tokens, strings and lines
   static sUI   tsl_cache_size;
   static void AllocTSLCache(void);
   static void FreeTSLCache(void);

public:

   // this is filled w/ TKSToken ids. string parameters are marked with sequence <=DXT1_STRINGVAL, stringindex    
   struct __tokens { 
      sUI max_elements; 
      sUI num_elements; 
      sU16 *elements; 
   } tokens; 

   // This array stores all unresolved strings. the strings are indexed as described above
   struct __strings { 
      sUI max_elements; 
      sUI num_elements; 
      YAC_String *elements; 
   } strings; 

   // This array contains the start-token index of all lines in the scanned script. Debugging.. 
   struct __lines { 
      sUI max_elements; 
      sUI num_elements; 
      sU32 *elements;  // increased to 32bit on 23Oct2015
   } lines; 

   sBool b_own_tsl; // 1=own tokens/strings/lines elements, 0=reference to tsl_cache (before optimization)

   YAC_String       *source;             // pointer to source-script
   sUI               source_off;         // current offset in source
   sUI               source_line;        // current line in source 
   TKS_Token        *token_entries;      // token table 
   sUI               num_token_entries;  // current token table 

   sBool             b_xml; 

private:
   void  skipSpaces      (void);
   sU32  nextToken       (void);
   void  addToken        (sU16 _tok);
   void  addLine         (void);
   sU32  addUniqueString (YAC_String *); 

   void  initTSLFromCache (void);
   void  destructStrings  (void); // call d'tor for strings.elements

   YAC_String        str_const; 

public:
   TKS_TokenizedScript  (void);
   ~TKS_TokenizedScript ();
 
#ifdef TKS_EXPORT_ENTITY_SCANNER 
  static void InitEntityTree(void); 
#endif 
  sU32  scan            (YAC_String *, sBool _bRefCopy, sBool _isxml, sSI _max_token=TKS_SCANNER_MAX_TOKENS, sSI _max_strings=TKS_SCANNER_MAX_STRINGS, sS16 _max_lines=TKS_SCANNER_MAX_LINES); /// @return Number of tokens generated, or >=TKSERR if an error occured
  void  printTokens     (sUI _start, sUI _end, sUI _mark, sUI _lineOffset);
  void  writeTokenLine  (sUI _line, YAC_String *_d);
  void  optimize        (void);
  
  void  tokensToString  (YAC_String *_r); // Rewrite source to _r as seen by the scanner (i.e. comments and layout-whitespace area stripped off)
  
  sUI   calcSize        (sBool _bLineNumbers); // Calculate size of tokenized script data
  void  writeTokens     (yacmemptr _buffer);
  sBool readTokens      (yacmemptr _buffer, sUI _buflen);
};


#endif
