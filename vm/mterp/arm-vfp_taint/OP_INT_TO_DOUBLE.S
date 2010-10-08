%verify "executed"
%include "arm-vfp_taint/funopWider.S" {"instr":"fsitod  d0, s0"}
