%verify "executed"
%include "armv6t2_taint/binop2addr.S" {"instr":"sub     r0, r0, r1"}
