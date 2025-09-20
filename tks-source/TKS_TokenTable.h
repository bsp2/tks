/// TKS_TokenTable.h
///
/// (c) 2001-2008 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///

#ifndef __TKS_TOKENTABLE_H__
#define __TKS_TOKENTABLE_H__

extern TKS_Token tks_lang_tokens[];

enum __tks_lang_tokens {
      TOKEN_VAR = 0, // variable names
      TOKEN_INTVAL, 
      TOKEN_FLOATVAL,
      TOKEN_STRINGVAL,
      TOKEN_BLOCKCOMMENT,
      TOKEN_LINECOMMENT,
      
      TOKEN_DOT,
      TOKEN_LISTSEP,
      TOKEN_ESAC,
      TOKEN_TERM,
      TOKEN_BEGBLOCK, // 10
      TOKEN_ENDBLOCK,
      TOKEN_BEGEXPR,
      TOKEN_ENDEXPR,
      TOKEN_INC,
      TOKEN_DEC,
      TOKEN_TO, // 16
      TOKEN_ADDASSIGN,
      TOKEN_SUBASSIGN,
      TOKEN_MULASSIGN,
      TOKEN_DIVASSIGN, // 20
      TOKEN_MODASSIGN,
      TOKEN_ASLASSIGN,
      TOKEN_ASRASSIGN,
      TOKEN_BORASSIGN,
      TOKEN_BANDASSIGN,
      TOKEN_EORASSIGN,
      TOKEN_ADD, 
      TOKEN_SUB,
      TOKEN_MUL,
      TOKEN_DIV, // 30
      TOKEN_MOD,
      TOKEN_ASL,
      TOKEN_ASR,
      TOKEN_LOR,
      TOKEN_BOR,
      TOKEN_LAND,
      TOKEN_BAND,
      TOKEN_EOR,
      TOKEN_MEMBER,
      TOKEN_CMPEQ, // 40
      TOKEN_RETOBJASSIGN, // =>
      TOKEN_RETOBJRET, // <=>
      TOKEN_ASSIGN,
      TOKEN_CMPNE,
      TOKEN_NOT, 
      TOKEN_BITNOT, 
      TOKEN_CMPLE,
      TOKEN_CMPLT,
      TOKEN_CMPGE,
      TOKEN_CMPGT, // 50
      TOKEN_TIF,
      TOKEN_TELSE,
      TOKEN_BEGARRAY,
      TOKEN_ENDARRAY,
      TOKEN_VALUE, 
      TOKEN_HASHINI,
      TOKEN_IF,
      TOKEN_ELSE,
      TOKEN_FOR,
      TOKEN_DO, // 60
      TOKEN_WHILE,
      TOKEN_CLASS,
      TOKEN_FUNCTION,
      TOKEN_RETURN,
      TOKEN_SWITCH,
      TOKEN_CASE,
      TOKEN_BREAK,
      TOKEN_RANGE, 
      TOKEN_INT,
      TOKEN_FLOAT, // 70
      TOKEN_OBJECT,
      TOKEN_STRING,
      TOKEN_DEFAULT,
      TOKEN_VOID,
      TOKEN_NULL,
      TOKEN_TRUE,
      TOKEN_FALSE, 
      TOKEN_MAYBE,
      TOKEN_E,  // xxx TODO: do these constants have to be tokens ? same goes for default, null, true, false, void
      TOKEN_LOG10, // 80
      TOKEN_LN10,
      TOKEN_1PI,
      TOKEN_PI2,
      TOKEN_2PI,
      TOKEN_PI,
      TOKEN_RANDMAX,
      TOKEN_1SQRT2,
      TOKEN_SQRT2,
      TOKEN_LOOP,
      TOKEN_USE, // 90
      TOKEN_NEW,
      TOKEN_FOREACH, 
      TOKEN_ENUM,
      TOKEN_DEFINE,
      TOKEN_DEFINEALT,
      TOKEN_TAG,
      TOKEN_LOCAL,
      TOKEN_MODULE,
      TOKEN_THIS,
      TOKEN_DEREF, // 100
      TOKEN_PREPARE,
      TOKEN_COMPILE, 
      TOKEN_EXTENDS,
      TOKEN_METHOD,
      TOKEN_PUBLIC,
      TOKEN_PRIVATE,
      TOKEN_PROTECTED,
      TOKEN_INSTANCEOF, 
      TOKEN_STATIC, 
      TOKEN_RETURNS, // 110
      TOKEN_ADDRESS, 
      TOKEN_CONSTRAINT, 
      TOKEN_DELEGATE,
      TOKEN_EXPLAIN,
      TOKEN_NOP,
      TOKEN_TRY,
      TOKEN_CATCH,
      TOKEN_FINALLY,
      TOKEN_THROW,
      TOKEN_ARRAY, // 120
      TOKEN_CHARS, // tcchar (aka chr$) ..
      TOKEN_NAMESPACE,
      TOKEN_ABSTRACT,
      TOKEN_VIRTUAL,
      
      TOKEN_NUMTOKENS
};

TKS_Token *   tks_get_token          (sU16 _tok);
const sChar * tks_get_token_name     (sU16 _tok); 
const sChar * tks_get_xml_token_name (sU16 _tok); 


#endif
