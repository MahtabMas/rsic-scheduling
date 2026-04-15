# Renting Servers in the Cloud — Scheduling Benchmark

> 🏆 Best Paper Award — Algorithms and Complexity (ALGO 2025)

This project is the implementation behind my PhD research on the *Renting Servers in the Cloud* (RSiC) problem. To the best of our knowledge, this is the first comprehensive empirical study of scheduling algorithms for RSiC using real-world data. All previous work evaluated algorithms only on synthetic random inputs; we use real Microsoft Azure VM workload data.

The project includes implementations of all existing clairvoyant and non-clairvoyant algorithms, as well as several new algorithms I proposed, some of which outperform all previously known algorithms.

This work is part of my PhD research at Concordia University, Montreal, Canada, supervised by Prof. Lata Narayanan and Prof. Denis Pankratov.

## Paper

If you use this code in your research, please cite:

```bibtex
@inproceedings{masoori2025rsic,
  title     = {Renting Servers in the Cloud: Empirical Study on Real-World Data},
  author    = {Masoori, Mahtab and Narayanan, Lata and Pankratov, Denis},
  booktitle = {Algorithms and Complexity (ALGO 2025)},
  year      = {2025},
  note      = {Best Paper Award},
  institution = {Concordia University, Montreal, QC, Canada}
}
```

## Key Results

All experiments use real Microsoft Azure VM workload data (Machine 0, Priority 0, 112,552 jobs, µ = 11,794,735).

### Non-Clairvoyant Algorithms
| Algorithm | Competitive Ratio |
|---|---|
| NextFit | 4.48 |
| ModifiedNextFit | 4.48 |
| WorstFit | 2.81 |
| RandomFit | 2.55 |
| MoveToFront | 2.48 |
| LastFit | 2.24 |
| BestFit | 2.21 |
| FirstFit | 2.00 |
| ModifiedFirstFit | 2.00 |

### Clairvoyant Algorithms
| Algorithm | Competitive Ratio |
|---|---|
| DepartureStrategy | 6.17 |
| Duration | 1.83 |
| Greedy | 1.64 |
| NewHybrid | 1.30 |
| HybridAlgorithm | 1.29 |

### New Combined Algorithms (this work)
| Algorithm | Competitive Ratio |
|---|---|
| Greedy-Greedy | 1.28 |
| Greedy-Duration | 1.28 |
| NewGreedy | 1.41 |
| **Greedy-Hybrid** | **1.18** ← best overall |
| FirstFit-FirstFit | 1.41 |
| FirstFit-BestFit | 1.42 |

## Dataset

The experiments use the **Azure Public Dataset** from:

> Hadary, O., Marshall, L., Menache, I., et al. (2020).
> *Protean: VM Allocation Service at Scale.*
> USENIX OSDI 2020.

Available at: https://github.com/Azure/AzurePublicDataset

Each VM request is treated as a job with five resource dimensions: CPU, memory, SSD, NIC, and HDD. We exclude HDD (frequent null values) and focus on the remaining four dimensions. Server capacity is set to E = 1000 per dimension.

## Project Structure

```
rsic-scheduling/
├── include/
│   ├── job.h                    # Job data structures (raw CSV and integer-scaled)
│   ├── server.h                 # Server lifecycle and capacity tracking
│   ├── opt.h                    # OPT lower bound computation
│   ├── base_algorithm.h         # Base class for all algorithms
│   ├── data_loader.h            # CSV parsing, filtering, and preprocessing
│   ├── plotter.h                # Data export for plotting
│   ├── experiment.h             # Benchmark runner
│   ├── utils.h                  # Utility functions
│   └── algorithms/
│       ├── algorithms.h         # Includes all algorithms
│       ├── non_clairvoyant/     # NextFit, FirstFit, BestFit, MTF, ...
│       ├── clairvoyant/         # HA, NewHybrid, Greedy, Duration, ...
│       └── combined/            # Greedy-Hybrid, Greedy-Greedy, ...
│           └── weakly_clairvoyant/  # FF-FF, FF-BF, BF-BF, BF-FF
├── src/
│   ├── server.cpp
│   ├── opt.cpp
│   ├── base_algorithm.cpp
│   ├── data_loader.cpp
│   ├── plotter.cpp
│   └── experiment.cpp
├── main.cpp
├── CMakeLists.txt
└── README.md
```

