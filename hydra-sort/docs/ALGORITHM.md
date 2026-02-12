# HYDRA-SORT Algorithm Documentation

This document provides detailed explanations of the algorithms and techniques used in HYDRA-SORT.

## Table of Contents

1. [Algorithm Selection Logic](#algorithm-selection-logic)
2. [Sorting Networks](#sorting-networks)
3. [Insertion Sort Optimizations](#insertion-sort-optimizations)
4. [Shell Sort](#shell-sort)
5. [Counting & Radix Sort](#counting--radix-sort)
6. [Introsort](#introsort)
7. [Merge Strategies](#merge-strategies)

---

## Algorithm Selection Logic

HYDRA-SORT uses an adaptive approach that analyzes input characteristics in O(n) time to select the optimal algorithm.

### Feature Extraction

The analysis pass computes:

- **n**: Array size
- **ρ (presortedness)**: Ratio of ascending runs to array size
- **R (range)**: Difference between max and min values
- **Distribution**: Variance estimate for radix sort decision

### Selection Thresholds

| Condition | Algorithm | Rationale |
|-----------|-----------|-----------|
| n ≤ 4 | Network-4 | Fixed 5 comparisons, register-only |
| n ≤ 8 | Network-8 | Fixed 19 comparisons, register-only |
| n ≤ 16 | Network-16 | Fixed 60 comparisons, minimal overhead |
| ρ ≥ 0.95 | Insertion | Near-linear for almost-sorted data |
| n ≤ 64 | Shell | Good cache behavior for small arrays |
| R ≤ 8n | Radix-256 | O(n) when range is bounded |
| n > 4096 | Parallel Block | Utilize both cores |
| Default | Introsort | Guaranteed O(n log n) |

---

## Sorting Networks

Sorting networks are fixed sequences of compare-exchange operations that sort any input of a given size.

### Properties

- **Data-oblivious**: Same operations regardless of input
- **Branch-free**: No conditional jumps
- **Parallelizable**: Independent operations can execute simultaneously

### Network-8 (Batcher Odd-Even)

```
Level 1: (0,1) (2,3) (4,5) (6,7)
Level 2: (0,2) (1,3) (4,6) (5,7)
Level 3: (1,2) (5,6)
Level 4: (0,4) (1,5) (2,6) (3,7)
Level 5: (2,4) (3,5)
Level 6: (1,2) (3,4) (5,6)
```

Total: 19 comparators

### Register-Only Implementation

By loading all elements into CPU registers before sorting, we eliminate memory access during the sort operation:

```c
register int32_t r0 = arr[0];
register int32_t r1 = arr[1];
// ... load all
// ... sorting network operations
arr[0] = r0;
arr[1] = r1;
// ... store all
```

Memory accesses: 2n (load + store) vs O(n log n) for comparison sorts.

---

## Insertion Sort Optimizations

### Sentinel Technique

Traditional insertion sort checks bounds on every inner loop iteration:

```c
while (j > 0 && arr[j-1] > key)  // TWO checks!
```

By placing a sentinel (minimum element) at position 0, we eliminate the bounds check:

```c
while (arr[j-1] > key)  // ONE check!
```

**Speedup**: ~1.3-1.5x on inner loop

### Unrolled Inner Loop

For small arrays, unrolling the inner loop reduces loop overhead:

```c
if (j >= 1 && arr[j-1] > key) { arr[j] = arr[j-1]; j--; }
if (j >= 1 && arr[j-1] > key) { arr[j] = arr[j-1]; j--; }
// ... continue if needed
```

---

## Shell Sort

Shell sort generalizes insertion sort by comparing elements separated by a gap.

### Ciura Gap Sequence

HYDRA-SORT uses the empirically-optimized Ciura sequence:

```
1, 4, 10, 23, 57, 132, 301, 701, 1750
```

This sequence provides O(n^(4/3)) average complexity in practice.

### Why Shell Sort?

For n = 32-64, shell sort outperforms both:
- Insertion sort (too slow for random data)
- Quicksort (too much overhead for small n)

---

## Counting & Radix Sort

### Counting Sort (uint8_t)

For 8-bit integers, counting sort achieves O(n + 256) = O(n) with tiny constants.

**Algorithm**:
1. Count occurrences of each value (0-255)
2. Reconstruct array from counts

**Cycles**: ~5n total

**vs Quicksort**: 10-25x faster for byte arrays!

### Radix Sort (Base-256)

For 32-bit integers with bounded range, radix sort processes 4 passes (one per byte):

**Per-pass operations**:
1. Count (256 buckets)
2. Prefix sum
3. Scatter to destination

**Total cycles**: ~20n per pass × 4 = 80n

**Crossover point**: Radix wins over quicksort when n > ~500 for uniformly distributed data.

---

## Introsort

Introsort combines quicksort's speed with heapsort's guaranteed O(n log n) worst case.

### Algorithm

1. Start with quicksort
2. Track recursion depth
3. If depth exceeds 2·log₂(n), switch to heapsort
4. Use insertion sort for small partitions (n ≤ 16)

### Pivot Selection

HYDRA uses median-of-three pivot selection:
- Compare arr[lo], arr[mid], arr[hi]
- Use median as pivot

This avoids O(n²) behavior on sorted/reverse-sorted inputs.

---

## Merge Strategies

### Four-Way Merge

Instead of pairwise merging, HYDRA can merge 4 sorted blocks simultaneously:

**Benefit**: log₄(K) passes instead of log₂(K)

**Trade-off**: 3 comparisons per element vs 1 for two-way

**Net effect**: ~1.33x faster for large K

### Sentinel-Based Merge

By placing INT32_MAX sentinels at the end of each input array, we eliminate bounds checking in the merge loop.

---

## References

1. Batcher, K. E. (1968). "Sorting Networks and Their Applications"
2. Musser, D. R. (1997). "Introspective Sorting and Selection Algorithms"
3. Ciura, M. (2001). "Best Increments for the Average Case of Shellsort"
4. McIlroy, P. M. (1993). "Optimistic Sorting and Information Theoretic Complexity"
