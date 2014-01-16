%verify "executed"
%include "armv5te_taint/OP_INVOKE_VIRTUAL.S" { "isrange":"1", "routine":"Range" }
