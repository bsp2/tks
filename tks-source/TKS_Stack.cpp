/// TKS_Stack.cpp
///
/// (c) 2001-2008 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///

#include "tks.h"
#include "TKS_Stack.h"


TKS_Stack::TKS_Stack(void) {
   stack_template = NULL;
   stack_array    = NULL;
   stack_size     = 0;
   stack_index    = 0;
}

TKS_Stack::~TKS_Stack() {
   stack_template = NULL;
   if(stack_array)
   {
      int i;
      for(i=0; i<stack_size; i++)
      {
         if(stack_array[i])
         {
            YAC_DELETE(stack_array[i]);
            stack_array[i] = NULL;
         }
      }
      delete [] stack_array;
      stack_array = NULL;
   }
   stack_size = 0;
   stack_index = 0;
}

void YAC_VCALL TKS_Stack::yacOperator(sSI _cmd, YAC_Object *_obj, YAC_Value *_r) {
   YAC_Object::yacOperator(_cmd, _obj, _r);
}

void TKS_Stack::setTemplate(YAC_Object *_t) {
   if(!stack_template)
   {
      stack_template = _t;
   }
}

YAC_Object *TKS_Stack::getTemplate(void) {
   return stack_template;
}

YAC_Object *TKS_Stack::push(void) {
   if(stack_index)
   {
      stack_index--;
      if(!stack_array[stack_index])
      {
         if(!stack_template)
         {
            Dprintf("[---] cannot Stack::push because template is not set.\n");
            return 0;
         }
         TKS_Context *ctx = tkscript->lockGlobalContext();
         stack_array[stack_index] = YAC_CLONE_POOLED(ctx, stack_template); // // xxx TKS_MT: use *real* thread context (no c'tors otherwise)
         tkscript->unlockGlobalContext();
      }
      return stack_array[stack_index];
   }
   else
   {
      Dprintf("[---] Stack::push overflow.\n");
      return 0;
   }
}

YAC_Object *TKS_Stack::pop(void) {
   if(stack_index!=stack_size)
   {
      stack_index++;
      if(stack_index!=stack_size)
         return stack_array[stack_index];
   }
   else
   {
      Dprintf("[---] Stack::pop overflow.\n");
   }
   return 0;
}

sSI TKS_Stack::setSize(sSI _i) {
   if(!stack_size)
   {
      if(_i>0)
      {
         stack_array=new YAC_Object*[_i];
         if(stack_array)
         {
            stack_size=_i;
            int i;
            for(i=0; i<stack_size; i++)
            {
               stack_array[i]=0;
            }
            stack_index=stack_size;
            return 1;
         }
         else
         {
            Dprintf("[!!!] Stack::setSize: allocation failed (size=%i).\n", stack_size);
         }
      }
   }
   else
   {
      Dprintf("[!!!] Stack::setSize: stack already allocated.\n");
   }
   return 0;
}

sSI TKS_Stack::getSize(void) {
   return stack_size;
}

void TKS_Stack::setIndex(sSI _i) {
   if(_i>=0 && _i<=stack_size) stack_index=_i;
}

sSI TKS_Stack::getIndex(void) {
   return stack_index;
}

void TKS_Stack::init(YAC_Object *_t, sSI _i) {
   setTemplate(_t);
   setSize(_i);
}
