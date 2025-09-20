/// tks_debug_linux.cpp 
/// (c) 2001-2005..2024 Carsten Busse <car.busse@gmx.de> 
///     - distributed under the terms of the GNU general public license (GPL). 
/// 
/// 
 
#include "tks.h" 
 
#ifdef YAC_LINUX 
 
#include <signal.h> 
#include <stdio.h> 
#include <stdlib.h> 
 
 
#include <sys/types.h> 
#include <unistd.h> 
 
sigset_t mySignalSet; 
static char *executableName; 
#ifdef DX_X86 
#ifdef YAC_64 //64bit
greg_t tmpR8;
greg_t tmpR9;
greg_t tmpR10;
greg_t tmpR11;
greg_t tmpR12;
greg_t tmpR13;
greg_t tmpR14;
greg_t tmpR15;
greg_t tmpRBP; // required for interaction with gdb, do not remove 
greg_t tmpRSP; // required for interaction with gdb, do not remove 
greg_t tmpRIP; // required for interaction with gdb, do not remove 
greg_t tmpRAX; // required for interaction with gdb, do not remove 
greg_t tmpRBX; // required for interaction with gdb, do not remove 
greg_t tmpRCX; // required for interaction with gdb, do not remove 
greg_t tmpRDX; // required for interaction with gdb, do not remove 
greg_t tmpRSI; // required for interaction with gdb, do not remove 
greg_t tmpRDI; // required for interaction with gdb, do not remove 
unsigned short int tmpCS; // required for interaction with gdb, do not remove 
unsigned short int tmpDS; // required for interaction with gdb, do not remove 
unsigned short int tmpES; // required for interaction with gdb, do not remove 
unsigned short int tmpFS; // required for interaction with gdb, do not remove 
unsigned short int tmpGS; // required for interaction with gdb, do not remove 
unsigned short int tmpSS; // required for interaction with gdb, do not remove 

greg_t tmpEFlags; // required for interaction with gdb, do not remove 

#else //32bit
greg_t tmpEBP; // required for interaction with gdb, do not remove 
greg_t tmpESP; // required for interaction with gdb, do not remove 
greg_t tmpEIP; // required for interaction with gdb, do not remove 
greg_t tmpEAX; // required for interaction with gdb, do not remove 
greg_t tmpEBX; // required for interaction with gdb, do not remove 
greg_t tmpECX; // required for interaction with gdb, do not remove 
greg_t tmpEDX; // required for interaction with gdb, do not remove 
greg_t tmpESI; // required for interaction with gdb, do not remove 
greg_t tmpEDI; // required for interaction with gdb, do not remove 
unsigned short int tmpCS; // required for interaction with gdb, do not remove 
unsigned short int tmpDS; // required for interaction with gdb, do not remove 
unsigned short int tmpES; // required for interaction with gdb, do not remove 
unsigned short int tmpFS; // required for interaction with gdb, do not remove 
unsigned short int tmpGS; // required for interaction with gdb, do not remove 
unsigned short int tmpSS; // required for interaction with gdb, do not remove 
greg_t tmpEFlags; // required for interaction with gdb, do not remove 
#endif //YAC:64
#endif //DX_X86
 
 
 
