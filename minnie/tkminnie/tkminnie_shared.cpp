
// to be included (once) by plugins that use tkminnie (in native code)

static YAC_CallbackId tkminnie_shared_callback_id;
tkminnie_shared_t *tkminnie_shared = NULL;


void tkminnie_shared_resolve(void) {

   // Import shared functions from tkminnie

   if(NULL == tkminnie_shared)
   {
      tkminnie_shared_callback_id = yac_host->yacCallbackGetIdByName(TKMINNIE_SHARED_NAME);
      // Dyac_host_printf("xxx tkminnie_shared_resolve: tkminnie_shared_callback_id=%d\n", tkminnie_shared_callback_id);
      tkminnie_shared = (tkminnie_shared_t*) yac_host->yacCallbackGetFunById(tkminnie_shared_callback_id);

      if(NULL == tkminnie_shared)
      {
         Dyac_host_printf("[---] tkminnie_shared_resolve: failed to resolve cbk %s\n", TKMINNIE_SHARED_NAME);
      }
   }
}
