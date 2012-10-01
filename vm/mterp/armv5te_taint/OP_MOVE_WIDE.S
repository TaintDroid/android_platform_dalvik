%verify "executed"
    /* move-wide vA, vB */
    /* NOTE: regs can overlap, e.g. "move v6,v7" or "move v7,v6" */
    mov     r2, rINST, lsr #8           @ r2<- A(+)
    mov     r3, rINST, lsr #12          @ r3<- B
    and     r2, r2, #15
// begin WITH_TAINT_TRACKING
    add     r3, rFP, r3, lsl #3         @ r3<- &fp[B]
    add     r9, rFP, r2, lsl #3         @ r9<- &fp[A]
    ldmia   r3, {r0-r3}                 @ r0/r1<- fp[B]
// end WITH_TAINT_TRACKING
    FETCH_ADVANCE_INST(1)               @ advance rPC, load rINST
    GET_INST_OPCODE(ip)                 @ extract opcode from rINST
// begin WITH_TAINT_TRACKING
    stmia   r9, {r0-r3}                 @ fp[A]<- r0/r1
// end WITH_TAINT_TRACKING
    GOTO_OPCODE(ip)                     @ jump to next instruction

