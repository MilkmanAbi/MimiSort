<div align="center">

# 🐉 HYDRA-SORT

**An experimental, ultra-custom sorting mechanism for RP2040**

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Platform: RP2040](https://img.shields.io/badge/Platform-RP2040-red.svg)](https://www.raspberrypi.com/products/rp2040/)
[![Status: Experimental](https://img.shields.io/badge/Status-Experimental-orange.svg)](#disclaimer)

*Focused on optimization, exploration, and benchmarking of unique sorting strategies.*

[Features](#features) • [Performance](#performance) • [Installation](#installation) • [Usage](#usage) • [Documentation](#documentation)

</div>

---

## Overview

HYDRA-SORT is a multi-layered, adaptive sorting system specifically engineered for the RP2040 microcontroller. It combines multiple sorting algorithms, hardware-aware optimizations, and dual-core parallelism to achieve significant speedups over standard implementations.

This project was developed collaboratively with Claude (Anthropic) as an exploration of algorithmic optimization on constrained embedded systems.

> ⚠️ **Disclaimer:** This is a research and learning project. It prioritizes exploration and experimentation over production readiness. Use at your own discretion.

---

## Features

### Adaptive Algorithm Selection
- **Input Analysis Engine** — Single O(n) pass detects presortedness, value range, and data distribution
- **Dynamic Strategy Selection** — Automatically chooses optimal algorithm based on input characteristics
- **Type-Specialized Paths** — Dedicated implementations for `uint8_t`, `uint16_t`, and `int32_t`

### Hardware-Aware Optimizations
- **Dual-Core Parallelism** — Distributes work across both Cortex-M0+ cores
- **Bank-Aware Memory Allocation** — Minimizes SRAM bank conflicts
- **RAM Function Placement** — Critical loops execute from RAM, avoiding Flash latency
- **DMA Integration** — Overlaps data transfer with computation

### Micro-Optimizations
- **Register-Only Sorting Networks** — Sort 4-16 elements entirely in CPU registers
- **Sentinel Elimination** — Removes bounds checks from inner loops
- **Branchless Comparators** — Minimizes pipeline stalls
- **SWAR Operations** — Process multiple small elements in parallel

### Algorithm Suite
| Algorithm | Use Case | Complexity |
|-----------|----------|------------|
| Sorting Networks | n ≤ 16 | O(1) |
| Insertion Sort (Sentinel) | Nearly sorted data | O(n) to O(n²) |
| Shell Sort (Ciura gaps) | Small-medium arrays | O(n log² n) |
| Radix Sort (Base-256) | Uniform integer data | O(n) |
| Counting Sort | uint8/uint16 data | O(n + k) |
| Introsort | General purpose | O(n log n) |
| Four-Way Cascade Merge | Block merging | O(n log n) |

---

## Performance

Benchmarks conducted on RP2040 @ 250MHz, sorting `int32_t` arrays.

### Speedup vs Standard Quicksort

| Array Size | HYDRA-SORT | Standard QSort |
|------------|------------|----------------|
| n = 100 | **1.5x** faster | baseline |
| n = 1,000 | **2.8x** faster | baseline |
| n = 10,000 | **4.0x** faster | baseline |
| n = 20,480 | **4.2x** faster | baseline |

### Specialized Type Performance

| Data Type | Algorithm Used | Speedup |
|-----------|----------------|---------|
| `uint8_t` | Counting Sort | **~25x** |
| `uint16_t` | 2-Pass Radix | **~5x** |
| `int32_t` | Adaptive Hybrid | **~4x** |

### Cycle Counts (n = 10,000)

```
Standard Quicksort:  ~1,330,000 cycles
HYDRA-SORT v2.0:     ~330,000 cycles
─────────────────────────────────────
Reduction:           75% fewer cycles
```

---

## Installation

### Prerequisites

- [Raspberry Pi Pico SDK](https://github.com/raspberrypi/pico-sdk) (v1.5.0+)
- CMake (v3.13+)
- ARM GCC Toolchain

### Clone Repository

```bash
git clone https://github.com/yourusername/hydra-sort.git
cd hydra-sort
```

### Build

```bash
mkdir build && cd build
cmake ..
make
```

### Integration

Copy `hydra_sort_v2.h` into your project and include it:

```c
#include "hydra_sort_v2.h"
```

---

## Usage

### Basic Usage

```c
#include "hydra_sort_v2.h"

int32_t data[1000];
int32_t aux[1000];  // Auxiliary buffer for radix sort

// Fill data...

// Sort with default balanced profile
hydra_sort(data, 1000, aux, HYDRA_PROFILE_BALANCED);
```

### Specialized Type Sorting

```c
// For uint8_t arrays (uses counting sort - extremely fast)
uint8_t bytes[5000];
hydra_sort_u8(bytes, 5000);

// For uint16_t arrays
uint16_t shorts[2000];
uint16_t aux_shorts[2000];
hydra_sort_u16(shorts, aux_shorts, 2000);
```

### Performance Profiles

```c
// Maximum speed, higher power consumption
hydra_sort(data, n, aux, HYDRA_PROFILE_ULTRA_FAST);

// Balanced speed and power (recommended)
hydra_sort(data, n, aux, HYDRA_PROFILE_BALANCED);

// Minimum energy consumption
hydra_sort(data, n, aux, HYDRA_PROFILE_LOW_POWER);
```

### Small Array Fast Paths

For known small sizes, use direct functions to avoid analysis overhead:

```c
int32_t arr4[4] = {3, 1, 4, 1};
hydra_sort4(arr4);  // ~30 cycles

int32_t arr8[8] = {5, 9, 2, 6, 5, 3, 5, 8};
hydra_sort8(arr8);  // ~150 cycles
```

---

## Documentation

### Project Structure

```
hydra-sort/
├── include/
│   └── hydra_sort_v2.h      # Main header (single-file library)
├── docs/
│   ├── ALGORITHM.md         # Detailed algorithm documentation
│   ├── OPTIMIZATION.md      # Optimization techniques explained
│   └── BENCHMARKS.md        # Full benchmark methodology & results
├── examples/
│   ├── basic_usage.c        # Simple usage example
│   ├── benchmark.c          # Benchmarking harness
│   └── parallel_demo.c      # Dual-core demonstration
├── tests/
│   └── test_correctness.c   # Correctness verification
├── CMakeLists.txt
├── LICENSE
└── README.md
```

### Algorithm Selection Logic

```
┌─────────────────────────────────────────────────────────┐
│                  HYDRA Decision Tree                    │
├─────────────────────────────────────────────────────────┤
│                                                         │
│  n ≤ 16?  ──YES──▶  Sorting Network (register-only)    │
│     │                                                   │
│     NO                                                  │
│     ▼                                                   │
│  Nearly sorted?  ──YES──▶  Insertion Sort (sentinel)   │
│     │                                                   │
│     NO                                                  │
│     ▼                                                   │
│  Small range?  ──YES──▶  Radix/Counting Sort           │
│     │                                                   │
│     NO                                                  │
│     ▼                                                   │
│  n > 4096?  ──YES──▶  Parallel Block Sort + Merge      │
│     │                                                   │
│     NO                                                  │
│     ▼                                                   │
│  Introsort (Quicksort + Heapsort fallback)             │
│                                                         │
└─────────────────────────────────────────────────────────┘
```

### Memory Requirements

| Configuration | RAM Usage | Flash Usage |
|---------------|-----------|-------------|
| Minimal | n + 512 B | ~4 KB |
| Standard | 2n + 1 KB | ~8 KB |
| Full (parallel) | 3n + 2 KB | ~12 KB |

---

## Roadmap

- [ ] PIO-accelerated counting sort
- [ ] DMA triple-buffering for external sort
- [ ] Interpolation-accelerated merge
- [ ] RP2350 optimizations (ARM Cortex-M33)
- [ ] Comprehensive benchmark suite
- [ ] Formal verification of correctness

---

## Contributing

Contributions are welcome! This is an experimental project, so feel free to:

- Propose new optimization techniques
- Add benchmarks for different scenarios
- Improve documentation
- Report bugs or correctness issues

Please open an issue to discuss significant changes before submitting a PR.

---

## Acknowledgments

- Developed in collaboration with [Claude](https://www.anthropic.com/claude) (Anthropic)
- Inspired by [Timsort](https://en.wikipedia.org/wiki/Timsort), [Introsort](https://en.wikipedia.org/wiki/Introsort), and various academic papers on adaptive sorting
- Built for the [RP2040](https://www.raspberrypi.com/products/rp2040/) microcontroller

---

## License

This project is licensed under the MIT License — see the [LICENSE](LICENSE) file for details.

---

<div align="center">

*"We're not just sorting data. We're making electrons work overtime."*

**HYDRA-SORT** — Experimental Sorting for Embedded Systems

</div>
