/// YAC_String.h
///
/// (c) 1998-2025 Bastian Spiegel <bs@tkscript.de>
///     - distributed under terms of the GNU general public license (GPL).
///

#ifndef __YAC_STRING_H__
#define __YAC_STRING_H__

enum __stringflags {
   TKS_STRFL_QUOT   = (1u << 24),  // \"\"
   TKS_STRFL_QUOTR  = (1u << 25),  // ~; region quote ~;
   TKS_STRFL_QUOT2  = (1u << 26),  // \'\'
   TKS_STRFL_IMM    = (1u << 29),  // immutable chars (always realloc) (constant strings)
   TKS_STRFL_LA     = (1u << 30),  // chars were allocated using linear allocator
   TKS_STRFL_DEL    = (1u << 31),  // chars are owned by this string
   TKS_STRFLQMASK   = (TKS_STRFL_QUOT | TKS_STRFL_QUOTR | TKS_STRFL_QUOT2),
   TKS_STRFL_PAD
};

#define UpperCase(a) ((sChar)( (a>='a'&&a<='z') ? (a&~32) : a ))
#define LowerCase(a) ((sChar)( (a>='A'&&a<='Z') ? (a|32)  : a ))


class YAC_String : public YAC_Object {
   //
   // a class for (buffered) string handling.
   //
public:
#ifdef TKS_STRING_OBJECTCOUNTER
   static sSI string_object_counter;
#endif // TKS_STRING_OBJECTCOUNTER

public:
   sUI               buflen; // ---- Total buffer size
   sUI               bflags; // ---- string flags (e.g. deleteme flag)
   sUI               length; // ---- Length of char array
   sUI               key;    // ---- hashkey (not unique), used for fast object comparison ----
   sU8              *chars;  // ---- array of 8 bit characters
   StaticList::Node *clones; // ---- used for (old-style) string lists (split(), words()). see StringIterator.

private:
   static sU8 *AllocChars(sUI n, sUI *_retBuflen, sUI *_retBFlags);
   void safeFreeChars(void);

  protected:
   void tokenizeEx (const YAC_Object *_cs, YAC_Object *_retRanges, YAC_Object *_r);

public:
   sBool         YAC_VCALL yacArrayAlloc             (sUI _len, sUI, sUI, sUI);
   sBool         YAC_VCALL yacArrayRealloc           (sUI _len, sUI, sUI, sUI);
   void          YAC_VCALL yacArraySet               (void *_context, sUI _index, YAC_Value *_value);
   void          YAC_VCALL yacArrayGet               (void *_context, sUI _index, YAC_Value *_ret);

   sUI           YAC_VCALL yacArrayGetWidth          (void) { return length; }
   sUI           YAC_VCALL yacArrayGetHeight         (void) { return 1; }
   sUI           YAC_VCALL yacArrayGetElementType    (void) { return 1; }
   sUI           YAC_VCALL yacArrayGetElementByteSize(void) { return sizeof(sChar); }
   sUI           YAC_VCALL yacArrayGetStride         (void) { return buflen; }
   void         *YAC_VCALL yacArrayGetPointer        (void) { return (void*)chars; }

   void          YAC_VCALL yacSerialize              (YAC_Object *, sUI);
   sUI           YAC_VCALL yacDeserialize            (YAC_Object *, sUI);

   sBool         YAC_VCALL yacCopy                   (YAC_Object *o);
   sBool         YAC_VCALL yacEquals                 (YAC_Object *o);

   void          YAC_VCALL yacValueOfI               (sSI);
   void          YAC_VCALL yacValueOfF32             (sF32);
   void          YAC_VCALL yacValueOfF64             (sF64);
   void          YAC_VCALL yacValueOfI64             (sS64);
   sBool         YAC_VCALL yacScanI                  (sSI *);
   sBool         YAC_VCALL yacScanF32                (sF32 *);
   sBool         YAC_VCALL yacScanF64                (sF64 *);
   sBool         YAC_VCALL yacScanI64                (sS64 *);

   sBool         YAC_VCALL yacToString               (YAC_String *s) const;

   sBool         YAC_VCALL yacIteratorInit           (YAC_Iterator *) const;

