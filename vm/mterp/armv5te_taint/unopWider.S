%default {"preinstr":""}
    /*
     * Generic 32bit-to-64bit unary operation.  Provide an "instr" line
     * that specifies an instruction that performs "result = op r0", where
     * "result" is a 64-bit quantity in r0/r1.
     *
     * For: int-to-long, int-to-double, float-to-long, float-to-double
     */
    /* unop vA, vB */
    mov     r9, rINST, lsr #8           @ r9<- A+
    mov     r3, rINST, lsr #12          @ r3<- B
    and     r9, r9, #15
    GET_VREG(r0, r3)                    @ r0<- vB
// begin WITH_TAINT_TRACKING
	SET_TAINT_FP(r2)
	GET_VREG_TAINT(r10, r3, r2)
    add     r9, rFP, r9, lsl #3         @ r9<- &fp[A]
// end WITH_TAINT_TRACKING
    $preinstr                           @ optional op; may set condition codes
    FETCH_ADVANCE_INST(1)               @ advance rPC, load rINST
    $instr                              @ r0<- op, r0-r3 changed
    GET_INST_OPCODE(ip)                 @ extract opcode from rINST
	b     .L${opcode}_finish
%break

.L${opcode}_finish:
// begin WITH_TAINT_TRACKING
//    stmia   r9, {r0-r1}                 @ vA/vA+1<- r0/r1
	str		r0, [r9, #0]
	str		r10, [r9, #4]
	str		r1, [r9, #8]
	str		r10, [r9, #12]
// end WITH_TAINT_TRACKING
    GOTO_OPCODE(ip)                     @ jump to next instruction
    /* 10-11 instructions */
