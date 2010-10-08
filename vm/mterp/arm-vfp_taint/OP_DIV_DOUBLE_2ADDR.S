%verify "executed"
%include "arm-vfp_taint/fbinopWide2addr.S" {"instr":"fdivd   d2, d0, d1"}
