%verify "executed"
    /* move-wide/16 vAAAA, vBBBB */
    /* NOTE: regs can overlap, e.g. "move v6,v7" or "move v7,v6" */
    FETCH(r3, 2)                        @ r3<- BBBB
    FETCH(r2, 1)                        @ r2<- AAAA
// begin WITH_TAINT_TRACKING
    add     r3, rFP, r3, lsl #3         @ r3<- &fp[BBBB]
    add     r9, rFP, r2, lsl #3         @ r9<- &fp[AAAA]
    ldmia   r3, {r0-r3}                 @ r0/r1<- fp[BBBB]
// end WITH_TAINT_TRACKING
    FETCH_ADVANCE_INST(3)               @ advance rPC, load rINST
    GET_INST_OPCODE(ip)                 @ extract opcode from rINST
// begin WITH_TAINT_TRACKING
    stmia   r9, {r0-r3}                 @ fp[AAAA]<- r0/r1
// end WITH_TAINT_TRACKING
    GOTO_OPCODE(ip)                     @ jump to next instruction

