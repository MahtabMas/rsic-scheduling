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
| ModifiedNextFit (MNF) | 4.48 |
| WorstFit | 2.81 |
| RandomFit | 2.55 |
| MoveToFront (MTF) | 2.48 |
| LastFit | 2.24 |
| BestFit | 2.21 |
| FirstFit | 2.00 |
| ModifiedFirstFit (MFF) | 2.00 |

### Clairvoyant Algorithms
| Algorithm | Competitive Ratio |
|---|---|
| Departure Strategy | 6.17 |
| Duration Strategy | 1.83 |
| Greedy | 1.64 |
| New Hybrid (HA⊕d) | 1.30 |
| Hybrid Algorithm (HA) | 1.29 |

### New Combined Algorithms (this work)
| Algorithm | Competitive Ratio |
|---|---|
| New Greedy | 1.41 |
| FirstFit-FirstFit | 1.41 |
| FirstFit-BestFit | 1.42 |
| Greedy-Greedy | 1.28 |
| Greedy-Duration | 1.28 |
| **Greedy-Hybrid** | **1.18** ← best overall |

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
├── analysis/
│   └── mip_optimal.py           # Offline MIP optimal solver (OR-Tools/SCIP)
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

| Algorithm | Description |
|---|---|
| NextFit | One open server at a time |
| ModifiedNextFit (MNF) | Separate NextFit for large and small jobs |
| FirstFit | First server with enough capacity |
| ModifiedFirstFit (MFF) | Separate FirstFit for large and small jobs |
| BestFit | Tightest-fitting server |
| WorstFit | Loosest-fitting server |
| LastFit | Most recently opened server first |
| RandomFit | Random eligible server |
| MoveToFront (MTF) | Most recently used server first |

### Clairvoyant
Job duration is known at arrival time.

| Algorithm | Description |
|---|---|
| Departure Strategy | Groups jobs by departure time intervals |
| Duration Strategy | Groups jobs by exponential duration buckets |
| Hybrid Algorithm (HA) | Classifies by duration and arrival time, O(√log µ) competitive |
| New Hybrid (HA⊕d) | d independent HA copies, one per dimension |
| Greedy | Minimises server duration extension |

### Combined (this work)
Splits jobs into short and long using a duration threshold τ.

| Algorithm | Type | Description |
|---|---|---|
| Greedy-Greedy | Clairvoyant | Greedy for both short and long jobs |
| Greedy-Hybrid | Clairvoyant | Greedy for short, HA for long: best overall |
| Greedy-Duration | Clairvoyant | Greedy for short, Duration Strategy for long |
| New Greedy | Clairvoyant | Greedy variant with extension threshold |
| FirstFit-FirstFit | Weakly Clairvoyant | FirstFit for both, recommended safe choice |
| FirstFit-BestFit | Weakly Clairvoyant | FirstFit for short, BestFit for long |
| BestFit-BestFit | Weakly Clairvoyant | BestFit for both |
| BestFit-FirstFit | Weakly Clairvoyant | BestFit for short, FirstFit for long |

## Offline Optimal (MIP)

The main experiments report competitive ratios against the lower bound from Lemma 1 (Murhekar et al.), since computing the exact offline optimal is intractable on the full dataset of 112,552 jobs. To validate this choice, we formulate the true offline optimal as a MILP and solve it on representative µ-filtered batches using OR-Tools/SCIP.

**Formulation.** Given $n$ jobs with start time $a_j$, end time $f_j$, resource request $\mathbf{r}_j \in \mathbb{Z}^4$, and $m$ identical servers with capacity $E = 1000$ per dimension:

$$\min \sum_i rent_i$$

subject to:

$$\sum_i x_{ij} = 1 \quad \forall j \tag{1}$$

$$\sum_{j:\, a_j \leq t < f_j} r_j^k \cdot x_{ij} \leq E \quad \forall i, k, t \tag{2}$$

$$e_i \geq f_j \cdot x_{ij} \quad \forall i, j \tag{3}$$

$$o_i \leq a_j + M(1 - x_{ij}) \quad \forall i, j \tag{4}$$

$$e_i - o_i \leq M \cdot y_i \quad \forall i \tag{5}$$

$$y_i \geq x_{ij} \quad \forall i, j \tag{6}$$

$$rent_i \geq e_i - o_i \quad \forall i \tag{7}$$

$$x_{ij},\, y_i \in \{0,1\}, \quad e_i,\, o_i,\, rent_i \geq 0$$

Constraint (2) is time-aware: at each event time $t$, only jobs active on server $i$ at that moment contribute to the capacity check. Constraints (3)–(4) model server open and close times via big-M. Constraint (7) ensures non-negative rental duration.

**Setup.**

```bash
cd analysis
python3 -m venv mip_env
source mip_env/bin/activate
pip install ortools pandas numpy
# update base_path in mip_optimal.py to point to your mu-filtered CSV files
python3 mip_optimal.py
```

To generate the µ-filtered CSV files, set `apply_mu_filter = true` and the desired `custom_mu` in `main.cpp`, rebuild, and run the C++ scheduler. Repeat for each µ value.

**Results** (batch size = 20 jobs, Machine 0, Priority 0):

| µ | Greedy/LB | MIP/LB | Greedy/MIP | Status |
|---|---|---|---|---|
| 2 | 1.1432 | 1.0001 | 1.1431 | OPTIMAL |
| 5 | 1.3556 | 1.0660 | 1.2717 | OPTIMAL |
| 10 | 1.5897 | 1.0526 | 1.5103 | OPTIMAL |
| 100 | 1.0337 | 1.0032 | 1.0304 | OPTIMAL |

MIP/LB ratios between 1.0001 and 1.0660 confirm the lower bound is tight across all µ values, validating its use as a proxy for OPT in the main experiments.

