%verify "executed"
%include "arm-vfp_taint/fbinopWide.S" {"instr":"faddd   d2, d0, d1"}
