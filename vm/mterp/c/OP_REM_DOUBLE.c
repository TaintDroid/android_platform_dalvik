HANDLE_OPCODE(OP_REM_DOUBLE /*vAA, vBB, vCC*/)
    {
        u2 srcRegs;
        vdst = INST_AA(inst);
        srcRegs = FETCH(1);
        vsrc1 = srcRegs & 0xff;
        vsrc2 = srcRegs >> 8;
        ILOGV("|%s-double v%d,v%d,v%d", "mod", vdst, vsrc1, vsrc2);
        SET_REGISTER_DOUBLE(vdst,
            fmod(GET_REGISTER_DOUBLE(vsrc1), GET_REGISTER_DOUBLE(vsrc2)));
/* ifdef WITH_TAINT_TRACKING */
        SET_REGISTER_TAINT_DOUBLE(vdst,
	    (GET_REGISTER_TAINT_DOUBLE(vsrc1)|GET_REGISTER_TAINT_DOUBLE(vsrc2)));
/* endif */
    }
    FINISH(2);
OP_END
