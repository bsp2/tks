/// TKS_XMLParser.cpp
///
/// (c) 2004-2024 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///

/*
 * returns a Tree of HashTables
 *
 */

#include "tks.h"

#include "YAC_ValueObject.h"
#include "YAC_TreeNode.h"
#include "YAC_HashTable.h"

#include "TKS_XMLParser.h"

#include "EntityTree.h"

#ifdef TKS_EXPORT_ENTITY_SCANNER
static EntityTree entity_tree;
#endif

#define TKS_ISNUM(a) ((a) >= 48 && (a) <= 57)

TKS_XMLParser::TKS_XMLParser(void) {
   source_off  = 0u;
   source_line = 0u;

   lines.elements = NULL;
   lines.num_elements = 0u;
   lines.max_elements = 0u;

   flow_text = NULL;
   last_word = NULL;
   tree.root = NULL;
   tree.c    = NULL;
   tree.t    = NULL;
}

TKS_XMLParser::~TKS_XMLParser() {
   cleanup();
}

void TKS_XMLParser::cleanup(void) {
   source.free();
   if(lines.elements)
   {
      delete [] lines.elements;
      lines.elements = NULL;
   }
   clearFlowText();
   clearLastWord();
}

sUI TKS_XMLParser::allocError(sUI _reason, const char *_msg) {
   if(_msg)
   {
      Dprintf("[---] TKS_XMLParser::%s (allocError=0x%x)\n", _msg, _reason);
   }
   else
   {
      Dprintf("[---] TKS_XMLParser::(allocError=0x%x)\n", _reason);
   }
   error_code = _reason;
   return error_code;
}

sUI TKS_XMLParser::parseError(sUI _reason, const char *_msg) {
   printChars(source_off-40u, source_off+40u, source_off);
   Dprintf("[---] TKS_XMLParser: \"%s\" in line %i.\n", GetErrorStringByCode(_reason), lines.num_elements);
   if(_msg)
   {
      Dprintf("[---] TKS_XMLParser: %s \n", _msg);
   }
   error_code = _reason;
   return error_code;
}

void TKS_XMLParser::skipTo(sSI _idx) {
   source_off = _idx;
   sp = (char*)source.chars + source_off;
}

void TKS_XMLParser::skipSpaces(void) {
   sp--;
   for(;;)
   {
      switch(*++sp)
      {
         case 0:
            return;

         case '\n': addLine();
         case '\014':
         case '\t':
         case '\r':
         case ' ':
            break;

         default:
            source_off = (sUI) size_t(sp - (char*)source.chars);
            return;
      }
   }
}

sSI TKS_XMLParser::indexOfChar(char _c) {
   beginLines();
   char *s = sp - 1;
   char *st = sp;
   while(++s < se)
   {
      char c = *s;
      if(c == _c)
         return sSI(s - st);
      switch(c)
      {
         case 0:
            return -1;

         case '\n':
            addLine();
            // run into next case
         case '\014':
         case '\t':
         case '\r':
         case ' ':
         default:
            break;

         case '>':
            return -1;
      }
   }
   return -1;
}

sSI TKS_XMLParser::indexOfExpectedChar(char _c) {
   beginLines();
   char *s  = sp - 1;
   char *st = sp;
   while(++s < se)
   {
      char c = *s;
      if(c == _c)
         return sSI(s - st);
      switch(c)
      {
         case 0:
            return -1;

         case '\n':
            addLine();
            // run into next case
         case '\014':
         case '\t':
         case '\r':
         case ' ':
            break;

         case '>':
         default:
            return -1;
      }
   }
   return -1;
}

sBool TKS_XMLParser::skipToChar(char _c) { // find '<'
   sSI idx = indexOfChar(_c);
   if(-1 != idx)
   {
      source_off += idx;
      sp += idx;
      return YAC_TRUE;
   }
   else
   {
      rewindLines();
      return YAC_FALSE;
   }
}

