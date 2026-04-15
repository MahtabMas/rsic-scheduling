#pragma once
#include <vector>
#include <string>
#include "job.h"

// saves raw double job data to CSV files for plotting (before integer conversion)
void saveDataForPlotting_raw(const std::vector<RawJob_dimensional>& jobs, const std::string& output_dir);

// saves integer job data to CSV files for plotting (after integer conversion)
void saveDataForPlotting_int(const std::vector<Job_dimensional>& jobs, const std::string& output_dir);
