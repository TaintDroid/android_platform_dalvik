%verify "executed"
    /* const-wide/16 vAA, #+BBBB */
    FETCH_S(r0, 1)                      @ r0<- ssssBBBB (sign-extended)
    mov     r3, rINST, lsr #8           @ r3<- AA
// begin WITH_TAINT_TRACKING
    mov     r2, r0, asr #31             @ r1<- ssssssss
// end WITH_TAINT_TRACKING
    FETCH_ADVANCE_INST(2)               @ advance rPC, load rINST
// begin WITH_TAINT_TRACKING
    add     r9, rFP, r3, lsl #3         @ r3<- &fp[AA]
// end WITH_TAINT_TRACKING
    GET_INST_OPCODE(ip)                 @ extract opcode from rINST
// begin WITH_TAINT_TRACKING
	SET_TAINT_CLEAR(r1)
	SET_TAINT_CLEAR(r3)
    stmia   r9, {r0-r3}                 @ vAA<- r0/r1
// end WITH_TAINT_TRACKING
    GOTO_OPCODE(ip)                     @ jump to next instruction

