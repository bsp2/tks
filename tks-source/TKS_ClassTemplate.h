/// TKS_ClassTemplate.h
////
/// (c) 2001-2026 Bastian Spiegel <bs@tkscript.de>
///     - distributed under terms of the Lesser GNU General Public License (LGPL)
///

#ifndef TKS_CLASSTEMPLATE_H__
#define TKS_CLASSTEMPLATE_H__


enum __tkclasstemplateflags {
	TKS_CLTFL_ONCE=(1<<0), // never create another instance of this class

	TKS_CLTFL_PAD
};

class TKS_ClassTemplate : public YAC_Object {
public:
	YAC_Object      *template_object;
	YAC_CommandListY commandsy;
	YAC_String       class_name;
	sUI              may_instantiate; // 0=static, 1=normal, 2=no instance but dynamic
	sBool            isplugin;
	sUI              flags;
	sUI              quad_hash_table[512+1]; // init by setObject cmdindex|length|16bithash
	TKS_SignalEntry  signals[TKS_MAXSIGNALS];
	sUI              num_signals;
	TKS_CachedObject static_co;
   sUI              pool_priority; // pool priority class (low, medium, high, see YAC_POOL_PRIORITY_xxx)
   sUI              pool_size; // Object size (number of dwords), required for pooled allocations

public:
	TKS_ClassTemplate(void);
	~TKS_ClassTemplate();

	void          init           (void);                                           // ---- clear pointers (constructor)
	void          clean          (void);                                           // ---- delete mapping, template and unset class_name (destructor)
	sBool         setTemplateY   (YAC_Object *_template);                          // ---- init commands, class_name, template_object, YInG/YAC interface
   void          addToQuadHash  (sUI _i, YAC_String *_mnRoot);

   const YAC_CommandY *findCommandY   (YAC_String *_name) const;
	      YAC_CommandY *findCommandY2  (YAC_String *_name) const;
};


#endif // TKS_CLASSTEMPLATE_H__
