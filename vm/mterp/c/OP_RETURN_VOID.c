HANDLE_OPCODE(OP_RETURN_VOID /**/)
    ILOGV("|return-void");
#ifndef NDEBUG
    retval.j = 0xababababULL;    // placate valgrind
#endif
/* ifdef WITH_TAINT_TRACKING */
    SET_RETURN_TAINT(TAINT_CLEAR);
/* endif */
    GOTO_returnFromMethod();
OP_END
