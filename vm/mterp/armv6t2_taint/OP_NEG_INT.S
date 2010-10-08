%verify "executed"
%include "armv6t2_taint/unop.S" {"instr":"rsb     r0, r0, #0"}
