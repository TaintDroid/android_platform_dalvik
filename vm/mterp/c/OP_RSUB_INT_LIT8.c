HANDLE_OPCODE(OP_RSUB_INT_LIT8 /*vAA, vBB, #+CC*/)
    {
        u2 litInfo;
        vdst = INST_AA(inst);
        litInfo = FETCH(1);
        vsrc1 = litInfo & 0xff;
        vsrc2 = litInfo >> 8;
        ILOGV("|%s-int/lit8 v%d,v%d,#+0x%02x", "rsub", vdst, vsrc1, vsrc2);
        SET_REGISTER(vdst, (s1) vsrc2 - (s4) GET_REGISTER(vsrc1));
/* ifdef WITH_TAINT_TRACKING */
        SET_REGISTER_TAINT(vdst, GET_REGISTER_TAINT(vsrc1));
/* endif */
    }
    FINISH(2);
OP_END
