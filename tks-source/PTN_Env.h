/// PTN_Env.h
///
/// (c) 2001-2026 Bastian Spiegel <bs@tkscript.de>
///     - distributed under terms of the Lesser GNU General Public License (LGPL)
///

#ifndef PTN_ENV_H__
#define PTN_ENV_H__


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
      continue_flag = YAC_TRUE;
   }

   // Copy constructor
   PTN_Env(TKS_Context *_context) {
      context       = _context;
      continue_flag = YAC_TRUE;
   }

   void initDefault(void); // => continue_flag=1, context=yacContextGetDefault()
};


#endif // PTN_ENV_H__
