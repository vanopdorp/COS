#ifndef FPU_MATH_H
#define FPU_MATH_H

#include <stdint.h>

// float add: a + b
static inline float fadd(float a, float b) {
    float result;
    __asm__ __volatile__ (
        "fld %1\n"
        "fadd %2\n"
        "fstp %0\n"
        : "=m"(result)
        : "m"(a), "m"(b)
    );
    return result;
}

// float sub: a - b
static inline float fsub(float a, float b) {
    float result;
    __asm__ __volatile__ (
        "fld %1\n"
        "fsub %2\n"
        "fstp %0\n"
        : "=m"(result)
        : "m"(a), "m"(b)
    );
    return result;
}

// float mul: a * b
static inline float fmul(float a, float b) {
    float result;
    __asm__ __volatile__ (
        "fld %1\n"
        "fmul %2\n"
        "fstp %0\n"
        : "=m"(result)
        : "m"(a), "m"(b)
    );
    return result;
}

// float div: a / b
static inline float fdiv(float a, float b) {
    float result;
    __asm__ __volatile__ (
        "fld %1\n"
        "fdiv %2\n"
        "fstp %0\n"
        : "=m"(result)
        : "m"(a), "m"(b)
    );
    return result;
}

// float sqrt: √a
static inline float fsqrt(float a) {
    float result;
    __asm__ __volatile__ (
        "fld %1\n"
        "fsqrt\n"
        "fstp %0\n"
        : "=m"(result)
        : "m"(a)
    );
    return result;
}

// float abs: |a|
static inline float fabs(float a) {
    float result;
    __asm__ __volatile__ (
        "fld %1\n"
        "fabs\n"
        "fstp %0\n"
        : "=m"(result)
        : "m"(a)
    );
    return result;
}

// float neg: -a
static inline float fneg(float a) {
    float result;
    __asm__ __volatile__ (
        "fld %1\n"
        "fchs\n"
        "fstp %0\n"
        : "=m"(result)
        : "m"(a)
    );
    return result;
}

// float sin(a)
static inline float fsin(float a) {
    float result;
    __asm__ __volatile__ (
        "fld %1\n"
        "fsin\n"
        "fstp %0\n"
        : "=m"(result)
        : "m"(a)
    );
    return result;
}

// float cos(a)
static inline float fcos(float a) {
    float result;
    __asm__ __volatile__ (
        "fld %1\n"
        "fcos\n"
        "fstp %0\n"
        : "=m"(result)
        : "m"(a)
    );
    return result;
}

// float log(a) — natuurlijke logaritme
static inline float flog(float a) {
    float result;
    __asm__ __volatile__ (
        "fldln2\n"     // ln(2)
        "fld %1\n"     // a
        "fyl2x\n"      // log2(a) * ln(2) = ln(a)
        "fstp %0\n"
        : "=m"(result)
        : "m"(a)
    );
    return result;
}

// float exp(a) — e^a
static inline float fexp(float a) {
    float result;
    __asm__ __volatile__ (
        "fld %1\n"
        "fldl2e\n"     // log2(e)
        "fmul\n"
        "f2xm1\n"
        "fld1\n"
        "fadd\n"
        "fstp %0\n"
        : "=m"(result)
        : "m"(a)
    );
    return result;
}

// float round(a) — afronden naar dichtstbijzijnde int
static inline float fround(float a) {
    float result;
    __asm__ __volatile__ (
        "fld %1\n"
        "frndint\n"
        "fstp %0\n"
        : "=m"(result)
        : "m"(a)
    );
    return result;
}

#endif
