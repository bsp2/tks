/// VMCore.cpp
///
/// (c) 2002-2014 Bastian Spiegel <bs@tkscript.de>
///     - distributed under terms of the GNU general public license (GPL).
///

//#define SAVE_CPUTABLE defined

//#define JIT_DEBUG defined

#include "tks.h"

#ifdef TKS_JIT

// System page size, adjust if necessary. Needed for code page alignment
#define TKSVM_PAGE_SIZE 4096

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef YAC_LINUX
#include <sys/mman.h>
#ifdef DX_ARM
#include <sys/unistd.h>
#include <sys/syscall.h>
#endif // DX_ARM
#endif // YAC_LINUX

#ifdef YAC_WIN32
#include <windows.h>
#endif

#include "tks_inc_class.h"
#include "tks_inc_jit.h"
#include "vmopsz.h"
////#include "TKS_Mutex.h"
#include "YAC_IntArray.h"
#include "YAC_FloatArray.h"
#include "YAC_StringArray.h"
#include "YAC_ObjectArray.h"
#include "YAC_PointerArray.h"
#include "YAC_ClassArray.h"

#define TKSVM_LVAR_CONTEXT 0
#define TKSVM_LVAR_ENV 1

#ifndef SAVE_CPUTABLE 
#ifdef DX_X86
#include "tkscputbl_x86.cpp"
#endif
#ifdef DX_68K
#include "tkscputbl_68k.cpp"
#endif
#ifdef DX_ARM
#include "tkscputbl_arm.cpp"
#endif
#else
#ifdef DX_X86
#include "vmjit.cpp"
#endif // DX_X86
#ifdef DX_ARM
extern "C" {
}
#endif // DX_ARM
#endif // SAVE_CPUTABLE


#ifdef DX_ARM
extern "C" {
extern void ocx_prologue(void);
extern sSI __aeabi_idivmod(sSI, sSI);
extern sSI __divsi3(sSI, sSI);
}
#endif // DX_ARM


// 0x31bdc-4*C0FF = 0x17e0
//#define Dconstoff 0x17e0
//#define Dioff 0x18e0
//#define Dvaroff 0x15e0
//#define Diaoff 0x1160
//#define Dfaoff 0x1320 
#ifdef YAC_LITTLE_ENDIAN
#define Dopsep 0xFFFF05C7
#define Dopsep2 0xCCCCFFFF 
#else 
#define Dopsep 0xC705FFFF 
#define Dopsep2 0xFFFFCCCC 
#endif
// sizeof opcode seperator
#ifdef DX_X86
#define Dsizepad 10 
#endif

#ifdef DX_68K
#define Dsizepad 8
#endif

#ifdef DX_ARM
#define Dsizepad 8
#endif

#ifdef YAC_64
#define TKS_ALIGNPTR(p, s) ((sU8*) ( ( ((sU64)(p)) + ((s)-1) ) & (~((s)-1)) ) )
#else
#define TKS_ALIGNPTR(p, s) ((sU8*) ( ( ((sU32)(p)) + ((s)-1) ) & (~((s)-1)) ) )
#endif // YAC_64

// size of temporary variable space relative to ebp
#define Dltmpsize 0x20 

// position of array boundary limits -0x1c=low, -0x18=hi
#define Dlboundoff 0x1c

sUI vm86_off[VM_NUMOPCODES];
sU16 vm86_sz[VM_NUMOPCODES];
sUI vm86del;

static sU8 *vm_i86;
static sU8 *vm_i86first; /// pointer to first opcode in CPUtable
sU16 vm_sz_startup;
sU16 vm_sz_cleanup;

// include vm_x86_ocargs, vm_x86_ocargid tables
//#include "vm86args.cpp"

static sUI vm_iarr_dummy=0;
static sF32 vm_farr_dummy=0.0f;
////static sUI vm_array_size_table[]={0, 1,3,7,15,31,63,127,255,511,1023,2047,4095,8191,16383,32767,65535,131071,262143,524287,1048576,2097151,4194303,8388607,16777215,0};

sU8 *off_opcodes[65536];  // maps opcode# to generated code address
sU16 woff_opcodes[65536]; // maps vm_pc to opcode#
 
//[">",">=","<","<=","!=","=="]   => ["<=","<",">=",">","==","!="]; 
static const sUI op_invert_tbl[6]={3, 2, 1, 0, 5, 4}; 


////TKS_Context *VMCore::context = NULL; 
////PTN_Env *VMCore::vm_env = NULL; 

#ifdef TKS_MT
////TKS_Mutex *VMCore::mutex = NULL; // TKS_MT: JIT prologue is protected by mutex
////sSI VMCore::vmcore__objectcounter = 0;
#endif

////PTN_Env tksvm_env;   // XXXX not thread safe


#ifdef DX_ARM
static void loc_arm_cache_flush(void *_startAddr, void *_endAddr) {
#ifdef JIT_DEBUG
   printf("xxx __clear_cache: start=%p end=%p sz=%u\n", _startAddr, _endAddr, (sU32)_endAddr - (sU32)_startAddr);
#endif
   __clear_cache(_startAddr, _endAddr);
#if 0
#ifdef __ARM_NR_cacheflush
   sUI result;

   ////syscall(__ARM_NR_cacheflush, _startAddr, _endAddr, 0); // Seems to be broken on many ARM Linux installations ?!

   // Call cacheflush syscall directly
   __asm__ __volatile__ (
			 "mov     r0, %1\n"
			 "sub     r1, %2, #1\n"
			 "mov     r2, #0\n"
			 "swi     " __sys1(__ARM_NR_cacheflush) "\n"
			 "mov     %0, r0\n"
			 : "=r" (result)
			 : "r" (_startAddr), "r" (_endAddr)
			 : "r0","r1","r2");
   
#else
#warning __ARM_NR_cacheflush is not available !
#endif // __ARM_NR_cacheflush
#endif // 0
}
#endif // DX_ARM


#ifdef DX_ARM
#if 0
static sF32 tksvm_sinf(sF32 _a) {
   return (sF32)sin(_a);
}
#endif
#endif // DX_ARM


VMCore::VMCore(void) {
	sUI i;

	for(i=0; i<TKS_VMCORE_NUMARRAYS; i++)
	{
		vm_iarr[i].adr.u32 = &vm_iarr_dummy;
		vm_iarr[i].maskb=0;
		vm_iarr[i].iadr=0;
		vm_iarr[i].num_elements=0;
		vm_iarr[i].ia = NULL;

		vm_farr[i].adr.f32 = &vm_farr_dummy;
		vm_farr[i].mask=0;
		vm_farr[i].iadr=0;
		vm_farr[i].num_elements=0;
		vm_farr[i].fa = NULL;
	}

	for(i=0; i<TKS_VMCORE_NUMCONSTS; i++)
	{
		vm_const[i].u32=0;
		vm_constused[i]=0;
	}

	for(i=0; i<TKS_VMCORE_NUMVARS; i++)
	{
		vm_vars[i].u32=0;
		vm_varnames[i]="var";
	}

	for(i=0; i<TKS_VMCORE_NUMLVARS; i++) 
	{ 
		vm_localvarnames[i]=0; //"lvar"; 
	} 


	vm_code                 = NULL;
	parser_tree             = NULL;
	jit.sz_code             = 0;
   jit.sz_aligned          = 0;
	jit.code                = NULL; 
   jit.nonaligned_code     = NULL;
	jit.b_allowArrayCache   = YAC_TRUE; 
	////tksvm_env.continue_flag = 1; 

#ifdef TKS_MT
   ////if(vmcore__objectcounter++ == 0)
   ////{
   ////   VMCore::mutex = new TKS_Mutex();
   ////}
#endif // TKS_MT
}

VMCore::~VMCore() {
	clean();

#ifdef TKS_MT
   ////if(--vmcore__objectcounter == 0)
   ////{
   ////   delete VMCore::mutex;
   ////   VMCore::mutex = NULL;
   ////}
#endif // TKS_MT
}
 
void VMCore::cleanJIT(void) { 
	if(NULL != vm_code) 
   { 
      delete [] vm_code;
      vm_code = NULL; 
   }

	if(NULL != jit.nonaligned_code) 
	{ 
#ifdef YAC_LINUX
      if(NULL != jit.code)
      {
         ::mprotect((void*)jit.code, (size_t)jit.sz_aligned, PROT_READ | PROT_WRITE | PROT_EXEC);
      }
#endif // YAC_LINUX
		delete [] jit.nonaligned_code; 
      jit.nonaligned_code = NULL; 
		jit.code = NULL; 
		jit.sz_code=0; 
	} 
} 

void VMCore::clean(void) {
   cleanJIT(); 

   if(parser_tree)
   {
      delete parser_tree;
      parser_tree = NULL;
   } 
}

void VMCore::InitTables(void) {
	static int init_done=0; if(init_done) return; init_done=1; 
	VMCore::ScanCPUTable();
}

sUI VMCore::getID(void) const {
	return PTN_FOR;
}

void VMCore::prepareCompile(void) {
	vm_code = new sU16[65536];
	vm_pc = 0;
	use_arrays = 0;

#ifdef DX_ARM
   // must start with 8-byte aligned stack
   //  (note) the way the stack is aligned is actually wrong and only works with functions <5 args.
   //         If the stack is not aligned to 8 bytes, functions like printf/sscanf will break.
   //         Other ones will still work so as a hack/workaround the alignment is only done
   //         for functions that pass all args in registers (r0..r3) and for functions with
   //         more args ..well, they shouldn't call printf etc.. o_O
   //         On the upside, the 'YAC' C++ interface usually wraps the entire arglist in
   //         a single argument and calls the native function via a wrapper function so
   //         this limitation only affects the builtin tks functions (see api.cpp)
   current_stack_offset = 0;
#endif

   addLocalVarRef("=context=");
   addLocalVarRef("=env=");

#ifdef DX_ARM
   addConstInt((sS32)&__aeabi_idivmod);  // const 0 (# 0)
   addConstInt((sS32)&sinf);             // const 1 (# 4)
   ////addConstInt((sS32)&tksvm_sinf);       // const 1 (# 4)
   addConstInt((sS32)&cosf);             // const 2 (# 8)
   addConstInt((sS32)&tanf);             // const 3 (#12)
   addConstInt((sS32)&__divsi3);         // const 4 (#16)
#endif // DX_ARM

   // Write JIT prologue:

   // Read pointer to current script context and store in local var =context=
   ////pushci( (sS32) &VMCore::context );  // xxx not 64bit safe
   ////addAPICall1( (sUI) &tksvm_get_ipointer);
   ////addOpcode(VMOP_SRETI);
	////addOpcode(VMOP_POPLV);
	////vm_code[vm_pc++] = TKSVM_LVAR_CONTEXT; // 0=first added local var ("=context=")

   // Read pointer to current script env. and store in local var =env=
   ////pushci( (sS32) &VMCore::vm_env );  // xxx not 64bit safe
   ////addAPICall1( (sUI) &tksvm_get_ipointer);
   ////addOpcode(VMOP_SRETI);
	////addOpcode(VMOP_POPLV);
	////vm_code[vm_pc++] = TKSVM_LVAR_ENV; // 1=second added local var ("=env=")

#ifdef TKS_MT
   ////pushci( (sS32) VMCore::mutex );  // xxx not 64bit safe
   ////addAPICall1( (sUI) tksvm_unlock_mutex );
   ////addOpcode(VMOP_INCSTP); // cleanup mutex pointer
#endif
   
}

void VMCore::finishCompile(void) {
	if(vm_pc && vm_code)
	{
		sU16 *nw = new sU16[vm_pc+8];
		sUI i;
		for(i=0; i<vm_pc; i++)
      {
         nw[i] = vm_code[i];
      }
		delete [] vm_code;
		vm_code=nw;
	}

}

void VMCore::addOpcode(sU16 _opcodeid) {
   if(vm_pc < 65536)
   {
      vm_code[vm_pc++] = _opcodeid;

#ifdef DX_ARM
      current_stack_offset += (sSI)vm_opcodestz[_opcodeid];
      // (note) peephole optimization will not change stack offset during apicalls
#endif
   }
}

sU16 VMCore::addConstInt(sSI _i) {
	if(vm_pc<65536)
	{
		sU16 i;
		for(i=0; i<TKS_VMCORE_NUMCONSTS; i++)
		{
			if(vm_constused[i]&&vm_const[i].si==_i)
			{
				//vm_code[vm_pc++]=i;
				return i;
			}
			if(!vm_constused[i])
			{
				vm_const[i].si=_i;
				vm_constused[i]=1;
				//vm_code[vm_pc++]=i;
				return i;
			}
		}
		//------- too many constants --------
	}
	return 0;
}

void VMCore::addConstFloat(sF32 _f) {
	if(vm_pc < 65536u)
	{
		sU16 i;
		for(i=0; i<TKS_VMCORE_NUMCONSTS; i++)
		{
         yacmemptr p; p.any = (void*)&_f;
			if(vm_constused[i] && vm_const[i].ui==*p.u32)
			{
				vm_code[vm_pc++]=i;
				return;
			}
			if(!vm_constused[i])
			{
				vm_const[i].f32=_f;
				vm_constused[i]=1;
				vm_code[vm_pc++]=i;
				return;
			}
		}
		//------- too many constants --------
	}
}

sU16 VMCore::addVarRef(yacmemptr _d, const char *_name) {
	if(vm_pc<65536)
	{
		sU16 i;
		for(i=0; i<TKS_VMCORE_NUMVARS; i++)
		{
			if(vm_vars[i].any)
			{
				if(vm_vars[i].any==_d.any)
				{
					//-------- use existing var reference --------
					//vm_code[vm_pc++]=i;
					return i;
				}
			}
			else
			{
				vm_vars[i].any=_d.any;
				vm_varnames[i]=_name;
				//vm_code[vm_pc++]=i;
				return i;
			}
		}
		//-------- variable table full --------
		tkscript->printf("[---] VMCore::addVarRef: table is full.\n");
	}
	return 0;
}



sU16 VMCore::addLocalVarRef(const char *_name) {
	if(vm_pc<65536)
	{
		sU16 i;
		for(i=0; i<TKS_VMCORE_NUMLVARS; i++)
		{
			if(vm_localvarnames[i])
			{
				if(vm_localvarnames[i]==_name) // xxx compare pointers
				{
					//-------- use existing var reference --------
					//vm_code[vm_pc++]=i;
					return i;
				}
			}
			else
			{
				//vm_lvars[i].any=_d.any;
				vm_localvarnames[i]=_name;
				//vm_code[vm_pc++]=i;
				return i;
			}
		}
		//-------- variable table full --------
		tkscript->printf("[---] VMCore::addLocalVarRef: table is full.\n");
	}
	return 0;
}


sU16 VMCore::addIntArray(TKS_CachedObject *_ia) {
	use_arrays=1;
	//sUI mask=0;
	sU16 i;
	for(i=0; i<TKS_VMCORE_NUMARRAYS; i++)
	{
		if(vm_iarr[i].ia)
		{
			if(vm_iarr[i].ia==_ia)
         {
#ifdef DX_ARM
            // (note) use vm_vars to load int** in IASEL
            yacmemptr d;
            d.iany = &vm_iarr[i].adr.any;
            return addVarRef(d, "iarr");
#else
				return i;
#endif // DX_ARM
         }
		}
		else
		{
			if((!vm_iarr[i].iadr)&&(!vm_iarr[i].num_elements))
			{
				//-------- add new int array --------
				vm_iarr[i].ia = _ia;
#ifdef DX_ARM
            // (note) use vm_vars to load int** in IASEL
            yacmemptr d;
            d.iany = &vm_iarr[i].adr.any;
            return addVarRef(d, "iarr");
#else
				return i;
#endif // DX_ARM
			}
		}
	}
	// xxx table full
	return 0;
}

sU16 VMCore::addFloatArray(TKS_CachedObject *_fa) {
	use_arrays=1;
	sU16 i;
	for(i=0; i<TKS_VMCORE_NUMARRAYS; i++)
	{
		if(vm_farr[i].fa)
		{
			if(vm_farr[i].fa==_fa)
         {
#ifdef DX_ARM
            // (note) use vm_vars to load float** in FASEL
            yacmemptr d;
            d.iany = &vm_farr[i].adr.any;
            return addVarRef(d, "farr");
#else
				return i;
#endif // DX_ARM
         }
		}
		else
		{
			if((!vm_farr[i].iadr)&&(!vm_farr[i].num_elements))
			{
				//-------- add new float array --------
				vm_farr[i].fa = _fa;

#ifdef DX_ARM
            // (note) use vm_vars to load float** in FASEL
            yacmemptr d;
            d.iany = &vm_farr[i].adr.any;
            return addVarRef(d, "farr");
#else
				return i;
#endif // DX_ARM
			}
		}
	}
	// xxx table full
	return 0;
}

void VMCore::typecastStack(sU8 _from, sU8 _to) {
	switch(_from)
	{
	case 0:
		switch(_to)
		{
		case 0:
			break;
		case 1:
			pushci(0);
			break;
		case 2:
			pushcf(0.0f);
			break;
		case 3: 
			/// xxx push 0 object 
			Dprintf("[---] VMCore: warning: cannot typecast %i to %i\n", _from, _to); 
			break;
		}
		break;
	case 1:
		switch(_to)
		{
		case 0:
			addOpcode(VMOP_SIPOP); // pop 4 bytes
			break;
		case 1:
			break;
		case 2:
			addOpcode(VMOP_STCIF);
			break;
		case 3:
			/////addOpcodeIDX(VMOPCL_STCIO);
			Dprintf("[---] VMCore: warning: cannot typecast %i to %i\n", _from, _to); 
			break;
		}
		break;

	case 2:
		switch(_to)
		{
		case 0:
			addOpcode(VMOP_SIPOP); // pop 4 bytes
			break;
		case 1:
			addOpcode(VMOP_STCFI);
			break;
		case 2:
			break;
		case 3:
			//addOpcodeIDX(VMOPCL_STCFO); 
			Dprintf("[---] VMCore: warning: cannot typecast %i to %i\n", _from, _to); 
			break;
		}
		break;

	case 3:
		switch(_to)
		{
		case 0:
			/// XXX pop object
			Dprintf("[---] VMCore: warning: cannot typecast %i to %i\n", _from, _to); 
			break;
		case 1:
			////addOpcodeIDX(VMOPCL_STCOI);
         //addOpcodeIDX(VMOPCL_SITESTBZ); 
//#ifdef TKS_DCON 
//			tkscript->printf("[---] VMCore: warning: cannot typecast %i to %i\n", _from, _to); 
//#endif 
			break;
		case 2:
			//addOpcodeIDX(VMOPCL_STCOF);
         addOpcode(VMOP_SITESTBZ); 
         addOpcode(VMOP_STCIF); 
//#ifdef TKS_DCON 
//			tkscript->printf("[---] VMCore: warning: cannot typecast %i to %i\n", _from, _to); 
//#endif 
			break;
		case 3:
			break;
		}
		break;
	}
}

