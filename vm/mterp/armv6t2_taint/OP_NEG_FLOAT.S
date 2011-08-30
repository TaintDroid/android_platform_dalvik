%verify "executed"
%include "armv6t2_taint/unop.S" {"instr":"add     r0, r0, #0x80000000"}
