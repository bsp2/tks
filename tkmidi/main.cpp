
#include <windows.h>

#ifndef MOD_WAVETABLE // VC6
#define MOD_WAVETABLE   6  /* hardware wavetable synth */
#endif
#ifndef MOD_SWSYNTH
#define MOD_SWSYNTH     7  /* software synth */
#endif



#include <yac.h>
#include <yac_host.cpp>
YAC_Host *yac_host;

//static const char*midiout_device_name = "Microsoft GS Wavetable SW Synth";
//static const char*midiout_device_name = "Roland JUNO-G";
static const char*midiout_device_name = "MIDI Yoke NT:  1";


static LARGE_INTEGER pfcFrequency;

#define __DWORD_PTR DWORD

static int ticks;

static sF64 time_results[10000];
static sF64 time_delta_results[10000];

sF64 GetMilliSecondsDouble(void) {
   LARGE_INTEGER pfcCurrent;
   if( ::QueryPerformanceCounter(&pfcCurrent) )
   {
      sF64 s = ((sF64)pfcCurrent.QuadPart) / ((sF64)pfcFrequency.QuadPart);
     
      //return (1000.0 * (s- ((sSI)s)));
      return 1000.0 * s;
   }
   else
   {
      return 0.0;
   }
}


static HMIDIOUT midiout_handle;


static MIDIHDR midiout_hdr;//[16];
static int current_hdr;

MMRESULT timer_handle;
static sF64 t_last;
static sF64 t_tick_count;

static const sU8 mdat__noteoffon[] = { 
   0x80, 50, 64,
   0x90, 50, 96,
};


static int events;

void __stdcall winmm__timer_cbk(UINT uTimerId, UINT uMsg, __DWORD_PTR dwUser, __DWORD_PTR dw1, __DWORD_PTR dw2) {
   if(uTimerId==timer_handle)
   {
      sF64 t = GetMilliSecondsDouble();
      sF64 dt;
      if(t_last<1)
      {
         dt=0.0;
      }
      else
      {
         dt= (t-t_last);
      }
      if( dt > 0.8f) // workaround first 460 callbacks where deltatime is 0,02ms (?!!)
      {
         time_results[ticks++] = t;
      }
      t_last = t;
      t_tick_count += dt;
      if(t_tick_count >= (20*0.125))
      {
         t_tick_count -= 20*0.125;
         midiout_hdr.lpData = (char*) mdat__noteoffon;
         midiout_hdr.dwBufferLength = midiout_hdr.dwBytesRecorded = sizeof(mdat__noteoffon);
         //midiOutPrepareHeader(midiout_handle, &midiout_hdr, sizeof(MIDIHDR));
         //midiOutLongMsg(midiout_handle, &midiout_hdr, sizeof(MIDIHDR));
         //midiOutUnprepareHeader(midiout_handle, &midiout_hdr, sizeof(MIDIHDR));

         //midiOutShortMsg(midiout_handle, 0x00C0); // ProgChange 0
         //midiOutShortMsg(midiout_handle, 0x403280); // Noteoff
         //midiOutShortMsg(midiout_handle, 0x403290); // noteOn

         midiOutShortMsg(midiout_handle, 0x403399); // noteOn

         events++;
      }

   }

}


int main(int _argc, char **_argv) {

   // Query midi out devices
   {
      int bOpened = 0;
      UINT numDevs = midiOutGetNumDevs();
      MIDIOUTCAPS midiOutCaps;
      for(UINT i=0; i<numDevs; i++)
      {
         if(midiOutGetDevCaps(i, &midiOutCaps, sizeof(MIDIOUTCAPS)) == MMSYSERR_NOERROR)
         {
            printf("[...] midiout[%i]: name=\"%s\" tech=%s chMask=%04x \n", 
               i, midiOutCaps.szPname, 
               (midiOutCaps.wTechnology==MOD_MIDIPORT)  ? "MOD_MIDIPORT" :
               (midiOutCaps.wTechnology==MOD_SYNTH)     ? "MOD_SYNTH" :
               (midiOutCaps.wTechnology==MOD_SQSYNTH)   ? "MOD_SQSYNTH" :
               (midiOutCaps.wTechnology==MOD_FMSYNTH)   ? "MOD_FMSYNTH" :
               (midiOutCaps.wTechnology==MOD_MAPPER)    ? "MOD_MAPPER" :
               (midiOutCaps.wTechnology==MOD_WAVETABLE) ? "MOD_WAVETABLE" :
               (midiOutCaps.wTechnology==MOD_SWSYNTH)   ? "MOD_SWSYNTH" :
               "<unknown>",
               midiOutCaps.wChannelMask
                  );
            if(!strcmp(midiOutCaps.szPname, midiout_device_name))
            {
               printf("*FOUND*\n");
               if(midiOutOpen(&midiout_handle, i, NULL, NULL, CALLBACK_NULL) == MMSYSERR_NOERROR)
               {
                  printf("[...] midi device \"%s\" opened.\n", midiout_device_name);
                  memset(&midiout_hdr, 0, sizeof(MIDIHDR));
                  current_hdr = 0;
                  bOpened=1;
               }
               else
               {
                  printf("[---] failed to opend midi device \"%s\".\n", midiout_device_name);
                  return 10;
               }
            }
         }
      }
      if(!bOpened)
      {
         printf("[---] midi device \"%s\" not present.\n", midiout_device_name);
         return 10;
      }
   }


   ::QueryPerformanceFrequency(&pfcFrequency);

   ticks = 0;
   events = 0;
   t_last = 0.0f;
   t_tick_count = 0.0f;
   timer_handle = timeSetEvent(1, 1, winmm__timer_cbk, NULL, TIME_PERIODIC | TIME_CALLBACK_FUNCTION);

   if(timer_handle == NULL)
   {
      printf("[---] failed to set multimedia timer.\n");
      midiOutClose(midiout_handle);
      return 10;
   }

   // Wait 30 seconds
   Sleep(3*20*64);

   // Stop timer
   timeKillEvent(timer_handle);


   // Close midi device
   midiOutClose(midiout_handle);


   // Print results
   printf("ticks=%i\n", ticks);
   printf("events=%i\n", events);

   sF64 current = 0;
   for(int i=0; i<ticks; i++)
   {
      time_delta_results[i] = time_results[i] - current;
      current = time_results[i];
   }


   return 0;
}
