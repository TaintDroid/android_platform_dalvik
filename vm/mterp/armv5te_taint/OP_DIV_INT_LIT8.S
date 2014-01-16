%verify "executed"
%include "armv5te_taint/binopLit8.S" {"instr":"bl     __aeabi_idiv","chkzero":"1"}
