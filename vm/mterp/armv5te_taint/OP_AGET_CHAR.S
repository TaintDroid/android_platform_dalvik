%verify "executed"
%include "armv5te_taint/OP_AGET.S" { "load":"ldrh", "shift":"1" }
