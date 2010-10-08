%verify "executed"
%include "armv5te_taint/OP_APUT.S" { "store":"strh", "shift":"1" }
