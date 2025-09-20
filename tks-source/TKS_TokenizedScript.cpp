/// TKS_TokenizedScript.cpp
///
/// (c) 2001-2025 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///

#include "tks.h"

#include "TKS_Token.h"
#include "TKS_TokenTable.h"
#include "TKS_TokenizedScript.h"

#include "EntityTree.h"

#define TKS_ISNUM(a) ((a)>=48&&(a)<=57)


#ifdef TKS_EXPORT_ENTITY_SCANNER
static EntityTree entity_tree;
#endif

//
// Note: this cache is re-used by all script modules.
//       it stores the (temporary) lines/strings/tokens arrays
//       before optimization
//
void *TKS_TokenizedScript::tsl_cache = NULL;
sUI   TKS_TokenizedScript::tsl_cache_size = 0;


TKS_TokenizedScript::TKS_TokenizedScript(void) {
   source               = NULL;
   source_off           = 0u;
   source_line          = 0u;
   token_entries        = tks_lang_tokens;
   num_token_entries    = TOKEN_NUMTOKENS;
   b_xml                = YAC_FALSE;
   tokens.elements      = NULL;
   tokens.num_elements  = 0u;
   tokens.max_elements  = 0u;
   strings.elements     = NULL;
   strings.num_elements = 0u;
   strings.max_elements = 0u;
   lines.elements       = NULL;
   lines.num_elements   = 0u;
   lines.max_elements   = 0u;
   b_own_tsl            = YAC_FALSE;
}

TKS_TokenizedScript::~TKS_TokenizedScript() {
   destructStrings();

   if(b_own_tsl)
   {
      delete [] tokens.elements;
      free(strings.elements);
      delete [] lines.elements;
   }
}

void TKS_TokenizedScript::AllocTSLCache(void) {
   tsl_cache_size =
      (sizeof(sU16)       * TKS_SCANNER_MAX_TOKENS) +
      (sizeof(YAC_String) * TKS_SCANNER_MAX_STRINGS) +
      (sizeof(sU32)       * TKS_SCANNER_MAX_LINES)
      ;
   tsl_cache = ::malloc(tsl_cache_size);
}

void TKS_TokenizedScript::FreeTSLCache(void) {
   if(NULL != tsl_cache)
   {
      ::free(tsl_cache);
      tsl_cache = NULL;
   }
}

void TKS_TokenizedScript::initTSLFromCache(void) {

   if(NULL == tsl_cache)
      AllocTSLCache();

   memset(tsl_cache, 0, tsl_cache_size);

   tokens.elements = (sU16*) ( ((sU8*)tsl_cache) + 0 );
   tokens.max_elements = TKS_SCANNER_MAX_TOKENS;
   tokens.num_elements = 0;

   strings.elements = (YAC_String*) ( ((sU8*)tsl_cache) + sizeof(sU16)*TKS_SCANNER_MAX_TOKENS );
   strings.max_elements = TKS_SCANNER_MAX_STRINGS;
   strings.num_elements = 0;

   lines.elements = (sU32*) ( ((sU8*)strings.elements) + sizeof(YAC_String)*TKS_SCANNER_MAX_STRINGS);
   lines.max_elements = TKS_SCANNER_MAX_LINES;
   lines.num_elements = 0;
}

void TKS_TokenizedScript::destructStrings(void) {
   for(sUI i = 0u; i < strings.num_elements; i++)
   {
      YAC_String *s = &strings.elements[i];
      s->YAC_String::~YAC_String();
   }
}

void TKS_TokenizedScript::skipSpaces(void) {
   sChar *s = (sChar*) source->chars + source_off;
   for(;;)
   {
      switch(*s++)
      {
         case 0:
            source_off++;
            return;

         case '\n': addLine();
         case '\014':
         case '\t':
         case ' ':
            source_off++;
            break;

         default: return;
      }
   }
}

void TKS_TokenizedScript::addToken(sU16 _tok) {
   if(tokens.num_elements < tokens.max_elements)
   {
      tokens.elements[tokens.num_elements++] = _tok;
   }
   else
   {
      Dprintf("[---] fatal error: maximum number of tokens exceeded (%d)\n", tokens.max_elements);
      tkscript->handleFatalError();
   }
}

void TKS_TokenizedScript::addLine(void) {
   source_line++;
   if(lines.num_elements < lines.max_elements)
   {
      lines.elements[lines.num_elements++] = tokens.num_elements; // next token starts new line
   }
   else
   {
      tkscript->printf("[---] maximum number of lines (%d) exceeded.\n", lines.max_elements);
      tkscript->handleFatalError();
   }
}

