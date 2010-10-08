%verify "executed"
%include "armv6t2_taint/unop.S" {"instr":"uxth    r0, r0"}
