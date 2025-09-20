/// TKS_XMLParser.h
///
/// (c) 2001-2024 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///
#ifndef __TKS_XML_PARSER_H__
#define __TKS_XML_PARSER_H__


#define TKS_XMLParser_MAX_ALLOWED_LINES 1000000
#define TKS_XMLParser_DEFAULT_MAX_LINES   32000
#define TKS_XMLParser_STACK_DEPTH          1024

#define TKS_XMLParser_ERRNOERROR                      0x0000
#define TKS_XMLParser_ERRALLOWEDNUMBEROFLINESEXCEEDED 0x0011
#define TKS_XMLParser_ERRCANNOTALLOCLINES             0x0012
#define TKS_XMLParser_ERRTOOMANYLINES                 0x0013
#define TKS_XMLParser_ERREMPTYSOURCE                  0x0021
#define TKS_XMLParser_ERRPREMATUREENDOFQUOTE          0x0031
#define TKS_XMLParser_ERRATTRIBUTEASSIGNEXPECTED      0x0041
#define TKS_XMLParser_ERRPARSINGATTRIBUTENAME         0x0042
#define TKS_XMLParser_ERRCREATINGATTRIBUTE            0x0043
#define TKS_XMLParser_ERRCREATINGFLOWTEXTATTRIBUTE    0x0044
#define TKS_XMLParser_ERRPREMATUREENDOFFLOWTEXT       0x0045
#define TKS_XMLParser_ERRSTRAYENDELEMENT              0x0046
#define TKS_XMLParser_ERRATTRIBUTEVALUEEXPECTED       0x0051
#define TKS_XMLParser_ERRPARSINGATTRIBUTEVALUE        0x0052
#define TKS_XMLParser_ERRPREMATUREENDOFELEMENT        0x0061
#define TKS_XMLParser_ERRELEMENTNESTING               0x0062
#define TKS_XMLParser_ERRPARSINGELEMENTNAME           0x0063
#define TKS_XMLParser_ERRPARSINGMETAELEMENT           0x0071
#define TKS_XMLParser_ERRPARSINGCOMMENT               0x0072
#define TKS_XMLParser_ERRPARSINGHEADER                0x0073
#define TKS_XMLParser_ERRSTACKUNDERFLOW               0x0081
#define TKS_XMLParser_ERRSTACKOVERFLOW                0x0082
#define TKS_XMLParser_ERRUNTERMINATEDENTITY           0x0091

enum __xml_entities {
   XML_ENTITY_NONE = 0,

   XML_ENTITY_NBSP,
   XML_ENTITY_LT,
   XML_ENTITY_GT,
   XML_ENTITY_AMP,
   
   XML_ENTITY_NUM
};

extern sUI xml_entity_tree_getIDByString(YAC_String *_s);


class TKS_XMLParser {
public:

	struct __lines {
	  sUI max_elements;
	  sUI num_elements;
	  sUI *elements;
     sUI saved_linenr;
	} lines;

  char          *sp;
  char          *se;
  YAC_String     source;       // source-script
  sUI            source_off;   // current offset in source
  sUI            source_line;  // current line in source
  sUI            error_code;   // last error code
  YAC_String    *flow_text;    // current flow text
  YAC_String    *last_word;    // last parsed word
  YAC_HashTable *attributes;   // set of attributes for current element

  struct __tree {
    YAC_TreeNode *root;
    YAC_TreeNode *c;
    YAC_TreeNode *t;
    sBool         first_inner;
  } tree;

private:

  // ---- clean up the mess from the last parsing session
  void        cleanup         (void);

  // ---- find 3.14, 42, -3.14, -42, "string", 'string', "print 'hello.'", 'test\n\'\"'
  YAC_String *scanNextWord    (void);

  // ---- find <element  or </element, return element name and whether it was a closing element.
  YAC_String *scanNextElement (sBool *_ret_el_close);

  // ---- scan flow text until start of next <element or EOF
  YAC_String *scanFlowText    (void);

  // ---- append to current flow_text, which will be created if necessary
  void        addFlowText     (YAC_String *);

  // ---- delete flow text (if exists)
  void        clearFlowText   (void);

  // ---- delete last word (if exists)
  void        clearLastWord   (void);

  // ---- unlink flow text (reset to null) and return deletable flow_text string
  YAC_String *unlinkFlowText  (void);

  // ---- unlink last parsed word (reset to null) and return deletable word_text string
  YAC_String *unlinkLastWord  (void);

  // ---- adjust sp and source_off to _idx
  void        skipTo          (sSI _idx);

  // ---- skip _num chars, return false if end of buffer has been reached, true otherwise.
  sBool       skipForward     (sUI _num);

  // ---- skip forward until next non-blank character
  void        skipSpaces      (void);

  // ---- return index of character _c occurence or -1
  sSI         indexOfChar     (char _c);

  // ---- return index of character _c occurence or -1 if any non-blank char was found before _c
  sSI         indexOfExpectedChar(char _c);

  // ---- skip to next occurrence of character _c
  sBool       skipToChar      (char _c);

  // ---- skip to next occurence of character _c, false if any non-blank char was found before _c
  sBool       skipToExpectedChar(char _c);

  // ---- find next '<', return index or -1
  sBool       skipToElementStart  (void);

  // ---- find next attribute or attribute value
  sBool       skipToWordStart (void);

  // ---- find end of comment '-->'
  sBool       skipToCommentEnd (void);

  // ---- find end of header '?>'
  sBool       skipToHeaderEnd (void); 

  // ---- remember current source line in lines array and increase the number of lines
  void        addLine         (void);
  void        addLineAtOffset (sUI);
  void        beginLines      (void);
  void        rewindLines     (void);

  // ---- abort with parseerror.
  sUI         parseError      (sUI _reason, const char *_msg);

  // ---- Get human-readable error message
  static const char *GetErrorStringByCode(sUI _code);

  // ---- allocation error
  sUI         allocError      (sUI _reason, const char *_msg);

  // ---- insert new TreeNode that stores the flow_text String
  void        appendStringNode (YAC_String *);

  // ---- append an element to the current tree
  void        appendElementNode      (void);

public:
      TKS_XMLParser  (void);
      ~TKS_XMLParser ();

public:
  // ---- parse _max_lines from XML string, return Tree(Node) of HashTables in _r
  void        parse           (YAC_String *, sUI _max_lines, YAC_Value *_r);

  // ---- print an extract from the input buffer. used by parseError()
  void        printChars      (sUI _charstart, sUI _charend, sUI _mark);

#ifdef TKS_EXPORT_ENTITY_SCANNER
  static void InitEntityTree (void);
#endif

};


#endif // __TKS_XML_PARSER_H__
