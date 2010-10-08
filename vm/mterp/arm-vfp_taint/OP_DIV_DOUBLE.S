%verify "executed"
%include "arm-vfp_taint/fbinopWide.S" {"instr":"fdivd   d2, d0, d1"}