   sUI           YAC_VCALL yacOperatorPriority       (void) { return YAC_OP_PRIO_STRING; }
   void          YAC_VCALL yacOperator               (sSI , YAC_Object*, YAC_Value *_r);
   void          YAC_VCALL yacOperatorI              (sSI _cmd, sSI _intval, YAC_Value*);
   void          YAC_VCALL yacOperatorF32            (sSI _cmd, sF32 _floatval, YAC_Value *);
   void          YAC_VCALL yacOperatorF64            (sSI _cmd, sF64 _doubleval, YAC_Value *);
   sSI           YAC_VCALL yacTensorRank             (void) {return YAC_TENSOR_RANK_VAR;} // actually a matter of interpretation, could either be SCALAR or VECTOR



   void        init                      (void);
   YAC_String *clone                     (void);
   void        pushStack                 (YAC_String *_o);  /// ---- Remember an object in the clones list ----

   // Write unicode char as UTF-8 and return new byte offset
   //  (note) chars buffer must be large enough to hold the appended data
   sUI writeUTF8 (sUI _unicode, sUI _byteOffset);

   YAC_H(YAC_String);
   YAC_POOLED_H(YAC_String, YAC_POOL_PRIORITY_HIGH);

   YAC_String       (void); // calls init()
   YAC_String       (sSI); /// Create empty string and do not increase object_counter  (statics)
   YAC_String       (const sChar *e);
   YAC_String       (YAC_String *o);
   ~YAC_String      ();

   YAC_String&   operator =         (YAC_String *o);
   YAC_String&   operator =         (const sChar *e);
   sU8           operator []        (sUI i) const;

   sBool         alloc              (sUI len);
   void          free               (void);
   void          freeStack          (void);
   sBool         empty              (void);
   void          fixLength          (void); /// Fix string length by counting number of chars until EOF (0)
   void          visit              (const char *_cstring);
   void          visit              (YAC_String *_o);
   void          refCopy            (YAC_String *_o); // grab buffer from _o
   void          copyBuffered       (YAC_String *_o, sChar *_buf); //
   static sBool  CopyTo             (const sChar *a, sUI a_length, sUI a_offset,
                                     sChar *b, sUI b_length, sUI b_offset, sUI num_chars);
   sBool         copy               (const sChar *e);
   sBool         compare            (const sChar *e) const;
   sBool         compare            (YAC_String& s);
   sBool         compare            (YAC_String *s);
   static sUI    Replace            (sChar *s, sUI i, sChar a, sChar b);
   sUI           replaceTable       (const sUI *offsets, sUI k, const YAC_String *a, const YAC_String *b);
   sUI           replace            (sChar a, sChar b);
   sUI           replace            (const YAC_String *a, const YAC_String *b);
   sUI           replace            (const sChar *a, const YAC_String *b);
   sUI           replace            (const sChar *e, const sChar *f);
   sUI           replace            (const YAC_String *a, const sChar *f);
   void         _replace_YAC_RARG   (const YAC_String *, const YAC_String *, YAC_Object *_r);
   void         _replace_YAC_RVAL   (const YAC_String *, const YAC_String *, YAC_Value *_r);
   void         _replace_YAC_RSELF  (const YAC_String *, const YAC_String *);
   sSI          _replaceReturnNum   (const YAC_String *, const YAC_String *);