void VMCore::disassemble(YAC_String *_s, sU16 _pc, sU16 _epc) { 
#ifdef DX_VMDASM
#define Ddasm1(a,s)     if(oc<((a)+(1))) { _s->append((s)); _pc++; oc=0xffff; }
#define Ddasm1lv(a,s)   if(oc<((a)+(1))) { t.printf((s), vm_localvarnames[vm_code[_pc+1]]); _s->append(&t); _pc+=2; oc=0xffff; }
#define Ddasm1c(a,s)    if(oc<((a)+(1))) { t.printf((s), vm_const[vm_code[_pc+1]].s32, vm_const[vm_code[_pc+1]].f32); _s->append(&t); _pc+=2; oc=0xffff; }
#define Ddasm1v(a,s)    if(oc<((a)+(1))) { t.printf((s), vm_varnames[vm_code[_pc+1]]); _s->append(&t); _pc+=2; oc=0xffff; }
#define Ddasm1lvlv(a,s) if(oc<((a)+(1))) { t.printf((s), vm_localvarnames[vm_code[_pc+1]], vm_localvarnames[vm_code[_pc+2]]); _s->append(&t); _pc+=3; oc=0xffff; }
#define Ddasm1lvci(a,s) if(oc<((a)+(1))) { t.printf((s), vm_localvarnames[vm_code[_pc+1]], vm_const[vm_code[_pc+2]].s32); _s->append(&t); _pc+=3; oc=0xffff; }
#define Ddasm1lvcf(a,s) if(oc<((a)+(1))) { t.printf((s), vm_localvarnames[vm_code[_pc+1]], vm_const[vm_code[_pc+2]].f32); _s->append(&t); _pc+=3; oc=0xffff; } 
#define Ddasm1vv(a,s)   if(oc<((a)+1))   { t.printf((s), vm_varnames[vm_code[_pc+1]], vm_varnames[vm_code[_pc+2]]); _s->append(&t); _pc+=3; oc=0xffff; } 
#define Ddasm1vci(a,s)  if(oc<((a)+1))   { t.printf((s), vm_varnames[vm_code[_pc+1]], vm_const[vm_code[_pc+2]].s32); _s->append(&t); _pc+=3; oc=0xffff; } 
#define Ddasm1vcf(a,s)  if(oc<((a)+1))   { t.printf((s), vm_varnames[vm_code[_pc+1]], vm_const[vm_code[_pc+2]].f32); _s->append(&t); _pc+=3; oc=0xffff; } 

	
	YAC_String t;
	t.alloc(1024);
	static const char*opnames[6]={">",">=","<","<=","!=","=="};
	sU16 off;
	sU16 cmdi=0;
	while(_pc<_epc)
	{
		t.printf("(#%04i)%08x: ", cmdi++, _pc);
		_s->append(&t);
		sU16 oc=vm_code[_pc];
		Ddasm1(VMOP_HALT, "halt;\n");
		if(oc<(VMOP_BRA+1))
		{
			t.printf("bra %04x;\n", vm_code[_pc+1]);
			_s->append(&t);
			_pc+=2;
			oc=0xFFFF;
		}
		////if(oc<(vmopcl[VMOPCL_SFCMP]+6))
		////{
		////	off=oc-vmopcl[VMOPCL_SFCMP];
		////	t.printf("sfcmp %s %04x;\n", opnames[off], vm_code[_pc+1]);
		////	_s->append(&t);
		////	_pc+=2;
		////	oc=0xFFFF;
		////}
		////if(oc<(vmopcl[VMOPCL_SICMP]+6))
		////{
		////	off=oc-vmopcl[VMOPCL_SICMP];
		////	t.printf("sicmp %s %04x;\n", opnames[off], vm_code[_pc+1]);
		////	_s->append(&t);
		////	_pc+=2;
		////	oc=0xFFFF;
		////}
		if(oc<(VMOP_SITESTZ+1))
		{
			off = oc - VMOP_SITESTZ;
			t.printf("sitestz %04x;\n", vm_code[_pc+1]);
			_s->append(&t);
			_pc+=2;
			oc=0xFFFF;
		}
		if(oc<(VMOP_SITESTZP+1))
		{
			off = oc - VMOP_SITESTZP;
			t.printf("sitestzp %04x;\n", vm_code[_pc+1]);
			_s->append(&t);
			_pc+=2;
			oc=0xFFFF;
		}
		if(oc<(VMOP_BSR+1))
		{
			t.printf("bsr %04x;\n", vm_code[_pc+1]);
			_s->append(&t);
			_pc+=2;
			oc=0xFFFF;
		}
		Ddasm1(VMOP_RTS, "rts;\n");
		if(oc<(VMOP_SILOOP+1))
		{
			t.printf("siloop %04x;\n", vm_code[_pc+1]);
			_s->append(&t);
			_pc+=2;
			oc=0xFFFF;
		}
		////Ddasm1(VMOPCL_SWAPS, "swaps;\n");
		Ddasm1lv(VMOP_PUSHLV, "pushlv %s;\n");
		Ddasm1lv(VMOP_PUSHLIVINC, "pushlivinc %s;\n");
		Ddasm1lv(VMOP_PUSHINCLIV, "pushincliv %s;\n");
		Ddasm1lv(VMOP_PUSHLIVDEC, "pushlivdec %s;\n");
		Ddasm1lv(VMOP_PUSHDECLIV, "pushdecliv %s;\n");
		Ddasm1lv(VMOP_POPLV, "poplv %s;\n");
		Ddasm1v(VMOP_PUSHV, "pushv %s;\n");
		Ddasm1v(VMOP_PUSHIVINC, "pushivinc %s;\n");
		Ddasm1v(VMOP_PUSHINCIV, "pushinciv %s;\n");
		Ddasm1v(VMOP_PUSHIVDEC, "pushivdec %s;\n");
		Ddasm1v(VMOP_PUSHDECIV, "pushdeciv %s;\n");
		Ddasm1v(VMOP_POPV, "popv %s;\n");
		Ddasm1c(VMOP_PUSHC, "pushc %i (%ff);\n"); 
		Ddasm1c(VMOP_LOADFXC, "loadfxc %i (%ff);\n"); 
		Ddasm1lvlv(VMOP_MOVLV, "movlv %s %s;\n");
		if(oc<(VMOP_MOVLVC+1))
		{
			t.printf("movlvc %s %i (%ff);\n", vm_localvarnames[vm_code[_pc+1]], vm_const[vm_code[_pc+2]].s32, vm_const[vm_code[_pc+2]].f32);
			_s->append(&t);
			_pc+=3;
			oc=0xFFFF;
		} 
		Ddasm1vv(VMOP_MOVV, "movv %s %s;\n");
		if(oc<(VMOP_MOVVC+1))
		{
			t.printf("movvc %s %i (%ff);\n", vm_varnames[vm_code[_pc+1]], vm_const[vm_code[_pc+2]].s32, vm_const[vm_code[_pc+2]].f32);
			_s->append(&t);
			_pc+=3;
			oc=0xFFFF;
		}
		Ddasm1lv(VMOP_INCLIV, "incliv %s;\n");
		Ddasm1lv(VMOP_DECLIV, "decliv %s;\n");
		Ddasm1v(VMOP_INCIV, "inciv %s;\n");
		Ddasm1v(VMOP_DECIV, "deciv %s;\n"); 
 
		Ddasm1lvlv(VMOP_LIVANDLIV, "livandliv %s %s;\n");
		Ddasm1lvlv(VMOP_LIVORLIV, "livorliv %s %s;\n");
		Ddasm1lvlv(VMOP_LIVEORLIV, "liveorliv %s %s;\n");
		Ddasm1lvlv(VMOP_LIVMODLIV, "livmodliv %s %s;\n");
		Ddasm1lvlv(VMOP_LIVADDLIV, "livaddliv %s %s;\n");
		Ddasm1lvlv(VMOP_LIVSUBLIV, "livsubliv %s %s;\n");
		Ddasm1lvlv(VMOP_LIVMULLIV, "livmulliv %s %s;\n");
		Ddasm1lvlv(VMOP_LIVDIVLIV, "livdivliv %s %s;\n");
		Ddasm1lvlv(VMOP_LIVASLLIV, "livaslliv %s %s;\n");
		Ddasm1lvlv(VMOP_LIVASRLIV, "livasrliv %s %s;\n");
		Ddasm1lvlv(VMOP_PUSHLIVANDLIV, "pushlivandliv %s %s;\n");
		Ddasm1lvlv(VMOP_PUSHLIVORLIV, "pushlivorliv %s %s;\n");
		Ddasm1lvlv(VMOP_PUSHLIVEORLIV, "pushliveorliv %s %s;\n");
		Ddasm1lvlv(VMOP_PUSHLIVMODLIV, "pushlivmodliv %s %s;\n");
		Ddasm1lvlv(VMOP_PUSHLIVADDLIV, "pushlivaddliv %s %s;\n");
		Ddasm1lvlv(VMOP_PUSHLIVSUBLIV, "pushlivsubliv %s %s;\n");
		Ddasm1lvlv(VMOP_PUSHLIVMULLIV, "pushlivmulliv %s %s;\n");
		Ddasm1lvlv(VMOP_PUSHLIVDIVLIV, "pushlivdivliv %s %s;\n");
		Ddasm1lvlv(VMOP_PUSHLIVASLLIV, "pushlivaslliv %s %s;\n");
		Ddasm1lvlv(VMOP_PUSHLIVASRLIV, "pushlivasrliv %s %s;\n"); 

		////Ddasm1lvci(VMOPCL_LIVANDC, "livandc %s %i;\n");
		////Ddasm1lvci(VMOPCL_LIVORC,  "livorc %s %i;\n");
		////Ddasm1lvci(VMOPCL_LIVEORC, "liveorc %s %i;\n");
		////Ddasm1lvci(VMOPCL_LIVMODC, "livmodc %s %i;\n");
		////Ddasm1lvci(VMOPCL_LIVADDC, "livaddc %s %i;\n");
		////Ddasm1lvci(VMOPCL_LIVSUBC, "livsubc %s %i;\n");
		////Ddasm1lvci(VMOPCL_LIVMULC, "livmulc %s %i;\n");
		////Ddasm1lvci(VMOPCL_LIVDIVC, "livdivc %s %i;\n");
		////Ddasm1lvci(VMOPCL_LIVASLC, "livaslc %s %i;\n");
		////Ddasm1lvci(VMOPCL_LIVASRC, "livasrc %s %i;\n");
		Ddasm1lvci(VMOP_PUSHLIVANDC, "pushlivandc %s %i;\n");
		Ddasm1lvci(VMOP_PUSHLIVORC, "pushlivorc %s %i;\n");
		Ddasm1lvci(VMOP_PUSHLIVEORC, "pushliveorc %s %i;\n");
		Ddasm1lvci(VMOP_PUSHLIVMODC, "pushlivmodc %s %i;\n");
		Ddasm1lvci(VMOP_PUSHLIVADDC, "pushlivaddc %s %i;\n");
		Ddasm1lvci(VMOP_PUSHLIVSUBC, "pushlivsubc %s %i;\n");
		Ddasm1lvci(VMOP_PUSHLIVMULC, "pushlivmulc %s %i;\n");
		Ddasm1lvci(VMOP_PUSHLIVDIVC, "pushlivdivc %s %i;\n");
		Ddasm1lvci(VMOP_PUSHLIVASLC, "pushlivaslc %s %i;\n");
		Ddasm1lvci(VMOP_PUSHLIVASRC, "pushlivasrc %s %i;\n"); 

		Ddasm1lvlv(VMOP_LFVADDLFV, "lfvaddlfv %s %s;\n");
		Ddasm1lvlv(VMOP_LFVSUBLFV, "lfvsublfv %s %s;\n");
		Ddasm1lvlv(VMOP_LFVMULLFV, "lfvmullfv %s %s;\n");
		Ddasm1lvlv(VMOP_LFVDIVLFV, "lfvdivlfv %s %s;\n");
		Ddasm1lvlv(VMOP_PUSHLFVADDLFV, "pushlfvaddlfv %s %s;\n");
		Ddasm1lvlv(VMOP_PUSHLFVSUBLFV, "pushlfvsublfv %s %s;\n");
		Ddasm1lvlv(VMOP_PUSHLFVMULLFV, "pushlfvmullfv %s %s;\n");
		Ddasm1lvlv(VMOP_PUSHLFVDIVLFV, "pushlfvdivlfv %s %s;\n"); 

		////Ddasm1lvcf(VMOPCL_LFVADDC, "lfvaddc %s %f;\n");
		////Ddasm1lvcf(VMOPCL_LFVSUBC, "lfvsubc %s %f;\n");
		////Ddasm1lvcf(VMOPCL_LFVMULC, "lfvmulc %s %f;\n");
		////Ddasm1lvcf(VMOPCL_LFVDIVC, "lfvdivc %s %f;\n");
		Ddasm1lvcf(VMOP_PUSHLFVADDC, "pushlfvaddc %s %f;\n");
		Ddasm1lvcf(VMOP_PUSHLFVSUBC, "pushlfvsubc %s %f;\n");
		Ddasm1lvcf(VMOP_PUSHLFVMULC, "pushlfvmulc %s %f;\n");
		Ddasm1lvcf(VMOP_PUSHLFVDIVC, "pushlfvdivc %s %f;\n"); 

		Ddasm1(VMOP_SINOT, "sinot;\n");
		Ddasm1(VMOP_SIINV, "siinv;\n");
		Ddasm1(VMOP_SINEG, "sineg;\n");
		Ddasm1(VMOP_SIQUAD, "siquad;\n");
		Ddasm1(VMOP_SITESTBZ, "sitestbz;\n");
		Ddasm1(VMOP_SITESTBZ2, "sitestbz2;\n");
		Ddasm1(VMOP_SFNEG, "sfneg;\n");
		Ddasm1(VMOP_SFQUAD, "sfquad;\n");
		////Ddasm1(VMOPCL_SFPOW, "sfpow;\n");
		Ddasm1(VMOP_SFSIN, "sfsin;\n");
		Ddasm1(VMOP_SFCOS, "sfcos;\n");
		Ddasm1(VMOP_SFTAN, "sftan;\n");
		////Ddasm1(VMOPCL_SFATAN2, "sfatan2;\n");
		Ddasm1(VMOP_SFSQRT, "sfsqrt;\n");
		////Ddasm1(VMOPCL_SIRND, "sirnd;\n");
		Ddasm1(VMOP_SIABS, "siabs;\n");
		Ddasm1(VMOP_SFABS, "sfabs;\n");
		Ddasm1(VMOP_SFFRAC, "sffrac;\n");
		Ddasm1(VMOP_SFROUND, "sfround;\n");
		Ddasm1(VMOP_SIAND, "siand;\n");
		Ddasm1(VMOP_SIOR, "sior;\n");
		Ddasm1(VMOP_SIEOR, "sieor;\n");
		Ddasm1(VMOP_SIMOD, "simod;\n");
		Ddasm1(VMOP_SIADD, "siadd;\n");
		Ddasm1(VMOP_SISUB, "sisub;\n");
		////if(oc<(vmopcl[VMOPCL_SIADDQ]+8))
		////{
		////	off=oc-vmopcl[VMOPCL_SIADDQ];
		////	t.printf("siaddq%i;\n", off);
		////	_s->append(&t);
		////	_pc+=1;
		////	oc=0xFFFF;
		////}
		////if(oc<(vmopcl[VMOPCL_SISUBQ]+8))
		////{
		////	off=oc-vmopcl[VMOPCL_SISUBQ];
		////	t.printf("sisubq%i;\n", off);
		////	_s->append(&t);
		////	_pc+=1;
		////	oc=0xFFFF;
		////}
		Ddasm1(VMOP_SIMUL, "simul;\n");
		Ddasm1(VMOP_SIDIV, "sidiv;\n");
		Ddasm1(VMOP_SIASL, "siasl;\n");
		Ddasm1(VMOP_SIASR, "siasr;\n");
		if(oc<(VMOP_SICMPBGT+6))
		{
			off = oc - VMOP_SICMPBGT;
			t.printf("sicmpb %s;\n", opnames[off]);
			_s->append(&t);
			_pc+=1;
			oc=0xFFFF;
		}
		Ddasm1(VMOP_SFADD, "sfadd;\n");
		Ddasm1(VMOP_SFSUB, "sfsub;\n");
		Ddasm1(VMOP_SFMUL, "sfmul;\n");
		Ddasm1(VMOP_SFDIV, "sfdiv;\n");
		if(oc<(VMOP_SFCMPBGT+6))
		{
			off = oc - VMOP_SFCMPBGT;
			t.printf("sfcmpb %s;\n", opnames[off]);
			_s->append(&t);
			_pc+=1;
			oc=0xFFFF;
		}
		Ddasm1(VMOP_STCIF, "stcif;\n");
		Ddasm1(VMOP_STCIF2, "stcif2;\n");
		Ddasm1(VMOP_STCFI, "stcfi;\n");
		Ddasm1(VMOP_STCFI2, "stcfi2;\n");
		if(oc<(VMOP_IASEL+1))
		{
			t.printf("iasel %i;\n", vm_code[_pc+1]);
			_s->append(&t);
			_pc+=2;
			oc=0xFFFF;
		}
		if(oc<(VMOP_FASEL+1))
		{
			t.printf("fasel %i;\n", vm_code[_pc+1]);
			_s->append(&t);
			_pc+=2;
			oc=0xFFFF;
		} 
		Ddasm1(VMOP_SIAPUSHL, "siapushl;\n");
		Ddasm1(VMOP_SIAPUSHLBC, "siapushlbc;\n");
		Ddasm1(VMOP_SIAPOPL, "siapopl;\n");
		Ddasm1(VMOP_SIAPOPLBC, "siapoplbc;\n");
		////Ddasm1(VMOPCL_OTSWITCH, "otswitch;\n");
		////Ddasm1(VMOPCL_SIPUSH, "sipush;\n");
		Ddasm1(VMOP_SIPOP, "sipop;\n");
		////Ddasm1(VMOPCL_STCOI, "stcoi;\n");
		////Ddasm1(VMOPCL_STCOF, "stcof;\n");
		Ddasm1(VMOP_BEGAPICALL0, "begapicall0;\n"); 
		Ddasm1(VMOP_BEGAPICALL1, "begapicall1;\n"); 
		Ddasm1(VMOP_BEGAPICALL2, "begapicall2;\n"); 
		Ddasm1(VMOP_BEGAPICALL3, "begapicall3;\n"); 
		Ddasm1(VMOP_BEGAPICALL4, "begapicall4;\n"); 
		Ddasm1(VMOP_BEGAPICALL5, "begapicall5;\n"); 
		Ddasm1(VMOP_BEGAPICALL6, "begapicall6;\n"); 
		Ddasm1(VMOP_BEGAPICALL7, "begapicall7;\n"); 
		Ddasm1(VMOP_BEGAPICALL8, "begapicall8;\n"); 
		Ddasm1(VMOP_APICALL,  "apicall;\n"); 
		Ddasm1(VMOP_ENDAPICALL0, "endapicall0;\n"); 
		Ddasm1(VMOP_ENDAPICALL1, "endapicall1;\n"); 
		Ddasm1(VMOP_ENDAPICALL2, "endapicall2;\n"); 
		Ddasm1(VMOP_ENDAPICALL3, "endapicall3;\n"); 
		Ddasm1(VMOP_ENDAPICALL4, "endapicall4;\n"); 
		Ddasm1(VMOP_ENDAPICALL5, "endapicall5;\n"); 
		Ddasm1(VMOP_ENDAPICALL6, "endapicall6;\n"); 
		Ddasm1(VMOP_ENDAPICALL7, "endapicall7;\n"); 
		Ddasm1(VMOP_ENDAPICALL8, "endapicall8;\n"); 
		Ddasm1(VMOP_INCSTP, "incstp;\n"); 
		Ddasm1(VMOP_DECSTP, "decstp;\n"); 
		////Ddasm1(VMOPCL_SFSIN01, "sfsin01;\n");
		////Ddasm1(VMOPCL_SFCOS01, "sfcos01;\n");
		////Ddasm1(VMOPCL_SISWAPW, "siswapw;\n");
		Ddasm1(VMOP_SIPACKARGB, "sipackargb;\n");
		////Ddasm1(VMOPCL_SIUNPACKARGB, "siunpackargb;\n");
		////Ddasm1(VMOPCL_SICLAMPB, "siclampb;\n");
		////Ddasm1(VMOPCL_SIMIX212, "simix212;\n");
		////Ddasm1(VMOPCL_SIMIX22, "simix22;\n");
		////Ddasm1(VMOPCL_SIMK16, "simk16;\n");
		////Ddasm1(VMOPCL_SISQRT, "sisqrt;\n");
		Ddasm1(VMOP_SRETI, "sreti;\n"); 
		Ddasm1(VMOP_SRETF, "sretf;\n"); 
		Ddasm1(VMOP_PUSHS, "pushs;\n"); 
 
		Ddasm1vv(VMOP_IVANDIV, "ivandiv %s %s;\n"); 
		Ddasm1vv(VMOP_IVORIV,  "ivorlv %s %s;\n"); 
		Ddasm1vv(VMOP_IVEORIV, "iveoriv %s %s;\n"); 
		Ddasm1vv(VMOP_IVMODIV, "ivmodiv %s %s;\n"); 
		Ddasm1vv(VMOP_IVADDIV, "ivaddiv %s %s;\n"); 
		Ddasm1vv(VMOP_IVSUBIV, "ivsubiv %s %s;\n"); 
		Ddasm1vv(VMOP_IVMULIV, "ivmuliv %s %s;\n"); 
		Ddasm1vv(VMOP_IVDIVIV, "ivdiviv %s %s;\n"); 
		Ddasm1vv(VMOP_IVASLIV, "ivasliv %s %s;\n"); 
		Ddasm1vv(VMOP_IVASRIV, "ivasriv %s %s;\n"); 
		Ddasm1vv(VMOP_PUSHIVANDIV, "pushivandiv %s %s;\n"); 
		Ddasm1vv(VMOP_PUSHIVORIV,  "pushivoriv %s %s;\n"); 
		Ddasm1vv(VMOP_PUSHIVEORIV, "pushiveoriv %s %s;\n"); 
		Ddasm1vv(VMOP_PUSHIVMODIV, "pushivmodiv %s %s;\n"); 
		Ddasm1vv(VMOP_PUSHIVADDIV, "pushivaddiv %s %s;\n"); 
		Ddasm1vv(VMOP_PUSHIVSUBIV, "pushivsubiv %s %s;\n"); 
		Ddasm1vv(VMOP_PUSHIVMULIV, "pushivmuliv %s %s;\n"); 
		Ddasm1vv(VMOP_PUSHIVDIVIV, "pushivdiviv %s %s;\n"); 
		Ddasm1vv(VMOP_PUSHIVASLIV, "pushivasliv %s %s;\n"); 
		Ddasm1vv(VMOP_PUSHIVASRIV, "pushivasriv %s %s;\n"); 
 
		////Ddasm1vci(VMOPCL_IVANDC, "ivandc %s %i;\n"); 
		////Ddasm1vci(VMOPCL_IVORC,  "ivorc %s %i;\n"); 
		////Ddasm1vci(VMOPCL_IVEORC, "iveorc %s %i;\n"); 
		////Ddasm1vci(VMOPCL_IVMODC, "ivmodc %s %i;\n"); 
		////Ddasm1vci(VMOPCL_IVADDC, "ivaddc %s %i;\n"); 
		////Ddasm1vci(VMOPCL_IVSUBC, "ivsubc %s %i;\n"); 
		////Ddasm1vci(VMOPCL_IVMULC, "ivmulc %s %i;\n"); 
		////Ddasm1vci(VMOPCL_IVDIVC, "ivdivc %s %i;\n"); 
		////Ddasm1vci(VMOPCL_IVASLC, "ivaslc %s %i;\n"); 
		////Ddasm1vci(VMOPCL_IVASRC, "ivasrc %s %i;\n"); 
		Ddasm1vci(VMOP_PUSHIVANDC, "pushivandc %s %i;\n"); 
		Ddasm1vci(VMOP_PUSHIVORC,  "pushivorc %s %i;\n"); 
		Ddasm1vci(VMOP_PUSHIVEORC, "pushiveorc %s %i;\n"); 
		Ddasm1vci(VMOP_PUSHIVMODC, "pushivmodc %s %i;\n"); 
		Ddasm1vci(VMOP_PUSHIVADDC, "pushivaddc %s %i;\n"); 
		Ddasm1vci(VMOP_PUSHIVSUBC, "pushivsubc %s %i;\n"); 
		Ddasm1vci(VMOP_PUSHIVMULC, "pushivmulc %s %i;\n"); 
		Ddasm1vci(VMOP_PUSHIVDIVC, "pushivdivc %s %i;\n"); 
		Ddasm1vci(VMOP_PUSHIVASLC, "pushivaslc %s %i;\n"); 
		Ddasm1vci(VMOP_PUSHIVASRC, "pushivasrc %s %i;\n"); 
 
		Ddasm1vv(VMOP_FVADDFV, "fvaddfv %s %s;\n"); 
		Ddasm1vv(VMOP_FVSUBFV, "fvsubfv %s %s;\n"); 
		Ddasm1vv(VMOP_FVMULFV, "fvmulfv %s %s;\n"); 
		Ddasm1vv(VMOP_FVDIVFV, "fvdivfv %s %s;\n"); 
		Ddasm1vv(VMOP_PUSHFVADDFV, "pushfvaddfv %s %s;\n"); 
		Ddasm1vv(VMOP_PUSHFVSUBFV, "pushfvsubfv %s %s;\n"); 
		Ddasm1vv(VMOP_PUSHFVMULFV, "pushfvmulfv %s %s;\n"); 
		Ddasm1vv(VMOP_PUSHFVDIVFV, "pushfvdivfv %s %s;\n"); 
 
		////Ddasm1vcf(VMOPCL_FVADDC, "fvaddc %s %f;\n"); 
		////Ddasm1vcf(VMOPCL_FVSUBC, "fvsubc %s %f;\n"); 
		////Ddasm1vcf(VMOPCL_FVMULC, "fvmulc %s %f;\n"); 
		////Ddasm1vcf(VMOPCL_FVDIVC, "fvdivc %s %f;\n"); 
		Ddasm1vcf(VMOP_PUSHFVADDC, "pushfvaddc %s %f;\n"); 
		Ddasm1vcf(VMOP_PUSHFVSUBC, "pushfvsubc %s %f;\n"); 
		Ddasm1vcf(VMOP_PUSHFVMULC, "pushfvmulc %s %f;\n"); 
		Ddasm1vcf(VMOP_PUSHFVDIVC, "pushfvdivc %s %f;\n"); 
 
		if(oc<(VMOP_JGTIVIV+6)) 
		{ 
			off = oc - VMOP_JGTIVIV; 
			t.printf("jiviv %s %s %s %04x;\n", vm_varnames[vm_code[_pc+1]], opnames[off], vm_varnames[vm_code[_pc+2]], vm_code[_pc+3]); 
			_s->append(&t); 
			_pc+=4; 
			oc=0xFFFF; 
		} 
 
		if(oc<(VMOP_JGTIVIC+6)) 
		{ 
			off = oc - VMOP_JGTIVIC; 
			t.printf("jivic %s %s %i %04x;\n", vm_varnames[vm_code[_pc+1]], opnames[off], vm_const[vm_code[_pc+2]].si, vm_code[_pc+3]); 
			_s->append(&t); 
			_pc+=4; 
			oc=0xFFFF; 
		} 

		if(oc!=0xFFFF)
		{
			_s->append("ill;\n");
			_pc=_epc;
		}
	}
#else 
	tkscript->printf("[---] VMCore: disassembler support not compiled in.\n"); 
#endif
}


#ifdef JIT_DEBUG 
static sU8 *xxx_jit = NULL;
#ifdef DX_ARM
void jit_exec(sU8 *_code, TKS_Context *_context, PTN_Env *_env, void *_vmvars, void *_vmconst) {

   ((void (YAC_CALL*)(TKS_Context *, PTN_Env *, void*, void*))_code)(_context, _env, _vmvars, _vmconst);
}
#else
void jit_exec(sU8 *_code, TKS_Context *_context, PTN_Env *_env) {

   ((void (YAC_CALL*)(TKS_Context *, PTN_Env *))_code)(_context, _env);
}
#endif // DX_ARM
#endif // JIT_DEBUG


static void VMCore__eval(PTN_Env *_env, const PTN_Statement *_st) { 
   Dtracest;
	VMCore *st = (VMCore *)_st; // xxx discard const qualifier because of array tables (iadr -> adr.si
	YAC_FloatArray *fa; 
	YAC_Object *ia; 

	if(tkscript->configuration.b_forceint || (NULL == st->jit.code))
	{
		if(NULL != st->parser_tree)
      {
			st->parser_tree->evalFirst(_env);
      }
		return;
	}

   // TKS_MT: prepare for JIT prologue
#ifdef TKS_MT
   ////st->mutex->lock(); // unlocked at the end of the prologue
#endif

   // (note) the mutex and the vm_env/context fields should be replaced by code that simply
   //         passes these values in registers or the stack. Then again, this whole JIT
   //         should be replaced by something better..
   ////VMCore::vm_env  = _env;
   ////VMCore::context = _env->context;

	if(st->use_arrays)
	{
		sSI i;
		//-------- array initializers --------
		for(i=0; i<TKS_VMCORE_NUMARRAYS; i++)
		{
			// -------- int arrays --------
			if(st->vm_iarr[i].iadr)
			{
				st->vm_iarr[i].adr.s32 = *st->vm_iarr[i].iadr;
				sUI mask=(*st->vm_iarr[i].num_elements)<<2;
				st->vm_iarr[i].maskb=mask   ; // xxx
			}
			else
				if(st->vm_iarr[i].ia)
				{ 
					ia=st->vm_iarr[i].ia->value.object_val; 
					if(ia&&(ia->yacArrayGetElementByteSize()==sizeof(sSI))) 
					{ 
						sUI mask=ia->yacArrayGetHeight()*ia->yacArrayGetStride(); 
						st->vm_iarr[i].adr.s32 = (sSI*)ia->yacArrayGetPointer();//ia->elements; 
						if(!(st->vm_iarr[i].adr.u32 && mask) ) 
							goto vmcore__eval_bail_out;
						st->vm_iarr[i].maskb=mask   ; // xxx
					} 
					else {  
  
vmcore__eval_bail_out: 
#ifdef TKS_MT
                  ////st->mutex->unlock();
#endif
						st->parser_tree->evalFirst(_env);  
						return;  
					} // fallback to interpreter 
				}
				else
					break;
		}

		
		for(i=0; i<TKS_VMCORE_NUMARRAYS; i++)
		{
			// -------- float arrays --------
			if(st->vm_farr[i].iadr)
			{
				st->vm_farr[i].adr.f32=*st->vm_farr[i].iadr;
				st->vm_farr[i].mask=(*st->vm_farr[i].num_elements)<<2;
			}
			else
				if(st->vm_farr[i].fa)
				{
					fa=(YAC_FloatArray*)st->vm_farr[i].fa->value.object_val;
					if(fa&&(fa->yacArrayGetElementByteSize()==sizeof(sF32))) 
					{ 
						st->vm_farr[i].adr.f32=(sF32*)fa->yacArrayGetPointer(); 
						st->vm_farr[i].mask=(fa->yacArrayGetHeight()*fa->yacArrayGetMaxElements())<<2;
						if(!(st->vm_farr[i].adr.f32 && st->vm_farr[i].mask) ) 
							goto vmcore__eval_bail_out; 
					}
					else goto vmcore__eval_bail_out; // fallback to interpreter 
				}
				else
					break;
		}
	}

	/*FILE *f=fopen("jit.bin", "wb+");
   fwrite((void*)st->jit.code, 1024, 1, f);
   fclose(f);*/

#ifdef JIT_DEBUG
   xxx_jit = (sU8*)(st->jit.code);
#endif

#ifdef DX_ARM
#ifdef JIT_DEBUG
   jit_exec((sU8*)(st->jit.code), _env->context, _env, (void*)st->vm_vars, (void*)st->vm_const);
#else
   ((void (YAC_CALL*)(TKS_Context *, PTN_Env *, void*, void*))st->jit.code)
      (_env->context, _env,(void*)st->vm_vars, (void*)st->vm_const);
#endif // JIT_DEBUG

#else
// X86
#ifdef JIT_DEBUG
   jit_exec((sU8*)(st->jit.code), _env->context, _env);
#else
   ((void (YAC_CALL*)(TKS_Context *, PTN_Env *))st->jit.code)(_env->context, _env);
#endif // JIT_DEBUG
#endif // DX_ARM

   ////// The VMCore::mutex has been unlocked in the JIT prologue!
}
 
Fevalst VMCore::getEvalSt(void) const {
   return VMCore__eval;  
} 
 
void VMCore::eval(PTN_Env *_env) const {  
   VMCore__eval(_env, this);  
} 
 
