void VMCore_CPUTable(void *_context, void *_env) { int i1,i2,i3,i4,i5,i6,i7,i8,i9,i10,i11,i12,i13,i14,i15,i16,i17,i18,i19,i20,i21,i22,i23,i24,i25,i26,i27,i28,i29,i30,i31,i32,i33,i34,i35,i36,i37,i38,i39,i40,i41,i42,i43,i44,i45,i46,i47,i48,i49,i50,i51,i52,i53,i54,i55,i56,i57,i58,i59,i60,i61,i62,i63,i64;
__asm mov dword ptr[ebp-0x1c], 0
__asm { mov eax, dword ptr [_context]
 mov dword ptr [ebp-20h], eax
 mov eax, dword ptr [_env]
 mov dword ptr [ebp-24h], eax }
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
  __asm {
mov eax, 0xC0DEC0DE
}
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
__asm {
  pop eax
  cmp eax, 0
  jne sitestz_l1
  mov eax, 0xC0DEC0DE
sitestz_l1:
}
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
__asm {
  cmp dword ptr[esp+0], 0
  jne sitestzp_l1
  mov eax, 0xC0DEC0DE
sitestzp_l1:
}
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
__asm {
  mov eax, 0xCA77C0DE
}
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
__asm {
ret
}
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
__asm {
  dec dword ptr[esp]
  je siloop_l1
  mov eax, 0xC0DEC0DE
siloop_l1:
}
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
__asm {
   push dword ptr[ebp-0xC0FFC7C7]
}
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
__asm {
   push dword ptr[ebp-0xC0FFC7C7]
   inc dword ptr[ebp-0xC0FFC7C7]
}
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
__asm {
   inc dword ptr[ebp-0xC0FFC7C7]
   push dword ptr[ebp-0xC0FFC7C7]
}
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
__asm {
   push dword ptr[ebp-0xC0FFC7C7]
   dec dword ptr[ebp-0xC0FFC7C7]
}
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
__asm {
   dec dword ptr[ebp-0xC0FFC7C7]
   push dword ptr[ebp-0xC0FFC7C7]
}
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
__asm {
   pop dword ptr[ebp-0xC0FFC7C7]
}
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
__asm {
   _emit 0xFF
   _emit 0x35
   _emit 0xC4
   _emit 0xC4
   _emit 0xFF
   _emit 0xC0
}
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
__asm {
   mov eax, dword ptr[0xC0FFC4C4]
   push dword ptr[eax]
   inc dword ptr[eax]
}
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
__asm {
   mov eax, dword ptr[0xC0FFC4C4]
   inc dword ptr[eax]
   push dword ptr[eax]
}
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
__asm {
   mov eax, dword ptr[0xC0FFC4C4]
   push dword ptr[eax]
   dec dword ptr[eax]
}
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
__asm {
   mov eax, dword ptr[0xC0FFC4C4]
   dec dword ptr[eax]
   push dword ptr[eax]
}
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
__asm {
   _emit 0x8f
   _emit 0x05
   _emit 0xC4
   _emit 0xC4
   _emit 0xFF
   _emit 0xC0
}
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
__asm {
   mov eax, 0xC0FFC0FF
   push eax
}
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
__asm {
   mov eax, 0xC0FFC0FF
}
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
__asm {
   mov eax, dword ptr[ebp-0xC0FFC7C8]
   mov dword ptr[ebp-0xC0FFC7C7], eax
}
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
__asm {
   mov eax, 0xC0FFC0FE
   mov dword ptr[ebp-0xC0FFC7C7], eax
}
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
__asm {
   _emit 0xA1
   _emit 0xC5
   _emit 0xc4
   _emit 0xFF
   _emit 0xC0
   _emit 0xA3
   _emit 0xC4
   _emit 0xC4
   _emit 0xFF
   _emit 0xC0
}
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
__asm {
   _emit 0xc7
   _emit 0x05
   _emit 0xC4
   _emit 0xC4
   _emit 0xFF
   _emit 0xC0
   _emit 0xFE
   _emit 0xC0
   _emit 0xFF
   _emit 0xC0
}
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
__asm {
   inc dword ptr[ebp-0xC0FFC7C7]
}
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
__asm {
   dec dword ptr[ebp-0xC0FFC7C7]
}
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
__asm {
   _emit 0xFF
   _emit 0x05
   _emit 0xC4
   _emit 0xC4
   _emit 0xFF
   _emit 0xC0
}
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
__asm {
   _emit 0xFF
   _emit 0x0D
   _emit 0xC4
   _emit 0xC4
   _emit 0xFF
   _emit 0xC0
}
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
__asm {
   mov eax, dword ptr[ebp-0xC0FFC7C8]
   and dword ptr[ebp-0xC0FFC7C7], eax
}
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
__asm {
   mov eax, dword ptr[ebp-0xC0FFC7C8]
   or dword ptr[ebp-0xC0FFC7C7], eax
}
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
__asm {
   mov eax, dword ptr[ebp-0xC0FFC7C8]
   xor dword ptr[ebp-0xC0FFC7C7], eax
}
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
__asm {
   mov ecx, dword ptr[ebp-0xC0FFC7C8]
   mov eax, dword ptr[ebp-0xC0FFC7C7]
   cdq
   idiv ecx
   mov dword ptr[ebp-0xC0FFC7C7], edx
}
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
__asm {
   mov eax, dword ptr[ebp-0xC0FFC7C8]
   add dword ptr[ebp-0xC0FFC7C7], eax
}
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
__asm {
   mov eax, dword ptr[ebp-0xC0FFC7C8]
   sub dword ptr[ebp-0xC0FFC7C7], eax
}
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
__asm {
   mov eax, dword ptr[ebp-0xC0FFC7C7]
   imul eax, dword ptr[ebp-0xC0FFC7C8]
   mov dword ptr[ebp-0xC0FFC7C7], eax
}
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
__asm {
   mov eax, dword ptr[ebp-0xC0FFC7C7]
   cdq
   mov ecx, dword ptr[ebp-0xC0FFC7C8]
   idiv ecx
   mov dword ptr[ebp-0xC0FFC7C7], eax
}
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
__asm {
   mov eax, dword ptr[ebp-0xC0FFC7C7]
   mov ecx, dword ptr[ebp-0xC0FFC7C8]
   sal eax, cl
   mov dword ptr[ebp-0xC0FFC7C7], eax
}
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
__asm {
   mov eax, dword ptr[ebp-0xC0FFC7C7]
   mov ecx, dword ptr[ebp-0xC0FFC7C8]
   sar eax, cl
   mov dword ptr[ebp-0xC0FFC7C7], eax
}
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
__asm {
   mov eax, dword ptr[ebp-0xC0FFC7C7]
   and eax, dword ptr[ebp-0xC0FFC7C8]
   push eax
}
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
__asm {
   mov eax, dword ptr[ebp-0xC0FFC7C7]
   or eax, dword ptr[ebp-0xC0FFC7C8]
   push eax
}
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
__asm {
   mov eax, dword ptr[ebp-0xC0FFC7C7]
   xor eax, dword ptr[ebp-0xC0FFC7C8]
   push eax
}
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
__asm {
   mov ecx, dword ptr[ebp-0xC0FFC7C8]
   mov eax, dword ptr[ebp-0xC0FFC7C7]
   cdq
   idiv ecx
   push edx
}
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
__asm {
   mov eax, dword ptr[ebp-0xC0FFC7C7]
   add eax, dword ptr[ebp-0xC0FFC7C8]
   push eax
}
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
__asm {
   mov eax, dword ptr[ebp-0xC0FFC7C7]
   sub eax, dword ptr[ebp-0xC0FFC7C8]
   push eax
}
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
__asm {
   mov eax, dword ptr[ebp-0xC0FFC7C7]
   imul eax, dword ptr[ebp-0xC0FFC7C8]
   push eax
}
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
__asm {
   mov eax, dword ptr[ebp-0xC0FFC7C7]
   cdq
   mov ecx, dword ptr[ebp-0xC0FFC7C8]
   idiv ecx
   push eax
}
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
__asm {
   mov eax, dword ptr[ebp-0xC0FFC7C7]
   mov ecx, dword ptr[ebp-0xC0FFC7C8]
   sal eax, cl
   push eax
}
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
__asm {
   mov eax, dword ptr[ebp-0xC0FFC7C7]
   mov ecx, dword ptr[ebp-0xC0FFC7C8]
   sar eax, cl
   push eax
}
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
__asm {
   mov eax, dword ptr[ebp-0xC0FFC7C7]
   and eax, 0xC0FFC0FE
   push eax
}
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
__asm {
   mov eax, dword ptr[ebp-0xC0FFC7C7]
   or eax, 0xC0FFC0FE
   push eax
}
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
__asm {
   mov eax, dword ptr[ebp-0xC0FFC7C7]
   xor eax, 0xC0FFC0FE
   push eax
}
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
__asm {
   mov ecx, 0xC0FFC0FE
   mov eax, dword ptr[ebp-0xC0FFC7C7]
   cdq
   idiv ecx
   push edx
}
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
__asm {
   mov eax, dword ptr[ebp-0xC0FFC7C7]
   add eax, 0xC0FFC0FE
   push eax
}
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
__asm {
   mov eax, dword ptr[ebp-0xC0FFC7C7]
   sub eax, 0xC0FFC0FE
   push eax
}
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
__asm {
   mov eax, dword ptr[ebp-0xC0FFC7C7]
   imul eax, 0xC0FFC0FE
   push eax
}
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
__asm {
   mov eax, dword ptr[ebp-0xC0FFC7C7]
   cdq
   mov ecx, 0xC0FFC0FE
   idiv ecx
   push eax
}
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
__asm {
   mov eax, dword ptr[ebp-0xC0FFC7C7]
   mov ecx, 0xC0FFC0FE
   sal eax, cl
   push eax
}
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
__asm {
   mov eax, dword ptr[ebp-0xC0FFC7C7]
   mov ecx, 0xC0FFC0FE
   sar eax, cl
   push eax
}
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
__asm {
   fld dword ptr[ebp-0xC0FFC7C7]
   fadd dword ptr[ebp-0xC0FFC7C8]
   fstp dword ptr[ebp-0xC0FFC7C7]
}
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
__asm {
   fld dword ptr[ebp-0xC0FFC7C7]
   fsub dword ptr[ebp-0xC0FFC7C8]
   fstp dword ptr[ebp-0xC0FFC7C7]
}
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
__asm {
   fld dword ptr[ebp-0xC0FFC7C7]
   fmul dword ptr[ebp-0xC0FFC7C8]
   fstp dword ptr[ebp-0xC0FFC7C7]
}
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
__asm {
   fld dword ptr[ebp-0xC0FFC7C7]
   fdiv dword ptr[ebp-0xC0FFC7C8]
   fstp dword ptr[ebp-0xC0FFC7C7]
}
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
__asm {
   fld dword ptr[ebp-0xC0FFC7C7]
   sub esp, 4
   fadd dword ptr[ebp-0xC0FFC7C8]
   fstp dword ptr[esp]
}
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
__asm {
   fld dword ptr[ebp-0xC0FFC7C7]
   sub esp, 4
   fsub dword ptr[ebp-0xC0FFC7C8]
   fstp dword ptr[esp]
}
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
__asm {
   fld dword ptr[ebp-0xC0FFC7C7]
   sub esp, 4
   fmul dword ptr[ebp-0xC0FFC7C8]
   fstp dword ptr[esp]
}
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
__asm {
   fld dword ptr[ebp-0xC0FFC7C7]
   sub esp, 4
   fdiv dword ptr[ebp-0xC0FFC7C8]
   fstp dword ptr[esp]
}
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
__asm {
   fld dword ptr[ebp-0xC0FFC7C7]
   sub esp, 4
   mov dword ptr[ebp-4], 0xC0FFC0FE
   fadd dword ptr[ebp-4]
   fstp dword ptr[esp]
}
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
__asm {
   fld dword ptr[ebp-0xC0FFC7C7]
   sub esp, 4
   mov dword ptr[ebp-4], 0xC0FFC0FE
   fsub dword ptr[ebp-4]
   fstp dword ptr[esp]
}
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
__asm {
   fld dword ptr[ebp-0xC0FFC7C7]
   sub esp, 4
   mov dword ptr[ebp-4], 0xC0FFC0FE
   fmul dword ptr[ebp-4]
   fstp dword ptr[esp]
}
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
__asm {
   fld dword ptr[ebp-0xC0FFC7C7]
   sub esp, 4
   mov dword ptr[ebp-4], 0xC0FFC0FE
   fdiv dword ptr[ebp-4]
   fstp dword ptr[esp]
}
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
__asm {
   pop eax
   cmp eax, 0
   jne sinot_l1
   mov eax, 1
   jmp sinot_l2
sinot_l1:
   xor eax, eax
sinot_l2:
   push eax
}
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
__asm {
   pop eax
   not eax
   push eax
}
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
__asm {
   pop eax
   neg eax
   push eax
}
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
__asm {
   pop eax
   imul eax, eax
   push eax
}
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
__asm {
   xor ecx, ecx
   pop eax
   cmp eax, ecx
   setnz ecx
   push ecx
}
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
__asm {
   xor ecx, ecx
   mov eax, dword ptr[esp+4]
   cmp eax, ecx
   setnz ecx
   mov dword ptr[esp+4], ecx
}
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
__asm {
   mov eax, dword ptr [esp+0]
   xor eax, 80000000h
   mov dword ptr [esp+0], eax
}
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
__asm {
   fld dword ptr [esp]
   fmul dword ptr [esp]
   fstp dword ptr [esp]
}
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
__asm {
   fld dword ptr [esp]
   fsin
   fstp dword ptr [esp]
}
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
__asm {
   fld dword ptr [esp]
   fcos
   fstp dword ptr [esp]
}
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
__asm {
   fld dword ptr [esp]
   fptan
   fdivrp st(1),st
   fstp dword ptr [esp]
}
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
__asm {
   fld dword ptr [esp]
   fsqrt
   fstp dword ptr[esp]
}
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
__asm {
  pop eax
  mov edx, eax
  sar edx, 31
  xor eax, edx
  sub eax, edx
  push eax
}
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
__asm {
  fld dword ptr [esp]
  fabs
  fstp dword ptr [esp]
}
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
__asm {
   fld dword ptr[esp]
   fst st(1)
   frndint
   fsub
   fstp dword ptr[esp]
}
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
__asm {
   fld dword ptr[esp]
   frndint
   fstp dword ptr[esp]
}
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
__asm {
   pop eax
   and eax, dword ptr[esp]
   mov dword ptr[esp], eax
}
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
__asm {
   pop eax
   or eax, dword ptr[esp]
   mov dword ptr[esp], eax
}
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
__asm {
   pop eax
   xor eax, dword ptr[esp]
   mov dword ptr[esp], eax
}
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
__asm {
   pop ecx
   pop eax
   cdq
   idiv ecx
   push edx
}
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
__asm {
   pop eax
   add dword ptr[esp], eax
}
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
__asm {
   pop eax
   sub dword ptr[esp], eax
}
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
__asm {
   pop eax
   imul eax, dword ptr[esp]
   mov dword ptr[esp], eax
}
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
__asm {
   pop ecx
   pop eax
   cdq
   idiv ecx
   push eax
}
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
__asm {
   pop ecx
   pop eax
   sal eax, cl
   push eax
}
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
__asm {
   pop ecx
   pop eax
   sar eax, cl
   push eax
}
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
__asm {
   pop eax
   xor ecx, ecx
   cmp dword ptr[esp], eax
   setg ecx
   mov dword ptr[esp], ecx
}
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
__asm {
   pop eax
   xor ecx, ecx
   cmp dword ptr[esp], eax
   setge ecx
   mov dword ptr[esp], ecx
}
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
__asm {
   pop eax
   xor ecx, ecx
   cmp dword ptr[esp], eax
   setl ecx
   mov dword ptr[esp], ecx
}
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
__asm {
   pop eax
   xor ecx, ecx
   cmp dword ptr[esp], eax
   setle ecx
   mov dword ptr[esp], ecx
}
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
__asm {
   pop eax
   xor ecx, ecx
   cmp dword ptr[esp], eax
   setne ecx
   mov dword ptr[esp], ecx
}
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
__asm {
   pop eax
   xor ecx, ecx
   cmp dword ptr[esp], eax
   sete ecx
   mov dword ptr[esp], ecx
}
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
__asm {
   fld dword ptr[esp]
   fadd dword ptr[esp+4]
   add esp, 4
   fstp dword ptr[esp]
}
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
__asm {
   fld dword ptr[esp+4]
   fsub dword ptr[esp]
   add esp, 4
   fstp dword ptr[esp]
}
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
__asm {
   fld dword ptr[esp+4]
   fmul dword ptr[esp]
   add esp, 4
   fstp dword ptr[esp]
}
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
__asm {
   fld dword ptr[esp+4]
   fdiv dword ptr[esp]
   add esp, 4
   fstp dword ptr[esp]
}
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
__asm {
   fld dword ptr[esp+4]
   fcomp dword ptr[esp]
   xor ecx, ecx
   fnstsw ax
   test ah, 0x41
   sete ecx
   add esp, 4
   mov dword ptr[esp], ecx
}
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
__asm {
   fld dword ptr[esp+4]
   fcomp dword ptr[esp]
   xor ecx, ecx
   fnstsw ax
   test ah, 0x1
   sete ecx
   add esp, 4
   mov dword ptr[esp], ecx
}
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
__asm {
   fld dword ptr[esp+4]
   fcomp dword ptr[esp]
   xor ecx, ecx
   fnstsw ax
   test ah, 0x1
   setne ecx
   add esp, 4
   mov dword ptr[esp], ecx
}
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
__asm {
   fld dword ptr[esp+4]
   fcomp dword ptr[esp]
   xor ecx, ecx
   fnstsw ax
   test ah, 0x41
   setne ecx
   add esp, 4
   mov dword ptr[esp], ecx
}
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
__asm {
   fld dword ptr[esp+4]
   fcomp dword ptr[esp]
   xor ecx, ecx
   fnstsw ax
   test ah, 0x40
   sete ecx
   add esp, 4
   mov dword ptr[esp], ecx
}
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
__asm {
   fld dword ptr[esp+4]
   fcomp dword ptr[esp]
   xor ecx, ecx
   fnstsw ax
   test ah, 0x40
   setne ecx
   add esp, 4
   mov dword ptr[esp], ecx
}
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
__asm {
   fild dword ptr[esp]
   fstp dword ptr[esp]
}
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
__asm {
   fild dword ptr[esp+4]
   fstp dword ptr[esp+4]
}
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
__asm {
   fld dword ptr[esp]
   fistp dword ptr[esp]
}
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
__asm {
   fld dword ptr[esp+4]
   fistp dword ptr[esp+4]
}
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
__asm {
   mov esi, 0xC0FFAAA1
   mov esi, dword ptr[esi]
}
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
__asm {
   mov esi, 0xC0FFAAA1
   mov eax, 0xC0FFABA1
   mov esi, dword ptr[esi]
   mov eax, dword ptr[eax]
   mov dword ptr[ebp-0x18], eax
}
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
__asm {
   mov esi, 0xC0FFAAA2
   mov esi, dword ptr[esi]
}
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
__asm {
   mov esi, 0xC0FFAAA2
   mov eax, 0xC0FFABA2
   mov esi, dword ptr[esi]
   mov eax, dword ptr[eax]
   mov dword ptr[ebp-0x18], eax
}
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
__asm {
    pop edi
    push dword ptr[esi+edi*4]
}
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
__asm {
    pop edi
    rol edi, 2
    bound edi, dword ptr[ebp-0x1c]
    push dword ptr[esi+edi]
}
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
__asm {
    pop edi
    pop eax
    mov dword ptr[esi+edi*4], eax
}
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
__asm {
    pop edi
    pop eax
    rol edi, 2
    bound edi, dword ptr[ebp-0x1c]
    mov dword ptr[esi+edi], eax
}
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
  __asm { 
    pop eax
  }
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
__asm {
call eax
}
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
__asm {
add esp, 4
}
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
__asm {
sub esp, 4
}
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
__asm {
   mov ecx, 8
   pop eax
   mov dl, al
   rol edx, cl
   pop eax
   mov dl, al
   rol edx, cl
   pop eax
   mov dl, al
   rol edx, cl
   pop eax
   mov dl, al
   push edx
}
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
__asm {
   mov dword ptr[esp], eax
}
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
__asm {
   fstp dword ptr[esp]
}
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
__asm {
   push esp
}
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
__asm {
   _emit 0xA1
   _emit 0xC5
   _emit 0xc4
   _emit 0xFF
   _emit 0xC0
   _emit 0x21
   _emit 0x05
   _emit 0xC4
   _emit 0xc4
   _emit 0xFF
   _emit 0xC0
}
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
__asm {
   _emit 0xA1
   _emit 0xC5
   _emit 0xc4
   _emit 0xFF
   _emit 0xC0
   _emit 0x09
   _emit 0x05
   _emit 0xC4
   _emit 0xc4
   _emit 0xFF
   _emit 0xC0
}
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
__asm {
   _emit 0xA1
   _emit 0xC5
   _emit 0xc4
   _emit 0xFF
   _emit 0xC0
   _emit 0x31
   _emit 0x05
   _emit 0xC4
   _emit 0xc4
   _emit 0xFF
   _emit 0xC0
}
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
__asm {
   _emit 0x8B
   _emit 0x0D
   _emit 0xC5
   _emit 0xc4
   _emit 0xFF
   _emit 0xC0
   _emit 0xA1
   _emit 0xC4
   _emit 0xc4
   _emit 0xFF
   _emit 0xC0
   cdq
   idiv ecx
   _emit 0x89
   _emit 0x15
   _emit 0xC4
   _emit 0xc4
   _emit 0xFF
   _emit 0xC0
}
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
__asm {
   _emit 0xA1
   _emit 0xC5
   _emit 0xc4
   _emit 0xFF
   _emit 0xC0
   _emit 0x01
   _emit 0x05
   _emit 0xC4
   _emit 0xc4
   _emit 0xFF
   _emit 0xC0
}
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
__asm {
   _emit 0xA1
   _emit 0xC5
   _emit 0xc4
   _emit 0xFF
   _emit 0xC0
   _emit 0x29
   _emit 0x05
   _emit 0xC4
   _emit 0xc4
   _emit 0xFF
   _emit 0xC0
}
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
__asm {
   _emit 0xA1
   _emit 0xC4
   _emit 0xc4
   _emit 0xFF
   _emit 0xC0
   _emit 0x0F
   _emit 0xAF
   _emit 0x05
   _emit 0xC5
   _emit 0xc4
   _emit 0xFF
   _emit 0xC0
   _emit 0xa3
   _emit 0xC4
   _emit 0xc4
   _emit 0xFF
   _emit 0xC0
}
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
__asm {
   _emit 0xA1
   _emit 0xC4
   _emit 0xc4
   _emit 0xFF
   _emit 0xC0
   cdq
   _emit 0x8B
   _emit 0x0D
   _emit 0xC5
   _emit 0xc4
   _emit 0xFF
   _emit 0xC0
   idiv ecx
   _emit 0xa3
   _emit 0xC4
   _emit 0xc4
   _emit 0xFF
   _emit 0xC0
}
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
__asm {
   _emit 0xA1
   _emit 0xC4
   _emit 0xc4
   _emit 0xFF
   _emit 0xC0
   _emit 0x8B
   _emit 0x0D
   _emit 0xC5
   _emit 0xc4
   _emit 0xFF
   _emit 0xC0
   sal eax, cl
   _emit 0xa3
   _emit 0xC4
   _emit 0xc4
   _emit 0xFF
   _emit 0xC0
}
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
__asm {
   _emit 0xA1
   _emit 0xC4
   _emit 0xc4
   _emit 0xFF
   _emit 0xC0
   _emit 0x8B
   _emit 0x0D
   _emit 0xC5
   _emit 0xc4
   _emit 0xFF
   _emit 0xC0
   sar eax, cl
   _emit 0xa3
   _emit 0xC4
   _emit 0xc4
   _emit 0xFF
   _emit 0xC0
}
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
__asm {
   _emit 0xA1
   _emit 0xC4
   _emit 0xc4
   _emit 0xFF
   _emit 0xC0
   _emit 0x03
   _emit 0x05
   _emit 0xC5
   _emit 0xc4
   _emit 0xFF
   _emit 0xC0
   push eax
}
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
__asm {
   _emit 0xA1
   _emit 0xC4
   _emit 0xc4
   _emit 0xFF
   _emit 0xC0
   _emit 0x0B
   _emit 0x05
   _emit 0xC5
   _emit 0xc4
   _emit 0xFF
   _emit 0xC0
   push eax
}
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
__asm {
   _emit 0xA1
   _emit 0xC4
   _emit 0xc4
   _emit 0xFF
   _emit 0xC0
   _emit 0x33
   _emit 0x05
   _emit 0xC5
   _emit 0xc4
   _emit 0xFF
   _emit 0xC0
   push eax
}
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
__asm {
   _emit 0x8B
   _emit 0x0D
   _emit 0xC5
   _emit 0xc4
   _emit 0xFF
   _emit 0xC0
   _emit 0xA1
   _emit 0xC4
   _emit 0xc4
   _emit 0xFF
   _emit 0xC0
   cdq
   idiv ecx
   push edx
}
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
__asm {
   _emit 0xA1
   _emit 0xC4
   _emit 0xc4
   _emit 0xFF
   _emit 0xC0
   _emit 0x03
   _emit 0x05
   _emit 0xC5
   _emit 0xc4
   _emit 0xFF
   _emit 0xC0
   push eax
}
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
__asm {
   _emit 0xA1
   _emit 0xC4
   _emit 0xc4
   _emit 0xFF
   _emit 0xC0
   _emit 0x2B
   _emit 0x05
   _emit 0xC5
   _emit 0xc4
   _emit 0xFF
   _emit 0xC0
   push eax
}
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
__asm {
   _emit 0xA1
   _emit 0xC4
   _emit 0xc4
   _emit 0xFF
   _emit 0xC0
   _emit 0x0F
   _emit 0xAF
   _emit 0x05
   _emit 0xC5
   _emit 0xc4
   _emit 0xFF
   _emit 0xC0
   push eax
}
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
__asm {
   _emit 0xA1
   _emit 0xC4
   _emit 0xc4
   _emit 0xFF
   _emit 0xC0
   cdq
   _emit 0x8B
   _emit 0x0D
   _emit 0xC5
   _emit 0xc4
   _emit 0xFF
   _emit 0xC0
   idiv ecx
   push eax
}
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
__asm {
   _emit 0xA1
   _emit 0xC4
   _emit 0xc4
   _emit 0xFF
   _emit 0xC0
   _emit 0x8B
   _emit 0x0D
   _emit 0xC5
   _emit 0xc4
   _emit 0xFF
   _emit 0xC0
   sal eax, cl
   push eax
}
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
__asm {
   _emit 0xA1
   _emit 0xC4
   _emit 0xc4
   _emit 0xFF
   _emit 0xC0
   _emit 0x8B
   _emit 0x0D
   _emit 0xC5
   _emit 0xc4
   _emit 0xFF
   _emit 0xC0
   sar eax, cl
   push eax
}
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
__asm {
   _emit 0xA1
   _emit 0xC4
   _emit 0xc4
   _emit 0xFF
   _emit 0xC0
   and eax, 0xC0FFC0FE
   push eax
}
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
__asm {
   _emit 0xA1
   _emit 0xC4
   _emit 0xc4
   _emit 0xFF
   _emit 0xC0
   or eax, 0xC0FFC0FE
   push eax
}
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
__asm {
   _emit 0xA1
   _emit 0xC4
   _emit 0xc4
   _emit 0xFF
   _emit 0xC0
   xor eax, 0xC0FFC0FE
   push eax
}
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
__asm {
   mov ecx, 0xC0FFC0FE
   _emit 0xA1
   _emit 0xC4
   _emit 0xc4
   _emit 0xFF
   _emit 0xC0
   cdq
   idiv ecx
   push edx
}
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
__asm {
   _emit 0xA1
   _emit 0xC4
   _emit 0xc4
   _emit 0xFF
   _emit 0xC0
   add eax, 0xC0FFC0FE
   push eax
}
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
__asm {
   _emit 0xA1
   _emit 0xC4
   _emit 0xc4
   _emit 0xFF
   _emit 0xC0
   sub eax, 0xC0FFC0FE
   push eax
}
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
__asm {
   _emit 0xA1
   _emit 0xC4
   _emit 0xc4
   _emit 0xFF
   _emit 0xC0
   imul eax, 0xC0FFC0FE
   push eax
}
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
__asm {
   _emit 0xA1
   _emit 0xC4
   _emit 0xc4
   _emit 0xFF
   _emit 0xC0
   cdq
   mov ecx, 0xC0FFC0FE
   idiv ecx
   push eax
}
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
__asm {
   _emit 0xA1
   _emit 0xC4
   _emit 0xc4
   _emit 0xFF
   _emit 0xC0
   mov ecx, 0xC0FFC0FE
   sal eax, cl
   push eax
}
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
__asm {
   _emit 0xA1
   _emit 0xC4
   _emit 0xc4
   _emit 0xFF
   _emit 0xC0
   mov ecx, 0xC0FFC0FE
   sar eax, cl
   push eax
}
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
__asm {
   _emit 0xd9
   _emit 0x05
   _emit 0xC4
   _emit 0xc4
   _emit 0xFF
   _emit 0xC0
   _emit 0xd8
   _emit 0x05
   _emit 0xC5
   _emit 0xc4
   _emit 0xFF
   _emit 0xC0
   _emit 0xd9
   _emit 0x1d
   _emit 0xC4
   _emit 0xc4
   _emit 0xFF
   _emit 0xC0
}
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
__asm {
   _emit 0xd9
   _emit 0x05
   _emit 0xC4
   _emit 0xc4
   _emit 0xFF
   _emit 0xC0
   _emit 0xd8
   _emit 0x25
   _emit 0xC5
   _emit 0xc4
   _emit 0xFF
   _emit 0xC0
   _emit 0xd9
   _emit 0x1d
   _emit 0xC4
   _emit 0xc4
   _emit 0xFF
   _emit 0xC0
}
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
__asm {
   _emit 0xd9
   _emit 0x05
   _emit 0xC4
   _emit 0xc4
   _emit 0xFF
   _emit 0xC0
   _emit 0xd8
   _emit 0x0d
   _emit 0xC5
   _emit 0xc4
   _emit 0xFF
   _emit 0xC0
   _emit 0xd9
   _emit 0x1d
   _emit 0xC4
   _emit 0xc4
   _emit 0xFF
   _emit 0xC0
}
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
__asm {
   _emit 0xd9
   _emit 0x05
   _emit 0xC4
   _emit 0xc4
   _emit 0xFF
   _emit 0xC0
   _emit 0xd8
   _emit 0x35
   _emit 0xC5
   _emit 0xc4
   _emit 0xFF
   _emit 0xC0
   _emit 0xd9
   _emit 0x1d
   _emit 0xC4
   _emit 0xc4
   _emit 0xFF
   _emit 0xC0
}
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
__asm {
   _emit 0xd9
   _emit 0x05
   _emit 0xC4
   _emit 0xc4
   _emit 0xFF
   _emit 0xC0
   sub esp, 4
   _emit 0xd8
   _emit 0x05
   _emit 0xC5
   _emit 0xc4
   _emit 0xFF
   _emit 0xC0
   fstp dword ptr[esp]
}
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
__asm {
   _emit 0xd9
   _emit 0x05
   _emit 0xC4
   _emit 0xc4
   _emit 0xFF
   _emit 0xC0
   sub esp, 4
   _emit 0xd8
   _emit 0x25
   _emit 0xC5
   _emit 0xc4
   _emit 0xFF
   _emit 0xC0
   fstp dword ptr[esp]
}
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
__asm {
   _emit 0xd9
   _emit 0x05
   _emit 0xC4
   _emit 0xc4
   _emit 0xFF
   _emit 0xC0
   sub esp, 4
   _emit 0xd8
   _emit 0x0d
   _emit 0xC5
   _emit 0xc4
   _emit 0xFF
   _emit 0xC0
   fstp dword ptr[esp]
}
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
__asm {
   _emit 0xd9
   _emit 0x05
   _emit 0xC4
   _emit 0xc4
   _emit 0xFF
   _emit 0xC0
   sub esp, 4
   _emit 0xd8
   _emit 0x35
   _emit 0xC5
   _emit 0xc4
   _emit 0xFF
   _emit 0xC0
   fstp dword ptr[esp]
}
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
__asm {
   _emit 0xd9
   _emit 0x05
   _emit 0xC4
   _emit 0xc4
   _emit 0xFF
   _emit 0xC0
   sub esp, 4
   mov dword ptr[ebp-4], 0xC0FFC0FE
   fadd dword ptr[ebp-4]
   fstp dword ptr[esp]
}
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
__asm {
   _emit 0xd9
   _emit 0x05
   _emit 0xC4
   _emit 0xc4
   _emit 0xFF
   _emit 0xC0
   sub esp, 4
   mov dword ptr[ebp-4], 0xC0FFC0FE
   fsub dword ptr[ebp-4]
   fstp dword ptr[esp]
}
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
__asm {
   _emit 0xd9
   _emit 0x05
   _emit 0xC4
   _emit 0xc4
   _emit 0xFF
   _emit 0xC0
   sub esp, 4
   mov dword ptr[ebp-4], 0xC0FFC0FE
   fmul dword ptr[ebp-4]
   fstp dword ptr[esp]
}
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
__asm {
   _emit 0xd9
   _emit 0x05
   _emit 0xC4
   _emit 0xc4
   _emit 0xFF
   _emit 0xC0
   sub esp, 4
   mov dword ptr[ebp-4], 0xC0FFC0FE
   fdiv dword ptr[ebp-4]
   fstp dword ptr[esp]
}
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
__asm {
   _emit 0xA1
   _emit 0xC4
   _emit 0xc4
   _emit 0xFF
   _emit 0xC0
   _emit 0x3B
   _emit 0x05
   _emit 0xC5
   _emit 0xc4
   _emit 0xFF
   _emit 0xC0
  jle jgtiviv_l1
  mov eax, 0xC0DEC0D3
jgtiviv_l1:
}
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
__asm {
   _emit 0xA1
   _emit 0xC4
   _emit 0xc4
   _emit 0xFF
   _emit 0xC0
   _emit 0x3B
   _emit 0x05
   _emit 0xC5
   _emit 0xc4
   _emit 0xFF
   _emit 0xC0
  jl jgeiviv_l1
  mov eax, 0xC0DEC0D3
jgeiviv_l1:
}
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
__asm {
   _emit 0xA1
   _emit 0xC4
   _emit 0xc4
   _emit 0xFF
   _emit 0xC0
   _emit 0x3B
   _emit 0x05
   _emit 0xC5
   _emit 0xc4
   _emit 0xFF
   _emit 0xC0
  jge jltiviv_l1
  mov eax, 0xC0DEC0D3
jltiviv_l1:
}
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
__asm {
   _emit 0xA1
   _emit 0xC4
   _emit 0xc4
   _emit 0xFF
   _emit 0xC0
   _emit 0x3B
   _emit 0x05
   _emit 0xC5
   _emit 0xc4
   _emit 0xFF
   _emit 0xC0
  jg jleiviv_l1
  mov eax, 0xC0DEC0D3
jleiviv_l1:
}
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
__asm {
   _emit 0xA1
   _emit 0xC4
   _emit 0xc4
   _emit 0xFF
   _emit 0xC0
   _emit 0x3B
   _emit 0x05
   _emit 0xC5
   _emit 0xc4
   _emit 0xFF
   _emit 0xC0
  je jneiviv_l1
  mov eax, 0xC0DEC0D3
jneiviv_l1:
}
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
__asm {
   _emit 0xA1
   _emit 0xC4
   _emit 0xc4
   _emit 0xFF
   _emit 0xC0
   _emit 0x3B
   _emit 0x05
   _emit 0xC5
   _emit 0xc4
   _emit 0xFF
   _emit 0xC0
  jne jeqiviv_l1
  mov eax, 0xC0DEC0D3
jeqiviv_l1:
}
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
__asm {
   _emit 0xA1
   _emit 0xC4
   _emit 0xc4
   _emit 0xFF
   _emit 0xC0
   cmp eax, 0xC0FFC0FE
  jle jgtivic_l1
  mov eax, 0xC0DEC0D3
jgtivic_l1:
}
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
__asm {
   _emit 0xA1
   _emit 0xC4
   _emit 0xc4
   _emit 0xFF
   _emit 0xC0
   cmp eax, 0xC0FFC0FE
  jl jgeivic_l1
  mov eax, 0xC0DEC0D3
jgeivic_l1:
}
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
__asm {
   _emit 0xA1
   _emit 0xC4
   _emit 0xc4
   _emit 0xFF
   _emit 0xC0
   cmp eax, 0xC0FFC0FE
  jge jltivic_l1
  mov eax, 0xC0DEC0D3
jltivic_l1:
}
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
__asm {
   _emit 0xA1
   _emit 0xC4
   _emit 0xc4
   _emit 0xFF
   _emit 0xC0
   cmp eax, 0xC0FFC0FE
  jg jleivic_l1
  mov eax, 0xC0DEC0D3
jleivic_l1:
}
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
__asm {
   _emit 0xA1
   _emit 0xC4
   _emit 0xc4
   _emit 0xFF
   _emit 0xC0
   cmp eax, 0xC0FFC0FE
  je jneivic_l1
  mov eax, 0xC0DEC0D3
jneivic_l1:
}
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;
__asm {
   _emit 0xA1
   _emit 0xC4
   _emit 0xc4
   _emit 0xFF
   _emit 0xC0
   cmp eax, 0xC0FFC0FE
  jne jeqivic_l1
  mov eax, 0xC0DEC0D3
jeqivic_l1:
}
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;

errz_vmcore:
vm86del=0x12345678;
 *(sU32*)0xFFFFFFFF=0xCCCCCCCC;

}
