/// PTN_Env.h
///
/// (c) 2001-2009 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///
#ifndef __PTN_ENV_H__
#define __PTN_ENV_H__


// ---- "Environment" which is passed to each script statement
class PTN_Env {
public:
   YAC_Value    cret;          // Current function/method return value (e.g. "return = <expr>;")
                               // Note: "cret" cannot be part of "context" or recursive fun calls would not work correctly
   sSI          continue_flag; // Used for break/return
   TKS_Context *context;       // Context handle (stacks, this-object, exceptions, ..)

   // NOP constructor
   PTN_Env(void) { }


   // Copy constructor
   PTN_Env(PTN_Env *_env) { 
      context       = _env->context;
      continue_flag = 1; 
   }

   // Copy constructor
   PTN_Env(TKS_Context *_context) { 
      context       = _context;
      continue_flag = 1; 
   }

   void initDefault(void); // => continue_flag=1, context=yacContextGetDefault()
};


#endif // __PTN_ENV_H__
