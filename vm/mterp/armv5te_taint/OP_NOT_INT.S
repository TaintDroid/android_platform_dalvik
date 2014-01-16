%verify "executed"
%include "armv5te_taint/unop.S" {"instr":"mvn     r0, r0"}
