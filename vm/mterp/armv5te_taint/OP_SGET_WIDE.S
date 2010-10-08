%verify "executed"
%verify "field already resolved"
%verify "field not yet resolved"
%verify "field cannot be resolved"
    /*
     * 64-bit SGET handler.
     */
    /* sget-wide vAA, field@BBBB */
    ldr     r2, [rGLUE, #offGlue_methodClassDex]    @ r2<- DvmDex
    FETCH(r1, 1)                        @ r1<- field ref BBBB
    ldr     r2, [r2, #offDvmDex_pResFields] @ r2<- dvmDex->pResFields
    ldr     r0, [r2, r1, lsl #2]        @ r0<- resolved StaticField ptr
    cmp     r0, #0                      @ is resolved entry null?
    beq     .L${opcode}_resolve         @ yes, do resolve
    b		.L${opcode}_finish

%break

.L${opcode}_finish:
    mov     r1, rINST, lsr #8           @ r1<- AA
    ldrd    r2, [r0, #offStaticField_value] @ r2/r3<- field value (aligned)
// begin WITH_TAINT_TRACKING
    add     r1, rFP, r1, lsl #3         @ r1<- &fp[AA]
    ldr		r0, [r0, #offStaticField_taint] @ r0<- taint value
    FETCH_ADVANCE_INST(2)               @ advance rPC, load rINST
//    stmia   r1, {r2-r3}                 @ vAA/vAA+1<- r2/r3
	str		r2, [r1, #0]
	str		r0, [r1, #4]
	str		r3, [r1, #8]
	str		r0, [r1, #12]
// end WITH_TAINT_TRACKING
    GET_INST_OPCODE(ip)                 @ extract opcode from rINST
    GOTO_OPCODE(ip)                     @ jump to next instruction

    /*
     * Continuation if the field has not yet been resolved.
     *  r1: BBBB field ref
     */
.L${opcode}_resolve:
    ldr     r2, [rGLUE, #offGlue_method]    @ r2<- current method
    EXPORT_PC()                         @ resolve() could throw, so export now
    ldr     r0, [r2, #offMethod_clazz]  @ r0<- method->clazz
    bl      dvmResolveStaticField       @ r0<- resolved StaticField ptr
    cmp     r0, #0                      @ success?
    bne     .L${opcode}_finish          @ yes, finish
    b       common_exceptionThrown      @ no, handle exception

