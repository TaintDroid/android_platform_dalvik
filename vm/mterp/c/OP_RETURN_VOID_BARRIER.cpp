HANDLE_OPCODE(OP_RETURN_VOID_BARRIER /**/)
    ILOGV("|return-void");
#ifndef NDEBUG
    retval.j = 0xababababULL;   /* placate valgrind */
#endif
/* ifdef WITH_TAINT_TRACKING */
    SET_RETURN_TAINT(TAINT_CLEAR);
/* endif */
    ANDROID_MEMBAR_STORE();
    GOTO_returnFromMethod();
OP_END
