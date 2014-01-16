%verify "executed"
%include "armv6t2_taint/binop2addr.S" {"preinstr":"and     r1, r1, #31", "instr":"mov     r0, r0, asr r1"}
