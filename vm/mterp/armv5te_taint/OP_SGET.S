// begin WITH_TAINT_TRACKING
%default { "barrier":"@ no-op ", "volatile":"0"}
// end WITH_TAINT_TRACKING
%verify "executed"
%verify "field already resolved"
%verify "field not yet resolved"
%verify "field cannot be resolved"
    /*
     * General 32-bit SGET handler.
     *
     * for: sget, sget-object, sget-boolean, sget-byte, sget-char, sget-short
     */
    /* op vAA, field@BBBB */
    ldr     r2, [rSELF, #offThread_methodClassDex]    @ r2<- DvmDex
    FETCH(r1, 1)                        @ r1<- field ref BBBB
    ldr     r10, [r2, #offDvmDex_pResFields] @ r10<- dvmDex->pResFields
    ldr     r0, [r10, r1, lsl #2]       @ r0<- resolved StaticField ptr
    cmp     r0, #0                      @ is resolved entry null?
    beq     .L${opcode}_resolve         @ yes, do resolve
.L${opcode}_finish: @ field ptr in r0
// begin WITH_TAINT_TRACKING
    bl		.L${opcode}_taint_prop
    FETCH_ADVANCE_INST(2)               @ advance rPC, load rINST
    SET_VREG(r0, r2)                    @ fp[AA]<- r0
// end WITH_TAINT_TRACKING
    GET_INST_OPCODE(ip)                 @ extract opcode from rINST
    GOTO_OPCODE(ip)                     @ jump to next instruction
%break

    /*
     * Continuation if the field has not yet been resolved.
     *  r1:  BBBB field ref
     *  r10: dvmDex->pResFields
     */
.L${opcode}_resolve:
    ldr     r2, [rSELF, #offThread_method]    @ r2<- current method
#if defined(WITH_JIT)
    add     r10, r10, r1, lsl #2        @ r10<- &dvmDex->pResFields[field]
#endif
    EXPORT_PC()                         @ resolve() could throw, so export now
    ldr     r0, [r2, #offMethod_clazz]  @ r0<- method->clazz
    bl      dvmResolveStaticField       @ r0<- resolved StaticField ptr
    cmp     r0, #0                      @ success?
    beq     common_exceptionThrown      @ no, handle exception
#if defined(WITH_JIT)
    /*
     * If the JIT is actively building a trace we need to make sure
     * that the field is fully resolved before including this instruction.
     */
    bl      common_verifyField
#endif
    b       .L${opcode}_finish


.L${opcode}_taint_prop:
//    .if     $volatile
//    add     r0, r0, #offStaticField_value		@ r0<- address of field
//    bl      dvmQuasiAtomicRead32SfieldTaint		@ r0/r1<- value/taint
//    .else
    ldr	    r1, [r0, #offStaticField_taint] @ r1<- taint value
    ldr     r0, [r0, #offStaticField_value] @ r0<- field value
    $barrier                            @ acquiring load
//    .endif
    mov     r2, rINST, lsr #8           @ r2<- AA
    SET_TAINT_FP(r3)
    SET_VREG_TAINT(r1, r2, r3)
    bx      lr
