%verify "executed"
    /* move-wide/from16 vAA, vBBBB */
    /* NOTE: regs can overlap, e.g. "move v6,v7" or "move v7,v6" */
    FETCH(r3, 1)                        @ r3<- BBBB
    mov     r2, rINST, lsr #8           @ r2<- AA
// begin WITH_TAINT_TRACKING
    add     r3, rFP, r3, lsl #3         @ r3<- &fp[BBBB]
    add     r9, rFP, r2, lsl #3         @ r9<- &fp[AA]
    ldmia   r3, {r0-r3}                 @ r0/r1<- fp[BBBB]
// end WITH_TAINT_TRACKING
    FETCH_ADVANCE_INST(2)               @ advance rPC, load rINST
    GET_INST_OPCODE(ip)                 @ extract opcode from rINST
// begin WITH_TAINT_TRACKING
    stmia   r9, {r0-r3}                 @ fp[AA]<- r0/r1
// end WITH_TAINT_TRACKING
    GOTO_OPCODE(ip)                     @ jump to next instruction

