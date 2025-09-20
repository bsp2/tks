/// tkmidi.h
///
/// (c) 2014 Bastian Spiegel <bs@tkscript.de>
///     - Distributed under terms of the Lesser GNU General Public License (LGPL).
///       See COPYING and <http://www.gnu.org/licenses/licenses.html#LGPL> for further information.
///
///
/// changed: 04Feb2014, 25Aug2014
///

#ifndef __TKMIDI_H__
#define __TKMIDI_H__


#include <alsa/asoundlib.h>

#ifndef SND_TIMER_GLOBAL_HRTIMER
#define SND_TIMER_GLOBAL_HRTIMER   3
#endif


class AlsaDeviceInfo;

class AlsaDeviceInfo {
  public:
   AlsaDeviceInfo *next;

   YAC_String device_path; // "hw:0,0,0", .. 
   YAC_String device_name; // "MIDISPORT 4x4 ANNIV 1", ..
};


#endif // __TKMIDI_H__
