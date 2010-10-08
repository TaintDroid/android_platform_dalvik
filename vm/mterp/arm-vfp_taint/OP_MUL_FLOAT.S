%verify "executed"
%include "arm-vfp_taint/fbinop.S" {"instr":"fmuls   s2, s0, s1"}
