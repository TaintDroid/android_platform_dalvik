%verify "executed"
%include "armv5te_taint/binopWide2addr.S" {"instr":"bl      __aeabi_dsub"}
