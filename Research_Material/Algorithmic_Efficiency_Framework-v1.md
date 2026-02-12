# Algorithmic Efficiency Mathematical Framework
## Clock-Adjusted Complexity Analysis & Hybrid Sorting Mechanisms

---

## 1. Clock Speed Model with Heuristic Variance

### 1.1 Base Clock Model

Let the nominal clock frequency be:

$$f_0 = 250 \text{ MHz} = 250 \times 10^6 \text{ Hz}$$

The actual instantaneous frequency incorporates multiple variance factors:

$$f_{actual}(t) = f_0 \cdot \left(1 + \sum_{i=1}^{k} \epsilon_i(t)\right)$$

Where each $\epsilon_i$ represents a heuristic adjustment factor:

| Factor | Symbol | Typical Range | Model |
|--------|--------|---------------|-------|
| Thermal drift | $\epsilon_T$ | ±5% | $\epsilon_T = -\alpha_T \cdot (T - T_{ref}) / T_{max}$ |
| Voltage fluctuation | $\epsilon_V$ | ±3% | $\epsilon_V = \beta_V \cdot (V_{dd} - V_{nom}) / V_{nom}$ |
| Process variation | $\epsilon_P$ | ±2% | Static per-chip constant |
| Dynamic scaling | $\epsilon_D$ | ±10% | Governor-controlled |

### 1.2 Composite Variance Model

For a bounded ±10% total variance:

$$\epsilon_{total} \in [-0.10, +0.10]$$

$$f_{actual} \in [f_0 \cdot 0.90, f_0 \cdot 1.10] = [225 \text{ MHz}, 275 \text{ MHz}]$$

**Stochastic Model** (for simulation):

$$\epsilon_{total} \sim \mathcal{N}(0, \sigma^2) \text{ truncated to } [-0.10, 0.10]$$

Where $\sigma \approx 0.033$ gives 99.7% within bounds (3σ rule).

### 1.3 Effective Cycle Time

$$\tau_{cycle}(t) = \frac{1}{f_{actual}(t)}$$

**At nominal**: $\tau_0 = 4 \text{ ns}$

**Range**: $\tau \in [3.64 \text{ ns}, 4.44 \text{ ns}]$

---

## 2. Time Complexity to Real-Time Mapping

### 2.1 Fundamental Equation

The actual execution time for an algorithm:

$$T_{exec}(n) = \frac{C_{op} \cdot g(n)}{f_{actual}}$$

Where:
- $C_{op}$ = cycles per fundamental operation (constant overhead)
- $g(n)$ = complexity function (number of operations)
- $f_{actual}$ = effective clock frequency

### 2.2 Complexity Functions

| Complexity | $g(n)$ | Name |
|------------|--------|------|
| $O(1)$ | $g_1(n) = c_1$ | Constant |
| $O(n)$ | $g_n(n) = c_2 \cdot n$ | Linear |
| $O(n^2)$ | $g_{n2}(n) = c_3 \cdot n^2$ | Quadratic |
| $O(2^n)$ | $g_{exp}(n) = c_4 \cdot 2^n$ | Exponential |

Where $c_i$ are algorithm-specific constants (leading coefficients).

### 2.3 Detailed Time Equations

**Constant Time O(1):**
$$T_1(n) = \frac{C_{op} \cdot c_1}{f_0 (1 + \epsilon)} = \frac{k_1}{f_0(1 + \epsilon)}$$

**Linear Time O(n):**
$$T_n(n) = \frac{C_{op} \cdot c_2 \cdot n}{f_0 (1 + \epsilon)} = \frac{k_2 \cdot n}{f_0(1 + \epsilon)}$$

**Quadratic Time O(n²):**
$$T_{n^2}(n) = \frac{C_{op} \cdot c_3 \cdot n^2}{f_0 (1 + \epsilon)} = \frac{k_3 \cdot n^2}{f_0(1 + \epsilon)}$$

**Exponential Time O(2ⁿ):**
$$T_{2^n}(n) = \frac{C_{op} \cdot c_4 \cdot 2^n}{f_0 (1 + \epsilon)} = \frac{k_4 \cdot 2^n}{f_0(1 + \epsilon)}$$

---

## 3. Efficiency Metrics

### 3.1 Absolute Efficiency