sBool TKS_XMLParser::skipToExpectedChar(char _c) { // find '<' / '=' / '>'
   sSI idx = indexOfExpectedChar(_c);
   if(-1 != idx)
   {
      source_off += idx;
      sp += idx;
      return YAC_TRUE;
   }
   else
   {
      rewindLines();
      return YAC_FALSE;
   }
}

sBool TKS_XMLParser::skipToCommentEnd(void) { // find -->
   sChar hist[2]; hist[0] = hist[1] = 0;
   for(;;)
   {
      sChar c = *sp++;
      switch(c)
      {
         case 0:
            return YAC_FALSE;

         case '\n':
            addLine();
            hist[0] = hist[1] = 0;
            break;

         default:
            hist[0] = hist[1];
            hist[1] = c;
            break;

         case '>':
            if( (hist[0]=='-') && (hist[1]=='-') )
            {
               source_off = (sUI) size_t(sp - (char*)source.chars);
               return YAC_TRUE;
            }
            break;
      }
   }
}

sBool TKS_XMLParser::skipToHeaderEnd(void) { // search ?>
   sChar hist = 0;
   for(;;)
   {
      sChar c = *sp++;
      switch(c)
      {
      case 0:
         return YAC_FALSE;

      case '\n':
         addLine();
         hist = 0;
         break;

      default:
         hist = c;
         break;

      case '>':
         if('?' == hist)
         {
            source_off = (sUI) size_t(sp-(char*)source.chars);
            return YAC_TRUE;
         }
         break;
      }
   }
}

sBool TKS_XMLParser::skipToElementStart(void) { // find '<'
   if(skipToExpectedChar('<'))
   {
      if('!' == sp[1])
      {
         if( ('-' == sp[2]) && ('-' == sp[3]) ) // <!-- comment?
         {
            if(!skipToCommentEnd())
            {
               parseError(TKS_XMLParser_ERRPARSINGCOMMENT, "skipToElementStart: error while looking for end of <!-- comment.");
            }
            else
            {
               return skipToElementStart();
            }
         }
         else
         {
            if(!skipToChar('>')) // skip meta element for now; will be parsed in future releases
            {
               parseError(TKS_XMLParser_ERRPARSINGMETAELEMENT, "skipToElementStart: error while looking for end of '<!' meta element.");
            }
            else
            {
               if(!skipForward(1))
                  parseError(TKS_XMLParser_ERRPARSINGMETAELEMENT, "skipToElementStart: error while looking for end of '<!' meta element.");
               return skipToElementStart();
            }
         }
      }
      else
      {
         if('?' == sp[1]) // <? .. ?> xml header
         {
            if(!skipToHeaderEnd())
            {
               parseError(TKS_XMLParser_ERRPARSINGHEADER, "skipToHeaderStart: error while looking for end of <? header.");
            }
            else
            {
               return skipToElementStart();
            }
         }
         else
         {
            return YAC_TRUE;
         }
      }
   }
   return YAC_FALSE;
}

sBool TKS_XMLParser::skipToWordStart(void) { // search until '<', break at default char
   beginLines();
   sp--;
   while(++sp < se)
   {
      switch(*sp)
      {
      case 0:
         rewindLines();
         return YAC_FALSE;

      case '\n':
         addLine();
         // run into next case
      case '\014':
      case '\t':
      case '\r':
      case ' ':
         break;

      case '/':
      case '>':
         sp = (char*)source.chars + source_off;
         rewindLines();
         return YAC_FALSE;

      default:
         source_off = (sUI) size_t(sp - (char*)source.chars);
         return YAC_TRUE;
      }
   }
   rewindLines();
   return YAC_FALSE;
}

