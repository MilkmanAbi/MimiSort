/**
 * HYDRA-SORT Benchmark
 * 
 * Compares HYDRA-SORT performance against standard qsort.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pico/stdlib.h"
#include "pico/time.h"
#include "hydra_sort_v2.h"

// Test sizes
static const size_t TEST_SIZES[] = {16, 64, 256, 1024, 4096, 10000};
static const size_t NUM_TESTS = sizeof(TEST_SIZES) / sizeof(TEST_SIZES[0]);

// Number of iterations for averaging
#define ITERATIONS 10

// Maximum array size
#define MAX_SIZE 10000

// Buffers
static int32_t data_original[MAX_SIZE];
static int32_t data_hydra[MAX_SIZE];
static int32_t data_qsort[MAX_SIZE];
static int32_t aux_buffer[MAX_SIZE];

// Comparison function for qsort
static int compare_int32(const void* a, const void* b) {
    int32_t va = *(const int32_t*)a;
    int32_t vb = *(const int32_t*)b;
    return (va > vb) - (va < vb);
}

// Fill array with random data
static void fill_random(int32_t* arr, size_t n) {
    for (size_t i = 0; i < n; i++) {
        arr[i] = rand();
    }
}

// Fill array with nearly sorted data
static void fill_nearly_sorted(int32_t* arr, size_t n) {
    for (size_t i = 0; i < n; i++) {
        arr[i] = i;
    }
    // Perturb ~5% of elements
    for (size_t i = 0; i < n / 20; i++) {
        size_t idx = rand() % n;
        arr[idx] = rand() % n;
    }
}

// Verify sorted
static bool verify_sorted(int32_t* arr, size_t n) {
    for (size_t i = 1; i < n; i++) {
        if (arr[i] < arr[i-1]) return false;
    }
    return true;
}

// Benchmark a single configuration
static void run_benchmark(const char* name, size_t n, void (*fill_func)(int32_t*, size_t)) {
    uint64_t hydra_total = 0;
    uint64_t qsort_total = 0;
    
    for (int iter = 0; iter < ITERATIONS; iter++) {
        // Generate test data
        fill_func(data_original, n);
        
        // Copy for HYDRA
        memcpy(data_hydra, data_original, n * sizeof(int32_t));
        
        // Benchmark HYDRA
        absolute_time_t start = get_absolute_time();
        hydra_sort(data_hydra, n, aux_buffer, HYDRA_PROFILE_ULTRA_FAST);
        absolute_time_t end = get_absolute_time();
        hydra_total += absolute_time_diff_us(start, end);
        
        // Copy for qsort
        memcpy(data_qsort, data_original, n * sizeof(int32_t));
        
        // Benchmark qsort
        start = get_absolute_time();
        qsort(data_qsort, n, sizeof(int32_t), compare_int32);
        end = get_absolute_time();
        qsort_total += absolute_time_diff_us(start, end);
    }
    
    // Calculate averages
    float hydra_avg = (float)hydra_total / ITERATIONS;
    float qsort_avg = (float)qsort_total / ITERATIONS;
    float speedup = qsort_avg / hydra_avg;
    
    // Verify correctness
    bool hydra_correct = verify_sorted(data_hydra, n);
    bool qsort_correct = verify_sorted(data_qsort, n);
    
    printf("│ %-12s │ %6zu │ %9.1f │ %9.1f │ %5.2fx │ %s │\n",
           name, n, hydra_avg, qsort_avg, speedup,
           (hydra_correct && qsort_correct) ? " OK " : "FAIL");
}

int main() {
    stdio_init_all();
    sleep_ms(2000);
    
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════════╗\n");
    printf("║                    HYDRA-SORT BENCHMARK                           ║\n");
    printf("║                     RP2040 @ 125 MHz                              ║\n");
    printf("╚═══════════════════════════════════════════════════════════════════╝\n\n");
    
    // Seed random
    srand(12345);
    
    // =================================================================
    // Random Data Benchmark
    // =================================================================
    printf("RANDOM DATA\n");
    printf("┌──────────────┬────────┬───────────┬───────────┬────────┬──────┐\n");
    printf("│ Test         │ Size   │ HYDRA(µs) │ qsort(µs) │ Speedup│ Check│\n");
    printf("├──────────────┼────────┼───────────┼───────────┼────────┼──────┤\n");
    
    for (size_t i = 0; i < NUM_TESTS; i++) {
        run_benchmark("Random", TEST_SIZES[i], fill_random);
    }
    
    printf("└──────────────┴────────┴───────────┴───────────┴────────┴──────┘\n\n");
    
    // =================================================================
    // Nearly Sorted Data Benchmark
    // =================================================================
    printf("NEARLY SORTED DATA\n");
    printf("┌──────────────┬────────┬───────────┬───────────┬────────┬──────┐\n");
    printf("│ Test         │ Size   │ HYDRA(µs) │ qsort(µs) │ Speedup│ Check│\n");
    printf("├──────────────┼────────┼───────────┼───────────┼────────┼──────┤\n");
    
    for (size_t i = 0; i < NUM_TESTS; i++) {
        run_benchmark("NearlySorted", TEST_SIZES[i], fill_nearly_sorted);
    }
    
    printf("└──────────────┴────────┴───────────┴───────────┴────────┴──────┘\n\n");
    
    // =================================================================
    // uint8_t Benchmark (Counting Sort)
    // =================================================================
    printf("UINT8_T DATA (Counting Sort)\n");
    printf("┌──────────────┬────────┬───────────┬───────────┬────────┐\n");
    printf("│ Test         │ Size   │ HYDRA(µs) │ qsort(µs) │ Speedup│\n");
    printf("├──────────────┼────────┼───────────┼───────────┼────────┤\n");
    
    static uint8_t bytes_original[MAX_SIZE];
    static uint8_t bytes_hydra[MAX_SIZE];
    static uint8_t bytes_qsort[MAX_SIZE];
    
    int compare_u8(const void* a, const void* b) {
        return *(uint8_t*)a - *(uint8_t*)b;
    }
    
    for (size_t i = 0; i < NUM_TESTS; i++) {
        size_t n = TEST_SIZES[i];
        uint64_t hydra_total = 0, qsort_total = 0;
        
        for (int iter = 0; iter < ITERATIONS; iter++) {
            for (size_t j = 0; j < n; j++) bytes_original[j] = rand() % 256;
            
            memcpy(bytes_hydra, bytes_original, n);
            absolute_time_t start = get_absolute_time();
            hydra_sort_u8(bytes_hydra, n);
            hydra_total += absolute_time_diff_us(start, get_absolute_time());
            
            memcpy(bytes_qsort, bytes_original, n);
            start = get_absolute_time();
            qsort(bytes_qsort, n, 1, compare_u8);
            qsort_total += absolute_time_diff_us(start, get_absolute_time());
        }
        
        float h = (float)hydra_total / ITERATIONS;
        float q = (float)qsort_total / ITERATIONS;
        printf("│ uint8        │ %6zu │ %9.1f │ %9.1f │ %5.1fx │\n", n, h, q, q/h);
    }
    
    printf("└──────────────┴────────┴───────────┴───────────┴────────┘\n\n");
    
    printf("═══════════════════════════════════════════════════════════════════\n");
    printf("Benchmark complete!\n");
    printf("═══════════════════════════════════════════════════════════════════\n");
    
    while (1) {
        tight_loop_contents();
    }
    
    return 0;
}