sUI TKS_TokenizedScript::nextToken(void) {
   sUI ret = ERRTKS;
   skipSpaces(); // ---- adjust input pointer to first non-blank char
   sSI chleft = source->length-source_off;
   if(chleft < 1)
      return ERRTKSEOF;
   sSI i;
   sSI j;
   sSI k;
   sChar *s;
   sSI off;
   off = 0;

   s = (sChar*)source->chars+source_off;
   k = 0; // let k be the length of the unknown string
   sSI   quot_mode = 0;   // "hello there"
   sSI   dquot_mode = 0;  // 'trace "hello there !";'
   sSI   quot_flag = 0;   // 1=\"\", 2=\'\'
   sBool dquot_first = 0;
   sBool bcharconst = 0;
   sBool bfloat = 1;
   sBool bfloatpm = 0;
   sBool bfloate = 0;  // 'e' / 'E' found
   sBool bfloatd = 0; // decimal point found
   sBool bfloatn = 0; // number before any other character found

   if('~' == s[0] && ';' == s[1])
   {
      // ~; region quote string constant that appears as regular script code in editors ~;

      // find closing ~; and return region as string constant
      sChar cp = 0u;
      sBool bLineComment = 0;  // //
      sBool bComment = 0;      // /*..*/
      for(i = 2; i <= chleft; i++)
      {
         sChar c = s[i];
         // tkscript->printf("xxx nextToken: find region end i=%d c='%c'(%u)\n", i, c, c);
         switch(c)
         {
            default:
               break;
            case '\n':
               addLine();
               bLineComment = 0;
               break;
            case '/':
               if(!(quot_mode | dquot_first | bLineComment))
               {
                  if('/' == cp && !bComment)
                     bLineComment = 1;
                  else if('*' == cp)
                     bComment = 0;
               }
               break;
            case '*':
               if(!(quot_mode | dquot_first | bLineComment | bComment))
               {
                  if('/' == cp)
                     bComment = 1;
               }
               break;

               // escape \[ntrabvef"'] within string. e.g. "hello\nworld" => "hello\\nworld"
               //   see YAC_String::substEscapeSeq()
            case 16: // \t
            case 17: // \n
            case 18: // \"
            case 19: // \'
            case 21: // \r
            case 22: // \b
            case 23: // \a
            case 24: // \f
            case 25: // \v
            case 26: // \e
               if(15 == cp)  // 15 is prefix for escaped characters (see substEscapeSeq())
               {
                  // resubstitute escaped character
                  // str_const.chars[str_const.length-2] = '\\';  // replace 15 by '\'
                  s[i-1] = '\\';  // replace 15 by '\'
                  // tkscript->printf("xxx escape ntre\n");
                  switch(c)
                  {
                     default: break;
                     case 16: c = (sChar)'t';   break; // \t
                     case 17: c = (sChar)'n';   break; // \n
                     case 18: c = (sChar)'\"';  break; // \"
                     case 19: c = (sChar)'\'';  break; // \'
                     case 21: c = (sChar)'r';   break; // \r
                     case 22: c = (sChar)'b';   break; // \b
                     case 23: c = (sChar)'a';   break; // \a
                     case 24: c = (sChar)'f';   break; // \f
                     case 25: c = (sChar)'v';   break; // \v
                     case 26: c = (sChar)'e';   break; // \e
                  }
                  s[i] = c;
               }
               break;
            case '\'':
               if(!(quot_mode | dquot_first | bLineComment | bComment))
                  dquot_first = 1;
               else if(dquot_first)
                  dquot_first = 0;
               break;
            case '\"':
               if(!(dquot_first | quot_mode | bLineComment | bComment))
                  quot_mode = 1;
               else if(quot_mode)
                  quot_mode = 0;
               break;
            case ';':
               if(!(quot_mode | dquot_first | bLineComment | bComment))
               {
                  if('~' == cp)
                  {
                     // found end of ~;region~;
                     k = i - 1/*skip first trailing ;*/ - 2/*skip leading ~;*/;
                     source->substring(&str_const, source_off+2, k);
                     // str_const.chars[--str_const.length-1] = 0;  // replace ~ by ASCIIZ
                     ret = TOKEN_STRINGVAL;
                     str_const.setQuotFlag(3);
                     // tkscript->printf("xxx nextToken: region quote end i=%d data=>>%s<< quotFlag=%d\n", i, (const char*)str_const.chars, str_const.getQuotFlag());
                     source_off += i + 1;
                     addToken((sU16) ret);
                     addToken((sU16) addUniqueString(&str_const));
                     return ret;
                  }
               }
               break;
         }
         cp = c;
      }
      return ERRTKSEOFQUOTR;
   }

   //Dpcscan_startSample(findDelim);
   for(i=0; !k && i<=(sSI)chleft; i++)
   {
      switch(s[i])
      {
         case 'e':
         case 'E':
            bfloat = bfloate = (i>0) && bfloatn && bfloat && !bfloatpm && !bfloate;
            break;
         case '+':
         case '-':
            if(!bfloate)
            {
               if(! ((quot_mode&&(!dquot_first))||(dquot_mode&&dquot_first)))
               {
                  k = i ? i : 1;
                  bfloat = 0;
               }
            }
            else
            {
               bfloatpm = 1; // allow [+-]<number> or <number>[+-]e<number>
            }
            break;
         case '0':
         case '1':
         case '2':
         case '3':
         case '4':
         case '5':
         case '6':
         case '7':
         case '8':
         case '9':
            bfloatn = bfloat;
            break;
         case 'f':
            bfloat = bfloatn;
            break;
         default:
            bfloat = 0;
            break;

         case '.':
            //
            if(!bfloatn || bfloatd)
            {
               if(!(quot_mode || dquot_mode))
               {
                  k =i - off;
                  if(!k)
                  {
                     k = 1;
                  }
                  bfloat = 0;
               }
            }
            else
            {
               bfloatd = 1;
            }
            break;
         case '\'': // enter/leave double quote mode?
            bfloat=0;
            dquot_mode=1-dquot_mode;
            if(quot_mode&&(!dquot_first)) // "trace 'hello, world.';" style
            {
               dquot_first=0;// xxx
               //off=i+1;
            }
            else
            {
               if((s[i+1]==0x0f)&&(s[i+3]=='\''))
               {
                  dquot_mode=0; // '\'' 0x0F <esccode> '\'' magic sequence
                  i+=3;
               }
               else
               {
                  dquot_first=1;
                  if(!dquot_mode) // finished?
                  {
                     k=i-off;
                     if(k>1)
                     {
                        quot_flag=2; // regular 'test' char sequence
                     }
                     else
                     {
                        quot_flag=0; // ASCII constant 'A'
                        bcharconst=1;
                     }
                  }
                  else
                  {
                     // 'trace "hello, world.";' style
                     if(s[i+1]=='\'')
                     {
                        dquot_mode=0;
                        // empty string
                        k=0xDEAF;
                     }
                     else
                        off=i+1; // set start
                  }
               }
            }
            break;
         case '\"': // enter/leave quote mode?
            bfloat=0;
            quot_mode=1-quot_mode;
            if(dquot_mode&&dquot_first)
            {

               //off=i+1; // set start
            }
            else
            {
               if(!quot_mode)
               {
                  k=i-off;
                  quot_flag=1;
               }
               else
               {
                  dquot_first=0;
                  if(s[i+1]=='\"')
                  {
                     // empty string
                     k=0xDEAF;
                  }
                  else off=i+1;
               }
            }
            break;
         case '\n': // blank
            if(quot_mode || dquot_mode) addLine();
         case '\t':
         case '\014':
         case ' ':
         case '%':
         case '/':
         case '*':
         case '#':
         case '!':  // unknown strings can be
         case '&':  // int, float or string
         case '|':  // constants/variables
         case '^':  // they
         case '=':  // are
         case '(':  // indentified
         case ')':  //
         case ';':  // in pass2 (parser)
         case ',':
         case '[':
         case ']':
         case '{':
         case '}':
         case '<':
         case '>':
         case '?':
         case '~':
         case ':':
            bfloat=0;
            if( (quot_mode && (!dquot_first)) || (dquot_mode && dquot_first) )
               break;
         case 0:
            if( (quot_mode && (!dquot_first)) || (dquot_mode && dquot_first))
               return ERRTKSEOFQUOT;
            if(i==0)
            {
               if(s[0]!='#')
               {
                  k=1;     // end of buffer reached
               }
               else
               {
                  if((s[1]=='[')||(s[1]=='('))
                     k=2;
               }
            }
            else
               k=i;

            break;
      }
   } // for..

   //Dpcscan_stopSample(findDelim);

   if( k != 0xDEAF && k > 0)
   {
      if(k==1)
      {
         // ---find end of +=, /*, */, // token
         sU8 cc=source->chars[source_off];
         sU8 cn=source->chars[source_off+1];
         sU8 cnn=source->chars[source_off+2];
         switch(cc)
         {
            case '.':
               switch(cn)
               {
                  case '.': k++; break;
                     break;
               }
               break;
            case ':':
               switch(cn)
               {
                  case ':': k++; break;
                     break;
               }
               break;
            case ';':
               switch(cn)
               {
                  case ';': k++; break;
                     break;
               }
               break;
            case '|':
            case '&':
            case '^':
               k+=(cn==cc)||(cn=='=');
               break;
            case '%':
               switch(cn)
               {
                  case '=': k++; break;
                     break;
               }
               break;
            case '/':
               switch(cn)
               {
                  case '*':
                  case '/':
                  case '=': k++; break;
                     break;
               }
               break;
            case '*':
               switch(cn)
               {
                  case '/':
                  case '=': k++; break;
                     break;
               }
               break;
            case '-':
               switch(cn)
               {
                  case '-':
                  case '>':
                  case '=': k++; break;
               }
               break;
            case '+':
               switch(cn)
               {
                  case '+':
                  case '=': k++; break;
               }
               break;
            case '!':
               switch(cn)
               {
                  case '=': k++; break;
               }
               break;
            case '=':
               switch(cn)
               {
                  case '=':
                  case '>':
                     k++;
                     break;
               }
               break;
            case '<':
               switch(cn)
               {
                  case '=':
                     if('>' == cnn)
                     {
                        k+=2;
                     }
                     else
                     {
                        k++;
                     }
                     break;
                  case '<': if('=' == cnn) k+=2; else k++; break;
               }
               break;
            case '>':
               switch(cn)
               {
                  case '=': k++; break;
                  case '>': if(source->chars[source_off+2]=='=') k+=2; else k++; break;
               }
               break;
         }
      }

      //Dpcscan_startSample(str_const_substring);
      source->substring(&str_const, source_off+off, k);
      //Dpcscan_stopSample(str_const_substring);

      if(off)
      {
         //tkscript->printf("+++ str_const=\"%s\"\n", (sChar*)str_const.chars);
         ret = TOKEN_STRINGVAL;
         if(bcharconst)
         {
            sU8 cconst = str_const.chars[0];
            str_const.copy("              ");
            str_const.printf("%i", cconst);
         }
      }
      else
      {
         //clogf("+++ k=%i unknown string=%s\n", k, (sChar*)str_const.chars);
         sUI tok_id;
         if(bfloat)
         {
            tok_id = 0;
         }
         else
         {
            //Dpcscan_startSample(entitytree_getIDByString);
            tok_id = tks_entity_tree_getIDByString(&str_const);
            //Dpcscan_stopSample(entitytree_getIDByString);
         }
         if(tok_id)
         {
            //clogf("+++ found token %i \"%s\"\n", i, (sChar*)token_entries[i].chars);
            source_off+=k;
            if(tok_id==TOKEN_LINECOMMENT)
            {
               s+=k; // adjust input pointer to next line
               for(j=0; j<(sSI)(source->length-source_off); j++)
               {
                  switch(s[j])
                  {
                     case '\n':
                     case 0:
                        source_off+=j;
                        return (sUI)tok_id;
                  }
               }
               return ERRTKSEOFLINECOMMENT;
            }
            if(tok_id==TOKEN_BLOCKCOMMENT)
            {
               s+=k; // adjust input pointer to end of block comment
               for(j=0; j<(sSI)(source->length-source_off); j++)
               {
                  switch(s[j])
                  {
                     case '\n':
                        addLine();
                        break;
                     case '*':
                        if(s[j+1]=='/')
                        {
                           source_off+=j+2;
                           return (sUI)i;
                        }
                        break;
                     case 0:
                        return ERRTKSEOFBLOCKCOMMENT;
                  }
               }
               return ERRTKSEOSBLOCKCOMMENT;
            }

            // ---- regular token ----
            addToken((sU16)tok_id);
            return (sUI)tok_id;
         }
         else
         {
            ret=TOKEN_VAR;
         }
      }
      // ---- escaped string \"\" or unidentified identifier
      str_const.setQuotFlag(quot_flag);  // 1=\"\", 2=\'\'
      source_off+=k + ((off!=0)?(off+1):0);
      addToken((sU16) ret);
      addToken((sU16) addUniqueString(&str_const));
      return ret;
   }
   else
   {
      // empty string
      source_off+=2;
      str_const.empty();
      str_const.setQuotFlag(1);
      addToken(TOKEN_STRINGVAL);
      addToken((sU16) addUniqueString(&str_const));
      return TOKEN_STRINGVAL;
   }
   // unreachable
}


