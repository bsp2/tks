/// inc_lib.h
///
/// (c) 2001-2026 Bastian Spiegel <bs@tkscript.de>
///     - distributed under terms of the GNU general public license (GPL).
///
///

#ifdef __cplusplus
extern "C" {
#endif

extern void  tks_lib_init (int _argc, char **_argv);
extern int   tks_lib_run (const char *_pathName);
extern void *tks_lib_find_function (const char *_funcName);
extern void  tks_lib_eval_void_function (void *_functionHandle);
extern void  tks_lib_stop (void);
extern void  tks_lib_exit (void);

#ifdef __cplusplus
}
#endif
