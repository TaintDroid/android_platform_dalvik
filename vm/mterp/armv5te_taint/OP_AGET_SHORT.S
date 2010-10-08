%verify "executed"
%include "armv5te_taint/OP_AGET.S" { "load":"ldrsh", "shift":"1" }
