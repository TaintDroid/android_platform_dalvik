%verify "executed"
    /* const-wide vAA, #+HHHHhhhhBBBBbbbb */
    FETCH(r0, 1)                        @ r0<- bbbb (low)
    FETCH(r1, 2)                        @ r1<- BBBB (low middle)
    FETCH(r2, 3)                        @ r2<- hhhh (high middle)
    orr     r0, r0, r1, lsl #16         @ r0<- BBBBbbbb (low word)
    FETCH(r3, 4)                        @ r3<- HHHH (high)
    mov     r9, rINST, lsr #8           @ r9<- AA
// begin WITH_TAINT_TRACKING
    orr     r2, r2, r3, lsl #16         @ r2<- HHHHhhhh (high word)
// end WITH_TAINT_TRACKING
    FETCH_ADVANCE_INST(5)               @ advance rPC, load rINST
// begin WITH_TAINT_TRACKING
    add     r9, rFP, r9, lsl #3         @ r9<- &fp[AA]
// end WITH_TAINT_TRACKING
    GET_INST_OPCODE(ip)                 @ extract opcode from rINST
// begin WITH_TAINT_TRACKING
	SET_TAINT_CLEAR(r1)
	SET_TAINT_CLEAR(r3)
    stmia   r9, {r0-r3}                 @ vAA<- r0/r1
// end WITH_TAINT_TRACKING
    GOTO_OPCODE(ip)                     @ jump to next instruction

