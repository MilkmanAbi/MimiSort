/**
 * HYDRA-SORT Basic Usage Example
 * 
 * Demonstrates the core functionality of HYDRA-SORT on RP2040.
 */

#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hydra_sort_v2.h"

#define ARRAY_SIZE 100

// Simple helper to check if array is sorted
static bool is_sorted(int32_t* arr, size_t n) {
    for (size_t i = 1; i < n; i++) {
        if (arr[i] < arr[i-1]) return false;
    }
    return true;
}

// Print array (for small arrays)
static void print_array(const char* label, int32_t* arr, size_t n) {
    printf("%s: [", label);
    for (size_t i = 0; i < n && i < 10; i++) {
        printf("%ld", (long)arr[i]);
        if (i < n - 1 && i < 9) printf(", ");
    }
    if (n > 10) printf(", ...");
    printf("]\n");
}

int main() {
    // Initialize stdio
    stdio_init_all();
    sleep_ms(2000);  // Wait for USB connection
    
    printf("\n");
    printf("╔═══════════════════════════════════════╗\n");
    printf("║     HYDRA-SORT Basic Usage Demo       ║\n");
    printf("╚═══════════════════════════════════════╝\n\n");
    
    // =================================================================
    // Example 1: Basic int32_t sorting
    // =================================================================
    printf("Example 1: Basic int32_t sorting\n");
    printf("─────────────────────────────────\n");
    
    int32_t data[ARRAY_SIZE];
    int32_t aux[ARRAY_SIZE];
    
    // Fill with random data
    for (size_t i = 0; i < ARRAY_SIZE; i++) {
        data[i] = rand() % 1000;
    }
    
    print_array("Before", data, ARRAY_SIZE);
    
    // Sort!
    hydra_sort(data, ARRAY_SIZE, aux, HYDRA_PROFILE_BALANCED);
    
    print_array("After", data, ARRAY_SIZE);
    printf("Sorted correctly: %s\n\n", is_sorted(data, ARRAY_SIZE) ? "YES" : "NO");
    
    // =================================================================
    // Example 2: Small array (uses register-only sorting network)
    // =================================================================
    printf("Example 2: Small array (n=8)\n");
    printf("────────────────────────────\n");
    
    int32_t small[8] = {42, 17, 93, 8, 55, 3, 71, 29};
    
    print_array("Before", small, 8);
    hydra_sort8(small);  // Direct call for known size
    print_array("After", small, 8);
    printf("Sorted correctly: %s\n\n", is_sorted(small, 8) ? "YES" : "NO");
    
    // =================================================================
    // Example 3: uint8_t sorting (uses counting sort - very fast!)
    // =================================================================
    printf("Example 3: uint8_t sorting (counting sort)\n");
    printf("──────────────────────────────────────────\n");
    
    uint8_t bytes[256];
    for (size_t i = 0; i < 256; i++) {
        bytes[i] = rand() % 256;
    }
    
    printf("Before: [%d, %d, %d, %d, %d, ...]\n", 
           bytes[0], bytes[1], bytes[2], bytes[3], bytes[4]);
    
    hydra_sort_u8(bytes, 256);
    
    printf("After:  [%d, %d, %d, %d, %d, ...]\n",
           bytes[0], bytes[1], bytes[2], bytes[3], bytes[4]);
    
    bool bytes_sorted = true;
    for (size_t i = 1; i < 256; i++) {
        if (bytes[i] < bytes[i-1]) { bytes_sorted = false; break; }
    }
    printf("Sorted correctly: %s\n\n", bytes_sorted ? "YES" : "NO");
    
    // =================================================================
    // Example 4: Nearly sorted data (insertion sort path)
    // =================================================================
    printf("Example 4: Nearly sorted data\n");
    printf("─────────────────────────────\n");
    
    int32_t nearly[50];
    for (size_t i = 0; i < 50; i++) {
        nearly[i] = i * 10;  // Already sorted
    }
    // Add a few perturbations
    nearly[10] = 5;
    nearly[30] = 250;
    
    int32_t nearly_aux[50];
    
    printf("Nearly sorted array with 2 perturbations\n");
    hydra_sort(nearly, 50, nearly_aux, HYDRA_PROFILE_BALANCED);
    printf("Sorted correctly: %s\n\n", is_sorted(nearly, 50) ? "YES" : "NO");
    
    // =================================================================
    // Done!
    // =================================================================
    printf("═══════════════════════════════════════\n");
    printf("All examples completed successfully!\n");
    printf("═══════════════════════════════════════\n");
    
    while (1) {
        tight_loop_contents();
    }
    
    return 0;
}
