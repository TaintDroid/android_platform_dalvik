%verify "executed"
%include "armv5te_taint/unopWider.S" {"instr":"mov     r1, r0, asr #31"}