void VMCore::ScanCPUTable(void) { 
#ifdef SAVE_CPUTABLE
	// ---- generate new CPU table and save to "tkscpu.tbl" ----
	sU16 i;
	//VMCore_CPUTable();
	// the following two lines hack into the function compiled/assembled
	// with visualC++ (6.0). the CPU table will be extracted and saved
	// to disk so it can be used for other x86 builds of TKS.
#ifdef DX_X86
	sU8 *i86 = (sU8*)&VMCore_CPUTable;
	i86+=(*(sU32*)(i86+1))+5;  // +sizeof jmp opcode (5) !!
#endif

#ifdef DX_ARM
   sU8 *i86 = (sU8*) &ocx_prologue;
#endif

	sU8 *c=i86;
	vm_i86=c;

	// find first delimiter 
	for(i=0; i<200; i++)
	{ 
#ifdef DX_ODDADDR 
		sU32 *c32=(sU32*)(c+i);
		if(*c32==Dopsep)
		{
			if(c32[1]==Dopsep2)
			{
				c=(c+i);
				break;
			}
		} 
#else 
		// big endian, written for 68K 
		if(c[i]==0xC7) 
			if(c[i+1]==0x05) 
				if(c[i+2]==0xFF) 
					if(c[i+3]==0xFF) 
						if(c[i+4]==0xFF) 
							if(c[i+5]==0xFF) 
								if(c[i+6]==xCC) 
									if(c[i+7]==0xCC) 
									{ 
										c=c+i; 
										break; 
									} 
#endif // DX_ODDADDR
	}

	// codeoffset (fun init code size ) = c-i86
	vm_sz_startup = (sU16) (c - i86);
	c += Dsizepad; 
	vm_i86first = c;
	sU8 *cfirst = c;

	// first opcode i86 code = c
	for(i=0; i<VM_NUMOPCODES; i++)
	{ 
		vm86_off[i]=(sUI)(c-cfirst);
		sU8 *cstart=c;
		sBool bnf=1;
		while(bnf)
		{
			// search end of current opcode 
#ifdef DX_ODDADDR
			while(*((sU32*)c)!=Dopsep) c++; 
			if( ((sU32*)c)[1]==Dopsep2) 
#else 
			sBool bf=1; 
			do { 
				bf=(c[0]!=0xC7)||(c[1]!=0x05)||(c[2]!=0xFF)|| 
				   (c[3]==0xFF); 
				if(bf) 
					c++; 
			} while(bf); 
			if(c[4]==0xFF) 
				if(c[5]==0xFF) 
					if(c[6]==xCC) 
						if(c[7]==0xCC) 
#endif // DX_ODDADDR
			{
				vm86_sz[i]=(c-cstart);
				c+=Dsizepad; 
				bnf=0;
			}
			else c++;
		}
	}

	c=c;

#ifdef DX_X86
	vm_sz_cleanup=7; // xxx hardcoded
	// pop edi
	// pop esi
	// pop ebx
	// mov esp, ebp
	// pop ebp
	// ret
	// 5F,5E,5B,8B,E5,5D,C3
#endif

#ifdef DX_68K
	vm_sz_cleanup=6; // xxx hardcoded MC680x0
	// 4CDF7CFC		movem.l (sp)+,d2-d7/a2-a6 
	// 4E75			rts 
#endif

#ifdef DX_ARM
   vm_sz_cleanup = 8; // xxx hardcoded ARM
   // add sp, r6, #256
   // pop {r3-r11, pc}
#endif // DX_ARM

   {
      FILE *fh = fopen(
#ifdef DX_X86
         "tkscpu_x86.tbl"
#elif defined(DX_ARM)
         "tkscpu_arm.tbl"
#endif
         , "wb");
	   printf("cpu table size=%i bytes.\n", (int)(c-vm_i86));
	   fwrite((const void*)vm_i86, (int) (c-vm_i86), 1, fh);
      fclose(fh);
   }
#else








	// ---- use "portable" assembled CPU table (converted to "C" array) ----
	sU16 i; 


#ifdef DX_X86
	sU8 *i86 = ((sU8*)tks_cpu_tbl_x86) + 4; // skip size
#endif

#ifdef DX_68K
	sU8 *i86 = ((sU8*)tks_cpu_tbl_68k) + 4; // skip size
#endif

#ifdef DX_ARM
	sU8 *i86 = ((sU8*)tks_cpu_tbl_arm) + 4; // skip size
#endif

	vm_i86=i86; //((sU8*)tks_cpu_tbl)+4; 
	sU8 *c=i86;
 
	// find first delimiter
	for(i=0; i<200; i++)
	{
#ifdef DX_ODDADDR 
		sU32 *c32=(sU32*)(c+i); 
		if(*c32==Dopsep) 
		{ 
			if(c32[1]==Dopsep2) 
			{ 
				c=(c+i); 
				break; 
			} 
		} 
#else 
		// big endian, written for 68K 
		if(c[i]==0xC7) 
			if(c[i+1]==0x05) 
				if(c[i+2]==0xFF) 
					if(c[i+3]==0xFF) 
						if(c[i+4]==0xFF) 
							if(c[i+5]==0xFF) 
								if(c[i+6]==0xCC) 
									if(c[i+7]==0xCC) 
									{ 
										c=c+i; 
										break; 
									} 
#endif // DX_ODDADDR
	} 

	// codeoffset (fun init code size ) = c-i86
	vm_sz_startup = (sU16)(c - i86);
	c += Dsizepad; 
	vm_i86first = c;
	sU8 *cfirst = c;
	// first opcode i86 code = c
	
	for(i=0; i<VM_NUMOPCODES; i++)
	{
      ////::printf("search opcode %i\n", i);
		vm86_off[i]=(sU32)(c-cfirst);
		sU8 *cstart = c;
		sBool bnf = 1;

		while(bnf)
		{
			// search end of current opcode
#ifdef DX_ODDADDR
			while(*((sU32*)c)!=Dopsep) c++;
			if( ((sU32*)c)[1]==Dopsep2)
#else
			sBool bf=1;
			c--;
			do { 
			  c++;
			  bf=(c[0]!=0xC7)||(c[1]!=0x05)||(c[2]!=0xFF)||(c[3]!=0xFF);
			} while(bf);
			if(c[4]==0xFF)
				if(c[5]==0xFF)
					if(c[6]==0xCC)
						if(c[7]==0xCC)
#endif
			{
			    //::printf("vm86_sz[%i]=%i\n",i, (sUI)(c-cstart));
				vm86_sz[i]=(sU16)(c-cstart);
				c+=Dsizepad;
				bnf=0;
			}
			else
			  c+=4;
		}
	}

	c = c;

//#ifdef TKS_DCON
//	if(tkscript->configuration.debug_level>1)
//	    ::printf("loaded cpu table. size=%i bytes.\n", (int)(c-vm_i86));
//#endif

#ifdef DX_X86
	vm_sz_cleanup=7; // xxx hardcoded x86
	// pop edi 
	// pop esi 
	// pop ebx 
	// mov esp, ebp 
	// pop ebp 
	// ret 
	// 5F,5E,5B,8B,E5,5D,C3 
#endif 

#ifdef DX_68K 
	vm_sz_cleanup=6; // xxx hardcoded MC680x0
	// 4CDF7CFC		movem.l (sp)+,d2-d7/a2-a6 
	// 4E75			rts 
#endif

#ifdef DX_ARM
   vm_sz_cleanup = 8; // xxx hardcoded ARM
   // add sp, r6, #256
   // pop {r3-r11, pc}  // r3 is saved/restored to maintain 8 byte stack alignment
#endif // DX_ARM


#endif // SAVE_CPUTABLE
	
}

sUI VMCore::estimateJITSize(void) {
	sUI r = 0;
	sU16 *c = vm_code;
	sU16 *ce = vm_code+vm_pc;

	while(c < ce)
	{
		r += vm86_sz[*c];
		c += vm_opcodesz[*c];
	}

	return r;
}

sBool VMCore::semanticCheck(void) {
   return
      (parser_tree ? parser_tree->semanticCheckAll() : 1)
      ;
}

sBool VMCore::resolveXRef(void) {
   return
      (parser_tree ? parser_tree->resolveXRefAll() : 1)
      ;
}

void VMCore::optimize(void) {
   if(NULL != parser_tree)  
   {  
      parser_tree->optimizeAll();

      if(!compileThis()) 
      { 
         Dprintf("[---] VMCore::compile() failed.\n"); 
         cleanJIT(); 
         return;
      } 
   }  
}

void VMCore::subGenCallList(void) {
	if(NULL != parser_tree)
   {
		parser_tree->genCallList();
   }
}

sBool VMCore::compileThis(void) {
	if(NULL != parser_tree)
	{
		// ---- set if this is a compiled tks node ---- 
		c_array = NULL;

		if(255 == parser_tree->compileHybridStatement(this)) 
      {
         return YAC_FALSE;
      }
	}

	addOpcode(VMOP_HALT);

	finishCompile();

	return createJITCode();
}

