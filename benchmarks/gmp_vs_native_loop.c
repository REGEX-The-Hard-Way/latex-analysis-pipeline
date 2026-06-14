#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <gmp.h>

#define CLOCK_MS(start, end) \
    (((double)(end) - (double)(start)) / CLOCKS_PER_SEC * 1000.0)

#define ITERATIONS 10000000UL

static void bench_native_long(void) {
    clock_t start, end;
    volatile unsigned long sink = 0;
    unsigned long i;

    start = clock();
    unsigned long acc = 0;
    for (i = 0; i < ITERATIONS; i++) {
        acc += i;
        acc *= 2;
        if (acc > 1000000) acc /= 3;
    }
    sink = acc;
    end = clock();
    printf("native (unsigned long)  : %10.3f ms  (result = %lu)\n",
           CLOCK_MS(start, end), sink);
}

static void bench_gmp_mpz(void) {
    clock_t start, end;
    unsigned long i;

    mpz_t acc, tmp, three, million;
    mpz_init(acc);
    mpz_init(tmp);
    mpz_init_set_ui(three, 3);
    mpz_init_set_ui(million, 1000000);

    start = clock();
    for (i = 0; i < ITERATIONS; i++) {
        mpz_add_ui(acc, acc, i);
        mpz_mul_2exp(acc, acc, 1);
        if (mpz_cmp(acc, million) > 0) {
            mpz_tdiv_q(acc, acc, three);
        }
    }
    end = clock();
    printf("gmp (mpz_t)             : %10.3f ms  (result = ", CLOCK_MS(start, end));
    mpz_out_str(stdout, 10, acc);
    printf(")\n");

    mpz_clear(acc);
    mpz_clear(tmp);
    mpz_clear(three);
    mpz_clear(million);
}

static void bench_native_uint64(void) {
    clock_t start, end;
    volatile unsigned long long sink = 0;
    unsigned long long i;

    start = clock();
    unsigned long long acc = 0;
    for (i = 0; i < ITERATIONS; i++) {
        acc += i;
        acc <<= 1;
        if (acc > 1000000) acc /= 3;
    }
    sink = acc;
    end = clock();
    printf("native (uint64_t)       : %10.3f ms  (result = %llu)\n",
           CLOCK_MS(start, end), sink);
}

static void bench_gmp_simple_loop(void) {
    clock_t start, end;
    unsigned long i;

    mpz_t acc;
    mpz_init_set_ui(acc, 0);

    start = clock();
    for (i = 0; i < ITERATIONS; i++) {
        mpz_add_ui(acc, acc, 1);
    }
    end = clock();
    printf("gmp simple increment     : %10.3f ms  (result = ", CLOCK_MS(start, end));
    mpz_out_str(stdout, 10, acc);
    printf(")\n");

    mpz_clear(acc);
}

static void bench_native_simple_loop(void) {
    clock_t start, end;
    volatile unsigned long sink = 0;
    unsigned long i;

    start = clock();
    unsigned long acc = 0;
    for (i = 0; i < ITERATIONS; i++) {
        acc++;
        asm volatile("" : "+r"(acc));
    }
    sink = acc;
    end = clock();
    printf("native simple increment : %10.3f ms  (result = %lu)\n",
           CLOCK_MS(start, end), sink);
}

int main(void) {
    printf("=== GMP vs Native C for-loop benchmark ===\n");
    printf("Iterations: %lu\n\n", ITERATIONS);

    printf("--- Simple increment ---\n");
    bench_native_simple_loop();
    bench_gmp_simple_loop();

    printf("\n--- Mixed arithmetic ---\n");
    bench_native_long();
    bench_native_uint64();
    bench_gmp_mpz();

    printf("\n=== Summary ===\n");
    printf("GMP function calls involve heap allocation and type abstraction,\n");
    printf("while native loops compile directly to CPU instructions.\n");
    printf("GMP is useful when you need arbitrary-precision beyond 64 bits.\n");

    return 0;
}
