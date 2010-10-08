%verify "executed"
%include "armv5te_taint/unopWide.S" {"instr":"bl      __aeabi_l2d"}
