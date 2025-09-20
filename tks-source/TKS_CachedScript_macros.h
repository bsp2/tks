/// TKS_CachedScript_macros.h 
/// 
/// author : (c) 2001-2008 Bastian Spiegel <bs@tkscript.de> 
///    published under terms of the GNU general public license (GPL). 
///
/// this was put into a separate file in order to not confuse the emacs c++ mode .>

#define Dpushstate pushState()
#define Dpopstate popState()
#define Dungetc toki--; stok--
#define Dungetstr toki-=2; stok-=2
#define Dungettok if(tok==TOKEN_VAR) { Dungetstr; } else { Dungetc; }
#define Dsrcloc ((module_index<<16)|linenr)
#define Dsetsrcloc popnode.node->src_loc=((module_index<<16)|linenr); /*popnode.node->tkscript=tkscript*/
//#define Dsetsrcloct tnode.node->src_loc=((module_index<<16)|linenr); /*tnode.node->tkscript=tkscript*/
#define Dsetparami(p,v) state_stack[stp].param[p].value=v
#define Dgetparami(p) state_stack[stp].param[p].value
#define Dsetparamn(p,v) state_stack[stp].param[p].node.node=v
#define Dgetparamn(p) state_stack[stp].param[p].node 
#define Dsetparams(p,v) state_stack[stp].param[p].str=v  
#define Dgetparams(p) state_stack[stp].param[p].str
#define Dsetparamv(p,v) state_stack[stp].param[p].any=v;
#define Dgetparamv(p) state_stack[stp].param[p].any
#define Dsetparamnspace(p,v) state_stack[stp].param[p].any = (void*)v
#define Dgetparamnspace(p) (TKS_Namespace*)state_stack[stp].param[p].any
#define Dparseerror parseError((sU16)linenr/*, (sU16)state, (sU16)state_index*/, toki, (sU16)tok); return 0
//#define Dfindconst(p) co=0; co=tkscript->global_constant_namespace.findEntry(p) 
#define Dclassparseerror parseError(linenr/*, state, state_index*/, toki, tok); return 0
#define Dexptok(a) if(tok!=(a)) { Dparseerror; }
#define Dtryindirectobjectexpr_head \
                    if(ntok==TOKEN_DOT)\
                    {\
                       Dsetparamn(0, popnode.expr);\
                       state=ST_IEXPR;\
                       state_index=100;\
                       break;\
                    }\
                    else\
                    {\
                       if(ntok==TOKEN_BEGARRAY)\
                       {\
                          Dsetparamn(0, popnode.expr);\
                          state=ST_IARRAYEXPR;\
                          state_index=100;\
                          break;\
                       }
						  

#define Dtryindirectobjectexprnopopstate Dtryindirectobjectexpr_head  }

#define Dtryindirectobjectexpr Dtryindirectobjectexpr_head \
                       else\
                       {\
                          popState();\
                       }\
                    }

// ---- test whether class member a.b is accessible from current context 
#define DcheckMemberAccess(a,b) \
              if(!(a)->allowMemberAccess((b), this, cclassdecl, create_namespace))\
              {\
                 Dparseerror;\
              }
 
// ---- test whether current (class-) function/method may see class member "a". 
// ---- functions/methods shadow class members. 
// ---- class functions may not see (non-static) class members. 
// ---- if the test fails, the current (resolved) var is set to 0 so the parser  
// ---- falls back to local and global variables. 
#define DcheckFunMemberAccess(a) \
            if(cfunction) \
            {\
               if(cfunction->findFunVar(a)) \
               {\
                  co=0; \
               }\
               else if(cfunction->is_class_function)\
               {\
                  co=0; \
               }\
            } (void)0
 
 
 
// ---- test whether class method a.b is accessible from current context 
#define DcheckMethodAccess(a,b) \
				if(!(a)->allowMethodCall((b), this, cclassdecl, create_namespace))\
				  {\
				    Dparseerror;\
				  }
 
// ---- test whether class function a.b is accessible from current context 
#define DcheckFunctionAccess(a,b) \
				if(!(a)->allowFunctionCall((b), this, cclassdecl, create_namespace))\
				  {\
				    Dparseerror;\
				  }

// ---- get current globalvar cache (for creating of new variables), use module cache if default namespace is selected
#define Dglobalvars ( (create_namespace==&Dtkscompiler->default_namespace) ? module_vars : create_namespace->global_vars )

// ---- get current namespace array
#define Dfreezenspaces(a) \
   if(NULL == cnspace)\
   {\
     a = active_namespaces->freeze();\
   }\
   else\
   {\
      a = cnspace->freeze();\
      cnspace = NULL;\
   }\
   while(0)

// ---- find namespace
#define Dfindnspace \
   if(NULL != cnspace) \
   {\
      Dparseerror;\
   }\
   else\
   {\
      cnspace = tkscript->findNamespace(sv);\
      if(NULL == cnspace)\
      {\
         Dparseerror;\
      }\
   }

// ---- find namespace (within scanner pass)
#define Dclassfindnspace \
   if(NULL != cnspace) \
   {\
      Dclassparseerror;\
   }\
   else\
   {\
      cnspace = tkscript->findNamespace(sv);\
      if(NULL == cnspace)\
      {\
         tkscript->printf("[---] namespace \"%s\" does not exist.\n", (char*)sv->chars);\
         Dclassparseerror;\
      }\
   }

// ---- verify that no namespace is selected
#define Dassertnonspace \
   if(NULL != cnspace) \
   {\
      Dparseerror;\
   }

// ---- verify that no namespace is selected (within scanner pass)
#define Dclassassertnonspace \
   if(NULL != cnspace) \
   {\
      Dclassparseerror;\
   }

// ---- verify that no or the YAC namespace is selected (within scanner pass)
#define Dclassassertnonoryacspace \
   if(NULL != cnspace) \
   {\
      if(cnspace != &Dtkscompiler->yac_namespace)\
      {\
         Dclassparseerror;\
      }\
   }

// ---- select default namespace, e.g. ::MyClass
#define Ddefaultnspace \
   Dassertnonspace; \
   if(TOKEN_VAR != ntok) \
   {\
      Dparseerror; \
   }\
   cnspace = &Dtkscompiler->default_namespace;\
   break

// ---- select default namespace, e.g. ::MyClass
#define Dclassdefaultnspace \
   Dclassassertnonspace; \
   if(TOKEN_VAR != ntok) \
   {\
      Dclassparseerror; \
   }\
   cnspace = &Dtkscompiler->default_namespace;\
   break

// ---- check whether '::' follows and parse nspace. use substate 's' to skip '::' token
// ---- if current token is already '::' then set the default namespace and loop the current substate
#define Dparsenspace(s) \
   if(TOKEN_MEMBER == ntok) \
   {\
      Dfindnspace;\
      state_index = (s);\
      break; \
   }\
   else if(TOKEN_MEMBER == tok) \
   {\
      Ddefaultnspace;\
   }

// ---- check whether '::' follows and parse nspace. use substate 's' to skip '::' token (within scanner pass)
// ---- if current token is already '::' then set the default namespace and loop the current substate
#define Dclassparsenspace(s) \
   if(TOKEN_MEMBER == ntok) \
   {\
      Dclassfindnspace;\
      state_index = (s);\
      break; \
   }\
   else if(TOKEN_MEMBER == tok) \
   {\
      Dclassdefaultnspace;\
   }
