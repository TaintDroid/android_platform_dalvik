    /*
     * Generic 64-bit floating point "/2addr" binary operation.  Provide
     * an "instr" line that specifies an instruction that performs
     * "d2 = d0 op d1".
     *
     * For: add-double/2addr, sub-double/2addr, mul-double/2addr,
     *      div-double/2addr
     */
    /* binop/2addr vA, vB */
    mov     r3, rINST, lsr #12          @ r3<- B
    mov     r9, rINST, lsr #8           @ r9<- A+
    VREG_INDEX_TO_ADDR(r3, r3)          @ r3<- &vB
    and     r9, r9, #15                 @ r9<- A
// begin WITH_TAINT_TRACKING
//    fldd    d1, [r3]                    @ d1<- vB
	flds	s2, [r3]
	flds	s3, [r3, #8]
	ldr		r0, [r3, #4]
// end WITH_TAINT_TRACKING
    VREG_INDEX_TO_ADDR(r9, r9)          @ r9<- &vA
    FETCH_ADVANCE_INST(1)               @ advance rPC, load rINST
// begin WITH_TAINT_TRACKING
//    fldd    d0, [r9]                    @ d0<- vA
    flds	s0, [r9]
    flds	s1, [r9, #8]
    ldr		r1, [r9, #4]
// end WITH_TAINT_TRACKING
	b     .L${opcode}_finish
%break

.L${opcode}_finish:
    $instr                              @ d2<- op
    GET_INST_OPCODE(ip)                 @ extract opcode from rINST
// begin WITH_TAINT_TRACKING
	orr		r0, r0, r1
//    fstd    d2, [r9]                    @ vAA<- d2
	fsts	s4, [r9]
	fsts	s5, [r9, #8]
	str		r0, [r9, #4]
	str		r0, [r9, #12]
// end WITH_TAINT_TRACKING
    GOTO_OPCODE(ip)                     @ jump to next instruction
