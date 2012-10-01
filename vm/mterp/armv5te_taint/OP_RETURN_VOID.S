%verify "executed"
    SET_TAINT_CLEAR(r1)
    str     r1, [rSELF, #offThread_rtaint]
    b       common_returnFromMethod

