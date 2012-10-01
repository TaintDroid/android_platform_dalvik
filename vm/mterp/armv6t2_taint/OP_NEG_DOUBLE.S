%verify "executed"
%include "armv6t2_taint/unopWide.S" {"instr":"add     r1, r1, #0x80000000"}
