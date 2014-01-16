%verify "executed"
/* EABI doesn't define a double remainder function, but libm does */
%include "armv6t2_taint/binopWide2addr.S" {"instr":"bl      fmod"}
