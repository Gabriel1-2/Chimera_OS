// File: src/switch.S
// This file provides the context switching routine for Project Chimera.
// It defines the assembly function swtch which saves the current context and loads a new one.

	.text
	.global swtch
	.type swtch, @function
/* 
 * void swtch(struct context **old, struct context *new);
 * a0 = pointer to old context pointer.
 * a1 = pointer to new context.
 */
swtch:
    /* Save callee-saved registers into the memory pointed by a0. */
    /* Save return address (ra) */
    sd ra, 0(a0)
    /* Save s0 - s11 registers */
    sd s0,  8(a0)
    sd s1,  16(a0)
    sd s2,  24(a0)
    sd s3,  32(a0)
    sd s4,  40(a0)
    sd s5,  48(a0)
    sd s6,  56(a0)
    sd s7,  64(a0)
    sd s8,  72(a0)
    sd s9,  80(a0)
    sd s10, 88(a0)
    sd s11, 96(a0)
    
    /* Load new context values from the pointer in a1. */
    ld ra, 0(a1)
    ld s0,  8(a1)
    ld s1,  16(a1)
    ld s2,  24(a1)
    ld s3,  32(a1)
    ld s4,  40(a1)
    ld s5,  48(a1)
    ld s6,  56(a1)
    ld s7,  64(a1)
    ld s8,  72(a1)
    ld s9,  80(a1)
    ld s10, 88(a1)
    ld s11, 96(a1)
    
    ret
