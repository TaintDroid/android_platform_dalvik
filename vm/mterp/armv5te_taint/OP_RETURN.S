%verify "executed"
    /*
     * Return a 32-bit value.  Copies the return value into the "glue"
     * structure, then jumps to the return handler.
     *
     * for: return, return-object
     */
    /* op vAA */
    mov     r2, rINST, lsr #8           @ r2<- AA
    GET_VREG(r0, r2)                    @ r0<- vAA
// begin WITH_TAINT_TRACKING
	SET_TAINT_FP(r1)
	GET_VREG_TAINT(r3, r2, r1)
	str		r3, [rGLUE, #offGlue_rtaint]
// end WITH_TAINT_TRACKING
    str     r0, [rGLUE, #offGlue_retval] @ retval.i <- vAA
    b       common_returnFromMethod