sBool VMCore::createJITCode(void) {
	sUI i;
	sU16 *c;
	sU16 *ce;
 
	if(tkscript->configuration.b_jitopt) 
	{
	// -------- post compile code analysis and optimization --------
	sU16 *reloctbl=new sU16[(vm_pc+1)*2];
	for(i=0; i<((vm_pc+1)*2); i++) reloctbl[i]=0xFFFF;
	sU16 reloci=0;
	c=vm_code;
	i=0; 
	while(i<vm_pc)
	{
		sU16 top; 
		// ---- 2004/02
		if( 
			// ---- check for pushv/popv seq ---- 
			(c[i]==VMOP_PUSHV)&& 
			(c[i+2]==VMOP_POPV) 
			) 
		{ 
			c[i]=VMOP_MOVV; 
			top=c[i+1]; 
			c[i+1]=c[i+3]; 
			c[i+2]=top; 
			c[i+3]=0xFFFF; // virtual nop 
			i+=4; 
			reloctbl[reloci++]=i; 
			reloctbl[reloci++]=1; 
		} 
		else 
		if( 
			// ---- check for pushc/popv seq ---- 
			(c[i]==VMOP_PUSHC)&& 
			(c[i+2]==VMOP_POPV) 
			) 
		{ 
			c[i]=VMOP_MOVVC; 
			top=c[i+1]; 
			c[i+1]=c[i+3]; 
			c[i+2]=top; 
			c[i+3]=0xFFFF; // virtual nop 
			i+=4; 
			reloctbl[reloci++]=i; 
			reloctbl[reloci++]=1; 
		} 
		else 
		if( 
			// ---- check for pushv/pushv/siand/popv seq---- 
			(c[i]==VMOP_PUSHV)&& 
			(c[i+2]==VMOP_PUSHV)&& 
			(c[i+4]==VMOP_SIAND)&& 
			(c[i+5]==VMOP_POPV)&& 
			( (c[i+6]==c[i+1])||(c[i+6]==c[i+3]) ) // pop var that got pushed? 
			) 
		{ 
			c[i]=VMOP_IVANDIV; 
			sU16 dst=c[i+6]; 
			sU16 src=(c[i+1]==dst)?c[i+3]:c[i+1]; // a+b == b+a, check order 
			c[i+1]=dst; 
			c[i+2]=src; 
			c[i+3]=0xFFFF; // virtual nop 
			c[i+4]=0xFFFF; // virtual nop 
			c[i+5]=0xFFFF; // virtual nop 
			c[i+6]=0xFFFF; // virtual nop 
			i+=7; 
			reloctbl[reloci++]=i; 
			reloctbl[reloci++]=4; 
		} 
		else 
		if( 
			// ---- check for pushv/pushv/sior/popv seq---- 
			(c[i]==VMOP_PUSHV)&& 
			(c[i+2]==VMOP_PUSHV)&& 
			(c[i+4]==VMOP_SIOR)&& 
			(c[i+5]==VMOP_POPV)&& 
			( (c[i+6]==c[i+1])||(c[i+6]==c[i+3]) ) // pop var that got pushed? 
			) 
		{ 
			c[i]=VMOP_IVORIV; 
			sU16 dst=c[i+6]; 
			sU16 src=(c[i+1]==dst)?c[i+3]:c[i+1]; // a+b == b+a, check order 
			c[i+1]=dst; 
			c[i+2]=src; 
			c[i+3]=0xFFFF; // virtual nop 
			c[i+4]=0xFFFF; // virtual nop 
			c[i+5]=0xFFFF; // virtual nop 
			c[i+6]=0xFFFF; // virtual nop 
			i+=7; 
			reloctbl[reloci++]=i; 
			reloctbl[reloci++]=4; 
		} 
		else 
		if( 
			// ---- check for pushv/pushv/sieor/popv seq---- 
			(c[i]==VMOP_PUSHV)&& 
			(c[i+2]==VMOP_PUSHV)&& 
			(c[i+4]==VMOP_SIEOR)&& 
			(c[i+5]==VMOP_POPV)&& 
			( (c[i+6]==c[i+1])||(c[i+6]==c[i+3]) ) // pop var that got pushed? 
			) 
		{ 
			c[i]=VMOP_IVEORIV; 
			sU16 dst=c[i+6]; 
			sU16 src=(c[i+1]==dst)?c[i+3]:c[i+1]; // a+b == b+a, check order 
			c[i+1]=dst; 
			c[i+2]=src; 
			c[i+3]=0xFFFF; // virtual nop 
			c[i+4]=0xFFFF; // virtual nop 
			c[i+5]=0xFFFF; // virtual nop 
			c[i+6]=0xFFFF; // virtual nop 
			i+=7; 
			reloctbl[reloci++]=i; 
			reloctbl[reloci++]=4; 
		} 
		else 
		if( 
			// ---- check for pushv/pushv/simod/popv seq---- 
			(c[i]==VMOP_PUSHV)&& 
			(c[i+2]==VMOP_PUSHV)&& 
			(c[i+4]==VMOP_SIMOD)&& 
			(c[i+5]==VMOP_POPV)&& 
			(c[i+6]==c[i+1]) // pop var that got pushed? 
			) 
		{ 
			c[i]=VMOP_IVMODIV; 
			c[i+2]=c[i+3]; 
			c[i+3]=0xFFFF; // virtual nop 
			c[i+4]=0xFFFF; // virtual nop 
			c[i+5]=0xFFFF; // virtual nop 
			c[i+6]=0xFFFF; // virtual nop 
			i+=7; 
			reloctbl[reloci++]=i; 
			reloctbl[reloci++]=4; 
		} 
		else 
		if( 
			// ---- check for pushv/pushv/siadd/popv seq---- 
			(c[i]==VMOP_PUSHV)&& 
			(c[i+2]==VMOP_PUSHV)&& 
			(c[i+4]==VMOP_SIADD)&& 
			(c[i+5]==VMOP_POPV)&& 
			( (c[i+6]==c[i+1])||(c[i+6]==c[i+3]) ) // pop var that got pushed? 
			) 
		{ 
			c[i]=VMOP_IVADDIV; 
			sU16 dst=c[i+6]; 
			sU16 src=(c[i+1]==dst)?c[i+3]:c[i+1]; // a+b == b+a, check order 
			c[i+1]=dst; 
			c[i+2]=src; 
			c[i+3]=0xFFFF; // virtual nop 
			c[i+4]=0xFFFF; // virtual nop 
			c[i+5]=0xFFFF; // virtual nop 
			c[i+6]=0xFFFF; // virtual nop 
			i+=7; 
			reloctbl[reloci++]=i; 
			reloctbl[reloci++]=4; 
		} 
		else 
		if( 
			// ---- check for pushv/pushv/sisub/popv seq---- 
			(c[i]==VMOP_PUSHV)&& 
			(c[i+2]==VMOP_PUSHV)&& 
			(c[i+4]==VMOP_SISUB)&& 
			(c[i+5]==VMOP_POPV)&& 
			(c[i+6]==c[i+1]) // pop var that got pushed? 
			) 
		{ 
			c[i]=VMOP_IVSUBIV; 
			c[i+2]=c[i+3]; 
			c[i+3]=0xFFFF; // virtual nop 
			c[i+4]=0xFFFF; // virtual nop 
			c[i+5]=0xFFFF; // virtual nop 
			c[i+6]=0xFFFF; // virtual nop 
			i+=7; 
			reloctbl[reloci++]=i; 
			reloctbl[reloci++]=4; 
		} 
		else 
		if( 
			// ---- check for pushv/pushv/simul/popv seq---- 
			(c[i]==VMOP_PUSHV)&& 
			(c[i+2]==VMOP_PUSHV)&& 
			(c[i+4]==VMOP_SIMUL)&& 
			(c[i+5]==VMOP_POPV)&& 
			( (c[i+6]==c[i+1])||(c[i+6]==c[i+3]) ) // pop var that got pushed? 
			) 
		{ 
			c[i]=VMOP_IVMULIV; 
			sU16 dst=c[i+6]; 
			sU16 src=(c[i+1]==dst)?c[i+3]:c[i+1]; // a+b == b+a, check order 
			c[i+1]=dst; 
			c[i+2]=src; 
			c[i+3]=0xFFFF; // virtual nop 
			c[i+4]=0xFFFF; // virtual nop 
			c[i+5]=0xFFFF; // virtual nop 
			c[i+6]=0xFFFF; // virtual nop 
			i+=7; 
			reloctbl[reloci++]=i; 
			reloctbl[reloci++]=4; 
		} 
		else 
		if( 
			// ---- check for pushv/pushv/sidiv/popv seq---- 
			(c[i]==VMOP_PUSHV)&& 
			(c[i+2]==VMOP_PUSHV)&& 
			(c[i+4]==VMOP_SIDIV)&& 
			(c[i+5]==VMOP_POPV)&& 
			(c[i+6]==c[i+1]) // pop var that got pushed? 
			) 
		{ 
			c[i]=VMOP_IVDIVIV; 
			c[i+2]=c[i+3]; 
			c[i+3]=0xFFFF; // virtual nop 
			c[i+4]=0xFFFF; // virtual nop 
			c[i+5]=0xFFFF; // virtual nop 
			c[i+6]=0xFFFF; // virtual nop 
			i+=7; 
			reloctbl[reloci++]=i; 
			reloctbl[reloci++]=4; 
		} 
		else 
		if( 
			// ---- check for pushv/pushv/siasl/popv seq---- 
			(c[i]==VMOP_PUSHV)&& 
			(c[i+2]==VMOP_PUSHV)&& 
			(c[i+4]==VMOP_SIASL)&& 
			(c[i+5]==VMOP_POPV)&& 
			(c[i+6]==c[i+1]) // pop var that got pushed? 
			) 
		{ 
			c[i]=VMOP_IVASLIV; 
			c[i+2]=c[i+3]; 
			c[i+3]=0xFFFF; // virtual nop 
			c[i+4]=0xFFFF; // virtual nop 
			c[i+5]=0xFFFF; // virtual nop 
			c[i+6]=0xFFFF; // virtual nop 
			i+=7; 
			reloctbl[reloci++]=i; 
			reloctbl[reloci++]=4; 
		} 
		else 
		if( 
			// ---- check for pushv/pushv/siasr/popv seq---- 
			(c[i]==VMOP_PUSHV)&& 
			(c[i+2]==VMOP_PUSHV)&& 
			(c[i+4]==VMOP_SIASR)&& 
			(c[i+5]==VMOP_POPV)&& 
			(c[i+6]==c[i+1]) // pop var that got pushed? 
			) 
		{ 
			c[i]=VMOP_IVASRIV; 
			c[i+2]=c[i+3]; 
			c[i+3]=0xFFFF; // virtual nop 
			c[i+4]=0xFFFF; // virtual nop 
			c[i+5]=0xFFFF; // virtual nop 
			c[i+6]=0xFFFF; // virtual nop 
			i+=7; 
			reloctbl[reloci++]=i; 
			reloctbl[reloci++]=4; 
		} 
		else 
		if( 
			// ---- check for pushv/pushv/sfadd/popv seq---- 
			(c[i]==VMOP_PUSHV)&& 
			(c[i+2]==VMOP_PUSHV)&& 
			(c[i+4]==VMOP_SFADD)&& 
			(c[i+5]==VMOP_POPV)&& 
			( (c[i+6]==c[i+1])||(c[i+6]==c[i+3]) ) // pop var that got pushed? 
			) 
		{ 
			c[i]=VMOP_FVADDFV; 
			sU16 dst=c[i+6]; 
			sU16 src=(c[i+1]==dst)?c[i+3]:c[i+1]; // a+b == b+a, check order 
			c[i+1]=dst; 
			c[i+2]=src; 
			c[i+3]=0xFFFF; // virtual nop 
			c[i+4]=0xFFFF; // virtual nop 
			c[i+5]=0xFFFF; // virtual nop 
			c[i+6]=0xFFFF; // virtual nop 
			i+=7; 
			reloctbl[reloci++]=i; 
			reloctbl[reloci++]=4; 
		} 
		else 
		if( 
			// ---- check for pushv/pushv/sfsub/popv seq---- 
			(c[i]==VMOP_PUSHV)&& 
			(c[i+2]==VMOP_PUSHV)&& 
			(c[i+4]==VMOP_SFSUB)&& 
			(c[i+5]==VMOP_POPV)&& 
			(c[i+6]==c[i+1]) // pop var that got pushed? 
			) 
		{ 
			c[i]=VMOP_FVSUBFV; 
			c[i+2]=c[i+3]; 
			c[i+3]=0xFFFF; // virtual nop 
			c[i+4]=0xFFFF; // virtual nop 
			c[i+5]=0xFFFF; // virtual nop 
			c[i+6]=0xFFFF; // virtual nop 
			i+=7; 
			reloctbl[reloci++]=i; 
			reloctbl[reloci++]=4; 
		} 
		else 
		if( 
			// ---- check for pushv/pushv/sfmul/popv seq---- 
			(c[i]==VMOP_PUSHV)&& 
			(c[i+2]==VMOP_PUSHV)&& 
			(c[i+4]==VMOP_SFMUL)&& 
			(c[i+5]==VMOP_POPV)&& 
			( (c[i+6]==c[i+1])||(c[i+6]==c[i+3]) ) // pop var that got pushed? 
			) 
		{ 
			c[i]=VMOP_FVMULFV; 
			sU16 dst=c[i+6]; 
			sU16 src=(c[i+1]==dst)?c[i+3]:c[i+1]; // a+b == b+a, check order 
			c[i+1]=dst; 
			c[i+2]=src; 
			c[i+3]=0xFFFF; // virtual nop 
			c[i+4]=0xFFFF; // virtual nop 
			c[i+5]=0xFFFF; // virtual nop 
			c[i+6]=0xFFFF; // virtual nop 
			i+=7; 
			reloctbl[reloci++]=i; 
			reloctbl[reloci++]=4; 
		} 
		else 
		if( 
			// ---- check for pushv/pushv/sfdiv/popv seq---- 
			(c[i]==VMOP_PUSHV)&& 
			(c[i+2]==VMOP_PUSHV)&& 
			(c[i+4]==VMOP_SFDIV)&& 
			(c[i+5]==VMOP_POPV)&& 
			(c[i+6]==c[i+1]) // pop var that got pushed? 
			) 
		{ 
			c[i]=VMOP_FVDIVFV; 
			c[i+2]=c[i+3]; 
			c[i+3]=0xFFFF; // virtual nop 
			c[i+4]=0xFFFF; // virtual nop 
			c[i+5]=0xFFFF; // virtual nop 
			c[i+6]=0xFFFF; // virtual nop 
			i+=7; 
			reloctbl[reloci++]=i; 
			reloctbl[reloci++]=4; 
		} 
		else 
		if( 
			// ---- check for pushv/pushc/siand seq ---- 
			(c[i]==VMOP_PUSHV)&& 
			(c[i+2]==VMOP_PUSHC)&& 
			(c[i+4]==VMOP_SIAND) 
			) 
		{ 
			c[i]=VMOP_PUSHIVANDC; 
			c[i+2]=c[i+3]; 
			c[i+3]=0xFFFF; // virtual nop 
			c[i+4]=0xFFFF; // virtual nop 
			i+=5; 
			reloctbl[reloci++]=i; 
			reloctbl[reloci++]=2; 
		} 
		else 
		if( 
			// ---- check for pushc/pushv/siand seq ---- 
			(c[i]==VMOP_PUSHC)&& 
			(c[i+2]==VMOP_PUSHV)&& 
			(c[i+4]==VMOP_SIAND) 
			) 
		{ 
			c[i]=VMOP_PUSHIVANDC; 
			sU16 t=c[i+1]; 
			c[i+1]=c[i+3]; 
			c[i+2]=t; 
			c[i+3]=0xFFFF; // virtual nop 
			c[i+4]=0xFFFF; // virtual nop 
			i+=5; 
			reloctbl[reloci++]=i; 
			reloctbl[reloci++]=2; 
		} 
		else 
		if( 
			// ---- check for pushv/pushc/sior seq ---- 
			(c[i]==VMOP_PUSHV)&& 
			(c[i+2]==VMOP_PUSHC)&& 
			(c[i+4]==VMOP_SIOR) 
			) 
		{ 
			c[i]=VMOP_PUSHIVORC; 
			c[i+2]=c[i+3]; 
			c[i+3]=0xFFFF; // virtual nop 
			c[i+4]=0xFFFF; // virtual nop 
			i+=5; 
			reloctbl[reloci++]=i; 
			reloctbl[reloci++]=2; 
		} 
		else 
		if( 
			// ---- check for pushc/pushv/sior seq ---- 
			(c[i]==VMOP_PUSHC)&& 
			(c[i+2]==VMOP_PUSHV)&& 
			(c[i+4]==VMOP_SIOR) 
			) 
		{ 
			c[i]=VMOP_PUSHIVORC; 
			sU16 t=c[i+1]; 
			c[i+1]=c[i+3]; 
			c[i+2]=t; 
			c[i+3]=0xFFFF; // virtual nop 
			c[i+4]=0xFFFF; // virtual nop 
			i+=5; 
			reloctbl[reloci++]=i; 
			reloctbl[reloci++]=2; 
		} 
		else 
		if( 
			// ---- check for pushv/pushc/sieor seq ---- 
			(c[i]==VMOP_PUSHV)&& 
			(c[i+2]==VMOP_PUSHC)&& 
			(c[i+4]==VMOP_SIEOR) 
			) 
		{ 
			c[i]=VMOP_PUSHIVEORC; 
			c[i+2]=c[i+3]; 
			c[i+3]=0xFFFF; // virtual nop 
			c[i+4]=0xFFFF; // virtual nop 
			i+=5; 
			reloctbl[reloci++]=i; 
			reloctbl[reloci++]=2; 
		} 
		else 
		if( 
			// ---- check for pushc/pushv/sieor seq ---- 
			(c[i]==VMOP_PUSHC)&& 
			(c[i+2]==VMOP_PUSHV)&& 
			(c[i+4]==VMOP_SIEOR) 
			) 
		{ 
			c[i]=VMOP_PUSHIVEORC; 
			sU16 t=c[i+1]; 
			c[i+1]=c[i+3]; 
			c[i+2]=t; 
			c[i+3]=0xFFFF; // virtual nop 
			c[i+4]=0xFFFF; // virtual nop 
			i+=5; 
			reloctbl[reloci++]=i; 
			reloctbl[reloci++]=2; 
		} 
		else 
		if( 
			// ---- check for pushv/pushc/simod seq ---- 
			(c[i]==VMOP_PUSHV)&& 
			(c[i+2]==VMOP_PUSHC)&& 
			(c[i+4]==VMOP_SIMOD) 
			) 
		{ 
			c[i]=VMOP_PUSHIVMODC; 
			c[i+2]=c[i+3]; 
			c[i+3]=0xFFFF; // virtual nop 
			c[i+4]=0xFFFF; // virtual nop 
			i+=5; 
			reloctbl[reloci++]=i; 
			reloctbl[reloci++]=2; 
		} 
		else 
		if( 
			// ---- check for pushv/pushc/siadd seq ---- 
			(c[i]==VMOP_PUSHV)&& 
			(c[i+2]==VMOP_PUSHC)&& 
			(c[i+4]==VMOP_SIADD) 
			) 
		{ 
			c[i]=VMOP_PUSHIVADDC; 
			c[i+2]=c[i+3]; 
			c[i+3]=0xFFFF; // virtual nop 
			c[i+4]=0xFFFF; // virtual nop 
			i+=5; 
			reloctbl[reloci++]=i; 
			reloctbl[reloci++]=2; 
		} 
		else 
		if( 
			// ---- check for pushc/pushv/siadd seq ---- 
			(c[i]==VMOP_PUSHC)&& 
			(c[i+2]==VMOP_PUSHV)&& 
			(c[i+4]==VMOP_SIADD) 
			) 
		{ 
			c[i]=VMOP_PUSHIVADDC; 
			sU16 t=c[i+1]; 
			c[i+1]=c[i+3]; 
			c[i+2]=t; 
			c[i+3]=0xFFFF; // virtual nop 
			c[i+4]=0xFFFF; // virtual nop 
			i+=5; 
			reloctbl[reloci++]=i; 
			reloctbl[reloci++]=2; 
		} 
		else 
		if( 
			// ---- check for pushv/pushc/sisub seq ---- 
			(c[i]==VMOP_PUSHV)&& 
			(c[i+2]==VMOP_PUSHC)&& 
			(c[i+4]==VMOP_SISUB) 
			) 
		{ 
			c[i]=VMOP_PUSHIVSUBC; 
			c[i+2]=c[i+3]; 
			c[i+3]=0xFFFF; // virtual nop 
			c[i+4]=0xFFFF; // virtual nop 
			i+=5; 
			reloctbl[reloci++]=i; 
			reloctbl[reloci++]=2; 
		} 
		else 
		if( 
			// ---- check for pushv/pushc/simul seq ---- 
			(c[i]==VMOP_PUSHV)&& 
			(c[i+2]==VMOP_PUSHC)&& 
			(c[i+4]==VMOP_SIMUL) 
			) 
		{ 
			c[i]=VMOP_PUSHIVMULC; 
			c[i+2]=c[i+3]; 
			c[i+3]=0xFFFF; // virtual nop 
			c[i+4]=0xFFFF; // virtual nop 
			i+=5; 
			reloctbl[reloci++]=i; 
			reloctbl[reloci++]=2; 
		} 
		else 
		if( 
			// ---- check for pushc/pushv/simul seq ---- 
			(c[i]==VMOP_PUSHC)&& 
			(c[i+2]==VMOP_PUSHV)&& 
			(c[i+4]==VMOP_SIMUL) 
			) 
		{ 
			c[i]=VMOP_PUSHIVMULC; 
			sU16 t=c[i+1]; 
			c[i+1]=c[i+3]; 
			c[i+2]=t; 
			c[i+3]=0xFFFF; // virtual nop 
			c[i+4]=0xFFFF; // virtual nop 
			i+=5; 
			reloctbl[reloci++]=i; 
			reloctbl[reloci++]=2; 
		} 
		else 
		if( 
			// ---- check for pushv/pushc/sidiv seq ---- 
			(c[i]==VMOP_PUSHV)&& 
			(c[i+2]==VMOP_PUSHC)&& 
			(c[i+4]==VMOP_SIDIV) 
			) 
		{ 
			c[i]=VMOP_PUSHIVDIVC; 
			c[i+2]=c[i+3]; 
			c[i+3]=0xFFFF; // virtual nop 
			c[i+4]=0xFFFF; // virtual nop 
			i+=5; 
			reloctbl[reloci++]=i; 
			reloctbl[reloci++]=2; 
		} 
		else 
		if( 
			// ---- check for pushv/pushc/siasl seq ---- 
			(c[i]==VMOP_PUSHV)&& 
			(c[i+2]==VMOP_PUSHC)&& 
			(c[i+4]==VMOP_SIASL) 
			) 
		{ 
			c[i]=VMOP_PUSHIVASLC; 
			c[i+2]=c[i+3]; 
			c[i+3]=0xFFFF; // virtual nop 
			c[i+4]=0xFFFF; // virtual nop 
			i+=5; 
			reloctbl[reloci++]=i; 
			reloctbl[reloci++]=2; 
		} 
		else 
		if( 
			// ---- check for pushv/pushc/siasr seq ---- 
			(c[i]==VMOP_PUSHV)&& 
			(c[i+2]==VMOP_PUSHC)&& 
			(c[i+4]==VMOP_SIASR) 
			) 
		{ 
			c[i]=VMOP_PUSHIVASRC; 
			c[i+2]=c[i+3]; 
			c[i+3]=0xFFFF; // virtual nop 
			c[i+4]=0xFFFF; // virtual nop 
			i+=5; 
			reloctbl[reloci++]=i; 
			reloctbl[reloci++]=2; 
		} 
		else 
		if( 
			// ---- check for pushv/pushc/sfadd seq ---- 
			(c[i]==VMOP_PUSHV)&& 
			(c[i+2]==VMOP_PUSHC)&& 
			(c[i+4]==VMOP_SFADD) 
			) 
		{ 
			c[i]=VMOP_PUSHFVADDC; 
			c[i+2]=c[i+3]; 
			c[i+3]=0xFFFF; // virtual nop 
			c[i+4]=0xFFFF; // virtual nop 
			i+=5; 
			reloctbl[reloci++]=i; 
			reloctbl[reloci++]=2; 
		} 
		else 
		if( 
			// ---- check for pushc/pushv/sfadd seq ---- 
			(c[i]==VMOP_PUSHC)&& 
			(c[i+2]==VMOP_PUSHV)&& 
			(c[i+4]==VMOP_SFADD) 
			) 
		{ 
			c[i]=VMOP_PUSHFVADDC; 
			sU16 t=c[i+1]; 
			c[i+1]=c[i+3]; 
			c[i+2]=t; 
			c[i+3]=0xFFFF; // virtual nop 
			c[i+4]=0xFFFF; // virtual nop 
			i+=5; 
			reloctbl[reloci++]=i; 
			reloctbl[reloci++]=2; 
		} 
		else 
		if( 
			// ---- check for pushv/pushc/sfsub seq ---- 
			(c[i]==VMOP_PUSHV)&& 
			(c[i+2]==VMOP_PUSHC)&& 
			(c[i+4]==VMOP_SFSUB) 
			) 
		{ 
			c[i]=VMOP_PUSHFVSUBC; 
			c[i+2]=c[i+3]; 
			c[i+3]=0xFFFF; // virtual nop 
			c[i+4]=0xFFFF; // virtual nop 
			i+=5; 
			reloctbl[reloci++]=i; 
			reloctbl[reloci++]=2; 
		} 
		else 
		if( 
			// ---- check for pushlv/pushc/sfmul seq ---- 
			(c[i]==VMOP_PUSHV)&& 
			(c[i+2]==VMOP_PUSHC)&& 
			(c[i+4]==VMOP_SFMUL) 
			) 
		{ 
			c[i]=VMOP_PUSHFVMULC; 
			c[i+2]=c[i+3]; 
			c[i+3]=0xFFFF; // virtual nop 
			c[i+4]=0xFFFF; // virtual nop 
			i+=5; 
			reloctbl[reloci++]=i; 
			reloctbl[reloci++]=2; 
		} 
		else 
		if( 
			// ---- check for pushc/pushv/sfmul seq ---- 
			(c[i]==VMOP_PUSHC)&& 
			(c[i+2]==VMOP_PUSHV)&& 
			(c[i+4]==VMOP_SFMUL) 
			) 
		{ 
			c[i]=VMOP_PUSHFVMULC; 
			sU16 t=c[i+1]; 
			c[i+1]=c[i+3]; 
			c[i+2]=t; 
			c[i+3]=0xFFFF; // virtual nop 
			c[i+4]=0xFFFF; // virtual nop 
			i+=5; 
			reloctbl[reloci++]=i; 
			reloctbl[reloci++]=2; 
		} 
		else 
		if( 
			// ---- check for pushv/pushc/sfdiv seq ---- 
			(c[i]==VMOP_PUSHV)&& 
			(c[i+2]==VMOP_PUSHC)&& 
			(c[i+4]==VMOP_SFDIV) 
			) 
		{ 
			c[i]=VMOP_PUSHFVDIVC; 
			c[i+2]=c[i+3]; 
			c[i+3]=0xFFFF; // virtual nop 
			c[i+4]=0xFFFF; // virtual nop 
			i+=5; 
			reloctbl[reloci++]=i; 
			reloctbl[reloci++]=2; 
		} 
		else 
		if( 
			// ---- check for pushv/pushv/siand seq---- 
			(c[i]==VMOP_PUSHV)&& 
			(c[i+2]==VMOP_PUSHV)&& 
			(c[i+4]==VMOP_SIAND) 
			) 
		{ 
			c[i]=VMOP_PUSHIVANDIV; 
			c[i+2]=c[i+3]; 
			c[i+3]=0xFFFF; // virtual nop 
			c[i+4]=0xFFFF; // virtual nop 
			i+=5; 
			reloctbl[reloci++]=i; 
			reloctbl[reloci++]=2; 
		} 
		else 
		if( 
			// ---- check for pushv/pushv/sior seq---- 
			(c[i]==VMOP_PUSHV)&& 
			(c[i+2]==VMOP_PUSHV)&& 
			(c[i+4]==VMOP_SIOR) 
			) 
		{ 
			c[i]=VMOP_PUSHIVORIV; 
			c[i+2]=c[i+3]; 
			c[i+3]=0xFFFF; // virtual nop 
			c[i+4]=0xFFFF; // virtual nop 
			i+=5; 
			reloctbl[reloci++]=i; 
			reloctbl[reloci++]=2; 
		} 
		else 
		if( 
			// ---- check for pushv/pushv/sieor seq---- 
			(c[i]==VMOP_PUSHV)&& 
			(c[i+2]==VMOP_PUSHV)&& 
			(c[i+4]==VMOP_SIEOR) 
			) 
		{ 
			c[i]=VMOP_PUSHIVEORIV; 
			c[i+2]=c[i+3]; 
			c[i+3]=0xFFFF; // virtual nop 
			c[i+4]=0xFFFF; // virtual nop 
			i+=5; 
			reloctbl[reloci++]=i; 
			reloctbl[reloci++]=2; 
		} 
		else 
		if( 
			// ---- check for pushv/pushv/simod seq---- 
			(c[i]==VMOP_PUSHV)&& 
			(c[i+2]==VMOP_PUSHV)&& 
			(c[i+4]==VMOP_SIMOD) 
			) 
		{ 
			c[i]=VMOP_PUSHIVMODIV; 
			c[i+2]=c[i+3]; 
			c[i+3]=0xFFFF; // virtual nop 
			c[i+4]=0xFFFF; // virtual nop 
			i+=5; 
			reloctbl[reloci++]=i; 
			reloctbl[reloci++]=2; 
		} 
		else 
		if( 
			// ---- check for pushv/pushv/siadd seq---- 
			(c[i]==VMOP_PUSHV)&& 
			(c[i+2]==VMOP_PUSHV)&& 
			(c[i+4]==VMOP_SIADD) 
			) 
		{ 
			c[i]=VMOP_PUSHIVADDIV; 
			c[i+2]=c[i+3]; 
			c[i+3]=0xFFFF; // virtual nop 
			c[i+4]=0xFFFF; // virtual nop 
			i+=5; 
			reloctbl[reloci++]=i; 
			reloctbl[reloci++]=2; 
		} 
		else 
		if( 
			// ---- check for pushv/pushv/sisub seq---- 
			(c[i]==VMOP_PUSHV)&& 
			(c[i+2]==VMOP_PUSHV)&& 
			(c[i+4]==VMOP_SISUB) 
			) 
		{ 
			c[i]=VMOP_PUSHIVSUBIV; 
			c[i+2]=c[i+3]; 
			c[i+3]=0xFFFF; // virtual nop 
			c[i+4]=0xFFFF; // virtual nop 
			i+=5; 
			reloctbl[reloci++]=i; 
			reloctbl[reloci++]=2; 
		} 
		else 
		if( 
			// ---- check for pushv/pushv/simul seq---- 
			(c[i]==VMOP_PUSHV)&& 
			(c[i+2]==VMOP_PUSHV)&& 
			(c[i+4]==VMOP_SIMUL) 
			) 
		{ 
			c[i]=VMOP_PUSHIVMULIV; 
			c[i+2]=c[i+3]; 
			c[i+3]=0xFFFF; // virtual nop 
			c[i+4]=0xFFFF; // virtual nop 
			i+=5; 
			reloctbl[reloci++]=i; 
			reloctbl[reloci++]=2; 
		} 
		else 
		if( 
			// ---- check for pushv/pushv/sidiv seq---- 
			(c[i]==VMOP_PUSHV)&& 
			(c[i+2]==VMOP_PUSHV)&& 
			(c[i+4]==VMOP_SIDIV) 
			) 
		{ 
			c[i]=VMOP_PUSHIVDIVIV; 
			c[i+2]=c[i+3]; 
			c[i+3]=0xFFFF; // virtual nop 
			c[i+4]=0xFFFF; // virtual nop 
			i+=5; 
			reloctbl[reloci++]=i; 
			reloctbl[reloci++]=2; 
		} 
		else 
		if( 
			// ---- check for pushv/pushv/siasl seq---- 
			(c[i]==VMOP_PUSHV)&& 
			(c[i+2]==VMOP_PUSHV)&& 
			(c[i+4]==VMOP_SIASL) 
			) 
		{ 
			c[i]=VMOP_PUSHIVASLIV; 
			c[i+2]=c[i+3]; 
			c[i+3]=0xFFFF; // virtual nop 
			c[i+4]=0xFFFF; // virtual nop 
			i+=5; 
			reloctbl[reloci++]=i; 
			reloctbl[reloci++]=2; 
		} 
		else 
		if( 
			// ---- check for pushv/pushv/siasr seq---- 
			(c[i]==VMOP_PUSHV)&& 
			(c[i+2]==VMOP_PUSHV)&& 
			(c[i+4]==VMOP_SIASR) 
			) 
		{ 
			c[i]=VMOP_PUSHIVASRIV; 
			c[i+2]=c[i+3]; 
			c[i+3]=0xFFFF; // virtual nop 
			c[i+4]=0xFFFF; // virtual nop 
			i+=5; 
			reloctbl[reloci++]=i; 
			reloctbl[reloci++]=2; 
		} 
		else 
		if( 
			// ---- check for pushv/pushv/sfadd seq---- 
			(c[i]==VMOP_PUSHV)&& 
			(c[i+2]==VMOP_PUSHV)&& 
			(c[i+4]==VMOP_SFADD) 
			) 
		{ 
			c[i]=VMOP_PUSHFVADDFV; 
			c[i+2]=c[i+3]; 
			c[i+3]=0xFFFF; // virtual nop 
			c[i+4]=0xFFFF; // virtual nop 
			i+=5; 
			reloctbl[reloci++]=i; 
			reloctbl[reloci++]=2; 
		} 
		else 
		if( 
			// ---- check for pushv/pushv/sfsub seq---- 
			(c[i]==VMOP_PUSHV)&& 
			(c[i+2]==VMOP_PUSHV)&& 
			(c[i+4]==VMOP_SFSUB) 
			) 
		{ 
			c[i]=VMOP_PUSHFVSUBFV; 
			c[i+2]=c[i+3]; 
			c[i+3]=0xFFFF; // virtual nop 
			c[i+4]=0xFFFF; // virtual nop 
			i+=5; 
			reloctbl[reloci++]=i; 
			reloctbl[reloci++]=2; 
		} 
		else 
		if( 
			// ---- check for pushv/pushv/sfmul seq---- 
			(c[i]==VMOP_PUSHV)&& 
			(c[i+2]==VMOP_PUSHV)&& 
			(c[i+4]==VMOP_SFMUL) 
			) 
		{ 
			c[i]=VMOP_PUSHFVMULFV; 
			c[i+2]=c[i+3]; 
			c[i+3]=0xFFFF; // virtual nop 
			c[i+4]=0xFFFF; // virtual nop 
			i+=5; 
			reloctbl[reloci++]=i; 
			reloctbl[reloci++]=2; 
		} 
		else 
		if( 
			// ---- check for pushlv/pushlv/sfdiv seq---- 
			(c[i]==VMOP_PUSHV)&& 
			(c[i+2]==VMOP_PUSHV)&& 
			(c[i+4]==VMOP_SFDIV) 
			) 
		{ 
			c[i]=VMOP_PUSHFVDIVFV; 
			c[i+2]=c[i+3]; 
			c[i+3]=0xFFFF; // virtual nop 
			c[i+4]=0xFFFF; // virtual nop 
			i+=5; 
			reloctbl[reloci++]=i; 
			reloctbl[reloci++]=2; 
		} 
 
        // ---- 2002/05 
		else 
		if(
			// ---- check for pushlv/poplv seq ----
			(c[i]==VMOP_PUSHLV)&&
			(c[i+2]==VMOP_POPLV)
			)
		{
			c[i]=VMOP_MOVLV;
			top=c[i+1];
			c[i+1]=c[i+3];
			c[i+2]=top;
			c[i+3]=0xFFFF; // virtual nop
			i+=4;
			reloctbl[reloci++]=i;
			reloctbl[reloci++]=1;
		}
		else
		if(
			// ---- check for pushc/poplv seq ----
			(c[i]==VMOP_PUSHC)&&
			(c[i+2]==VMOP_POPLV)
			)
		{
			c[i]=VMOP_MOVLVC;
			top=c[i+1];
			c[i+1]=c[i+3];
			c[i+2]=top;
			c[i+3]=0xFFFF; // virtual nop
			i+=4;
			reloctbl[reloci++]=i;
			reloctbl[reloci++]=1;
		}
		else
		if(
			// ---- check for pushlv/pushlv/siand/poplv seq----
			(c[i]==VMOP_PUSHLV)&&
			(c[i+2]==VMOP_PUSHLV)&&
			(c[i+4]==VMOP_SIAND)&&
			(c[i+5]==VMOP_POPLV)&&
			( (c[i+6]==c[i+1])||(c[i+6]==c[i+3]) ) // pop var that got pushed?
			)
		{
			c[i]=VMOP_LIVANDLIV;
			sU16 dst=c[i+6];
			sU16 src=(c[i+1]==dst)?c[i+3]:c[i+1]; // a+b == b+a, check order
			c[i+1]=dst;
			c[i+2]=src;
			c[i+3]=0xFFFF; // virtual nop
			c[i+4]=0xFFFF; // virtual nop
			c[i+5]=0xFFFF; // virtual nop
			c[i+6]=0xFFFF; // virtual nop
			i+=7;
			reloctbl[reloci++]=i;
			reloctbl[reloci++]=4;
		}
		else
		if(
			// ---- check for pushlv/pushlv/sior/poplv seq----
			(c[i]==VMOP_PUSHLV)&&
			(c[i+2]==VMOP_PUSHLV)&&
			(c[i+4]==VMOP_SIOR)&&
			(c[i+5]==VMOP_POPLV)&&
			( (c[i+6]==c[i+1])||(c[i+6]==c[i+3]) ) // pop var that got pushed?
			)
		{
			c[i]=VMOP_LIVORLIV;
			sU16 dst=c[i+6];
			sU16 src=(c[i+1]==dst)?c[i+3]:c[i+1]; // a+b == b+a, check order
			c[i+1]=dst;
			c[i+2]=src;
			c[i+3]=0xFFFF; // virtual nop
			c[i+4]=0xFFFF; // virtual nop
			c[i+5]=0xFFFF; // virtual nop
			c[i+6]=0xFFFF; // virtual nop
			i+=7;
			reloctbl[reloci++]=i;
			reloctbl[reloci++]=4;
		}
		else
		if(
			// ---- check for pushlv/pushlv/sieor/poplv seq----
			(c[i]==VMOP_PUSHLV)&&
			(c[i+2]==VMOP_PUSHLV)&&
			(c[i+4]==VMOP_SIEOR)&&
			(c[i+5]==VMOP_POPLV)&&
			( (c[i+6]==c[i+1])||(c[i+6]==c[i+3]) ) // pop var that got pushed?
			)
		{
			c[i]=VMOP_LIVEORLIV;
			sU16 dst=c[i+6];
			sU16 src=(c[i+1]==dst)?c[i+3]:c[i+1]; // a+b == b+a, check order
			c[i+1]=dst;
			c[i+2]=src;
			c[i+3]=0xFFFF; // virtual nop
			c[i+4]=0xFFFF; // virtual nop
			c[i+5]=0xFFFF; // virtual nop
			c[i+6]=0xFFFF; // virtual nop
			i+=7;
			reloctbl[reloci++]=i;
			reloctbl[reloci++]=4;
		}
		else
		if(
			// ---- check for pushlv/pushlv/simod/poplv seq----
			(c[i]==VMOP_PUSHLV)&&
			(c[i+2]==VMOP_PUSHLV)&&
			(c[i+4]==VMOP_SIMOD)&&
			(c[i+5]==VMOP_POPLV)&&
			(c[i+6]==c[i+1]) // pop var that got pushed?
			)
		{
			c[i]=VMOP_LIVMODLIV;
			c[i+2]=c[i+3];
			c[i+3]=0xFFFF; // virtual nop
			c[i+4]=0xFFFF; // virtual nop
			c[i+5]=0xFFFF; // virtual nop
			c[i+6]=0xFFFF; // virtual nop
			i+=7;
			reloctbl[reloci++]=i;
			reloctbl[reloci++]=4;
		}
		else
		if(
			// ---- check for pushlv/pushlv/siadd/poplv seq----
			(c[i]==VMOP_PUSHLV)&&
			(c[i+2]==VMOP_PUSHLV)&&
			(c[i+4]==VMOP_SIADD)&&
			(c[i+5]==VMOP_POPLV)&&
			( (c[i+6]==c[i+1])||(c[i+6]==c[i+3]) ) // pop var that got pushed?
			)
		{
			c[i]=VMOP_LIVADDLIV;
			sU16 dst=c[i+6];
			sU16 src=(c[i+1]==dst)?c[i+3]:c[i+1]; // a+b == b+a, check order
			c[i+1]=dst;
			c[i+2]=src;
			c[i+3]=0xFFFF; // virtual nop
			c[i+4]=0xFFFF; // virtual nop
			c[i+5]=0xFFFF; // virtual nop
			c[i+6]=0xFFFF; // virtual nop
			i+=7;
			reloctbl[reloci++]=i;
			reloctbl[reloci++]=4;
		}
		else
		if(
			// ---- check for pushlv/pushlv/sisub/poplv seq----
			(c[i]==VMOP_PUSHLV)&&
			(c[i+2]==VMOP_PUSHLV)&&
			(c[i+4]==VMOP_SISUB)&&
			(c[i+5]==VMOP_POPLV)&&
			(c[i+6]==c[i+1]) // pop var that got pushed?
			)
		{
			c[i]=VMOP_LIVSUBLIV;
			c[i+2]=c[i+3];
			c[i+3]=0xFFFF; // virtual nop
			c[i+4]=0xFFFF; // virtual nop
			c[i+5]=0xFFFF; // virtual nop
			c[i+6]=0xFFFF; // virtual nop
			i+=7;
			reloctbl[reloci++]=i;
			reloctbl[reloci++]=4;
		}
		else
		if(
			// ---- check for pushlv/pushlv/simul/poplv seq----
			(c[i]==VMOP_PUSHLV)&&
			(c[i+2]==VMOP_PUSHLV)&&
			(c[i+4]==VMOP_SIMUL)&&
			(c[i+5]==VMOP_POPLV)&&
			( (c[i+6]==c[i+1])||(c[i+6]==c[i+3]) ) // pop var that got pushed?
			)
		{
			c[i]=VMOP_LIVMULLIV;
			sU16 dst=c[i+6];
			sU16 src=(c[i+1]==dst)?c[i+3]:c[i+1]; // a+b == b+a, check order
			c[i+1]=dst;
			c[i+2]=src;
			c[i+3]=0xFFFF; // virtual nop
			c[i+4]=0xFFFF; // virtual nop
			c[i+5]=0xFFFF; // virtual nop
			c[i+6]=0xFFFF; // virtual nop
			i+=7;
			reloctbl[reloci++]=i;
			reloctbl[reloci++]=4;
		}
		else
		if(
			// ---- check for pushlv/pushlv/sidiv/poplv seq----
			(c[i]==VMOP_PUSHLV)&&
			(c[i+2]==VMOP_PUSHLV)&&
			(c[i+4]==VMOP_SIDIV)&&
			(c[i+5]==VMOP_POPLV)&&
			(c[i+6]==c[i+1]) // pop var that got pushed?
			)
		{
			c[i]=VMOP_LIVDIVLIV;
			c[i+2]=c[i+3];
			c[i+3]=0xFFFF; // virtual nop
			c[i+4]=0xFFFF; // virtual nop
			c[i+5]=0xFFFF; // virtual nop
			c[i+6]=0xFFFF; // virtual nop
			i+=7;
			reloctbl[reloci++]=i;
			reloctbl[reloci++]=4;
		}
		else
		if(
			// ---- check for pushlv/pushlv/siasl/poplv seq----
			(c[i]==VMOP_PUSHLV)&&
			(c[i+2]==VMOP_PUSHLV)&&
			(c[i+4]==VMOP_SIASL)&&
			(c[i+5]==VMOP_POPLV)&&
			(c[i+6]==c[i+1]) // pop var that got pushed?
			)
		{
			c[i]=VMOP_LIVASLLIV;
			c[i+2]=c[i+3];
			c[i+3]=0xFFFF; // virtual nop
			c[i+4]=0xFFFF; // virtual nop
			c[i+5]=0xFFFF; // virtual nop
			c[i+6]=0xFFFF; // virtual nop
			i+=7;
			reloctbl[reloci++]=i;
			reloctbl[reloci++]=4;
		}
		else
		if(
			// ---- check for pushlv/pushlv/siasr/poplv seq----
			(c[i]==VMOP_PUSHLV)&&
			(c[i+2]==VMOP_PUSHLV)&&
			(c[i+4]==VMOP_SIASR)&&
			(c[i+5]==VMOP_POPLV)&&
			(c[i+6]==c[i+1]) // pop var that got pushed?
			)
		{
			c[i]=VMOP_LIVASRLIV;
			c[i+2]=c[i+3];
			c[i+3]=0xFFFF; // virtual nop
			c[i+4]=0xFFFF; // virtual nop
			c[i+5]=0xFFFF; // virtual nop
			c[i+6]=0xFFFF; // virtual nop
			i+=7;
			reloctbl[reloci++]=i;
			reloctbl[reloci++]=4;
		}
		else
		if(
			// ---- check for pushlv/pushlv/sfadd/poplv seq----
			(c[i]==VMOP_PUSHLV)&&
			(c[i+2]==VMOP_PUSHLV)&&
			(c[i+4]==VMOP_SFADD)&&
			(c[i+5]==VMOP_POPLV)&&
			( (c[i+6]==c[i+1])||(c[i+6]==c[i+3]) ) // pop var that got pushed?
			)
		{
			c[i]=VMOP_LFVADDLFV;
			sU16 dst=c[i+6];
			sU16 src=(c[i+1]==dst)?c[i+3]:c[i+1]; // a+b == b+a, check order
			c[i+1]=dst;
			c[i+2]=src;
			c[i+3]=0xFFFF; // virtual nop
			c[i+4]=0xFFFF; // virtual nop
			c[i+5]=0xFFFF; // virtual nop
			c[i+6]=0xFFFF; // virtual nop
			i+=7;
			reloctbl[reloci++]=i;
			reloctbl[reloci++]=4;
		}
		else
		if(
			// ---- check for pushlv/pushlv/sfsub/poplv seq----
			(c[i]==VMOP_PUSHLV)&&
			(c[i+2]==VMOP_PUSHLV)&&
			(c[i+4]==VMOP_SFSUB)&&
			(c[i+5]==VMOP_POPLV)&&
			(c[i+6]==c[i+1]) // pop var that got pushed?
			)
		{
			c[i]=VMOP_LFVSUBLFV;
			c[i+2]=c[i+3];
			c[i+3]=0xFFFF; // virtual nop
			c[i+4]=0xFFFF; // virtual nop
			c[i+5]=0xFFFF; // virtual nop
			c[i+6]=0xFFFF; // virtual nop
			i+=7;
			reloctbl[reloci++]=i;
			reloctbl[reloci++]=4;
		}
		else
		if(
			// ---- check for pushlv/pushlv/sfmul/poplv seq----
			(c[i]==VMOP_PUSHLV)&&
			(c[i+2]==VMOP_PUSHLV)&&
			(c[i+4]==VMOP_SFMUL)&&
			(c[i+5]==VMOP_POPLV)&&
			( (c[i+6]==c[i+1])||(c[i+6]==c[i+3]) ) // pop var that got pushed?
			)
		{
			c[i]=VMOP_LFVMULLFV;
			sU16 dst=c[i+6];
			sU16 src=(c[i+1]==dst)?c[i+3]:c[i+1]; // a+b == b+a, check order
			c[i+1]=dst;
			c[i+2]=src;
			c[i+3]=0xFFFF; // virtual nop
			c[i+4]=0xFFFF; // virtual nop
			c[i+5]=0xFFFF; // virtual nop
			c[i+6]=0xFFFF; // virtual nop
			i+=7;
			reloctbl[reloci++]=i;
			reloctbl[reloci++]=4;
		}
		else
		if(
			// ---- check for pushlv/pushlv/sfdiv/poplv seq----
			(c[i]==VMOP_PUSHLV)&&
			(c[i+2]==VMOP_PUSHLV)&&
			(c[i+4]==VMOP_SFDIV)&&
			(c[i+5]==VMOP_POPLV)&&
			(c[i+6]==c[i+1]) // pop var that got pushed?
			)
		{
			c[i]=VMOP_LFVDIVLFV;
			c[i+2]=c[i+3];
			c[i+3]=0xFFFF; // virtual nop
			c[i+4]=0xFFFF; // virtual nop
			c[i+5]=0xFFFF; // virtual nop
			c[i+6]=0xFFFF; // virtual nop
			i+=7;
			reloctbl[reloci++]=i;
			reloctbl[reloci++]=4;
		}
		else
		if(
			// ---- check for pushlv/pushc/siand seq ----
			(c[i]==VMOP_PUSHLV)&&
			(c[i+2]==VMOP_PUSHC)&&
			(c[i+4]==VMOP_SIAND)
			)
		{
			c[i]=VMOP_PUSHLIVANDC;
			c[i+2]=c[i+3];
			c[i+3]=0xFFFF; // virtual nop
			c[i+4]=0xFFFF; // virtual nop
			i+=5;
			reloctbl[reloci++]=i;
			reloctbl[reloci++]=2;
		}
		else
		if(
			// ---- check for pushc/pushlv/siand seq ----
			(c[i]==VMOP_PUSHC)&&
			(c[i+2]==VMOP_PUSHLV)&&
			(c[i+4]==VMOP_SIAND)
			)
		{
			c[i]=VMOP_PUSHLIVANDC;
			sU16 t=c[i+1];
			c[i+1]=c[i+3];
			c[i+2]=t;
			c[i+3]=0xFFFF; // virtual nop
			c[i+4]=0xFFFF; // virtual nop
			i+=5;
			reloctbl[reloci++]=i;
			reloctbl[reloci++]=2;
		}
		else
		if(
			// ---- check for pushlv/pushc/sior seq ----
			(c[i]==VMOP_PUSHLV)&&
			(c[i+2]==VMOP_PUSHC)&&
			(c[i+4]==VMOP_SIOR)
			)
		{
			c[i]=VMOP_PUSHLIVORC;
			c[i+2]=c[i+3];
			c[i+3]=0xFFFF; // virtual nop
			c[i+4]=0xFFFF; // virtual nop
			i+=5;
			reloctbl[reloci++]=i;
			reloctbl[reloci++]=2;
		}
		else
		if(
			// ---- check for pushc/pushlv/sior seq ----
			(c[i]==VMOP_PUSHC)&&
			(c[i+2]==VMOP_PUSHLV)&&
			(c[i+4]==VMOP_SIOR)
			)
		{
			c[i]=VMOP_PUSHLIVORC;
			sU16 t=c[i+1];
			c[i+1]=c[i+3];
			c[i+2]=t;
			c[i+3]=0xFFFF; // virtual nop
			c[i+4]=0xFFFF; // virtual nop
			i+=5;
			reloctbl[reloci++]=i;
			reloctbl[reloci++]=2;
		}
		else
		if(
			// ---- check for pushlv/pushc/sieor seq ----
			(c[i]==VMOP_PUSHLV)&&
			(c[i+2]==VMOP_PUSHC)&&
			(c[i+4]==VMOP_SIEOR)
			)
		{
			c[i]=VMOP_PUSHLIVEORC;
			c[i+2]=c[i+3];
			c[i+3]=0xFFFF; // virtual nop
			c[i+4]=0xFFFF; // virtual nop
			i+=5;
			reloctbl[reloci++]=i;
			reloctbl[reloci++]=2;
		}
		else
		if(
			// ---- check for pushc/pushlv/sieor seq ----
			(c[i]==VMOP_PUSHC)&&
			(c[i+2]==VMOP_PUSHLV)&&
			(c[i+4]==VMOP_SIEOR)
			)
		{
			c[i]=VMOP_PUSHLIVEORC;
			sU16 t=c[i+1];
			c[i+1]=c[i+3];
			c[i+2]=t;
			c[i+3]=0xFFFF; // virtual nop
			c[i+4]=0xFFFF; // virtual nop
			i+=5;
			reloctbl[reloci++]=i;
			reloctbl[reloci++]=2;
		}
		else
		if(
			// ---- check for pushlv/pushc/simod seq ----
			(c[i]==VMOP_PUSHLV)&&
			(c[i+2]==VMOP_PUSHC)&&
			(c[i+4]==VMOP_SIMOD)
			)
		{
			c[i]=VMOP_PUSHLIVMODC;
			c[i+2]=c[i+3];
			c[i+3]=0xFFFF; // virtual nop
			c[i+4]=0xFFFF; // virtual nop
			i+=5;
			reloctbl[reloci++]=i;
			reloctbl[reloci++]=2;
		}
		else
		if(
			// ---- check for pushlv/pushc/siadd seq ----
			(c[i]==VMOP_PUSHLV)&&
			(c[i+2]==VMOP_PUSHC)&&
			(c[i+4]==VMOP_SIADD)
			)
		{
			c[i]=VMOP_PUSHLIVADDC;
			c[i+2]=c[i+3];
			c[i+3]=0xFFFF; // virtual nop
			c[i+4]=0xFFFF; // virtual nop
			i+=5;
			reloctbl[reloci++]=i;
			reloctbl[reloci++]=2;
		}
		else
		if(
			// ---- check for pushc/pushlv/siadd seq ----
			(c[i]==VMOP_PUSHC)&&
			(c[i+2]==VMOP_PUSHLV)&&
			(c[i+4]==VMOP_SIADD)
			)
		{
			c[i]=VMOP_PUSHLIVADDC;
			sU16 t=c[i+1];
			c[i+1]=c[i+3];
			c[i+2]=t;
			c[i+3]=0xFFFF; // virtual nop
			c[i+4]=0xFFFF; // virtual nop
			i+=5;
			reloctbl[reloci++]=i;
			reloctbl[reloci++]=2;
		}
		else
		if(
			// ---- check for pushlv/pushc/sisub seq ----
			(c[i]==VMOP_PUSHLV)&&
			(c[i+2]==VMOP_PUSHC)&&
			(c[i+4]==VMOP_SISUB)
			)
		{
			c[i]=VMOP_PUSHLIVSUBC;
			c[i+2]=c[i+3];
			c[i+3]=0xFFFF; // virtual nop
			c[i+4]=0xFFFF; // virtual nop
			i+=5;
			reloctbl[reloci++]=i;
			reloctbl[reloci++]=2;
		}
		else
		if(
			// ---- check for pushlv/pushc/simul seq ----
			(c[i]==VMOP_PUSHLV)&&
			(c[i+2]==VMOP_PUSHC)&&
			(c[i+4]==VMOP_SIMUL)
			)
		{
			c[i]=VMOP_PUSHLIVMULC;
			c[i+2]=c[i+3];
			c[i+3]=0xFFFF; // virtual nop
			c[i+4]=0xFFFF; // virtual nop
			i+=5;
			reloctbl[reloci++]=i;
			reloctbl[reloci++]=2;
		}
		else
		if(
			// ---- check for pushc/pushlv/simul seq ----
			(c[i]==VMOP_PUSHC)&&
			(c[i+2]==VMOP_PUSHLV)&&
			(c[i+4]==VMOP_SIMUL)
			)
		{
			c[i]=VMOP_PUSHLIVMULC;
			sU16 t=c[i+1];
			c[i+1]=c[i+3];
			c[i+2]=t;
			c[i+3]=0xFFFF; // virtual nop
			c[i+4]=0xFFFF; // virtual nop
			i+=5;
			reloctbl[reloci++]=i;
			reloctbl[reloci++]=2;
		}
		else
		if(
			// ---- check for pushlv/pushc/sidiv seq ----
			(c[i]==VMOP_PUSHLV)&&
			(c[i+2]==VMOP_PUSHC)&&
			(c[i+4]==VMOP_SIDIV)
			)
		{
			c[i]=VMOP_PUSHLIVDIVC;
			c[i+2]=c[i+3];
			c[i+3]=0xFFFF; // virtual nop
			c[i+4]=0xFFFF; // virtual nop
			i+=5;
			reloctbl[reloci++]=i;
			reloctbl[reloci++]=2;
		}
		else
		if(
			// ---- check for pushlv/pushc/siasl seq ----
			(c[i]==VMOP_PUSHLV)&&
			(c[i+2]==VMOP_PUSHC)&&
			(c[i+4]==VMOP_SIASL)
			)
		{
			c[i]=VMOP_PUSHLIVASLC;
			c[i+2]=c[i+3];
			c[i+3]=0xFFFF; // virtual nop
			c[i+4]=0xFFFF; // virtual nop
			i+=5;
			reloctbl[reloci++]=i;
			reloctbl[reloci++]=2;
		}
		else
		if(
			// ---- check for pushlv/pushc/siasr seq ----
			(c[i]==VMOP_PUSHLV)&&
			(c[i+2]==VMOP_PUSHC)&&
			(c[i+4]==VMOP_SIASR)
			)
		{
			c[i]=VMOP_PUSHLIVASRC;
			c[i+2]=c[i+3];
			c[i+3]=0xFFFF; // virtual nop
			c[i+4]=0xFFFF; // virtual nop
			i+=5;
			reloctbl[reloci++]=i;
			reloctbl[reloci++]=2;
		}
		else
		if(
			// ---- check for pushlv/pushc/sfadd seq ----
			(c[i]==VMOP_PUSHLV)&&
			(c[i+2]==VMOP_PUSHC)&&
			(c[i+4]==VMOP_SFADD)
			)
		{
			c[i]=VMOP_PUSHLFVADDC;
			c[i+2]=c[i+3];
			c[i+3]=0xFFFF; // virtual nop
			c[i+4]=0xFFFF; // virtual nop
			i+=5;
			reloctbl[reloci++]=i;
			reloctbl[reloci++]=2;
		}
		else
		if(
			// ---- check for pushc/pushlv/sfadd seq ----
			(c[i]==VMOP_PUSHC)&&
			(c[i+2]==VMOP_PUSHLV)&&
			(c[i+4]==VMOP_SFADD)
			)
		{
			c[i]=VMOP_PUSHLFVADDC;
			sU16 t=c[i+1];
			c[i+1]=c[i+3];
			c[i+2]=t;
			c[i+3]=0xFFFF; // virtual nop
			c[i+4]=0xFFFF; // virtual nop
			i+=5;
			reloctbl[reloci++]=i;
			reloctbl[reloci++]=2;
		}
		else
		if(
			// ---- check for pushlv/pushc/sfsub seq ----
			(c[i]==VMOP_PUSHLV)&&
			(c[i+2]==VMOP_PUSHC)&&
			(c[i+4]==VMOP_SFSUB)
			)
		{
			c[i]=VMOP_PUSHLFVSUBC;
			c[i+2]=c[i+3];
			c[i+3]=0xFFFF; // virtual nop
			c[i+4]=0xFFFF; // virtual nop
			i+=5;
			reloctbl[reloci++]=i;
			reloctbl[reloci++]=2;
		}
		else
		if(
			// ---- check for pushlv/pushc/sfmul seq ----
			(c[i]==VMOP_PUSHLV)&&
			(c[i+2]==VMOP_PUSHC)&&
			(c[i+4]==VMOP_SFMUL)
			)
		{
			c[i]=VMOP_PUSHLFVMULC;
			c[i+2]=c[i+3];
			c[i+3]=0xFFFF; // virtual nop
			c[i+4]=0xFFFF; // virtual nop
			i+=5;
			reloctbl[reloci++]=i;
			reloctbl[reloci++]=2;
		}
		else
		if(
			// ---- check for pushc/pushlv/sfmul seq ----
			(c[i]==VMOP_PUSHC)&&
			(c[i+2]==VMOP_PUSHLV)&&
			(c[i+4]==VMOP_SFMUL)
			)
		{
			c[i]=VMOP_PUSHLFVMULC;
			sU16 t=c[i+1];
			c[i+1]=c[i+3];
			c[i+2]=t;
			c[i+3]=0xFFFF; // virtual nop
			c[i+4]=0xFFFF; // virtual nop
			i+=5;
			reloctbl[reloci++]=i;
			reloctbl[reloci++]=2;
		}
		else
		if(
			// ---- check for pushlv/pushc/sfdiv seq ----
			(c[i]==VMOP_PUSHLV)&&
			(c[i+2]==VMOP_PUSHC)&&
			(c[i+4]==VMOP_SFDIV)
			)
		{
			c[i]=VMOP_PUSHLFVDIVC;
			c[i+2]=c[i+3];
			c[i+3]=0xFFFF; // virtual nop
			c[i+4]=0xFFFF; // virtual nop
			i+=5;
			reloctbl[reloci++]=i;
			reloctbl[reloci++]=2;
		}
		else
		if(
			// ---- check for pushlv/pushlv/siand seq----
			(c[i]==VMOP_PUSHLV)&&
			(c[i+2]==VMOP_PUSHLV)&&
			(c[i+4]==VMOP_SIAND)
			)
		{
			c[i]=VMOP_PUSHLIVANDLIV;
			c[i+2]=c[i+3];
			c[i+3]=0xFFFF; // virtual nop
			c[i+4]=0xFFFF; // virtual nop
			i+=5;
			reloctbl[reloci++]=i;
			reloctbl[reloci++]=2;
		}
		else
		if(
			// ---- check for pushlv/pushlv/sior seq----
			(c[i]==VMOP_PUSHLV)&&
			(c[i+2]==VMOP_PUSHLV)&&
			(c[i+4]==VMOP_SIOR)
			)
		{
			c[i]=VMOP_PUSHLIVORLIV;
			c[i+2]=c[i+3];
			c[i+3]=0xFFFF; // virtual nop
			c[i+4]=0xFFFF; // virtual nop
			i+=5;
			reloctbl[reloci++]=i;
			reloctbl[reloci++]=2;
		}
		else
		if(
			// ---- check for pushlv/pushlv/sieor seq----
			(c[i]==VMOP_PUSHLV)&&
			(c[i+2]==VMOP_PUSHLV)&&
			(c[i+4]==VMOP_SIEOR)
			)
		{
			c[i]=VMOP_PUSHLIVEORLIV;
			c[i+2]=c[i+3];
			c[i+3]=0xFFFF; // virtual nop
			c[i+4]=0xFFFF; // virtual nop
			i+=5;
			reloctbl[reloci++]=i;
			reloctbl[reloci++]=2;
		}
		else
		if(
			// ---- check for pushlv/pushlv/simod seq----
			(c[i]==VMOP_PUSHLV)&&
			(c[i+2]==VMOP_PUSHLV)&&
			(c[i+4]==VMOP_SIMOD)
			)
		{
			c[i]=VMOP_PUSHLIVMODLIV;
			c[i+2]=c[i+3];
			c[i+3]=0xFFFF; // virtual nop
			c[i+4]=0xFFFF; // virtual nop
			i+=5;
			reloctbl[reloci++]=i;
			reloctbl[reloci++]=2;
		}
		else
		if(
			// ---- check for pushlv/pushlv/siadd seq----
			(c[i]==VMOP_PUSHLV)&&
			(c[i+2]==VMOP_PUSHLV)&&
			(c[i+4]==VMOP_SIADD)
			)
		{
			c[i]=VMOP_PUSHLIVADDLIV;
			c[i+2]=c[i+3];
			c[i+3]=0xFFFF; // virtual nop
			c[i+4]=0xFFFF; // virtual nop
			i+=5;
			reloctbl[reloci++]=i;
			reloctbl[reloci++]=2;
		}
		else
		if(
			// ---- check for pushlv/pushlv/sisub seq----
			(c[i]==VMOP_PUSHLV)&&
			(c[i+2]==VMOP_PUSHLV)&&
			(c[i+4]==VMOP_SISUB)
			)
		{
			c[i]=VMOP_PUSHLIVSUBLIV;
			c[i+2]=c[i+3];
			c[i+3]=0xFFFF; // virtual nop
			c[i+4]=0xFFFF; // virtual nop
			i+=5;
			reloctbl[reloci++]=i;
			reloctbl[reloci++]=2;
		}
		else
		if(
			// ---- check for pushlv/pushlv/simul seq----
			(c[i]==VMOP_PUSHLV)&&
			(c[i+2]==VMOP_PUSHLV)&&
			(c[i+4]==VMOP_SIMUL)
			)
		{
			c[i]=VMOP_PUSHLIVMULLIV;
			c[i+2]=c[i+3];
			c[i+3]=0xFFFF; // virtual nop
			c[i+4]=0xFFFF; // virtual nop
			i+=5;
			reloctbl[reloci++]=i;
			reloctbl[reloci++]=2;
		}
		else
		if(
			// ---- check for pushlv/pushlv/sidiv seq----
			(c[i]==VMOP_PUSHLV)&&
			(c[i+2]==VMOP_PUSHLV)&&
			(c[i+4]==VMOP_SIDIV)
			)
		{
			c[i]=VMOP_PUSHLIVDIVLIV;
			c[i+2]=c[i+3];
			c[i+3]=0xFFFF; // virtual nop
			c[i+4]=0xFFFF; // virtual nop
			i+=5;
			reloctbl[reloci++]=i;
			reloctbl[reloci++]=2;
		}
		else
		if(
			// ---- check for pushlv/pushlv/siasl seq----
			(c[i]==VMOP_PUSHLV)&&
			(c[i+2]==VMOP_PUSHLV)&&
			(c[i+4]==VMOP_SIASL)
			)
		{
			c[i]=VMOP_PUSHLIVASLLIV;
			c[i+2]=c[i+3];
			c[i+3]=0xFFFF; // virtual nop
			c[i+4]=0xFFFF; // virtual nop
			i+=5;
			reloctbl[reloci++]=i;
			reloctbl[reloci++]=2;
		}
		else
		if(
			// ---- check for pushlv/pushlv/siasr seq----
			(c[i]==VMOP_PUSHLV)&&
			(c[i+2]==VMOP_PUSHLV)&&
			(c[i+4]==VMOP_SIASR)
			)
		{
			c[i]=VMOP_PUSHLIVASRLIV;
			c[i+2]=c[i+3];
			c[i+3]=0xFFFF; // virtual nop
			c[i+4]=0xFFFF; // virtual nop
			i+=5;
			reloctbl[reloci++]=i;
			reloctbl[reloci++]=2;
		}
		else
		if(
			// ---- check for pushlv/pushlv/sfadd seq----
			(c[i]==VMOP_PUSHLV)&&
			(c[i+2]==VMOP_PUSHLV)&&
			(c[i+4]==VMOP_SFADD)
			)
		{
			c[i]=VMOP_PUSHLFVADDLFV;
			c[i+2]=c[i+3];
			c[i+3]=0xFFFF; // virtual nop
			c[i+4]=0xFFFF; // virtual nop
			i+=5;
			reloctbl[reloci++]=i;
			reloctbl[reloci++]=2;
		}
		else
		if(
			// ---- check for pushlv/pushlv/sfsub seq----
			(c[i]==VMOP_PUSHLV)&&
			(c[i+2]==VMOP_PUSHLV)&&
			(c[i+4]==VMOP_SFSUB)
			)
		{
			c[i]=VMOP_PUSHLFVSUBLFV;
			c[i+2]=c[i+3];
			c[i+3]=0xFFFF; // virtual nop
			c[i+4]=0xFFFF; // virtual nop
			i+=5;
			reloctbl[reloci++]=i;
			reloctbl[reloci++]=2;
		}
		else
		if(
			// ---- check for pushlv/pushlv/sfmul seq----
			(c[i]==VMOP_PUSHLV)&&
			(c[i+2]==VMOP_PUSHLV)&&
			(c[i+4]==VMOP_SFMUL)
			)
		{
			c[i]=VMOP_PUSHLFVMULLFV;
			c[i+2]=c[i+3];
			c[i+3]=0xFFFF; // virtual nop
			c[i+4]=0xFFFF; // virtual nop
			i+=5;
			reloctbl[reloci++]=i;
			reloctbl[reloci++]=2;
		}
		else
		if(
			// ---- check for pushlv/pushlv/sfdiv seq----
			(c[i]==VMOP_PUSHLV)&&
			(c[i+2]==VMOP_PUSHLV)&&
			(c[i+4]==VMOP_SFDIV)
			)
		{
			c[i]=VMOP_PUSHLFVDIVLFV;
			c[i+2]=c[i+3];
			c[i+3]=0xFFFF; // virtual nop
			c[i+4]=0xFFFF; // virtual nop
			i+=5;
			reloctbl[reloci++]=i;
			reloctbl[reloci++]=2;
		}
		else 
		if( 
			// ---- check for pushv/pushv/sicmp/sitestz sequence ---- 
			(c[i]==VMOP_PUSHV)&& 
			(c[i+2]==VMOP_PUSHV)&& 
			( (c[i+4]>=VMOP_SICMPBGT)&&(c[i+4]<(VMOP_SICMPBGT+6)) )&& 
			(c[i+5]==VMOP_SITESTZ) 
			) 
		{ 
			c[i]=VMOP_JGTIVIV+ op_invert_tbl[(c[i+4]-VMOP_SICMPBGT)]; 
			//c[i+1]=c[i+1]; //var1 
			c[i+2]=c[i+3]; //var2 
			c[i+3]=c[i+6]; //label 
			c[i+4]=c[i+5]=c[i+6]=0xFFFF; 
			i+=7; 
			reloctbl[reloci++]=i; 
			reloctbl[reloci++]=3; 
		} 
		else 
		if( 
			// ---- check for pushv/pushc/sicmp/sitestz sequence ---- 
			(c[i]==VMOP_PUSHV)&& 
			(c[i+2]==VMOP_PUSHC)&& 
			((c[i+4]>=VMOP_SICMPBGT)&&(c[i+4]<(VMOP_SICMPBGT+6)) )&& 
			(c[i+5]==VMOP_SITESTZ) 
			) 
		{ 
			c[i]=VMOP_JGTIVIC+ op_invert_tbl[(c[i+4]-VMOP_SICMPBGT)]; 
			//c[i+1]=c[i+1]; //var1 
			c[i+2]=c[i+3]; //const 
			c[i+3]=c[i+6]; //label 
			c[i+4]=c[i+5]=c[i+6]=0xFFFF; 
			i+=7; 
			reloctbl[reloci++]=i; 
			reloctbl[reloci++]=3; 
		} 
		else
		{
			i+=vm_opcodesz[c[i]];
		}
	}


	// -------- zap NOPs, relocate code and finish post code optimization --------
	c=vm_code;
	i=0;
	sUI di=0;
	while(i<vm_pc)
	{
		if(c[i]!=0xFFFF) // nop?
		{
			sU16 opsz=vm_opcodesz[c[i]];
			sU16 j;
			for(j=0; j<opsz; j++)
				c[di+j]=c[i+j];
			i+=opsz;
			di+=opsz;
		}
		else
			i++;
	}
	vm_pc=di;
	i=0;
	while(i<vm_pc)
	{
		sU16 op=c[i];
		if(
			(op==VMOP_BRA)||
			////((op>=vmopcl[VMOPCL_SFCMP])&&(op<(vmopcl[VMOPCL_SFCMP]+6))) ||
			////((op>=vmopcl[VMOPCL_SICMP])&&(op<(vmopcl[VMOPCL_SICMP]+6))) ||
			(op==VMOP_SITESTZ)||
		 	(op==VMOP_SITESTZP)||
			(op==VMOP_SILOOP) 
			)
		{
			sU16 bt=c[i+1]; // get branch target
			sU16 j=0;
			sU16 rel=0;
			while(reloctbl[j++]<=bt) 
				rel+=reloctbl[j++];
			c[i+1]-=rel; // reloc branch
		} 
		else if(  ((op>=VMOP_JGTIVIV)&&(op<=VMOP_JEQIVIV))|| 
			((op>=VMOP_JGTIVIC)&&(op<=VMOP_JEQIVIC)) 
			) 
		{ 
			sU16 bt=c[i+3]; // get branch target 
			sU16 j=0; 
			sU16 rel=0; 
			while(reloctbl[j++]<=bt)  
				rel+=reloctbl[j++]; 
			c[i+3]-=rel; // reloc branch 
		} 
		i+=vm_opcodesz[op];
	}
	delete [] reloctbl;
 
	} // if b_jitopt

	// -------- print disassembly to console -------- 
	if(tkscript->configuration.b_showasm)
	{
		YAC_String s;
		//s.setEngine(tkscript);
		s.alloc(32768);
		disassemble(&s, 0, vm_pc);
		tkscript->print((char*)s.chars, 0); // print to stdout
	}

	// ---- fix array opcodes to use boundary checked versions ----
#ifdef DX_X86
	if(tkscript->configuration.b_checkbounds)
#else
    	if(0)
#endif
	{
		c=vm_code;
		i=0;
		while(i<vm_pc)
		{
			sU16 op=c[i];
			if( 
				(op==VMOP_IASEL)||
				(op==VMOP_IASEL)||
				(op==VMOP_FASEL)||
				//(op==vmopcl[VMOPCL_APUSHL])||
				(op==VMOP_SIAPUSHL)||
				//(op==vmopcl[VMOPCL_APUSHW])||
				//(op==vmopcl[VMOPCL_SIAPUSHW])||
				//(op==vmopcl[VMOPCL_APUSHB])||
				//(op==vmopcl[VMOPCL_SIAPUSHB])||
				//(op==vmopcl[VMOPCL_APUSH4B])||
				//(op==vmopcl[VMOPCL_SIAPUSH4B])||
				//(op==vmopcl[VMOPCL_APUSHNEXTL])||
				//(op==vmopcl[VMOPCL_APUSHNEXTW])||
				//(op==vmopcl[VMOPCL_APUSHNEXTB])||
				//(op==vmopcl[VMOPCL_APUSHNEXT4B])||
				//(op==vmopcl[VMOPCL_APUSHL16])||
				//(op==vmopcl[VMOPCL_APUSHW16])||
				//(op==vmopcl[VMOPCL_APUSHB16])||
				//(op==vmopcl[VMOPCL_APUSH4B16])||
				//(op==vmopcl[VMOPCL_APOPL])||
				(op==VMOP_SIAPOPL)
				//(op==vmopcl[VMOPCL_APOPW])||
				//(op==vmopcl[VMOPCL_SIAPOPW])||
				//(op==vmopcl[VMOPCL_APOPB])||
				//(op==vmopcl[VMOPCL_SIAPOPB])||
				//(op==vmopcl[VMOPCL_APOP4B])||
				//(op==vmopcl[VMOPCL_SIAPOP4B])||
				//(op==vmopcl[VMOPCL_APOPNEXTL])||
				//(op==vmopcl[VMOPCL_APOPNEXTW])||
				//(op==vmopcl[VMOPCL_APOPNEXTB])||
				//(op==vmopcl[VMOPCL_APOPNEXT4B])||
				//(op==vmopcl[VMOPCL_APOPL16])||
				//(op==vmopcl[VMOPCL_APOPW16])||
				//(op==vmopcl[VMOPCL_APOPB16])||
				//(op==vmopcl[VMOPCL_APOP4B16])||
				//(op==vmopcl[VMOPCL_APUSHL2])||
				//(op==vmopcl[VMOPCL_APUSHL3])||
				//(op==vmopcl[VMOPCL_APUSHL4])||
				//(op==vmopcl[VMOPCL_APUSHNEXTL2])||
				//(op==vmopcl[VMOPCL_APUSHNEXTL3])||
				//(op==vmopcl[VMOPCL_APUSHNEXTL4])||
				//(op==vmopcl[VMOPCL_APUSHL216])||
				//(op==vmopcl[VMOPCL_APUSHL316])||
				//(op==vmopcl[VMOPCL_APUSHL416])||
				//(op==vmopcl[VMOPCL_APOPL2])||
				//(op==vmopcl[VMOPCL_APOPL3])||
				//(op==vmopcl[VMOPCL_APOPL4])||
				//(op==vmopcl[VMOPCL_APOPNEXTL2])||
				//(op==vmopcl[VMOPCL_APOPNEXTL3])||
				//(op==vmopcl[VMOPCL_APOPNEXTL4])||
				//(op==vmopcl[VMOPCL_APOPL216])||
				//(op==vmopcl[VMOPCL_APOPL316])||
				//(op==vmopcl[VMOPCL_APOPL416])
				)
					c[i]++; // use boundary checked version of opcode
			i+=vm_opcodesz[op];
		}
	}


#ifdef DX_X86
	////sUI r=estimateJITSize()+5+vm_sz_startup+vm_sz_cleanup; 
   sUI r = estimateJITSize() + vm_sz_startup + vm_sz_cleanup; 
#endif 

#if defined(DX_68K) || defined(DX_ARM)
	sUI r = estimateJITSize() + vm_sz_startup + vm_sz_cleanup; 
#endif

//  	::printf("estimated jit size=%i\n", r);
	if(r > 0)
	{
      sUI alignedSz = ((r + TKSVM_PAGE_SIZE-1)/TKSVM_PAGE_SIZE) * TKSVM_PAGE_SIZE;
      jit.nonaligned_code = new sU8[alignedSz + TKSVM_PAGE_SIZE]; 

      if(tkscript->configuration.debug_level > 20)
			tkscript->printf("[...] JIT program size: %i bytes. VM program size: %i bytes\n", r, vm_pc*2); 
      
		if(NULL != jit.nonaligned_code)
		{
         // Align JIT code to page boundary
         jit.code = TKS_ALIGNPTR(jit.nonaligned_code, TKSVM_PAGE_SIZE);
			jit.sz_code = r;
         jit.sz_aligned = alignedSz;

#ifdef YAC_WIN32
			DWORD old;
			VirtualProtect(jit.code, r, PAGE_EXECUTE_READWRITE, &old);
#endif

#ifdef YAC_LINUX
			::mprotect((void*)jit.code, (size_t)alignedSz, PROT_READ|PROT_WRITE|PROT_EXEC);
#endif
			// Amiga w/ MMU does not need this 
 

			//-------- actually create new codepage --------
			c  = vm_code;
			ce = vm_code + vm_pc;

			//-------- write startup --------
			sU8 *d = jit.code; 

#ifdef DX_X86
			////*d++=0xE9; // jmp
			////*(sU32*)d=0; // jump to next statement ..
			////d+=4; 
#endif

			sU8 *s = vm_i86; 

         // Write prologue

#if defined(DX_X86) || defined(DX_ARM)
			for(i=0; i<vm_sz_startup; i++)
         {
				*d++ = *s++;
         }
#endif 

#ifdef DX_68K 
			*d++=0x48;	// 68k startup:			 
			*d++=0xe7;  // 48E7FFFE		movem.l d2-d6/a2-a6, -(sp) 
			*d++=0x3f;
			*d++=0x3e;
#endif 

			// 68k cleanup: 
			// 4CDF7FFF		movem.l (sp)+,d0-d7/a0-a6 
			// 4E75			rts 

			//-------- write program opcodes --------
			for(i=0; i<65536; i++)
			{
            // (todo) safe to skip this?
            off_opcodes[i]=(sU8*)0xFFFFFFFF;
			}


			// E8 call
			// D9 01 fld [ecx]
			// C7 05 FFFFFFFF CCCCCCCC 
			// 6A push
			// 8B mov ecx,
			// FF 24 jmp [eax*4+...]
			// 81 C1 add exc, 0xh

			sUI k;
			i=0;

			while(c < ce)
			{
				sU16 j=vm86_sz[*c];
				woff_opcodes[((sU16)(((sU32)c)-((sU32)vm_code)))>>1] = i;
				off_opcodes[i++] = d;
				s = vm_i86first + vm86_off[*c];

				for(k=0; k<j; k++)
				{
					*d++ = *s++;
				}

				c += vm_opcodesz[*c];
			}


			//-------- scan generated code and fix jumps --------
#ifdef DX_X86
			////s = jit.code + 5 + vm_sz_startup;
         s = jit.code + vm_sz_startup;
#else
			s = jit.code + vm_sz_startup;
#endif

#ifdef JIT_DEBUG
         printf("xxx vm_sz_startup=%u, s=%p\n", vm_sz_startup, s);
#endif

			c = vm_code;
			i = 0;

			while(s < d)
			{
				if(s >= (off_opcodes[i] + vm86_sz[*c]))
				{ 
//  				  ::printf("next opcode\n");
					c += vm_opcodesz[*c]; 
					i++;
				}

#ifdef DX_X86
            s = fixInstructionX86(c, s, d);
#endif

#ifdef DX_68K
            s = fixInstruction68K(c, s, d);
#endif

#ifdef DX_ARM
            s = fixInstructionARM(c, (sU32*)s, d, i);
#endif

			} // while (s < d)

			//-------- write cleanup / epilogue -------- 
#ifdef DX_X86 
			*d++ = 0x5F;
			*d++ = 0x5E;
			*d++ = 0x5B;
			*d++ = 0x8B;
			*d++ = 0xE5;
			*d++ = 0x5D;
			*d++ = 0xC3; 
#endif // DX_X86

#ifdef DX_68K 
			*d++ = 0x4c;	// 4CDF7cFc		movem.l (sp)+,d0-d7/a0-a6 
			*d++ = 0xdf; 
			*d++ = 0x7c; 
			*d++ = 0xFc; 
			*d++ = 0x4e;	// 4E75			rts 
			*d++ = 0x75; 
#endif // DX_68K

#ifdef DX_ARM
         *d++ = 0x01;   // add sp, r6, #256
         *d++ = 0xdc;
         *d++ = 0x86;
         *d++ = 0xe2;

         *d++ = 0xF8;   // pop {r3-r11, lr}  (note) r3 is saved/restored to maintain 8 byte stack alignment
         *d++ = 0x8F;
         *d++ = 0xBD;
         *d++ = 0xE8;
#endif // DX_ARM

#ifdef YAC_LINUX
         // (todo) align 'r' to page size ?!
         // (note) delete[] on protected area results in SEGV_ACCERR error
			::mprotect((void*)jit.code, (size_t)alignedSz, PROT_READ|PROT_EXEC);

#ifdef DX_ARM
         loc_arm_cache_flush((void*)jit.code, (void*)(((sU8*)jit.code) + alignedSz));
#endif // DX_ARM

#endif // YAC_LINUX

			return 1;
		}
	}
	return 0;
}

