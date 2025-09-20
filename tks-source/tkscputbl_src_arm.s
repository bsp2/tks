
ocx_fixme:	
	
ocx_prologue:	.global ocx_prologue
	push {r3-r11, lr}       @ push r3 so stack is aligned to 8 bytes
	mov r8, r2		@ vm_vars
	mov r5, r3		@ vm_const
	sub r6, sp, #256	@ lvars
	mov sp, r6
	str r0, [r6, #0] 	@ store lvar =context=
	str r1, [r6, #4]	@ store lvar =env=
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC

oc_halt:
	@ b ocx_fixme  NO-OP (always the last instruction)
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC

oc_bra:
	b ocx_fixme
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC

oc_sitestz:
	pop {r0}
	cmp r0, #0
	beq ocx_fixme
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC

oc_sitestzp:
	ldr r0, [sp, #0]
	cmp r0, #0
	beq ocx_fixme
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC

oc_bsr:
	bl ocx_fixme
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC

oc_rts:
	mov pc, lr
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC

oc_siloop:
	ldr r0, [sp, #0]
	subs r0, r0, #1
	str r0, [sp, #0]
	bne ocx_fixme
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC

oc_pushlv:
	ldr r0, [r6, #1023]   @1023 will be replaced by actual local var offset
	push {r0}
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC

oc_pushlivinc:
	ldr r0, [r6, #1023]
	push {r0}
	add r0, r0, #1
	str r0, [r6, #1023]
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC

oc_pushincliv:	
	ldr r0, [r6, #1023]
	add r0, r0, #1
	push {r0}
	str r0, [r6, #1023]
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC

oc_pushlivdec:
	ldr r0, [r6, #1023]
	push {r0}
	sub r0, r0, #1
	str r0, [r6, #1023]
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC

oc_pushdecliv:	
	ldr r0, [r6, #1023]
	sub r0, r0, #1
	push {r0}
	str r0, [r6, #1023]
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC

oc_poplv:
	pop {r0}
	str r0, [r6, #1023]
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC

oc_pushv:
	ldr r0, [r8, #1023]  @1023 will be replaced by actual varaddr offset
	ldr r0, [r0]
	push {r0}
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC

oc_pushivinc:
	ldr r0, [r8, #1023]  @1023 will be replaced by actual varaddr offset
	ldr r1, [r0]
	push {r1}
	add r1, r1, #1
	str r1, [r0]
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC

oc_pushinciv:
	ldr r0, [r8, #1023]  @1023 will be replaced by actual varaddr offset
	ldr r1, [r0]
	add r1, r1, #1
	push {r1}
	str r1, [r0]
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC

oc_pushivdec:
	ldr r0, [r8, #1023]  @1023 will be replaced by actual varaddr offset
	ldr r1, [r0]
	push {r1}
	sub r1, r1, #1
	str r1, [r0]
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC

oc_pushdeciv:
	ldr r0, [r8, #1023]  @1023 will be replaced by actual varaddr offset
	ldr r1, [r0]
	sub r1, r1, #1
	push {r1}
	str r1, [r0]
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC

oc_popv:
	ldr r0, [r8, #1023]  @1023 will be replaced by actual varaddr offset
	pop {r1}
	str r1, [r0]
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC

oc_pushc:
	ldr r0, [r5, #1023]  @1023 will be replaced by actual const offset
	push {r0}
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC

oc_loadfxc:
	@ load fxnaddr for APICALL*
	ldr r4, [r5, #1023]
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC

oc_movlv:
	ldr r0, [r6, #1023]
	str r0, [r6, #1023]
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC

oc_movlvc:
	ldr r0, [r5, #1023]
	str r0, [r6, #1023]
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC

oc_movv:
	ldr r0, [r8, #1023] 	@ srcvaraddr
	ldr r1, [r8, #1023]	@ dstvaraddr
	ldr r0, [r0]		@ srcval
	str r0, [r1]
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC

oc_movvc:
	ldr r0, [r5, #1023] 	@ constval
	ldr r1, [r8, #1023]	@ dstvaraddr
	str r0, [r1]
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC

oc_incliv:
	ldr r0, [r6, #1023] 	@ localvar
	add r0, r0, #1
	str r0, [r6, #1023]
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC

oc_decliv:
	ldr r0, [r6, #1023] 	@ localvar
	sub r0, r0, #1
	str r0, [r6, #1023]
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC

oc_inciv:
	ldr r0, [r8, #1023] 	@ varaddr
	ldr r1, [r0]
	add r1, r1, #1
	str r1, [r0]
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC

oc_deciv:
	ldr r0, [r8, #1023] 	@ varaddr
	ldr r1, [r0]
	sub r1, r1, #1
	str r1, [r0]
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC

oc_livandliv:
	ldr r0, [r6, #1023] 	@ dstlvar
	ldr r1, [r6, #1023] 	@ arglvar
	and r0, r0, r1
	str r0, [r6, #1023]
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC

oc_livorliv:
	ldr r0, [r6, #1023] 	@ dstlvar
	ldr r1, [r6, #1023] 	@ arglvar
	orr r0, r0, r1
	str r0, [r6, #1023]
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC

oc_liveorliv:
	ldr r0, [r6, #1023] 	@ dstlvar
	ldr r1, [r6, #1023] 	@ arglvar
	eor r0, r0, r1
	str r0, [r6, #1023]
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC

oc_livmodliv:
	ldr r0, [r6, #1023] 	@ dstlvar
	ldr r1, [r6, #1023] 	@ arglvar
	ldr r2, [r5, #0]	@ __aeabi_idivmod
	blx r2
	str r1, [r6, #1023]	@ r0=div r1=mod
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC

oc_livaddliv:
	ldr r0, [r6, #1023] 	@ dstlvar
	ldr r1, [r6, #1023] 	@ arglvar
	add r0, r0, r1
	str r0, [r6, #1023]
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC

oc_livsubliv:
	ldr r0, [r6, #1023] 	@ dstlvar
	ldr r1, [r6, #1023] 	@ arglvar
	sub r0, r0, r1
	str r0, [r6, #1023]
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC

oc_livmulliv:
	ldr r0, [r6, #1023] 	@ dstlvar
	ldr r1, [r6, #1023] 	@ arglvar
	mul r0, r0, r1
	str r0, [r6, #1023]
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC

oc_livdivliv:
	ldr r0, [r6, #1023] 	@ dstlvar
	ldr r1, [r6, #1023] 	@ arglvar
	ldr r3, [r5, #16]	@ __idivsi3
	blx r3
	str r0, [r6, #1023]
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC

oc_livaslliv:
	ldr r0, [r6, #1023] 	@ dstlvar
	ldr r1, [r6, #1023] 	@ arglvar
	lsl r0, r0, r1
	str r0, [r6, #1023]
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC

oc_livasrliv:
	ldr r0, [r6, #1023] 	@ dstlvar
	ldr r1, [r6, #1023] 	@ arglvar
	asr r0, r0, r1
	str r0, [r6, #1023]
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC

oc_pushlivandliv:
	ldr r0, [r6, #1023] 	@ dstlvar
	ldr r1, [r6, #1023] 	@ arglvar
	and r0, r0, r1
	push {r0}
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC

oc_pushlivorliv:
	ldr r0, [r6, #1023] 	@ dstlvar
	ldr r1, [r6, #1023] 	@ arglvar
	orr r0, r0, r1
	push {r0}
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC

oc_pushliveorliv:
	ldr r0, [r6, #1023] 	@ dstlvar
	ldr r1, [r6, #1023] 	@ arglvar
	eor r0, r0, r1
	push {r0}
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC

oc_pushlivmodliv:
	ldr r0, [r6, #1023] 	@ dstlvar
	ldr r1, [r6, #1023] 	@ arglvar
	ldr r2, [r5, #0]	@ __aeabi_idivmod
	blx r2
	push {r1}		@ r0=div r1=mod
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC

oc_pushlivaddliv:
	ldr r0, [r6, #1023] 	@ dstlvar
	ldr r1, [r6, #1023] 	@ arglvar
	add r0, r0, r1
	push {r0}
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC

oc_pushlivsubliv:
	ldr r0, [r6, #1023] 	@ dstlvar
	ldr r1, [r6, #1023] 	@ arglvar
	sub r0, r0, r1
	push {r0}
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC

oc_pushlivmulliv:
	ldr r0, [r6, #1023] 	@ dstlvar
	ldr r1, [r6, #1023] 	@ arglvar
	mul r0, r0, r1
	push {r0}
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC

oc_pushlivdivliv:
	ldr r0, [r6, #1023] 	@ dstlvar
	ldr r1, [r6, #1023] 	@ arglvar
	ldr r3, [r5, #16]	@ __idivsi3
	blx r3
	push {r0}
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC

oc_pushlivaslliv:
	ldr r0, [r6, #1023] 	@ dstlvar
	ldr r1, [r6, #1023] 	@ arglvar
	lsl r0, r0, r1
	push {r0}
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC

oc_pushlivasrliv:
	ldr r0, [r6, #1023] 	@ dstlvar
	ldr r1, [r6, #1023] 	@ arglvar
	asr r0, r0, r1
	push {r0}
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC
	
oc_pushlivandc:
	ldr r0, [r6, #1023] 	@ dstlvar
	ldr r1, [r5, #1023] 	@ constval
	and r0, r0, r1
	push {r0}
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC

oc_pushlivorc:
	ldr r0, [r6, #1023] 	@ dstlvar
	ldr r1, [r5, #1023] 	@ constval
	orr r0, r0, r1
	push {r0}
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC

oc_pushliveorc:
	ldr r0, [r6, #1023] 	@ dstlvar
	ldr r1, [r5, #1023] 	@ constval
	eor r0, r0, r1
	push {r0}
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC

oc_pushlivmodc:
	ldr r0, [r6, #1023] 	@ dstlvar
	ldr r1, [r5, #1023] 	@ constval
	ldr r2, [r5, #0]	@ __aeabi_idivmod
	blx r2
	push {r1}		@ r0=div r1=mod
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC

oc_pushlivaddc:
	ldr r0, [r6, #1023] 	@ dstlvar
	ldr r1, [r5, #1023] 	@ constval
	add r0, r0, r1
	push {r0}
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC

oc_pushlivsubc:
	ldr r0, [r6, #1023] 	@ dstlvar
	ldr r1, [r5, #1023] 	@ constval
	sub r0, r0, r1
	push {r0}
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC

oc_pushlivmulc:
	ldr r0, [r6, #1023] 	@ dstlvar
	ldr r1, [r5, #1023] 	@ constval
	mul r0, r0, r1
	push {r0}
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC

oc_pushlivdivc:
	ldr r0, [r6, #1023] 	@ dstlvar
	ldr r1, [r5, #1023] 	@ constval
	ldr r3, [r5, #16]	@ __divsi3
	blx r3
	push {r0}
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC

oc_pushlivaslc:
	ldr r0, [r6, #1023] 	@ dstlvar
	ldr r1, [r5, #1023] 	@ constval
	lsl r0, r0, r1
	push {r0}
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC

oc_pushlivasrc:
	ldr r0, [r6, #1023] 	@ dstlvar
	ldr r1, [r5, #1023] 	@ constval
	asr r0, r0, r1
	push {r0}
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC

oc_lfvaddlfv:
	vldr s0, [r6, #1020]	@ dstlvar
	vldr s1, [r6, #1020]  	@ arglvar
	vadd.f32 s0, s0, s1
	vstr s0, [r6, #1020]
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC

oc_lfvsublfv:
	vldr s0, [r6, #1020]	@ dstlvar
	vldr s1, [r6, #1020]  	@ arglvar
	vsub.f32 s0, s0, s1
	vstr s0, [r6, #1020]
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC

oc_lfvmullfv:
	vldr s0, [r6, #1020]	@ dstlvar
	vldr s1, [r6, #1020]  	@ arglvar
	vmul.f32 s0, s0, s1
	vstr s0, [r6, #1020]
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC

oc_lfvdivlfv:
	vldr s0, [r6, #1020]	@ dstlvar
	vldr s1, [r6, #1020]  	@ arglvar
	vdiv.f32 s0, s0, s1
	vstr s0, [r6, #1020]
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC
	
oc_pushlfvaddlfv:
	vldr s0, [r6, #1020]	@ dstlvar
	vldr s1, [r6, #1020]  	@ arglvar
	vadd.f32 s0, s0, s1
	vmov r0, s0
	push {r0}
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC

oc_pushlfvsublfv:
	vldr s0, [r6, #1020]	@ dstlvar
	vldr s1, [r6, #1020]  	@ arglvar
	vsub.f32 s0, s0, s1
	vmov r0, s0
	push {r0}
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC

oc_pushlfvmullfv:
	vldr s0, [r6, #1020]	@ dstlvar
	vldr s1, [r6, #1020]  	@ arglvar
	vmul.f32 s0, s0, s1
	vmov r0, s0
	push {r0}
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC

oc_pushlfvdivlfv:
	vldr s0, [r6, #1020]	@ dstlvar
	vldr s1, [r6, #1020]  	@ arglvar
	vdiv.f32 s0, s0, s1
	vmov r0, s0
	push {r0}
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC

oc_pushlfvaddc:
	vldr s0, [r6, #1020]	@ dstlvar
	vldr s1, [r5, #1020]  	@ constval
	vadd.f32 s0, s0, s1
	vmov r0, s0
	push {r0}
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC

oc_pushlfvsubc:
	vldr s0, [r6, #1020]	@ dstlvar
	vldr s1, [r5, #1020]  	@ constval
	vsub.f32 s0, s0, s1
	vmov r0, s0
	push {r0}
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC

oc_pushlfvmulc:
	vldr s0, [r6, #1020]	@ dstlvar
	vldr s1, [r5, #1020]  	@ constval
	vmul.f32 s0, s0, s1
	vmov r0, s0
	push {r0}
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC

oc_pushlfvdivc:
	vldr s0, [r6, #1020]	@ dstlvar
	vldr s1, [r6, #1020]  	@ constval
	vdiv.f32 s0, s0, s1
	vmov r0, s0
	push {r0}
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC

oc_sinot:
	ldr r0, [sp, #0]
	cmp r0, #0
	moveq r0, #1
	movne r0, #0
	str r0, [sp, #0]
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC

oc_siinv:
	ldr r0, [sp, #0]
	mvn r0, r0
	str r0, [sp, #0]
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC

oc_sineg:
	ldr r0, [sp, #0]
	mvn r0, r0
	add r0, r0, #1
	str r0, [sp, #0]
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC

oc_siquad:
	ldr r0, [sp, #0]
	mul r0, r0, r0
	str r0, [sp, #0]
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC

oc_sitestbz:
	ldr r0, [sp, #0]
	cmp r0, #0
	movne r0, #1
	str r0, [sp, #0]
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC

oc_sitestbz2:
	ldr r0, [sp, #4]
	cmp r0, #0
	movne r0, #1
	str r0, [sp, #4]
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC

oc_sfneg:
	@vldr s0, [sp, #0]
	@vneg.f32 s0, s0
	@vstr s0, [sp, #0]
	ldr r0, [sp, #0]
	eor r0, r0, #0x80000000
	str r0, [sp, #0]
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC

oc_sfquad:
	vldr s0, [sp, #0]
	vmul.f32 s0, s0, s0
	vstr s0, [sp, #0]
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC

oc_sfsin:
	ldr r0, [sp, #0]
	ldr r2, [r5, #4]	@ sinf
	blx r2
	str r0, [sp, #0]
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC

oc_sfcos:
	ldr r0, [sp, #0]
	ldr r2, [r5, #8]	@ cosf
	blx r2
	str r0, [sp, #0]
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC

oc_sftan:
	ldr r0, [sp, #0]
	ldr r2, [r5, #12]	@ tanf
	blx r2
	str r0, [sp, #0]
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC

oc_sfsqrt:
	vldr s0, [sp, #0]
	vsqrt.f32 s0, s0
	vstr s0, [sp, #0]
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC

oc_siabs:
	ldr r0, [sp, #0]
	cmp r0, #0
	mvnlt r0, r0
	addlt r0, r0, #1
	str r0, [sp, #0]
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC

oc_sfabs:
	vldr s0, [sp, #0]
	vabs.f32 s0, s0
	vstr s0, [sp, #0]
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC

oc_sffrac:
	vldr s0, [sp, #0]
	vcvt.s32.f32 s1, s0
	vcvt.f32.s32 s2, s1
	vsub.f32 s0, s0, s2
	vstr s0, [sp, #0]
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC

oc_sfround:
	vldr s0, [sp, #0]
	vcvt.s32.f32 s1, s0
	vcvt.f32.s32 s2, s1
	vstr s2, [sp, #0]
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC

oc_siand:
	pop {r0}
	ldr r1, [sp, #0]
	and r0, r0, r1
	str r0, [sp, #0]
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC

oc_sior:
	pop {r0}
	ldr r1, [sp, #0]
	orr r0, r0, r1
	str r0, [sp, #0]
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC

oc_sieor:
	pop {r0}
	ldr r1, [sp, #0]
	eor r0, r0, r1
	str r0, [sp, #0]
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC

oc_simod:
	pop {r1}
	ldr r0, [sp, #0]
	ldr r2, [r5, #0] 	@ __aeabi_idivmod
	blx r2
	str r1, [sp, #0]	@ r0=div r1=mod
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC

oc_siadd:
	pop {r0}
	ldr r1, [sp, #0]
	add r0, r0, r1
	str r0, [sp, #0]
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC

oc_sisub:
	pop {r1}
	ldr r0, [sp, #0]
	sub r0, r0, r1
	str r0, [sp, #0]
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC

oc_simul:
	pop {r0}
	ldr r1, [sp, #0]
	mul r0, r0, r1
	str r0, [sp, #0]
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC

oc_sidiv:
	pop {r1}
	ldr r0, [sp, #0]
	ldr r2, [r5, #16]	@ __divsi3
	blx r2
	str r0, [sp, #0]
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC

oc_siasl:
	pop {r1}
	ldr r0, [sp, #0]
	lsl r0, r0, r1
	str r0, [sp, #0]
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC

oc_siasr:
	pop {r1}
	ldr r0, [sp, #0]
	asr r0, r0, r1
	str r0, [sp, #0]
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC

oc_sicmpbgt:
	@ '>'
	pop {r1}
	ldr r0, [sp, #0]
	cmp r0, r1
	movgt r0, #1
	movle r0, #0
	str r0, [sp, #0]
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC

oc_sicmpbge:	
	@ '>='
	pop {r1}
	ldr r0, [sp, #0]
	cmp r0, r1
	movge r0, #1
	movlt r0, #0
	str r0, [sp, #0]
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC

oc_sicmpblt:	
	@ '<'
	pop {r1}
	ldr r0, [sp, #0]
	cmp r0, r1
	movlt r0, #1
	movge r0, #0
	str r0, [sp, #0]
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC

oc_sicmpble:	
	@ '<='
	pop {r1}
	ldr r0, [sp, #0]
	cmp r0, r1
	movle r0, #1
	movgt r0, #0
	str r0, [sp, #0]
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC

oc_sicmpbne:
	@ '!='
	pop {r1}
	ldr r0, [sp, #0]
	cmp r0, r1
	movne r0, #1
	moveq r0, #0
	str r0, [sp, #0]
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC

oc_sicmpbeq:
	@ '=='
	pop {r1}
	ldr r0, [sp, #0]
	cmp r0, r1
	moveq r0, #1
	movne r0, #0
	str r0, [sp, #0]
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC

oc_sfadd:
	vldr s1, [sp, #0]
	vldr s0, [sp, #4]
	add sp, sp, #4
	vadd.f32 s0, s0, s1
	vstr s0, [sp, #0]
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC

oc_sfsub:
	vldr s1, [sp, #0]
	vldr s0, [sp, #4]
	add sp, sp, #4
	vsub.f32 s0, s0, s1
	vstr s0, [sp, #0]
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC

oc_sfmul:
	vldr s1, [sp, #0]
	vldr s0, [sp, #4]
	add sp, sp, #4
	vmul.f32 s0, s0, s1
	vstr s0, [sp, #0]
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC

oc_sfdiv:
	vldr s1, [sp, #0]
	vldr s0, [sp, #4]
	add sp, sp, #4
	vdiv.f32 s0, s0, s1
	vstr s0, [sp, #0]
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC

oc_sfcmpbgt:
	@ '>'
	vldr s1, [sp, #0]
	vldr s0, [sp, #4]
	add sp, sp, #4
	vcmp.f32 s0, s1
	vmrs APSR_nzcv, FPSCR
	movgt r0, #1
	movle r0, #0
	str r0, [sp, #0]
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC

oc_sfcmpbge:	
	@ '>='
	vldr s1, [sp, #0]
	vldr s0, [sp, #4]
	add sp, sp, #4
	vcmp.f32 s0, s1
	vmrs APSR_nzcv, FPSCR
	movge r0, #1
	movlt r0, #0
	str r0, [sp, #0]
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC

oc_sfcmpblt:	
	@ '<'
	vldr s1, [sp, #0]
	vldr s0, [sp, #4]
	add sp, sp, #4
	vcmp.f32 s0, s1
	vmrs APSR_nzcv, FPSCR
	movlt r0, #1
	movge r0, #0
	str r0, [sp, #0]
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC

oc_sfcmpble:	
	@ '<='
	vldr s1, [sp, #0]
	vldr s0, [sp, #4]
	add sp, sp, #4
	vcmp.f32 s0, s1
	vmrs APSR_nzcv, FPSCR
	movle r0, #1
	movgt r0, #0
	str r0, [sp, #0]
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC

oc_sfcmpbne:	
	@ '!='
	vldr s1, [sp, #0]
	vldr s0, [sp, #4]
	add sp, sp, #4
	vcmp.f32 s0, s1
	vmrs APSR_nzcv, FPSCR
	movne r0, #1
	moveq r0, #0
	str r0, [sp, #0]
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC

oc_sfcmpbeq:	
	@ '=='
	vldr s1, [sp, #0]
	vldr s0, [sp, #4]
	add sp, sp, #4
	vcmp.f32 s0, s1
	vmrs APSR_nzcv, FPSCR
	moveq r0, #1
	movne r0, #0
	str r0, [sp, #0]
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC

oc_stcif:
	vldr s0, [sp, #0]
	vcvt.f32.s32 s0, s0
	vstr s0, [sp, #0]
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC

oc_stcif2:
	vldr s0, [sp, #4]
	vcvt.f32.s32 s0, s0
	vstr s0, [sp, #4]
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC

oc_stcfi:
	vldr s0, [sp, #0]
	vcvt.s32.f32 s0, s0
	vstr s0, [sp, #0]
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC

oc_stcfi2:
	vldr s0, [sp, #4]
	vcvt.s32.f32 s0, s0
	vstr s0, [sp, #4]
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC

oc_iasel:
	ldr r7, [r8, #1023]	@ load ref. to array addr (&vm_iarr.adr)
	ldr r7, [r7]		@ load current array addr
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC

oc_iaselbc:			@ (note) boundary check not supported on ARM
	ldr r7, [r8, #1023]	@ load ref. to array addr (&vm_iarr.adr)
	ldr r7, [r7]		@ load current array addr
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC

oc_fasel:
	ldr r7, [r8, #1023]	@ load ref. to array addr (&vm_farr.adr)
	ldr r7, [r7]		@ load current array addr
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC

oc_faselbc:			@ (note) boundary check not supported on ARM
	ldr r7, [r8, #1023]	@ load ref. to array addr (&vm_farr.adr)
	ldr r7, [r7]		@ load current array addr
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC

oc_siapushl:
	ldr r0, [sp, #0]
	ldr r0, [r7, r0, LSL #2]
	str r0, [sp, #0]
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC

oc_siapushlbc:			@ (note) boundary check not supported on ARM
	ldr r0, [sp, #0]
	ldr r0, [r7, r0, LSL #2]
	str r0, [sp, #0]
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC

oc_siapopl:
	pop {r0}
	pop {r1}
	str r1, [r7, r0, LSL #2]
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC

oc_siapoplbc:			@ (note) boundary check not supported on ARM
	pop {r0}
	pop {r1}
	str r1, [r7, r0, LSL #2]
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC

oc_sipop:
	pop {r0}
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC

oc_begapicall0:
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC

oc_begapicall1:
	ldr r0, [sp, #0]
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC

oc_begapicall2:
	ldr r0, [sp, #0]
	ldr r1, [sp, #4]
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC

oc_begapicall3:
	ldr r0, [sp, #0]
	ldr r1, [sp, #4]
	ldr r2, [sp, #8]
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC

oc_begapicall4:
	ldr r0, [sp, #0]
	ldr r1, [sp, #4]
	ldr r2, [sp, #8]
	ldr r3, [sp, #12]
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC

oc_begapicall5:
	ldr r0, [sp, #0]
	ldr r1, [sp, #4]
	ldr r2, [sp, #8]
	ldr r3, [sp, #12]
	add sp, sp, #16		@ pass the rest of the args on stack
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC

oc_begapicall6:
	ldr r0, [sp, #0]
	ldr r1, [sp, #4]
	ldr r2, [sp, #8]
	ldr r3, [sp, #12]
	add sp, sp, #16		@ pass the rest of the args on stack
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC

oc_begapicall7:
	ldr r0, [sp, #0]
	ldr r1, [sp, #4]
	ldr r2, [sp, #8]
	ldr r3, [sp, #12]
	add sp, sp, #16		@ pass the rest of the args on stack
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC

oc_begapicall8:
	ldr r0, [sp, #0]
	ldr r1, [sp, #4]
	ldr r2, [sp, #8]
	ldr r3, [sp, #12]
	add sp, sp, #16		@ pass the rest of the args on stack
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC

oc_apicall:
	blx r4
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC

oc_endapicall0:	
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC
oc_endapicall1:	
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC
oc_endapicall2:	
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC
oc_endapicall3:	
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC
oc_endapicall4:	
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC

oc_endapicall5:	
	sub sp, sp, #16		@ stack clean up done by caller
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC

oc_endapicall6:	
	sub sp, sp, #16		@ stack clean up done by caller
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC

oc_endapicall7:	
	sub sp, sp, #16		@ stack clean up done by caller
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC

oc_endapicall8:	
	sub sp, sp, #16		@ stack clean up done by caller
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC

oc_incstp:
	add sp, sp, #4
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC

oc_decstp:
	sub sp, sp, #4
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC

oc_sipackargb:
	pop {r0}		@ pop alpha
	lsl r0, #24
	pop {r1}		@ pop red
	orr r0, r0, r1, LSL #16
	pop {r1}		@ pop green
	orr r0, r0, r1, LSL #8
	ldr r1, [sp, #0]	@ load blue
	orr r0, r0, r1
	str r0, [sp, #0]
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC

oc_sreti:
	str r0, [sp, #0]
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC

oc_sretf:
	str r0, [sp, #0]
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC

oc_pushs:
	push {sp}
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC
	
oc_ivandiv:
	ldr r0, [r8, #1023] 	@ dstvaraddr
	ldr r1, [r8, #1023] 	@ argvaraddr
	ldr r2, [r0]
	ldr r1, [r1]
	and r2, r2, r1
	str r2, [r0]
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC

oc_ivoriv:
	ldr r0, [r8, #1023] 	@ dstvaraddr
	ldr r1, [r8, #1023] 	@ argvaraddr
	ldr r2, [r0]
	ldr r1, [r1]
	orr r2, r2, r1
	str r2, [r0]
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC

oc_iveoriv:
	ldr r0, [r8, #1023] 	@ dstvaraddr
	ldr r1, [r8, #1023] 	@ argvaraddr
	ldr r2, [r0]
	ldr r1, [r1]
	eor r2, r2, r1
	str r2, [r0]
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC

oc_ivmodiv:
	ldr r4, [r8, #1023] 	@ dstvaraddr
	ldr r3, [r8, #1023] 	@ argvaraddr
	ldr r0, [r4]
	ldr r1, [r3]
	ldr r2, [r5, #0]	@ __aeabi_idivmod
	blx r2
	str r1, [r4]		@ r0=div r1=mod
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC

oc_ivaddiv:
	ldr r0, [r8, #1023] 	@ dstvaraddr
	ldr r1, [r8, #1023] 	@ argvaraddr
	ldr r2, [r0]
	ldr r1, [r1]
	add r2, r2, r1
	str r2, [r0]
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC

oc_ivsubiv:
	ldr r0, [r8, #1023] 	@ dstvaraddr
	ldr r1, [r8, #1023] 	@ argvaraddr
	ldr r2, [r0]
	ldr r1, [r1]
	sub r2, r2, r1
	str r2, [r0]
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC

oc_ivmuliv:
	ldr r0, [r8, #1023] 	@ dstvaraddr
	ldr r1, [r8, #1023] 	@ argvaraddr
	ldr r2, [r0]
	ldr r1, [r1]
	mul r2, r2, r1
	str r2, [r0]
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC

oc_ivdiviv:
	ldr r4, [r8, #1023] 	@ dstvaraddr
	ldr r1, [r8, #1023] 	@ argvaraddr
	ldr r0, [r4]
	ldr r1, [r1]
	ldr r3, [r5, #16]	@ __divsi3
	blx r3
	str r0, [r4]
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC

oc_ivasliv:
	ldr r0, [r8, #1023] 	@ dstvaraddr
	ldr r1, [r8, #1023] 	@ argvaraddr
	ldr r2, [r0]
	ldr r1, [r1]
	lsl r2, r2, r1
	str r2, [r0]
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC

oc_ivasriv:
	ldr r0, [r8, #1023] 	@ dstvaraddr
	ldr r1, [r8, #1023] 	@ argvaraddr
	ldr r2, [r0]
	ldr r1, [r1]
	asr r2, r2, r1
	str r2, [r0]
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC

oc_pushivandiv:
	ldr r0, [r8, #1023] 	@ dstvaraddr
	ldr r1, [r8, #1023] 	@ argvaraddr
	ldr r2, [r0]
	ldr r1, [r1]
	and r2, r2, r1
	push {r2}
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC

oc_pushivoriv:
	ldr r0, [r8, #1023] 	@ dstvaraddr
	ldr r1, [r8, #1023] 	@ argvaraddr
	ldr r2, [r0]
	ldr r1, [r1]
	orr r2, r2, r1
	push {r2}
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC

oc_pushiveoriv:
	ldr r0, [r8, #1023] 	@ dstvaraddr
	ldr r1, [r8, #1023] 	@ argvaraddr
	ldr r2, [r0]
	ldr r1, [r1]
	eor r2, r2, r1
	push {r2}
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC

oc_pushivmodiv:
	ldr r4, [r8, #1023] 	@ dstvaraddr
	ldr r3, [r8, #1023] 	@ argvaraddr
	ldr r0, [r4]
	ldr r1, [r3]
	ldr r2, [r5, #0]	@ __aeabi_idivmod
	blx r2
	push {r1}		@ r0=div r1=mod
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC

oc_pushivaddiv:
	ldr r0, [r8, #1023] 	@ dstvaraddr
	ldr r1, [r8, #1023] 	@ argvaraddr
	ldr r2, [r0]
	ldr r1, [r1]
	add r2, r2, r1
	push {r2}
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC

oc_pushivsubiv:
	ldr r0, [r8, #1023] 	@ dstvaraddr
	ldr r1, [r8, #1023] 	@ argvaraddr
	ldr r2, [r0]
	ldr r1, [r1]
	sub r2, r2, r1
	push {r2}
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC

oc_pushivmuliv:
	ldr r0, [r8, #1023] 	@ dstvaraddr
	ldr r1, [r8, #1023] 	@ argvaraddr
	ldr r2, [r0]
	ldr r1, [r1]
	mul r2, r2, r1
	push {r2}
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC

oc_pushivdiviv:
	ldr r4, [r8, #1023] 	@ dstvaraddr
	ldr r1, [r8, #1023] 	@ argvaraddr
	ldr r0, [r4]
	ldr r1, [r1]
	ldr r3, [r5, #16]	@ __divsi3
	blx r3
	push {r0}
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC

oc_pushivasliv:
	ldr r0, [r8, #1023] 	@ dstvaraddr
	ldr r1, [r8, #1023] 	@ argvaraddr
	ldr r2, [r0]
	ldr r1, [r1]
	lsl r2, r2, r1
	push {r2}
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC

oc_pushivasriv:
	ldr r0, [r8, #1023] 	@ dstvaraddr
	ldr r1, [r8, #1023] 	@ argvaraddr
	ldr r2, [r0]
	ldr r1, [r1]
	asr r2, r2, r1
	push {r2}
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC

oc_pushivandc:	
	ldr r0, [r8, #1023] 	@ dstvaraddr
	ldr r1, [r5, #1023] 	@ constval
	ldr r2, [r0]
	and r2, r2, r1
	push {r2}
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC

oc_pushivorc:
	ldr r0, [r8, #1023] 	@ dstvaraddr
	ldr r1, [r5, #1023] 	@ constval
	ldr r2, [r0]
	orr r2, r2, r1
	push {r2}
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC

oc_pushiveorc:
	ldr r0, [r8, #1023] 	@ dstvaraddr
	ldr r1, [r5, #1023] 	@ constval
	ldr r2, [r0]
	eor r2, r2, r1
	push {r2}
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC

oc_pushivmodc:
	ldr r0, [r8, #1023] 	@ dstvaraddr
	ldr r1, [r5, #1023] 	@ constval
	ldr r0, [r0]
	ldr r2, [r5, #0]	@ __aeabi_idivmod
	blx r2
	push {r1}		@ r0=div r1=mod
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC

oc_pushivaddc:
	ldr r0, [r8, #1023] 	@ dstvaraddr
	ldr r1, [r5, #1023] 	@ constval
	ldr r2, [r0]
	add r2, r2, r1
	push {r2}
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC

oc_pushivsubc:
	ldr r0, [r8, #1023] 	@ dstvaraddr
	ldr r1, [r5, #1023] 	@ constval
	ldr r2, [r0]
	sub r2, r2, r1
	push {r2}
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC

oc_pushivmulc:
	ldr r0, [r8, #1023] 	@ dstvaraddr
	ldr r1, [r5, #1023] 	@ constval
	ldr r2, [r0]
	mul r2, r2, r1
	push {r2}
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC

oc_pushivdivc:
	ldr r0, [r8, #1023] 	@ dstvaraddr
	ldr r1, [r5, #1023] 	@ constval
	ldr r0, [r0]
	ldr r3, [r5, #16]	@ __divsi3
	blx r3
	push {r0}
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC

oc_pushivaslc:
	ldr r0, [r8, #1023] 	@ dstvaraddr
	ldr r1, [r5, #1023] 	@ constval
	ldr r2, [r0]
	lsl r2, r2, r1
	push {r2}
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC

oc_pushivasrc:
	ldr r0, [r8, #1023] 	@ dstvaraddr
	ldr r1, [r5, #1023] 	@ constval
	ldr r2, [r0]
	asr r2, r2, r1
	push {r2}
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC

oc_fvaddfv:
	ldr r0, [r8, #1023]	@ dstvar
	ldr r1, [r8, #1023]  	@ argvar
	vldr s0, [r0]
	vldr s1, [r1]
	vadd.f32 s0, s0, s1
	vstr s0, [r0]		@ store dstvar
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC

oc_fvsubfv:
	ldr r0, [r8, #1023]	@ dstvar
	ldr r1, [r8, #1023]  	@ argvar
	vldr s0, [r0]
	vldr s1, [r1]
	vsub.f32 s0, s0, s1
	vstr s0, [r0]		@ store dstvar
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC

oc_fvmulfv:
	ldr r0, [r8, #1023]	@ dstvar
	ldr r1, [r8, #1023]  	@ argvar
	vldr s0, [r0]
	vldr s1, [r1]
	vmul.f32 s0, s0, s1
	vstr s0, [r0]		@ store dstvar
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC

oc_fvdivfv:
	ldr r0, [r8, #1023]	@ dstvar
	ldr r1, [r8, #1023]  	@ argvar
	vldr s0, [r0]
	vldr s1, [r1]
	vdiv.f32 s0, s0, s1
	vstr s0, [r0]		@ store dstvar
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC

oc_pushfvaddfv:
	ldr r0, [r8, #1023]	@ dstvar
	ldr r1, [r8, #1023]  	@ argvar
	vldr s0, [r0]
	vldr s1, [r1]
	vadd.f32 s0, s0, s1
	sub sp, sp, #4
	vstr s0, [sp, #0]
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC

oc_pushfvsubfv:
	ldr r0, [r8, #1023]	@ dstvar
	ldr r1, [r8, #1023]  	@ argvar
	vldr s0, [r0]
	vldr s1, [r1]
	vsub.f32 s0, s0, s1
	sub sp, sp, #4
	vstr s0, [sp, #0]
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC

oc_pushfvmulfv:
	ldr r0, [r8, #1023]	@ dstvar
	ldr r1, [r8, #1023]  	@ argvar
	vldr s0, [r0]
	vldr s1, [r1]
	vmul.f32 s0, s0, s1
	sub sp, sp, #4
	vstr s0, [sp, #0]
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC

oc_pushfvdivfv:
	ldr r0, [r8, #1023]	@ dstvar
	ldr r1, [r8, #1023]  	@ argvar
	vldr s0, [r0]
	vldr s1, [r1]
	vdiv.f32 s0, s0, s1
	sub sp, sp, #4
	vstr s0, [sp, #0]
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC

oc_pushfvaddc:
	ldr r0, [r8, #1023]	@ dstvar
	vldr s1, [r5, #1020]  	@ constval
	vldr s0, [r0]
	vadd.f32 s0, s0, s1
	sub sp, sp, #4
	vstr s0, [sp, #0]
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC

oc_pushfvsubc:
	ldr r0, [r8, #1023]	@ dstvar
	vldr s1, [r5, #1020]  	@ constval
	vldr s0, [r0]
	vsub.f32 s0, s0, s1
	sub sp, sp, #4
	vstr s0, [sp, #0]
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC

oc_pushfvmulc:
	ldr r0, [r8, #1023]	@ dstvar
	vldr s1, [r5, #1020]  	@ constval
	vldr s0, [r0]
	vmul.f32 s0, s0, s1
	sub sp, sp, #4
	vstr s0, [sp, #0]
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC

oc_pushfvdivc:
	ldr r0, [r8, #1023]	@ dstvar
	vldr s1, [r5, #1020]  	@ constval
	vldr s0, [r0]
	vdiv.f32 s0, s0, s1
	sub sp, sp, #4
	vstr s0, [sp, #0]
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC

oc_jgtiviv:
	ldr r0, [r8, #1023] 	@ lhs var
	ldr r1, [r8, #1023]	@ rhs var
	ldr r0, [r0]
	ldr r1, [r1]
	cmp r0, r1
	bgt ocx_fixme
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC

oc_jgeiviv:
	ldr r0, [r8, #1023] 	@ lhs var
	ldr r1, [r8, #1023]	@ rhs var
	ldr r0, [r0]
	ldr r1, [r1]
	cmp r0, r1
	bge ocx_fixme
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC

oc_jltiviv:
	ldr r0, [r8, #1023] 	@ lhs var
	ldr r1, [r8, #1023]	@ rhs var
	ldr r0, [r0]
	ldr r1, [r1]
	cmp r0, r1
	blt ocx_fixme
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC

oc_jleiviv:
	ldr r0, [r8, #1023] 	@ lhs var
	ldr r1, [r8, #1023]	@ rhs var
	ldr r0, [r0]
	ldr r1, [r1]
	cmp r0, r1
	ble ocx_fixme
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC

oc_jneiviv:
	ldr r0, [r8, #1023] 	@ lhs var
	ldr r1, [r8, #1023]	@ rhs var
	ldr r0, [r0]
	ldr r1, [r1]
	cmp r0, r1
	bne ocx_fixme
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC

oc_jeqiviv:
	ldr r0, [r8, #1023] 	@ lhs var
	ldr r1, [r8, #1023]	@ rhs var
	ldr r0, [r0]
	ldr r1, [r1]
	cmp r0, r1
	beq ocx_fixme
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC

oc_jgtivic:
	ldr r0, [r8, #1023] 	@ lhs var
	ldr r1, [r5, #1023]	@ rhs constval
	ldr r0, [r0]
	cmp r0, r1
	bgt ocx_fixme
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC

oc_jgeivic:
	ldr r0, [r8, #1023] 	@ lhs var
	ldr r1, [r5, #1023]	@ rhs constval
	ldr r0, [r0]
	cmp r0, r1
	bge ocx_fixme
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC

oc_jltivic:
	ldr r0, [r8, #1023] 	@ lhs var
	ldr r1, [r5, #1023]	@ rhs constval
	ldr r0, [r0]
	cmp r0, r1
	blt ocx_fixme
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC

oc_jleivic:
	ldr r0, [r8, #1023] 	@ lhs var
	ldr r1, [r5, #1023]	@ rhs constval
	ldr r0, [r0]
	cmp r0, r1
	ble ocx_fixme
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC

oc_jneivic:
	ldr r0, [r8, #1023] 	@ lhs var
	ldr r1, [r5, #1023]	@ rhs constval
	ldr r0, [r0]
	cmp r0, r1
	bne ocx_fixme
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC

oc_jeqivic:
	ldr r0, [r8, #1023] 	@ lhs var
	ldr r1, [r5, #1023]	@ rhs constval
	ldr r0, [r0]
	cmp r0, r1
	beq ocx_fixme
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC
	
ocx_epilogue:
	add sp, r6, #256	@ cleanup lvars
	pop {r3-r11,lr}
	.byte 0xC7, 0x05, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0xCC
