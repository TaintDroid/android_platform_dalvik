%verify "executed"
%include "armv6t2_taint/unopWide.S" {"preinstr":"rsbs    r0, r0, #0", "instr":"rsc     r1, r1, #0"}
