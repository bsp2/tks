// TKS_PerfCount_win32.cpp 
/// 
/// (c) 2009 by Bastian Spiegel <bs@tkscript.de> 
///     - distributed under terms of the GNU general public license (GPL). 
/// 

#include "tks.h"

#include "TKS_PerfCount.h"

#include <windows.h>

void TKS_PerfCount::GetPerfCount(sU64 *_r) {
   ::QueryPerformanceCounter((LARGE_INTEGER*)_r);
}
