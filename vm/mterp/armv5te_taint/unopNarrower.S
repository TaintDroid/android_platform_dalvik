%default {"preinstr":""}
    /*
     * Generic 64bit-to-32bit unary operation.  Provide an "instr" line
     * that specifies an instruction that performs "result = op r0/r1", where
     * "result" is a 32-bit quantity in r0.
     *
     * For: long-to-float, double-to-int, double-to-float
     *
     * (This would work for long-to-int, but that instruction is actually
     * an exact match for OP_MOVE.)
     */
    /* unop vA, vB */
    mov     r3, rINST, lsr #12          @ r3<- B
    mov     r9, rINST, lsr #8           @ r9<- A+
// begin WITH_TAINT_TRACKING
    add     r3, rFP, r3, lsl #3         @ r3<- &fp[B]
    and     r9, r9, #15
//    ldmia   r3, {r0-r1}                 @ r0/r1<- vB/vB+1
	ldr		r0, [r3, #0]
	ldr		r10, [r3, #4]
	ldr		r1, [r3, #8]
// end WITH_TAINT_TRACKING
    FETCH_ADVANCE_INST(1)               @ advance rPC, load rINST
    $preinstr                           @ optional op; may set condition codes
    $instr                              @ r0<- op, r0-r3 changed
    GET_INST_OPCODE(ip)                 @ extract opcode from rINST
// begin WITH_TAINT_TRACKING
    SET_VREG(r0, r9)                    @ vA<- r0
	SET_TAINT_FP(r1)
	SET_VREG_TAINT(r10, r9, r1)
// end WITH_TAINT_TRACKING
    GOTO_OPCODE(ip)                     @ jump to next instruction
    /* 10-11 instructions */
