/// inc_lib.h
///
/// (c) 2001-2026 Bastian Spiegel <bs@tkscript.de>
///     - distributed under terms of the Lesser GNU General Public License (LGPL)
///
///

#ifdef __cplusplus
extern "C" {
#endif

#ifndef NULL
#define NULL 0
#endif // NULL

typedef struct tks_lib_value_s {
	union __my_value {
		float   float_val;
		int     int_val;
		void   *object_val;
		void   *string_val;
      void   *any;
	} value;
   int type;  // 0=void  1=int32   2=float32  3=object(ref)  4=string(ref)
} tks_lib_value_t;

extern void tks_lib_value_init_void   (tks_lib_value_t *_v);
extern void tks_lib_value_init_int    (tks_lib_value_t *_v, int _intVal);
extern void tks_lib_value_init_float  (tks_lib_value_t *_v, float _floatVal);
extern void tks_lib_value_init_object (tks_lib_value_t *_v, void *_objectVal);

typedef void *tks_lib_context_t;
typedef void *tks_lib_module_t;
typedef void *tks_lib_function_t;
typedef void *tks_lib_variable_t;

// initialize scriptengine
extern void tks_lib_init (int _argc, char **_argv);

// run script (.tks) or script executable (.tkx)
// call tks_lib_init() script function (if it exists)
// returns 1 if script has been loaded successfully, 0 otherwise
//  (note) the script stays in memory
//  (note) the script may simple declare classes and not run any main() / global code
extern int tks_lib_run (const char *_pathName);

// get default script execution context (multi-threading)
extern tks_lib_context_t tks_lib_get_default_context (void);

// create script execution context (multi-threading)
extern tks_lib_context_t tks_lib_create_context (void);

// set default script execution context (multi-threading)
extern void tks_lib_set_default_context (tks_lib_context_t _context);

// destroy script execution context (multi-threading)
extern void tks_lib_destroy_context (tks_lib_context_t _context);

// find a script function in the currently loaded script (.tks or .tkx)
//  (note) "MMyModule.MyFunction" addresses functions in specific modules
extern tks_lib_function_t tks_lib_find_function (const char *_funcName);

// compile new temporary script module
// returns module handle
extern tks_lib_module_t tks_lib_compile_module (const char *_source);

// delete temporary script module
extern void tks_lib_delete_module (tks_lib_module_t _moduleHandle);

// find a script function in a script module
extern tks_lib_function_t tks_lib_find_module_function (tks_lib_module_t _moduleHandle, const char *_funcName);

// find a script variable in a temporary script module
extern tks_lib_variable_t tks_lib_find_module_variable (tks_lib_module_t _moduleHandle, const char *_varName);

// find a script variable in a script function
extern tks_lib_variable_t tks_lib_find_function_variable (tks_lib_function_t _functionHandle, const char *_varName);

// get value of a script variable
//  (note) objects are always returned as (non-deletable) references
extern void tks_lib_get_variable (tks_lib_variable_t _variableHandle, tks_lib_value_t *_retValue);

// replace value a script variable
//  (note) if the script variable contains a deletable object value, it will be deleted
extern void tks_lib_set_variable (tks_lib_variable_t _variableHandle, tks_lib_value_t *_value);

// assign value to script variable
//  (note) if the script variable contains an object value, the new value will be assigned to the object
extern void tks_lib_assign_variable (tks_lib_variable_t _variableHandle, tks_lib_value_t *_value);

// eval function with no return value and up to 16 arguments
extern void tks_lib_eval_void_function (tks_lib_context_t      _contextOrNull,
                                        tks_lib_function_t     _functionHandle,
                                        unsigned int           _numArgs,
                                        const tks_lib_value_t *_args
                                        );

// eval function with integer return value and up to 16 arguments
extern int tks_lib_eval_int_function (tks_lib_context_t      _contextOrNull,
                                      tks_lib_function_t     _functionHandle,
                                      unsigned int           _numArgs,
                                      const tks_lib_value_t *_args
                                      );

// eval function with float return value and up to 16 arguments
extern float tks_lib_eval_float_function (tks_lib_context_t      _contextOrNull,
                                          tks_lib_function_t     _functionHandle,
                                          unsigned int           _numArgs,
                                          const tks_lib_value_t *_args
                                          );

// eval function with object(reference) return value and up to 16 arguments
//  (note) if the script function returns a _deletable_ object, it will be discarded and this function returns NULL
extern void *tks_lib_eval_object_function (tks_lib_context_t      _contextOrNull,
                                           tks_lib_function_t     _functionHandle,
                                           unsigned int           _numArgs,
                                           const tks_lib_value_t *_args
                                           );

// call tks_lib_exit script function (if it exists)
// print profiling results (if enabled)
// stop all contexts
extern void  tks_lib_stop (void);

// shutdown script engine
//  (delete all script modules, ..)
extern void  tks_lib_exit (void);

#ifdef __cplusplus
}
#endif
