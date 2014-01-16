%verify "executed"
%verify "exception handled"
    /*
     * Execute a "native inline" instruction, using "/range" semantics.
     * Same idea as execute-inline, but we get the args differently.
     *
     * We need to call an InlineOp4Func:
     *  bool (func)(u4 arg0, u4 arg1, u4 arg2, u4 arg3, JValue* pResult)
     *
     * The first four args are in r0-r3, pointer to return value storage
     * is on the stack.  The function's return value is a flag that tells
     * us if an exception was thrown.
     */
    /* [opt] execute-inline/range {vCCCC..v(CCCC+AA-1)}, inline@BBBB */
    ldrh    r2, [rSELF, #offThread_subMode]
    FETCH(r10, 1)                       @ r10<- BBBB
    EXPORT_PC()                         @ can throw
    ands    r2, #kSubModeDebugProfile   @ Any going on?
    bne     .L${opcode}_debugmode       @ yes - take slow path
// begin WITH_TAINT_TRACKING
    b       .L${opcode}_resume
%break
// end WITH_TAINT_TRACKING

.L${opcode}_resume:
    add     r1, rSELF, #offThread_retval  @ r1<- &self->retval
// begin WITH_TAINT_TRACKING
//    sub     sp, sp, #8                  @ make room for arg, +64 bit align
    sub     sp, sp, #4                  @ make room for arg, +64 bit align
    mov     r0, rINST, lsr #8           @ r0<- AA
    str     r1, [sp]                    @ push &self->retval
    add     r1, rSELF, #offThread_rtaint  @ r1< &self->rtaint
    sub     sp, sp, #4			@ make room for arg
    str     r1, [sp]                    @ push &self->rtaint
    bl      .L${opcode}_continue        @ make call; will return after
    add     sp, sp, #16                 @ pop stack 4x
// end WITH_TAINT_TRACKING
    cmp     r0, #0                      @ test boolean result of inline
    beq     common_exceptionThrown      @ returned false, handle exception
    FETCH_ADVANCE_INST(3)               @ advance rPC, load rINST
    GET_INST_OPCODE(ip)                 @ extract opcode from rINST
    GOTO_OPCODE(ip)                     @ jump to next instruction
%break

    /*
     * Extract args, call function.
     *  r0 = #of args (0-4)
     *  r10 = call index
     *  lr = return addr, above  [DO NOT bl out of here w/o preserving LR]
     */
.L${opcode}_continue:
    rsb     r0, r0, #4                  @ r0<- 4-r0
    FETCH(r9, 2)                        @ r9<- CCCC
    add     pc, pc, r0, lsl #3          @ computed goto, 2 instrs each
    bl      common_abort                @ (skipped due to ARM prefetch)
4:  add     ip, r9, #3                  @ base+3
    GET_VREG(r3, ip)                    @ r3<- vBase[3]
3:  add     ip, r9, #2                  @ base+2
    GET_VREG(r2, ip)                    @ r2<- vBase[2]
2:  add     ip, r9, #1                  @ base+1
    GET_VREG(r1, ip)                    @ r1<- vBase[1]
1:  add     ip, r9, #0                  @ (nop)
    GET_VREG(r0, ip)                    @ r0<- vBase[0]
0:
// begin WITH_TAINT_TRACKING
// push arg0_taint and arg1_taint
    SET_TAINT_FP(r11)
    add     ip, r9, #1                  
    GET_VREG_TAINT(ip, ip, r11)
    sub     sp, sp, #4			@ make room for arg
    str     ip, [sp]                    @ push arg1_taint
    GET_VREG_TAINT(ip, r9, r11)
    sub     sp, sp, #4			@ make room for arg
    str     ip, [sp]                    @ push arg0_taint
// end WITH_TAINT_TRACKING
    ldr     r9, .L${opcode}_table       @ table of InlineOperation
5:  add     r9, pc
    ldr     pc, [r9, r10, lsl #4]       @ sizeof=16, "func" is first entry
    @ (not reached)


    /*
     * We're debugging or profiling.
     * r10: opIndex
     */
.L${opcode}_debugmode:
    mov     r0, r10
    bl      dvmResolveInlineNative
    cmp     r0, #0                      @ did it resolve?
    beq     .L${opcode}_resume          @ no, just move on
    mov     r9, r0                      @ remember method
    mov     r1, rSELF
    bl      dvmFastMethodTraceEnter     @ (method, self)
    add     r1, rSELF, #offThread_retval@ r1<- &self->retval
// begin WITH_TAINT_TRACKING
//    sub     sp, sp, #8                  @ make room for arg, +64 bit align
    sub     sp, sp, #4                  @ make room for arg, +64 bit align
    mov     r0, rINST, lsr #8           @ r0<- AA
    str     r1, [sp]                    @ push &self->retval
    add     r1, rSELF, #offThread_rtaint  @ r1< &self->rtaint
    sub     sp, sp, #4			@ make room for arg
    str     r1, [sp]                    @ push &self->rtaint
// end WITH_TAINT_TRACKING
    bl      .L${opcode}_continue        @ make call; will return after
    mov     r9, r0                      @ save result of inline
// begin WITH_TAINT_TRACKING
//    add     sp, sp, #8                  @ pop stack
    add     sp, sp, #16                 @ pop stack 4x
// end WITH_TAINT_TRACKING
    mov     r0, rINST                   @ r0<- method
    mov     r1, rSELF
    bl      dvmFastNativeMethodTraceExit  @ (method, self)
    cmp     r9, #0                      @ test boolean result of inline
    beq     common_exceptionThrown      @ returned false, handle exception
    FETCH_ADVANCE_INST(3)               @ advance rPC, load rINST
    GET_INST_OPCODE(ip)                 @ extract opcode from rINST
    GOTO_OPCODE(ip)                     @ jump to next instruction




.L${opcode}_table:
    .word   PCREL_REF(gDvmInlineOpsTable,5b)

