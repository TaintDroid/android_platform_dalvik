/*--- end of opcodes ---*/

bail:
    ILOGD("|-- Leaving interpreter loop");      // note "curMethod" may be NULL

#ifdef WITH_TAINT_TRACKING
    self->interpSave.rtaint = rtaint;
#endif
    self->interpSave.retval = retval;
}
