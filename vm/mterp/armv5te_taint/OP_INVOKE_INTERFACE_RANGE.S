%verify "executed"
%include "armv5te_taint/OP_INVOKE_INTERFACE.S" { "isrange":"1", "routine":"Range" }
