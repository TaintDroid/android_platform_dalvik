%default {"preinstr":""}
    /*
     * Generic 64-bit unary operation.  Provide an "instr" line that
     * specifies an instruction that performs "result = op r0/r1".
     * This could be an ARM instruction or a function call.
     *
     * For: neg-long, not-long, neg-double, long-to-double, double-to-long
     */
    /* unop vA, vB */
    mov     r9, rINST, lsr #8           @ r9<- A+
    mov     r3, rINST, lsr #12          @ r3<- B
    and     r9, r9, #15
// begin WITH_TAINT_TRACKING
    add     r3, rFP, r3, lsl #3         @ r3<- &fp[B]
    add     r9, rFP, r9, lsl #3         @ r9<- &fp[A]
//    ldmia   r3, {r0-r1}                 @ r0/r1<- vAA
	ldr		r0, [r3, #0]
	ldr		r1, [r3, #8]
	ldr		r10, [r3, #4]
// end WITH_TAINT_TRACKING
    FETCH_ADVANCE_INST(1)               @ advance rPC, load rINST
    $preinstr                           @ optional op; may set condition codes
    $instr                              @ r0/r1<- op, r2-r3 changed
    GET_INST_OPCODE(ip)                 @ extract opcode from rINST
	b     .L${opcode}_finish
%break

.L${opcode}_finish:
// begin WITH_TAINT_TRACKING
//    stmia   r9, {r0-r1}                 @ vAA<- r0/r1
	str		r0, [r9, #0]
	str		r10, [r9, #4]
	str		r1, [r9, #8]
	str		r10, [r9, #12]
// end WITH_TAINT_TRACKING
    GOTO_OPCODE(ip)                     @ jump to next instruction
    /* 12-13 instructions */
