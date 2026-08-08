/// inc_lib.h
///
/// (c) 2001-2026 Bastian Spiegel <bs@tkscript.de>
///     - distributed under terms of the GNU general public license (GPL).
///
///

#ifdef __cplusplus
extern "C" {
#endif

void  tks_lib_init (int _argc, char **_argv);
int   tks_lib_run (const char *_pathName);
void *tks_lib_find_function (const char *_funcName);
void  tks_lib_eval_void_function (void *_functionHandle);
void tks_lib_stop (void);
void tks_lib_exit (void);

#ifdef __cplusplus
}
#endif
