%verify "executed"
    /* const/4 vA, #+B */
    mov     r1, rINST, lsl #16          @ r1<- Bxxx0000
    mov     r0, rINST, lsr #8           @ r0<- A+
    FETCH_ADVANCE_INST(1)               @ advance rPC, load rINST
    mov     r1, r1, asr #28             @ r1<- sssssssB (sign-extended)
    and     r0, r0, #15
// BEGIN WITH_TAINT_TRACKING
	SET_TAINT_FP(r2)
	SET_TAINT_CLEAR(r3)
    SET_VREG_TAINT(r3, r0, r2)
// END WITH_TAINT_TRACKING
    GET_INST_OPCODE(ip)                 @ ip<- opcode from rINST
    SET_VREG(r1, r0)                    @ fp[A]<- r1
    GOTO_OPCODE(ip)                     @ execute next instruction

