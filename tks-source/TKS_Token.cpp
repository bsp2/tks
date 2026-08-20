/// TKS_Token.cpp
///
/// (c) 2001-2026 Bastian Spiegel <bs@tkscript.de>
///     - distributed under terms of the Lesser GNU General Public License (LGPL)
///
///
#include "tks.h"
#include "TKS_Token.h"


TKS_Token::TKS_Token(const sChar * _c,
                     sU16    _id,
                     sU16    _l ,
                     sBool   _bBlankMustFollow
                     )
{
   chars  = _c;
   length = _l;
   id     = _id;
   b_blank_must_follow = _bBlankMustFollow;
}

TKS_Token::~TKS_Token() {
   chars  = NULL;
   length = 0;
   id     = 0;
}

TKS_Token::TKS_Token(void) {
   chars  = NULL;
   length = 0;
   id     = 0;
}