sUI TKS_TokenizedScript::addUniqueString(YAC_String *_s) {
   Dpcscan_startSample(addUniqueString);
#ifndef DX_SCANNER_FASTADDUNIQUESTRING
   sUI i;
   sSI oQuotFlag = _s->getQuotFlag();
   for(i=0; i<strings.num_elements; i++)
   {
      if(strings.elements[i].compare(_s))
      {
         if(strings.elements[i].getQuotFlag() == oQuotFlag)
         {
            Dpcscan_stopSample(addUniqueString);
            return i;
         }
      }
   }
#endif
   //////strings.add(_s);
   // TODO: buffer overrun..
   if(strings.num_elements == strings.max_elements)
   {
      Dprintf("[---] maximum number of strings exceeded (%d)\n", strings.num_elements);
      tkscript->handleFatalError();
      return 0;
   }
   YAC_String *d = (YAC_String*) &strings.elements[strings.num_elements++];
   new(d)YAC_String();
   d->refCopy(_s);
   Dpcscan_stopSample(addUniqueString);
   return strings.num_elements-1;
}

sUI TKS_TokenizedScript::scan(YAC_String *_s, sBool _bRefCopy,
                              sBool _xml, sSI _max_tokens, sSI _max_strings, sS16 _max_lines
                              )
{
   Dpcscan_startSample(scanPrepare);
   b_xml=_xml;

   YAC_String source_script;
   Dpcscan_startSample(scanPrepare1);
   if(_bRefCopy)
   {
      // Grab chars from _s
      source_script.refCopy(_s);
   }
   else
   {
      // Copy chars (temp modules)
      source_script.yacCopy(_s);
   }
   source_script.removeCR();
   Dpcscan_stopSample(scanPrepare1);

   Dpcscan_startSample(scanPrepare2);
   source_script.substEscapeSeq();
   source_script.replace("\' \'", "32 ");
   source_script.replace("\'\"\'", "34 ");
   Dpcscan_stopSample(scanPrepare2);
   /*sU8 shi;
     for(shi=32; shi<127; shi++)
     {
     YAC_String replstr;
     YAC_String replstrd;
     replstrd.copy("                    ");
     replstr.copy("\' \'");
     replstr.chars[1]=shi;
     replstrd.printf("%i", shi);
     source_script.replace(&replstr, &replstrd);
     }*/


   sUI ret=0;
   sUI ctok;
   sSI i;
   sSI open_brackets=0;
   sSI open_arrays=0;
   sSI open_blocks=0;
   if(source_script.isBlank())
   {
      Dprintf("[---] scan: String is empty.\n");
      return ERRTKS;
   }

   Dpcscan_startSample(scanPrepare3);
   initTSLFromCache();
   Dpcscan_stopSample(scanPrepare3);

   Dpcscan_stopSample(scanPrepare);

   if(tokens.elements && strings.elements && lines.elements)
   {
      source = &source_script;
      source_off = 0;
      source_line = 0;
      addLine(); // begin first line with token 0
      Dpcscan_startSample(nextToken);
      while( (ctok=nextToken())<ERRTKS )
      {
         Dpcscan_stopSample(nextToken);
         Dpcscan_startSample(nextToken);
         // CLOG("!!! got token "<<ctok<<"\n");
      }
      Dpcscan_stopSample(nextToken);
      ret=ctok;
      switch(ctok)
      {
         case ERRTKSEOF:
            //dcon.printf("[+++] found EOF.\n");
            break;
         case ERRTKSEOFLINECOMMENT:
            Dprintf("[---] EOF found while reading line comment.\n");
            break;
         case ERRTKSEOFBLOCKCOMMENT:
            Dprintf("[---] EOF found while looking for end of block comment.\n");
            ////tokens.empty();
            ////strings.empty();
            ////lines.empty();
            tokens.num_elements=0;
            strings.num_elements=0;
            lines.num_elements=0;
            break;
         case ERRTKSEOSBLOCKCOMMENT:
            Dprintf("[---] end of string reached while looking for end of block comment.\n");
            tokens.num_elements=0;
            strings.num_elements=0;
            lines.num_elements=0;
            break;
         case ERRTKSEOFQUOT:
            Dprintf("[---] reached EOF while looking for matching >\"<\n");
            tokens.num_elements=0;
            strings.num_elements=0;
            lines.num_elements=0;
            break;
         case ERRTKSEOFQUOTR:
            Dprintf("[---] reached EOF while looking for matching >~;<\n");
            tokens.num_elements=0;
            strings.num_elements=0;
            lines.num_elements=0;
            break;
      }
      if(tokens.num_elements)
      {
         lines.elements[lines.num_elements++] = tokens.max_elements;
         sU32 *lineptr = lines.elements;
         sU32 linenr = 1;
         (void)linenr;
         lineptr++;
         for(i=0; i<(sSI)tokens.num_elements; i++)
         {
            if(tokens.elements[i]==TOKEN_ENDBLOCK)
               open_blocks--;
            if(tokens.elements[i]==TOKEN_ENDEXPR)
               open_brackets--;
            if(tokens.elements[i]==TOKEN_ENDARRAY)
               open_arrays--;

            if((sUI)i > *lineptr)
            {
               lineptr++;
               linenr++;
            }

            if(tokens.elements[i]==TOKEN_BEGBLOCK)
               open_blocks++;
            if(tokens.elements[i]==TOKEN_HASHINI)
               open_arrays++;
            if(tokens.elements[i]==TOKEN_BEGEXPR)
               open_brackets++;
            if(tokens.elements[i]==TOKEN_VALUE)
               open_brackets++;
            if(tokens.elements[i]==TOKEN_BEGARRAY)
               open_arrays++;

            if(tokens.elements[i]==TOKEN_VAR||
               tokens.elements[i]==TOKEN_INTVAL||
               tokens.elements[i]==TOKEN_FLOATVAL||
               tokens.elements[i]==TOKEN_STRINGVAL)
            {
               i++;
            }
         }
         if(open_arrays)
         {
            Dprintf("\n[---] warning: missing %i >]<.\n", open_arrays);
         }
         if(open_brackets)
         {
            Dprintf("\n[---] warning: missing %i >)<.\n", open_brackets);
         }
         if(open_blocks)
         {
            Dprintf("\n[---] warning: missing %i >}<.\n", open_blocks);
         }

         if(tkscript->yacGetDebugLevel()>=90)
         {
            Dprintf("[+++] scanned %i tokens and %i symbols in %i %s (%i chars).\n",
               tokens.num_elements,
               strings.num_elements,
               (source_line?source_line:source_line+1),
               (char*)(!source_line?"line":"lines"),
               source_off);
         }

         ret = tokens.num_elements;
      }
   }
   else
   {
      Dprintf("[---] TKS_TokenizedScript::scan memory allocation failed.\n");
      ret=ERRTKS;
   }
   return ret;
}

