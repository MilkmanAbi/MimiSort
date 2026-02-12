/**
 * HYDRA-SORT Correctness Tests
 * 
 * Verifies that all sorting algorithms produce correct results
 * across various edge cases and data patterns.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "pico/stdlib.h"
#include "hydra_sort_v2.h"

#define MAX_TEST_SIZE 1024

static int32_t test_data[MAX_TEST_SIZE];
static int32_t aux_buffer[MAX_TEST_SIZE];
static int tests_passed = 0;
static int tests_failed = 0;

// Verify array is sorted
static bool is_sorted_i32(int32_t* arr, size_t n) {
    for (size_t i = 1; i < n; i++) {
        if (arr[i] < arr[i-1]) return false;
    }
    return true;
}

static bool is_sorted_u8(uint8_t* arr, size_t n) {
    for (size_t i = 1; i < n; i++) {
        if (arr[i] < arr[i-1]) return false;
    }
    return true;
}

static bool is_sorted_u16(uint16_t* arr, size_t n) {
    for (size_t i = 1; i < n; i++) {
        if (arr[i] < arr[i-1]) return false;
    }
    return true;
}

// Test result macro
#define TEST(name, condition) do { \
    if (condition) { \
        printf("  [PASS] %s\n", name); \
        tests_passed++; \
    } else { \
        printf("  [FAIL] %s\n", name); \
        tests_failed++; \
    } \
} while(0)

// =============================================================================
// TEST CATEGORIES
// =============================================================================

void test_sorting_networks() {
    printf("\n── Sorting Networks ──────────────────────────\n");
    
    // Sort 4
    int32_t arr4[4] = {4, 2, 3, 1};
    hydra_sort4(arr4);
    TEST("sort4 basic", is_sorted_i32(arr4, 4));
    
    int32_t arr4_sorted[4] = {1, 2, 3, 4};
    hydra_sort4(arr4_sorted);
    TEST("sort4 already sorted", is_sorted_i32(arr4_sorted, 4));
    
    int32_t arr4_reverse[4] = {4, 3, 2, 1};
    hydra_sort4(arr4_reverse);
    TEST("sort4 reverse", is_sorted_i32(arr4_reverse, 4));
    
    int32_t arr4_equal[4] = {5, 5, 5, 5};
    hydra_sort4(arr4_equal);
    TEST("sort4 all equal", is_sorted_i32(arr4_equal, 4));
    
    // Sort 8
    int32_t arr8[8] = {8, 4, 7, 2, 5, 1, 6, 3};
    hydra_sort8(arr8);
    TEST("sort8 basic", is_sorted_i32(arr8, 8));
    
    int32_t arr8_reverse[8] = {8, 7, 6, 5, 4, 3, 2, 1};
    hydra_sort8(arr8_reverse);
    TEST("sort8 reverse", is_sorted_i32(arr8_reverse, 8));
    
    // Sort 16
    int32_t arr16[16];
    for (int i = 0; i < 16; i++) arr16[i] = 16 - i;
    hydra_sort16(arr16);
    TEST("sort16 reverse", is_sorted_i32(arr16, 16));
}

void test_insertion_sort() {
    printf("\n── Insertion Sort ────────────────────────────\n");
    
    // Nearly sorted
    for (int i = 0; i < 50; i++) test_data[i] = i;
    test_data[25] = -1;  // One perturbation
    hydra_insertion_small(test_data, 50);
    TEST("insertion nearly sorted", is_sorted_i32(test_data, 50));
    
    // Reverse
    for (int i = 0; i < 32; i++) test_data[i] = 32 - i;
    hydra_insertion_small(test_data, 32);
    TEST("insertion reverse", is_sorted_i32(test_data, 32));
    
    // Random
    for (int i = 0; i < 32; i++) test_data[i] = rand() % 100;
    hydra_insertion_small(test_data, 32);
    TEST("insertion random", is_sorted_i32(test_data, 32));
}

void test_shell_sort() {
    printf("\n── Shell Sort ────────────────────────────────\n");
    
    // Medium size random
    for (int i = 0; i < 100; i++) test_data[i] = rand() % 1000;
    hydra_shell_sort(test_data, 100);
    TEST("shell random n=100", is_sorted_i32(test_data, 100));
    
    // Larger
    for (int i = 0; i < 500; i++) test_data[i] = rand() % 10000;
    hydra_shell_sort(test_data, 500);
    TEST("shell random n=500", is_sorted_i32(test_data, 500));
}

void test_counting_sort() {
    printf("\n── Counting Sort ─────────────────────────────\n");
    
    // uint8
    static uint8_t bytes[256];
    for (int i = 0; i < 256; i++) bytes[i] = rand() % 256;
    hydra_counting_sort_u8(bytes, 256);
    TEST("counting u8 n=256", is_sorted_u8(bytes, 256));
    
    // All same value
    for (int i = 0; i < 100; i++) bytes[i] = 42;
    hydra_counting_sort_u8(bytes, 100);
    TEST("counting u8 all same", is_sorted_u8(bytes, 100));
    
    // uint16
    static uint16_t shorts[512];
    static uint16_t shorts_aux[512];
    for (int i = 0; i < 512; i++) shorts[i] = rand() % 65536;
    hydra_counting_sort_u16(shorts, shorts_aux, 512);
    TEST("counting u16 n=512", is_sorted_u16(shorts, 512));
}

void test_radix_sort() {
    printf("\n── Radix Sort ────────────────────────────────\n");
    
    static uint32_t udata[500];
    static uint32_t uaux[500];
    
    for (int i = 0; i < 500; i++) udata[i] = rand();
    hydra_radix_sort_256(udata, uaux, 500);
    TEST("radix n=500", is_sorted_i32((int32_t*)udata, 500));
    
    // Small range
    for (int i = 0; i < 200; i++) udata[i] = rand() % 100;
    hydra_radix_sort_256(udata, uaux, 200);
    TEST("radix small range", is_sorted_i32((int32_t*)udata, 200));
}

void test_introsort() {
    printf("\n── Introsort ─────────────────────────────────\n");
    
    // Various sizes
    for (int n = 17; n <= 1000; n *= 3) {
        for (int i = 0; i < n; i++) test_data[i] = rand() % 10000;
        hydra_introsort(test_data, n);
        char name[32];
        snprintf(name, sizeof(name), "introsort n=%d", n);
        TEST(name, is_sorted_i32(test_data, n));
    }
    
    // Worst case for quicksort (sorted input)
    for (int i = 0; i < 1000; i++) test_data[i] = i;
    hydra_introsort(test_data, 1000);
    TEST("introsort sorted input", is_sorted_i32(test_data, 1000));
    
    // Reverse sorted
    for (int i = 0; i < 1000; i++) test_data[i] = 1000 - i;
    hydra_introsort(test_data, 1000);
    TEST("introsort reverse input", is_sorted_i32(test_data, 1000));
}

void test_main_entry() {
    printf("\n── Main Entry (hydra_sort) ───────────────────\n");
    
    // Let the main function choose algorithm
    for (int i = 0; i < 100; i++) test_data[i] = rand() % 1000;
    hydra_sort(test_data, 100, aux_buffer, HYDRA_PROFILE_BALANCED);
    TEST("hydra_sort n=100", is_sorted_i32(test_data, 100));
    
    for (int i = 0; i < 1000; i++) test_data[i] = rand() % 10000;
    hydra_sort(test_data, 1000, aux_buffer, HYDRA_PROFILE_ULTRA_FAST);
    TEST("hydra_sort n=1000 ultra_fast", is_sorted_i32(test_data, 1000));
    
    for (int i = 0; i < 500; i++) test_data[i] = rand() % 5000;
    hydra_sort(test_data, 500, aux_buffer, HYDRA_PROFILE_LOW_POWER);
    TEST("hydra_sort n=500 low_power", is_sorted_i32(test_data, 500));
}

void test_edge_cases() {
    printf("\n── Edge Cases ────────────────────────────────\n");
    
    // Empty
    hydra_sort(test_data, 0, aux_buffer, HYDRA_PROFILE_BALANCED);
    TEST("empty array", true);  // Should not crash
    
    // Single element
    test_data[0] = 42;
    hydra_sort(test_data, 1, aux_buffer, HYDRA_PROFILE_BALANCED);
    TEST("single element", test_data[0] == 42);
    
    // Two elements
    test_data[0] = 5; test_data[1] = 3;
    hydra_sort(test_data, 2, aux_buffer, HYDRA_PROFILE_BALANCED);
    TEST("two elements", is_sorted_i32(test_data, 2));
    
    // All duplicates
    for (int i = 0; i < 100; i++) test_data[i] = 7;
    hydra_sort(test_data, 100, aux_buffer, HYDRA_PROFILE_BALANCED);
    TEST("all duplicates", is_sorted_i32(test_data, 100));
    
    // Negative numbers
    for (int i = 0; i < 100; i++) test_data[i] = rand() % 200 - 100;
    hydra_sort(test_data, 100, aux_buffer, HYDRA_PROFILE_BALANCED);
    TEST("negative numbers", is_sorted_i32(test_data, 100));
    
    // INT32 extremes
    test_data[0] = INT32_MAX;
    test_data[1] = INT32_MIN;
    test_data[2] = 0;
    test_data[3] = INT32_MAX - 1;
    test_data[4] = INT32_MIN + 1;
    hydra_sort(test_data, 5, aux_buffer, HYDRA_PROFILE_BALANCED);
    TEST("int32 extremes", is_sorted_i32(test_data, 5));
}

int main() {
    stdio_init_all();
    sleep_ms(2000);
    
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════╗\n");
    printf("║           HYDRA-SORT CORRECTNESS TESTS                ║\n");
    printf("╚═══════════════════════════════════════════════════════╝\n");
    
    srand(54321);
    
    test_sorting_networks();
    test_insertion_sort();
    test_shell_sort();
    test_counting_sort();
    test_radix_sort();
    test_introsort();
    test_main_entry();
    test_edge_cases();
    
    printf("\n═══════════════════════════════════════════════════════\n");
    printf("RESULTS: %d passed, %d failed\n", tests_passed, tests_failed);
    printf("═══════════════════════════════════════════════════════\n\n");
    
    if (tests_failed == 0) {
        printf("✓ All tests passed!\n");
    } else {
        printf("✗ Some tests failed!\n");
    }
    
    while (1) {
        tight_loop_contents();
    }
    
    return 0;
}
