#pragma once
#include <vector>
#include <string>
#include <cstdint>
#include "job.h"

// reads raw Azure CSV file, returns all jobs with priority
std::vector<RawJob_dimensional> readCSVAndCreateJobs(const std::string& filename);

// filters jobs by priority: true = priority 0 only, false = all priorities
std::vector<RawJob_dimensional> filterByPriority(const std::vector<RawJob_dimensional>& jobs, bool priority_zero_only);

// convert raw double jobs to integer jobs
std::vector<Job_dimensional> convertToIntegerJobs(const std::vector<RawJob_dimensional>& jobs);

// filters jobs to keep only those within [d_lower, mu * d_lower] duration window
std::vector<Job_dimensional> filterByMu(const std::vector<Job_dimensional>& jobs, int64_t mu);

// computes mu = max_duration / min_duration from the job list
int64_t computeMu(const std::vector<Job_dimensional>& jobs);








