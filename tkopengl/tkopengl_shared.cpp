
// to be included (once) by plugins that use tkopengl (in native code)

static YAC_CallbackId tkopengl_shared_callback_id;
tkopengl_shared_t *tkopengl_shared = NULL;


void tkopengl_shared_resolve(void) {

   // Import shared functions from tkopengl

   if(NULL == tkopengl_shared)
   {
      tkopengl_shared_callback_id = yac_host->yacCallbackGetIdByName(TKOPENGL_SHARED_NAME);
      tkopengl_shared = (tkopengl_shared_t*) yac_host->yacCallbackGetFunById(tkopengl_shared_callback_id);

      if(NULL == tkopengl_shared)
      {
         Dyac_host_printf("[---] tkopengl_shared_resolve: failed to resolve cbk %s\n", TKOPENGL_SHARED_NAME);
      }
   }
}