YAC_String *TKS_XMLParser::scanFlowText(void) { // search until '<', break at EOF
   beginLines();
   for(sUI i = source_off; i < source.length; i++)
   {
      switch(source.chars[i])
      {
         case 0:
            rewindLines();
            return NULL;

         case '<':
            if(i != source_off)
            {
               // ---- copy chars between >...<
               YAC_String *s = YAC_New_String();
               s->alloc( (i - source_off) + 1u );
               sUI k, j;
               for(k = 0u, j = source_off; j < i; j++)
               {
                  s->chars[k++] = source.chars[j];
               }
               s->chars[k++] = 0u;
               s->length = k;
               s->key = YAC_LOSTKEY;
               source_off = i; // xxx
               sp = (char*)(source.chars + i); // xxx
               return s;
            }
            else
               return NULL;

         case '>':
#if 0  // [21Mar2024] allow stray '>' in flow text (GraphForm "->" edges)
            rewindLines();
            parseError(TKS_XMLParser_ERRSTRAYENDELEMENT, 0);
            return NULL;
#else
            break;
#endif

         case '\n':
            addLine();
            // run into default case
         default:
            break;
      }
   }
   // ---- premature EOF found
   rewindLines();
   parseError(TKS_XMLParser_ERRPREMATUREENDOFFLOWTEXT, 0);
   return NULL;
}

sBool TKS_XMLParser::skipForward(sUI _inc) {
   if(source_off >= source.length)
      return YAC_FALSE;
   sUI soe = source_off + _inc;
   while(source_off < soe)
   {
      source_off++;
      if('\n' == *sp++)
         addLine();
   }
   return YAC_TRUE;
}

YAC_String *TKS_XMLParser::scanNextElement(sBool *_ret_is_closing) {
   YAC_String *r = NULL;
   if(skipToElementStart())
   {
      if( (sp[1] != '/') && ( ((sp[1] < 'A') || (sp[1] > 'z')) && (sp[1] != ':')) )
      {
         parseError(TKS_XMLParser_ERRPARSINGELEMENTNAME, 0);
         Dprintf("[---] TKS_XMLParser: element name may not start with \'%c\'.\n",
            (char)sp[0]);
         return NULL;
      }
      else
      {
         sBool is_closing = ('/' == sp[1]);
         *_ret_is_closing = is_closing;
         if(skipForward(1 + is_closing))
         {
            r = scanNextWord(); // scan element name
         }
      }
   }
   return r;
}

YAC_String *TKS_XMLParser::scanNextWord(void) {
   YAC_String *r = NULL;
   if(skipToWordStart())
   {
      sBool quot_mode   = YAC_FALSE;
      sBool dquot_mode  = YAC_FALSE;
      sBool quot_flag   = YAC_FALSE;
      sBool dquot_first = YAC_FALSE;
      sUI i;
      sUI il = (source.length - source_off);
      sUI k = 0u; // let k be the length of the unknown string
      sUI off = 0u;
      beginLines();
      for(i = 0u; !k && i < il; i++)
      {
         switch(sp[i])
         {
         case '\'': // enter/leave double quote mode?
            dquot_mode = 1 - dquot_mode;
            if(quot_mode&&(!dquot_first)) // "trace 'hello, world.';" style
            {
               dquot_first = YAC_FALSE;// xxx
               // // off=i+1;
            }
            else
            {
               if( (sp[i+1]==0x0f) && (sp[i+3]=='\'') )
               {
                  dquot_mode = YAC_FALSE; // '\'' 0x0F <esccode> '\'' magic sequence  , see YAC_String::resubstEscapeSeq()
                  i += 3u;
               }
               else
               {
                  dquot_first = YAC_TRUE;
                  if(!dquot_mode) // finished?
                  {
                     k = i - off;
                     if(k > 1u)
                     {
                        quot_flag = YAC_TRUE; // regular 'test' char sequence
                     }
                     else
                     {
                        quot_flag = YAC_FALSE; // ASCII constant 'A'
                        //bcharconst=1;
                     }
                  }
                  else
                  {
                     // 'trace "hello, world.";' style
                     if('\'' == sp[i+1])
                     {
                        dquot_mode = YAC_FALSE;
                        // empty string
                        k = 0xDEAFu;
                     }
                     else
                        off = i + 1u; // set start
                  }
               }
            }
            break;
         case '\"': // enter/leave quote mode?
            quot_mode = 1 - quot_mode;
            if(dquot_mode && dquot_first)
            {
               // // off=i+1; // set start
            }
            else
            {
               if(!quot_mode)
               {
                  k = i - off;
                  quot_flag = YAC_TRUE;
               }
               else
               {
                  dquot_first = YAC_FALSE;
                  if('\"' == sp[i+1])
                  {
                     // empty string
                     k = 0xDEAFu;
                  }
                  else
                     off = i + 1u;
               }
            }
            break;
         case '\n': // blank
            addLine();
            // run into next case label
         case '\t': //
         case '\r':
         case '\014':
         case ' ':  //
         case '=':  // are
         case '<':
         case '/':
         case '>':
            if( (quot_mode && !dquot_first) || (dquot_mode && dquot_first) )
               break;
         case 0:
            if( (quot_mode && !dquot_first) || (dquot_mode && dquot_first) )
            {
               parseError(TKS_XMLParser_ERRPREMATUREENDOFQUOTE, 0);
               rewindLines();
               return NULL;
            }
            k = i;     // end of buffer reached
            break;
         }
      } // for..

      if(i < il)
      {
         if(!last_word)
            last_word = (YAC_String *)YAC_NEW_CORE(YAC_CLID_STRING);
         else
            last_word->empty();
         if( k && (0xDEAFu != k) )
         {
            source.substring(last_word, source_off+off, k);
            source_off += k + ((0u != off) ? (off + 1u) : 0u);
            sp = (char*)(source.chars+source_off);
            last_word->setQuotFlag(quot_flag);
         }
         else
         {
            // ---- empty string ("", '')
            source_off += 2u;
            sp += 2;
            last_word->setQuotFlag(1);
         }
         r = last_word;
      }
  }
  /////printf("xxx scanNextWord: \"%s\".\n", r->chars);
  return r;
}


