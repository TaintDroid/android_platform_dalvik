%verify "executed"
%verify "null object"
    /* For: iput-quick, iput-object-quick */
    /* op vA, vB, offset@CCCC */
    mov     r2, rINST, lsr #12          @ r2<- B
    FETCH(r1, 1)                        @ r1<- field byte offset
    GET_VREG(r3, r2)                    @ r3<- fp[B], the object pointer
    ubfx    r2, rINST, #8, #4           @ r2<- A
    cmp     r3, #0                      @ check object for null
    beq     common_errNullObject        @ object was null
    GET_VREG(r0, r2)                    @ r0<- fp[A]
// begin WITH_TAINT_TRACKING
    SET_TAINT_FP(r9)
    GET_VREG_TAINT(r10, r2, r9)
// end WITH_TAINT_TRACKING
    FETCH_ADVANCE_INST(2)               @ advance rPC, load rINST
    str     r0, [r3, r1]                @ obj.field (always 32 bits)<- r0
// begin WITH_TAINT_TRACKING
    add	    r1, r1, #4
    str     r10, [r3, r1]
// end WITH_TAINT_TRACKING
    GET_INST_OPCODE(ip)                 @ extract opcode from rINST
    GOTO_OPCODE(ip)                     @ jump to next instruction
