HANDLE_OPCODE($opcode /*vAA*/)
    vsrc1 = INST_AA(inst);
    ILOGV("|return%s v%d",
        (INST_INST(inst) == OP_RETURN) ? "" : "-object", vsrc1);
    retval.i = GET_REGISTER(vsrc1);
/* ifdef WITH_TAINT_TRACKING */
    SET_RETURN_TAINT(GET_REGISTER_TAINT(vsrc1));
/* endif */
    GOTO_returnFromMethod();
OP_END
