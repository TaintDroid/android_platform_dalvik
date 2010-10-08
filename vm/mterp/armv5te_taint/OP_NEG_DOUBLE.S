%verify "executed"
%include "armv5te_taint/unopWide.S" {"instr":"add     r1, r1, #0x80000000"}
