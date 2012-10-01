%verify "executed"
%include "armv6t2_taint/unopWide.S" {"preinstr":"mvn     r0, r0", "instr":"mvn     r1, r1"}
