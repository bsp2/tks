/// vmcore.h 
/// 
/// (c) 2002-2014 Bastian Spiegel <bs@tkscript.de> 
///     - distributed under terms of the GNU general public license (GPL). 
/// 
#ifndef __TKS_VMCORE_H___
#define __TKS_VMCORE_H___

#ifdef TKS_JIT

#include "vmopnum.h"

//#define VM_CONAMECHARS cached_object->name ? (char*)cached_object->name->chars : NULL
//#define VM_VARNAMECHARS var->name ? (char*)var->name->chars : NULL
#define VM_CONAMECHARS (char*)cached_object->name.chars
#define VM_VARNAMECHARS (char*)var->name.chars

class VMIntArray {
public:
   yacmemptr         adr;
   sUI               maskb;         // num_elements*4
   sUI              *num_elements;
   sS32            **iadr;          // unused?!
   TKS_CachedObject *ia;
};

class VMFloatArray {
public:
   yacmemptr         adr;
   sUI               mask;          // height*max_elements*4
   sUI              *num_elements;
   sF32            **iadr;          // unused?!
   TKS_CachedObject *fa;
};

extern sU16 vm_sz_startup;
extern sU16 vm_sz_cleanup;
extern sU8  vm_opcodesz[VM_NUMOPCODES];
extern sUI  vm86_off[VM_NUMOPCODES]; //-------- VM JIT code offset table --------
extern sU16 vm86_sz[VM_NUMOPCODES]; //-------- VM JIT code size table --------
extern sUI  vm86del;

// Limits per compile() block:
#define TKS_VMCORE_NUMARRAYS   32
#define TKS_VMCORE_NUMVARS    128 
#define TKS_VMCORE_NUMLVARS    64 
#define TKS_VMCORE_NUMCONSTS  256


#ifdef DX_X86
extern void VMCore_CPUTable(void *_context, void *_env);
#endif


class VMCore : public PTN_Statement {
public:
   sBool          vm_constused[TKS_VMCORE_NUMCONSTS];
   yacmem         vm_const[TKS_VMCORE_NUMCONSTS];
   sU16          *vm_code;
   sUI            vm_pc; // during assembly
   VMIntArray     vm_iarr[TKS_VMCORE_NUMARRAYS];
   VMFloatArray   vm_farr[TKS_VMCORE_NUMARRAYS];
   yacmemptr      vm_vars[TKS_VMCORE_NUMVARS];
   sBool          use_arrays;
   const char    *vm_varnames[TKS_VMCORE_NUMVARS];
   const char    *vm_localvarnames[TKS_VMCORE_NUMLVARS]; 
   
   PTN_Statement     *parser_tree; 
   TKS_CachedObject *c_array; // current array var

   // (note) The context and vm_env fields are used as a (very clumsy) way to
   //         pass the actual context/env pointers to the JIT code.
   ////static TKS_Context   *context; 
   ////static PTN_Env       *vm_env; 

#ifdef TKS_MT
   ////static TKS_Mutex    *mutex; // TKS_MT: JIT prologue code is protected by mutex
   ////static sSI vmcore__objectcounter;
#endif

#ifdef DX_ARM
   sSI current_stack_offset; // used to align stack on ARM CPUs
#endif

   struct _jit {
      sU8 *code;            // Must be aligned to page size (for mprotect())
      sU8 *nonaligned_code; // Allocate pointer, not necessarily aligned to page size..
      sUI  sz_code;         // Actual code size (without alignment justifications)
      sUI  sz_aligned;      // Alloc size (including alignment)
      sBool b_allowArrayCache; 
   } jit;

  protected:
#ifdef DX_X86
   sU8 *fixInstructionX86(sU16 *c, sU8 *s, sU8 *d);
#endif

#ifdef DX_68K
   sU8 *fixInstruction68K(sU16 *c, sU8 *s, sU8 *d);
#endif

#ifdef DX_ARM
   sU8 *fixInstructionARM(sU16 *c, sU32 *s, sU8 *d, sUI i);
#endif

   void execAPICall(void);
   
   
   public:
      VMCore(void);
      ~VMCore();
      
       sUI     getID             (void) const;
       void    prepareCompile    (void);
       void    finishCompile     (void);
       void    eval              (PTN_Env *_env) const; 
       Fevalst getEvalSt         (void) const;
       void    addOpcode         (sU16 _opcodeid); 
       sU16    addConstInt       (sS32); 
       void    addConstFloat     (sF32);
       sU16    addVarRef         (yacmemptr _d, const char *);
       sU16    addLocalVarRef    (const char *);
       sU16    addIntArray       (TKS_CachedObject *);
       sU16    addFloatArray     (TKS_CachedObject *);
static void    InitTables        (void);	
       void    clean             (void); 
       void    cleanJIT          (void);
       void    typecastStack     (sU8 _from, sU8 _to); // _from may be 4 (= variable )
       void    disassemble       (YAC_String *_s, sU16 _pc, sU16 _epc);
static void    ScanCPUTable      (void);
       sU32    estimateJITSize   (void);
       sBool   compileThis       (void);
       sBool   createJITCode     (void);

       sBool   semanticCheck     (void); 
       void    subGenCallList    (void);
       sBool   resolveXRef       (void);
       void    optimize          (void); 

       void    addHybridStatement(PTN_Statement *); 
       void    addAPICall1       (sUI _fadr/**void(_name(...))**/); 
       void    addAPICall2       (sUI _fadr/**void(_name(...))**/); 
       void    addAPICall3       (sUI _fadr/**void(_name(...))**/); 
       void    addCPPCallY       (const YAC_CommandY*, sUI _numRetObj); 
       void    addCCallY         (const YAC_CommandY*, sUI _numRetObj); 
       void    loadfxc           (sS32); 
       void    pushci            (sS32); 
       void    pushcf            (sF32); 
       void    pushv             (TKS_CachedObject*); 
       sU8     apush             (TKS_CachedObject*); 
       void    objassign         (sU16 _left_clid, sU16 _right_clid); // may use speedpaths 
       void    selectIntArray    (TKS_CachedObject*); 
       void    selectFloatArray  (TKS_CachedObject*);  
       void    resetArrayCache   (void); // called by loop/do/while/for
       void    pushContext       (void);
       void    pushEnv           (void);
};

#if 0
/** will be reallocated into the "compile" VMCore node **/
class VMCoreInline : public VMCore {
public:
   sU8 compile(VMCore *); // relocate code
}; 
#endif // 0

#else // no TKS_JIT

// Stub class for pure interpreter builds
//
class VMCore : public PTN_Statement {
public:
   PTN_Statement *parser_tree; 
   
public:
   VMCore(void);
   ~VMCore();
   
   sUI     getID             (void) const;
   void    eval              (PTN_Env *_env) const;
   Fevalst getEvalSt         (void) const;
   sBool   semanticCheck     (void); 
   void    subGenCallList    (void);
   sBool   resolveXRef       (void);
   void    optimize          (void); 
};

#endif // TKS_JIT

#endif 
