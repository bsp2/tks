/// hook.h
///
/// (c) 2013-2025 Bastian Spiegel <bs@tkscript.de>
///     - Distributed under terms of the Lesser GNU General Public License (LGPL).
///       See COPYING and <http://www.gnu.org/licenses/licenses.html#LGPL> for further information.
///
///

#ifndef TKOPENGL_HOOK_H__
#define TKOPENGL_HOOK_H__


extern void tkopengl_hook_install (void);

extern void tkopengl_hook_open_cbk (void);

extern void tkopengl_hook_close_cbk (void);

extern void tkopengl_hook_remove (void);


#endif // TKOPENGL_HOOK_H__
