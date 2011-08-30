%verify "executed"
%include "armv5te_taint/binopWide.S" {"instr":"bl      __aeabi_dmul"}
