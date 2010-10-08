%verify "executed"
%include "arm-vfp_taint/fbinop.S" {"instr":"fdivs   s2, s0, s1"}
