%verify "executed"
%include "armv5te_taint/OP_CMPL_DOUBLE.S" { "naninst":"mov     r1, #1" }
