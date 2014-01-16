%verify "executed"
%include "armv5te_taint/OP_PACKED_SWITCH.S" { "func":"dvmInterpHandleSparseSwitch" }
