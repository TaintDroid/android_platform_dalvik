%verify "executed"
%include "armv5te_taint/binop2addr.S" {"instr":"bl      __aeabi_fadd"}
