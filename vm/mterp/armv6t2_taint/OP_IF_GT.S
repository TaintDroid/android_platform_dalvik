%verify "executed"
%include "armv6t2_taint/bincmp.S" { "revcmp":"le" }