void TKS_XMLParser::addLine(void) {
   addLineAtOffset(source_off);
}

void TKS_XMLParser::addLineAtOffset (sUI) {
   source_line++;
   if(source_line < (lines.max_elements - 1u))
   {
      lines.elements[lines.num_elements++] = source_off;
      lines.elements[lines.num_elements]   = source.length;
   }
   else
      allocError(TKS_XMLParser_ERRTOOMANYLINES, "addLine: maximum number of lines exceeded.");
}

void TKS_XMLParser::beginLines(void) {
   lines.saved_linenr = lines.num_elements;
}

void TKS_XMLParser::rewindLines(void) {
   if(lines.saved_linenr)
   {
      lines.num_elements = lines.saved_linenr;
      lines.saved_linenr = 0u;
   }
}

void TKS_XMLParser::parse(YAC_String *_s, sUI _max_lines, YAC_Value *_r) {
   cleanup();

   _r->initObject(0,0);

   if(_max_lines)
   {
      if(_max_lines > TKS_XMLParser_MAX_ALLOWED_LINES)
      {
         allocError(TKS_XMLParser_ERRALLOWEDNUMBEROFLINESEXCEEDED, "");
         return;
      }
   }
   else
      _max_lines = TKS_XMLParser_DEFAULT_MAX_LINES;

   source.yacCopy(_s);
   source.removeCR();
   source.substEscapeSeq();

   if(source.isBlank())
   {
      allocError(TKS_XMLParser_ERREMPTYSOURCE, "String is empty.");
      return;
   }

   // ---- tree structure
   tree.root        = NULL;
   tree.c           = NULL;
   tree.t           = NULL;
   tree.first_inner = YAC_FALSE;

   // ---- elements
   YAC_String s_id; s_id.visit("id");

   // ---- parser node stack
   sUI           stacki = TKS_XMLParser_STACK_DEPTH;
   YAC_TreeNode *stack[TKS_XMLParser_STACK_DEPTH];

   // ---- allocate lines
   lines.elements = new(std::nothrow) sUI[_max_lines];
   if(NULL == lines.elements)
   {
      allocError(TKS_XMLParser_ERRCANNOTALLOCLINES, "cannot allocate lines.");
      return;
   }
   lines.num_elements = 0u;
   lines.max_elements = _max_lines;
   lines.saved_linenr = 0u;

   // ---- scanner variables
   source_off  = 0u;
   source_line = 0u;
   sp = (char*)source.chars;
   se = (char*)source.chars+source.length;
   addLine(); // begin first line with character at index 0
   sBool is_closing;
   error_code = TKS_XMLParser_ERRNOERROR;
   sBool b_nextel = YAC_TRUE;
   YAC_String ft_name; ft_name.visit("<>"); // name of flow text attribute
   YAC_String s_entity; s_entity.buflen = 0u; s_entity.bflags = 0u;
   while(b_nextel && !error_code)
   {
      b_nextel = scanNextElement(&is_closing) && !error_code;
      if(b_nextel)
      {
         YAC_String *s_element = last_word;
         if(is_closing)
         {
            //tkscript->printf("found closing element \"</%s>\".\n", s_element->chars);
            if(tree.first_inner)
            {
               tree.first_inner = YAC_FALSE;
               if(tkscript->configuration.debug_level > 90)
               {
                  Dprintf("[...] TKS_XMLParser: warning, empty <element></element>.\n");
               }
            }
            if(TKS_XMLParser_STACK_DEPTH == stacki)
               parseError(TKS_XMLParser_ERRSTACKOVERFLOW, "stack overflow (pop)");
            else
            {
               tree.c = stack[stacki++];
               ////printf("xxx pop stacki=%d tree.c =%p name=\"%s\"\n", stacki-1, tree.c, tree.c->name.chars);
               if(!tree.c->name.compare(s_element))
               {
                  parseError(TKS_XMLParser_ERRELEMENTNESTING, 0);
                  Dprintf("[---] TKS_XMLParser: </element> nesting error: expected \"%s\", got \"%s\". (stacki=%i)\n",
                     (char*)tree.c->name.chars, (char*)s_element->chars, stacki);
                  goto SevereParserException_thrown;
               }
            }
         }
         else
         {
            appendElementNode();
         }
         //else
         //   tkscript->printf("found element \"<%s>\".\n", s_element->chars);

         // ---- scan attributes
         sBool b_nextat = YAC_TRUE;
         while(b_nextat && !error_code)
         {
            b_nextat = skipToWordStart();
            if(b_nextat)
            {
               if('<' == sp[0])
                  parseError(TKS_XMLParser_ERRPARSINGATTRIBUTENAME, "attribute name may not start with \'<\'.");
               else
                  // ---- scan attribute name
                  if(!scanNextWord())
                     parseError(TKS_XMLParser_ERRPARSINGATTRIBUTENAME, "error while parsing attribute name.");
                  else
                  {
                     YAC_String *s_attribute_name = unlinkLastWord();
                     if(!skipToExpectedChar('='))
                        parseError(TKS_XMLParser_ERRATTRIBUTEASSIGNEXPECTED, "missing \'=\' token in attribute=value assignment.");
                     else
                     {
                        if(!skipForward(1))
                           parseError(TKS_XMLParser_ERRPARSINGATTRIBUTEVALUE, "error while parsing attribute value.");
                        else
                        {
                           if(!skipToWordStart())
                              parseError(TKS_XMLParser_ERRATTRIBUTEVALUEEXPECTED, "expected attribute value.");
                           else
                              if('<' == sp[0])
                                 parseError(TKS_XMLParser_ERRPARSINGATTRIBUTEVALUE, "attribute value may not start with \'<\'.");
                              else
                              {
                                 // ---- scan attribute value
                                 if(!scanNextWord())
                                    parseError(TKS_XMLParser_ERRPARSINGATTRIBUTEVALUE, "error while parsing attribute value.");
                                 else
                                 {
                                    // ---- attribute (value) parsed
                                    //if(is_closing)
                                    //parseError(TKS_XMLParser_ERRCLOSINGATTRIBUTES, "</element> closing elements must not have attributes.");
                                    YAC_String *s_attribute_value = unlinkLastWord();
                                    /*tkscript->printf("found attribute \"%s\"=\"%s\"\n",
                                       s_attribute_name ->chars,
                                       s_attribute_value->chars
                                       );*/
                                    if(!is_closing)
                                    {
                                       s_attribute_value->resubstEscapeSeq2();
                                       TKS_CachedObject *co = attributes->varcache.createStringEntry(s_attribute_name, 1, s_attribute_value, 1);
                                       if(NULL == co)
                                       {
                                          parseError(TKS_XMLParser_ERRCREATINGATTRIBUTE, "error creating hashtable entry.");
                                       }
                                       else
                                       {
                                          if(s_attribute_name->compare(&s_id))
                                          {
                                             tree.c->id.yacCopy(s_attribute_value);
                                          }
                                       }
                                    }
                                    else
                                    {
                                       delete s_attribute_value; // closing tags may not have attributes
                                    }
                                    s_attribute_value = NULL;
                                 }
                              }
                        } // skipToWordStart attribute value
                     } // skipToExpectedChar(=)
                     delete s_attribute_name;
                     s_attribute_name = NULL;
                  } // s_attribute_name
            } // if b_nextat
            else
            {
               // ---- all attributes parsed
            }
         } // while b_nextat

         // ---- element parsed ----
         sSI j = indexOfChar('/');
         if(-1 != j)
         {
            ////if(is_closing)
            ////parseError(TKS_XMLParser_ERRCLOSECLOSINGELEMENT, 0)
            //tkscript->printf("   ^^element structure requires no closing element.\n");
            skipTo(j + source_off + 1u);
         }
         else
         {
            if(!is_closing)
            {
               tree.first_inner = YAC_TRUE;
               if(!stacki)
               {
                  parseError(TKS_XMLParser_ERRSTACKUNDERFLOW, "stack underflow (push).");
                  goto SevereParserException_thrown;
               }
               stack[--stacki] = tree.c; // save current TreeNode and begin inner nodes
               ////printf("xxx push stacki=%d tree.c=%p name=\"%s\"\n", stacki, tree.c, tree.c->name.chars);
            }
         }

         if(!skipToExpectedChar('>'))
            parseError(TKS_XMLParser_ERRPREMATUREENDOFELEMENT, "error while looking for '>' end of element.");
         else
         {
            skipForward(1);
            // ---- skipped to end of element
            // ---- now grab flow text until occurence of next '<'
            YAC_String *ft = scanFlowText();
            if(ft)
            {
               if(!ft->isBlank())
               {
                  ft->resubstEscapeSeq2();
#if 0
                  // 11Jan2020: must now be done by caller
                  //             (the replacement code is buggy, incomplete and also redundant (see String.replace()))
                  // ---- now replace known entities in ft
                  YAC_String *ft_ent = NULL;
                  sSI idx  = 0;
                  sSI idxe = -1;
                  sSI ft_ent_off = 0;
                  sSI ft_off = 0;
                  while(-1 != idx)
                  {
                     idx=ft->indexOf('&', idxe + 1);
                     if(-1 != idx)
                     {
                        idxe = ft->indexOf(';', idx);
                        if(-1 != idxe)
                        {
                           // ---- create view into string 'ft'
                           s_entity.chars  = ft->chars + idx;
                           s_entity.length = (1 + 1 + idxe - idx); // extra 1 for 'null' (which is actually not there)
                           // ---- tokenize string
                           sUI tokid = xml_entity_tree_getIDByString(&s_entity);
                           char c_repl;
                           switch(tokid)
                           {
                              default:              c_repl = 0;   break;
                              case XML_ENTITY_NBSP: c_repl = ' '; break;
                              case XML_ENTITY_LT:   c_repl = '<'; break;
                              case XML_ENTITY_GT:   c_repl = '>'; break;
                              case XML_ENTITY_AMP:  c_repl = '&'; break;
                           }
                           if(c_repl)
                           {
                              if(!ft_ent)
                              {
                                 ft_ent = YAC_New_String();
                                 ft_ent->alloc(ft->length);
                              }
                              // ---- copy chars until start of entity
                              while(ft_off < idx)
                              {
                                 ft_ent->chars[ft_ent_off++] = ft->chars[ft_off++];
                              }
                              ft_ent->chars[ft_ent_off++] = c_repl;
                           }
                           ft_off = idx = idxe + 1;
                        }
                        else
                        {
                           // ---- unterminated &entity
                           parseError(TKS_XMLParser_ERRUNTERMINATEDENTITY, 0);
                           idx = -1;
                        }
                     }
                  }
                  if(ft_ent)
                  {
                     // ---- copy chars until end of original string
                     if(((sUI)ft_off) < ft->length)
                     {
                        while(((sUI)ft_off) < ft->length)
                        {
                           ft_ent->chars[ft_ent_off++] = ft->chars[ft_off++];
                        }
                     }
                     else
                     {
                          ft_ent->chars[ft_ent_off++] = 0;
                     }
                     ft_ent->length = ft_ent_off;
                     ft_ent->key    = YAC_LOSTKEY;

                     // ---- use substituted string
                     delete ft;
                     ft = ft_ent;
                  }
#endif
                  TKS_CachedObject *co = attributes->varcache.createStringEntry(&ft_name, 1, ft, 1);
                  if(NULL == co)
                  {
                     parseError(TKS_XMLParser_ERRCREATINGFLOWTEXTATTRIBUTE, "error creating flow text <> hashtable entry.");
                  }
               }
               else
               {
                  delete ft;
               }
            }
            // else: >< encountered
         }


      } // if s_element
      //else
      //{
         // ---- all elements parsed
         //tkscript->printf("...all elements parsed.\n");
      //}
   } // while b_nextel

   // ---- structure parsed
   if(!error_code)
   {
      YAC_RETO(tree.root, (NULL != tree.root));
      clearFlowText();
      clearLastWord();
      tree.root = NULL;
      tree.c    = NULL;
      tree.t    = NULL;
      return;
   }
SevereParserException_thrown:
   YAC_RETO(0,0);
   if(tree.root)
   {
      delete tree.root;
      tree.root = NULL;
   }
   tree.c = NULL;
   tree.t = NULL;
   clearFlowText();
   clearLastWord();
   return;
}

