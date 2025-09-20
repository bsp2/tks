     YAC_Value::YAC_Value                                    (void)                                             {type=0;}
     YAC_Value::~YAC_Value                                   ()                                                 { }
void YAC_Value::initVoid                                     (void)                                             {deleteme=0;type=0;value.any=0;}
void YAC_Value::initInt                                      (sSI _i)                                           {deleteme=0;type=1;value.int_val=_i;}
void YAC_Value::initFloat                                    (sF32 _f)                                          {deleteme=0;type=2;value.float_val=_f;}
void YAC_Value::initString                                   (YAC_String *_s, sBool _del)                       {deleteme=_del;type=4;value.string_val=_s;}
#ifndef YAC_NO_HOST
void YAC_Value::initNewString                                (YAC_String *s)                                    {unsetFast(); type=4; deleteme=1; value.string_val=YAC_New_String(); value.string_val->yacCopy(s); }
#endif // YAC_NO_HOST
void YAC_Value::initObject                                   (YAC_Object *_o, sBool _del)                       {deleteme=_del;type=3;value.object_val=_o;}
void YAC_Value::initNull                                     (void)                                             {unsetFast(); type=YAC_TYPE_OBJECT; value.any = 0; deleteme = 0; }
void YAC_Value::safeInitInt                                  (sSI _i)                                           {unsetFast(); initInt(_i); }
void YAC_Value::safeInitFloat                                (sF32 _f)                                          {unsetFast(); initFloat(_f); }
void YAC_Value::safeInitString                               (YAC_String *_s, sBool _del)                       {unsetFast(); initString(_s, _del); }
void YAC_Value::safeInitObject                               (YAC_Object *_o, sBool _new)                       {unsetFast(); initObject(_o, _new); }
#ifndef YAC_NO_HOST
void YAC_Value::initEmptyString                              (void)                                             {deleteme=1; type=YAC_TYPE_STRING; value.string_val=(YAC_String*)YAC_NEW_CORE_POOLED(YAC_CLID_STRING);}
#else
void YAC_Value::initEmptyString                              (void)                                             {deleteme=1; type=YAC_TYPE_STRING; value.string_val=YAC_New_String();}
#endif // YAC_NO_HOST
#ifndef YAC_NO_HOST
void YAC_Value::typecast                                     (sUI _type)                                        {
   sSI         i_t;
   sF32        f_t;
   switch(_type)
   {
   default:
      unset();
      break;
   case 1:
      switch(type)
      {
      default:
      case 0:
         initInt(0);
         break;
      case 1:
         break;
      case 2:
         initInt((sSI)value.float_val);
         break;
      case 3:
         //i_t=(value.object_val!=0);
         //safeInitInt(i_t);
         if(value.object_val)
         {
            value.object_val->yacScanI(&i_t);
            safeInitInt(i_t);
         }
         else
            initInt(0);
         break;
      case 4:
         if(value.string_val)
         {
            ////if(value.string_val->checkFloatConversion())
            {
               value.string_val->yacScanF32(&f_t);
               safeInitInt((sSI)f_t);
            }
            ////else
            //// {
            ////   value.string_val->yacScanI(&i_t);
            ////   safeInitInt(i_t);
            ///}
         }
         else
         {
            safeInitInt(0);
         }
         break;
      }
      break;
      case 2:
         switch(type)
         {
         default:
         case 0:
            initFloat(0.0f);
            break;
         case 1:
            initFloat((sF32)value.int_val);
            break;
         case 2:
            break;
         case 3:
         case 4:
            // ---- convert object to float value
            if(value.object_val)
               value.object_val->yacScanF32(&f_t);
            else
               f_t=0.0f;
            safeInitFloat(f_t);
            break;
         }
         break;
         case 3:
            switch(type)
            {
            default:
            case YAC_TYPE_VOID:
               // ---- typecast void to "null" Object
               initObject(NULL, 0);
               break;
            case YAC_TYPE_INT:
               // ---- typecast int to Integer Object
               {
                  YAC_Integer *io = (YAC_Integer*) YAC_NEW_CORE_POOLED(YAC_CLID_INTEGER);
                  io->value=value.int_val;
                  initObject(io, 1);
               }
               break;
            case YAC_TYPE_FLOAT:
               // ---- typecast float to Float Object
               {
                  YAC_Float *fo = (YAC_Float*) YAC_NEW_CORE_POOLED(YAC_CLID_FLOAT);
                  fo->value=value.float_val;
                  initObject(fo, 1);
               }
               break;
            case YAC_TYPE_OBJECT:
            case YAC_TYPE_STRING:
               break;
            }
            break;
            case 4:
               {
                  //YAC_String *s;
                  switch(type)
                  {
                  default:
                  case YAC_TYPE_VOID:
                     initEmptyString();
                     break;
                  case YAC_TYPE_INT:
                     i_t = value.int_val;
                     value.string_val = (YAC_String*) YAC_NEW_CORE_POOLED(YAC_CLID_STRING16);
                     value.string_val->printf("%i", i_t);
                     deleteme=1;
                     type=YAC_TYPE_STRING;
                     break;
                  case YAC_TYPE_FLOAT:
                     f_t = value.float_val;
                     value.string_val = (YAC_String*) YAC_NEW_CORE_POOLED(YAC_CLID_STRING32);
                     value.string_val->printf("%g", f_t);
                     deleteme=1;
                     type=YAC_TYPE_STRING;
                     break;
                  case YAC_TYPE_OBJECT:
                     if(YAC_VALID(value.object_val))
                     {
                        if(value.object_val->class_ID!=YAC_CLID_STRING) // string fix?
                        {
                           YAC_String *s = (YAC_String*) YAC_NEW_CORE_POOLED(YAC_CLID_STRING);
                           value.object_val->yacToString(s); // convert Object to String
                           safeInitString(s, 1);
                        }
                     }
                     else
                     {
                        // ---- convert null object to empty String ""
                        initString((YAC_String*) YAC_NEW_CORE_POOLED(YAC_CLID_STRING), 1);
                     }
                     break;
                  case YAC_TYPE_STRING:
                     break;
                  }
               }
               break;
   }
}
#endif // YAC_NO_HOST

void YAC_Value::unsetFast                                    (void)                                             {if(type>=YAC_TYPE_OBJECT&&deleteme&&value.any)YAC_DELETE(value.object_val);}
void YAC_Value::unset                                        (void)                                             {if(type>=YAC_TYPE_OBJECT&&deleteme&&value.any)YAC_DELETE(value.object_val);value.object_val=0;type=0;deleteme=0;}
void YAC_Value::operator =                                   (YAC_Value *_v)                                    {deleteme=_v->deleteme;_v->deleteme=0;type=_v->type;value=_v->value;};
void YAC_Value::copyRef                                      (const YAC_Value *_o)                         {type=_o->type;class_type=_o->class_type;value.any=_o->value.any;deleteme=0;}
