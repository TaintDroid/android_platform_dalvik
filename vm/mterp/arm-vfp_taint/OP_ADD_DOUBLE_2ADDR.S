%verify "executed"
%include "arm-vfp_taint/fbinopWide2addr.S" {"instr":"faddd   d2, d0, d1"}
