%verify "executed"
%include "armv5te_taint/OP_INVOKE_SUPER_QUICK.S" { "isrange":"1", "routine":"Range" }
