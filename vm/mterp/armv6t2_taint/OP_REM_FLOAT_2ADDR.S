%verify "executed"
/* EABI doesn't define a float remainder function, but libm does */
%include "armv6t2_taint/binop2addr.S" {"instr":"bl      fmodf"}
