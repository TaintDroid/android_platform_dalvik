%verify "executed"
%include "armv5te_taint/binop.S" {"instr":"eor     r0, r0, r1"}
