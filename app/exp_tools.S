/*
 * sum.S
 *
 *  Created on: May 4, 2022
 *      Author: igor
 */
.global _sum
_sum:
	ldrd    r2, r0, [r0]
    add     r0, r0, r2
    bx      lr

.extern Ai
.extern Xi

.global _expb_1
.global _expb_2
.global _expb_3
.global _expb_4
// stm32 does not support absolute addressing
_expb_1:
	push 	{R4-R7, LR}
	ldr 	R0, =Ai
	mov		R1, #1
	mov		R2, #2
	mov		R3, #3
	mov		R4, #4
	mov		R5, #5
	mov		R6, #6
	mov		R7, #7
	stm 	R0!, {r1,r2,r3,r4,r5,r6,r7}
	mov		R7, #8
	str		R7, [R0]
	pop		{R4-R7, pc}
_expb_2:
	push 	{R4-R7, LR}
	ldr 	R0, =Xi
	mov		R1, #9
	mov		R2, #3
	mov		R3, #2
	mov		R4, #0
	mov		R5, #1
	mov		R6, #9
	mov		R7, #7
	stm 	R0!, {r1,r2,r3,r4,r5,r6,r7}
	mov		R7, #1
	str		R7, [R0]
	pop		{R4-R7, pc}

_expb_3:
    ldrd    r2, r3, [r1]
    push    {r4, r5}
    ldr r4, [r0, #4]
    ldr r5, [r0]
    mul r4, r3, r4
    ldr r3, [r0, #8]
    mla r2, r5, r2, r4
    ldr r5, [r1, #8]
    ldr r4, [r0, #12]
    mla r2, r5, r3, r2
    ldr r5, [r1, #12]
    ldr r3, [r0, #16]
    mla r4, r5, r4, r2
    ldr r5, [r1, #16]
    ldr r2, [r0, #20]
    mla r4, r5, r3, r4
    ldr r5, [r1, #20]
    ldr r3, [r0, #24]
    mla r2, r5, r2, r4
    ldr r5, [r1, #24]
    ldr r4, [r0, #28]
    mla r3, r5, r3, r2
    ldr r0, [r1, #28]
    mla r0, r0, r4, r3
    pop {r4, r5}
    bx  lr


_expb_4:
    movs    r3, #0
    mov r2, r0
    mov r0, r3
    push    {r4, r5, r6, lr}
.L2:
	lsl	r6, r3, #2
    ldr r4, [r2, r6]
    ldr r5, [r1, r6]
    add    r3, r3, #1
    cmp r3, #8
    mla r0, r5, r4, r0
    bne .L2
    pop {r4, r5, r6, pc}