#ifdef DX_X86
sU8 *VMCore::fixInstructionX86(sU16 *c, sU8 *s, sU8 *d) {
   ////if( (*c==vmopcl[VMOPCL_OTSWITCH]) && ( (*(sU32*)s) == 0x90909090 ) )
   ////{
   ////	// create switch jump table
   ////	*(sU32*)s=(sU32)(off_opcodes[woff_opcodes[c[1]]]);
   ////	s+=4;
   ////	*(sU32*)s=(sU32)(off_opcodes[woff_opcodes[c[2]]]);
   ////	s+=4;
   ////	*(sU32*)s=(sU32)(off_opcodes[woff_opcodes[c[3]]]);
   ////	s+=4;
   ////	*(sU32*)s=(sU32)(off_opcodes[woff_opcodes[c[4]]]);
   ////	s+=4;
   ////} 
   if(*s++==0xb8) // MOV EAX
   {
      ////if( (*(sU32*)(s)) == 0xC0DE1234 )
      ////{
      ////   s[-1]=0xE9; // jump to vm_end 
      ////   *(sU32*)s=(sU32)(d-s-4);
      ////   s+=4;
      ////}
      ////else
      {
         if( (*(sU32*)(s)) ==0xC0DEC0DE)
         {
            // branch
            s[-1]=0xE9; // JMP
            *(sU32*)s=(sU32)(off_opcodes[woff_opcodes[c[1]]]-s-4);
            s+=4;
         }
         else
            if( (*(sU32*)(s)) ==0xC0DEC0FE)
            {
               // branch
               s[-1]=0xE9; // JMP
               *(sU32*)s=(sU32)(off_opcodes[woff_opcodes[c[2]]]-s-4);
               s+=4;
            } 
            else
               if( (*(sU32*)(s)) ==0xC0DEC0D3) 
               { 
                  // branch 
                  s[-1]=0xE9; // JMP 
                  *(sU32*)s=(sU32)(off_opcodes[woff_opcodes[c[3]]]-s-4); 
                  s+=4; 
               } 
               else
						if( (*(sU32*)s) == 0xC0FFC4C4 )
						{
							// variable src
							*(sU32*)s=(sU32) vm_vars[c[1]].ui; // push adr of var
							s+=4;
						}
      }
   } 
   else
   {
      if( (*(sU32*)(s-1)) == 0xC0FFC0FF )
      {
         // const 
         *(sU32*)(s-1) = (sU32) vm_const[c[1]].ui;
         //s+=4; 
         s+=3;
      }
      else
         if( (*(sU32*)(s-1)) == 0xC0FFC0FE )
         {
            // const, 2nd arg
            *(sU32*)(s-1) = (sU32) vm_const[c[2]].ui;
            //s+=4; 
            s+=3;
         }
         else
            if( (*(sU32*)(s-1)) == 0xC0DE4567 )
            {
               // otswitch
               // jump table
               *(sU32*)(s-1)= (sU32)(s+5+2);
               s+=5; // xXXX
            }
            else
               //if( (*(sU32*)s) == 0xC0FFC4C4 ) 
               if( (*(sU32*)(s-1)) == 0xC0FFC4C4 )
               {
                  // variable dest
                  *(sU32*)(s-1)=(sU32) vm_vars[c[1]].ui; // push adr of var
                  //s+=4; 
                  s+=3;
               }
               else
						if( (*(sU32*)(s-1)) == 0xC0FFC4C5 )
						{
							// variable dest
							*(sU32*)(s-1)=(sU32) vm_vars[c[2]].ui; // push adr of var, 2nd arg
							//s+=4; 
							s+=3;
						}
						else
                     if( ((*(sU32*)&s[-1])) == 0xC0FFAAA1 )
                     {
                        // int array selector
                        *(sU32*)&s[-1] = (sU32) &vm_iarr[c[1]].adr.ui;
                        s+=3;
                     }
                     else
                        if( ((*(sU32*)&s[-1])) == 0xC0FFABA1 )
                        {
                           // int array upper byte boundary
                           *(sU32*)&s[-1] = (sU32) &vm_iarr[c[1]].maskb;
                           s+=3;
                        }
                        else
                           if( ((*(sU32*)&s[-1])) == 0xC0FFAAA2 )
                           {
                              // float array selector
                              *(sU32*)&s[-1] = (sU32) &vm_farr[c[1]].adr.ui;
                              s+=3;
                           }
                           else
                              if( ((*(sU32*)&s[-1])) == 0xC0FFABA2 )
                              {
                                 // float array upper byte boundary
                                 *(sU32*)&s[-1] = (sU32) &vm_farr[c[1]].mask;
                                 s+=3;
                              }
                              else
                                 //if( ((*(sU32*)&s[-1])) == (sU32) (-0xC0FFC7C7) )
                                 if( ((*(sU32*)&s[-1])) == (sU32) ( (~((sU32)0xC0FFC7C7))+1 ) )
                                 {
                                    // local variable access (relative to ebp)
                                    sU32 t=(( (((sU32)c[1])<<2)+Dltmpsize ));
                                    *(sU32*)&s[-1] = (~t)+1 ;
                                    s+=3;
                                 }
                                 else
                                    //if( ((*(sU32*)&s[-1])) == (sU32) (-0xC0FFC7C8) )
                                    if( ((*(sU32*)&s[-1])) == (sU32) ( (~((sU32)0xC0FFC7C8))+1 ) )
                                    {
                                       // 2nd local variable access (relative to ebp)
                                       sU32 t=(( (((sU32)c[2])<<2)+Dltmpsize ));
                                       *(sU32*)&s[-1] = (~t)+1;
                                       s+=3;
                                    }

   }

   return s;
}
#endif // DX_X86

