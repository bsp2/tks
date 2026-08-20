/// TKS_Token.h
///
/// (c) 2001-2026 Bastian Spiegel <bs@tkscript.de>
///     - distributed under terms of the Lesser GNU General Public License (LGPL)
///
///

#ifndef TKS_TOKEN_H__
#define TKS_TOKEN_H__


/// Pass1 scanner token
class TKS_Token {
public:
   const sChar *chars;
   sU16   length;
   sU16   id;
   sBool  b_blank_must_follow;

public:
   TKS_Token (const sChar*, sU16 _id, sU16 _l, sBool _bBlankMustFollow = 0);
   TKS_Token (void);
   ~TKS_Token();

};


#endif // TKS_TOKEN_H__
