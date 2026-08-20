/// PTN_BreakableStatement.h
///
/// (c) 2014-2026 Bastian Spiegel <bs@tkscript.de>
///     - distributed under terms of the Lesser GNU General Public License (LGPL)
///

#ifndef PTN_BREAKABLESTATEMENT_H__
#define PTN_BREAKABLESTATEMENT_H__


class PTN_BreakableStatement : public PTN_Statement {
#ifdef TKS_JIT
   sU16 **vmpc_branch_fixups; // addr of 'break' jump to loop end PC val. alloc'd when adding first fixup-PC
   sUI num_vmpc_branch_fixups;
#endif // TKS_JIT

public:
   PTN_BreakableStatement(void);
   ~PTN_BreakableStatement();

   void jitAddBreakPC  (sU16 *_d);
   void jitFixBreakPCs (sU16 _vmpcLoopEnd);
};

#endif // PTN_BREAKABLESTATEMENT_H__