#ifdef DX_68K
sU8 *VMCore::fixInstruction68K(sU16 *c, sU8 *s, sU8 *e) {
   switch(*s)
   {
      default:
         s++;
         break;
      case 0xC0: 
         switch(s[1])
         { 
            default:
               s++;
               break;
            case 0xDE:
               switch(s[2])
               { 
						default:
                     s++;
                     break;
						case 0x12:
							switch(s[3])
							{
                        default:
                           s++;
                           break;
                        case 0x34:
                           *(sU32*)s=(sU32)d; // halt
                           s+=4;
                           break;
							}
							break;
						case 0xC0:
							switch(s[3])
							{
                        default:
                           s++;
                           break;
                        case 0xDE:
                           // branch
                           *(sU32*)s=(sU32)(off_opcodes[woff_opcodes[c[1]]]);
                           s+=4;
                           break;
                        case 0xFE:
                           // branch
                           *(sU32*)s=(sU32)(off_opcodes[woff_opcodes[c[2]]]);
                           s+=4;
                           break;
							}
							break;
               }
               break;
            case 0xFF:
               switch(s[2])
               {
						default:
                     s++;
                     break;
						case 0xC0:
							switch(s[3])
							{
                        default:
                           s++;
                           break;
                        case 0xFF:
                           // const 
                           *(sU32*)(s) = (sU32) vm_const[c[1]].ui;
                           s+=4;
                           break;
                        case 0xFE:
                           // const, 2nd arg
                           *(sU32*)(s) = (sU32) vm_const[c[2]].ui;
                           s+=4;
                           break;
							}
							break;
						case 0xC4:
							switch(s[3])
							{
                        default:
                           s++;
                           break;
                        case 0xC4:
                           // variable 
                           *(sU32*)s=(sU32) vm_vars[c[1]].ui; // push adr of var
                           s+=4;
                           break;
                        case 0xC5:
                           // variable dest
                           *(sU32*)s=(sU32) vm_vars[c[2]].ui; // push adr of var, 2nd arg
                           s+=4;
                           break;
							}
							break;
						case 0xFA:
							switch(s[3])
							{
                        default:
                           s++;
                           break;
                        case 0xA2:
                           // float array selector
                           *(sU32*)s = (sU32) &vm_farr[c[1]].adr.ui;
                           s+=4;
                           break;
							}
						case 0xAA:
							switch(s[3])
							{
                        default:
                           s++;
                           break;
                        case 0xA1:
                           // int array selector
                           *(sU32*)s = (sU32) &vm_iarr[c[1]].adr.ui;
                           s+=4;
                           break;
                        case 0xA2:
                           // float array selector
                           *(sU32*)s = (sU32) &vm_farr[c[1]].adr.ui;
                           s+=4;
                           break;
							}
							break;
						case 0xAB:
							switch(s[3])
							{
                        default:
                           s++;
                           break;
                        case 0xA1:
                           // int array upper byte boundary
                           *(sU32*)s = (sU32) &vm_iarr[c[1]].maskb;
                           s+=4;
                           break;
                        case 0xA2:
                           // float array upper byte boundary
                           *(sU32*)s = (sU32) &vm_farr[c[1]].mask;
                           s+=4;
                           break;
							}
               }
               break;
         }
         break;
   }

   return s;
}
#endif // DX_68K

