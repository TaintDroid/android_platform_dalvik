%verify "executed"
%include "armv6t2_taint/unopWider.S" {"instr":"mov     r1, r0, asr #31"}
