%verify "executed"
%verify "exception handled"
    /*
     * Execute a "native inline" instruction.
     *
     * We need to call an InlineOp4Func:
     *  bool (func)(u4 arg0, u4 arg1, u4 arg2, u4 arg3, JValue* pResult)
     *
     * The first four args are in r0-r3, pointer to return value storage
     * is on the stack.  The function's return value is a flag that tells
     * us if an exception was thrown.
     *
     * TUNING: could maintain two tables, pointer in Thread and
     * swap if profiler/debuggger active.
     */
    /* [opt] execute-inline vAA, {vC, vD, vE, vF}, inline@BBBB */
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
    mov     r0, rINST, lsr #12          @ r0<- B
    str     r1, [sp]                    @ push &self->retval
    add     r1, rSELF, #offThread_rtaint  @ r1< &self->rtaint
    sub     sp, sp, #4			@ make room for arg,
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
     *
     * Other ideas:
     * - Use a jump table from the main piece to jump directly into the
     *   AND/LDR pairs.  Costs a data load, saves a branch.
     * - Have five separate pieces that do the loading, so we can work the
     *   interleave a little better.  Increases code size.
     */
.L${opcode}_continue:
    rsb     r0, r0, #4                  @ r0<- 4-r0
    FETCH(rINST, 2)                     @ rINST<- FEDC
    add     pc, pc, r0, lsl #3          @ computed goto, 2 instrs each
    bl      common_abort                @ (skipped due to ARM prefetch)
// begin WITH_TAINT_TRACKING
4:  and     ip, rINST, #0xf000          @ isolate F
    ldr     r3, [rFP, ip, lsr #9]       @ r3<- vF (shift right 12, left 2)
3:  and     ip, rINST, #0x0f00          @ isolate E
    ldr     r2, [rFP, ip, lsr #5]       @ r2<- vE
2:  and     ip, rINST, #0x00f0          @ isolate D
    ldr     r1, [rFP, ip, lsr #1]       @ r1<- vD
1:  and     ip, rINST, #0x000f          @ isolate C
    ldr     r0, [rFP, ip, lsl #3]       @ r0<- vC
0:
// push arg0_taint and arg1_taint
    SET_TAINT_FP(r11)
    and     ip, rINST, #0x00f0          @ isolate D
    ldr     ip, [r11, ip, lsr #1]       @ ip<-arg1_taint
    sub     sp, sp, #4			@ make room for arg
    str     ip, [sp]                    @ push arg1_taint
    and     ip, rINST, #0x000f          @ isolate C
    ldr     ip, [r11, ip, lsl #3]       @ ip<-arg0_taint
    sub     sp, sp, #4			@ make room for arg
    str     ip, [sp]                    @ push arg0_taint
// end WITH_TAINT_TRACKING
    ldr     rINST, .L${opcode}_table    @ table of InlineOperation
5:  add     rINST, pc
    ldr     pc, [rINST, r10, lsl #4]    @ sizeof=16, "func" is first entry
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
    mov     r0, rINST, lsr #12          @ r0<- B
    str     r1, [sp]                    @ push &self->retval
    add     r1, rSELF, #offThread_rtaint  @ r1< &self->rtaint
    sub     sp, sp, #4			@ make room for arg,
    str     r1, [sp]                    @ push &self->rtaint
// end WITH_TAINT_TRACKING
    bl      .L${opcode}_continue        @ make call; will return after
    mov     rINST, r0                   @ save result of inline
// begin WITH_TAINT_TRACKING
//    add     sp, sp, #8                  @ pop stack
    add     sp, sp, #16                 @ pop stack 4x
// end WITH_TAINT_TRACKING
    mov     r0, r9                      @ r0<- method
    mov     r1, rSELF
    bl      dvmFastNativeMethodTraceExit @ (method, self)
    cmp     rINST, #0                   @ test boolean result of inline
    beq     common_exceptionThrown      @ returned false, handle exception
    FETCH_ADVANCE_INST(3)               @ advance rPC, load rINST
    GET_INST_OPCODE(ip)                 @ extract opcode from rINST
    GOTO_OPCODE(ip)                     @ jump to next instruction




.L${opcode}_table:
    .word   PCREL_REF(gDvmInlineOpsTable,5b)
