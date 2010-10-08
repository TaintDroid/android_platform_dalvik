%verify "executed"
    /*
     * Signed 64-bit integer multiply, "/2addr" version.
     *
     * See OP_MUL_LONG for an explanation.
     *
     * We get a little tight on registers, so to avoid looking up &fp[A]
     * again we stuff it into rINST.
     */
    /* mul-long/2addr vA, vB */
    mov     r9, rINST, lsr #8           @ r9<- A+
    mov     r1, rINST, lsr #12          @ r1<- B
// begin WITH_TAINT_TRACKING
	bl		mul_long_2addr_taint_prop
// end WITH_TAINT_TRACKING
    umull   r9, r10, r2, r0             @  r9/r10 <- ZxX
    mla     r2, r0, r3, ip              @  r2<- YxX + (ZxW)
    mov     r0, rINST                   @ r0<- &fp[A] (free up rINST)
    FETCH_ADVANCE_INST(1)               @ advance rPC, load rINST
    add     r10, r2, r10                @  r10<- r10 + low(ZxW + (YxX))
    GET_INST_OPCODE(ip)                 @ extract opcode from rINST
// begin WITH_TAINT_TRACKING
//    stmia   r0, {r9-r10}                @ vAA/vAA+1<- r9/r10
    str		r9, [r0, #0]
    str		r10, [r0, #8]
    str		r10, [r0, #12]
    ldmfd   sp!, {r10}
    str		r10, [r0, #4]
// end WITH_TAINT_TRACKING
    GOTO_OPCODE(ip)                     @ jump to next instruction

%break

mul_long_2addr_taint_prop:
    and     r9, r9, #15
    add     r1, rFP, r1, lsl #3         @ r1<- &fp[B]
    add     rINST, rFP, r9, lsl #3      @ rINST<- &fp[A]
//    ldmia   r1, {r2-r3}                 @ r2/r3<- vBB/vBB+1
    ldr		r2, [r1, #0]
    ldr		r9, [r1, #4]
    ldr		r3, [r1, #8]
//    ldmia   rINST, {r0-r1}              @ r0/r1<- vAA/vAA+1
    ldr		r0, [rINST, #0]
    ldr		r10, [rINST, #4]
    ldr		r1, [rINST, #8]
	orr		r10, r9, r10
	stmfd   sp!, {r10}
	mul     ip, r2, r1                  @  ip<- ZxW
	bx		lr
