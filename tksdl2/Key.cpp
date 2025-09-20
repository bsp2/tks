/// Key.cpp
///
/// (c) 2001-2023 by Bastian Spiegel <bs@tkscript.de>
///     - Distributed under terms of the Lesser GNU General Public License (LGPL).
///       See COPYING and <http://www.gnu.org/licenses/licenses.html#LGPL> for further information.
///

#include <stdlib.h>

#define YAC_BIGSTRING
#include <yac.h>

#ifdef YAC_MACOS
// #define MACOS_RAW_KEY_TABLES  defined
#endif // YAC_MACOS

#include "inc_sdl.h"

#include "Key.h"
#include "HAL.h"

extern YAC_Host *yac_host;

#ifdef MACOS_RAW_KEY_TABLES
extern sUI macos_local_to_raw_key_table_af[100*2];
extern sUI macos_local_to_raw_key_table_al[100*2];
extern sUI macos_local_to_raw_key_table_at[100*2];
extern sUI macos_local_to_raw_key_table_au[100*2];
extern sUI macos_local_to_raw_key_table_az[100*2];
extern sUI macos_local_to_raw_key_table_be[100*2];
extern sUI macos_local_to_raw_key_table_bg[100*2];
extern sUI macos_local_to_raw_key_table_br[100*2];
extern sUI macos_local_to_raw_key_table_by[100*2];
extern sUI macos_local_to_raw_key_table_ca[100*2];
extern sUI macos_local_to_raw_key_table_ch[100*2];
extern sUI macos_local_to_raw_key_table_cn[100*2];
extern sUI macos_local_to_raw_key_table_cz[100*2];
extern sUI macos_local_to_raw_key_table_de[100*2];
extern sUI macos_local_to_raw_key_table_dk[100*2];
extern sUI macos_local_to_raw_key_table_ee[100*2];
extern sUI macos_local_to_raw_key_table_eg[100*2];
extern sUI macos_local_to_raw_key_table_es[100*2];
extern sUI macos_local_to_raw_key_table_fi[100*2];
extern sUI macos_local_to_raw_key_table_fr[100*2];
extern sUI macos_local_to_raw_key_table_gr[100*2];
extern sUI macos_local_to_raw_key_table_hr[100*2];
extern sUI macos_local_to_raw_key_table_hu[100*2];
extern sUI macos_local_to_raw_key_table_ie[100*2];
extern sUI macos_local_to_raw_key_table_il[100*2];
extern sUI macos_local_to_raw_key_table_in[100*2];
extern sUI macos_local_to_raw_key_table_is[100*2];
extern sUI macos_local_to_raw_key_table_it[100*2];
extern sUI macos_local_to_raw_key_table_jp[100*2];
extern sUI macos_local_to_raw_key_table_kr[100*2];
extern sUI macos_local_to_raw_key_table_krd[100*2];
extern sUI macos_local_to_raw_key_table_lk[100*2];
extern sUI macos_local_to_raw_key_table_lt[100*2];
extern sUI macos_local_to_raw_key_table_lv[100*2];
extern sUI macos_local_to_raw_key_table_mk[100*2];
extern sUI macos_local_to_raw_key_table_mx[100*2];
extern sUI macos_local_to_raw_key_table_nl[100*2];
extern sUI macos_local_to_raw_key_table_no[100*2];
extern sUI macos_local_to_raw_key_table_pk[100*2];
extern sUI macos_local_to_raw_key_table_pl[100*2];
extern sUI macos_local_to_raw_key_table_pt[100*2];
extern sUI macos_local_to_raw_key_table_ro[100*2];
extern sUI macos_local_to_raw_key_table_rs[100*2];
extern sUI macos_local_to_raw_key_table_ru[100*2];
extern sUI macos_local_to_raw_key_table_se[100*2];
extern sUI macos_local_to_raw_key_table_sk[100*2];
extern sUI macos_local_to_raw_key_table_si[100*2];
extern sUI macos_local_to_raw_key_table_th[100*2];
extern sUI macos_local_to_raw_key_table_tm[100*2];
extern sUI macos_local_to_raw_key_table_tr[100*2];
extern sUI macos_local_to_raw_key_table_ua[100*2];
extern sUI macos_local_to_raw_key_table_uk[100*2];
extern sUI macos_local_to_raw_key_table_us[100*2];
extern sUI macos_local_to_raw_key_table_hawaii[100*2];
extern sUI macos_local_to_raw_key_table_vn[100*2];
extern sUI macos_local_to_raw_key_table_wales[100*2];
#endif // MACOS_RAW_KEY_TABLES

