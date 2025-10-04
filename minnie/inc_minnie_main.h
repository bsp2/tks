#include "inc_yac.h"
#define MINNIE_SKIP_TYPEDEFS defined
#ifdef USE_MINNIE_MIB_SETUP
#define MINNIE_IMPLEMENTATION defined
#endif // USE_MINNIE_MIB_SETUP
#include "minnie.h"
#include "MinnieDrawable.h"
#include "inc_opengl.h"
#include "shadervg/inc_shadervg.h"

#ifdef USE_MINNIE_MIB_SETUP
#if defined(__cplusplus)
extern "C" {
#endif
extern void minnie_impl_init (void);
extern void minnie_impl_exit (void);
#if defined(__cplusplus)
} // extern "C"
#endif
#endif // USE_MINNIE_MIB_SETUP
