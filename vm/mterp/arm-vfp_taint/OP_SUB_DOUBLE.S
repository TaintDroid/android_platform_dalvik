%verify "executed"
%include "arm-vfp_taint/fbinopWide.S" {"instr":"fsubd   d2, d0, d1"}