void TKS_TokenizedScript::writeTokenLine(sUI _linenr, YAC_String *_d) {
   if(_linenr < lines.num_elements)
   {
      sUI start = lines.elements[_linenr];
      sUI end;
      if(_linenr == (lines.num_elements-1))
      {
         end = tokens.num_elements;
      }
      else
      {
         end = lines.elements[_linenr+1];
      }

      sBool isString = 0;
      while(start < end)
      {
         sU16 tok = tokens.elements[start++];
         if(isString)
         {
            isString = 0;
            _d->append(&strings.elements[tok]);
            _d->append(" ");
         }
         else if(tok<=TOKEN_STRINGVAL)
         {
            isString = 1;
         }
         else
         {
            _d->append(tks_get_token_name(tok));
            _d->append(" ");
         }
      }
   }
}

void TKS_TokenizedScript::printTokens(sUI _start, sUI _end, sUI _mark, sUI _lineOffset) {
#ifdef TKS_DCON
   //static char buf[]="\033[30m";
   sUI i;
   sBool mark;
   sU32 *lineptr = lines.elements;
   sS32 linenr = -sS32(_lineOffset);
   while(_start >= *lineptr)
   {
      lineptr++;
      linenr++;
   }

   Dprintf("[---] line %4i:  ", linenr);

   i = _start;
   sBool zap_blanklines=0;
   for(; i < _end && (i < (sUI)tokens.num_elements); i++)
   {
      if(i >= *lineptr)
      {
         linenr++;
         if( (_mark >= *lineptr) && (_mark < lineptr[1]) )
         {
            Dprintf("\n[---] ****>%4i:  ", linenr);
         }
         else
         {
            Dprintf("\n[---] line %4i:  ", linenr);
         }
         zap_blanklines = 1;
         lineptr++;
         while(i >= *lineptr)
         {
            linenr++;
            if( (_mark >= *lineptr) && (_mark < lineptr[1]) )
            {
               Dprintf("\n[---] ****>%4i:  ", linenr);
            }
            else if(!zap_blanklines)
            {
               Dprintf("\n[---] line %4i:  ", linenr);
               zap_blanklines = 1;
            }
            lineptr++;
         }
      }
      mark = 0;
      sUI tei = tokens.elements[i];
      switch(tei)
      {
         case TOKEN_BEGARRAY:
         case TOKEN_ENDARRAY:
         case TOKEN_BEGEXPR:
         case TOKEN_ENDEXPR:
         case TOKEN_BEGBLOCK:
         case TOKEN_ENDBLOCK:
         default:
            mark = (i == _mark);
            if(mark)
            {
               Dprintf("***%s***", (const char*) tks_get_token_name((sU16)tei));
            }
            else
            {
               Dprintf("%s ", (const char*) tks_get_token_name((sU16)tei));
            }
            break;
         case TOKEN_VAR:
         case TOKEN_INTVAL:
         case TOKEN_FLOATVAL:
         case TOKEN_STRINGVAL:
            mark = (i == _mark || (i+1) == _mark);
            i++;
            tei = tokens.elements[i];
            YAC_String*t;
            if(tei < strings.num_elements)
            {
               t = &strings.elements[tei];
               if(mark)
               {
                  if(t->getQuotFlag())
                  {
                     Dprintf("***\"%s\"*** ", (sChar*)t->chars);
                  }
                  else
                  {
                     Dprintf("***%s*** ", (sChar*)t->chars);
                  }
               }
               else
               {
                  if(t->getQuotFlag())
                  {
                     Dprintf("\"%s\" ", (sChar*)t->chars);
                  }
                  else
                  {
                     Dprintf("%s ", (sChar*)t->chars);
                  }
               }
            }
            break;
      }
   }
   Dprintf("\n");
#endif // TKS_DCON
}

