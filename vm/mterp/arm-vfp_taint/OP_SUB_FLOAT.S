%verify "executed"
%include "arm-vfp_taint/fbinop.S" {"instr":"fsubs   s2, s0, s1"}
