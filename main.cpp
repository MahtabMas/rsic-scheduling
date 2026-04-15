#include <iostream>
#include <string>
#include "include/data_loader.h"
#include "include/plotter.h"
#include "include/experiment.h"

int main() {
    // ---- Configuration ----
    std::string csv_path    = "/Users/m_masoor/Desktop/Data/Azure_Data0.csv";
    std::string output_dir  = "/Users/m_masoor/Desktop/Plot/";
    bool priority_zero_only = true;   // false = keep both priorities
    bool apply_mu_filter    = false;  // true = filter by custom mu
    int64_t custom_mu       = 100;    // only used if apply_mu_filter = true
    bool save_plots_raw     = true;   // save raw double plotting CSVs
    bool save_plots_int     = true;   // save integer plotting CSVs

    // ---- Algorithm Parameters (optimal values from grid search) ----
    int64_t threshold_clairvoyant        = 400000000;   // optimal τ for Greedy-Hybrid, Greedy-Departure
    int64_t threshold_greedy_greedy      = 1900000000;  // optimal τ for Greedy-Greedy
    int64_t threshold_greedy_duration    = 1900000000;  // optimal τ for Greedy-Duration
    int64_t threshold_new_greedy         = 2100000000;  // optimal τ for NewGreedy
    int64_t threshold_weakly_clairvoyant = 1300000000;  // optimal τ for FF-FF, FF-BF, BF-BF, BF-FF
    int64_t interval_departure           = 100;         // interval length for DepartureStrategy
    int64_t interval_duration            = 100;         // base α for Duration Strategy

    // ---- Grid Search Configuration ----
    bool run_grid_search_flag = false;      // true = run grid search for combined algorithms
    int64_t threshold_min  = 100000000;     // 10^8
    int64_t threshold_max  = 10000000000;   // 10^10
    int64_t threshold_step = 100000000;     // 10^8

    // ---- Step 1: Load data ----
    std::vector<RawJob_dimensional> rawJobs = readCSVAndCreateJobs(csv_path);

    // ---- Step 2: Filter by priority ----
    rawJobs = filterByPriority(rawJobs, priority_zero_only);

    // ---- Step 3: Save raw plots (before conversion) ----
    if (save_plots_raw)
        saveDataForPlotting_raw(rawJobs, output_dir);

    // ---- Step 4: Convert to integers ----
    std::vector<Job_dimensional> jobs = convertToIntegerJobs(rawJobs);

    // ---- Step 5: Save integer plots (after conversion) ----
    if (save_plots_int)
        saveDataForPlotting_int(jobs, output_dir);

    // ---- Step 6: Filter by mu (optional) ----
    if (apply_mu_filter)
        jobs = filterByMu(jobs, custom_mu);

    // ---- Step 7: Compute mu ----
    int64_t mu = computeMu(jobs);
    std::cout << "mu = " << mu << std::endl;

    // ---- Step 8: Run experiments ----
    run_Azure_experiment_dimensional(mu, jobs, output_dir,
        threshold_clairvoyant,
        threshold_greedy_greedy,
        threshold_greedy_duration,
        threshold_new_greedy,
        threshold_weakly_clairvoyant,
        interval_departure,
        interval_duration);

    // ---- Step 9: Grid Search (optional) ----
    if (run_grid_search_flag)
        run_grid_search(mu, jobs, output_dir, threshold_min, threshold_max, threshold_step);

    return 0;
}