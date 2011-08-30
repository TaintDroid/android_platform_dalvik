HANDLE_OPCODE($opcode /*vAAAA, vBBBB*/)
    vdst = FETCH(1);
    vsrc1 = FETCH(2);
    ILOGV("|move%s/16 v%d,v%d %s(v%d=0x%08x)",
        (INST_INST(inst) == OP_MOVE_16) ? "" : "-object", vdst, vsrc1,
        kSpacing, vdst, GET_REGISTER(vsrc1));
    SET_REGISTER(vdst, GET_REGISTER(vsrc1));
/* ifdef WITH_TAINT_TRACKING */
    SET_REGISTER_TAINT(vdst, GET_REGISTER_TAINT(vsrc1));
/* endif */
    FINISH(3);
OP_END
