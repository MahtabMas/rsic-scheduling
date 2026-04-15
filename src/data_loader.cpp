#include <fstream>
#include <sstream>
#include <algorithm>
#include <iostream>
#include <cmath>
#include <set>
#include <limits>
#include <numeric>
#include "../include/data_loader.h"

std::vector<RawJob_dimensional> readCSVAndCreateJobs(const std::string& filename) {
    std::ifstream file(filename);
    std::string line;
    std::vector<RawJob_dimensional> csvJobs;

    if (!file.is_open()) {
        std::cerr << "Could not open the file: " << filename << std::endl;
        return csvJobs;
    }

    std::getline(file, line); // skip header line

    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string item;
        std::vector<std::string> row;

        while (std::getline(ss, item, ',')) {
            row.push_back(item);
        }

        if (row.size() < 10) continue; // ensure we have enough columns
        double start    = std::stod(row[3]);
        double end      = std::stod(row[4]);
        double core     = std::stod(row[5]);
        double memory   = std::stod(row[6]);
        double ssd      = std::stod(row[7]);
        double nic      = std::stod(row[8]);
        int priority    = std::stoi(row[9]);

        std::vector<double> sizes = {core, memory, ssd, nic};
        csvJobs.push_back(RawJob_dimensional(start, end, sizes, priority));
    }
    file.close();
    return csvJobs;
}

std::vector<RawJob_dimensional> filterByPriority(const std::vector<RawJob_dimensional>& jobs, bool priority_zero_only) {
    if (!priority_zero_only) return jobs; // keep all jobs

    std::vector<RawJob_dimensional> filtered;
    for (const auto& job : jobs) {
        if (job.priority == 0) {
            filtered.push_back(job);
        }
    }
    return filtered;
}

std::vector<Job_dimensional> convertToIntegerJobs(const std::vector<RawJob_dimensional>& jobs) {
    double eps = 1e-9;

    // sort jobs by start time
    std::vector<RawJob_dimensional> sortedJobs = jobs;
    std::sort(sortedJobs.begin(), sortedJobs.end(), [](const RawJob_dimensional& a, const RawJob_dimensional& b) {
        return a.start < b.start;
    });

    // find minimum non-zero time gap (delta) across all event times
    std::set<double> eventTimesSet;
    for (const auto& job : sortedJobs) {
        eventTimesSet.insert(job.start);
        eventTimesSet.insert(job.end);
    }
    std::vector<double> sortedEventTimes(eventTimesSet.begin(), eventTimesSet.end());

    double delta = std::numeric_limits<double>::infinity();
    for (size_t i = 0; i < sortedEventTimes.size() - 1; ++i) {
        double diff = sortedEventTimes[i + 1] - sortedEventTimes[i];
        if (diff > eps && diff < delta)
            delta = diff;
    }

    double m = sortedEventTimes[0]; // earliest event time

    // convert each job to integer-scaled Job_dimensional
    std::vector<Job_dimensional> newJobs;
    for (auto& job : sortedJobs) {
        double scaledStart = (job.start - m) / delta;
        double scaledEnd   = (job.end   - m) / delta;

        int64_t iStart = static_cast<int64_t>(std::ceil(scaledStart));
        int64_t iEnd   = static_cast<int64_t>(std::ceil(scaledEnd));

        std::vector<int64_t> intSize;
        for (auto& s : job.size)
            intSize.push_back(static_cast<int64_t>(std::round(s * 1000)));
        newJobs.emplace_back(iStart, iEnd, intSize);
    }
    return newJobs;
}

std::vector<Job_dimensional> filterByMu(const std::vector<Job_dimensional>& jobs, int64_t mu) {
    // collect and sort all durations
    std::vector<int64_t> listDurations;
    for (const auto& job : jobs)
        listDurations.push_back(job.end - job.start);
    std::sort(listDurations.begin(), listDurations.end());

    // sliding window to find interval [d_lower, mu * d_lower] with most jobs
    int64_t maxJobCount = 0;
    std::pair<int64_t, int64_t> bestInterval = {0, 0};
    size_t n = listDurations.size();
    size_t i = 0, j = 0;

    while (i < n) {
        int64_t lowerBound = listDurations[i];
        int64_t upperBound = mu * lowerBound;
        while (j < n && listDurations[j] <= upperBound)
            j++;
        int64_t jobCount = j - i;
        if (jobCount > maxJobCount) {
            maxJobCount = jobCount;
            bestInterval = {lowerBound, upperBound};
        }
        i++;
    }

    // keep only jobs within the best interval
    std::vector<Job_dimensional> filteredJobs;
    for (const auto& job : jobs) {
        int64_t duration = job.end - job.start;
        if (duration >= bestInterval.first && duration <= bestInterval.second)
            filteredJobs.push_back(job);
    }

    // sort filtered jobs by start time
    std::sort(filteredJobs.begin(), filteredJobs.end(), compareJobs);

    return filteredJobs;
}

int64_t computeMu(const std::vector<Job_dimensional>& jobs) {
    std::vector<int64_t> durations;
    for (const auto& job : jobs)
        durations.push_back(job.end - job.start);
    
    std::sort(durations.begin(), durations.end());

    int64_t minDuration = durations.front();
    int64_t maxDuration = durations.back();
    double average = std::accumulate(durations.begin(), durations.end(), 0.0) / durations.size();
    int64_t p25 = durations[durations.size() * 0.25];
    int64_t median = durations[durations.size() * 0.5];
    int64_t p75 = durations[durations.size() * 0.75];

    std::cout << "Min Duration: " << minDuration << std::endl;
    std::cout << "Max Duration: " << maxDuration << std::endl;
    std::cout << "25th Percentile: " << p25 << std::endl;
    std::cout << "Median: " << median << std::endl;
    std::cout << "75th Percentile: " << p75 << std::endl;
    std::cout << "Average: " << average << std::endl;

    if (minDuration == 0) {
        std::cerr << "Min duration is zero, cannot compute mu." << std::endl;
        return 0;
    }

    int64_t mu = static_cast<int64_t>(std::ceil(static_cast<double>(maxDuration) / minDuration));
    std::cout << "mu: " << mu << std::endl;
    return mu;
}