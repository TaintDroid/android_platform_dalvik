%verify "executed"
    /* for move, move-object, long-to-int */
    /* op vA, vB */
    mov     r1, rINST, lsr #12          @ r1<- B from 15:12
    mov     r0, rINST, lsr #8           @ r0<- A from 11:8
    FETCH_ADVANCE_INST(1)               @ advance rPC, load rINST
    GET_VREG(r2, r1)                    @ r2<- fp[B]
    and     r0, r0, #15
// BEGIN WITH_TAINT_TRACKING
	SET_TAINT_FP(r3)
	GET_VREG_TAINT(r1, r1, r3)
	SET_VREG_TAINT(r1, r0, r3)
// END WITH_TAINT_TRACKING
    GET_INST_OPCODE(ip)                 @ ip<- opcode from rINST
    SET_VREG(r2, r0)                    @ fp[A]<- r2
    GOTO_OPCODE(ip)                     @ execute next instruction

