HANDLE_OPCODE(OP_REM_FLOAT_2ADDR /*vA, vB*/)
    vdst = INST_A(inst);
    vsrc1 = INST_B(inst);
    ILOGV("|%s-float-2addr v%d,v%d", "mod", vdst, vsrc1);
    SET_REGISTER_FLOAT(vdst,
        fmodf(GET_REGISTER_FLOAT(vdst), GET_REGISTER_FLOAT(vsrc1)));
/* ifdef WITH_TAINT_TRACKING */
        SET_REGISTER_TAINT_FLOAT(vdst,
	    (GET_REGISTER_TAINT_FLOAT(vdst)|GET_REGISTER_TAINT_FLOAT(vsrc1)));
/* endif */
    FINISH(1);
OP_END
