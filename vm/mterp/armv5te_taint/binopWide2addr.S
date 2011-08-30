%default {"preinstr":"", "result0":"r0", "result1":"r1", "chkzero":"0"}
    /*
     * Generic 64-bit "/2addr" binary operation.  Provide an "instr" line
     * that specifies an instruction that performs "result = r0-r1 op r2-r3".
     * This could be an ARM instruction or a function call.  (If the result
     * comes back in a register other than r0, you can override "result".)
     *
     * If "chkzero" is set to 1, we perform a divide-by-zero check on
     * vCC (r1).  Useful for integer division and modulus.
     *
     * For: add-long/2addr, sub-long/2addr, div-long/2addr, rem-long/2addr,
     *      and-long/2addr, or-long/2addr, xor-long/2addr, add-double/2addr,
     *      sub-double/2addr, mul-double/2addr, div-double/2addr,
     *      rem-double/2addr
     */
    /* binop/2addr vA, vB */
    mov     r9, rINST, lsr #8           @ r9<- A+
    mov     r1, rINST, lsr #12          @ r1<- B
    and     r9, r9, #15
// begin WITH_TAINT_TRACKING
    bl		.L${opcode}_taint_prop
// end WITH_TAINT_TRACKING
    .if $chkzero
    orrs    ip, r2, r3                  @ second arg (r2-r3) is zero?
    beq     common_errDivideByZero
    .endif
    FETCH_ADVANCE_INST(1)               @ advance rPC, load rINST

    $preinstr                           @ optional op; may set condition codes
    $instr                              @ result<- op, r0-r3 changed
    GET_INST_OPCODE(ip)                 @ extract opcode from rINST
// begin WITH_TAINT_TRACKING
//    stmia   r9, {$result0,$result1}     @ vAA/vAA+1<- $result0/$result1
    str		$result0, [r9, #0]
    str		r10, [r9, #4]
    str		$result1, [r9, #8]
    str		r10, [r9, #12]
// end WITH_TAINT_TRACKING
    GOTO_OPCODE(ip)                     @ jump to next instruction
    /* 12-15 instructions */

%break

.L${opcode}_taint_prop:
    add     r1, rFP, r1, lsl #3         @ r1<- &fp[B]
    add     r9, rFP, r9, lsl #3         @ r9<- &fp[A]
//    ldmia   r1, {r2-r3}                 @ r2/r3<- vBB/vBB+1
    ldr		r2, [r1, #0]
    ldr		r10, [r1, #4]
    ldr		r3, [r1, #8]
//    ldmia   r9, {r0-r1}                 @ r0/r1<- vAA/vAA+1
    ldr		r0, [r9, #0]
    ldr		r1, [r9, #8]
    stmfd   sp!, {r9}
    ldr		r9, [r9, #4]
    orr		r10, r9, r10
    ldmfd   sp!, {r9}
	bx		lr
