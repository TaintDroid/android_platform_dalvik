HANDLE_OPCODE(OP_EXECUTE_INLINE_RANGE /*{vCCCC..v(CCCC+AA-1)}, inline@BBBB*/)
    {
        u4 arg0, arg1, arg2, arg3;
        arg0 = arg1 = arg2 = arg3 = 0;      /* placate gcc */

#ifdef WITH_TAINT_TRACKING
	u4 arg0_taint, arg1_taint;
	arg0_taint = arg1_taint = 0;
#endif /*WITH_TAINT_TRACKING*/

        EXPORT_PC();

        vsrc1 = INST_AA(inst);      /* #of args */
        ref = FETCH(1);             /* inline call "ref" */
        vdst = FETCH(2);            /* range base */
        ILOGV("|execute-inline-range args=%d @%d {regs=v%d-v%d}",
            vsrc1, ref, vdst, vdst+vsrc1-1);

        assert((vdst >> 16) == 0);  // 16-bit type -or- high 16 bits clear
        assert(vsrc1 <= 4);

        switch (vsrc1) {
        case 4:
            arg3 = GET_REGISTER(vdst+3);
            /* fall through */
        case 3:
            arg2 = GET_REGISTER(vdst+2);
            /* fall through */
        case 2:
            arg1 = GET_REGISTER(vdst+1);
#ifdef WITH_TAINT_TRACKING
	    arg1_taint = GET_REGISTER_TAINT(vdst+1);
#endif /*WITH_TAINT_TRACKING*/
            /* fall through */
        case 1:
            arg0 = GET_REGISTER(vdst+0);
#ifdef WITH_TAINT_TRACKING
            arg0_taint = GET_REGISTER_TAINT(vdst+0);
#endif /*WITH_TAINT_TRACKING*/
            /* fall through */
        default:        // case 0
            ;
        }

        if (self->interpBreak.ctl.subMode & kSubModeDebugProfile) {
#ifdef WITH_TAINT_TRACKING
            if (!dvmPerformInlineOp4Dbg(arg0, arg1, arg2, arg3, arg0_taint, arg1_taint, &rtaint, &retval, ref))
                GOTO_exceptionThrown();
#else
            if (!dvmPerformInlineOp4Dbg(arg0, arg1, arg2, arg3, &retval, ref))
                GOTO_exceptionThrown();
#endif /*WITH_TAINT_TRACKING*/
        } else {
#ifdef WITH_TAINT_TRACKING
            if (!dvmPerformInlineOp4Std(arg0, arg1, arg2, arg3, arg0_taint, arg1_taint, &rtaint, &retval, ref))
                GOTO_exceptionThrown();
#else
            if (!dvmPerformInlineOp4Std(arg0, arg1, arg2, arg3, &retval, ref))
                GOTO_exceptionThrown();
#endif /*WITH_TAINT_TRACKING*/
        }
    }
    FINISH(3);
OP_END
