%verify "executed"
%include "armv5te_taint/OP_AGET.S" { "load":"ldrb", "shift":"0" }
