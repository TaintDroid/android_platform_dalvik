%verify "executed"
%include "armv5te_taint/binop.S" {"instr":"sub     r0, r0, r1"}
