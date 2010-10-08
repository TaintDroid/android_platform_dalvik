%default { "load":"ldr", "shift":"2" }
%verify "executed"
    /*
     * Array get, 32 bits or less.  vAA <- vBB[vCC].
     *
     * Note: using the usual FETCH/and/shift stuff, this fits in exactly 17
     * instructions.  We use a pair of FETCH_Bs instead.
     *
     * for: aget, aget-object, aget-boolean, aget-byte, aget-char, aget-short
     */
    /* op vAA, vBB, vCC */
    FETCH_B(r2, 1, 0)                   @ r2<- BB
    mov     r9, rINST, lsr #8           @ r9<- AA
    FETCH_B(r3, 1, 1)                   @ r3<- CC
    GET_VREG(r0, r2)                    @ r0<- vBB (array object)
    GET_VREG(r1, r3)                    @ r1<- vCC (requested index)
    cmp     r0, #0                      @ null array object?
    beq     common_errNullObject        @ yes, bail
// begin WITH_TAINT_TRACKING
	bl		.L${opcode}_taint_prop_1
// end WITH_TAINT_TRACKING
	ldr     r3, [r0, #offArrayObject_length]    @ r3<- arrayObj->length
    add     r0, r0, r1, lsl #$shift     @ r0<- arrayObj + index*width
    cmp     r1, r3                      @ compare unsigned index, length
// begin WITH_TAINT_TRACKING
//    bcs     common_errArrayIndex        @ index >= length, bail	// PJG: in subroutine
//    FETCH_ADVANCE_INST(2)               @ advance rPC, load rINST // PJG: in subroutine
	bl		.L${opcode}_taint_prop_2
// end WITH_TAINT_TRACKING
	$load   r2, [r0, #offArrayObject_contents]  @ r2<- vBB[vCC]
    GET_INST_OPCODE(ip)                 @ extract opcode from rINST
    SET_VREG(r2, r9)                    @ vAA<- r2
    GOTO_OPCODE(ip)                     @ jump to next instruction

%break

.L${opcode}_taint_prop_1:
	ldr		r2, [r0, #offArrayObject_taint]
	SET_TAINT_FP(r10)
    GET_VREG_TAINT(r3, r3, r10)
	orr		r2, r3, r2					@ r2<- r2 | r1
	bx		lr

.L${opcode}_taint_prop_2:
    bcs     common_errArrayIndex        @ index >= length, bail
    FETCH_ADVANCE_INST(2)               @ advance rPC, load rINST
	SET_TAINT_FP(r3)
	SET_VREG_TAINT(r2, r9, r3)
    bx		lr