void TKS_TokenizedScript::optimize(void) {
   sUI i;

   // Alloc optimized tokens array
   // and merge (multi-line) strings
   if(tokens.num_elements)
   {
      sU16 lt = TOKEN_NOP;
      sU16 lsi = 0;
      sU16 *na = new sU16[tokens.num_elements]; // xxx chk alloc
      YAC_String *ns = NULL;
      sU16 *lsip = 0;
      for(i=0; i<tokens.num_elements; i++)
      {
         sU16 ct = tokens.elements[i];
         if(ct <= TOKEN_STRINGVAL)
         {
            if(lt == TOKEN_VAR)
            {
               YAC_String *cs = &strings.elements[tokens.elements[i+1]]; // get current string
               YAC_String *ls = &strings.elements[lsi]; // get last string
               if( cs->getQuotFlag() && ls->getQuotFlag() )
               {
                  if(!ns)
                  {
                     ns = &strings.elements[strings.num_elements];
                     new(ns)YAC_String();
                     *lsip = lsi = (sU16) strings.num_elements++;
                     ns->yacCopy(ls);
                  }
                  ns->append(cs);
                  // //cs->empty();
                  na[i++] = TOKEN_NOP; // discard appended string token
                  na[i]   = TOKEN_NOP;
               }
               else
               {
                  na[i++] = ct;
                  lsi = tokens.elements[i]; // index
                  lsip = &na[i];
                  na[i] = lsi;
                  ns = 0;

               }
            }
            else
            {
               na[i++] = ct;
               lsi = tokens.elements[i]; // index
               lsip = &na[i];
               na[i] = lsi;
               lt = TOKEN_VAR;
               ns = 0;
            }
         }
         else
         {
            na[i] = ct; // regular token
            lt = ct;
         }
      }
      //////delete [] tokens.elements; // note: do not delete, it's just a cache ref.
      tokens.elements     = na;
      tokens.max_elements = tokens.num_elements;
   }
   else
   {
      tokens.elements     = NULL;
      tokens.max_elements = 0u;
   }


   //Dprintf("[dbg] TokenizedScript::optimize:\n\t#strings=%d\n\t#tokens=%d\n\t#lines=%d\n", strings.num_elements, tokens.num_elements, lines.num_elements);

   // Alloc optimized string array
   if(strings.num_elements)
   {
      ////YAC_String *nsa=new YAC_String[strings.num_elements];
      YAC_String *nsa = (YAC_String*) malloc(sizeof(YAC_String)*strings.num_elements);
      for(i=0; i<strings.num_elements; i++)
      {
         new(&nsa[i])YAC_String();
         nsa[i].refCopy(&strings.elements[i]);
         // Note: no need to call d'tor since buffer is already unlinked
         //       simply decrease object counter. tsl_cache strings are batch-freed later on.
         YAC_String::string_object_counter--;
         YAC_Object::object_counter--;
      }
      // Note: do not delete, it's just a cache ref.
      ////delete [] strings.elements;
      ////free(strings.elements);
      strings.elements     = nsa;
      strings.max_elements = strings.num_elements;
   }
   else
   {
      strings.elements     = NULL;
      strings.max_elements = 0;
   }

   // Alloc optimized line array
   if(lines.num_elements)
   {
      sU32 *na = new sU32[lines.num_elements+1];
      for(i=0; i<lines.num_elements; i++)
      {
         na[i] = lines.elements[i];
      }
      // Note: do not delete, it's just a cache ref.
      ////delete [] lines.elements;
      na[i] = 0x7F00; // ??
      lines.elements     = na;
      lines.max_elements = lines.num_elements;
   }

   b_own_tsl = 1;
}

void TKS_TokenizedScript::tokensToString(YAC_String *_r) {
   sUI toki=0;
   sBool bLastWasString = 0;
   while(toki<tokens.num_elements)
   {
      sU16 tok = tokens.elements[toki++];
      if(tok<=TOKEN_STRINGVAL)
      {
         if(bLastWasString)
         {
            _r->append(" ");
         }
         else
         {
            bLastWasString=1;
         }
         // String constant
         YAC_String *sv = &strings.elements[tokens.elements[toki++]];
         switch(sv->getQuotFlag())
         {
         case 1: _r->append("\""); break;
         case 2: _r->append("\'"); break;
         }
         _r->append(sv);
         switch(sv->getQuotFlag())
         {
         case 1: _r->append("\""); break;
         case 2: _r->append("\'"); break;
         }
      }
      else
      {
         TKS_Token *t = tks_get_token(tok);
         if(bLastWasString && t->b_blank_must_follow)
         {
            _r->append(" ");
         }
         bLastWasString = 0;
         _r->append( t->chars );
         if(t->b_blank_must_follow)
            _r->append(" ");
      }
      if( (toki&31)==0 )
      {
         _r->append("\n");
      }
   }
}

// "magic" token replacements for String indices < 256
#define TOK_LOWID 224
#define TOK_LOWID_VAR (TOK_LOWID + TOKEN_VAR)
#define TOK_LOWID_INTVAL (TOK_LOWID + TOKEN_INTVAL)
#define TOK_LOWID_FLOATVAL (TOK_LOWID + TOKEN_FLOATVAL)
#define TOK_LOWID_STRINGVAL (TOK_LOWID + TOKEN_STRINGVAL)

sUI TKS_TokenizedScript::calcSize(sBool _bLineNumbers) { // xxx changes token data (TOK_LOWID!)
   sUI i;
   YAC_String *s;

   sUI r = sizeof(sU8); // Version byte

   // Token data + num_elements field
   r += sizeof(sUI);
   for(i=0; i<tokens.num_elements; i++)
   {
      sU16 tok = tokens.elements[i];
      r += sizeof(sU8);
      if(tok <= TOKEN_STRINGVAL)
      {
         // String constant
         i++;
         if(tokens.elements[i] > 255)
         {
            r += sizeof(sU16); // Add another byte for high index strings
         }
         else
         {
            tokens.elements[i-1] += TOK_LOWID;
            r += sizeof(sU8); // Low index (0..255) string
         }
      }
   }

   // Line offsets + num_elements field
   if(_bLineNumbers)
   {
      r += lines.num_elements * sizeof(sU32) + sizeof(sUI);
   }

   // String data + num_elements field
   r += sizeof(sUI);
   s = strings.elements;
   for(i=0; i<strings.num_elements; i++)
   {
      ////r += sizeof(sUI); // string length
      r += sizeof(sU8); // quot flag
      r += sizeof(sU8) * (s++->length); // string data
   }

   return r;
}


