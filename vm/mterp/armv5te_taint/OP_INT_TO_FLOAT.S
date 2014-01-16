%verify "executed"
%include "armv5te_taint/unop.S" {"instr":"bl      __aeabi_i2f"}
