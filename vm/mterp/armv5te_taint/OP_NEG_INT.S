%verify "executed"
%include "armv5te_taint/unop.S" {"instr":"rsb     r0, r0, #0"}