   //sUI replace(YAC_String& a, sChar *f);
   /// Replace substring <b>a</b> at offsets accessed in the
   /// <b>offsets</b> table with indices 0..<b>k</b> with <b>b</b> up to
   /// index <b>i</b> in this string. The string can be replaced in several runs
   /// and this is why the new index <b>i</b> is returned (may change if <b>b</b>.length
   /// varies from <b>a</b>.length).
   /// <br>
   /// <b>k</b> occurences of <b>a</b> will be replaced by <b>b</b>.
   /// @param i Offset in string
   /// @param offsets Offset table holding offset in string for each occurence of <b>a</b>.
   /// @param k Number of table entries
   /// @param a Look for this string
   /// @param b And replace it with this one.
   /// @return New offset i
   static void   ToLower             (sChar *e, sUI e_length);
   static void   ToUpper             (sChar *e, sUI e_length);
   sBool         append              (const sChar _c);
   sBool         append              (const sChar *e);
   sBool         append              (const YAC_String *s);
   void          _append_YAC_RARG    (const YAC_Object *_s, YAC_Object *_r);
   void          _append_YAC_RVAL    (const YAC_Object *_s, YAC_Value *_r);
   void          _append_YAC_RSELF   (const YAC_Object *_s);
   void          _appendRepeat_YAC_RSELF (const YAC_String *_s, sSI _numRepeats);
   void          _appendRepeat_YAC_RARG  (const YAC_String *_s, sSI _numRepeats, YAC_Object *_r);
   void          _appendRepeat_YAC_RVAL  (const YAC_String *_s, sSI _numRepeats, YAC_Value *_r);
   sBool         insert              (sUI _i, const YAC_String *_s);
   sBool         insert              (sUI _i, const char *_s);
   void          insert_INT          (sSI _index, const YAC_Object *_s);
   void          _insert_YAC_RARG    (sSI _index, const YAC_Object *_s, YAC_Object *_r);
   void          _insert_YAC_RVAL    (sSI _index, const YAC_Object *_s, YAC_Value *_r);
   void          _insert_YAC_RSELF   (sSI _index, const YAC_Object *_s);
   sBool         startsWith          (const sChar *e) const;
   sBool         startsWith          (const YAC_String *s) const;
   sBool         endsWith            (const sChar *e) const;
   sBool         endsWith            (const YAC_String *s) const;
   sBool         substring           (YAC_String *s, sUI start, sUI len) const;
   sBool         substring           (YAC_String& s, sUI start, sUI length) const;
   sBool         substring           (YAC_String *s, sUI start) const;
   sBool         substring           (YAC_String& s, sUI start) const;
   void          substring_INT       (sUI _startbyteoff, sUI _endbyteoff, YAC_String *_s) const;
   void          _substring_YAC_RARG (sUI _startbyteoff, sUI _endbyteoff, YAC_Object *_r) const;
   void          _substring_YAC_RVAL (sUI _startbyteoff, sUI _endbyteoff, YAC_Value *_r) const;
   void          _substring_YAC_RSELF(sUI _startbyteoff, sUI _endbyteoff);
   sBool         isPredecessor       (const YAC_String *o, sBool case_sensitive=0) const;
   sBool         _isPredecessor      (YAC_String *_o, sBool _bCaseSensitive);
   sSI           indexOf             (const sChar *e, sUI start=0) const;
   sSI           indexOf             (sChar _c, sUI _start=0) const;
   sSI           indexOf             (const YAC_String *s, sUI start=0) const;
   sSI           indexOf             (const YAC_String& s, sUI start=0) const;
   sUI           numIndicesOf        (const YAC_String *s) const;
   sUI           numIndicesOf        (const sChar *e) const;
   sSI           lastIndexOf         (const YAC_String *s, sUI start=0) const;
   sSI           lastIndexOf         (sChar _c, sUI _start) const;
   sBool         isSubstring         (const YAC_String *s, sUI start=0) const;
   void          valueOf             (YAC_Object *o);
   sBool         scan                (sChar *) const;
   sUI           getKey              (void); /// ---- Used for object comparison. Calls genKey() if key "has been lost"
   void          genKey              (void); /// ---- Produce a (not necessarily unique) string key.
   sU16          checkConversions    (sBool _bInternal = YAC_TRUE) const;
   sBool         checkIntConversion  (void) const;
   sBool         checkFloatConversion(void) const;
   sUI           sum                 (void) const;
   sUI           sumChars            (void) const; // for indexOf
   sBool         sio_readFile        (sBool _local, char *_fname);
   sBool         sio_readFile        (sBool _local, YAC_String *_fname);
   sBool         sio_writeFile       (char *_fname);
   sBool         sio_writeFile       (YAC_String *_fname);
   sBool         readRawFile         (sChar *fname);
   sBool         readRawFile         (YAC_String*fname);
   sBool         writeFile           (sChar *fname);
   sBool         writeFile           (YAC_String *_fname);
   void          trim                (YAC_String *_src);
   void          _trim_YAC_RARG      (YAC_Object *_s);
   void          _trim_YAC_RVAL      (YAC_Value *_r);
   void          _trim_YAC_RSELF     (void);
   sBool         scanProtected       (const YAC_String *_src, sSI _mode=0);
   void          setQuotFlag         (sSI _quotFlag);  // 1="" 2='' 3=;;region;;
   sSI           getQuotFlag         (void) const;
   void          substEscapeSeq      (void);
   void          resubstEscapeSeq    (void);
   void          resubstEscapeSeq2   (void);
   void          removeCR            (void);
   void          printf              (const char *_fmt, ...);
   void          setRawBuffer        (sU8 *_data, sUI _len);
   void          getDirName          (YAC_String *_r) const;
   void        replaceRegion             (sSI _startOff, sSI _endOff, YAC_Object *_repl);
   void        _replaceRegion_YAC_RARG   (sSI _startOff, sSI _endOff, YAC_Object *_repl, YAC_Object *_r);
   void        _replaceRegion_YAC_RVAL   (sSI _startOff, sSI _endOff, YAC_Object *_repl, YAC_Value *_r);
   void        _replaceRegion_YAC_RSELF  (sSI _startOff, sSI _endOff, YAC_Object *_repl);
   sSI         _replaceIncludes          (YAC_String *_optFilePrefix); // Replace `/some/file` strings by actual file content
   sSI         split                     (sSI _delimiter);
   sSI         words                     (sSI _consider_quotes);
   sUI         getLength                 (void) const;
   sUI         getBufLen                 (void) const;
   sUI         getNumChars               (void) const;
   sSI         _indexOf                  (const YAC_Object*, sSI) const;
   sSI         _lastIndexOf              (const YAC_Object *) const;
   sSI          charsetIndexOf           (const YAC_String *_charset, sSI _off) const;
   sSI          indexOfControlCharacter  (sSI _off) const;
   sSI         _indexOfChar              (sSI _a, sSI _start) const;
   sSI         _revIndexOfChar           (sSI _a, sSI _start) const;
   sSI         _indexOfFirstDifference   (YAC_Object *_s) const;
   void        _charsetBlacklist_YAC_RARG    (YAC_Object *_s, YAC_Object *_r);
   void        _charsetBlacklist_YAC_RVAL    (YAC_Object *_s, YAC_Value *_r);
   void        _charsetBlacklist_YAC_RSELF   (YAC_Object *_s);
   void        _charsetWhitelist_YAC_RARG    (YAC_Object *_s, YAC_Object *_r);
   void        _charsetWhitelist_YAC_RVAL    (YAC_Object *_s, YAC_Value *_r);
   void        _charsetWhitelist_YAC_RSELF   (YAC_Object *_s);
   sSI         _loadLocal                    (YAC_String *_filerl, sSI _asciiflag);
   sSI         _saveLocal                    (YAC_String *_filerl);
   sBool       _alloc                        (sUI _numChars);
   sBool       realloc                       (sUI _numChars, sBool _bCopy = YAC_TRUE);
   sBool       _realloc                      (sSI _numChars);
   sSI         _getc                         (sSI offset) const;
   void        _putc                         (sSI offset, sSI _char);
   sSI         isBlank                       (void) const; /// Check if string only contains blank characters (" ", "\n". "\t", 0)
   void        _copy                         (YAC_Object *);
   YAC_String *getWord                       (sSI) const;
   sSI         _startsWith                   (const YAC_String *) const;
   sSI         _endsWith                     (const YAC_String *) const;
   sSI         _patternMatch                 (YAC_Object *_s); // need default return other than 0, hence no YAC_String *
   sSI         _numIndicesOf                 (const YAC_String *_s) const;
   sSI         _load                         (YAC_String *_filerl, sSI _asciiflag);
   void        _parseXML                     (YAC_Value *_r);
   void        _splitSpace_YAC_RARG          (sSI _consider_quotes, YAC_Object *_r);
   void        _splitSpace_YAC_RVAL          (sSI _consider_quotes, YAC_Value *_r);
   void        _splitChar_YAC_RARG           (sSI _char, YAC_Object *_r);
   void        _splitChar_YAC_RVAL           (sSI _char, YAC_Value *_r);
   void        _splitCharset_YAC_RARG        (const YAC_Object *_cs, YAC_Object *_r);
   void        _splitCharset_YAC_RVAL        (const YAC_Object *_cs, YAC_Value *_r);
   sSI         _wordIndexAtCharIndex         (sUI _charIdx, sSI _use_quot);
   void        _tokenize_YAC_RARG            (const YAC_Object *_cs, YAC_Object *_r);
   void        _tokenize_YAC_RVAL            (const YAC_Object *_cs, YAC_Value *_r);
   void        _tokenizeWithRanges_YAC_RARG  (const YAC_Object *_cs, YAC_Object *_retRanges, YAC_Object *_r);
   void        _tokenizeWithRanges_YAC_RVAL  (const YAC_Object *_cs, YAC_Object *_retRanges, YAC_Value *_r);
   void         toLower                      (void);
   void        _toLower_YAC_RARG             (YAC_Object *_r);
   void        _toLower_YAC_RVAL             (YAC_Value *_r);
   void        _toLower_YAC_RSELF            (void);
   void         toUpper                      (void);
   void        _toUpper_YAC_RARG             (YAC_Object *_r);
   void        _toUpper_YAC_RVAL             (YAC_Value *_r);
   void        _toUpper_YAC_RSELF            (void);
   void         abbrev                       (sSI _length);
   void        _abbrev_YAC_RARG              (sSI _length, YAC_Object *_r);
   void        _abbrev_YAC_RVAL              (sSI _length, YAC_Value *_r);
   void        _abbrev_YAC_RSELF             (sSI _length);
   void        deleteRegion                  (sSI _off, sSI _length);
   void        _deleteRegion_YAC_RARG        (sSI _off, sSI _length, YAC_Object *_r);
   void        _deleteRegion_YAC_RVAL        (sSI _off, sSI _length, YAC_Value *_r);
   void        _deleteRegion_YAC_RSELF       (sSI _off, sSI _length);
   sSI         _indexOfWordStart             (sSI _off) const;
   sSI         _charsetIndexOfWordStart      (YAC_String *_charset, sSI _off) const;
   sSI         _indexOfWordEnd               (sSI _off) const;
   sSI         _charsetIndexOfWordEnd        (YAC_String *_charset, sSI _off) const;
   sBool       _compare                      (YAC_Object *_s);
   sBool       _compareIgnoreCase            (const YAC_Object *_s) const;
   sBool       _compareIgnoreWhitespace      (const YAC_Object *_s) const;
   sSI          replaceChar                  (sSI _a, sSI _b);
   sSI         _replaceCharReturnNum         (sSI _a, sSI _b);
   void        _replaceChar_YAC_RARG         (sSI _a, sSI _b, YAC_Object *_r);
   void        _replaceChar_YAC_RVAL         (sSI _a, sSI _b, YAC_Value *_r);
   void        _replaceChar_YAC_RSELF        (sSI _a, sSI _b);
   void        _New                          (YAC_Object *_s, YAC_Value *_r);
   void        _visit                        (const YAC_Object *_s, sSI _off, sSI _len);
   sSI         _getChecksum                  (void);
   sSI         _getChecksumIgnoreWhitespace  (void);
   void        _getDirName_YAC_RVAL          (YAC_Value *_r) const;
   void        _getDirName_YAC_RARG          (YAC_Object *_r) const;
   void        _escapeControlChars_YAC_RSELF (void);
   void        _escapeControlChars_YAC_RVAL  (YAC_Value *_r) const;
   void        _escapeControlChars_YAC_RARG  (YAC_Object *_r) const;
   sSI         _checkConversions             (void);
   void        _getDebugString               (YAC_Value *_r);
   sUI         _getMetaFlags                 (void) const;
   sBool       _wasQuoted                    (void) const;
   sBool       _wasQuotedSingle              (void) const;
   sBool       _wasQuotedDouble              (void) const;
   sUI         _numCharsAt                   (sSI _char, sUI _off) const;
   void        _utf8ToASCII_YAC_RARG         (YAC_Object *_r);
   void        _utf8ToASCII_YAC_RVAL         (YAC_Value *_r);
   void        _utf8ToASCII_YAC_RSELF        (void);

   // (note) CP1252 is the same as ISO8859-1 except for the 0x80..0x9F area (ctl codes in ISO)
   // (note) CP1252 is the HTML5 default encoding for text labeled as ISO8859-1
   void        _utf8ToCP1252_YAC_RARG  (YAC_Object *_r);
   void        _utf8ToCP1252_YAC_RVAL  (YAC_Value *_r);
   void        _utf8ToCP1252_YAC_RSELF (void);

   void        _cp1252ToUTF8_YAC_RARG  (sBool _bBOM, YAC_Object *_r);
   void        _cp1252ToUTF8_YAC_RVAL  (sBool _bBOM, YAC_Value *_r);
   void        _cp1252ToUTF8_YAC_RSELF (sBool _bBOM);

   sBool       _hasUTF8BOM (void) const;
   sBool       _isUTF8 (void) const;
};


class _String : public YAC_String {
public:
   YAC(_String);
};


#endif // __YAC_STRING_H__
