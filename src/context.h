#ifndef CONTEXT_H
#define CONTEXT_H

#include <stdint.h>

/*
 * The context structure used for context switching.
 * It saves the callee-saved registers that the C calling convention requires.
 * These include: ra (return address) and s0-s11.
 */
struct context {
    uint64_t ra;
    uint64_t s0;
    uint64_t s1;
    uint64_t s2;
    uint64_t s3;
    uint64_t s4;
    uint64_t s5;
    uint64_t s6;
    uint64_t s7;
    uint64_t s8;
    uint64_t s9;
    uint64_t s10;
    uint64_t s11;
};

#endif // CONTEXT_H
