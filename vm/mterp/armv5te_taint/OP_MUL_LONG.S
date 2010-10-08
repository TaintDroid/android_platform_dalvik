%verify "executed"
    /*
     * Signed 64-bit integer multiply.
     *
     * Consider WXxYZ (r1r0 x r3r2) with a long multiply:
     *        WX
     *      x YZ
     *  --------
     *     ZW ZX
     *  YW YX
     *
     * The low word of the result holds ZX, the high word holds
     * (ZW+YX) + (the high overflow from ZX).  YW doesn't matter because
     * it doesn't fit in the low 64 bits.
     *
     * Unlike most ARM math operations, multiply instructions have
     * restrictions on using the same register more than once (Rd and Rm
     * cannot be the same).
     */
    /* mul-long vAA, vBB, vCC */
    FETCH(r0, 1)                        @ r0<- CCBB
    and     r2, r0, #255                @ r2<- BB
    mov     r3, r0, lsr #8              @ r3<- CC
// begin WITH_TAINT_TRACKING
    bl		mul_long_taint_prop
// end WITH_TAINT_TRACKING
    mul     ip, r2, r1                  @  ip<- ZxW
    umull   r9, r10, r2, r0             @  r9/r10 <- ZxX
    mla     r2, r0, r3, ip              @  r2<- YxX + (ZxW)
    mov     r0, rINST, lsr #8           @ r0<- AA
    add     r10, r2, r10                @  r10<- r10 + low(ZxW + (YxX))
// begin WITH_TAINT_TRACKING
    add     r0, rFP, r0, lsl #3         @ r0<- &fp[AA]
// end WITH_TAINT_TRACKING
    FETCH_ADVANCE_INST(2)               @ advance rPC, load rINST
    b       .L${opcode}_finish
%break

.L${opcode}_finish:
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

mul_long_taint_prop:
    add     r2, rFP, r2, lsl #3         @ r2<- &fp[BB]
    add     r3, rFP, r3, lsl #3         @ r3<- &fp[CC]
//    ldmia   r2, {r0-r1}                 @ r0/r1<- vBB/vBB+1
    ldr		r0, [r2, #0]
    ldr		r9, [r2, #4]
    ldr		r1, [r2, #8]
//    ldmia   r3, {r2-r3}                 @ r2/r3<- vCC/vCC+1
    ldr		r2, [r3, #0]
    ldr		r10, [r3, #4]
    ldr		r3, [r3, #8]
    orr		r10, r9, r10
	stmfd   sp!, {r10}
	bx		lr
