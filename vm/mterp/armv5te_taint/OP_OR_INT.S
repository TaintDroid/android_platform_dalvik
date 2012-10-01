%verify "executed"
%include "armv5te_taint/binop.S" {"instr":"orr     r0, r0, r1"}
