%verify "executed"
%include "armv5te_taint/OP_APUT.S" { "store":"strb", "shift":"0" }
