%verify "executed"
%verify "null object"
    /* iget-wide-quick vA, vB, offset@CCCC */
    mov     r2, rINST, lsr #12          @ r2<- B
    GET_VREG(r3, r2)                    @ r3<- object we're operating on
// begin WITH_TAINT_TRACKING
	SET_TAINT_FP(r0)
	GET_VREG_TAINT(r9, r2, r0)
// end WITH_TAINT_TRACKING
    FETCH(r1, 1)                        @ r1<- field byte offset
    cmp     r3, #0                      @ check object for null
    mov     r2, rINST, lsr #8           @ r2<- A(+)
    beq     common_errNullObject        @ object was null
// begin WITH_TAINT_TRACKING
	add		r10, r1, #8
    ldrd    r0, [r3, r1]                @ r0<- obj.field (64 bits, aligned)
    ldr		r10, [r3, r10]
    orr		r10, r9, r10
// end WITH_TAINT_TRACKING
    and     r2, r2, #15
    FETCH_ADVANCE_INST(2)               @ advance rPC, load rINST
// begin WITH_TAINT_TRACKING
	bl		iget_wide_quick_taint_prop
// end WITH_TAINT_TRACKING
    GOTO_OPCODE(ip)                     @ jump to next instruction

%break

iget_wide_quick_taint_prop:
    add     r3, rFP, r2, lsl #3         @ r3<- &fp[A]
    GET_INST_OPCODE(ip)                 @ extract opcode from rINST
//    stmia   r3, {r0-r1}                 @ fp[A]<- r0/r1
	str		r0, [r3, #0]
	str		r10, [r3, #4]
	str		r1, [r3, #8]
	str		r10, [r3, #12]
	bx		lr