## Build Instructions

### Requirements
- C++17 compiler (GCC ≥ 9, Clang ≥ 10)
- CMake ≥ 3.16

### Build
```bash
git clone https://github.com/your-username/rsic-scheduling.git
cd rsic-scheduling
mkdir build && cd build
cmake ..
make
```

### Run
```bash
./rsic_scheduler
```

## Configuration

All experiment parameters are set in `main.cpp`:

```cpp
// ---- Configuration ----
std::string csv_path    = "path/to/Azure_Data.csv";  // path to dataset
bool priority_zero_only = true;   // false = keep both priorities
bool apply_mu_filter    = false;  // true = filter jobs by custom mu
int64_t custom_mu       = 100;    // only used if apply_mu_filter = true
bool save_plots_raw     = true;   // save raw double plotting CSVs
bool save_plots_int     = true;   // save integer plotting CSVs

// ---- Algorithm Parameters (optimal values from grid search) ----
int64_t threshold_clairvoyant        = 400000000;   // optimal τ for Greedy-Hybrid
int64_t threshold_greedy_greedy      = 1900000000;  // optimal τ for Greedy-Greedy
int64_t threshold_greedy_duration    = 1900000000;  // optimal τ for Greedy-Duration
int64_t threshold_new_greedy         = 2100000000;  // optimal τ for NewGreedy
int64_t threshold_weakly_clairvoyant = 1300000000;  // optimal τ for weakly clairvoyant

// ---- Grid Search Configuration ----
bool run_grid_search_flag = false;    // true = run grid search for combined algorithms
int64_t threshold_min  = 100000000;  // 10^8
int64_t threshold_max  = 10000000000;// 10^10
int64_t threshold_step = 100000000;  // 10^8
```

## Algorithms

### Non-Clairvoyant
Job duration is unknown at arrival time.

| Class | Description |
|---|---|
| `NextFit_dimensional` | One open server at a time |
| `ModifiedNextFit_dimensional` | Separate NextFit for large and small jobs |
| `FirstFit_dimensional` | First server with enough capacity |
| `ModifiedFirstFit_dimensional` | Separate FirstFit for large and small jobs |
| `BestFit_dimensional` | Tightest-fitting server |
| `WorstFit_dimensional` | Loosest-fitting server |
| `LastFit_dimensional` | Most recently opened server first |
| `RandomFit_dimensional` | Random eligible server |
| `MTF_dimensional` | MoveToFront: most recently used server first |

### Clairvoyant
Job duration is known at arrival time.

| Class | Description |
|---|---|
| `DepartureStrategy` | Groups jobs by departure time intervals |
| `Duration` | Groups jobs by exponential duration buckets |
| `HybridAlgorithm` | HA: classifies by duration and arrival time, O(√log µ) competitive |
| `NewHybrid` | HA⊕d: d independent HA copies, one per dimension |
| `Greedy_dimensional` | Minimises server duration extension |

### Combined (this work)
Splits jobs into short and long using a duration threshold τ.

| Class | Type | Description |
|---|---|---|
| `Greedy_Greedy` | Clairvoyant | Greedy for both short and long jobs |
| `Greedy_dimensional_with_Hybrid` | Clairvoyant | Greedy for short, HA for long: best overall |
| `Greedy_Duration` | Clairvoyant | Greedy for short, Duration for long |
| `NewGreedy` | Clairvoyant | Greedy variant with extension threshold |
| `FirstFit_FirstFit` | Weakly Clairvoyant | FirstFit for both; recommended safe choice |
| `FirstFit_BestFit` | Weakly Clairvoyant | FirstFit for short, BestFit for long |
| `BestFit_BestFit` | Weakly Clairvoyant | BestFit for both |
| `BestFit_FirstFit` | Weakly Clairvoyant | BestFit for short, FirstFit for long |

## License
MIT License