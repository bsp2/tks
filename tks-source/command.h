/// command.h 
/// 
/// (c) 2001-2010 Bastian Spiegel <bs@tkscript.de> 
///     - distributed under terms of the GNU general public license (GPL). 
/// 

#ifndef __TKS_COMMAND_H__
#define __TKS_COMMAND_H__

 
class YAC_CommandY { 
   //
   // xxx TODO: handle case where retarg or retval or retself are defined but not the "root" method
   //            (e.g. TKS::evalMethodByName currently cannot handle this)
   //
public: 
	const char *  y_name; 
	sUI           y_arg_num; 
	const sUI  *  y_arg_types; 
	const char**  y_arg_otypes; 
	sUI        *  y_arg_class_IDs; 
	sUI           y_return_type; 
	const char *  y_return_otype; 
	sUI           y_return_class_ID; 
	const void *  y_adr; 
	sUI           y_callstyle; // 0=void, 1=yacmemptr _args, +2,4,6,8 return type 
   YAC_CommandY *y_retarg_cmd;  // pointer to <y_name>_$YAC_RARG variant or NULL
   YAC_CommandY *y_retval_cmd;  // pointer to <y_name>_$YAC_RVAL variant or NULL
   YAC_CommandY *y_retself_cmd; // pointer to <y_name>_$YAC_RSELF variant or NULL
   sBool         b_method; // 1=delete retarg/retval/retself in d'tor
   YAC_String   *root_name; // only used in classes
 
#ifdef TKS_PROFILE_FUNCTIONS
   sF64        profile_millisec;
   sUI         profile_callcount;
   const char *profile_classname;
#endif //TKS_PROFILE_FUNCTIONS

public:
	YAC_CommandY(void); 
	~YAC_CommandY(); 
 
   void initCommand (void);

	void initMethod            (void); // 2nd stage constructor 
	void determineCPPCallStyle (sBool _bRARG); 
	void execRead              (YAC_Object *_o, YAC_Value *_r) const;
}; 
 
class YAC_CommandListY { 
public: 
	sUI           y_num_commands; 
	YAC_CommandY *y_commands; 
 
	YAC_CommandListY(void); 
	~YAC_CommandListY(); 
}; 
 

#endif // __TKS_COMMAND_H__
