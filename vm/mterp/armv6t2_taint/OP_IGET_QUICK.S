%verify "executed"
%verify "null object"
    /* For: iget-quick, iget-object-quick */
    /* op vA, vB, offset@CCCC */
    mov     r2, rINST, lsr #12          @ r2<- B
    FETCH(r1, 1)                        @ r1<- field byte offset
    GET_VREG(r3, r2)                    @ r3<- object we're operating on
// begin WITH_TAINT_TRACKING
    SET_TAINT_FP(r0)
    GET_VREG_TAINT(r9, r2, r0)
// end WITH_TAINT_TRACKING
    ubfx    r2, rINST, #8, #4           @ r2<- A
    cmp     r3, #0                      @ check object for null
    beq     common_errNullObject        @ object was null
    ldr     r0, [r3, r1]                @ r0<- obj.field (always 32 bits)
// begin WITH_TAINT_TRACKING
    bl      .L${opcode}_taint_prop
//    FETCH_ADVANCE_INST(2)               @ advance rPC, load rINST // in subroutine
// end WITH_TAINT_TRACKING
    SET_VREG(r0, r2)                    @ fp[A]<- r0
// begin WITH_TAINT_TRACKING
    SET_TAINT_FP(r0)
    SET_VREG_TAINT(r10, r2, r0)
// end WITH_TAINT_TRACKING
    GET_INST_OPCODE(ip)                 @ extract opcode from rINST
    GOTO_OPCODE(ip)                     @ jump to next instruction

%break

.L${opcode}_taint_prop:
    add     r1, r1, #4
    ldr     r10, [r3, r1]
    orr     r10, r9, r10
    FETCH_ADVANCE_INST(2)               @ advance rPC, load rINST
    bx      lr

