%verify "executed"
    /*
     * Long integer shift.  This is different from the generic 32/64-bit
     * binary operations because vAA/vBB are 64-bit but vCC (the shift
     * distance) is 32-bit.  Also, Dalvik requires us to mask off the low
     * 6 bits of the shift distance.
     */
    /* ushr-long vAA, vBB, vCC */
    FETCH(r0, 1)                        @ r0<- CCBB
    mov     r9, rINST, lsr #8           @ r9<- AA
    and     r3, r0, #255                @ r3<- BB
    mov     r0, r0, lsr #8              @ r0<- CC
// begin WITH_TAINT_TRACKING
	bl		ushr_long_taint_prop
// end WITH_TAINT_TRACKING

    mov     r0, r0, lsr r2              @  r0<- r2 >> r2
    rsb     r3, r2, #32                 @  r3<- 32 - r2
    orr     r0, r0, r1, asl r3          @  r0<- r0 | (r1 << (32-r2))
    subs    ip, r2, #32                 @  ip<- r2 - 32
    movpl   r0, r1, lsr ip              @  if r2 >= 32, r0<-r1 >>> (r2-32)
    FETCH_ADVANCE_INST(2)               @ advance rPC, load rINST
    b       .L${opcode}_finish
%break

.L${opcode}_finish:
    mov     r1, r1, lsr r2              @  r1<- r1 >>> r2
    GET_INST_OPCODE(ip)                 @ extract opcode from rINST
// begin WITH_TAINT_TRACKING
//    stmia   r9, {r0-r1}                 @ vAA/vAA+1<- r0/r1
	str		r0, [r9, #0]
	str		r10, [r9, #4]
	str		r1, [r9, #8]
	str		r10, [r9, #12]
// end WITH_TAINT_TRACKING
    GOTO_OPCODE(ip)                     @ jump to next instruction

ushr_long_taint_prop:
    add     r3, rFP, r3, lsl #3         @ r3<- &fp[BB]
    GET_VREG(r2, r0)                    @ r2<- vCC
	SET_TAINT_FP(r1)
	GET_VREG_TAINT(r0, r0, r1)
//    ldmia   r3, {r0-r1}                 @ r0/r1<- vBB/vBB+1
	ldr		r1, [r3, #4]
	orr		r10, r0, r1
	ldr		r0, [r3, #0]
	ldr		r1, [r3, #8]
    and     r2, r2, #63                 @ r0<- r0 & 0x3f
    add     r9, rFP, r9, lsl #3         @ r9<- &fp[AA]
    bx		lr
