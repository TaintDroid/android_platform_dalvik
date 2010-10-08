%verify "executed"
%include "armv5te_taint/bincmp.S" { "revcmp":"ne" }
