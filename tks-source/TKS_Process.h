/// TKS_Process.h
///
/// (c) 2018-2024 Bastian Spiegel <bs@tkscript.de>
///     - distributed under terms of the GNU general public license (GPL).
///
#ifndef __TKS_PROCESS_H__
#define __TKS_PROCESS_H__


#ifdef YAC_WIN32
#include <strsafe.h>
#include <intsafe.h>
#include <wchar.h>
#include <windows.h>
#endif // YAC_WIN32

#if defined(YAC_LINUX) || defined(YAC_MACOS)
#include <unistd.h>
#include <sys/socket.h>

#include <fcntl.h>
#include <signal.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/ioctl.h>
#endif // defined(YAC_LINUX) || defined(YAC_MACOS)


class TKS_Process : public YAC_StreamBase {
public:
#if defined(YAC_LINUX) || defined(YAC_MACOS)
   int fd1[2];
   int fd2[2];
   pid_t pid;
#endif // defined(YAC_LINUX) || defined(YAC_MACOS)

#ifdef YAC_WIN32
   HANDLE child_in_rd;
   HANDLE child_in_wr;
   HANDLE child_out_rd;
   HANDLE child_out_wr;
   PROCESS_INFORMATION piProcInfo;
#endif // YAC_WIN32

public:
   TKS_Process(void);
   ~TKS_Process();

   sUI   YAC_VCALL yacStreamGetSize      (void);
   sU8   YAC_VCALL yacStreamReadI8       (void);
   sSI   YAC_VCALL yacStreamReadString   (YAC_String *, sUI _maxlen);
   sSI   YAC_VCALL yacStreamReadBuffer   (YAC_Buffer *_b, sUI _off, sUI _max, sBool _resize);
   sSI   YAC_VCALL yacStreamWrite        (sU8 *, sUI _num);
   void  YAC_VCALL yacStreamWriteI8      (sU8);
   sBool YAC_VCALL yacIsStream           (void);

  public:
   // Create new process and execute command
   //  - cmd: full command pathname (e.g. "/bin/ls")
   //  - args: StringArray or null
   sBool exec (YAC_String *_cmd, YAC_Object *_args);

   // Query number of available bytes. Wait up to <timeoutMillisec> for new data to arrive.
   //
   sUI waitAvail (sUI _timeoutMillisec);

   // Send SIGTERM, wait 1sec, then send SIGKILL
   void kill (void);

   // Get pid of current process
   sSI GetPID (void);

   // Send signal to other process
   //  - uses ::kill() on Linux / macOS
   //  - n/a on Windows
   //  - 'signal' is one of SIG* (see signal.h).
   //     ! macOS: /Library/Developer/CommandLineTools/SDKs/MacOSX13.3.sdk/usr/include/sys/signal.h
   //     ! Linux: /usr/include/bits/signum.h
   // (note) SIGURG=16(macOS) / 23(Linux)
   // (note) SIGUSR1=30(macOS) / 10(Linux)
   //
   void SendSignal (sSI _pid, sSI _signal);
   void SendSignal_SIGUSR1 (sSI _pid);

   // Wait until this process has received SIGUSR1
   void WaitSignal_SIGUSR1 (void);

};


#endif // __YAC_BUFFER_H__