#if 0
// from SDL/include/SDL_keysym.h:
typedef enum {
	KMOD_NONE  = 0x0000,
	KMOD_LSHIFT= 0x0001,
	KMOD_RSHIFT= 0x0002,
	KMOD_LCTRL = 0x0040,
	KMOD_RCTRL = 0x0080,
	KMOD_LALT  = 0x0100,
	KMOD_RALT  = 0x0200,
	KMOD_LMETA = 0x0400,
	KMOD_RMETA = 0x0800,
	KMOD_NUM   = 0x1000,
	KMOD_CAPS  = 0x2000,
	KMOD_MODE  = 0x4000,
	KMOD_RESERVED = 0x8000
} SDLMod;
#endif


_Key::_Key(void) {
   vkey = vmod = unicode = 0;
}

_Key::_Key(sUI _vkey, sUI _vmod, sUI _unicode) {
   vkey = _vkey;
   vmod = _vmod;
}

void YAC_VCALL _Key::yacSerialize(YAC_Object *_ofs, sUI _rtti) {
   YAC_BEG_SERIALIZE();
   
   YAC_SERIALIZE_I32(vkey);
   YAC_SERIALIZE_I32(vmod);
}

sUI YAC_VCALL _Key::yacDeserialize(YAC_Object *_ifs, sUI _rtti) {
   YAC_BEG_DESERIALIZE();
   
   vkey=YAC_DESERIALIZE_I32();
   vmod=YAC_DESERIALIZE_I32();
   
   return 1;
}

void YAC_VCALL _Key::yacOperator(sSI _cmd, YAC_Object *_o, YAC_Value *_r) {
   switch(_cmd)
   {
   default:
      YAC_Object::yacOperator(_cmd, _o, _r);
      break;

   case YAC_OP_ASSIGN:
      if(YAC_CHK(_o, class_ID))
      {
         _Key *o = (_Key*)_o;
         vkey    = o->vkey;
         vmod    = o->vmod;
         unicode = o->unicode;
      }
      break;

   case YAC_OP_CEQ:
      if(YAC_CHK(_o, class_ID))
      {
         _Key *o = (_Key*)_o;
         YAC_RETI( (o->vkey==vkey) && (o->vmod==vmod) );
      }
      break;

   case YAC_OP_CNE:
      if(YAC_CHK(_o, class_ID))
      {
         _Key *o = (_Key*)_o;
         YAC_RETI( (o->vkey!=vkey) || (o->vmod!=vmod) );
      }
      break;
   }
}

sUI _Key::_getCode(void) {
   return (vkey & 0xFFFF);
}

void _Key::_setCode(sUI _c) {
   vkey = (vkey & 0xFFFF0000) | ((sUI)_c);
}

sUI _Key::_getMod(void) {
   return vmod;
}

void _Key::_setMod(sUI _c) {
   vmod = (sUI)_c;
}

sUI _Key::_getPressed(void) {
   return ((vkey & VKEY_PRESSED) ? (vkey & 0xFFFF) : 0);
}

void _Key::_setPressed(sUI _c) {
   vkey = VKEY_PRESSED | (((sUI)_c) & 0xFFFF);
}

sUI _Key::_getReleased(void) {
   return ( (!(vkey & VKEY_PRESSED)) ? (vkey & 0xFFFF) : 0);
}

void _Key::_setReleased(sUI _c) {
   vkey = (sUI) (_c & 0xFFFF);
}

sUI _Key::_getAlpha(void) {
   sUI t = vkey&0xFFFF;
   if(t >= ' ' && t <= 'z')
   {
      return t;
   }
   else
   {
      return 0;
   }
}

void _Key::_setAlpha(sUI _c) {
   if(_c >= ' ' && _c <= 'z')
   {
      vkey = _c;
   }
   else
   {
      vkey = 0;
   }
}

sUI _Key::_getUnicode(void) {
   return unicode;
}

void _Key::_setUnicode(sSI _c) {
   unicode = (sUI)_c;
}


typedef struct _tks_keyname_entry {
   sU32 code;
   const char *name;
} tks_keyname_entry ;


