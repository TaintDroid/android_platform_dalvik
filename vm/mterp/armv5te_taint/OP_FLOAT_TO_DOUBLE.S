%verify "executed"
%include "armv5te_taint/unopWider.S" {"instr":"bl      __aeabi_f2d"}
