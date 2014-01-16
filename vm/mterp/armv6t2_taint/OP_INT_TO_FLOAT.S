%verify "executed"
%include "armv6t2_taint/unop.S" {"instr":"bl      __aeabi_i2f"}