void sa_sigaction_SIGFPE(int sig, siginfo_t *info, void *extra) { 
   yac_host->printf("SIGFPE got called!\n"); 
   tkscript->printAllFunctionCallStacks(); 
 
   if (info->si_signo==SIGFPE) { 
      yac_host->printf("SIGFPE Acknowledged Errno: %d Signal Source: %d\n",info->si_errno,info->si_code); 
      yac_host->printf("Source: "); 
      switch (info->si_code) { 
      case SI_USER:yac_host->printf("User raised Signal!\n"); 
				     break; 
      case SI_KERNEL:yac_host->printf("Kernel raised Signal!\n"); 
         break; 
      case SI_QUEUE:yac_host->printf("Signal Queue sent Signal!\n"); 
         break; 
      case SI_TIMER:yac_host->printf("Timer expired\n"); 
         break; 
      case SI_MESGQ:yac_host->printf("Message Queue state changed\n"); 
         break; 
      case SI_ASYNCIO:yac_host->printf("AIO completed\n"); 
         break; 
      case SI_SIGIO:yac_host->printf("queued SIGIO\n"); 
         break; 
      case FPE_INTDIV:yac_host->printf("Integer Divide by Zero!\n"); 
         break; 
      case FPE_INTOVF:yac_host->printf("Integer Overflow!\n"); 
         break; 
      case FPE_FLTDIV:yac_host->printf("Floating Point Divide by Zero!\n"); 
         break; 
      case FPE_FLTOVF:yac_host->printf("Floating Point Overflow!\n"); 
         break; 
      case FPE_FLTUND:yac_host->printf("Floating Point Underflow!\n"); 
         break; 
      case FPE_FLTRES:yac_host->printf("Floating Point inexact Result!\n"); 
         break; 
      case FPE_FLTINV:yac_host->printf("Floating Point Invalid Operation!\n"); 
         break; 
      case FPE_FLTSUB:yac_host->printf("Floating Point subscript out of range!\n"); 
         break; 
      default:yac_host->printf("Unknown Code: %d\n",info->si_code); 
      } 
      ////yac_host->printf("Program failed at address %010p\n",info->si_addr); 
      yac_host->printf("Program failed at address %p\n",info->si_addr); 
      sigset_t blockSet; 
      sigemptyset(&blockSet); 
      sigaddset(&blockSet,SIGFPE); 
      sigaddset(&blockSet,SIGILL); 
      sigaddset(&blockSet,SIGSEGV); 
      sigprocmask(SIG_BLOCK,&blockSet,NULL); 
       

#ifdef DX_X86 
      ucontext_t *context=(ucontext_t*)extra; 

#ifdef YAC_64 //64bit
      yac_host->printf("RAX: %016x RBX: %016x RCX: %016x RDX: %016x RSI: %016x RDI: %016x\n", 
         context->uc_mcontext.gregs[REG_RAX],context->uc_mcontext.gregs[REG_RBX],context->uc_mcontext.gregs[REG_RCX],context->uc_mcontext.gregs[REG_RDX], 
         context->uc_mcontext.gregs[REG_RSI],context->uc_mcontext.gregs[REG_RDI]); 
      yac_host->printf("RSP: %016x RBP: %016x RIP: %016x\n", 
         context->uc_mcontext.gregs[REG_RSP],context->uc_mcontext.gregs[REG_RBP],context->uc_mcontext.gregs[REG_RIP]); 
      yac_host->printf("R08: %016x R09: %016x R10: %016x R11: %016x R12: %016x R13: %016x R14: %016x R15: %016x\n", 
         context->uc_mcontext.gregs[REG_R8],context->uc_mcontext.gregs[REG_R9],context->uc_mcontext.gregs[REG_R10],context->uc_mcontext.gregs[REG_R11], 
         context->uc_mcontext.gregs[REG_R12],context->uc_mcontext.gregs[REG_R13],context->uc_mcontext.gregs[REG_R14],context->uc_mcontext.gregs[REG_R15]); 
      yac_host->printf("CSGSFS:  %016x\n", 
         context->uc_mcontext.gregs[REG_CSGSFS]); 
      yac_host->printf("TrapNo: %016x Error: %016x EFlags %016x CR2: %016x OLDMASK: %016x\n", 
         context->uc_mcontext.gregs[REG_TRAPNO],context->uc_mcontext.gregs[REG_ERR],context->uc_mcontext.gregs[REG_EFL],
	 context->uc_mcontext.gregs[REG_CR2],context->uc_mcontext.gregs[REG_OLDMASK]); 

	if (context->uc_mcontext.fpregs) {
		int i;
		yac_host->printf("FPU cwd: %04x swd: %04x ftw: %04x fop: %04x\n",
			context->uc_mcontext.fpregs->cwd, context->uc_mcontext.fpregs->swd,
			context->uc_mcontext.fpregs->ftw, context->uc_mcontext.fpregs->fop);
		yac_host->printf("    rip: %016x rdp: %016x mxcsr: %08x mxcr_mask: %08x\n",
			context->uc_mcontext.fpregs->rip, context->uc_mcontext.fpregs->rdp,
			context->uc_mcontext.fpregs->mxcsr, context->uc_mcontext.fpregs->mxcr_mask);
		yac_host->printf("FPU Stack:\n");
		for (i=0;i<8;i++) 
			yac_host->printf("st(%d)=signficand %04x%04x%04x%04x exp %04x\n",i,
				context->uc_mcontext.fpregs->_st[i].significand[0],
				context->uc_mcontext.fpregs->_st[i].significand[1],
				context->uc_mcontext.fpregs->_st[i].significand[2],
				context->uc_mcontext.fpregs->_st[i].significand[3],
				context->uc_mcontext.fpregs->_st[i].exponent);
		yac_host->printf("XMM Registers:\n"); // X86_64 has both fpu and xmm separated of each other
		for (i=0;i<16;i++)
			yac_host->printf("xmm[%d]=%08x, %08x, %08x, %08x\n",i,
				context->uc_mcontext.fpregs->_xmm[i].element[0],
				context->uc_mcontext.fpregs->_xmm[i].element[1],
				context->uc_mcontext.fpregs->_xmm[i].element[2],
				context->uc_mcontext.fpregs->_xmm[i].element[3]);
	}
      if (info->si_addr!=(void*)context->uc_mcontext.gregs[REG_RIP]) info->si_addr=(void*)context->uc_mcontext.gregs[REG_RIP]; 


      if(NULL == ::getenv("TKS_NO_GDB"))
      {
         
         char buffer[16384]; 
       
         pid_t currentPID; 
         currentPID=getpid(); 
         sigprocmask(SIG_UNBLOCK,&blockSet,NULL); 
         int buflen=sprintf(buffer,"(echo set prompt;echo set pagination off;echo list *%p;echo disassemble %p %p;echo set tmpRAX=\\$rax;echo set tmpRBX=\\$rbx;echo set tmpRCX=\\$rcx;echo set tmpRDX=\\$rdx;echo set tmpRSI=\\$rsi;echo set tmpRDI=\\$rdi; echo set tmpR8=\\$r8; echo set tmpR9=\\$r9; echo set tmpR10=\\$r10; echo set tmpR11=\\$r11; echo set tmpR12=\\$r12; echo set tmpR13=\\$r13; echo set tmpR14=\\$r14; echo set tmpR15=\\$r15; echo set tmpCS=\\$cs;echo set tmpDS=\\$ds;echo set tmpES=\\$es;echo set tmpFS=\\$fs;echo set tmpGS=\\$gs;echo set tmpSS=\\$ss;echo set tmpRIP=\\$rip;echo set tmpRBP=\\$rbp;echo set tmpRSP=\\$rsp;echo set tmpEFlags=\\$eflags;echo set \\$rax=0x%016lx;echo set \\$rbx=0x%016lx;echo set \\$rcx=0x%016lx;echo set \\$rdx=0x%016lx;echo set \\$rsi=0x%016lx;echo set \\$rdi=0x%016lx;echo set \\$r8=0x%016lx; echo set \\$r9=0x%016lx; echo set \\$r10=0x%016lx; echo set \\$r11=0x%016lx; echo set \\$r12=0x%016lx; echo set \\$r13=0x%016lx; echo set \\$r14=0x%016lx; echo set \\$r15=0x%016lx; echo set \\$cs=0x%04x;echo set \\$fs=0x%04x;echo set \\$gs=0x%04x; echo set \\$rbp=0x%016lx;echo set \\$rsp=0x%016lx;echo set \\$rip=0x%016lx;echo set \\$eflags=0x%016lx;echo bt full;echo frame 0;echo info frame 0;echo echo Please exit the debugger through pressing \\<CTRL\\>-d\\!\\\\n;echo echo Good luck in finding your Bug \\:D \\!\\\\n;echo echo \\\\n;echo set prompt \\(tks-gdb\\);cat;echo set prompt;echo echo \\\\nQuitting Session ...\\\\n;echo set \\$rax=tmpRAX;echo set \\$rbx=tmpRBX;echo set \\$rcx=tmpRCX;echo set \\$rdx=tmpRDX;echo set \\$rsi=tmpRSI;echo set \\$rdi=tmpRDI;echo set \\$r8=tmpR8; echo set \\$r9=tmpR9; echo set \\$r10=tmpR10; echo set \\$r11=tmpR11; echo set \\$r12=tmpR12; echo set \\$r13=tmpR13; echo set \\$r14=tmpR14; echo set \\$r15=tmpR15; echo set \\$cs=tmpCS;echo set \\$ds=tmpDS;echo set \\$es=tmpES;echo set \\$fs=tmpFS;echo set \\$gs=tmpGS;echo set \\$ss=tmpSS;echo set \\$rsp=tmpRSP;echo set \\$rbp=tmpRBP;echo set \\$rip=tmpRIP;echo set \\$eflags=tmpEFlags;echo;echo detach;echo quit;sleep 2)|gdb --quiet %s %d;echo", 
                            info->si_addr,(char*)info->si_addr-16,(char*)info->si_addr+16, 
                            context->uc_mcontext.gregs[REG_RAX],context->uc_mcontext.gregs[REG_RBX], 
                            context->uc_mcontext.gregs[REG_RCX],context->uc_mcontext.gregs[REG_RDX], 
                            context->uc_mcontext.gregs[REG_RSI],context->uc_mcontext.gregs[REG_RDI], 
                            context->uc_mcontext.gregs[REG_R8],context->uc_mcontext.gregs[REG_R9],
                            context->uc_mcontext.gregs[REG_R10],context->uc_mcontext.gregs[REG_R11],
                            context->uc_mcontext.gregs[REG_R12],context->uc_mcontext.gregs[REG_R13],
                            context->uc_mcontext.gregs[REG_R14],context->uc_mcontext.gregs[REG_R15],
                            (unsigned int)((context->uc_mcontext.gregs[REG_CSGSFS])&0xffff), //context->uc_mcontext.gregs[REG_DS], 
                            //context->uc_mcontext.gregs[REG_ES],
                            (unsigned int)(((context->uc_mcontext.gregs[REG_CSGSFS])>>32)&0xffff), 
                            (unsigned int)(((context->uc_mcontext.gregs[REG_CSGSFS])>>16)&0xffff),//context->uc_mcontext.gregs[REG_SS], 
                            context->uc_mcontext.gregs[REG_RBP],context->uc_mcontext.gregs[REG_RSP], 
                            context->uc_mcontext.gregs[REG_RIP],context->uc_mcontext.gregs[REG_EFL], 
                            executableName,currentPID); 

         printf("Executing %d Byte String '%s'...\n",buflen,buffer); 
         
         int rsys = system(buffer);
         (void)rsys;
      }


#else //32bit

      yac_host->printf("EAX: %08x EBX: %08x ECX: %08x EDX: %08x ESI: %08x EDI: %08x\n", 
         context->uc_mcontext.gregs[REG_EAX],context->uc_mcontext.gregs[REG_EBX],context->uc_mcontext.gregs[REG_ECX],context->uc_mcontext.gregs[REG_EDX], 
         context->uc_mcontext.gregs[REG_ESI],context->uc_mcontext.gregs[REG_EDI]); 
      yac_host->printf("ESP: %08x EBP: %08x EIP: %08x\n", 
         context->uc_mcontext.gregs[REG_ESP],context->uc_mcontext.gregs[REG_EBP],context->uc_mcontext.gregs[REG_EIP]); 
      yac_host->printf("CS:  %08x DS:  %08x ES:  %08x FS:  %08x GS:  %08x SS:  %08x\n", 
         context->uc_mcontext.gregs[REG_CS],context->uc_mcontext.gregs[REG_DS],context->uc_mcontext.gregs[REG_ES],context->uc_mcontext.gregs[REG_FS], 
         context->uc_mcontext.gregs[REG_GS],context->uc_mcontext.gregs[REG_SS]); 
      yac_host->printf("TrapNo: %08x Error: %08x EFlags %08x\n", 
         context->uc_mcontext.gregs[REG_TRAPNO],context->uc_mcontext.gregs[REG_ERR],context->uc_mcontext.gregs[REG_EFL]); 

      if (context->uc_mcontext.fpregs) { 
         yac_host->printf("FPU cw: %08x sw: %08x tag: %08x ipoff: %08x cssel: %08x dataoff: %08x datasel: %08x status %08x\n", 
            context->uc_mcontext.fpregs->cw,context->uc_mcontext.fpregs->sw,context->uc_mcontext.fpregs->tag, 
            context->uc_mcontext.fpregs->ipoff,context->uc_mcontext.fpregs->cssel,context->uc_mcontext.fpregs->dataoff, 
            context->uc_mcontext.fpregs->datasel,context->uc_mcontext.fpregs->status); 
         for (int iter=0;iter<8;iter++) { 
            yac_host->printf("st[%d]=",iter); 
            for (int jter=0;jter<4;jter++) yac_host->printf("%04x",context->uc_mcontext.fpregs->_st[iter].significand[jter]); 
            yac_host->printf(" exp %04x\n",context->uc_mcontext.fpregs->_st[iter].exponent); 
         } 
      } 
      if (info->si_addr!=(void*)context->uc_mcontext.gregs[REG_EIP]) info->si_addr=(void*)context->uc_mcontext.gregs[REG_EIP]; 


      if(NULL == ::getenv("TKS_NO_GDB"))
      {
       
         char buffer[4096]; 
       
         pid_t currentPID; 
         currentPID=getpid(); 
         sigprocmask(SIG_UNBLOCK,&blockSet,NULL); 
         int buflen=sprintf(buffer,"(echo set prompt;echo set pagination off;echo list *%p;echo disassemble %p %p;echo set tmpEAX=\\$eax;echo set tmpEBX=\\$ebx;echo set tmpECX=\\$ecx;echo set tmpEDX=\\$edx;echo set tmpESI=\\$esi;echo set tmpEDI=\\$edi;echo set tmpCS=\\$cs;echo set tmpDS=\\$ds;echo set tmpES=\\$es;echo set tmpFS=\\$fs;echo set tmpGS=\\$gs;echo set tmpSS=\\$ss;echo set tmpEIP=\\$eip;echo set tmpEBP=\\$ebp;echo set tmpESP=\\$esp;echo set tmpEFlags=\\$eflags;echo set \\$eax=0x%08x;echo set \\$ebx=0x%08x;echo set \\$ecx=0x%08x;echo set \\$edx=0x%08x;echo set \\$esi=0x%08x;echo set \\$edi=0x%08x;echo set \\$cs=0x%08x;echo set \\$ds=0x%08x;echo set \\$es=0x%08x;echo set \\$fs=0x%08x;echo set \\$gs=0x%08x;echo set \\$ss=0x%08x; echo set \\$ebp=0x%08x;echo set \\$esp=0x%08x;echo set \\$eip=0x%08x;echo set \\$eflags=0x%08x;echo bt full;echo frame 0;echo info frame 0;echo echo Please exit the debugger through pressing \\<CTRL\\>-d\\!\\\\n;echo echo Good luck in finding your Bug \\:D \\!\\\\n;echo echo \\\\n;echo set prompt \\(tks-gdb\\);cat;echo set prompt;echo echo \\\\nQuitting Session ...\\\\n;echo set \\$eax=tmpEAX;echo set \\$ebx=tmpEBX;echo set \\$ecx=tmpECX;echo set \\$edx=tmpEDX;echo set \\$esi=tmpESI;echo set \\$edi=tmpEDI;echo set \\$cs=tmpCS;echo set \\$ds=tmpDS;echo set \\$es=tmpES;echo set \\$fs=tmpFS;echo set \\$gs=tmpGS;echo set \\$ss=tmpSS;echo set \\$esp=tmpESP;echo set \\$ebp=tmpEBP;echo set \\$eip=tmpEIP;echo set \\$eflags=tmpEFlags;echo;echo detach;echo quit;sleep 2)|gdb --quiet %s %d;echo", 
                            info->si_addr,(char*)info->si_addr-16,(char*)info->si_addr+16, 
                            context->uc_mcontext.gregs[REG_EAX],context->uc_mcontext.gregs[REG_EBX], 
                            context->uc_mcontext.gregs[REG_ECX],context->uc_mcontext.gregs[REG_EDX], 
                            context->uc_mcontext.gregs[REG_ESI],context->uc_mcontext.gregs[REG_EDI], 
                            context->uc_mcontext.gregs[REG_CS],context->uc_mcontext.gregs[REG_DS], 
                            context->uc_mcontext.gregs[REG_ES],context->uc_mcontext.gregs[REG_FS], 
                            context->uc_mcontext.gregs[REG_GS],context->uc_mcontext.gregs[REG_SS], 
                            context->uc_mcontext.gregs[REG_EBP],context->uc_mcontext.gregs[REG_ESP], 
                            context->uc_mcontext.gregs[REG_EIP],context->uc_mcontext.gregs[REG_EFL], 
                            executableName,currentPID); 
         printf("Executing %d Byte String '%s'...\n",buflen,buffer); 
         int rsys = system(buffer);
         (void)rsys;
      }       

#endif //YAC_64

#else // non DX_X86 

      if(NULL == ::getenv("TKS_NO_GDB"))
      {
         char buffer[1024]; 
         pid_t currentPID; 
         currentPID=getpid(); 
         sigprocmask(SIG_UNBLOCK,&blockSet,NULL); 
         
         int buflen=sprintf(buffer,"(echo set prompt;echo set pagination off;echo list *%p;echo disassemble %p %p;echo bt full;echo detach;echo quit;sleep 2)|gdb --quiet %s %d;echo", 
                            info->si_addr,(char*)info->si_addr-16,(char*)info->si_addr+16, 
                            executableName,currentPID); 

         printf("Executing %d Byte String '%s'...\n",buflen,buffer); 
         int rsys = system(buffer);
         (void)rsys;
      }
       
#endif // DX_X86 
       
       
   } 
   exit(3); 
    
} 
 