// Endian-safe dword/word writers
#define WU4(a) *_b.u8++ = (sU8) ( ((a)>>24) &255 ); \
               *_b.u8++ = (sU8) ( ((a)>>16) &255 ); \
               *_b.u8++ = (sU8) ( ((a)>> 8) &255 ); \
               *_b.u8++ = (sU8) (  (a)      &255 )
#define WU2(a) *_b.u8++ = (sU8) ( ((a)>> 8) &255 ); \
               *_b.u8++ = (sU8) (  (a)      &255 )
#define WU1(a) *_b.u8++ = (sU8) (a)
void TKS_TokenizedScript::writeTokens(yacmemptr _b) {
   sUI i;
   sU8 version;

   if(tkscript->configuration.b_tkx_linenumbers)
   {
      version = 3;  // v3=32bit line entries, v2=16bit line entries (pre 23Oct2015)
   }
   else
   {
      version = 1;
   }

   WU1(version); // Version

   // Tokendata + num_elements_field
   WU4(tokens.num_elements);
   for(i=0; i<tokens.num_elements; i++)
   {
      sU16 tok = tokens.elements[i];
      WU1( (sU8)tok );
      if(tok <= TOKEN_STRINGVAL)
      {
         // High index string
         i++;
         WU2( tokens.elements[i] );
      }
      else if( (tok>=TOK_LOWID) && (tok<=TOK_LOWID_STRINGVAL) )
      {
         // Low index string
         WU1( (sU8)tokens.elements[++i] );
      }
   }

   // (note) version 2 used 16bit line data (=> 65535 tokens per script, max)
   if(3 == version)
   {
      // Line offsets + num_elements field
      WU4(lines.num_elements);
      for(i=0; i<lines.num_elements; i++)
      {
         WU4(lines.elements[i]);
      }
   }

   // String data + num_elements field
   YAC_String *s = strings.elements;
   WU4(strings.num_elements);
   for(i=0; i<strings.num_elements; i++)
   {
      ////WU4( s->length );
      WU1( (sU8)s->getQuotFlag() );
      sUI j = 0;
      while(j<s->length) // Write chars incl. ASCIIZ
      {
         WU1(s->chars[j++]);
      }
      s++;
   }

}


// Endian-safe dword/word readers
#define RU4() (((sUI)_b.u8[0])<<24) | (((sUI)_b.u8[1])<<16) | (((sUI)_b.u8[2])<<8) | ((sUI)_b.u8[3]) ; _b.u32++ ; bufOff+=4
#define RU2() (sU16)( (((sUI)_b.u8[0])<<8) | ((sUI)_b.u8[1]) ) ; _b.u16++; bufOff+=2
#define RU1() *_b.u8++; bufOff++
#define ROF4 if( (bufOff+4)>=_bufLen ) return 0
#define ROF2 if( (bufOff+2)>=_bufLen ) return 0
#define ROF1 if( (bufOff+1)>=_bufLen ) return 0
sBool TKS_TokenizedScript::readTokens(yacmemptr _b, sUI _bufLen) {
   sUI i;
   sUI bufOff = 0;

   ROF1;
   sU8 version = RU1(); // Version
   // Version 1=no line numbers, 2=with line numbers(16bit), 3=with line numbers(32bit)
   if(version == 1 || version == 2 || version == 3)
   {
      // Tokendata + num_elements_field
      ROF4;
      tokens.num_elements = RU4();
      tokens.elements = new sU16[tokens.num_elements];
      if(tokens.elements == NULL)
      {
         Dprintf("[---] TKS_TokenizedScript::readTokens: failed to allocate %i tokens.\n", tokens.num_elements);
         // Error: Failed to allocate tokens
         tokens.num_elements = 0;
         return 0;
      }
      tokens.max_elements = tokens.num_elements;
      sU16 *ctok = tokens.elements;
      for(i=0; i<tokens.num_elements; i++)
      {
         ROF1;
         sU8 tok = RU1();
         *ctok++ = (sU16) tok;
         if(tok <= TOKEN_STRINGVAL)
         {
            // High index string
            ROF2;
            *ctok++ = RU2();
            i++;
         }
         else if( (tok>=TOK_LOWID) && (tok<=TOK_LOWID_STRINGVAL) )
         {
            // Low index string
            ctok[-1] -= TOK_LOWID;
            ROF1;
            *ctok++ = (sU16) RU1();
            i++;
         }
      }

      if(3 == version)
      {
         // 32bit Line offsets + num_elements field
         ROF4;
         lines.num_elements = RU4();
         lines.elements = new sU32[lines.num_elements];
         if(lines.elements == NULL)
         {
            Dprintf("[---] TKS_TokenizedScript::readTokens: failed to allocate %i lines.\n", lines.num_elements);
            // Error: Failed to allocate lines
            lines.num_elements = 0;
            return 0;
         }
         lines.max_elements = lines.num_elements;
         sU32 *cline = lines.elements;
         for(i=0; i<lines.num_elements; i++)
         {
            ROF4;
            *cline++ = RU4();
         }
      }
      else if(2 == version) // pre 23Oct2015 (16bit line entries)
      {
         // 16bit Line offsets + num_elements field
         ROF4;
         lines.num_elements = RU4();
         lines.elements = new sU32[lines.num_elements];
         if(lines.elements == NULL)
         {
            Dprintf("[---] TKS_TokenizedScript::readTokens: failed to allocate %i lines.\n", lines.num_elements);
            // Error: Failed to allocate lines
            lines.num_elements = 0;
            return 0;
         }
         lines.max_elements = lines.num_elements;
         sU32 *cline = lines.elements;
         for(i = 0; i<lines.num_elements; i++)
         {
            ROF2;
            *cline++ = RU2();
         }
      }
      else
      {
         // Version 1: fake line offsets; source becomes squeezed into a single line
         lines.elements = new sU32[2];
         lines.elements[0] = 0;
         lines.elements[1] = 0xFFFF;
         lines.num_elements = lines.max_elements = 2;
      }

      // String data + num_elements field
      ROF4;
      strings.num_elements = RU4();
      ////strings.elements = new YAC_String[strings.num_elements]/*()*/; // when () is used, IntelC++ 10.1.011 does not call constructors!
      strings.elements = (YAC_String*) malloc(sizeof(YAC_String) * strings.num_elements);
      if(strings.elements == NULL)
      {
         Dprintf("[---] TKS_TokenizedScript::readTokens: failed to allocate %i strings.\n", strings.num_elements);
         // Error: Failed to allocate strings
         strings.num_elements = 0;
         return 0;
      }
      strings.max_elements = strings.num_elements;
      YAC_String *s = strings.elements;
      for(i=0; i<strings.num_elements; i++)
      {
         ////ROF4;
         ////sUI strLen = RU4();

         new(s)YAC_String();

         ROF1;
         sU8 quotFlag = RU1();

         // Determine string length
         sUI strLen = 0;
         sUI tOff = bufOff;
         sUI j = 0;
         for(;;)
         {
            if(tOff++ >= _bufLen)
               return 0; // Overflow check
            if(0u == _b.u8[j++])
               break;
         }
         strLen = j;

         if(s->alloc( strLen )) // xxx always allocs arg+1 ?!
         {
            s->setQuotFlag( quotFlag );
            ////if( (bufOff + strLen) >= _bufLen) return 0; // Overflow check
            j = 0;
            while(strLen--)
            {
               s->chars[j++] = RU1();
            }
            s++->length = j;
         }
         else
         {
            Dprintf("[---] TKS_TokenizedScript::readTokens: failed to allocate string %i (length=%i).\n", i, strLen);
            // Error: Failed to allocate string
            return 0;
         }
      }

      // Tokens loaded
      b_own_tsl = 1;
      return 1;
   }
   return 0;

}


