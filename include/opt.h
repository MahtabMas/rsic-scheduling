#pragma once
#include <vector>
#include <algorithm>
#include <cstdint>
#include "job.h"

class Opt_dimensional{
private:
    std::vector<Job_dimensional> jobs;
public:
    Opt_dimensional(const std::vector<Job_dimensional>& list_of_intervals);
    std::vector<int64_t> volumeAtTime(int64_t time);
    std::pair<long long, long long> ceilLBandSpan(const std::vector<int64_t>& E);
};