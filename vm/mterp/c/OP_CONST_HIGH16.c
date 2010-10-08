HANDLE_OPCODE(OP_CONST_HIGH16 /*vAA, #+BBBB0000*/)
    vdst = INST_AA(inst);
    vsrc1 = FETCH(1);
    ILOGV("|const/high16 v%d,#0x%04x0000", vdst, vsrc1);
    SET_REGISTER(vdst, vsrc1 << 16);
/* ifdef WITH_TAINT_TRACKING */
    SET_REGISTER_TAINT(vdst, TAINT_CLEAR);
/* endif */
    FINISH(2);
OP_END
