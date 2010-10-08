%verify "executed"
%include "arm-vfp_taint/funop.S" {"instr":"ftosizs s1, s0"}
