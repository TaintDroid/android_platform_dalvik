%verify "executed"
// begin WITH_TAINT_TRACKING
    SET_TAINT_CLEAR(r1)
    str     r1, [rSELF, #offThread_rtaint]
// end WITH_TAINT_TRACKING
    b       common_returnFromMethod
