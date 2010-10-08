HANDLE_OPCODE($opcode /*vA, vB*/)
    vdst = INST_A(inst);
    vsrc1 = INST_B(inst);
    ILOGV("|move%s v%d,v%d %s(v%d=0x%08x)",
        (INST_INST(inst) == OP_MOVE) ? "" : "-object", vdst, vsrc1,
        kSpacing, vdst, GET_REGISTER(vsrc1));
    SET_REGISTER(vdst, GET_REGISTER(vsrc1));
/* ifdef WITH_TAINT_TRACKING */
    SET_REGISTER_TAINT(vdst, GET_REGISTER_TAINT(vsrc1));
/* endif */
    FINISH(1);
OP_END
