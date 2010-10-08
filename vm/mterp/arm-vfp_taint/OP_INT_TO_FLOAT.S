%verify "executed"
%include "arm-vfp_taint/funop.S" {"instr":"fsitos  s1, s0"}
