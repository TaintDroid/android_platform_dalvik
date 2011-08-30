%verify "executed"
    /* for: move/from16, move-object/from16 */
    /* op vAA, vBBBB */
    FETCH(r1, 1)                        @ r1<- BBBB
    mov     r0, rINST, lsr #8           @ r0<- AA
    FETCH_ADVANCE_INST(2)               @ advance rPC, load rINST
    GET_VREG(r2, r1)                    @ r2<- fp[BBBB]
// BEGIN WITH_TAINT_TRACKING
	SET_TAINT_FP(r3)
	GET_VREG_TAINT(r1, r1, r3)
	SET_VREG_TAINT(r1, r0, r3)
// END WITH_TAINT_TRACKING
    GET_INST_OPCODE(ip)                 @ extract opcode from rINST
    SET_VREG(r2, r0)                    @ fp[AA]<- r2
    GOTO_OPCODE(ip)                     @ jump to next instruction

