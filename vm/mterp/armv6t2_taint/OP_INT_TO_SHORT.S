%verify "executed"
%include "armv6t2_taint/unop.S" {"instr":"sxth    r0, r0"}
