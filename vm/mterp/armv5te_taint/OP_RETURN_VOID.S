%verify "executed"
	SET_TAINT_CLEAR(r1)
	str		r1, [rGLUE, #offGlue_rtaint]
    b       common_returnFromMethod

