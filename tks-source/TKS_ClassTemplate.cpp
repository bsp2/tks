/// TKS_ClassTemplate.cpp
///
/// (c) 2001-2020 Bastian Spiegel <bs@tkscript.de>
///     - distributed under terms of the GNU general public license (GPL).
///
///

#include "tks.h"
#include "Pool.h"
#include "ObjectPool.h"
#include "TKS_SignalEntry.h"
#include "TKS_ClassTemplate.h"
#include "TKS_StringIterator.h"


TKS_ClassTemplate::TKS_ClassTemplate(void) { init();  }

TKS_ClassTemplate::~TKS_ClassTemplate()    { clean(); }

void TKS_ClassTemplate::init(void) {
	template_object = 0;
	num_signals     = 0;
	flags           = 0;
	isplugin        = 0; 

	sU16 i; 
   for(i=0; i<513; i++) 
   {
      quad_hash_table[i] = 0;
   }
}

void TKS_ClassTemplate::clean(void) {
	if(!isplugin)
	{
		if(template_object)  
      { 
         delete template_object; 
      }
	}
	template_object=0;
	sUI i; for(i=0; i<513; i++) quad_hash_table[i]=0;
	class_name.empty();
}

sBool TKS_ClassTemplate::setTemplateY(YAC_Object *_template) {
	if(_template)
	{
		YAC_String ret_cls; ret_cls.visit((char*)_template->yacClassName());
		{
			clean();
			////class_name.yacCopy(&ret_cls);
         class_name.visit(&ret_cls);
			template_object = _template;
			sUI l = _template->yacMethodGetNum(); // at least operator

         commandsy.y_commands = new YAC_CommandY[l];
			commandsy.y_num_commands=l;
			const char **all_method_names         = _template->yacMethodGetNames();
			const sUI   *all_method_param_num     = _template->yacMethodGetNumParameters();
			const sUI  **all_method_param_types   = _template->yacMethodGetParameterTypes();
			const char***all_method_param_otypes  = _template->yacMethodGetParameterObjectTypes();
			const sUI   *all_method_return_types  = _template->yacMethodGetReturnTypes();
			const char **all_method_return_otypes = _template->yacMethodGetReturnObjectTypes();
			const void **all_method_adr           = _template->yacMethodGetAdr();
			sUI i;
			for(i=0; i<l; i++)
			{
            YAC_String mnRoot;
				////mnRoot.copy((const char*)all_method_names[i]);
            mnRoot.visit(all_method_names[i]);

            sUI numargs = all_method_param_num[i]; // may be adjusted if return object arg is used (RARG)

				YAC_CommandY *com = &commandsy.y_commands[i];

            // Handle >=0.9.9.0 =>, <=> style method variants (return argument/value)

            sBool bRARG  = (-1 != mnRoot.indexOf("_YAC_RARG"));
            sBool bRVAL  = (-1 != mnRoot.indexOf("_YAC_RVAL"));
            sBool bRSELF = (-1 != mnRoot.indexOf("_YAC_RSELF"));
            if(bRARG || bRVAL || bRSELF)
            {
               if((bRARG && bRVAL) || (bRARG && bRSELF) || (bRVAL && bRSELF))
               {
                  Dprintf("[---] register class method \"%s::%s\": method variant must be either RARG, RVAL or RSELF.\n", 
                     (char*)class_name.chars, (char*)mnRoot.chars
                     );
                  return 0;
               }
               
               if(bRARG)
               {
                  mnRoot.replace("_YAC_RARG", "");
               }
               else if(bRVAL)
               {
                  mnRoot.replace("_YAC_RVAL", "");
               }
               else if(bRSELF)
               {
                  mnRoot.replace("_YAC_RSELF", "");
               }
               
               // Validate function signature
               if(bRVAL)
               {
                  if(4 != all_method_return_types[i])
                  {
                     Dprintf("[---] register class method \"%s::%s\": RVAL function variant must have variable return type (YAC_Value *_r).\n", 
                        (char*)class_name.chars, (char*)mnRoot.chars
                        );
                     return 0;
                  }
               }
               else if(bRARG)
               {
                  if(0 == numargs)
                  {
                     Dprintf("[---] register class method \"%s::%s\": last argument to RARG function variant must be YAC_Object* (no args found).\n", 
                        (char*)class_name.chars, (char*)mnRoot.chars
                        );
                     return 0;
                  }
                  else 
                  {
                     if(0 != all_method_return_types[i])
                     {
                        Dprintf("[---] register class method \"%s::%s\": RARG function variant must have void return type.\n", 
                           (char*)class_name.chars, (char*)mnRoot.chars
                           );
                        return 0;
                     }
                     // Decrease argcount since the last argument is actually the return type!
                     numargs--; 
                     if(all_method_param_types[i][numargs] < YAC_TYPE_OBJECT)
                     {
                        Dprintf("[---] register class method \"%s::%s\": last argument to RARG function variant must be YAC_Object* (return arg has wrong type).\n", 
                           (char*)class_name.chars, (char*)mnRoot.chars
                           );
                        return 0;
                     }
                  }
               }
               
               // Find or create root method entry if it doesn't already exist
               YAC_CommandY *cRoot = findCommandY2(&mnRoot);
               if(NULL == cRoot)
               {
                  cRoot = com; // Current method slot becomes root slot for =>, <=> method variants

                  cRoot->root_name = new YAC_String();
                  ////cRoot->root_name->yacCopy(&mnRoot);
                  cRoot->root_name->refCopy(&mnRoot);
                  
                  cRoot->y_adr          = NULL; 
                  cRoot->y_name         = (const char*) cRoot->root_name->chars; 
                  cRoot->y_return_type  = 0; 
                  cRoot->y_return_otype = 0; 
                  cRoot->y_arg_num      = numargs; 
                  cRoot->y_arg_types    = NULL; 
                  cRoot->y_arg_otypes   = NULL; 
                  cRoot->y_callstyle    = 0; 
                  cRoot->b_method       = 1; // Delete retarg, retval YAC_CommandY pointer in d'tor

                  addToQuadHash(i, &mnRoot);

                  com = NULL;
                  
               }
               // Found/created root method entry
               if(bRARG)
               {
                  if(NULL == cRoot->y_retarg_cmd)
                  {
                     if(numargs == cRoot->y_arg_num)
                     {
                        com = cRoot->y_retarg_cmd = new YAC_CommandY();
                     }
                     else
                     {
                        Dprintf("[---] register class method \"%s::%s\": RARG method variant has different argcount than root function entry (%u != %u).\n", 
                           (char*)class_name.chars, (char*)mnRoot.chars, numargs, cRoot->y_arg_num
                           );
                        return 0;
                     }
                  }
                  else
                  {
                     Dprintf("[---] register class method \"%s::%s\": RARG method variant already registered.\n", 
                        (char*)class_name.chars, (char*)mnRoot.chars
                        );
                     return 0;
                  }
               }
               else if(bRVAL)
               {
                  if(NULL == cRoot->y_retval_cmd)
                  {
                     if(numargs == cRoot->y_arg_num)
                     {
                        com = cRoot->y_retval_cmd = new YAC_CommandY();
                     }
                     else
                     {
                        Dprintf("[---] register class method \"%s::%s\": RVAL method variant has different argcount than root function entry (%u != %u).\n", 
                           (char*)class_name.chars, (char*)mnRoot.chars, numargs, cRoot->y_arg_num
                           );
                        return 0;
                     }
                  }
                  else
                  {
                     Dprintf("[---] register class method \"%s::%s\": RVAL method variant already registered.\n", 
                        (char*)class_name.chars, (char*)mnRoot.chars
                        );
                     return 0;
                  }
               }
               else if(bRSELF)
               {
                  if(NULL == cRoot->y_retself_cmd)
                  {
                     if(numargs == cRoot->y_arg_num)
                     {
                        com = cRoot->y_retself_cmd = new YAC_CommandY();
                     }
                     else
                     {
                        Dprintf("[---] register class method \"%s::%s\": RSELF method variant has different argcount than root function entry (%u != %u).\n", 
                           (char*)class_name.chars, (char*)mnRoot.chars, numargs, cRoot->y_arg_num
                           );
                        return 0;
                     }
                  }
                  else
                  {
                     Dprintf("[---] register class method \"%s::%s\": RSELF method variant already registered.\n", 
                        (char*)class_name.chars, (char*)mnRoot.chars
                        );
                     return 0;
                  }
               }

            } // if(bRARG || bRVAL || bRSELF)



            // Initialize method slot (or retarg/retval member of root method slot if method uses RARG/RVAL/RSELF call style)
				com->initMethod();
				com->y_name         = all_method_names[i];
				com->y_arg_num      = numargs; /////all_method_param_num[i];
				com->y_arg_types    = all_method_param_types[i];
				com->y_arg_otypes   = all_method_param_otypes[i];
				com->y_return_type  = all_method_return_types[i];
				com->y_return_otype = all_method_return_otypes[i];
				com->y_adr          = all_method_adr[i];
				com->determineCPPCallStyle(bRARG);

#ifdef TKS_PROFILE_FUNCTIONS
            com->profile_classname = _template->yacClassName();
#endif // TKS_PROFILE_FUNCTIONS


            // Sanity check (debug?)
            if(numargs && !com->y_arg_types)
            {
               Dprintf("[---] C++ class \"%s\" method \"%s\" has %i args but does not export y_arg_types array.\n", 
                  class_name.chars,
                  com->y_name,
                  com->y_arg_num
                  );
               // Failed
               return 0;
            }

            if(com == &commandsy.y_commands[i]) // Only add root methods to hash table
            {
               addToQuadHash(i, &mnRoot);
            }

         } // for i < numMethods

			// get signal names and RTTIs
			YAC_String s;
			_template->yacGetSignalStringList(&s);
			TKS_StringIterator is;
			s.words(0);
			is.begin(&s);
			sU8 si=0;
			if(is.tail())
			{
				do
				{
					if(si<TKS_MAXSIGNALS)
					{
						sS32 ddi=is.current->indexOf(":");
						if(ddi==-1)
						{
							signals[si].name.yacCopy(is.current); // copy signame
							signals[si].rtti=0;
						}
						else
						{
							is.current->substring(&signals[si].name, 0, ddi); // extract signame
							if(((sS32)is.current->length)>ddi)
                     {
   							YAC_String t;
								is.current->substring(&t, ddi+1, (((sS32)is.current->length)-ddi-1)); // extract sigrtti
							   sSI srtti;
							   t.yacScanI(&srtti);
							   signals[si].rtti=srtti;
                     }
                     else
                     {
#ifdef TKS_DCON
                        printf("[---] TKS_ClassTemplate::setTemplateY: assertion \"(((sS32)is.current->length)>ddi)\" failed.\n");
#endif
                        signals[si].rtti= 0;//xxx
                     }
						}
						num_signals++;
					}
					si++;
				} while(is.previous());
			}
			is.end();
			s.freeStack();

         pool_priority = _template->yacPoolGetPriority();
         if(pool_priority >= YAC_NUM_POOL_PRIORITIES)
         {
#ifdef TKS_DCON
            printf("[~~~] TKS_ClassTemplate::setTemplateY: class \"%s\" requests invalid pool priority %u. Setting lowest priority.\n", 
               (char*)class_name.chars, 
               pool_priority
               );
#endif
            pool_priority = 0;
         }
         pool_size = _template->yacPoolGetSize();
         pool_size = ((pool_size + (ObjectPool::POOL_GRANULARITY-1)) / ObjectPool::POOL_GRANULARITY); // align to dword (almost certainly done by the c++ compiler already)
         if(pool_size >= ObjectPool::NUM_POOL_SLOTS)
         {
#ifdef TKS_DCON
            printf("[~~~] TKS_ClassTemplate::setTemplateY: class \"%s\" requests invalid pool size %u. Disabling pooling for this class.\n", 
               (char*)class_name.chars, 
               _template->yacPoolGetSize()
               );
#endif
            pool_size = 0;
         }
			return 1;
		}
	}
	return 0;
}