$$\eta_{abs}(n) = \frac{T_{ideal}(n)}{T_{actual}(n)} = \frac{g(n) / f_0}{g(n) / f_{actual}} = \frac{f_{actual}}{f_0} = 1 + \epsilon$$

### 3.2 Relative Efficiency (Algorithm Comparison)

Comparing algorithm A vs algorithm B:

$$\eta_{rel}(n) = \frac{T_B(n)}{T_A(n)} = \frac{g_B(n)}{g_A(n)}$$

**Linear vs Quadratic:**
$$\eta_{L/Q}(n) = \frac{n^2}{n} = n$$

*Linear is n times more efficient than quadratic.*

**Linear vs Exponential:**
$$\eta_{L/E}(n) = \frac{2^n}{n}$$

*This ratio explodes exponentially.*

### 3.3 Throughput Efficiency

Operations per second:

$$\Theta(n) = \frac{f_{actual}}{C_{op} \cdot g(n)}$$

**For sorting** (operations = comparisons + swaps):

$$\Theta_{sort}(n) = \frac{f_{actual}}{C_{cmp} \cdot N_{cmp}(n) + C_{swap} \cdot N_{swap}(n)}$$

---

## 4. Comparative Analysis: Linear vs Exponential

### 4.1 Crossover Point Analysis

Find n where two algorithms have equal time:

$$T_A(n) = T_B(n)$$
$$\frac{k_A \cdot g_A(n)}{f} = \frac{k_B \cdot g_B(n)}{f}$$
$$k_A \cdot g_A(n) = k_B \cdot g_B(n)$$

**Example: O(n²) vs O(n log n)**

$$c_3 \cdot n^2 = c_2 \cdot n \log n$$
$$n = \frac{c_2}{c_3} \cdot \log n$$

Solved numerically or via Lambert W function:

$$n^* = -\frac{c_2}{c_3} \cdot W_{-1}\left(-\frac{c_3}{c_2}\right)$$

### 4.2 Efficiency Ratio Over Input Size

Define the efficiency advantage function:

$$A(n) = \log_{10}\left(\frac{T_{slow}(n)}{T_{fast}(n)}\right)$$

**For O(n) vs O(2ⁿ):**

$$A(n) = \log_{10}\left(\frac{k_4 \cdot 2^n}{k_2 \cdot n}\right) = \log_{10}(k_4/k_2) + n\log_{10}(2) - \log_{10}(n)$$

$$A(n) \approx 0.301n - \log_{10}(n) + \text{const}$$

*Linear growth in orders of magnitude advantage.*

---

## 5. Hybrid Combinational Sorting Mechanism

### 5.1 Decision Function Architecture

The hybrid sorter selects sub-algorithms based on input characteristics:

$$\text{Algorithm} = \Phi(n, \rho, \sigma_d, M)$$

Where:
- $n$ = input size
- $\rho$ = pre-sortedness ratio (0 = random, 1 = sorted)
- $\sigma_d$ = data distribution variance
- $M$ = available memory

### 5.2 Pre-sortedness Metric

**Runs-based measure:**

$$\rho = 1 - \frac{R - 1}{n - 1}$$

Where R = number of ascending runs in the sequence.

**Inversion-based measure:**

$$\rho_{inv} = 1 - \frac{2I}{n(n-1)}$$

Where I = number of inversions (pairs where $a[i] > a[j]$ for $i < j$).

### 5.3 Hybrid Decision Matrix

```
┌─────────────────────────────────────────────────────────────────┐
│                    HYBRID SORT DECISION TREE                    │
├─────────────────────────────────────────────────────────────────┤
│                                                                 │
│  INPUT: Array A[0..n-1]                                         │
│                                                                 │
│  ┌──────────────┐                                               │
│  │ n ≤ θ₁ (16)  │──YES──▶ INSERTION SORT [O(n²) but low k]      │
│  └──────┬───────┘                                               │
│         │NO                                                     │
│         ▼                                                       │
│  ┌──────────────┐                                               │
│  │ ρ ≥ θ₂ (0.9) │──YES──▶ TIMSORT [O(n) for nearly sorted]      │
│  └──────┬───────┘                                               │
│         │NO                                                     │
│         ▼                                                       │
│  ┌──────────────┐                                               │
│  │ M ≥ n words  │──YES──▶ MERGE SORT [O(n log n), stable]       │
│  └──────┬───────┘                                               │
│         │NO                                                     │
│         ▼                                                       │
│  ┌──────────────────┐                                           │
│  │ σ_d low (uniform)│──YES──▶ RADIX SORT [O(nk), k=digits]      │
│  └──────┬───────────┘                                           │
│         │NO                                                     │
│         ▼                                                       │
│  INTROSORT [Quicksort + Heapsort fallback]                      │
│                                                                 │
└─────────────────────────────────────────────────────────────────┘
```

