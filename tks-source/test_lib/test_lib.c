// test libtks
//  - initialize scriptengine (tks_lib_init())
//  - load script module from filesystem (test.tks) and evaluate global statements
//  - create new script context (could be skipped / use default context instead)
//  - resolve script module function "Main.MyFunction"
//  - evaluate it (prints a message)
//  - create temporary script module
//  - resolve "TempFunction"
//  - evaluate it (adds two numbers)
//  - print the return value
//  - delete temporary script module
//  - destroy script context
//  - stop script execution (could be skipped)
//  - shutdown scriptengine (tks_lib_exit())

#include <stdio.h>

#include "../inc_lib.h"

int main(int argc, char**argv) {

   tks_lib_init(argc, argv);
   tks_lib_run("test.tks");

   tks_lib_context_t ctx = tks_lib_create_context();
   tks_lib_set_default_context(ctx);
   tks_lib_function_t f = tks_lib_find_function("Main.MyFunction");
   tks_lib_eval_void_function(ctx, f, 0, NULL);

   tks_lib_module_t m = tks_lib_compile_module(
      "// temp module test \n"
      " \n"
      "function TempFunction(int a, float b) { \n"
      "  return a + b; \n"
      "} \n"
                                               );
   f = tks_lib_find_module_function(m, "TempFunction");
   tks_lib_value_t args[2];
   tks_lib_value_init_int  (args+0, 42);
   tks_lib_value_init_float(args+1, 3.1415f);
   float rf = tks_lib_eval_float_function(ctx, f, 2, args);
   printf("TempFunction() returned %f\n", rf);

   tks_lib_delete_module(m);

   tks_lib_destroy_context(ctx);

   tks_lib_stop();
   tks_lib_exit();
}
