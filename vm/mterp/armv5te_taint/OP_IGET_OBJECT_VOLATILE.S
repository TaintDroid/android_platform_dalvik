%verify "executed"
%include "armv5te_taint/OP_IGET.S" {"volatile":"1","barrier":"SMP_DMB"}
