%verify "executed"
%include "arm-vfp_taint/funopNarrower.S" {"instr":"fcvtsd  s0, d0"}