void sa_sigaction_SIGILL(int sig, siginfo_t *info, void *extra) { 
   yac_host->printf("SIGILL got called!\n"); 
   tkscript->printAllFunctionCallStacks(); 
 
   if (info->si_signo==SIGILL) { 
      yac_host->printf("SIGILL Acknowledged Errno: %d Signal Source: %d\n",info->si_errno,info->si_code); 
      yac_host->printf("Source: "); 
      switch (info->si_code) { 
      case SI_USER:yac_host->printf("User raised Signal!\n"); 
				     break; 
      case SI_KERNEL:yac_host->printf("Kernel raised Signal!\n"); 
         break; 
      case SI_QUEUE:yac_host->printf("Signal Queue sent Signal!\n"); 
         break; 
      case SI_TIMER:yac_host->printf("Timer expired\n"); 
         break; 
      case SI_MESGQ:yac_host->printf("Message Queue state changed\n"); 
         break; 
      case SI_ASYNCIO:yac_host->printf("AIO completed\n"); 
         break; 
      case SI_SIGIO:yac_host->printf("queued SIGIO\n"); 
         break; 
      case ILL_ILLOPC:yac_host->printf("Illegal Opcode!\n"); 
         break; 
      case ILL_ILLOPN:yac_host->printf("Illegal Operand!\n"); 
         break; 
      case ILL_ILLADR:yac_host->printf("Illegal Addressing Mode!\n"); 
         break; 
      case ILL_ILLTRP:yac_host->printf("Illegal Trap!\n"); 
         break; 
      case ILL_PRVOPC:yac_host->printf("Privileged Opcode!\n"); 
         break; 
      case ILL_PRVREG:yac_host->printf("Privileged Register!\n"); 
         break; 
      case ILL_COPROC:yac_host->printf("Coprocessor Error!\n"); 
         break; 
      case ILL_BADSTK:yac_host->printf("Internal Stack Error!\n"); 
         break; 
      default:yac_host->printf("Unknown Code: %d\n",info->si_code); 
      } 
      yac_host->printf("Program failed at address %010p\n",info->si_addr); 
      sigset_t blockSet; 
      sigemptyset(&blockSet); 
      sigaddset(&blockSet,SIGFPE); 
      sigaddset(&blockSet,SIGILL); 
      sigaddset(&blockSet,SIGSEGV); 
      sigprocmask(SIG_BLOCK,&blockSet,NULL); 
       
#ifdef DX_X86 
      ucontext_t *context=(ucontext_t*)extra; 
#ifdef YAC_64 //64bit
      yac_host->printf("RAX: %016x RBX: %016x RCX: %016x RDX: %016x RSI: %016x RDI: %016x\n", 
         context->uc_mcontext.gregs[REG_RAX],context->uc_mcontext.gregs[REG_RBX],context->uc_mcontext.gregs[REG_RCX],context->uc_mcontext.gregs[REG_RDX], 
         context->uc_mcontext.gregs[REG_RSI],context->uc_mcontext.gregs[REG_RDI]); 
      yac_host->printf("RSP: %016x RBP: %016x RIP: %016x\n", 
         context->uc_mcontext.gregs[REG_RSP],context->uc_mcontext.gregs[REG_RBP],context->uc_mcontext.gregs[REG_RIP]); 
      yac_host->printf("R08: %016x R09: %016x R10: %016x R11: %016x R12: %016x R13: %016x R14: %016x R15: %016x\n", 
         context->uc_mcontext.gregs[REG_R8],context->uc_mcontext.gregs[REG_R9],context->uc_mcontext.gregs[REG_R10],context->uc_mcontext.gregs[REG_R11], 
         context->uc_mcontext.gregs[REG_R12],context->uc_mcontext.gregs[REG_R13],context->uc_mcontext.gregs[REG_R14],context->uc_mcontext.gregs[REG_R15]); 
      yac_host->printf("CSGSFS:  %016x\n", 
         context->uc_mcontext.gregs[REG_CSGSFS]); 
      yac_host->printf("TrapNo: %016x Error: %016x EFlags %016x CR2: %016x OLDMASK: %016x\n", 
         context->uc_mcontext.gregs[REG_TRAPNO],context->uc_mcontext.gregs[REG_ERR],context->uc_mcontext.gregs[REG_EFL],
	 context->uc_mcontext.gregs[REG_CR2],context->uc_mcontext.gregs[REG_OLDMASK]); 

	if (context->uc_mcontext.fpregs) {
		int i;
		yac_host->printf("FPU cwd: %04x swd: %04x ftw: %04x fop: %04x\n",
			context->uc_mcontext.fpregs->cwd, context->uc_mcontext.fpregs->swd,
			context->uc_mcontext.fpregs->ftw, context->uc_mcontext.fpregs->fop);
		yac_host->printf("    rip: %016x rdp: %016x mxcsr: %08x mxcr_mask: %08x\n",
			context->uc_mcontext.fpregs->rip, context->uc_mcontext.fpregs->rdp,
			context->uc_mcontext.fpregs->mxcsr, context->uc_mcontext.fpregs->mxcr_mask);
		yac_host->printf("FPU Stack:\n");
		for (i=0;i<8;i++) 
			yac_host->printf("st(%d)=signficand %04x%04x%04x%04x exp %04x\n",i,
				context->uc_mcontext.fpregs->_st[i].significand[0],
				context->uc_mcontext.fpregs->_st[i].significand[1],
				context->uc_mcontext.fpregs->_st[i].significand[2],
				context->uc_mcontext.fpregs->_st[i].significand[3],
				context->uc_mcontext.fpregs->_st[i].exponent);
		yac_host->printf("XMM Registers:\n"); // X86_64 has both fpu and xmm separated of each other
		for (i=0;i<16;i++)
			yac_host->printf("xmm[%d]=%08x, %08x, %08x, %08x\n",i,
				context->uc_mcontext.fpregs->_xmm[i].element[0],
				context->uc_mcontext.fpregs->_xmm[i].element[1],
				context->uc_mcontext.fpregs->_xmm[i].element[2],
				context->uc_mcontext.fpregs->_xmm[i].element[3]);
	}
      if (info->si_addr!=(void*)context->uc_mcontext.gregs[REG_RIP]) info->si_addr=(void*)context->uc_mcontext.gregs[REG_RIP]; 

      if(NULL == ::getenv("TKS_NO_GDB"))
      {
         char buffer[16384]; 
         
         pid_t currentPID; 
         currentPID=getpid(); 
         sigprocmask(SIG_UNBLOCK,&blockSet,NULL); 
         int buflen=sprintf(buffer,"(echo set prompt;echo set pagination off;echo list *%p;echo disassemble %p %p;echo set tmpRAX=\\$rax;echo set tmpRBX=\\$rbx;echo set tmpRCX=\\$rcx;echo set tmpRDX=\\$rdx;echo set tmpRSI=\\$rsi;echo set tmpRDI=\\$rdi; echo set tmpR8=\\$r8; echo set tmpR9=\\$r9; echo set tmpR10=\\$r10; echo set tmpR11=\\$r11; echo set tmpR12=\\$r12; echo set tmpR13=\\$r13; echo set tmpR14=\\$r14; echo set tmpR15=\\$r15; echo set tmpCS=\\$cs;echo set tmpDS=\\$ds;echo set tmpES=\\$es;echo set tmpFS=\\$fs;echo set tmpGS=\\$gs;echo set tmpSS=\\$ss;echo set tmpRIP=\\$rip;echo set tmpRBP=\\$rbp;echo set tmpRSP=\\$rsp;echo set tmpEFlags=\\$eflags;echo set \\$rax=0x%016lx;echo set \\$rbx=0x%016lx;echo set \\$rcx=0x%016lx;echo set \\$rdx=0x%016lx;echo set \\$rsi=0x%016lx;echo set \\$rdi=0x%016lx;echo set \\$r8=0x%016lx; echo set \\$r9=0x%016lx; echo set \\$r10=0x%016lx; echo set \\$r11=0x%016lx; echo set \\$r12=0x%016lx; echo set \\$r13=0x%016lx; echo set \\$r14=0x%016lx; echo set \\$r15=0x%016lx; echo set \\$cs=0x%04x;echo set \\$fs=0x%04x;echo set \\$gs=0x%04x; echo set \\$rbp=0x%016lx;echo set \\$rsp=0x%016lx;echo set \\$rip=0x%016lx;echo set \\$eflags=0x%016lx;echo bt full;echo frame 0;echo info frame 0;echo echo Please exit the debugger through pressing \\<CTRL\\>-d\\!\\\\n;echo echo Good luck in finding your Bug \\:D \\!\\\\n;echo echo \\\\n;echo set prompt \\(tks-gdb\\);cat;echo set prompt;echo echo \\\\nQuitting Session ...\\\\n;echo set \\$rax=tmpRAX;echo set \\$rbx=tmpRBX;echo set \\$rcx=tmpRCX;echo set \\$rdx=tmpRDX;echo set \\$rsi=tmpRSI;echo set \\$rdi=tmpRDI;echo set \\$r8=tmpR8; echo set \\$r9=tmpR9; echo set \\$r10=tmpR10; echo set \\$r11=tmpR11; echo set \\$r12=tmpR12; echo set \\$r13=tmpR13; echo set \\$r14=tmpR14; echo set \\$r15=tmpR15; echo set \\$cs=tmpCS;echo set \\$ds=tmpDS;echo set \\$es=tmpES;echo set \\$fs=tmpFS;echo set \\$gs=tmpGS;echo set \\$ss=tmpSS;echo set \\$rsp=tmpRSP;echo set \\$rbp=tmpRBP;echo set \\$rip=tmpRIP;echo set \\$eflags=tmpEFlags;echo;echo detach;echo quit;sleep 2)|gdb --quiet %s %d;echo", 
                            info->si_addr,(char*)info->si_addr-16,(char*)info->si_addr+16, 
                            context->uc_mcontext.gregs[REG_RAX],context->uc_mcontext.gregs[REG_RBX], 
                            context->uc_mcontext.gregs[REG_RCX],context->uc_mcontext.gregs[REG_RDX], 
                            context->uc_mcontext.gregs[REG_RSI],context->uc_mcontext.gregs[REG_RDI], 
                            context->uc_mcontext.gregs[REG_R8],context->uc_mcontext.gregs[REG_R9],
                            context->uc_mcontext.gregs[REG_R10],context->uc_mcontext.gregs[REG_R11],
                            context->uc_mcontext.gregs[REG_R12],context->uc_mcontext.gregs[REG_R13],
                            context->uc_mcontext.gregs[REG_R14],context->uc_mcontext.gregs[REG_R15],
                            (unsigned int)((context->uc_mcontext.gregs[REG_CSGSFS])&0xffff), //context->uc_mcontext.gregs[REG_DS], 
                            //context->uc_mcontext.gregs[REG_ES],
                            (unsigned int)(((context->uc_mcontext.gregs[REG_CSGSFS])>>32)&0xffff), 
                            (unsigned int)(((context->uc_mcontext.gregs[REG_CSGSFS])>>16)&0xffff),//context->uc_mcontext.gregs[REG_SS], 
                            context->uc_mcontext.gregs[REG_RBP],context->uc_mcontext.gregs[REG_RSP], 
                            context->uc_mcontext.gregs[REG_RIP],context->uc_mcontext.gregs[REG_EFL], 
                            executableName,currentPID); 
         
         printf("Executing %d Byte String '%s'...\n",buflen,buffer); 
         int rsys = system(buffer);
         (void)rsys;
      }

#else //32bit
      yac_host->printf("EAX: %08x EBX: %08x ECX: %08x EDX: %08x ESI: %08x EDI: %08x\n", 
         context->uc_mcontext.gregs[REG_EAX],context->uc_mcontext.gregs[REG_EBX],context->uc_mcontext.gregs[REG_ECX],context->uc_mcontext.gregs[REG_EDX], 
         context->uc_mcontext.gregs[REG_ESI],context->uc_mcontext.gregs[REG_EDI]); 
      yac_host->printf("ESP: %08x EBP: %08x EIP: %08x\n", 
         context->uc_mcontext.gregs[REG_ESP],context->uc_mcontext.gregs[REG_EBP],context->uc_mcontext.gregs[REG_EIP]); 
      yac_host->printf("CS:  %08x DS:  %08x ES:  %08x FS:  %08x GS:  %08x SS:  %08x\n", 
         context->uc_mcontext.gregs[REG_CS],context->uc_mcontext.gregs[REG_DS],context->uc_mcontext.gregs[REG_ES],context->uc_mcontext.gregs[REG_FS], 
         context->uc_mcontext.gregs[REG_GS],context->uc_mcontext.gregs[REG_SS]); 
      yac_host->printf("TrapNo: %08x Error: %08x EFlags %08x\n", 
         context->uc_mcontext.gregs[REG_TRAPNO],context->uc_mcontext.gregs[REG_ERR],context->uc_mcontext.gregs[REG_EFL]); 

      if (context->uc_mcontext.fpregs) { 
         yac_host->printf("FPU cw: %08x sw: %08x tag: %08x ipoff: %08x cssel: %08x dataoff: %08x datasel: %08x status %08x\n", 
            context->uc_mcontext.fpregs->cw,context->uc_mcontext.fpregs->sw,context->uc_mcontext.fpregs->tag, 
            context->uc_mcontext.fpregs->ipoff,context->uc_mcontext.fpregs->cssel,context->uc_mcontext.fpregs->dataoff, 
            context->uc_mcontext.fpregs->datasel,context->uc_mcontext.fpregs->status); 
         for (int iter=0;iter<8;iter++) { 
            yac_host->printf("st[%d]=",iter); 
            for (int jter=0;jter<4;jter++) yac_host->printf("%04x",context->uc_mcontext.fpregs->_st[iter].significand[jter]); 
            yac_host->printf(" exp %04x\n",context->uc_mcontext.fpregs->_st[iter].exponent); 
         } 
      } 
      if (info->si_addr!=(void*)context->uc_mcontext.gregs[REG_EIP]) info->si_addr=(void*)context->uc_mcontext.gregs[REG_EIP]; 

      if(NULL == ::getenv("TKS_NO_GDB"))
      {
       
         char buffer[4096]; 
       
         pid_t currentPID; 
         currentPID=getpid(); 
         sigprocmask(SIG_UNBLOCK,&blockSet,NULL); 
         int buflen=sprintf(buffer,"(echo set prompt;echo set pagination off;echo list *%p;echo disassemble %p %p;echo set tmpEAX=\\$eax;echo set tmpEBX=\\$ebx;echo set tmpECX=\\$ecx;echo set tmpEDX=\\$edx;echo set tmpESI=\\$esi;echo set tmpEDI=\\$edi;echo set tmpCS=\\$cs;echo set tmpDS=\\$ds;echo set tmpES=\\$es;echo set tmpFS=\\$fs;echo set tmpGS=\\$gs;echo set tmpSS=\\$ss;echo set tmpEIP=\\$eip;echo set tmpEBP=\\$ebp;echo set tmpESP=\\$esp;echo set tmpEFlags=\\$eflags;echo set \\$eax=0x%08x;echo set \\$ebx=0x%08x;echo set \\$ecx=0x%08x;echo set \\$edx=0x%08x;echo set \\$esi=0x%08x;echo set \\$edi=0x%08x;echo set \\$cs=0x%08x;echo set \\$ds=0x%08x;echo set \\$es=0x%08x;echo set \\$fs=0x%08x;echo set \\$gs=0x%08x;echo set \\$ss=0x%08x; echo set \\$ebp=0x%08x;echo set \\$esp=0x%08x;echo set \\$eip=0x%08x;echo set \\$eflags=0x%08x;echo bt full;echo frame 0;echo info frame 0;echo echo Please exit the debugger through pressing \\<CTRL\\>-d\\!\\\\n;echo echo Good luck in finding your Bug \\:D \\!\\\\n;echo echo \\\\n;echo set prompt \\(tks-gdb\\);cat;echo set prompt;echo echo \\\\nQuitting Session ...\\\\n;echo set \\$eax=tmpEAX;echo set \\$ebx=tmpEBX;echo set \\$ecx=tmpECX;echo set \\$edx=tmpEDX;echo set \\$esi=tmpESI;echo set \\$edi=tmpEDI;echo set \\$cs=tmpCS;echo set \\$ds=tmpDS;echo set \\$es=tmpES;echo set \\$fs=tmpFS;echo set \\$gs=tmpGS;echo set \\$ss=tmpSS;echo set \\$esp=tmpESP;echo set \\$ebp=tmpEBP;echo set \\$eip=tmpEIP;echo set \\$eflags=tmpEFlags;echo;echo detach;echo quit;sleep 2)|gdb --quiet %s %d;echo", 
                            info->si_addr,(char*)info->si_addr-16,(char*)info->si_addr+16, 
                            context->uc_mcontext.gregs[REG_EAX],context->uc_mcontext.gregs[REG_EBX], 
                            context->uc_mcontext.gregs[REG_ECX],context->uc_mcontext.gregs[REG_EDX], 
                            context->uc_mcontext.gregs[REG_ESI],context->uc_mcontext.gregs[REG_EDI], 
                            context->uc_mcontext.gregs[REG_CS],context->uc_mcontext.gregs[REG_DS], 
                            context->uc_mcontext.gregs[REG_ES],context->uc_mcontext.gregs[REG_FS], 
                            context->uc_mcontext.gregs[REG_GS],context->uc_mcontext.gregs[REG_SS], 
                            context->uc_mcontext.gregs[REG_EBP],context->uc_mcontext.gregs[REG_ESP], 
                            context->uc_mcontext.gregs[REG_EIP],context->uc_mcontext.gregs[REG_EFL], 
                            executableName,currentPID); 
         printf("Executing %d Byte String '%s'...\n",buflen,buffer); 
         int rsys = system(buffer);
         (void)rsys;
      }

#endif //YAC_64

#else // non DX_X86 

      if(NULL == ::getenv("TKS_NO_GDB"))
      {
         char buffer[1024]; 
         pid_t currentPID; 
         currentPID=getpid(); 
         sigprocmask(SIG_UNBLOCK,&blockSet,NULL); 
         int buflen=sprintf(buffer,"(echo set prompt;echo set pagination off;echo list *%p;echo disassemble %p %p;echo bt full;echo detach;echo quit;sleep 2)|gdb --quiet %s %d;echo", 
                            info->si_addr,(char*)info->si_addr-16,(char*)info->si_addr+16, 
                            executableName,currentPID); 
         printf("Executing %d Byte String '%s'...\n",buflen,buffer); 
         int rsys = system(buffer);
         (void)rsys;
      }
       
#endif // DX_X86 

   } 
   exit(2); 
} 
 
