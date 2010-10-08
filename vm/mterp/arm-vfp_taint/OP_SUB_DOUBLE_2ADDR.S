%verify "executed"
%include "arm-vfp_taint/fbinopWide2addr.S" {"instr":"fsubd   d2, d0, d1"}
