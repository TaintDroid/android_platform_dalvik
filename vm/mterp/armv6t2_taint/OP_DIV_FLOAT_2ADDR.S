%verify "executed"
%include "armv6t2_taint/binop2addr.S" {"instr":"bl      __aeabi_fdiv"}