void sa_sigaction_SIGSEGV(int sig, siginfo_t *info, void *extra) {
       yac_host->printf("SIGSEGV got called -new version-\n");
	       
   yac_host->printf("SIGSEGV got called!\n"); 
   tkscript->printAllFunctionCallStacks(); 
 
   if (info->si_signo==SIGSEGV) { 
      yac_host->printf("[...] sa_sigaction_SIGSEGV(): SIGSEGV Acknowledged Errno: %d Signal Source: %d\n",info->si_errno,info->si_code); 
      yac_host->printf("[...] sa_sigaction_SIGSEGV(): Source: "); 
      switch (info->si_code) { 
      case SI_USER:yac_host->printf("User raised Signal!\n"); 
				     break; 
      case SI_KERNEL:yac_host->printf("Kernel raised Signal!\n"); 
         break; 
      case SI_QUEUE:yac_host->printf("Signal Queue sent Signal!\n"); 
         break; 
      case SI_TIMER:yac_host->printf("Timer expired\n"); 
         break; 
      case SI_MESGQ:yac_host->printf("Message Queue state changed\n"); 
         break; 
      case SI_ASYNCIO:yac_host->printf("AIO completed\n"); 
         break; 
      case SI_SIGIO:yac_host->printf("queued SIGIO\n"); 
         break; 
      case SEGV_MAPERR:yac_host->printf("Address not mapped to Object!\n"); 
         break; 
      case SEGV_ACCERR:yac_host->printf("Invalid Permissions for mapped Object!\n"); 
         break; 
          
      default:yac_host->printf("[---] sa_sigaction_SIGSEGV(): Unknown Code: %d\n",info->si_code); break; 
      } 
      yac_host->printf("[---] sa_sigaction_SIGSEGV(): Program failed at address %010p\n",info->si_addr); 
      sigset_t blockSet; 
      sigemptyset(&blockSet); 
      sigaddset(&blockSet,SIGFPE); 
      sigaddset(&blockSet,SIGILL); 
      sigaddset(&blockSet,SIGSEGV); 
      sigprocmask(SIG_BLOCK,&blockSet,NULL); 

      
#ifdef DX_X86 
      ucontext_t *context=(ucontext_t*)extra; 
#ifdef YAC_64 //64bit
      yac_host->printf("RAX: %016x RBX: %016x RCX: %016x RDX: %016x RSI: %016x RDI: %016x\n", 
         context->uc_mcontext.gregs[REG_RAX],context->uc_mcontext.gregs[REG_RBX],context->uc_mcontext.gregs[REG_RCX],context->uc_mcontext.gregs[REG_RDX], 
         context->uc_mcontext.gregs[REG_RSI],context->uc_mcontext.gregs[REG_RDI]); 
      yac_host->printf("RSP: %016x RBP: %016x RIP: %016x\n", 
         context->uc_mcontext.gregs[REG_RSP],context->uc_mcontext.gregs[REG_RBP],context->uc_mcontext.gregs[REG_RIP]); 
      yac_host->printf("R08: %016x R09: %016x R10: %016x R11: %016x R12: %016x R13: %016x R14: %016x R15: %016x\n", 
         context->uc_mcontext.gregs[REG_R8],context->uc_mcontext.gregs[REG_R9],context->uc_mcontext.gregs[REG_R10],context->uc_mcontext.gregs[REG_R11], 
         context->uc_mcontext.gregs[REG_R12],context->uc_mcontext.gregs[REG_R13],context->uc_mcontext.gregs[REG_R14],context->uc_mcontext.gregs[REG_R15]); 
      yac_host->printf("CSGSFS:  %016x\n", 
         context->uc_mcontext.gregs[REG_CSGSFS]); 
      yac_host->printf("TrapNo: %016x Error: %016x EFlags %016x CR2: %016x OLDMASK: %016x\n", 
         context->uc_mcontext.gregs[REG_TRAPNO],context->uc_mcontext.gregs[REG_ERR],context->uc_mcontext.gregs[REG_EFL],
	 context->uc_mcontext.gregs[REG_CR2],context->uc_mcontext.gregs[REG_OLDMASK]); 

	if (context->uc_mcontext.fpregs) {
		int i;
		yac_host->printf("FPU cwd: %04x swd: %04x ftw: %04x fop: %04x\n",
			context->uc_mcontext.fpregs->cwd, context->uc_mcontext.fpregs->swd,
			context->uc_mcontext.fpregs->ftw, context->uc_mcontext.fpregs->fop);
		yac_host->printf("    rip: %016x rdp: %016x mxcsr: %08x mxcr_mask: %08x\n",
			context->uc_mcontext.fpregs->rip, context->uc_mcontext.fpregs->rdp,
			context->uc_mcontext.fpregs->mxcsr, context->uc_mcontext.fpregs->mxcr_mask);
		yac_host->printf("FPU Stack:\n");
		for (i=0;i<8;i++) 
			yac_host->printf("st(%d)=signficand %04x%04x%04x%04x exp %04x\n",i,
				context->uc_mcontext.fpregs->_st[i].significand[0],
				context->uc_mcontext.fpregs->_st[i].significand[1],
				context->uc_mcontext.fpregs->_st[i].significand[2],
				context->uc_mcontext.fpregs->_st[i].significand[3],
				context->uc_mcontext.fpregs->_st[i].exponent);
		yac_host->printf("XMM Registers:\n"); // X86_64 has both fpu and xmm separated of each other
		for (i=0;i<16;i++)
			yac_host->printf("xmm[%d]=%08x, %08x, %08x, %08x\n",i,
				context->uc_mcontext.fpregs->_xmm[i].element[0],
				context->uc_mcontext.fpregs->_xmm[i].element[1],
				context->uc_mcontext.fpregs->_xmm[i].element[2],
				context->uc_mcontext.fpregs->_xmm[i].element[3]);
	}
   if (info->si_addr!=(void*)context->uc_mcontext.gregs[REG_RIP]) 
      info->si_addr=(void*)context->uc_mcontext.gregs[REG_RIP]; 

   if(NULL == ::getenv("TKS_NO_GDB"))
   {
      char buffer[16384]; 
      
      pid_t currentPID; 
      currentPID=getpid(); 
      sigprocmask(SIG_UNBLOCK,&blockSet,NULL); 
      int buflen=sprintf(buffer,"(echo set prompt;echo set pagination off;echo list *%p;echo disassemble %p %p;echo set tmpRAX=\\$rax;echo set tmpRBX=\\$rbx;echo set tmpRCX=\\$rcx;echo set tmpRDX=\\$rdx;echo set tmpRSI=\\$rsi;echo set tmpRDI=\\$rdi; echo set tmpR8=\\$r8; echo set tmpR9=\\$r9; echo set tmpR10=\\$r10; echo set tmpR11=\\$r11; echo set tmpR12=\\$r12; echo set tmpR13=\\$r13; echo set tmpR14=\\$r14; echo set tmpR15=\\$r15; echo set tmpCS=\\$cs;echo set tmpDS=\\$ds;echo set tmpES=\\$es;echo set tmpFS=\\$fs;echo set tmpGS=\\$gs;echo set tmpSS=\\$ss;echo set tmpRIP=\\$rip;echo set tmpRBP=\\$rbp;echo set tmpRSP=\\$rsp;echo set tmpEFlags=\\$eflags;echo set \\$rax=0x%016lx;echo set \\$rbx=0x%016lx;echo set \\$rcx=0x%016lx;echo set \\$rdx=0x%016lx;echo set \\$rsi=0x%016lx;echo set \\$rdi=0x%016lx;echo set \\$r8=0x%016lx; echo set \\$r9=0x%016lx; echo set \\$r10=0x%016lx; echo set \\$r11=0x%016lx; echo set \\$r12=0x%016lx; echo set \\$r13=0x%016lx; echo set \\$r14=0x%016lx; echo set \\$r15=0x%016lx; echo set \\$cs=0x%04x;echo set \\$fs=0x%04x;echo set \\$gs=0x%04x; echo set \\$rbp=0x%016lx;echo set \\$rsp=0x%016lx;echo set \\$rip=0x%016lx;echo set \\$eflags=0x%016lx;echo bt full;echo frame 0;echo info frame 0;echo echo Please exit the debugger through pressing \\<CTRL\\>-d\\!\\\\n;echo echo Good luck in finding your Bug \\:D \\!\\\\n;echo echo \\\\n;echo set prompt \\(tks-gdb\\);cat;echo set prompt;echo echo \\\\nQuitting Session ...\\\\n;echo set \\$rax=tmpRAX;echo set \\$rbx=tmpRBX;echo set \\$rcx=tmpRCX;echo set \\$rdx=tmpRDX;echo set \\$rsi=tmpRSI;echo set \\$rdi=tmpRDI;echo set \\$r8=tmpR8; echo set \\$r9=tmpR9; echo set \\$r10=tmpR10; echo set \\$r11=tmpR11; echo set \\$r12=tmpR12; echo set \\$r13=tmpR13; echo set \\$r14=tmpR14; echo set \\$r15=tmpR15; echo set \\$cs=tmpCS;echo set \\$ds=tmpDS;echo set \\$es=tmpES;echo set \\$fs=tmpFS;echo set \\$gs=tmpGS;echo set \\$ss=tmpSS;echo set \\$rsp=tmpRSP;echo set \\$rbp=tmpRBP;echo set \\$rip=tmpRIP;echo set \\$eflags=tmpEFlags;echo;echo detach;echo quit;sleep 2)|gdb --quiet %s %d;echo", 
                         info->si_addr,(char*)info->si_addr-16,(char*)info->si_addr+16, 
                         context->uc_mcontext.gregs[REG_RAX],context->uc_mcontext.gregs[REG_RBX], 
                         context->uc_mcontext.gregs[REG_RCX],context->uc_mcontext.gregs[REG_RDX], 
                         context->uc_mcontext.gregs[REG_RSI],context->uc_mcontext.gregs[REG_RDI], 
                         context->uc_mcontext.gregs[REG_R8],context->uc_mcontext.gregs[REG_R9],
                         context->uc_mcontext.gregs[REG_R10],context->uc_mcontext.gregs[REG_R11],
                         context->uc_mcontext.gregs[REG_R12],context->uc_mcontext.gregs[REG_R13],
                         context->uc_mcontext.gregs[REG_R14],context->uc_mcontext.gregs[REG_R15],
                         (unsigned int)((context->uc_mcontext.gregs[REG_CSGSFS])&0xffff), //context->uc_mcontext.gregs[REG_DS], 
                         //context->uc_mcontext.gregs[REG_ES],
                         (unsigned int)(((context->uc_mcontext.gregs[REG_CSGSFS])>>32)&0xffff), 
                         (unsigned int)(((context->uc_mcontext.gregs[REG_CSGSFS])>>16)&0xffff),//context->uc_mcontext.gregs[REG_SS], 
                         context->uc_mcontext.gregs[REG_RBP],context->uc_mcontext.gregs[REG_RSP], 
                         context->uc_mcontext.gregs[REG_RIP],context->uc_mcontext.gregs[REG_EFL], 
                         executableName,currentPID); 

      printf("Executing %d Byte String '%s'...\n",buflen,buffer); 
      int rsys = system(buffer);
      (void)rsys;
   }

#else //32bit
      yac_host->printf("EAX: %08x EBX: %08x ECX: %08x EDX: %08x ESI: %08x EDI: %08x\n", 
         context->uc_mcontext.gregs[REG_EAX],context->uc_mcontext.gregs[REG_EBX],context->uc_mcontext.gregs[REG_ECX],context->uc_mcontext.gregs[REG_EDX], 
         context->uc_mcontext.gregs[REG_ESI],context->uc_mcontext.gregs[REG_EDI]); 
      yac_host->printf("ESP: %08x EBP: %08x EIP: %08x\n", 
         context->uc_mcontext.gregs[REG_ESP],context->uc_mcontext.gregs[REG_EBP],context->uc_mcontext.gregs[REG_EIP]); 
      yac_host->printf("CS:  %08x DS:  %08x ES:  %08x FS:  %08x GS:  %08x SS:  %08x\n", 
         context->uc_mcontext.gregs[REG_CS],context->uc_mcontext.gregs[REG_DS],context->uc_mcontext.gregs[REG_ES],context->uc_mcontext.gregs[REG_FS], 
         context->uc_mcontext.gregs[REG_GS],context->uc_mcontext.gregs[REG_SS]); 
      yac_host->printf("TrapNo: %08x Error: %08x EFlags %08x\n", 
         context->uc_mcontext.gregs[REG_TRAPNO],context->uc_mcontext.gregs[REG_ERR],context->uc_mcontext.gregs[REG_EFL]); 

      if (context->uc_mcontext.fpregs) { 
         yac_host->printf("FPU cw: %08x sw: %08x tag: %08x ipoff: %08x cssel: %08x dataoff: %08x datasel: %08x status %08x\n", 
            context->uc_mcontext.fpregs->cw,context->uc_mcontext.fpregs->sw,context->uc_mcontext.fpregs->tag, 
            context->uc_mcontext.fpregs->ipoff,context->uc_mcontext.fpregs->cssel,context->uc_mcontext.fpregs->dataoff, 
            context->uc_mcontext.fpregs->datasel,context->uc_mcontext.fpregs->status); 
         for (int iter=0;iter<8;iter++) { 
            yac_host->printf("st[%d]=",iter); 
            for (int jter=0;jter<4;jter++) yac_host->printf("%04x",context->uc_mcontext.fpregs->_st[iter].significand[jter]); 
            yac_host->printf(" exp %04x\n",context->uc_mcontext.fpregs->_st[iter].exponent); 
         } 
      } 
      if (info->si_addr!=(void*)context->uc_mcontext.gregs[REG_EIP]) info->si_addr=(void*)context->uc_mcontext.gregs[REG_EIP]; 
      
      if(NULL == ::getenv("TKS_NO_GDB"))
      {
       
         char buffer[4096]; 
         
         pid_t currentPID; 
         currentPID=getpid(); 
         sigprocmask(SIG_UNBLOCK,&blockSet,NULL); 
         int buflen=sprintf(buffer,"(echo set prompt;echo set pagination off;echo list *%p;echo disassemble %p %p;echo set tmpEAX=\\$eax;echo set tmpEBX=\\$ebx;echo set tmpECX=\\$ecx;echo set tmpEDX=\\$edx;echo set tmpESI=\\$esi;echo set tmpEDI=\\$edi;echo set tmpCS=\\$cs;echo set tmpDS=\\$ds;echo set tmpES=\\$es;echo set tmpFS=\\$fs;echo set tmpGS=\\$gs;echo set tmpSS=\\$ss;echo set tmpEIP=\\$eip;echo set tmpEBP=\\$ebp;echo set tmpESP=\\$esp;echo set tmpEFlags=\\$eflags;echo set \\$eax=0x%08x;echo set \\$ebx=0x%08x;echo set \\$ecx=0x%08x;echo set \\$edx=0x%08x;echo set \\$esi=0x%08x;echo set \\$edi=0x%08x;echo set \\$cs=0x%08x;echo set \\$ds=0x%08x;echo set \\$es=0x%08x;echo set \\$fs=0x%08x;echo set \\$gs=0x%08x;echo set \\$ss=0x%08x; echo set \\$ebp=0x%08x;echo set \\$esp=0x%08x;echo set \\$eip=0x%08x;echo set \\$eflags=0x%08x;echo bt full;echo frame 0;echo info frame 0;echo echo Please exit the debugger through pressing \\<CTRL\\>-d\\!\\\\n;echo echo Good luck in finding your Bug \\:D \\!\\\\n;echo echo \\\\n;echo set prompt \\(tks-gdb\\);cat;echo set prompt;echo echo \\\\nQuitting Session ...\\\\n;echo set \\$eax=tmpEAX;echo set \\$ebx=tmpEBX;echo set \\$ecx=tmpECX;echo set \\$edx=tmpEDX;echo set \\$esi=tmpESI;echo set \\$edi=tmpEDI;echo set \\$cs=tmpCS;echo set \\$ds=tmpDS;echo set \\$es=tmpES;echo set \\$fs=tmpFS;echo set \\$gs=tmpGS;echo set \\$ss=tmpSS;echo set \\$esp=tmpESP;echo set \\$ebp=tmpEBP;echo set \\$eip=tmpEIP;echo set \\$eflags=tmpEFlags;echo;echo detach;echo quit;sleep 2)|gdb --quiet %s %d;echo", 
                            info->si_addr,(char*)info->si_addr-16,(char*)info->si_addr+16, 
                            context->uc_mcontext.gregs[REG_EAX],context->uc_mcontext.gregs[REG_EBX], 
                            context->uc_mcontext.gregs[REG_ECX],context->uc_mcontext.gregs[REG_EDX], 
                            context->uc_mcontext.gregs[REG_ESI],context->uc_mcontext.gregs[REG_EDI], 
                            context->uc_mcontext.gregs[REG_CS],context->uc_mcontext.gregs[REG_DS], 
                            context->uc_mcontext.gregs[REG_ES],context->uc_mcontext.gregs[REG_FS], 
                            context->uc_mcontext.gregs[REG_GS],context->uc_mcontext.gregs[REG_SS], 
                            context->uc_mcontext.gregs[REG_EBP],context->uc_mcontext.gregs[REG_ESP], 
                            context->uc_mcontext.gregs[REG_EIP],context->uc_mcontext.gregs[REG_EFL], 
                            executableName,currentPID); 

         printf("Executing %d Byte String '%s'...\n",buflen,buffer); 
         int rsys = system(buffer);
         (void)rsys;
      }
       

#endif //YAC_64

#else // non DX_X86 

      if(NULL == ::getenv("TKS_NO_GDB"))
      {
         char buffer[1024]; 
         pid_t currentPID; 
         currentPID=getpid(); 
         sigprocmask(SIG_UNBLOCK,&blockSet,NULL); 
         int buflen=sprintf(buffer,"(echo set prompt;echo set pagination off;echo list *%p;echo disassemble %p %p;echo bt full;echo detach;echo quit;sleep 2)|gdb --quiet %s %d;echo", 
                            info->si_addr,(char*)info->si_addr-16,(char*)info->si_addr+16, 
                            executableName,currentPID); 

         printf("Executing %d Byte String '%s'...\n",buflen,buffer); 
         int rsys = system(buffer);
         (void)rsys;
      }
       
#endif // DX_X86 

   } 
   exit(1); 
} 
 
