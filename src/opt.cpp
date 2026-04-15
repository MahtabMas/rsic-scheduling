#include "../include/opt.h"


Opt_dimensional::Opt_dimensional(const std::vector<Job_dimensional>& list_of_intervals) : jobs(list_of_intervals) {}

std::vector<int64_t> Opt_dimensional::volumeAtTime(int64_t time) {
    //initialize the result vector with 0 at each element
    std::vector<int64_t> result(jobs[0].size.size(), 0);
    for (const Job_dimensional& job : jobs) {
        if (time >= job.start && time < job.end) {
            for (size_t i = 0; i < job.size.size(); ++i) {
                result[i] += job.size[i];
            }
        }
    }
    return result;
}

std::pair<long long, long long> Opt_dimensional::ceilLBandSpan(const std::vector<int64_t>& E) {
    long long ceilLB = 0LL;
    long long span = 0LL;
    std::vector<int64_t> eventTimes;

    for (const Job_dimensional& job : jobs) {
        eventTimes.push_back(job.start);
        eventTimes.push_back(job.end);
    }
    std::sort(eventTimes.begin(), eventTimes.end());
    for (size_t i = 0; i < eventTimes.size() - 1; i++) {
        int64_t duration = eventTimes[i + 1] - eventTimes[i];
        std::vector<int64_t> volume = volumeAtTime(eventTimes[i]);

        // Rounding logic
        for (size_t j = 0; j < volume.size(); ++j) {
            int64_t currentE = E[j];
            if (volume[j] % currentE != 0) {
                volume[j] = ((volume[j] / currentE) + 1);
            } else {
                volume[j] = volume[j]/currentE;
            }
        }
        // Find the maximum value in the rounded volume vector
        int64_t maxVolume = *std::max_element(volume.begin(), volume.end());
        if (maxVolume != 0) 
            // Increment span separately, if needed
            span += static_cast<long long>(duration); 
        // Use the maximum value in the calculation
        ceilLB += static_cast<long long>(duration) * maxVolume;  
    }
    return std::make_pair(ceilLB, span);
}

