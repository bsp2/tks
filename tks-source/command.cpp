/// command.cpp
///
/// (c) 1999-2013 Bastian Spiegel <bs@tkscript.de>
///    published under terms of the GNU general public license (GPL).
///
///
///

extern "C" {
#include <stdlib.h>
#include <stdio.h>
}

#include "tks.h"

YAC_CommandY::YAC_CommandY(void) {
   initCommand();

   b_method        = YAC_FALSE;
}

void YAC_CommandY::initCommand(void) {
	y_arg_class_IDs = NULL;
   y_adr           = NULL;
   y_retarg_cmd    = NULL;
   y_retval_cmd    = NULL;
   y_retself_cmd   = NULL;
   root_name       = NULL;

#ifdef TKS_PROFILE_FUNCTIONS
   profile_millisec  = 0.0;
   profile_callcount = 0;
   profile_classname = NULL;
#endif // TKS_PROFILE_FUNCTIONS
}

void YAC_CommandY::initMethod(void) { 
   initCommand();

   b_method        = YAC_TRUE;
} 
 
YAC_CommandY::~YAC_CommandY() {
	if(NULL != y_arg_class_IDs)
	{
		delete [] y_arg_class_IDs;
		y_arg_class_IDs = NULL;
	}

   if(NULL != y_retarg_cmd)
   {
      delete y_retarg_cmd;
      y_retarg_cmd = NULL;
   }

   if(NULL != y_retval_cmd)
   {
      delete y_retval_cmd;
      y_retval_cmd = NULL;
   }

   if(NULL != y_retself_cmd)
   {
      delete y_retself_cmd;
      y_retself_cmd = NULL;
   }

   if(NULL != root_name)
   {
      yac_host->yacDelete(root_name);
      root_name = NULL;
   }
}

void YAC_CommandY::determineCPPCallStyle(sBool _bRARG) {
	y_callstyle = (_bRARG || (y_arg_num>0)) + (y_return_type<<1);
}

YAC_CommandListY::YAC_CommandListY(void) {
	y_num_commands = 0;
	y_commands     = NULL;
}

YAC_CommandListY::~YAC_CommandListY() { 
	if(NULL != y_commands)
	{
		delete [] y_commands;
		y_commands = NULL;
	}
}

void YAC_CommandY::execRead(YAC_Object *_o, YAC_Value *_r) const {
	switch(y_return_type)
	{
	case 1:
		_r->initInt(((sSI(YAC_CALL*)(void*))y_adr)((void*)_o));
		break;

	case 2:
		_r->initFloat(((sF32(YAC_CALL*)(void*))y_adr)((void*)_o));
		break;

	case 3:
		_r->initObject(((YAC_Object*(YAC_CALL*)(void*))y_adr)((void*)_o), 0);
		break;

	case 4:
		((void(YAC_CALL*)(void*, YAC_Value*))y_adr)((void*)_o, _r);
		break;
	}

   Dfixfpuflags;
}
