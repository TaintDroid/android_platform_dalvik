%verify "executed"
%include "armv5te_taint/OP_IPUT.S" {"prebarrier":"SMP_DMB_ST", "postbarrier":"SMP_DMB", "volatile":"1"}
