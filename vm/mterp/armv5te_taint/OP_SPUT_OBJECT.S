// begin WITH_TAINT_TRACKING
%default { "barrier":"@ no-op ", "volatile":"0"}
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
    ldr     r2, [rGLUE, #offGlue_methodClassDex]    @ r2<- DvmDex
    FETCH(r1, 1)                        @ r1<- field ref BBBB
    ldr     r2, [r2, #offDvmDex_pResFields] @ r2<- dvmDex->pResFields
    ldr     r0, [r2, r1, lsl #2]        @ r0<- resolved StaticField ptr
    cmp     r0, #0                      @ is resolved entry null?
    bne     .L${opcode}_finish          @ no, continue
    ldr     r9, [rGLUE, #offGlue_method]    @ r9<- current method
    EXPORT_PC()                         @ resolve() could throw, so export now
    ldr     r0, [r9, #offMethod_clazz]  @ r0<- method->clazz
    bl      dvmResolveStaticField       @ r0<- resolved StaticField ptr
    cmp     r0, #0                      @ success?
    bne     .L${opcode}_finish          @ yes, finish
    b       common_exceptionThrown      @ no, handle exception

%break
.L${opcode}_finish:   @ field ptr in r0
    mov     r2, rINST, lsr #8           @ r2<- AA
    FETCH_ADVANCE_INST(2)               @ advance rPC, load rINST
    GET_VREG(r1, r2)                    @ r1<- fp[AA]
// begin WITH_TAINT_TRACKING
	SET_TAINT_FP(r3)
	GET_VREG_TAINT(r3, r2, r3)			@ r3<- taint
    ldr     r10, [rGLUE, #offGlue_cardTable]  @ r10<- card table base
    ldr     r9, [r0, #offField_clazz]   @ r9<- field->clazz
    GET_INST_OPCODE(ip)                 @ extract opcode from rINST
    .if     $volatile
    add		r2, r0, #offStaticField_value	@ r2<- addr
    mov		r0, r1							@ r0<- val
    mov		r1, r2							@ r1<- addr
    mov		r2, r3							@ r2<- taint
    bl      dvmQuasiAtomicSwap32SfieldTaint
    cmp     r0, #0                    		@ stored a null object?
    .else
    $barrier                            	@ releasing store
    str     r1, [r0, #offStaticField_value] @ field<- vAA
	str		r3, [r0, #offStaticField_taint]
	cmp     r1, #0                      	@ stored a null object?
	.endif
// end WITH_TAINT_TRACKING
    strneb  r10, [r10, r9, lsr #GC_CARD_SHIFT]  @ mark card based on obj head
    GOTO_OPCODE(ip)                     @ jump to next instruction

