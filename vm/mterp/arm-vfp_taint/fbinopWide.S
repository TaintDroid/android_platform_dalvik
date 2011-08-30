    /*
     * Generic 64-bit double-precision floating point binary operation.
     * Provide an "instr" line that specifies an instruction that performs
     * "d2 = d0 op d1".
     *
     * for: add-double, sub-double, mul-double, div-double
     */
    /* doubleop vAA, vBB, vCC */
    FETCH(r0, 1)                        @ r0<- CCBB
    mov     r9, rINST, lsr #8           @ r9<- AA
    mov     r3, r0, lsr #8              @ r3<- CC
    and     r2, r0, #255                @ r2<- BB
    VREG_INDEX_TO_ADDR(r3, r3)          @ r3<- &vCC
    VREG_INDEX_TO_ADDR(r2, r2)          @ r2<- &vBB
// begin WITH_TAINT_TRACKING
//    fldd    d1, [r3]                    @ d1<- vCC
//    fldd    d0, [r2]                    @ d0<- vBB
	flds	s2, [r3]
	flds	s3, [r3, #8]
	flds	s0, [r2]
	flds	s1, [r2, #8]
	ldr		r0, [r3, #4]
	ldr		r1, [r2, #4]
	orr		r0, r0, r1
// end WITH_TAINT_TRACKING
	b     .L${opcode}_finish
%break

.L${opcode}_finish:
    FETCH_ADVANCE_INST(2)               @ advance rPC, load rINST
    $instr                              @ s2<- op
    GET_INST_OPCODE(ip)                 @ extract opcode from rINST
    VREG_INDEX_TO_ADDR(r9, r9)          @ r9<- &vAA
// begin WITH_TAINT_TRACKING
//    fstd    d2, [r9]                    @ vAA<- d2
	fsts	s4, [r9]
	fsts	s5, [r9, #8]
	str		r0, [r9, #4]
	str		r0, [r9, #12]
// end WITH_TAINT_TRACKING
    GOTO_OPCODE(ip)                     @ jump to next instruction
