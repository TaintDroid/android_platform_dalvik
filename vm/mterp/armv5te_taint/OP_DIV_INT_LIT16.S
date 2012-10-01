%verify "executed"
%include "armv5te_taint/binopLit16.S" {"instr":"bl     __aeabi_idiv","chkzero":"1"}
