%verify "executed"
    /* move-result-wide vAA */
    mov     r2, rINST, lsr #8           @ r2<- AA
    add     r3, rGLUE, #offGlue_retval  @ r3<- &glue->retval
// begin WITH_TAINT_TRACKING
    add     r2, rFP, r2, lsl #3         @ r2<- &fp[AA]
// end WITH_TAINT_TRACKING
    ldmia   r3, {r0-r1}                 @ r0/r1<- retval.j
    FETCH_ADVANCE_INST(1)               @ advance rPC, load rINST
    GET_INST_OPCODE(ip)                 @ extract opcode from rINST
// begin WITH_TAINT_TRACKING
//    stmia   r2, {r0-r1}                 @ fp[AA]<- r0/r1
	ldr r3, [rGLUE, #offGlue_rtaint]
	str	r0, [r2, #0]
	str r3, [r2, #4]
	str r1, [r2, #8]
	str r3, [r2, #12]
// end WITH_TAINT_TRACKING
    GOTO_OPCODE(ip)                     @ jump to next instruction

