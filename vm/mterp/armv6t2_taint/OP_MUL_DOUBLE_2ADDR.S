%verify "executed"
%include "armv6t2_taint/binopWide2addr.S" {"instr":"bl      __aeabi_dmul"}
