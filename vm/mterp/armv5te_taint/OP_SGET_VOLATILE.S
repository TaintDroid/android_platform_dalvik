%verify "executed"
%include "armv5te_taint/OP_SGET.S" {"barrier":"SMP_DMB", "volatile":"1"}