static tks_keyname_entry tks_keynametable[] = {
   { VKEY_BACKSPACE,   "backspace" },
   { VKEY_TAB,         "tab" },
   { VKEY_CLEAR,       "clear" },
   { VKEY_RETURN,      "return" },
   { VKEY_PAUSE,       "pause" },
   { VKEY_ESCAPE,      "escape" },
   { VKEY_SPACE,       "space" },
   { VKEY_DELETE,      "delete" },
   { VKEY_KP_0,        "kp_0" },
   { VKEY_KP_1,        "kp_1" },
   { VKEY_KP_2,        "kp_2" },
   { VKEY_KP_3,        "kp_3" },
   { VKEY_KP_4,        "kp_4" },
   { VKEY_KP_5,        "kp_5" },
   { VKEY_KP_6,        "kp_6" },
   { VKEY_KP_7,        "kp_7" },
   { VKEY_KP_8,        "kp_8" },
   { VKEY_KP_9,        "kp_9" },
   { VKEY_KP_PERIOD,   "kp_period" },
   { VKEY_KP_DIVIDE,   "kp_divide" },
   { VKEY_KP_MULTIPLY, "kp_multiply" },
   { VKEY_KP_MINUS,    "kp_minus" },
   { VKEY_KP_PLUS,     "kp_plus" },
   { VKEY_KP_ENTER,    "kp_enter" },
   { VKEY_KP_EQUALS,   "kp_equals" },
   { VKEY_UP,          "up" },
   { VKEY_DOWN,        "down" },
   { VKEY_RIGHT,       "right" },
   { VKEY_LEFT,        "left" },
   { VKEY_INSERT,      "insert" },
   { VKEY_HOME,        "home" },
   { VKEY_END,         "end" },
   { VKEY_PAGEUP,      "pageup" },
   { VKEY_PAGEDOWN,    "pagedown" },
   { VKEY_F1,          "f1" },
   { VKEY_F2,          "f2" },
   { VKEY_F3,          "f3" },
   { VKEY_F4,          "f4" },
   { VKEY_F5,          "f5" },
   { VKEY_F6,          "f6" },
   { VKEY_F7,          "f7" },
   { VKEY_F8,          "f8" },
   { VKEY_F9,          "f9" },
   { VKEY_F10,         "f10" },
   { VKEY_F11,         "f11" },
   { VKEY_F12,         "f12" },
   { VKEY_F13,         "f13" },
   { VKEY_F14,         "f14" },
   { VKEY_F15,         "f15" },
   { VKEY_NUMLOCK,     "numlock" },
   { VKEY_CAPSLOCK,    "capslock" },
   { VKEY_SCROLLOCK,   "scrolllock" },  // "SCROLLLOCK" in SDL2
   { VKEY_RSHIFT,      "rshift" },
   { VKEY_LSHIFT,      "lshift" },
   { VKEY_RCTRL,       "rctrl" },
   { VKEY_LCTRL,       "lctrl" },
   { VKEY_RALT,        "ralt" },
   { VKEY_LALT,        "lalt" },
#if 1
   { VKEY_RMETA,       "rmeta" },
   { VKEY_LMETA,       "lmeta" },
#else
   { VKEY_RGUI,        "rmeta" },
   { VKEY_LGUI,        "lmeta" },
#endif
#if 1
   { VKEY_LSUPER,      "lsuper" },
   { VKEY_RSUPER,      "rsuper" },
#endif
   { VKEY_MODE,        "mode" },
   { VKEY_COMPOSE,     "compose" },
   { VKEY_HELP,        "help" },
   { VKEY_PRINT,       "print" },
   { VKEY_SYSREQ,      "sysreq" },
   { VKEY_BREAK,       "break" },
   { VKEY_MENU,        "menu" },
   { VKEY_POWER,       "power" },
   { VKEY_EURO,        "euro" },
   { 0, 0 }
};

static tks_keyname_entry tks_keymodnametable[] = {
   { VMOD_LCTRL,  "lctrl-" },
   { VMOD_LSHIFT, "lshift-" },
   { VMOD_RSHIFT, "rshift-" },
   { VMOD_RCTRL,  "rctrl-" },
   { VMOD_LALT,   "lalt-" },
   { VMOD_RALT,   "ralt-" },
#if 1
   { VMOD_LMETA,  "lmeta-" },
   { VMOD_RMETA,  "rmeta-" },
#else
   { VMOD_LGUI,  "lmeta-" },
   { VMOD_RGUI,  "rmeta-" },
#endif
   { VMOD_NUM,    "num-" },
   { VMOD_CAPS,   "caps-" },
   { VMOD_MODE,   "mode-" },
   { 0, 0 }
};



