%default {"volatile":"0"}
%verify "executed"
%verify "field already resolved"
%verify "field not yet resolved"
%verify "field cannot be resolved"
    /*
     * 64-bit SPUT handler.
     */
    /* sput-wide vAA, field@BBBB */
    ldr     r0, [rGLUE, #offGlue_methodClassDex]  @ r0<- DvmDex
    FETCH(r1, 1)                        @ r1<- field ref BBBB
    ldr     r0, [r0, #offDvmDex_pResFields] @ r0<- dvmDex->pResFields
    mov     r9, rINST, lsr #8           @ r9<- AA
    ldr     r2, [r0, r1, lsl #2]        @ r2<- resolved StaticField ptr
// begin WITH_TAINT_TRACKING
    add     r9, rFP, r9, lsl #3         @ r9<- &fp[AA]
// end WITH_TAINT_TRACKING
    cmp     r2, #0                      @ is resolved entry null?
    beq     .L${opcode}_resolve         @ yes, do resolve
.L${opcode}_finish: @ field ptr in r2, AA in r9
    FETCH_ADVANCE_INST(2)               @ advance rPC, load rINST
// begin WITH_TAINT_TRACKING
    bl		.L${opcode}_taint_prop
// end WITH_TAINT_TRACKING
    GET_INST_OPCODE(r10)                @ extract opcode from rINST
    .if $volatile
    add     r2, r2, #offStaticField_value @ r2<- pointer to data
// begin WITH_TAINT_TRACKING
    stmfd   sp!, {r3}							@ push taint to stack
    bl      dvmQuasiAtomicSwap64FieldTaint		@ stores r0/r1 into addr r2
    ldmfd   sp!, {r3}
    //bl      dvmQuasiAtomicSwap64        @ stores r0/r1 into addr r2
    .else
    strd    r0, [r2, #offStaticField_value] @ field<- vAA/vAA+1
    str		r3, [r2, #offStaticField_taint]
    .endif
// end WITH_TAINT_TRACKING
    GOTO_OPCODE(r10)                    @ jump to next instruction
%break

    /*
     * Continuation if the field has not yet been resolved.
     *  r1: BBBB field ref
     *  r9: &fp[AA]
     *
     * Returns StaticField pointer in r2.
     */
.L${opcode}_resolve:
    ldr     r2, [rGLUE, #offGlue_method]    @ r2<- current method
    EXPORT_PC()                         @ resolve() could throw, so export now
    ldr     r0, [r2, #offMethod_clazz]  @ r0<- method->clazz
    bl      dvmResolveStaticField       @ r0<- resolved StaticField ptr
    cmp     r0, #0                      @ success?
    mov     r2, r0                      @ copy to r2
    bne     .L${opcode}_finish          @ yes, finish
    b       common_exceptionThrown      @ no, handle exception


.L${opcode}_taint_prop:
//    ldmia   r9, {r0-r1}                 @ r0/r1<- vAA/vAA+1
	ldr r3, [r9, #4]
	ldr r0, [r9, #0]
	ldr r1, [r9, #8]
	bx		lr