### 5.4 Mathematical Selection Function

$$\Phi(n, \rho, \sigma_d, M) = \arg\min_{A \in \mathcal{A}} \left[ \hat{T}_A(n, \rho, \sigma_d, M) \right]$$

Where $\mathcal{A} = \{\text{Insertion, Tim, Merge, Radix, Intro}\}$

**Estimated time for each:**

$$\hat{T}_{ins}(n) = k_{ins} \cdot n^2 \cdot (1 - \rho)$$

$$\hat{T}_{tim}(n, \rho) = k_{tim} \cdot n \cdot (1 + (1-\rho) \cdot \log n)$$

$$\hat{T}_{merge}(n) = k_{merge} \cdot n \log n$$

$$\hat{T}_{radix}(n, \sigma_d) = k_{radix} \cdot n \cdot \lceil \log_b(\sigma_d \cdot n) \rceil$$

$$\hat{T}_{intro}(n) = k_{intro} \cdot n \log n$$

### 5.5 Threshold Optimisation

Optimal thresholds minimise expected total time over workload distribution:

$$(\theta_1^*, \theta_2^*, \ldots) = \arg\min_{\theta} \mathbb{E}_{(n,\rho,\sigma_d,M) \sim \mathcal{D}} \left[ T_{\Phi_\theta}(n, \rho, \sigma_d, M) \right]$$

**Empirically determined thresholds (typical values):**

| Threshold | Symbol | Typical Value | Purpose |
|-----------|--------|---------------|---------|
| Small array cutoff | $\theta_1$ | 16-32 | Switch to insertion sort |
| Presorted threshold | $\theta_2$ | 0.85-0.95 | Use adaptive sort |
| Recursion depth limit | $\theta_3$ | $2 \log_2 n$ | Introsort heap fallback |
| Radix efficiency threshold | $\theta_4$ | $n > 10^4$ | Consider radix sort |

---

## 6. Sub-Mechanism Equations

### 6.1 Insertion Sort (Small Arrays)

**Comparisons:**
$$C_{ins}(n, \rho) = \frac{n(n-1)}{2} \cdot (1 - \rho) + n - 1$$

**Best case (sorted):** $C_{ins} = n - 1$
**Worst case (reversed):** $C_{ins} = \frac{n(n-1)}{2}$

**Time with clock variance:**
$$T_{ins}(n) = \frac{(C_{cmp} \cdot C_{ins} + C_{swap} \cdot S_{ins})}{f_0(1 + \epsilon)}$$

### 6.2 Merge Sort (Guaranteed Performance)

**Comparisons:**
$$C_{merge}(n) = n \lceil \log_2 n \rceil - 2^{\lceil \log_2 n \rceil} + 1$$

**Approximation:** $C_{merge}(n) \approx n \log_2 n - 1.44n$

**Auxiliary space:** $M_{merge} = n$

### 6.3 Quicksort (Average Case Champion)

**Expected comparisons:**
$$\mathbb{E}[C_{quick}(n)] = 2n \ln n \approx 1.39 n \log_2 n$$

**Variance:**
$$\text{Var}[C_{quick}(n)] = \left(7 - \frac{2\pi^2}{3}\right)n^2 \approx 0.42 n^2$$

**Probability of worst case (without randomisation):**
$$P(\text{worst}) = \frac{2}{n!} \text{ (sorted or reverse sorted input)}$$

### 6.4 Introsort (Hybrid Quick+Heap)

**Depth-limited recursion:**
$$d_{max} = \lfloor 2 \log_2 n \rfloor$$

**Time-bound:**
$$T_{intro}(n) \leq k_{quick} \cdot n \log n + k_{heap} \cdot \frac{n}{2^{d_{max}}} \cdot \log\left(\frac{n}{2^{d_{max}}}\right)$$

Simplifies to $O(n \log n)$ guaranteed.

### 6.5 Timsort (Adaptive)

