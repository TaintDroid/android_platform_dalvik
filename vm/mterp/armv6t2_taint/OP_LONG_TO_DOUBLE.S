%verify "executed"
%include "armv6t2_taint/unopWide.S" {"instr":"bl      __aeabi_l2d"}