#ifdef DX_ARM
sU8 *VMCore::fixInstructionARM(sU16 *c, sU32 *s, sU8 *e, sUI i) {

   sU16 vmop = c[0];

#define Dfixbranch2(soff, vmdstpc) s[soff] = (s[soff] & 0xFF000000u) | ( (( ((sS32)off_opcodes[woff_opcodes[vmdstpc]]) - ((sS32)(s+soff+2)) ) >> 2) & 0x00FFFFFFu ) 

#ifdef JIT_DEBUG 
#define Dfixbranch(soff, vmdstpc) Dfixbranch2(soff, vmdstpc); printf("xxx Dfixbranch: cs=%p dst=%p (vmpc=%u) op=0x%08x\n", (s+soff+2), off_opcodes[woff_opcodes[vmdstpc]], vmdstpc, s[soff])
#else
#define Dfixbranch Dfixbranch2
#endif // JIT_DEBUG

#define Dfiximm10(soff, idx) s[soff] = (s[soff] & ~1023u) | (idx << 2)

// for vldr/vstr
#define Dfiximm8(soff, idx) s[soff] = (s[soff] & ~255u) | idx

   switch(vmop)
   {
      default:
         tkscript->printf("[---] VMCore::fixInstructionARM: internal error: unhandled VM opcode %u at VMPC=%u\n", vmop, i);
         s = NULL;
         break;

      case VMOP_HALT:
         break;
      case VMOP_BRA:
         Dfixbranch(0, c[1]);
         break;
      case VMOP_SITESTZ:
      case VMOP_SITESTZP:
         Dfixbranch(2, c[1]);
         break;
      case VMOP_BSR:                 // unused
         Dfixbranch(0, c[1]);
         break;
      case VMOP_RTS:                 // unused
         break;
      case VMOP_SILOOP:
         Dfixbranch(3, c[1]);
         break;
      case VMOP_PUSHLV:
         Dfiximm10(0, c[1]);
         break;
      case VMOP_PUSHLIVINC:
      case VMOP_PUSHINCLIV:
      case VMOP_PUSHLIVDEC:
      case VMOP_PUSHDECLIV:
         Dfiximm10(0, c[1]);
         Dfiximm10(3, c[1]);
         break;
      case VMOP_POPLV:
         Dfiximm10(1, c[1]);
         break;
      case VMOP_PUSHV:
         Dfiximm10(0, c[1]);
         break;
      case VMOP_PUSHIVINC:
      case VMOP_PUSHINCIV:
      case VMOP_PUSHIVDEC:
      case VMOP_PUSHDECIV:
      case VMOP_POPV:
      case VMOP_PUSHC:
      case VMOP_LOADFXC:              // load fxnaddr for APICALL*
         Dfiximm10(0, c[1]);
         break;
      case VMOP_MOVLV:
      case VMOP_MOVLVC:
      case VMOP_MOVV:
      case VMOP_MOVVC:
         Dfiximm10(0, c[2]);  // src
         Dfiximm10(1, c[1]);  // dst
         break;
      case VMOP_INCLIV:
      case VMOP_DECLIV:
         Dfiximm10(0, c[1]);
         Dfiximm10(2, c[1]);
         break;
      case VMOP_INCIV:
      case VMOP_DECIV:
         Dfiximm10(0, c[1]);
         break;
      case VMOP_LIVANDLIV:
      case VMOP_LIVORLIV:
      case VMOP_LIVEORLIV:
         Dfiximm10(0, c[1]);  // dst
         Dfiximm10(1, c[2]);  // src
         Dfiximm10(3, c[1]);  // dst
         break;
      case VMOP_LIVMODLIV:
         Dfiximm10(0, c[1]);  // dst
         Dfiximm10(1, c[2]);  // src
         Dfiximm10(4, c[1]);  // dst
         break;
      case VMOP_LIVADDLIV:
      case VMOP_LIVSUBLIV:
      case VMOP_LIVMULLIV:
      case VMOP_LIVDIVLIV:
      case VMOP_LIVASLLIV:
      case VMOP_LIVASRLIV:
         Dfiximm10(0, c[1]);  // dst
         Dfiximm10(1, c[2]);  // src
         Dfiximm10(3, c[1]);  // dst
         break;
      case VMOP_PUSHLIVANDLIV:
      case VMOP_PUSHLIVORLIV:
      case VMOP_PUSHLIVEORLIV:
      case VMOP_PUSHLIVMODLIV:
      case VMOP_PUSHLIVADDLIV:
      case VMOP_PUSHLIVSUBLIV:
      case VMOP_PUSHLIVMULLIV:
      case VMOP_PUSHLIVDIVLIV:
      case VMOP_PUSHLIVASLLIV:
      case VMOP_PUSHLIVASRLIV:
      case VMOP_PUSHLIVANDC:
      case VMOP_PUSHLIVORC:
      case VMOP_PUSHLIVEORC:
      case VMOP_PUSHLIVMODC:
      case VMOP_PUSHLIVADDC:
      case VMOP_PUSHLIVSUBC:
      case VMOP_PUSHLIVMULC:
      case VMOP_PUSHLIVDIVC:
      case VMOP_PUSHLIVASLC:
      case VMOP_PUSHLIVASRC:
         Dfiximm10(0, c[1]);  // lhs
         Dfiximm10(1, c[2]);  // rhs
         break;
      case VMOP_LFVADDLFV:
      case VMOP_LFVSUBLFV:
      case VMOP_LFVMULLFV:
      case VMOP_LFVDIVLFV:
         Dfiximm8(0, c[1]);  // lhs/dst
         Dfiximm8(1, c[2]);  // rhs
         Dfiximm8(3, c[1]);  // lhs/dst
         break;
      case VMOP_PUSHLFVADDLFV:
      case VMOP_PUSHLFVSUBLFV:
      case VMOP_PUSHLFVMULLFV:
      case VMOP_PUSHLFVDIVLFV:
      case VMOP_PUSHLFVADDC:
      case VMOP_PUSHLFVSUBC:
      case VMOP_PUSHLFVMULC:
      case VMOP_PUSHLFVDIVC:
         Dfiximm8(0, c[1]);  // lhs
         Dfiximm8(1, c[2]);  // rhs
         break;
      case VMOP_SINOT:
      case VMOP_SIINV:
      case VMOP_SINEG:
      case VMOP_SIQUAD:
      case VMOP_SITESTBZ:
      case VMOP_SITESTBZ2:
      case VMOP_SFNEG:
      case VMOP_SFQUAD:
      case VMOP_SFSIN:
      case VMOP_SFCOS:
      case VMOP_SFTAN:
      case VMOP_SFSQRT:
      case VMOP_SIABS:
      case VMOP_SFABS:
      case VMOP_SFFRAC:
      case VMOP_SFROUND:
      case VMOP_SIAND:
      case VMOP_SIOR:
      case VMOP_SIEOR:
      case VMOP_SIMOD:
      case VMOP_SIADD:
      case VMOP_SISUB:
      case VMOP_SIMUL:
      case VMOP_SIDIV:
      case VMOP_SIASL:
      case VMOP_SIASR:
      case VMOP_SICMPBGT:
      case VMOP_SICMPBGE:
      case VMOP_SICMPBLT:
      case VMOP_SICMPBLE:
      case VMOP_SICMPBNE:
      case VMOP_SICMPBEQ:
      case VMOP_SFADD:
      case VMOP_SFSUB:
      case VMOP_SFMUL:
      case VMOP_SFDIV:
      case VMOP_SFCMPBGT:
      case VMOP_SFCMPBGE:
      case VMOP_SFCMPBLT:
      case VMOP_SFCMPBLE:
      case VMOP_SFCMPBNE:
      case VMOP_SFCMPBEQ:
      case VMOP_STCIF:
      case VMOP_STCIF2:
      case VMOP_STCFI:
      case VMOP_STCFI2:
         break;
      case VMOP_IASEL:
      case VMOP_IASELBC:
      case VMOP_FASEL:
      case VMOP_FASELBC:
         Dfiximm10(0, c[1]);  // array var
         break;
      case VMOP_SIAPUSHL:
      case VMOP_SIAPUSHLBC:
      case VMOP_SIAPOPL:
      case VMOP_SIAPOPLBC:
      case VMOP_SIPOP:
      case VMOP_BEGAPICALL0:
      case VMOP_BEGAPICALL1:
      case VMOP_BEGAPICALL2:
      case VMOP_BEGAPICALL3:
      case VMOP_BEGAPICALL4:
      case VMOP_BEGAPICALL5:
      case VMOP_BEGAPICALL6:
      case VMOP_BEGAPICALL7:
      case VMOP_BEGAPICALL8:
      case VMOP_APICALL:
      case VMOP_ENDAPICALL0:
      case VMOP_ENDAPICALL1:
      case VMOP_ENDAPICALL2:
      case VMOP_ENDAPICALL3:
      case VMOP_ENDAPICALL4:
      case VMOP_ENDAPICALL5:
      case VMOP_ENDAPICALL6:
      case VMOP_ENDAPICALL7:
      case VMOP_ENDAPICALL8:
      case VMOP_INCSTP:
      case VMOP_DECSTP:
      case VMOP_SIPACKARGB:
      case VMOP_SRETI:
      case VMOP_SRETF:
      case VMOP_PUSHS:
         break;
      case VMOP_IVANDIV:
      case VMOP_IVORIV:
      case VMOP_IVEORIV:
      case VMOP_IVMODIV:
      case VMOP_IVADDIV:
      case VMOP_IVSUBIV:
      case VMOP_IVMULIV:
      case VMOP_IVDIVIV:
      case VMOP_IVASLIV:
      case VMOP_IVASRIV:
      case VMOP_PUSHIVANDIV:
      case VMOP_PUSHIVORIV:
      case VMOP_PUSHIVEORIV:
      case VMOP_PUSHIVMODIV:
      case VMOP_PUSHIVADDIV:
      case VMOP_PUSHIVSUBIV:
      case VMOP_PUSHIVMULIV:
      case VMOP_PUSHIVDIVIV:
      case VMOP_PUSHIVASLIV:
      case VMOP_PUSHIVASRIV:
      case VMOP_PUSHIVANDC:
      case VMOP_PUSHIVORC:
      case VMOP_PUSHIVEORC:
      case VMOP_PUSHIVMODC:
      case VMOP_PUSHIVADDC:
      case VMOP_PUSHIVSUBC:
      case VMOP_PUSHIVMULC:
      case VMOP_PUSHIVDIVC:
      case VMOP_PUSHIVASLC:
      case VMOP_PUSHIVASRC:
         Dfiximm10(0, c[1]); // dstvar resp. lhsvar
         Dfiximm10(1, c[2]); // srcvar resp. constidx resp. rhsvar resp. rhsconstval
         break;
      case VMOP_FVADDFV:
      case VMOP_FVSUBFV:
      case VMOP_FVMULFV:
      case VMOP_FVDIVFV:
         Dfiximm10(0, c[1]); // lhsvar
         Dfiximm10(1, c[2]); // rhsvar
         break;
      case VMOP_PUSHFVADDFV:
      case VMOP_PUSHFVSUBFV:
      case VMOP_PUSHFVMULFV:
      case VMOP_PUSHFVDIVFV:
         Dfiximm10(0, c[1]); // lhsvar
         Dfiximm10(1, c[2]); // rhsvar
         break;
      case VMOP_PUSHFVADDC:
      case VMOP_PUSHFVSUBC:
      case VMOP_PUSHFVMULC:
      case VMOP_PUSHFVDIVC:
         Dfiximm10(0, c[1]); // lhsvar
         Dfiximm8 (1, c[2]); // constidx
         break;
      case VMOP_JGTIVIV:
      case VMOP_JGEIVIV:
      case VMOP_JLTIVIV:
      case VMOP_JLEIVIV:
      case VMOP_JNEIVIV:
      case VMOP_JEQIVIV:
         Dfiximm10(0, c[1]); // lhsvar
         Dfiximm10(1, c[2]); // rhsvar
         Dfixbranch(5, c[3]);
         break;
      case VMOP_JGTIVIC:
      case VMOP_JGEIVIC:
      case VMOP_JLTIVIC:
      case VMOP_JLEIVIC:
      case VMOP_JNEIVIC:
      case VMOP_JEQIVIC:
         Dfiximm10(0, c[1]); // lhsvar
         Dfiximm10(1, c[2]); // rhsconst
         Dfixbranch(4, c[3]);
         break;
   }

   sUI sz = vm86_sz[vmop] >> 2;
   s += sz;

   return (sU8*)s;
}
#endif // DX_ARM


#if 0
sU8 VMCoreInline::compile(VMCore *_vm) {
	// -------- relocate vm bytecode to _vm --------
	sU16 *c=vm_code;
	sUI i;
	i=0;
	while(i<vm_pc)
	{
		sU16 op=c[i];
		if(
			(op==VMOP_BRA)||
			////((op>=vmopcl[VMOPCL_SFCMP])&&(op<(vmopcl[VMOPCL_SFCMP]+6))) ||
			////((op>=vmopcl[VMOPCL_SICMP])&&(op<(vmopcl[VMOPCL_SICMP]+6))) ||
			(op==VMOP_SITESTZ)||
			(op==VMOP_SITESTZP)||
			(op==VMOP_SILOOP) 
			)
		{
			// fix branch, labels are already resolved
			c[i+1]+=_vm->vm_pc;
		} 
		else 
		if( ((op>=VMOP_JGTIVIV)&&(op<=VMOP_JEQIVIV))|| 
			((op>=VMOP_JGTIVIC)&&(op<=VMOP_JEQIVIC)) 
			) 
		{ 
			c[i+3]+=_vm->vm_pc; 
		} 
		else
		if(
			(op==VMOP_PUSHC)
			)
		{
			// fix constant
			c[i+1]=_vm->addConstInt(*(sSI*)&vm_const[c[i+1]].ui);
		}
		else
		if(
			(op==VMOP_MOVLVC)||
			////(op==vmopcl[VMOPCL_LIVANDC])||
			////(op==vmopcl[VMOPCL_LIVORC])||
			////(op==vmopcl[VMOPCL_LIVEORC])||
			////(op==vmopcl[VMOPCL_LIVMODC])||
			////(op==vmopcl[VMOPCL_LIVADDC])||
			////(op==vmopcl[VMOPCL_LIVSUBC])||
			////(op==vmopcl[VMOPCL_LIVMULC])||
			////(op==vmopcl[VMOPCL_LIVDIVC])||
			////(op==vmopcl[VMOPCL_LIVASLC])||
			////(op==vmopcl[VMOPCL_LIVASRC])||
			(op==VMOP_PUSHLIVANDC)||
			(op==VMOP_PUSHLIVORC)||
			(op==VMOP_PUSHLIVEORC)||
			(op==VMOP_PUSHLIVMODC)||
			(op==VMOP_PUSHLIVADDC)||
			(op==VMOP_PUSHLIVSUBC)||
			(op==VMOP_PUSHLIVMULC)||
			(op==VMOP_PUSHLIVDIVC)||
			(op==VMOP_PUSHLIVASLC)||
			(op==VMOP_PUSHLIVASRC)||
			////(op==vmopcl[VMOPCL_LFVADDC])||
			////(op==vmopcl[VMOPCL_LFVSUBC])||
			////(op==vmopcl[VMOPCL_LFVMULC])||
			////(op==vmopcl[VMOPCL_LFVDIVC])||
			(op==VMOP_PUSHLFVADDC)||
			(op==VMOP_PUSHLFVSUBC)||
			(op==VMOP_PUSHLFVMULC)||
			(op==VMOP_PUSHLFVDIVC)
			)
		{
			// fix local var and constant
			c[i+1]=_vm->addLocalVarRef(vm_localvarnames[c[i+1]]);
			c[i+2]=_vm->addConstInt(*(sSI*)&vm_const[c[i+2]].ui);
		}
		else
		if(
			(op==VMOP_MOVVC)||
			////(op==vmopcl[VMOPCL_IVANDC])|| 
			////(op==vmopcl[VMOPCL_IVORC])|| 
			////(op==vmopcl[VMOPCL_IVEORC])|| 
			////(op==vmopcl[VMOPCL_IVMODC])|| 
			////(op==vmopcl[VMOPCL_IVADDC])|| 
			////(op==vmopcl[VMOPCL_IVSUBC])|| 
			////(op==vmopcl[VMOPCL_IVMULC])|| 
			////(op==vmopcl[VMOPCL_IVDIVC])|| 
			////(op==vmopcl[VMOPCL_IVASLC])|| 
			////(op==vmopcl[VMOPCL_IVASRC])|| 
			(op==VMOP_PUSHIVANDC)|| 
			(op==VMOP_PUSHIVORC)|| 
			(op==VMOP_PUSHIVEORC)|| 
			(op==VMOP_PUSHIVMODC)|| 
			(op==VMOP_PUSHIVADDC)|| 
			(op==VMOP_PUSHIVSUBC)|| 
			(op==VMOP_PUSHIVMULC)|| 
			(op==VMOP_PUSHIVDIVC)|| 
			(op==VMOP_PUSHIVASLC)|| 
			(op==VMOP_PUSHIVASRC)|| 
			////(op==vmopcl[VMOPCL_FVADDC])|| 
			////(op==vmopcl[VMOPCL_FVSUBC])|| 
			////(op==vmopcl[VMOPCL_FVMULC])|| 
			////(op==vmopcl[VMOPCL_FVDIVC])|| 
			(op==VMOP_PUSHFVADDC)|| 
			(op==VMOP_PUSHFVSUBC)|| 
			(op==VMOP_PUSHFVMULC)|| 
			(op==VMOP_PUSHFVDIVC) 
			)
		{
			// fix globalvar and constant
			c[i+1]=_vm->addVarRef(vm_vars[c[i+1]], vm_varnames[c[i+1]]);
			c[i+2]=_vm->addConstInt(*(sSI*)&vm_const[c[i+2]].ui);
		}
		else
		if(
			(op==VMOP_PUSHLV)||
			(op==VMOP_PUSHLIVINC)||
			(op==VMOP_PUSHINCLIV)||
			(op==VMOP_PUSHLIVDEC)||
			(op==VMOP_PUSHDECLIV)||
			(op==VMOP_POPLV)||
			//(op==vmopcl[VMOPCL_APUSHL])||
			//(op==vmopcl[VMOPCL_APUSHLBC])||
			//(op==vmopcl[VMOPCL_APUSHW])||
			//(op==vmopcl[VMOPCL_sBoolAPUSHWBC])||
			//(op==vmopcl[VMOPCL_APUSHB])||
			//(op==vmopcl[VMOPCL_APUSHBBC])||
			//(op==vmopcl[VMOPCL_APUSH4B])||
			//(op==vmopcl[VMOPCL_APUSH4BBC])||
			//(op==vmopcl[VMOPCL_APUSHNEXTL])||
			//(op==vmopcl[VMOPCL_APUSHNEXTLBC])||
			//(op==vmopcl[VMOPCL_APUSHNEXTW])||
			//(op==vmopcl[VMOPCL_APUSHNEXTWBC])||
			//(op==vmopcl[VMOPCL_APUSHNEXTB])||
			//(op==vmopcl[VMOPCL_APUSHNEXTBBC])||
			//(op==vmopcl[VMOPCL_APUSHNEXT4B])||
			//(op==vmopcl[VMOPCL_APUSHNEXT4BBC])||
			//(op==vmopcl[VMOPCL_APOPL])||
			//(op==vmopcl[VMOPCL_APOPLBC])||
			//(op==vmopcl[VMOPCL_APOPW])||
			//(op==vmopcl[VMOPCL_APOPWBC])||
			//(op==vmopcl[VMOPCL_APOPB])||
			//(op==vmopcl[VMOPCL_APOPBBC])||
			//(op==vmopcl[VMOPCL_APOP4B])||
			//(op==vmopcl[VMOPCL_APOP4BBC])||
			//(op==vmopcl[VMOPCL_APOPNEXTL])||
			//(op==vmopcl[VMOPCL_APOPNEXTLBC])||
			//(op==vmopcl[VMOPCL_APOPNEXTW])||
			//(op==vmopcl[VMOPCL_APOPNEXTWBC])||
			//(op==vmopcl[VMOPCL_APOPNEXTB])||
			//(op==vmopcl[VMOPCL_APOPNEXTBBC])||
			//(op==vmopcl[VMOPCL_APOPNEXT4B])||
			//(op==vmopcl[VMOPCL_APOPNEXT4BBC])||
			//(op==vmopcl[VMOPCL_APUSHL2])||
			//(op==vmopcl[VMOPCL_APUSHL2BC])||
			//(op==vmopcl[VMOPCL_APUSHL3])||
			//(op==vmopcl[VMOPCL_APUSHL3BC])||
			//(op==vmopcl[VMOPCL_APUSHL4])||
			//(op==vmopcl[VMOPCL_APUSHL4BC])||
			//(op==vmopcl[VMOPCL_APUSHNEXTL2])||
			//(op==vmopcl[VMOPCL_APUSHNEXTL2BC])||
			//(op==vmopcl[VMOPCL_APUSHNEXTL3])||
			//(op==vmopcl[VMOPCL_APUSHNEXTL3BC])||
			//(op==vmopcl[VMOPCL_APUSHNEXTL4])||
			//(op==vmopcl[VMOPCL_APUSHNEXTL4BC])||
			//(op==vmopcl[VMOPCL_APOPL2])||
			//(op==vmopcl[VMOPCL_APOPL2BC])||
			//(op==vmopcl[VMOPCL_APOPL3])||
			//(op==vmopcl[VMOPCL_APOPL3BC])||
			//(op==vmopcl[VMOPCL_APOPL4])||
			//(op==vmopcl[VMOPCL_APOPL4BC])||
			//(op==vmopcl[VMOPCL_APOPNEXTL2])||
			//(op==vmopcl[VMOPCL_APOPNEXTL2BC])||
			//(op==vmopcl[VMOPCL_APOPNEXTL3])||
			//(op==vmopcl[VMOPCL_APOPNEXTL3BC])||
			//(op==vmopcl[VMOPCL_APOPNEXTL4])||
			//(op==vmopcl[VMOPCL_APOPNEXTL4BC])||
			(op==VMOP_INCLIV)||
			(op==VMOP_DECLIV)
			)
		{
			// ---- fix local variable index (1st argument) ----
			c[i+1]=_vm->addLocalVarRef(vm_localvarnames[c[i+1]]);
		}
		else
		if(
			//(op==vmopcl[VMOPCL_APUSHL16])||
			//(op==vmopcl[VMOPCL_APUSHL16BC])||
			//(op==vmopcl[VMOPCL_APUSHW16])||
			//(op==vmopcl[VMOPCL_APUSHW16BC])||
			//(op==vmopcl[VMOPCL_APUSHB16])||
			//(op==vmopcl[VMOPCL_APUSHB16BC])||
			//(op==vmopcl[VMOPCL_APUSH4B16])||
			//(op==vmopcl[VMOPCL_APUSH4B16BC])||
			//(op==vmopcl[VMOPCL_APOPL16])||
			//(op==vmopcl[VMOPCL_APOPL16BC])||
			//(op==vmopcl[VMOPCL_APOPW16])||
			//(op==vmopcl[VMOPCL_APOPW16BC])||
			//(op==vmopcl[VMOPCL_APOPB16])||
			//(op==vmopcl[VMOPCL_APOPB16BC])||
			//(op==vmopcl[VMOPCL_APOP4B16])||
			//(op==vmopcl[VMOPCL_APOP4B16BC])||
			//(op==vmopcl[VMOPCL_APUSHL216])||
			//(op==vmopcl[VMOPCL_APUSHL216BC])||
			//(op==vmopcl[VMOPCL_APUSHL316])||
			//(op==vmopcl[VMOPCL_APUSHL316BC])||
			//(op==vmopcl[VMOPCL_APUSHL416])||
			//(op==vmopcl[VMOPCL_APUSHL416BC])||
			//(op==vmopcl[VMOPCL_APOPL216])||
			//(op==vmopcl[VMOPCL_APOPL216BC])||
			//(op==vmopcl[VMOPCL_APOPL316])||
			//(op==vmopcl[VMOPCL_APOPL316BC])||
			//(op==vmopcl[VMOPCL_APOPL416])||
			//(op==vmopcl[VMOPCL_APOPL416BC])||
			(op==VMOP_MOVLV)||
			(op==VMOP_LIVANDLIV)||
			(op==VMOP_LIVORLIV)||
			(op==VMOP_LIVEORLIV)||
			(op==VMOP_LIVMODLIV)||
			(op==VMOP_LIVADDLIV)||
			(op==VMOP_LIVSUBLIV)||
			(op==VMOP_LIVMULLIV)||
			(op==VMOP_LIVDIVLIV)||
			(op==VMOP_LIVASLLIV)||
			(op==VMOP_LIVASRLIV)||
			(op==VMOP_LFVADDLFV)||
			(op==VMOP_LFVSUBLFV)||
			(op==VMOP_LFVMULLFV)||
			(op==VMOP_LFVDIVLFV)||
			(op==VMOP_PUSHLIVANDLIV)||
			(op==VMOP_PUSHLIVORLIV)||
			(op==VMOP_PUSHLIVEORLIV)||
			(op==VMOP_PUSHLIVMODLIV)||
			(op==VMOP_PUSHLIVADDLIV)||
			(op==VMOP_PUSHLIVSUBLIV)||
			(op==VMOP_PUSHLIVMULLIV)||
			(op==VMOP_PUSHLIVDIVLIV)||
			(op==VMOP_PUSHLIVASLLIV)||
			(op==VMOP_PUSHLIVASRLIV)||
			(op==VMOP_PUSHLFVADDLFV)||
			(op==VMOP_PUSHLFVSUBLFV)||
			(op==VMOP_PUSHLFVMULLFV)||
			(op==VMOP_PUSHLFVDIVLFV)
			)
		{
			// ---- fix local variable index (2nd argument) ----
			c[i+1]=_vm->addLocalVarRef(vm_localvarnames[c[i+1]]);
			c[i+2]=_vm->addLocalVarRef(vm_localvarnames[c[i+2]]);
		}
		else
		if( 
			(op==VMOP_MOVV)||
			(op==VMOP_IVANDIV)|| 
			(op==VMOP_IVORIV)|| 
			(op==VMOP_IVEORIV)|| 
			(op==VMOP_IVMODIV)|| 
			(op==VMOP_IVADDIV)|| 
			(op==VMOP_IVSUBIV)|| 
			(op==VMOP_IVMULIV)|| 
			(op==VMOP_IVDIVIV)|| 
			(op==VMOP_IVASLIV)|| 
			(op==VMOP_IVASRIV)|| 
			(op==VMOP_FVADDFV)|| 
			(op==VMOP_FVSUBFV)|| 
			(op==VMOP_FVMULFV)|| 
			(op==VMOP_FVDIVFV)|| 
			(op==VMOP_PUSHIVANDIV)|| 
			(op==VMOP_PUSHIVORIV)|| 
			(op==VMOP_PUSHIVEORIV)|| 
			(op==VMOP_PUSHIVMODIV)|| 
			(op==VMOP_PUSHIVADDIV)|| 
			(op==VMOP_PUSHIVSUBIV)|| 
			(op==VMOP_PUSHIVMULIV)|| 
			(op==VMOP_PUSHIVDIVIV)|| 
			(op==VMOP_PUSHIVASLIV)|| 
			(op==VMOP_PUSHIVASRIV)|| 
			(op==VMOP_PUSHFVADDFV)|| 
			(op==VMOP_PUSHFVSUBFV)|| 
			(op==VMOP_PUSHFVMULFV)|| 
			(op==VMOP_PUSHFVDIVFV) 
			)
		{
			c[i+1]=_vm->addVarRef(vm_vars[c[i+1]], vm_varnames[c[i+1]]);
			c[i+2]=_vm->addVarRef(vm_vars[c[i+2]], vm_varnames[c[i+2]]);
		}
		else
		if(
			(op==VMOP_PUSHV)||
			(op==VMOP_PUSHIVINC)||
			(op==VMOP_PUSHINCIV)||
			(op==VMOP_PUSHIVDEC)||
			(op==VMOP_PUSHDECIV)||
			(op==VMOP_POPV)||
			(op==VMOP_INCIV)||
			(op==VMOP_DECIV)
			)
		{
			// ---- fix global variable index ----
			c[i+1]=_vm->addVarRef(vm_vars[c[i+1]], vm_varnames[c[i+1]]);
		}
		else
		if(
			(op==VMOP_IASEL)
			)
		{
			// ---- fix int array index ----
			if(vm_iarr[c[i+1]].ia)
			{
				// ---- regular IntArray ----
				c[i+1]=_vm->addIntArray(vm_iarr[c[i+1]].ia);
			}
		}
		else
		if(
			(op==VMOP_FASEL)
			)
		{
			// ---- fix float array index ----
			if(vm_farr[c[i+1]].fa)
			{
				// ---- regular FloatArray object ----
				c[i+1]=_vm->addFloatArray(vm_farr[c[i+1]].fa);
			}
		}
		i+=vm_opcodesz[op];
	}

	// ---- finally copy relocated bytecode into new _vm program mem ----
	for(i=0; i<vm_pc; i++)
	{
		_vm->vm_code[_vm->vm_pc++]=vm_code[i];
	}

	return next?next->compileHybridStatement(_vm):0;
}
#endif // 0
 
void VMCore::addHybridStatement(PTN_Statement *_st) { 
	pushci((sSI)_st); 
   pushEnv();
	addAPICall2((sUI)tksvm_hybridstat); 
	addOpcode(VMOP_INCSTP); // cleanup env
   addOpcode(VMOP_INCSTP); // cleanup st
} 

void VMCore::execAPICall(void) {
#ifdef DX_ARM
   sSI cso = (current_stack_offset & 7);
   ////printf("xxxx execAPICAll: current_stack_offset=%d\n", current_stack_offset);
   if(0 != cso)
   {
      // Align stack to 8-byte boundary
      addOpcode(VMOP_DECSTP);
   }
#endif // DX_ARM

	addOpcode(VMOP_APICALL);

#ifdef DX_ARM
   if(0 != cso)
   {
      // Undo stack alignment
      addOpcode(VMOP_INCSTP);
   }
#endif // DX_ARM
}
 
void VMCore::addAPICall1(sUI _name/**void(_name)(...)**/) { 
	loadfxc((sSI)_name); 
#ifndef DX_X86
	addOpcode(VMOP_BEGAPICALL1);
#endif
   execAPICall();
#ifndef DX_X86
	addOpcode(VMOP_ENDAPICALL1);
#endif
} 

void VMCore::addAPICall2(sUI _name/**void(_name)(...)**/) { 
	loadfxc((sSI)_name); 
#ifndef DX_X86
	addOpcode(VMOP_BEGAPICALL2);
#endif
   execAPICall();
#ifndef DX_X86
	addOpcode(VMOP_ENDAPICALL2);
#endif
} 

void VMCore::addAPICall3(sUI _name/**void(_name)(...)**/) { 
	loadfxc((sSI)_name); 
#ifndef DX_X86
	addOpcode(VMOP_BEGAPICALL3);
#endif
   execAPICall();
#ifndef DX_X86
	addOpcode(VMOP_ENDAPICALL3);
#endif
} 

