%verify "executed"
%include "arm-vfp_taint/funopNarrower.S" {"instr":"ftosizd  s0, d0"}
