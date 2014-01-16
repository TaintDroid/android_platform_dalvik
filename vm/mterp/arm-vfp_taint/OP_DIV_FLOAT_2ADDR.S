%verify "executed"
%include "arm-vfp_taint/fbinop2addr.S" {"instr":"fdivs   s2, s0, s1"}
