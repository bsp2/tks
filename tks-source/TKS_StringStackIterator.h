/// TKS_StringStackIterator.h
///
/// (c) 2001-2009 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///
#ifndef __TKS_STRINGSTACKITERATOR_H__
#define __TKS_STRINGSTACKITERATOR_H__


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


#endif // __TKS_STRINGSTACKITERATOR_H__
