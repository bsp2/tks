/// TKS_StringStackIterator.h
///
/// (c) 2001-2026 Bastian Spiegel <bs@tkscript.de>
///     - distributed under terms of the Lesser GNU General Public License (LGPL)
///

#ifndef TKS_STRINGSTACKITERATOR_H__
#define TKS_STRINGSTACKITERATOR_H__


class TKS_StringStackIterator : public YAC_Iterator {
  public:
   const YAC_String   *it_string;
   TKS_StringIterator  si;
   sBool               cont_it;

  public:
   TKS_StringStackIterator  (const YAC_String *);
   ~TKS_StringStackIterator ();

   void YAC_VCALL begin   (void);
   void YAC_VCALL getNext (YAC_Value *);
   void YAC_VCALL end     (void);

};


#endif // TKS_STRINGSTACKITERATOR_H__
