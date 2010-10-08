%verify "executed"
%include "armv5te_taint/binopLit8.S" {"instr":"add     r0, r0, r1"}
