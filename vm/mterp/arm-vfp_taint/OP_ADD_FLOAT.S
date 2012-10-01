%verify "executed"
%include "arm-vfp_taint/fbinop.S" {"instr":"fadds   s2, s0, s1"}
