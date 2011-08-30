%verify "executed"
    /* const-wide/32 vAA, #+BBBBbbbb */
    FETCH(r0, 1)                        @ r0<- 0000bbbb (low)
    mov     r3, rINST, lsr #8           @ r3<- AA
    FETCH_S(r2, 2)                      @ r2<- ssssBBBB (high)
    FETCH_ADVANCE_INST(3)               @ advance rPC, load rINST
    orr     r0, r0, r2, lsl #16         @ r0<- BBBBbbbb
// begin WITH_TAINT_TRACKING
    add     r9, rFP, r3, lsl #3         @ r9<- &fp[AA]
    mov     r2, r0, asr #31             @ r2<- ssssssss
// end WITH_TAINT_TRACKING
    GET_INST_OPCODE(ip)                 @ extract opcode from rINST
// begin WITH_TAINT_TRACKING
	SET_TAINT_CLEAR(r1)
	SET_TAINT_CLEAR(r3)
    stmia   r9, {r0-r3}                 @ vAA<- r0/r1
// end WITH_TAINT_TRACKING
    GOTO_OPCODE(ip)                     @ jump to next instruction

