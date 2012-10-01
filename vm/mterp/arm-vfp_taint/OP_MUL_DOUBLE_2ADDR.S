%verify "executed"
%include "arm-vfp_taint/fbinopWide2addr.S" {"instr":"fmuld   d2, d0, d1"}
