/// TKS_TokenTable.cpp
///
/// (c) 2001-2009 Bastian Spiegel . <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///
///

#include "tks.h"
#include "TKS_Token.h"
#include "TKS_TokenTable.h"

TKS_Token tks_lang_tokens[]={ 
      TKS_Token("@VAR@",       TOKEN_VAR         , 0 ),
      TKS_Token("@INTVAL@",    TOKEN_INTVAL      , 0 ),
      TKS_Token("@FLOATVAL@",  TOKEN_FLOATVAL    , 0 ),
      TKS_Token("@STRINGVAL@", TOKEN_STRINGVAL   , 0 ),
      TKS_Token("/*",          TOKEN_BLOCKCOMMENT, 2 ),
      TKS_Token("//",          TOKEN_LINECOMMENT , 2 ),
      TKS_Token(".",           TOKEN_DOT         , 1 ),
      TKS_Token(",",           TOKEN_LISTSEP     , 1 ),
      TKS_Token(";;",          TOKEN_ESAC        , 2 ),
      TKS_Token(";",           TOKEN_TERM        , 1 ),
      TKS_Token("{",           TOKEN_BEGBLOCK    , 1 ),
      TKS_Token("}",           TOKEN_ENDBLOCK    , 1 ),
      TKS_Token("(",           TOKEN_BEGEXPR     , 1 ),
      TKS_Token(")",           TOKEN_ENDEXPR     , 1 ),
      TKS_Token("++",          TOKEN_INC         , 2 ),
      TKS_Token("--",          TOKEN_DEC         , 2 ),
      TKS_Token("->",          TOKEN_TO          , 2 ),
      TKS_Token("+=",          TOKEN_ADDASSIGN   , 2 ),
      TKS_Token("-=",          TOKEN_SUBASSIGN   , 2 ),
      TKS_Token("*=",          TOKEN_MULASSIGN   , 2 ),
      TKS_Token("/=",          TOKEN_DIVASSIGN   , 2 ),
      TKS_Token("%=",          TOKEN_MODASSIGN   , 2 ),
      TKS_Token("<<=",         TOKEN_ASLASSIGN   , 3 ),
      TKS_Token(">>=",         TOKEN_ASRASSIGN   , 3 ),
      TKS_Token("|=",          TOKEN_BORASSIGN   , 2 ),
      TKS_Token("&=",          TOKEN_BANDASSIGN  , 2 ),
      TKS_Token("^=",          TOKEN_EORASSIGN   , 2 ), 
      TKS_Token("@",           TOKEN_ADDRESS     , 1 ), 
      TKS_Token("+",           TOKEN_ADD         , 1 ),
      TKS_Token("-",           TOKEN_SUB         , 1 ),
      TKS_Token("*",           TOKEN_MUL         , 1 ),
      TKS_Token("/",           TOKEN_DIV         , 1 ),
      TKS_Token("%",           TOKEN_MOD         , 1 ),
      TKS_Token("<<",          TOKEN_ASL         , 2 ),
      TKS_Token(">>",          TOKEN_ASR         , 2 ),
      TKS_Token("||",          TOKEN_LOR         , 2 ),
      TKS_Token("|",           TOKEN_BOR         , 1 ),
      TKS_Token("&&",          TOKEN_LAND        , 2 ),
      TKS_Token("&",           TOKEN_BAND        , 1 ),
      TKS_Token("^",           TOKEN_EOR         , 1 ),
      TKS_Token("::",          TOKEN_MEMBER      , 2 ),
      TKS_Token("==",          TOKEN_CMPEQ       , 2 ),
      TKS_Token("=>",          TOKEN_RETOBJASSIGN, 2 ),
      TKS_Token("<=>",         TOKEN_RETOBJRET   , 3 ),
      TKS_Token("=",           TOKEN_ASSIGN      , 1 ),
      TKS_Token("!=",          TOKEN_CMPNE       , 2 ),
      TKS_Token("!",           TOKEN_NOT         , 1 ),
      TKS_Token("~",           TOKEN_BITNOT      , 1 ),
      TKS_Token("<=",          TOKEN_CMPLE       , 2 ), // also used for pointer assignments
      TKS_Token("<",           TOKEN_CMPLT       , 1 ),
      TKS_Token(">=",          TOKEN_CMPGE       , 2 ),
      TKS_Token(">",           TOKEN_CMPGT       , 1 ),
      TKS_Token("?",           TOKEN_TIF         , 1 ),
      TKS_Token(":",           TOKEN_TELSE       , 1 ),
      TKS_Token("[",           TOKEN_BEGARRAY    , 1 ),
      TKS_Token("]",           TOKEN_ENDARRAY    , 1 ),
      TKS_Token("#(",          TOKEN_VALUE       , 2 ), 
      TKS_Token("#[",          TOKEN_HASHINI     , 2 ),
      TKS_Token("if",          TOKEN_IF          , 2, 1 ),
      TKS_Token("else",        TOKEN_ELSE        , 4, 1 ),
      TKS_Token("for",         TOKEN_FOR         , 3, 1 ),
      TKS_Token("do",          TOKEN_DO          , 2, 1 ),
      TKS_Token("while",       TOKEN_WHILE       , 5, 1 ) ,
      TKS_Token("class",       TOKEN_CLASS       , 5, 1 ),
      TKS_Token("function",    TOKEN_FUNCTION    , 8, 1 ),
      TKS_Token("return",      TOKEN_RETURN      , 6, 1 ),
      TKS_Token("switch",      TOKEN_SWITCH      , 6, 1 ),
      TKS_Token("case",        TOKEN_CASE        , 4, 1 ),
      TKS_Token("break",       TOKEN_BREAK       , 5, 0 ),
      TKS_Token("..",          TOKEN_RANGE       , 2 ),
      TKS_Token("int",         TOKEN_INT         , 3, 1 ), 
      TKS_Token("short",       TOKEN_INT         , 5, 1 ), 
      TKS_Token("char",        TOKEN_INT         , 4, 1 ), 
      TKS_Token("byte",        TOKEN_INT         , 4, 1 ), 
      TKS_Token("bool",        TOKEN_INT         , 4, 1 ), 
      TKS_Token("boolean",     TOKEN_INT         , 7, 1 ), 
      TKS_Token("float",       TOKEN_FLOAT       , 5, 1 ),
      TKS_Token("Object",      TOKEN_OBJECT      , 6, 1 ),
      TKS_Token("String",      TOKEN_STRING      , 6, 1 ),
      TKS_Token("default",     TOKEN_DEFAULT     , 7, 1 ),
      TKS_Token("null",        TOKEN_NULL        , 4, 1 ),
      TKS_Token("true",        TOKEN_TRUE        , 4, 1 ),
      TKS_Token("false",       TOKEN_FALSE       , 5, 1 ), 
      TKS_Token("maybe",       TOKEN_MAYBE       , 5, 1 ), 
      TKS_Token("E",           TOKEN_E           , 1 ),
      TKS_Token("LOG10",       TOKEN_LOG10       , 5 ),
      TKS_Token("LN10",        TOKEN_LN10        , 4 ),
      TKS_Token("1PI",         TOKEN_1PI         , 3 ),
      TKS_Token("PI2",         TOKEN_PI2         , 3 ),
      TKS_Token("2PI",         TOKEN_2PI         , 3 ),
      TKS_Token("PI",          TOKEN_PI          , 2 ),
      TKS_Token("RAND_MAX",    TOKEN_RANDMAX     , 8 ),
      TKS_Token("1SQRT2",      TOKEN_1SQRT2      , 6 ),
      TKS_Token("SQRT2",       TOKEN_SQRT2       , 5 ),
      ////TKS_Token("exp",         TOKEN_EXP         , 3, 1 ),
      ////TKS_Token("ln",          TOKEN_LN          , 2, 1 ),
      ////TKS_Token("sin",         TOKEN_SIN         , 3, 1 ),
      ////TKS_Token("cos",         TOKEN_COS         , 3, 1 ),
      ////TKS_Token("tan",         TOKEN_TAN         , 3, 1 ),
      ////TKS_Token("asin",        TOKEN_ASIN        , 4, 1 ),
      ////TKS_Token("acos",        TOKEN_ACOS        , 4, 1 ),
      ////TKS_Token("sqrt",        TOKEN_SQRT        , 4, 1 ),
      TKS_Token("loop",        TOKEN_LOOP        , 4, 1 ),
      ////TKS_Token("rad",         TOKEN_RAD         , 3, 1 ),
      ////TKS_Token("deg",         TOKEN_DEG         , 3, 1 ),
      ////TKS_Token("abs",         TOKEN_ABS         , 3, 1 ),
      ////TKS_Token("frac",        TOKEN_FRAC        , 4, 1 ),
      ////TKS_Token("tcint",       TOKEN_TCINT       , 5, 1 ),
      ////TKS_Token("tcfloat",     TOKEN_TCFLOAT     , 7, 1 ),
      ////TKS_Token("tcstring",    TOKEN_TCSTR       , 8, 1 ), 
      ////TKS_Token("tcobject",    TOKEN_TCOBJECT    , 8, 1 ),
      ////TKS_Token("tcpointer",   TOKEN_TCPOINTER   , 9, 1 ), 
      ////TKS_Token("tcchar",      TOKEN_TCCHR       , 6, 1 ),
      ////TKS_Token("round",       TOKEN_ROUND       , 5, 1 ),
      ////TKS_Token("rnd",         TOKEN_RND         , 3, 1 ), 
      ////TKS_Token("rand",        TOKEN_RND         , 4, 1 ), 
      TKS_Token("use",         TOKEN_USE         , 3, 1 ),
      TKS_Token("Array",       TOKEN_ARRAY       , 5, 1 ), // not really used currently (was used for Array<type> decls) but lets keep it reserved
      TKS_Token("new",         TOKEN_NEW         , 3, 1 ),
      TKS_Token("foreach",     TOKEN_FOREACH     , 7, 1 ), 
      ////TKS_Token("in",          TOKEN_IN          , 2, 1 ),
      TKS_Token("enum",        TOKEN_ENUM        , 4, 1 ),
      TKS_Token("define",      TOKEN_DEFINE      , 6, 1 ),
      TKS_Token("#define",     TOKEN_DEFINEALT   , 7, 1 ),
      TKS_Token("tag",         TOKEN_TAG         , 3, 1 ),
      TKS_Token("local",       TOKEN_LOCAL       , 5, 1 ),
      TKS_Token("module",      TOKEN_MODULE      , 6, 1 ),
      TKS_Token("this",        TOKEN_THIS        , 4, 1 ),
      ////TKS_Token("stdout",      TOKEN_STDOUT      , 6, 1 ),
      ////TKS_Token("stderr",      TOKEN_STDERR      , 6, 1 ),
      ////TKS_Token("dcon",        TOKEN_DCON        , 4, 1 ), 
      ////TKS_Token("die",         TOKEN_DIE         , 3, 1 ), 
      ////TKS_Token("print",       TOKEN_PRINT       , 5, 1 ), 
      ////TKS_Token("trace",       TOKEN_TRACE       , 5, 1 ),
      ////TKS_Token("dtrace",      TOKEN_DTRACE      , 6, 1 ),
      ////TKS_Token("stdflush",    TOKEN_FLUSH       , 8, 1 ),
      TKS_Token("deref",       TOKEN_DEREF       , 5, 1 ),
      TKS_Token("prepare",     TOKEN_PREPARE     , 7, 1 ),
      ////TKS_Token("2n",          TOKEN_2N          , 2, 1 ),
      ////TKS_Token("clamp",       TOKEN_CLAMP       , 5, 1 ),
      ////TKS_Token("wrap",        TOKEN_WRAP        , 4, 1 ),
      ////TKS_Token("rgb",         TOKEN_RGB         , 3, 1 ),
      ////TKS_Token("argb",        TOKEN_ARGB        , 4, 1 ),
      TKS_Token("compile",     TOKEN_COMPILE     , 7, 1 ),
      ////TKS_Token("typeid",      TOKEN_TYPEID      , 6, 1 ),
      ////TKS_Token("typename",    TOKEN_TYPENAME    , 8, 1 ),
      TKS_Token("extends",     TOKEN_EXTENDS     , 7, 1 ),
      TKS_Token("method",      TOKEN_METHOD      , 6, 1 ),
      TKS_Token("public",      TOKEN_PUBLIC      , 6, 1 ),
      TKS_Token("private",     TOKEN_PRIVATE     , 7, 1 ),
      TKS_Token("protected",   TOKEN_PROTECTED   , 9, 1 ),
      TKS_Token("instanceof",  TOKEN_INSTANCEOF  , 10, 1), 
      TKS_Token("static",      TOKEN_STATIC      , 6, 1 ), 
      TKS_Token("returns",     TOKEN_RETURNS     , 7 , 1), 
      TKS_Token("constraint",  TOKEN_CONSTRAINT  , 10, 1), 
      TKS_Token("delegate",    TOKEN_DELEGATE    , 8, 1 ), 
      TKS_Token("explain",     TOKEN_EXPLAIN     , 7, 1 ),
      TKS_Token("try",         TOKEN_TRY         , 3, 1 ),
      TKS_Token("catch",       TOKEN_CATCH       , 5, 1 ),
      TKS_Token("finally",     TOKEN_FINALLY     , 7, 1 ),
      TKS_Token("throw",       TOKEN_THROW       , 5, 1 ),
      TKS_Token("void",        TOKEN_VOID        , 4, 1 ),
      TKS_Token("tcchar",      TOKEN_CHARS       , 6, 1 ),
      TKS_Token("namespace",   TOKEN_NAMESPACE   , 9, 1 ),
      TKS_Token("abstract",    TOKEN_ABSTRACT    , 8, 1 ),
      TKS_Token("virtual",     TOKEN_VIRTUAL     , 7, 1 ),

      TKS_Token("__nop__",     TOKEN_NOP         , 7 )
};


const sChar *tks_get_token_name(sU16 _tok) { 
   switch(_tok) 
   { 
   case 0: return "(VAR)"; 
   case 1: return "(INTVAL)"; 
   case 2: return "(FLOATVAL)"; 
   case 3: return "(STRINGVAL"; 
   default: 
      {
         TKS_Token *tok = tks_get_token(_tok);
         if(tok != NULL)
         {
            return tok->chars;
         }
         else
         {
            return "(!INVALID!)\n";
         }
      }
   }
} 

TKS_Token *tks_get_token(sU16 _tok) { 
   switch(_tok) 
   { 
   case 0: 
   case 1: 
   case 2: 
   case 3: return NULL;
   default: 
      if(_tok < TOKEN_NUMTOKENS) 
      {
         sUI i = 0;
         while(tks_lang_tokens[i].id != TOKEN_NOP)  
         {
            if(tks_lang_tokens[i].id == _tok) 
            {
               return &tks_lang_tokens[i]; 
            }
            else 
            {
               i++; 
            } 
         }
      }
      tkscript->printf("-- tks_get_token_name(0x%04x) ERR: invalid token!\n", _tok);
      break;
   }
   return NULL;
} 