#ifdef TKS_EXPORT_ENTITY_SCANNER
sUI tks_entity_tree_getIDByString(YAC_String *_s) {
   return entity_tree.getEntityIDByString(_s);
}

void TKS_TokenizedScript::InitEntityTree(void) {
   entity_tree.addEntity("/*",          TOKEN_BLOCKCOMMENT);
   entity_tree.addEntity("//",          TOKEN_LINECOMMENT );
   entity_tree.addEntity(".",           TOKEN_DOT         );
   entity_tree.addEntity(",",           TOKEN_LISTSEP     );
   entity_tree.addEntity(";;",          TOKEN_ESAC        );
   entity_tree.addEntity(";",           TOKEN_TERM        );
   entity_tree.addEntity("{",           TOKEN_BEGBLOCK    );
   entity_tree.addEntity("}",           TOKEN_ENDBLOCK    );
   entity_tree.addEntity("(",           TOKEN_BEGEXPR     );
   entity_tree.addEntity(")",           TOKEN_ENDEXPR     );
   entity_tree.addEntity("++",          TOKEN_INC         );
   entity_tree.addEntity("--",          TOKEN_DEC         );
   entity_tree.addEntity("->",          TOKEN_TO          );
   entity_tree.addEntity("+=",          TOKEN_ADDASSIGN   );
   entity_tree.addEntity("-=",          TOKEN_SUBASSIGN   );
   entity_tree.addEntity("*=",          TOKEN_MULASSIGN   );
   entity_tree.addEntity("/=",          TOKEN_DIVASSIGN   );
   entity_tree.addEntity("%=",          TOKEN_MODASSIGN   );
   entity_tree.addEntity("<<=",         TOKEN_ASLASSIGN   );
   entity_tree.addEntity(">>=",         TOKEN_ASRASSIGN   );
   entity_tree.addEntity("|=",          TOKEN_BORASSIGN   );
   entity_tree.addEntity("&=",          TOKEN_BANDASSIGN  );
   entity_tree.addEntity("^=",          TOKEN_EORASSIGN   );
   entity_tree.addEntity("+",           TOKEN_ADD         );
   entity_tree.addEntity("-",           TOKEN_SUB         );
   entity_tree.addEntity("*",           TOKEN_MUL         );
   entity_tree.addEntity("/",           TOKEN_DIV         );
   entity_tree.addEntity("%",           TOKEN_MOD         );
   entity_tree.addEntity("<<",          TOKEN_ASL         );
   entity_tree.addEntity(">>",          TOKEN_ASR         );
   entity_tree.addEntity("||",          TOKEN_LOR         );
   entity_tree.addEntity("|",           TOKEN_BOR         );
   entity_tree.addEntity("&&",          TOKEN_LAND        );
   entity_tree.addEntity("&",           TOKEN_BAND        );
   entity_tree.addEntity("^",           TOKEN_EOR         );
   entity_tree.addEntity("::",          TOKEN_MEMBER      );
   entity_tree.addEntity("==",          TOKEN_CMPEQ       );
   entity_tree.addEntity("=>",          TOKEN_RETOBJASSIGN);
   entity_tree.addEntity("<=>",         TOKEN_RETOBJRET   );
   entity_tree.addEntity("=",           TOKEN_ASSIGN      );
   entity_tree.addEntity("!=",          TOKEN_CMPNE       );
   entity_tree.addEntity("!",           TOKEN_NOT         );
   entity_tree.addEntity("~",           TOKEN_BITNOT      );
   entity_tree.addEntity("<=",          TOKEN_CMPLE       );
   entity_tree.addEntity("<",           TOKEN_CMPLT       );
   entity_tree.addEntity(">=",          TOKEN_CMPGE       );
   entity_tree.addEntity(">",           TOKEN_CMPGT       );
   entity_tree.addEntity("?",           TOKEN_TIF         );
   entity_tree.addEntity(":",           TOKEN_TELSE       );
   entity_tree.addEntity("[",           TOKEN_BEGARRAY    );
   entity_tree.addEntity("]",           TOKEN_ENDARRAY    );
   entity_tree.addEntity("#(",          TOKEN_VALUE       );
   entity_tree.addEntity("#[",          TOKEN_HASHINI     );
   entity_tree.addEntity("if",          TOKEN_IF          );
   entity_tree.addEntity("else",        TOKEN_ELSE        );
   entity_tree.addEntity("for",         TOKEN_FOR         );
   entity_tree.addEntity("do",          TOKEN_DO          );
   entity_tree.addEntity("while",       TOKEN_WHILE       );
   entity_tree.addEntity("class",       TOKEN_CLASS       );
   entity_tree.addEntity("function",    TOKEN_FUNCTION    );
   entity_tree.addEntity("return",      TOKEN_RETURN      );
   entity_tree.addEntity("switch",      TOKEN_SWITCH      );
   entity_tree.addEntity("case",        TOKEN_CASE        );
   entity_tree.addEntity("break",       TOKEN_BREAK       );
   entity_tree.addEntity("..",          TOKEN_RANGE       );
   entity_tree.addEntity("int",         TOKEN_INT         );
   entity_tree.addEntity("short",       TOKEN_INT         );
   entity_tree.addEntity("char",        TOKEN_INT         );
   entity_tree.addEntity("byte",        TOKEN_INT         );
   entity_tree.addEntity("boolean",     TOKEN_INT         );
   entity_tree.addEntity("bool",        TOKEN_INT         );
   entity_tree.addEntity("float",       TOKEN_FLOAT       );
   entity_tree.addEntity("Object",      TOKEN_OBJECT      );
   entity_tree.addEntity("String",      TOKEN_STRING      );
   entity_tree.addEntity("default",     TOKEN_DEFAULT     );
   entity_tree.addEntity("void",        TOKEN_VOID        );
   entity_tree.addEntity("null",        TOKEN_NULL        );
   entity_tree.addEntity("true",        TOKEN_TRUE        );
   entity_tree.addEntity("false",       TOKEN_FALSE       );
   entity_tree.addEntity("maybe",       TOKEN_MAYBE       );
   entity_tree.addEntity("E",           TOKEN_E           );
   entity_tree.addEntity("LOG10",       TOKEN_LOG10       );
   entity_tree.addEntity("LN10",        TOKEN_LN10        );
   entity_tree.addEntity("1PI",         TOKEN_1PI         );
   entity_tree.addEntity("PI2",         TOKEN_PI2         );
   entity_tree.addEntity("2PI",         TOKEN_2PI         );
   entity_tree.addEntity("PI",          TOKEN_PI          );
   entity_tree.addEntity("RAND_MAX",    TOKEN_RANDMAX     );
   entity_tree.addEntity("1SQRT2",      TOKEN_1SQRT2      );
   entity_tree.addEntity("SQRT2",       TOKEN_SQRT2       );
   ////entity_tree.addEntity("exp",         TOKEN_EXP         );
   ////entity_tree.addEntity("ln",          TOKEN_LN          );
   ////entity_tree.addEntity("sin",         TOKEN_SIN         );
   ////entity_tree.addEntity("cos",         TOKEN_COS         );
   ////entity_tree.addEntity("tan",         TOKEN_TAN         );
   ////entity_tree.addEntity("asin",        TOKEN_ASIN        );
   ////entity_tree.addEntity("acos",        TOKEN_ACOS        );
   ////entity_tree.addEntity("sqrt",        TOKEN_SQRT        );
   entity_tree.addEntity("loop",        TOKEN_LOOP        );
   ////entity_tree.addEntity("rad",         TOKEN_RAD         );
   ////entity_tree.addEntity("deg",         TOKEN_DEG         );
   ////entity_tree.addEntity("abs",         TOKEN_ABS         );
   ////entity_tree.addEntity("frac",        TOKEN_FRAC        );
   ////entity_tree.addEntity("tcint",       TOKEN_TCINT       );
   ////entity_tree.addEntity("tcfloat",     TOKEN_TCFLOAT     );
   ////entity_tree.addEntity("tcstring",    TOKEN_TCSTR       );
   ////entity_tree.addEntity("tcobject",    TOKEN_TCOBJECT    );
   ////entity_tree.addEntity("tcpointer",   TOKEN_TCPOINTER   );
   ////entity_tree.addEntity("tcchar",      TOKEN_TCCHR       );
   ////entity_tree.addEntity("round",       TOKEN_ROUND       );
   ////entity_tree.addEntity("rnd",         TOKEN_RND         );
   ////entity_tree.addEntity("rand",        TOKEN_RND         );
   entity_tree.addEntity("use",         TOKEN_USE         );
   entity_tree.addEntity("Array",       TOKEN_ARRAY       );
   entity_tree.addEntity("new",         TOKEN_NEW         );
   entity_tree.addEntity("foreach",     TOKEN_FOREACH     );
   ////entity_tree.addEntity("in",          TOKEN_IN          );
   entity_tree.addEntity("enum",        TOKEN_ENUM        );
   entity_tree.addEntity("define",      TOKEN_DEFINE      );
   entity_tree.addEntity("#define",     TOKEN_DEFINEALT   );
   entity_tree.addEntity("tag",         TOKEN_TAG         );
   entity_tree.addEntity("local",       TOKEN_LOCAL       );
   entity_tree.addEntity("module",      TOKEN_MODULE      );
   entity_tree.addEntity("this",        TOKEN_THIS        );
   ////entity_tree.addEntity("stdout",      TOKEN_STDOUT      );
   ////entity_tree.addEntity("stderr",      TOKEN_STDERR      );
   ////entity_tree.addEntity("dcon",        TOKEN_DCON        );
   ////entity_tree.addEntity("die",         TOKEN_DIE         );
   ////entity_tree.addEntity("print",       TOKEN_PRINT       );
   ////entity_tree.addEntity("trace",       TOKEN_TRACE       );
   ////entity_tree.addEntity("dtrace",      TOKEN_DTRACE      );
   ////entity_tree.addEntity("stdflush",    TOKEN_FLUSH       );
   entity_tree.addEntity("deref",       TOKEN_DEREF       );
   entity_tree.addEntity("prepare",     TOKEN_PREPARE     );
   ////entity_tree.addEntity("2n",          TOKEN_2N          );
   ////entity_tree.addEntity("clamp",       TOKEN_CLAMP       );
   ////entity_tree.addEntity("wrap",        TOKEN_WRAP        );
   ////entity_tree.addEntity("rgb",         TOKEN_RGB         );
   ////entity_tree.addEntity("argb",        TOKEN_ARGB        );
   entity_tree.addEntity("compile",     TOKEN_COMPILE     );
   ////entity_tree.addEntity("typeid",      TOKEN_TYPEID      );
   ////entity_tree.addEntity("typename",    TOKEN_TYPENAME    );
   entity_tree.addEntity("extends",     TOKEN_EXTENDS     );
   entity_tree.addEntity("method",      TOKEN_METHOD      );
   entity_tree.addEntity("public",      TOKEN_PUBLIC      );
   entity_tree.addEntity("private",     TOKEN_PRIVATE     );
   entity_tree.addEntity("protected",   TOKEN_PROTECTED   );
   entity_tree.addEntity("instanceof",  TOKEN_INSTANCEOF  );
   entity_tree.addEntity("static",      TOKEN_STATIC      );
   entity_tree.addEntity("returns",     TOKEN_RETURNS     );
   entity_tree.addEntity("@",           TOKEN_ADDRESS     );
   entity_tree.addEntity("constraint",  TOKEN_CONSTRAINT  );
   entity_tree.addEntity("delegate",    TOKEN_DELEGATE    );
   entity_tree.addEntity("explain",     TOKEN_EXPLAIN     );
   entity_tree.addEntity("try",         TOKEN_TRY         );
   entity_tree.addEntity("catch",       TOKEN_CATCH       );
   entity_tree.addEntity("finally",     TOKEN_FINALLY     );
   entity_tree.addEntity("throw",       TOKEN_THROW       );
   entity_tree.addEntity("tcchar",      TOKEN_CHARS       );
   entity_tree.addEntity("namespace",   TOKEN_NAMESPACE   );
   entity_tree.addEntity("abstract",    TOKEN_ABSTRACT    );
   entity_tree.addEntity("virtual",     TOKEN_VIRTUAL     );

   entity_tree.exportToC("autogen_tks_entitytree_lookup.cpp", "tks_entity_tree_getIDByString");
}
#else
#include "autogen_tks_entitytree_lookup.cpp"
#endif
