HANDLE_OPCODE(OP_MOVE_EXCEPTION /*vAA*/)
    vdst = INST_AA(inst);
    ILOGV("|move-exception v%d", vdst);
    assert(self->exception != NULL);
    SET_REGISTER(vdst, (u4)self->exception);
/* ifdef WITH_TAINT_TRACKING */
    SET_REGISTER_TAINT(vdst, TAINT_CLEAR);
/* endif */
    dvmClearException(self);
    FINISH(1);
OP_END
