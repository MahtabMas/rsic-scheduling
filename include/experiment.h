#pragma once
#include <vector>
#include <string>
#include <cstdint>
#include "job.h"
#include "opt.h"
#include "algorithms/algorithms.h"

void get_ratio_Azure(const std::vector<std::vector<int64_t>>& Es, int64_t mu,
                     const std::vector<std::string>& algs_array, std::vector<Job_dimensional>& jobs,
                     const std::string& output_dir,
                     int64_t threshold_clairvoyant,
                     int64_t threshold_greedy_greedy,
                     int64_t threshold_greedy_duration,
                     int64_t threshold_new_greedy,
                     int64_t threshold_weakly_clairvoyant,
                     int64_t interval_departure,
                     int64_t interval_duration);

void run_Azure_experiment_dimensional(int64_t mu, std::vector<Job_dimensional>& jobs,
                                      const std::string& output_dir,
                                      int64_t threshold_clairvoyant,
                                      int64_t threshold_greedy_greedy,
                                      int64_t threshold_greedy_duration,
                                      int64_t threshold_new_greedy,
                                      int64_t threshold_weakly_clairvoyant,
                                      int64_t interval_departure,
                                      int64_t interval_duration);

// runs grid search over thresholds for combined algorithms, writes results to CSV
void run_grid_search(int64_t mu, 
                     std::vector<Job_dimensional>& jobs,
                     const std::string& output_dir,
                     int64_t threshold_min,
                     int64_t threshold_max,
                     int64_t threshold_step);