const char *_Key::getCodeName(sUI _key) {
   tks_keyname_entry *e = tks_keynametable;
   _key = _key & 0xFFFF;

   if(_key >= ' ' && _key < 127)
   {
      static char cs[2];
      cs[0] = (char)_key;
      cs[1] = 0;
      return cs;
   }
   else
   {
      while(e->code)
      {
         if(e->code == _key)
         {
            return e->name;
         }
         e++;
      }
   }
   return 0;
}

sUI _Key::getCodeByName(YAC_String *_s) {
   tks_keyname_entry *e = tks_keynametable;
   if(2 == _s->length)
   {
      return (sUI)_s->chars[0];
   }
   else
   {
      while(e->code)
      {
         if(_s->compare((char*)e->name))
         {
            return e->code;
         }
         e++;
      }
   }
   return 0;
}

sUI _Key::getModByName(YAC_String *_s) {
   tks_keyname_entry *e=tks_keymodnametable;
   while(e->code)
   {
      if(_s->compare((char*)e->name))
      {
         return e->code;
      }
      e++;
   }
   return 0;
}

void _Key::appendModName(YAC_String *_s, sUI _mod) {
   tks_keyname_entry *e = tks_keymodnametable;
   while(e->code)
   {
      if((e->code & _mod))
      {
         YAC_String s; 
         s.visit((char*)e->name);
         _s->yacOperatorAdd(&s);
      }
      e++;
   }
}

void _Key::setModifiersByString(YAC_String *_t) {
   vmod = 0;
   if( (_t->length>1) && (_t->chars!=NULL) )
   {
      sUI i;
      sUI j = 0;
      sChar *s = (sChar*)_t->chars;

      YAC_String s_minus; 
      s_minus.visit("-");

      for(i=0; i<_t->length; i++)
      {
         if(*s == '-')
         {
            *s = 0;
            YAC_String kt;
            kt.copy(s+j);
            kt.append(&s_minus);
            vmod |= getModByName(&kt);
            *s = '-';
            j = i + 1;
         }
      }
   }
}

void _Key::_getCodeName(YAC_Value *_r) {
   YAC_String *s = YAC_New_String();
   YAC_String t; 
   t.visit((char*)getCodeName(vkey));
   s->yacCopy(&t);
   YAC_RETS(s, 1);
}

void _Key::_setCodeName(YAC_String *_s) {
   vkey = getCodeByName(_s);
}

void _Key::_getModName(YAC_Value *_r) {
   YAC_String *s = YAC_New_String();
   appendModName(s, vmod);

   YAC_RETS(s, 1);
}

void _Key::_setModName(YAC_String *_s) {
   setModifiersByString(_s);
}

void _Key::_getName(YAC_Value *_r) {
   YAC_String *s = YAC_New_String();
   appendModName(s, vmod);

   YAC_String t; 
   t.visit((char*)getCodeName(vkey));

   s->yacOperatorAdd(&t);

   YAC_RETS(s, 1);
}

void _Key::_setName(YAC_Object *_s) {
   if(YAC_BCHK(_s, YAC_CLID_STRING))
   {
      YAC_String *s = (YAC_String*)_s;
      sSI li = s->lastIndexOf('-', 0);
      if(li != -1)
      {
         // ---- get keycode ----
         YAC_String kt;
         s->substring(&kt, li+1, s->length-li-1);
         vkey = getCodeByName(&kt);

         // ---- get modifiers ----
         YAC_String t;
         s->substring(&t, 0, li);
         setModifiersByString(&t);
      }
      else
      {
         /// ---- only set codename ----
         vmod = 0;
         vkey = getCodeByName(s);
      }
   }
}


void _Key::_getNameFor(sUI _vkey, sUI _vmod, YAC_Value *_r) {
   YAC_String *s = YAC_New_String();
   appendModName(s, (sUI)_vmod);
   YAC_String t;
   t.visit((char*)getCodeName((sUI)_vkey));
   s->yacOperatorAdd(&t);
   YAC_RETS(s, 1);
}

sBool _Key::_modNone(void) {
   return (0u == vmod);
}

sBool _Key::_modAlt(void) {
   return (vmod&VMOD_LALT) || (vmod&VMOD_RALT);
}

sBool _Key::_modAltOnly(void) {
   return (vmod == VMOD_LALT) || (vmod == VMOD_RALT);
}

sBool _Key::_modCmd(void) {
   // == Alt key on PC keyboards
   return (vmod&VMOD_LALT) || (vmod&VMOD_RALT);
}

