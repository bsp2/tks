/// TKS_Stack.h
///
/// (c) 2001-2009 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///
#ifndef __TKS_STACK_H__
#define __TKS_STACK_H__


class TKS_Stack : public YAC_Object {
public:
    YAC_Object  *stack_template;
    YAC_Object **stack_array;
    sSI          stack_size;
    sSI          stack_index;

public:
         TKS_Stack            (void);
         ~TKS_Stack           ();

    void  YAC_VCALL yacOperator    (sSI, YAC_Object *, YAC_Value *);
    sBool YAC_VCALL yacIsComposite (void) {return 1;}

    void        setTemplate   (YAC_Object*);
    YAC_Object *getTemplate   (void);
    YAC_Object *push          (void);
    YAC_Object *pop           (void);
    sSI         setSize       (sSI);
    sSI         getSize       (void);
    void        setIndex      (sSI);
    sSI         getIndex      (void);
    void        init          (YAC_Object *_template, sSI _size);

};

#endif // __TKS_STACK_H__
