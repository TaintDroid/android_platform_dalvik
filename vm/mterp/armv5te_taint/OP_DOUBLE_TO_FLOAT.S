%verify "executed"
%include "armv5te_taint/unopNarrower.S" {"instr":"bl      __aeabi_d2f"}