void TKS_XMLParser::addFlowText(YAC_String *_s) {
   if(!flow_text)
      flow_text = YAC_New_String();
   flow_text->append(_s);
}

void TKS_XMLParser::clearFlowText(void) {
   if(flow_text)
   {
      delete flow_text;
      flow_text = NULL;
   }
}

void TKS_XMLParser::clearLastWord(void) {
   if(last_word)
   {
      delete last_word;
      last_word = NULL;
   }
}

YAC_String *TKS_XMLParser::unlinkFlowText(void) {
   YAC_String *t = flow_text;
   flow_text = NULL;
   return t;
}

YAC_String *TKS_XMLParser::unlinkLastWord(void) {
   YAC_String *t = last_word;
   last_word = NULL;
   return t;
}


void TKS_XMLParser::appendElementNode(void) {
   // ---- begin new element ----
   tree.t = (YAC_TreeNode*)YAC_NEW_CORE(YAC_CLID_TREENODE);
   tree.t->parent = tree.c;
   if(!tree.root)
   {
      tree.root = tree.t;
   }
   if(tree.first_inner)
   {
      tree.first_inner = YAC_FALSE;
      if(tree.c)
         tree.c->right = tree.t; // branch into new list
   }
   else
   {
      if(tree.c)
         tree.c->left = tree.t; // add to list
   }
   tree.c = tree.t;
   attributes = (YAC_HashTable*)YAC_NEW_CORE(YAC_CLID_HASHTABLE);
   tree.c->initObject(attributes, 1);
   tree.c->name.refCopy(last_word);
}