sBool _Key::_modCmdOnly(void) {
   // == Alt key on PC keyboards
   return (vmod == VMOD_LALT) || (vmod == VMOD_RALT);
}

sBool _Key::_modShift(void) {
   return (vmod&VMOD_LSHIFT) || (vmod&VMOD_RSHIFT);
}

sBool _Key::_modShiftOnly(void) {
   return (vmod == VMOD_LSHIFT) || (vmod == VMOD_RSHIFT);
}

sBool _Key::_modCtrl(void) {
   return (vmod&VMOD_LCTRL) || (vmod&VMOD_RCTRL);
}

sBool _Key::_modCtrlOnly(void) {
   return (vmod == VMOD_LCTRL) || (vmod == VMOD_RCTRL);
}

sBool _Key::_modCaps(void) {
   return (VMOD_CAPS == (vmod&VMOD_CAPS));
}

sBool _Key::_modShiftOrCaps(void) {
   return (vmod&VMOD_LSHIFT) || (vmod&VMOD_RSHIFT) || (vmod&VMOD_CAPS);
}

sBool _Key::SetLocalToRawKeyTableByName(YAC_String *_name) {
#ifdef MACOS_RAW_KEY_TABLES
   if(YAC_Is_String(_name))
   {
      if(_name->compare("af"))
         return tks_HAL->setLocalToRawKeyTable(macos_local_to_raw_key_table_af);
      if(_name->compare("al"))
         return tks_HAL->setLocalToRawKeyTable(macos_local_to_raw_key_table_al);
      if(_name->compare("at"))
         return tks_HAL->setLocalToRawKeyTable(macos_local_to_raw_key_table_at);
      if(_name->compare("au"))
         return tks_HAL->setLocalToRawKeyTable(macos_local_to_raw_key_table_au);
      if(_name->compare("az"))
         return tks_HAL->setLocalToRawKeyTable(macos_local_to_raw_key_table_az);
      if(_name->compare("be"))
         return tks_HAL->setLocalToRawKeyTable(macos_local_to_raw_key_table_be);
      if(_name->compare("bg"))
         return tks_HAL->setLocalToRawKeyTable(macos_local_to_raw_key_table_bg);
      if(_name->compare("br"))
         return tks_HAL->setLocalToRawKeyTable(macos_local_to_raw_key_table_br);
      if(_name->compare("by"))
         return tks_HAL->setLocalToRawKeyTable(macos_local_to_raw_key_table_by);
      if(_name->compare("ca"))
         return tks_HAL->setLocalToRawKeyTable(macos_local_to_raw_key_table_ca);
      if(_name->compare("ch"))
         return tks_HAL->setLocalToRawKeyTable(macos_local_to_raw_key_table_ch);
      if(_name->compare("cn"))
         return tks_HAL->setLocalToRawKeyTable(macos_local_to_raw_key_table_cn);
      if(_name->compare("cz"))
         return tks_HAL->setLocalToRawKeyTable(macos_local_to_raw_key_table_cz);
      if(_name->compare("de"))
         return tks_HAL->setLocalToRawKeyTable(macos_local_to_raw_key_table_de);
      if(_name->compare("dk"))
         return tks_HAL->setLocalToRawKeyTable(macos_local_to_raw_key_table_dk);
      if(_name->compare("eg"))
         return tks_HAL->setLocalToRawKeyTable(macos_local_to_raw_key_table_eg);
      if(_name->compare("ee"))
         return tks_HAL->setLocalToRawKeyTable(macos_local_to_raw_key_table_ee);
      if(_name->compare("es"))
         return tks_HAL->setLocalToRawKeyTable(macos_local_to_raw_key_table_es);
      if(_name->compare("fi"))
         return tks_HAL->setLocalToRawKeyTable(macos_local_to_raw_key_table_fi);
      if(_name->compare("fr"))
         return tks_HAL->setLocalToRawKeyTable(macos_local_to_raw_key_table_fr);
      if(_name->compare("gr"))
         return tks_HAL->setLocalToRawKeyTable(macos_local_to_raw_key_table_gr);
      if(_name->compare("hr"))
         return tks_HAL->setLocalToRawKeyTable(macos_local_to_raw_key_table_hr);
      if(_name->compare("hu"))
         return tks_HAL->setLocalToRawKeyTable(macos_local_to_raw_key_table_hu);
      if(_name->compare("ie"))
         return tks_HAL->setLocalToRawKeyTable(macos_local_to_raw_key_table_ie);
      if(_name->compare("il"))
         return tks_HAL->setLocalToRawKeyTable(macos_local_to_raw_key_table_il);
      if(_name->compare("in"))
         return tks_HAL->setLocalToRawKeyTable(macos_local_to_raw_key_table_in);
      if(_name->compare("is"))
         return tks_HAL->setLocalToRawKeyTable(macos_local_to_raw_key_table_is);
      if(_name->compare("it"))
         return tks_HAL->setLocalToRawKeyTable(macos_local_to_raw_key_table_it);
      if(_name->compare("jp"))
         return tks_HAL->setLocalToRawKeyTable(macos_local_to_raw_key_table_jp);
      if(_name->compare("kr"))
         return tks_HAL->setLocalToRawKeyTable(macos_local_to_raw_key_table_kr);
      if(_name->compare("krd"))
         return tks_HAL->setLocalToRawKeyTable(macos_local_to_raw_key_table_krd);
      if(_name->compare("lk"))
         return tks_HAL->setLocalToRawKeyTable(macos_local_to_raw_key_table_lk);
      if(_name->compare("lt"))
         return tks_HAL->setLocalToRawKeyTable(macos_local_to_raw_key_table_lt);
      if(_name->compare("lv"))
         return tks_HAL->setLocalToRawKeyTable(macos_local_to_raw_key_table_lv);
      if(_name->compare("mk"))
         return tks_HAL->setLocalToRawKeyTable(macos_local_to_raw_key_table_mk);
      if(_name->compare("mx"))
         return tks_HAL->setLocalToRawKeyTable(macos_local_to_raw_key_table_mx);
      if(_name->compare("nl"))
         return tks_HAL->setLocalToRawKeyTable(macos_local_to_raw_key_table_nl);
      if(_name->compare("no"))
         return tks_HAL->setLocalToRawKeyTable(macos_local_to_raw_key_table_no);
      if(_name->compare("pk"))
         return tks_HAL->setLocalToRawKeyTable(macos_local_to_raw_key_table_pk);
      if(_name->compare("pl"))
         return tks_HAL->setLocalToRawKeyTable(macos_local_to_raw_key_table_pl);
      if(_name->compare("pt"))
         return tks_HAL->setLocalToRawKeyTable(macos_local_to_raw_key_table_pt);
      if(_name->compare("ro"))
         return tks_HAL->setLocalToRawKeyTable(macos_local_to_raw_key_table_ro);
      if(_name->compare("rs"))
         return tks_HAL->setLocalToRawKeyTable(macos_local_to_raw_key_table_rs);
      if(_name->compare("ru"))
         return tks_HAL->setLocalToRawKeyTable(macos_local_to_raw_key_table_ru);
      if(_name->compare("se"))
         return tks_HAL->setLocalToRawKeyTable(macos_local_to_raw_key_table_se);
      if(_name->compare("sk"))
         return tks_HAL->setLocalToRawKeyTable(macos_local_to_raw_key_table_sk);
      if(_name->compare("si"))
         return tks_HAL->setLocalToRawKeyTable(macos_local_to_raw_key_table_si);
      if(_name->compare("th"))
         return tks_HAL->setLocalToRawKeyTable(macos_local_to_raw_key_table_th);
      if(_name->compare("tm"))
         return tks_HAL->setLocalToRawKeyTable(macos_local_to_raw_key_table_tm);
      if(_name->compare("tr"))
         return tks_HAL->setLocalToRawKeyTable(macos_local_to_raw_key_table_tr);
      if(_name->compare("ua"))
         return tks_HAL->setLocalToRawKeyTable(macos_local_to_raw_key_table_ua);
      if(_name->compare("uk"))
         return tks_HAL->setLocalToRawKeyTable(macos_local_to_raw_key_table_uk);
      if(_name->compare("us"))
         return tks_HAL->setLocalToRawKeyTable(macos_local_to_raw_key_table_us);
      if(_name->compare("hawaii"))
         return tks_HAL->setLocalToRawKeyTable(macos_local_to_raw_key_table_hawaii);
      if(_name->compare("vn"))
         return tks_HAL->setLocalToRawKeyTable(macos_local_to_raw_key_table_vn);
      if(_name->compare("wales"))
         return tks_HAL->setLocalToRawKeyTable(macos_local_to_raw_key_table_wales);

      return tks_HAL->setLocalToRawKeyTable(NULL);
   }
#else
   (void)_name;
#endif // MACOS_RAW_KEY_TABLES
   return YAC_FALSE;
}