void VMCore::loadfxc(sSI _i) { 
	addOpcode(VMOP_LOADFXC); 
	vm_code[vm_pc++] = addConstInt(_i); 
} 
 
void VMCore::pushci(sSI _i) { 
	addOpcode(VMOP_PUSHC); 
	vm_code[vm_pc++] = addConstInt(_i); 
} 
 
void VMCore::pushcf(sF32 _f) { 
	addOpcode(VMOP_PUSHC); 
	addConstFloat(_f); // also adds vm_code[vm_pc++]... 
} 
 
void VMCore::pushv(TKS_CachedObject *var) { 
	yacmemptr varptr; 
	varptr.si=&var->value.int_val; 
	addOpcode(VMOP_PUSHV); 
   //vm_code[vm_pc++]=addVarRef(varptr, var->name ? (char*)var->name->chars : NULL);
   vm_code[vm_pc++]=addVarRef(varptr, (char*)var->name.chars);
} 
 
void VMCore::objassign(sU16 _left_clid, sU16 _right_clid) { 
	switch(_left_clid) 
	{ 
	case YAC_CLID_STRING: 
	default: 
		loadfxc((sSI)&tksvm_opassign);
#ifndef DX_X86
		addOpcode(VMOP_BEGAPICALL2); 
#endif
      execAPICall();
#ifndef DX_X86
      addOpcode(VMOP_ENDAPICALL2);
#endif
		break; 
	} 
} 
 
sU8 VMCore::apush(TKS_CachedObject *cached_object) { 
	YAC_Object *o=cached_object->value.object_val; 
	if(!o) return 0xFF; 
 
	switch(o->class_ID) 
	{ 
	default: 
		{ 
			switch(o->yacArrayGetElementType()) 
			{ 
			case 1: 
				selectIntArray(cached_object); 
				addOpcode(VMOP_SIAPUSHL); 
				return 1; 
			case 2: 
				selectFloatArray(cached_object); 
				addOpcode(VMOP_SIAPUSHL); 
				return 2; 
			case 3: 
			default: 
				Dprintf("[---] internal error: no JIT code path for array object (clid=%i).\n", 
					o->class_ID); 
				Dcterror(PTNERR_CANTCOMPILE); 
				return 0xFF; 
			} 
		} 
		case YAC_CLID_INTARRAY: 
			selectIntArray(cached_object); 
			addOpcode(VMOP_SIAPUSHL); 
			return 1; 
 
		case YAC_CLID_FLOATARRAY:  
      case TKS_CLID_ENVELOPE: 
         selectFloatArray(cached_object); 
         addOpcode(VMOP_SIAPUSHL); 
         return 2; 

		case YAC_CLID_OBJECTARRAY: 
			pushci((sSI)cached_object); 
			// stack: 
			//         +0 object 
			//         +1 array index 
			addAPICall2((sUI)&tksvm_objectarrayindexexpr); 
			addOpcode(VMOP_INCSTP); // cleanup object 
         addOpcode(VMOP_SRETI);  // overwrite arrayidx with return value
			return 3; 
 
		case YAC_CLID_POINTERARRAY: 
			pushci((sSI)cached_object); 
			// stack: 
			//         +0 object 
			//         +1 array index 
			addAPICall2((sUI)&tksvm_pointerarrayindexexpr); 
			addOpcode(VMOP_INCSTP); // cleanup object 
         addOpcode(VMOP_SRETI);  // overwrite arrayidx with return value
			return 3; 
 
		case YAC_CLID_STRINGARRAY: 
			pushci((sSI)cached_object); 
			// stack: 
			//         +0 object 
			//         +1 array index 
			addAPICall2((sUI)&tksvm_stringarrayindexexpr); 
			addOpcode(VMOP_INCSTP); // cleanup object 
         addOpcode(VMOP_SRETI);  // overwrite arrayidx with return value
			return 3; 
 
		case YAC_CLID_CLASSARRAY: 
			pushci((sSI)cached_object); 
			// stack: 
			//         +0 object 
			//         +1 array index 
			addAPICall2((sUI)&tksvm_classarrayindexexpr); 
			addOpcode(VMOP_INCSTP); // cleanup object 
         addOpcode(VMOP_SRETI);  // overwrite arrayidx with return value
			return 3; 
 
	} 
} 
 
 
void VMCore::addCPPCallY(const YAC_CommandY *command, sUI _numRetObj) {

   // (note) _numRetObj is either 0 or 1 (if => call syntax is used)
	
   loadfxc((sSI)command->y_adr); 

#ifndef DX_X86
	addOpcode(VMOP_BEGAPICALL2 + _numRetObj);   // 2 for 'this' and arglist*
#endif
   execAPICall();
#ifndef DX_X86
   addOpcode(VMOP_ENDAPICALL2 + _numRetObj);
#endif

	if(0 == command->y_return_type) 
   {
		addOpcode(VMOP_INCSTP); // discard object 
   }

   // (note) always wrapped arg
	if(0 != command->y_arg_num)
   {
		addOpcode(VMOP_INCSTP); // discard arg*
   }

	sUI i; 
	for(i=0; i<command->y_arg_num; i++) 
   {
		addOpcode(VMOP_INCSTP); // discard args  (todo) use INCSTPn
   }

	switch(command->y_return_type) 
	{ 
	case 1: 
	case 3: 
		addOpcode(VMOP_SRETI); // read from eax / d0 / r0
		break; 
	case 2: 
		addOpcode(VMOP_SRETF); // read from FPU register resp. r0
		break; 
	} 
} 
 
void VMCore::addCCallY(const YAC_CommandY *command, sUI _numRetObj) { 

   // (note) _numRetObj is either 0 or 1 (if => call syntax is used)

   loadfxc((sSI)command->y_adr);

   if(command->y_callstyle < 5) // use wrapped args ? 
   {
#ifndef DX_X86
	   addOpcode(VMOP_BEGAPICALL1 + _numRetObj); 
#endif
      execAPICall();
#ifndef DX_X86
      addOpcode(VMOP_ENDAPICALL1 + _numRetObj);
#endif
   }
   else
   {
#ifndef DX_X86
	   addOpcode(VMOP_BEGAPICALL0 + command->y_arg_num + _numRetObj); 
      // (note) do not align stack on ARM (affects only builtin functions)
#endif
      addOpcode(VMOP_APICALL);
#ifndef DX_X86
      addOpcode(VMOP_ENDAPICALL0 + command->y_arg_num + _numRetObj);
#endif
   }

	if(command->y_arg_num) 
	{ 
		if(command->y_callstyle < 5) 
      {
			addOpcode(VMOP_INCSTP); // discard arglist* 
      }

		sUI i; 
		for(i=0; i<command->y_arg_num; i++) 
      {
			addOpcode(VMOP_INCSTP); // discard args  (todo) use INCSTPn
      }
	}

	switch(command->y_return_type) 
	{ 
	case 1: 
	case 3: 
		addOpcode(VMOP_SRETI); // read from eax / d0 / r0
		break; 
	case 2: 
		addOpcode(VMOP_SRETF); // read from FPU register resp. r0
		break; 
	} 
} 
  
void VMCore::resetArrayCache(void) { 
   c_array = NULL; 
} 

void VMCore::selectIntArray(TKS_CachedObject *_var) { 
	if(jit.b_allowArrayCache && (c_array==_var) ) 
		return; 
	addOpcode(VMOP_IASEL); 
	vm_code[vm_pc++]=addIntArray(_var); 
	c_array=_var; 
} 
 
void VMCore::selectFloatArray(TKS_CachedObject *_var) { 
	if(jit.b_allowArrayCache && (c_array==_var) ) 
		return; 
	addOpcode(VMOP_FASEL); 
	vm_code[vm_pc++]=addFloatArray(_var); 
	c_array=_var; 
} 
 
void VMCore::pushContext (void) {
   addOpcode(VMOP_PUSHLV);
   vm_code[vm_pc++]=TKSVM_LVAR_CONTEXT;
}

void VMCore::pushEnv(void) {
   addOpcode(VMOP_PUSHLV);
   vm_code[vm_pc++]=TKSVM_LVAR_ENV;
}





 
 
void YAC_CALL tksvm_classmemberassign_if(TKS_CachedObject *_co, sUI id, sUI _if) { 
	// stack:    
	//        +0=var 
	//        +1=member 
	//        +2=if value 
	TKS_ScriptClassInstance*o=(TKS_ScriptClassInstance*)_co->value.object_val; 
    *(sUI*)&o->member_data[(sU16)((id&0xFFF) + o->class_decl->class_relocation_table[(id>>16)&0xFFFF])] = _if; 
} 
 
void YAC_CALL tksvm_classmemberassign_stack_if(TKS_Context *_context, sUI id, sUI _if) { 
	// stack:    
	//        +0=member 
	//        +1=if value 
   // xxx TKS_MT: MUST use *real* context (JIT only works in main thread for now)
    *(sUI*)&_context->tksvm_class_stack_object->member_data[(sU16)((id&0xFFF) + _context->tksvm_class_stack_object->class_decl->class_relocation_table[(id>>16)&0xFFFF])]  
		= _if; 
} 
 
sUI YAC_CALL tksvm_classmemberexpr_if(TKS_CachedObject *_co, sUI id) { 
	// stack:    
	//        +0=object 
	//        +1=member 
	TKS_ScriptClassInstance *o=(TKS_ScriptClassInstance*)_co->value.object_val; 
   ////*(sUI*)&id= (*(sUI*)&o->member_data[(sU16)((id&0xFFF) + o->class_decl->class_relocation_table[(id>>16)&0xFFFF])]); 
   return (*(sUI*)&o->member_data[(sU16)((id&0xFFF) + o->class_decl->class_relocation_table[(id>>16)&0xFFFF])]); 
} 
 
////void tksvm_classmemberexpr_o(sUI *_st) { 
YAC_Object *YAC_CALL tksvm_classmemberexpr_o(TKS_CachedObject *_co, sUI id) { 
	// stack:    
	//        +0=object 
	//        +1=member 
	TKS_ScriptClassInstance*o=(TKS_ScriptClassInstance*)_co->value.object_val; 
   ////*(YAC_Object**)&id= (*(YAC_Object**)&o->member_data[(sU16)((id&0xFF) + o->class_decl->class_relocation_table[(id>>16)&0xFFFF]+1)]); 
   return (*(YAC_Object**)&o->member_data[(sU16)((id&0xFFF) + o->class_decl->class_relocation_table[(id>>16)&0xFFFF]+1)]); 
} 
 
sUI YAC_CALL tksvm_classmemberexpr_stack_if(TKS_Context *_context, sUI id) { 
	// stack:    
	//        +0=member 
   // xxx TKS_MT: MUST use *real* context (JIT only works in main thread for now)

   // // *(sSI*)&id= (*(sSI*)&_context->tksvm_class_stack_object->member_data[(sU16)((id&0xFF) +  
	// // 	_context->tksvm_class_stack_object->class_decl->class_relocation_table[(id>>16)&0xFFFF])]); 
   return (*(sUI*)&_context->tksvm_class_stack_object->member_data[(sU16)((id&0xFFF) +  
		_context->tksvm_class_stack_object->class_decl->class_relocation_table[(id>>16)&0xFFFF])]); 
} 
 
YAC_Object *YAC_CALL tksvm_classmemberexpr_stack_o(TKS_Context *_context, sUI id) { 
	// stack:    
	//        +0=member 
   // xxx TKS_MT: MUST use *real* context (JIT only works in main thread for now)
   
   // // *(YAC_Object**)&id= (*(YAC_Object**)&_context->tksvm_class_stack_object->member_data[(sU16)((id&0xFF) +  
   // //   _context->tksvm_class_stack_object->class_decl->class_relocation_table[(id>>16)&0xFFFF]+1)]); 
   return (*(YAC_Object**)&_context->tksvm_class_stack_object->member_data[(sU16)((id&0xFFF) +  
	   _context->tksvm_class_stack_object->class_decl->class_relocation_table[(id>>16)&0xFFFF]+1)]); 
} 







// // void YAC_CALL tksvm_unsetvo(sUI _del, sUI _type, YAC_Object *_o) { 
// // 	// ---- unset variable object ---- 
// // 	// ---- stack: 
// // 	//             +2: value 
// // 	//             +1: type 
// // 	//             +0: deleteme 
// // 	if((_type>2)&&_del) 
// // 	{ 
// // 		delete _o; 
// // 		_o=0; 
// // 	} 
// // 	// ---- increase stack by +2 ---- 
// // } 
 
void YAC_CALL tksvm_opassign(YAC_Object *_this, YAC_Object *_o) { 
	// ---- generic object assign , never returns anything ---- 
	// ---- stack: 
	//             +1: Object *_o 
	//             +0: Object *_this 
	_this->yacOperatorAssign(_o); 
} 
 
// only called in expressions 
sUI YAC_CALL tksvm_operator(sSI _cmd, YAC_Object *_right, YAC_Object *_left) { 
	// ---- generic object operator ---- 
	// ---- stack: 
	//             +2: left Object 
	//             +1: right Object 
	//             +0: operator (TKSOP_, YAC_OP_) 
	if(_left)
	{
      YAC_Value r;
		_left->yacOperator(_cmd, _right, &r);////&tksvm_env.cret); 
		////*(sSI*)&_left = r.value.int_val;////tksvm_env.cret.value.int_val; 
      // (todo) assumes that yacOperator() does not return new/deletable object
      return *(sUI*)&r.value.int_val;
	} 
	else  
	{ 
		switch(_cmd) 
		{ 
      //case YAC_OP_CEQ:*(sSI*)&_left=((sUI)_left)==((sUI)_right);break;  
		//case YAC_OP_CNE:*(sSI*)&_left=((sUI)_left)!=((sUI)_right);break;  
         case YAC_OP_CEQ: 
            if(_right) 
            { 
               if(_right->class_ID==YAC_CLID_OBJECT)
               {
                  ////*(sSI*)&_left=1;
                  return 1;
               }
               else 
               {
                  ////*(sSI*)&_left=0;
                  return 0;
               }
            } 
            else
            {
               ////*(sSI*)&_left=1;
               return 1;
            }
            break;
      case YAC_OP_CNE:
         if(_right)
         {
            if(_right->class_ID==YAC_CLID_OBJECT)
            {
               ////*(sSI*)&_left=0;
               return 0;
            }
            else
            {
               ////*(sSI*)&_left=1;
               return 1;
            }
         }
         else
         {
            ////*(sSI*)&_left=0;
            return 0;
         }
         break;
		}
	}

   return (sUI)_left; // (todo) is this actually used?
} 
  
// ---- returns int, only used for comparisons
sSI YAC_CALL tksvm_operatori(sSI _cmd, sSI _right, YAC_Object *_left) {  
	// ---- generic object operator ----  
	// ---- stack:  
	//             +2: left Object  
	//             +1: right Object  
	//             +0: operator (TKSOP_, YAC_OP_)  
	if(_left)  
	{  
      YAC_Value r;
		_left->yacOperatorI(_cmd, _right, &r);////&tksvm_env.cret);  
		////*(sSI*)&_left = r.value.int_val;////tksvm_env.cret.value.int_val;
      return r.value.int_val;
	}  
	else   
	{  
		switch(_cmd)  
		{  
         case YAC_OP_CEQ:
            ////*(sSI*)&_left=((sUI)_left)==((sUI)_right);
            return ((sUI)_left)==((sUI)_right);
         case YAC_OP_CNE:
            ////*(sSI*)&_left=((sUI)_left)!=((sUI)_right);
            return ((sUI)_left)!=((sUI)_right);
		}  
	}
   return (sSI)_left; // (todo) is this actually used? (most likely not)
}  
 
// returns int (only used for comparisons) 
sUI YAC_CALL tksvm_operatorf(sSI _cmd, sF32 _right, YAC_Object *_left) {  
	// ---- generic object operator ----  
	// ---- stack:  
	//             +2: left Object  
	//             +1: right Object  
	//             +0: operator (TKSOP_, YAC_OP_)  
	if(_left)  
	{  
      YAC_Value r;
		_left->yacOperatorF32(_cmd, _right, &r);////&tksvm_env.cret);  
		////*(sSI*)&_left = r.value.int_val;////tksvm_env.cret.value.int_val;  
      return *(sUI*)&r.value.int_val;
	}  
	else   
	{  
      ////*(sSI*)&_left=0;
      return 0;
	}
}  
 
void YAC_CALL tksvm_pointerassign(TKS_CachedObject *co, YAC_Object *_o) { 
	// ---- stack: 
	//             +0 TKS_CachedObject *var 
	//             +1 Object *o 
	co->unsetFast(); 
	co->deleteme=0; 
	////co->flags=0; 

   if(_o) 
   {
      if(_o->class_ID==YAC_CLID_OBJECT) 
      {
         _o = NULL; // replace 'null' Object ptr with 0/NULL
      }
   }

	co->value.object_val = _o; 
	// ---- increase stack by +2 ---- 
} 
           
static YAC_Value*tksvm_local_co; 
void YAC_CALL tksvm_pointerassign_local(TKS_Context *_context, TKS_CachedObject *co, YAC_Object *_o) { 
	// ---- stack: 
	//             +0 TKS_CachedObject *var 
	//             +1 Object *o 
   // xxx TKS_MT: use *real* context
   // xxx the context should be passed as an argument to tksvm_hybridstat()
	tksvm_local_co=&_context->tks_fstackc[co->local_index]; 
	tksvm_local_co->unsetFast(); 
	tksvm_local_co->deleteme=0; 
	//co->flags=0;  

   if(_o)
   {
      if(_o->class_ID==YAC_CLID_OBJECT)
      {
         _o = NULL;  // replace 'null' Object ptr with 0/NULL
      }
   }

	tksvm_local_co->value.object_val = _o; 
	// ---- increase stack by +3 ---- 
} 
 
void YAC_CALL tksvm_delete(TKS_CachedObject *co) { 
	// ---- stack: 
	//             +0 TKS_CachedObject *var 
	co->unsetFast(); 
	co->deleteme=0;
   co->value.object_val = NULL;  
	////delPointer(); 
	// ---- increase stack by +1 ---- 
} 
 
void YAC_CALL tksvm_delete_local(TKS_Context *_context, TKS_CachedObject *co) { 
	// ---- stack: 
	//             +0 TKS_CachedObject *var 
   // xxx TKS_MT: use *real* context
   // xxx the context should be passed as an argument to tksvm_hybridstat()
	tksvm_local_co=&_context->tks_fstackc[co->local_index]; 
	tksvm_local_co->unsetFast(); 
	tksvm_local_co->deleteme=0;  
	tksvm_local_co->value.object_val=0;  
	//delPointer(); 
	// ---- increase stack by +2 ---- 
} 
 

void YAC_CALL tksvm_hybridstat(PTN_Env *_env, PTN_Statement *_st) { 
	// ---- stack: 
   //             +0 env**
	//             +1 statement* 
   // xxx TODO: the context should be passed as an argument to tksvm_hybridstat() (currently native code needs to get
   //           the context using TLS. however, most methods will not require the context, it is mostly used to raise
   //           exceptions from within native code
   //
	_st->eval(_env); 
	// ---- increase stack by +2 ---- 
} 
 
sS32 YAC_CALL tksvm_irnd(sS32 _val) {
	// ---- stack: 
	//             +0 max/int target 
	////_val=(sSI) ((rand()*(1.0f/((sF32)RAND_MAX))*_val)); 
   if(_val)
   {
      return (rand() % _val); 
   }
   else
   {
      return 0;
   }
} 
 
sF32 YAC_CALL tksvm_frnd(sF32 _val) {
	// ---- stack: 
	//             +0 max/float target 
	return (sF32) ((rand()*(0.999999999f/((sF32)RAND_MAX))*_val)); 
} 
 
YAC_Object *YAC_CALL tksvm_pointerarrayindexexpr(TKS_CachedObject *_co, sUI _idx) { 
	// stack: 
	//         +0 object 
	//         +1 array index 
	////(*(YAC_Object**)&_idx)=((YAC_PointerArray*)_co->value.object_val)->elements[_idx].value.object_val; 
   return ((YAC_PointerArray*)_co->value.object_val)->elements[_idx].value.object_val; 
} 
 
YAC_String *YAC_CALL tksvm_stringarrayindexexpr(TKS_CachedObject *_co, sUI _idx) { 
	// stack: 
	//         +0 object 
	//         +1 array index 
	////(*(YAC_String**)&_idx)=&((YAC_StringArray*)_co->value.object_val)->elements[_idx]; 
   return &((YAC_StringArray*)_co->value.object_val)->elements[_idx]; 
} 
 
YAC_Object *YAC_CALL tksvm_objectarrayindexexpr(TKS_CachedObject *_co, sUI _idx) { 
	// stack: 
	//         +0 object 
	//         +1 array index 
	////(*(YAC_Object**)&_idx)=((YAC_ObjectArray*)_co->value.object_val)->elements[_idx]; 
   return ((YAC_ObjectArray*)_co->value.object_val)->elements[_idx]; 
} 
 
YAC_Object *YAC_CALL tksvm_classarrayindexexpr(TKS_CachedObject *_co, sUI _idx) { 
	// stack: 
	//         +0 object 
	//         +1 array index 
   ////(*(YAC_Object**)&_idx)=((YAC_ClassArray*)_co->value.object_val)->elements[_idx]; 
	return (YAC_Object*) ((YAC_ClassArray*)_co->value.object_val)->elements[_idx]; 
} 
 
 
//#define DX_JIT_TRACKARRAYELEMENTS  see tks-config.h
 
/*void tksvm_vectorarrayindexassign(TKS_CachedObject *_co, sUI idx, TKSVector *s) { 
	// stack: 
	//         +0 var 
	//         +1 array int index 
	//         +2 rvalue (Vector) 
	TKSVectorArray *va=(TKSVectorArray*)_co->pointer.object; 
#ifdef DX_JIT_TRACKARRAYELEMENTS 
	if(idx>va->num_elements) va->num_elements=idx+1; // use counter 
#endif 
	TKSVector *d=&va->elements[idx]; 
	d->x=s->x; d->y=s->y; d->z=s->z; 
}*/ 
 
//void tksvm_pointerarrayindexassign(sUI *_st) { 
void YAC_CALL tksvm_pointerarrayindexassign(TKS_CachedObject *_co, sUI idx, YAC_Object *o) { 
	// stack: 
	//         +0 var 
	//         +1 array int index 
	//         +2 rvalue (Object) 
	YAC_PointerArray *pa=(YAC_PointerArray*)_co->value.object_val; 
#ifdef DX_JIT_TRACKARRAYELEMENTS 
	if(idx>pa->num_elements) pa->num_elements=idx+1; // use counter 
#endif 
	YAC_Value *d=&pa->elements[idx]; 
	d->unsetFast(); d->initObject(o, 0); // xxx fix string type 
} 
 
void YAC_CALL tksvm_stringarrayindexassign(TKS_CachedObject *_co, sUI idx, YAC_String *s) { 
	// stack: 
	//         +0 var 
	//         +1 array int index 
	//         +2 rvalue (String) 
	YAC_StringArray *sa=(YAC_StringArray*)_co->value.object_val; 
#ifdef DX_JIT_TRACKARRAYELEMENTS 
	if(idx>sa->num_elements) sa->num_elements=idx+1; // use counter 
#endif 
	sa->elements[idx].yacCopy(s); 
} 
 
void YAC_CALL tksvm_objectarrayindexassign(TKS_CachedObject *_co, sUI idx, YAC_Object *o) { 
	// stack: 
	//         +0 var 
	//         +1 array int index 
	//         +2 rvalue (Vector) 
	YAC_ObjectArray *oa=(YAC_ObjectArray*)_co->value.object_val; 
#ifdef DX_JIT_TRACKARRAYELEMENTS 
	if(idx>oa->num_elements) oa->num_elements=idx+1; // use counter 
#endif 
	oa->elements[idx]->yacOperatorAssign(o); 
} 
 
void YAC_CALL tksvm_classarrayindexassign(TKS_CachedObject *_co, sUI idx, TKS_ScriptClassInstance *cc) { 
	// stack: 
	//         +0 var 
	//         +1 array int index 
	//         +2 rvalue (Vector) 
	YAC_ClassArray*ca=(YAC_ClassArray*)_co->value.object_val; 
#ifdef DX_JIT_TRACKARRAYELEMENTS 
	if(idx>ca->num_elements) ca->num_elements=idx+1; // use counter 
#endif 
	TKS_ScriptClassInstance *d=ca->elements[idx]; 
	d->yacOperatorAssign(cc); 
} 
 
////extern void *YAC_CALL tksvm_get_ipointer (void**_v) {
////   ////// stack:
////   ///////         +0 void**
////   /////*(void**)&_v = *_v;
////   return *_v;
////}

////#ifdef TKS_MT
////extern void YAC_CALL tksvm_unlock_mutex (TKS_Mutex *_mutex) {
////   _mutex->unlock();
////}
////#endif


#else // no TKS_JIT

// ---- begin STUB ------------------------------------------

#include "tks_inc_class.h"
#include "VMCore.h"

VMCore::VMCore(void) {
   parser_tree = 0;
}

VMCore::~VMCore() {
   if(parser_tree)
   {
      delete parser_tree; parser_tree=0;
   }
}

sUI     VMCore::getID             (void)  const {
	return PTN_FOR; // make parser not require ";" at end of statement
}

void    VMCore::eval              (PTN_Env *_env) const {
   parser_tree->evalFirst(_env);
}

static void VMCore__eval(PTN_Env *_env, const PTN_Statement *_st) { 
   Dtracest;
	VMCore *st=(VMCore *)_st; 

   st->parser_tree->evalFirst(_env);
}


Fevalst VMCore::getEvalSt         (void) const {
   return VMCore__eval;
}

sBool   VMCore::semanticCheck     (void) {
   return (parser_tree ? parser_tree->semanticCheckAll() : 1);
}

sBool   VMCore::resolveXRef       (void) {
   return (parser_tree ? parser_tree->resolveXRefAll() : 1);
}

void    VMCore::optimize          (void) {
   if(parser_tree)
   {
      parser_tree->optimizeAll();
   }
}

void    VMCore::subGenCallList    (void) {
	if(parser_tree)
   {
		parser_tree->genCallList();
   }
}



// ---- end STUB --------------------------------------------

#endif // TKS_JIT 
 
