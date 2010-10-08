%verify "executed"
%include "armv5te_taint/OP_CMPL_FLOAT.S" { "naninst":"mov     r1, #1" }
