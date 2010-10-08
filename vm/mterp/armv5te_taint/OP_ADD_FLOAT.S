%verify "executed"
%include "armv5te_taint/binop.S" {"instr":"bl      __aeabi_fadd"}
