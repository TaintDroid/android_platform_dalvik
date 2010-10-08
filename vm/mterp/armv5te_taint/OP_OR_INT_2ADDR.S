%verify "executed"
%include "armv5te_taint/binop2addr.S" {"instr":"orr     r0, r0, r1"}
