// begin WITH_TAINT_TRACKING
%default { "postbarrier":"@ no-op ", "prebarrier":"@ no-op ", "volatile":"0" }
// end WITH_TAINT_TRACKING
%verify "executed"
%verify "field already resolved"
%verify "field not yet resolved"
%verify "field cannot be resolved"
    /*
     * 32-bit SPUT handler for objects
     *
     * for: sput-object, sput-object-volatile
     */
    /* op vAA, field@BBBB */
    ldr     r2, [rSELF, #offThread_methodClassDex]    @ r2<- DvmDex
    FETCH(r1, 1)                        @ r1<- field ref BBBB
    ldr     r10, [r2, #offDvmDex_pResFields] @ r10<- dvmDex->pResFields
    ldr     r0, [r10, r1, lsl #2]        @ r0<- resolved StaticField ptr
    cmp     r0, #0                      @ is resolved entry null?
    beq     .L${opcode}_resolve         @ yes, do resolve
.L${opcode}_finish:   @ field ptr in r0
    mov     r2, rINST, lsr #8           @ r2<- AA
    FETCH_ADVANCE_INST(2)               @ advance rPC, load rINST
    GET_VREG(r1, r2)                    @ r1<- fp[AA]
// begin WITH_TAINT_TRACKING
    SET_TAINT_FP(r3)
    GET_VREG_TAINT(r3, r2, r3)          @ r3<- taint
//    ldr     r2, [rSELF, #offThread_cardTable]  @ r2<- card table base
    ldr     r10, [rSELF, #offThread_cardTable]  @ r10<- card table base
// end WITH_TAINT_TRACKING
    ldr     r9, [r0, #offField_clazz]   @ r9<- field->clazz
    GET_INST_OPCODE(ip)                 @ extract opcode from rINST
    $prebarrier                        @ releasing store
    b       .L${opcode}_end
%break


.L${opcode}_end:
// begin WITH_TAINT_TRACKING
//    .if     $volatile
//    add	    r2, r0, #offStaticField_value       @ r2<- addr
//    mov	    r0, r1                              @ r0<- val
//    mov	    r1, r2                              @ r1<- addr
//    mov	    r2, r3                              @ r2<- taint
//    bl      dvmQuasiAtomicSwap32SfieldTaint
//    cmp     r0, #0                    	        @ stored a null object?
//    .else
    str     r1, [r0, #offStaticField_value] @ field<- vAA
    str	    r3, [r0, #offStaticField_taint]
//    $postbarrier
    cmp     r1, #0                              @ stored a null object?
//    .endif
//    strneb  r2, [r2, r9, lsr #GC_CARD_SHIFT]  @ mark card based on obj head
    strneb  r10, [r10, r9, lsr #GC_CARD_SHIFT]  @ mark card based on obj head
// end WITH_TAINT_TRACKING
    GOTO_OPCODE(ip)                     @ jump to next instruction

    /* Continuation if the field has not yet been resolved.
     * r1:  BBBB field ref
     * r10: dvmDex->pResFields
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
    b       .L${opcode}_finish          @ resume
