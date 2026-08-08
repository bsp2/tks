
#include "../inc_lib.h"

int main(int argc, char**argv) {

   tks_lib_init(argc, argv);
   tks_lib_run("test.tks");

   void *f = tks_lib_find_function("Main.MyFunction");
   tks_lib_eval_void_function(f);

   tks_lib_stop();
   tks_lib_exit();
}