void TKS_ClassTemplate::addToQuadHash(sUI _i, YAC_String *_mnRoot) {
   //
   // Add method to "quad_hash_table"
   //
   sUI h = (_mnRoot->getKey() & 0x000Fffffu) | (_i << 20)/*method idx*/;
   sUI *qp = &quad_hash_table[h & 511u];
   while(*qp) 
      qp++;
   *qp = h;
}

YAC_CommandY *TKS_ClassTemplate::findCommandY2(YAC_String *_s) const {
   sUI h = (_s->getKey() & 0x000Fffffu);
   const sUI *qp = &quad_hash_table[h & 511u];
   YAC_String t;
   do
   {
      if((*qp & 0x000fFFFFu) == h)
      {
         sUI ci = (*qp >> 20);
         if(ci < commandsy.y_num_commands)
         {
            YAC_CommandY *c = &commandsy.y_commands[ci];
            t.visit((char*)c->y_name);
            if(t.compare(_s)) return c;
         }
         else
         {
            return NULL;
         }
      }
   } 
   while(*++qp);
   return NULL;
}

const YAC_CommandY *TKS_ClassTemplate::findCommandY(YAC_String *_s) const {
	const YAC_CommandY *ret = NULL;
	if(_s)
	{
		ret = findCommandY2(_s);
		if(!ret)
		{
			// ---- search base classes ----
			sU8 *tcmap = &tkscript->cpp_typecast_map[template_object->class_ID][0];
			sUI i; 
         for(i=0; (i<YAC_MAX_CLASSES) && !ret; i++)
			{
				if(tcmap[i])
				{
					TKS_ClassTemplate *bclt=TKSCRIPT__ATOM_SPEEDPATH[i];
					if(bclt)
               {
						ret = bclt->findCommandY2(_s);
               }
				}
			}
		}
	}
	return ret;
}