void TKS_XMLParser::printChars(sUI _start, sUI _end, sUI _mark) {
#ifdef TKS_DCON
   if(_start > source.length)
      _start = 0u;
   if(_end > source.length)
      _end = source.length;
   // // sBool mark=0;
   sU32 *lineptr = lines.elements;
   sUI linenr = 0u;
   while(_start >= *lineptr)
   {
      lineptr++;
      linenr++;
   }
   Dprintf("[---]\tline %4i:  ", linenr);
   sUI i = _start;
   for(; (i < _end) && (i < source.length); i++)
   {
      if(i >= *lineptr)
      {
         linenr++;
         if( (_mark >= *lineptr) && (_mark < lineptr[1]) )
         {
            Dprintf("\n[---]\t****>%4i:  ", linenr);
         }
         else
         {
            Dprintf("\n[---]\tline %4i:  ", linenr);
         }
         lineptr++;
      }
      if(i == _mark)
      {
         Dprintf("****");
      }
      if( ((char)source.chars[i]) != '\n')
      {
         Dprintf("%c", (char)source.chars[i]);
      }
   }
   Dprintf("\n");
#endif // TKS_DCON
}

const char *TKS_XMLParser::GetErrorStringByCode(sUI _code) {
   switch(_code)
   {
      default: return "unknown error (oops)";
      case TKS_XMLParser_ERRNOERROR:                      return "ERRNOERROR";
      case TKS_XMLParser_ERRALLOWEDNUMBEROFLINESEXCEEDED: return "number of lines exceeded";
      case TKS_XMLParser_ERRCANNOTALLOCLINES:             return "cannot alloc lines";
      case TKS_XMLParser_ERRTOOMANYLINES:                 return "too many lines";
      case TKS_XMLParser_ERREMPTYSOURCE:                  return "empty source";
      case TKS_XMLParser_ERRPREMATUREENDOFQUOTE:          return "premature end of quoted string";
      case TKS_XMLParser_ERRATTRIBUTEASSIGNEXPECTED:      return "attribute assign expected";
      case TKS_XMLParser_ERRPARSINGATTRIBUTENAME:         return "error parsing attribute name";
      case TKS_XMLParser_ERRCREATINGATTRIBUTE:            return "error creating attribute";
      case TKS_XMLParser_ERRCREATINGFLOWTEXTATTRIBUTE:    return "error creating flowtext attribute";
      case TKS_XMLParser_ERRPREMATUREENDOFFLOWTEXT:       return "premature end of flow text";
      case TKS_XMLParser_ERRSTRAYENDELEMENT:              return "stray '>' found while parsing flow text";
      case TKS_XMLParser_ERRATTRIBUTEVALUEEXPECTED:       return "attribute value expected";
      case TKS_XMLParser_ERRPARSINGATTRIBUTEVALUE:        return "error parsing attribute value";
      case TKS_XMLParser_ERRPREMATUREENDOFELEMENT:        return "premature end of element";
      case TKS_XMLParser_ERRELEMENTNESTING:               return "wrong element nesting";
      case TKS_XMLParser_ERRPARSINGELEMENTNAME:           return "error parsing element name";
      case TKS_XMLParser_ERRPARSINGMETAELEMENT:           return "error parsing meta element";
      case TKS_XMLParser_ERRPARSINGCOMMENT:               return "error parsing comment";
      case TKS_XMLParser_ERRPARSINGHEADER:                return "error parsing header";
      case TKS_XMLParser_ERRSTACKUNDERFLOW:               return "stack underflow";
      case TKS_XMLParser_ERRSTACKOVERFLOW:                return "stack overflow";
      case TKS_XMLParser_ERRUNTERMINATEDENTITY:           return "unterminated entity";
   }
}

#ifdef TKS_EXPORT_ENTITY_SCANNER
sUI xml_entity_tree_getIDByString(YAC_String *_s) {
   return entity_tree.getEntityIDByString(_s);
}
void TKS_XMLParser::InitEntityTree(void) {
   entity_tree.addEntity("&nbsp;", XML_ENTITY_NBSP);
   entity_tree.addEntity("&lt;",   XML_ENTITY_LT);
   entity_tree.addEntity("&gt;",   XML_ENTITY_GT);
   entity_tree.addEntity("&amp;",  XML_ENTITY_AMP);

   entity_tree.exportToC("autogen_xml_entitytree_lookup.cpp", "xml_entity_tree_getIDByString");
}
#else
#include "autogen_xml_entitytree_lookup.cpp"
#endif
