%verify "executed"
%include "arm-vfp_taint/funopWider.S" {"instr":"fcvtds  d0, s0"}
