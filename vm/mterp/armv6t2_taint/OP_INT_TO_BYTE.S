%verify "executed"
%include "armv6t2_taint/unop.S" {"instr":"sxtb    r0, r0"}
