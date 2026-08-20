// TKS_PerfCount_win32.cpp
///
/// (c) 2009-2026 by Bastian Spiegel <bs@tkscript.de>
///     - distributed under terms of the Lesser GNU General Public License (LGPL)
///

#include "tks.h"

#include "TKS_PerfCount.h"

#include <windows.h>

void TKS_PerfCount::GetPerfCount(sU64 *_r) {
   ::QueryPerformanceCounter((LARGE_INTEGER*)_r);
}