**Run detection cost:** $O(n)$

**Merge cost (m runs):**
$$C_{tim}(n, m) = n \cdot \lceil \log_2 m \rceil$$

**For nearly sorted (m small):** Approaches $O(n)$
**For random (m ≈ n/minrun):** $O(n \log n)$

---

## 7. Composite Efficiency Formula

### 7.1 Total Hybrid Sort Time

$$T_{hybrid}(n) = T_{analyze}(n) + T_{selected}(n)$$

**Analysis overhead:**
$$T_{analyze}(n) = \frac{C_{analyze}}{f_{actual}} \cdot n \quad \text{(linear scan)}$$

Usually $C_{analyze} \ll C_{sort}$, so overhead is negligible.

### 7.2 Expected Performance

$$\mathbb{E}[T_{hybrid}(n)] = \sum_{A \in \mathcal{A}} P(\Phi = A) \cdot \mathbb{E}[T_A(n)]$$

### 7.3 Clock-Adjusted Efficiency Ratio

$$\eta_{hybrid}(n, \epsilon) = \frac{T_{baseline}(n, 0)}{T_{hybrid}(n, \epsilon)}$$

Where baseline is standard quicksort at nominal clock.

**With favorable conditions ($\rho$ high, optimal algorithm selected):**

$$\eta_{hybrid} > 1 + \epsilon \quad \text{(exceeds clock advantage)}$$

---

## 8. Practical Implementation Constants

### 8.1 Cycle Costs (ARM Cortex-M0+, e.g., RP2040)

| Operation | Cycles | Symbol |
|-----------|--------|--------|
| Compare (32-bit) | 1-2 | $C_{cmp}$ |
| Swap (via temp) | 3-6 | $C_{swap}$ |
| Array index | 2-3 | $C_{idx}$ |
| Function call | 4-8 | $C_{call}$ |
| Branch (taken) | 2-3 | $C_{br}$ |

### 8.2 Algorithm Constants (Measured)

| Algorithm | $k$ (cycles/op) | Typical Use Case |
|-----------|-----------------|------------------|
| Insertion | 5-8 | n < 16 |
| Merge | 12-15 | General, need stability |
| Quicksort | 8-12 | General, in-place |
| Heapsort | 15-20 | Guaranteed O(n log n) |
| Radix (base 256) | 4-6 | Integer keys, large n |

### 8.3 Final Time Estimation

$$T_{final}(n) = \frac{k_{algo} \cdot g(n)}{f_0 \cdot (1 + \epsilon)} \text{ seconds}$$

**Example: Quicksort, n=1000, 250MHz nominal, ε=0:**

$$T = \frac{10 \cdot 1000 \cdot \log_2(1000)}{250 \times 10^6} = \frac{10 \cdot 1000 \cdot 10}{250 \times 10^6} = \frac{100000}{250000000} = 0.4 \text{ ms}$$

---

## 9. Summary Equations Card

```
╔══════════════════════════════════════════════════════════════════╗
║                 QUICK REFERENCE EQUATIONS                        ║
╠══════════════════════════════════════════════════════════════════╣
║                                                                  ║
║  CLOCK:     f = f₀(1 + ε),  ε ∈ [-0.10, +0.10]                   ║
║                                                                  ║
║  TIME:      T(n) = k·g(n) / f                                    ║
║                                                                  ║
║  O(1):      T = k₁ / f                                           ║
║  O(n):      T = k₂·n / f                                         ║
║  O(n²):     T = k₃·n² / f                                        ║
║  O(2ⁿ):     T = k₄·2ⁿ / f                                        ║
║                                                                  ║
║  EFFICIENCY: η = f_actual / f_nominal = 1 + ε                    ║
║                                                                  ║
║  HYBRID:    Φ(n,ρ,σ,M) = argmin[T̂_A(n,ρ,σ,M)]                    ║
║                                                                  ║
║  PRESORTED: ρ = 1 - (R-1)/(n-1)                                  ║
║                                                                  ║
║  THRESHOLDS: θ₁=16 (small), θ₂=0.9 (presort), θ₃=2log₂n (depth)  ║
║                                                                  ║
╚══════════════════════════════════════════════════════════════════╝
```

---

*Framework Version 1.0 — Designed for embedded systems analysis. Make with Claude Opus 4.6 and MilkmanAbi.*
