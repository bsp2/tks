/// YAC_ListNodeIterator.h
///
/// (c) 2003-2009 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///
#ifndef __YAC_LISTNODEITERATOR_H__
#define __YAC_LISTNODEITERATOR_H__


class YAC_ListNodeIterator: public YAC_Iterator {
public:
	const YAC_ListNode *l;
	const YAC_ListNode *lc;

public:
	YAC_ListNodeIterator(const YAC_ListNode *_l); 

	~YAC_ListNodeIterator();
	
	void YAC_VCALL getNext(YAC_Value *_r);
};


#endif // __YAC_LISTNODEITERATOR_H__
