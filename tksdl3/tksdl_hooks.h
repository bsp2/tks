/// tksdl_hooks.h
///
/// (c) 2013-2014 Bastian Spiegel <bs@tkscript.de> 
///     - Distributed under terms of the Lesser GNU General Public License (LGPL).
///       See COPYING and <http://www.gnu.org/licenses/licenses.html#LGPL> for further information.
///
///

#ifndef __TKSDL_HOOKS_H__
#define __TKSDL_HOOKS_H__

struct tksdl_hook_s;
typedef struct tksdl_hook_s tksdl_hook_t;

// (note) first hook entry is list header (fxn==NULL)
struct tksdl_hook_s {
   void (*fxn) (void);
   tksdl_hook_t *next;
};

// window/view has been opened or resized
#define TKSDL_HOOK_NAME_OPEN   "tksdl_hook_open"

// window/view is about to be closed
#define TKSDL_HOOK_NAME_CLOSE  "tksdl_hook_close"


#ifdef TKSDL_BUILD
extern tksdl_hook_t tksdl_hook_open;
extern tksdl_hook_t tksdl_hook_close;

#else

static void tksdl_hook_add(const char *_name, tksdl_hook_t *_hook) {
   YAC_CallbackId hookId = yac_host->yacCallbackGetIdByName(_name);
   tksdl_hook_t *hook = (tksdl_hook_t*) yac_host->yacCallbackGetFunById(hookId);

   if(NULL != hook)
   {
      while(NULL != hook->next)
      {
         hook = hook->next;
      }

      hook->next = _hook;
   }
   else
   {
      yac_host->printf("[---] tksdl_hook_add: current hook is NULL! (name=\"%s\")\n", _name);
   }
}

static void tksdl_hook_remove(const char *_name, tksdl_hook_t *_hook) {
   YAC_CallbackId hookId = yac_host->yacCallbackGetIdByName(_name);
   tksdl_hook_t *hook = (tksdl_hook_t*) yac_host->yacCallbackGetFunById(hookId);
   tksdl_hook_t *hookPrev = NULL;

   ////printf("xxx tksdl_hook_remove: name=\"%s\" hook=%p\n", _name, hook);

   while(NULL != hook)
   {
      if(hook == _hook)
      {
         // (note) never the first/root node
         ////printf("xxx tksdl_hook_remove: remove hook=%p next=%p\n", _hook, hook->next);
         hookPrev->next = hook->next;
         hook->next = NULL;
         break;
      }
      else
      {
         hookPrev = hook;
         hook = hook->next;
      }
   }
}

#endif // TKSDL_BUILD

#endif // __TKSDL_HOOKS_H__