void tks_debug_init(int _argc, char **_argv) { 
   //::printf("installing new Signal Handlers\n"); 
   executableName=_argv[0]; 
//::printf("Entering tks_debug_init\n");
    
   ::sigemptyset(&mySignalSet); 
    
   struct sigaction FPEAction; 
   FPEAction.sa_sigaction=&sa_sigaction_SIGFPE; 
   FPEAction.sa_mask=mySignalSet; 
   FPEAction.sa_flags=SA_SIGINFO; //we want to get extensive knowledge of the error cause 
   int res=::sigaction(SIGFPE,&FPEAction,NULL);  
   if (res<0)  
   { 
      ::printf("[---] tks_debug_init<linux>(): sigaction failed: (FPE) %d\n",res); 
   } 
    
   struct sigaction ILLAction; 
   ILLAction.sa_sigaction=&sa_sigaction_SIGILL; 
   ILLAction.sa_mask=mySignalSet; 
   ILLAction.sa_flags=SA_SIGINFO; //we want to get extensive knowledge of the error cause 
   res=::sigaction(SIGILL,&ILLAction,NULL); 
   if (res<0)  
   { 
      ::printf("[---] tks_debug_init<linux>(): sigaction failed: (ILL) %d\n",res); 
   } 
    
   struct sigaction SEGVAction; 
   SEGVAction.sa_sigaction=&sa_sigaction_SIGSEGV; 
   SEGVAction.sa_mask=mySignalSet; 
   SEGVAction.sa_flags=SA_SIGINFO; //we want to get extensive knowledge of the error cause 
   res=::sigaction(SIGSEGV,&SEGVAction,NULL); 
   if (res<0)  
   { 
      ::printf("[---] tks_debug_init<linux>(): sigaction failed: (SEGV) %d\n",res); 
   } 
    
#ifdef DX_X86 
#ifdef YAC_64
   tmpRBP=tmpRSP=tmpRIP=tmpRAX=tmpRBX=tmpRCX=tmpRDX=tmpRSI=tmpRDI=tmpR8=tmpR9=tmpR10=tmpR11=tmpR12=tmpR13=tmpR14=tmpR15=tmpCS=tmpDS=tmpES=tmpFS=tmpGS=tmpSS=tmpEFlags=0;
#else
   //make the compiler happy and use the vars for him 
   tmpEBP=tmpESP=tmpEIP=tmpEAX=tmpEBX=tmpECX=tmpEDX=tmpESI=tmpEDI=tmpCS=tmpDS=tmpES=tmpFS=tmpGS=tmpSS=tmpEFlags=0; 
#endif
#endif 
   //Test Scenarios 
/*      ::printf("provoking SIGFPE\n"); 
   double test=5.3; 
   test/=0.33; 
   res=0; 
   res=1/0;*/
/*      ::printf("provoking SIGSEGV\n"); 
   *(int*)0=*(int*)1; */
/*      ::printf("provoking SIGILL\n"); 
   ::raise(SIGILL);*/
    
} 
 
void tks_debug_exit(void) { 
} 
 
#endif 
 

