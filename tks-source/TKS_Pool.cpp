/// TKS_Pool.cpp
///
/// (c) 2001-2010 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///

#include "tks.h"
#include "Pool.h"
#include "TKS_Pool.h"
#include "TKS_PoolIterator.h"
#include "PTN_Node.h"

sBool YAC_VCALL TKS_Pool::yacIteratorInit(YAC_Iterator *_it) const {
   Diteratorinit(TKS_PoolIterator);
   return 1;
}

TKS_Pool::TKS_Pool(void) {
   class_ID        = TKS_CLID_POOL; 
   fix_stringtype  = 0;
   init_classtype  = 0;
   template_object = 0;
}

TKS_Pool::~TKS_Pool(void) {
   fix_stringtype  = 0;
   init_classtype  = 0;
   template_object = 0; 
   TKS_Pool::_free();
}

void YAC_VCALL TKS_Pool::yacArrayGet(void *_context, sUI _id, YAC_Value *_r) {
   YAC_Object *o = (YAC_Object*) (*this)[_id];
   
   if(o != NULL)
   {
      _r->initObject(o, 0);
   }
   else
   {
      YAC_String *msg = (YAC_String*) YAC_NEW_CORE_POOLED(YAC_CLID_STRING128);
      //msg->alloc(96);
      msg->printf("Pool id >>%i<< is not in use", _id);
      PTN_Env env; env.continue_flag=1; env.context=(TKS_Context*)_context;
      PTN_Env*_env = &env;
      Drtthrow(NULL, TKS_EXCEPTION_READARRAYOUTOFBOUNDS, msg);
   }
} 

void YAC_VCALL TKS_Pool::yacArraySet(void *_context, sUI _id, YAC_Value *_value) { 
   YAC_Object *o = (YAC_Object*) (*this)[_id]; 

   if(o != NULL) 
   { 
      YAC_Value r;
      YAC_Value value; value = _value;
      if(value.type < YAC_TYPE_OBJECT)
      {
         value.typecast(YAC_TYPE_OBJECT);
      }
      o->yacOperator(YAC_OP_ASSIGN, value.value.object_val, &r);
      value.unsetFast();
      ////o->yacOperatorAssign(_value->value.object_val); 
   } 
   else
   {
      YAC_String *msg = (YAC_String*) YAC_NEW_CORE_POOLED(YAC_CLID_STRING128);
      ////msg->alloc(96);
      msg->printf("Pool id >>%i<< is not in use", _id);
      PTN_Env env; env.continue_flag=1; env.context=(TKS_Context*)_context;
      PTN_Env*_env = &env;
      Drtthrow(NULL, TKS_EXCEPTION_WRITEARRAYOUTOFBOUNDS, msg);
   }
}

void YAC_VCALL TKS_Pool::yacOperator(sSI _cmd, YAC_Object *_o, YAC_Value *_r) {
   if(_o!=((YAC_Object*)this))
   {
      if(_cmd==YAC_OP_ASSIGN)
      {
         /// xxx copy pool
         return;
      }
   }
   YAC_Object::yacOperator(_cmd, _o, _r);
}

void YAC_VCALL TKS_Pool::yacSerialize(YAC_Object *_ofs, sUI _rtti) {
   YAC_BEG_SERIALIZE();
   
   /// ---- first write template class name ----
   if(template_object)
   {
      /// ---- TKS classes start with '@' ----
      template_object->yacSerializeClassName(_ofs);
   }
   else
   {
      YAC_String tcln; 
      tcln.yacSerialize(_ofs, 0); // serialize empty string		
      return;
   }
   
   // ---- store pool size ----
   YAC_SERIALIZE_I16((sU16)max_elements);
   YAC_SERIALIZE_I16((sU16)used_elements);
   if(used_elements)
   {
      sSI i;
      for(i=0; i<used_elements; i++)
      {
         // ---- write single object ----
         YAC_Object *o = (YAC_Object*) elements[uselist[i]];
         o->yacSerialize(_ofs, 0);
      }
   }
}

sUI YAC_VCALL TKS_Pool::yacDeserialize(YAC_Object *_ifs, sUI _rtti) {
   YAC_BEG_DESERIALIZE();
   
   _free();
   
   /// ---- first read class name and find suitable template object ----
   YAC_String cln;
   //cln.setEngine(tkscript);
   cln.yacDeserialize(_ifs, 0);
   if(cln.isBlank())
   {
      return 1; // ---- error in serialization, no template was set ----
   }
   template_object = tkscript->findTemplateObjectByUniqueClassName(&cln);
   if(!template_object)
   {
      Dprintf("[---] Pool::deserialize: cannot find template for class \"%s\".\n",
         cln.chars);
      return 0; // ---- cannot find class ----
   }
   
   sUI me=YAC_DESERIALIZE_I16();
   sUI ne=YAC_DESERIALIZE_I16();
   
   if(((sUI)me)>=tkscript->configuration.streamMaxArraySize)
   {
      Dprintf("[---] Pool::deserialize: insane array size (maxElements) (%i>%i)\n",
         me, tkscript->configuration.streamMaxArraySize);
      return 0;
   }
   
   if(((sUI)ne)>=tkscript->configuration.streamMaxArraySize)
   {
      Dprintf("[---] Pool::deserialize: insane array size (numElements) (%i>%i)\n",
         ne, tkscript->configuration.streamMaxArraySize);
      return 0;
   }
   
   if(_alloc(me))
   {
      // ---- now restore objects ----
      sUI i;
      for(i=0; i<ne; i++)
      {
         /// ---- deserialize single object ----
         sSI eid=qAlloc();
         if(! ((YAC_Object*)elements[eid])->yacDeserialize(_ifs, 0))
         {
            return 0;
         }
      }
   }
   else
   {
      Dprintf("[---] Pool::deserialize: failed to alloc %i elements.\n", me);
      return 0;
   }
   
   /// max_elements/used_elements *should* be restored now
   return 1;
}


void TKS_Pool::setTemplate(YAC_Object *_o) {
   if(_o)
   {
      template_object=_o;
      fix_stringtype=(template_object->class_ID==YAC_CLID_STRING);
      init_classtype=(template_object->class_ID==YAC_CLID_CLASS);
   }
}

YAC_Object *TKS_Pool::getTemplate(void) {
   return template_object;
}

sSI TKS_Pool::_alloc(sSI _max) {
   if(template_object)
   {
      if(Pool::allocElements(_max))
      {
         int i;
         YAC_Object **op = (YAC_Object**) elements;
         TKS_Context *ctx = tkscript->lockGlobalContext();
         for(i=0; i<_max; i++) 
         {
            op[i] = YAC_CLONE_POOLED(ctx, template_object); // // xxx TKS_MT: use *real* thread context, reset objects in qAlloc() (no c'tors available without thread context)
         }
         tkscript->unlockGlobalContext();
         return 1;
      }
   }
   return 0;
}	

void TKS_Pool::_free(void) {
   int i;
   for(i=0; i<max_elements; i++)
   {
      if(NULL != elements[i])
      {
         YAC_DELETE( (YAC_Object*)elements[i] );
         elements[i] = NULL;
      }
   }
   Pool::freeElements();
}


sSI TKS_Pool::getNumElements(void) {
   return used_elements;
}

sSI TKS_Pool::getMaxElements(void) {
   return max_elements-1;
}

void TKS_Pool::_qFreeByObject(YAC_Object *_o) {
   int id=Pool::getIDByObject(_o);
   if(id!=-1) Pool::qFree(id);
}
