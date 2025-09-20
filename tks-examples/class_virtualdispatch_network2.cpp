
// Note: this example does NOT compile (is it valid C++ ? the error message is quite weird!)

/*

$ g++ test.cpp
test.cpp: In function 'int main(int, char**)':
test.cpp:106: error: request for member 'x' is ambiguous
test.cpp:10: error: candidates are: void Layer::x()
test.cpp:10: error:                 void Layer::x()

with msvc:
1>h:\tmp\testvdsp\testvdsp.cpp(64) : error C2385: ambiguous access of 'x'
1>        could be the 'x' in base 'Layer'
1>        or could be the 'x' in base 'Layer'

... :) ...

*/

#include <stdio.h>


class Layer {
public:
   virtual void consumeAction() {
      printf("Layer::consumeAction\n");
   }
   void x(void) { consumeAction(); }
};

class TableCellEditor : public Layer {
public:
   void y(void) { consumeAction(); }
};

class FloatParamEditor : public Layer {
public:
   void consumeAction(void) {
      printf("FloatParamEditor::consumeAction\n");
   }
   void z(void) { consumeAction(); }
};

class TableCellEditorFloatParam : public TableCellEditor, public FloatParamEditor {
public:

   void w(void) { TableCellEditor::consumeAction(); }
};


int main(int argc, char**_argv) {
   Layer l;
   l.consumeAction();
   l.x();

   TableCellEditor tce;
   tce.consumeAction();
   tce.x();
   tce.y();

   FloatParamEditor fpe;
   fpe.consumeAction();
   fpe.x();
   fpe.z();

   TableCellEditorFloatParam tcfpe;
   ////tcfpe.consumeAction();
   tcfpe.x();
   tcfpe.y();
   tcfpe.z();
   tcfpe.w();
}
