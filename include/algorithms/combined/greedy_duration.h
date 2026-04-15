#pragma once
#include <vector>
#include <cstdint>
#include <iostream>
#include "../clairvoyant/greedy.h"
#include "../clairvoyant/duration.h"

class Greedy_Duration {
public:
    Greedy_dimensional greedyShort;  // Regular greedy for short jobs
    Duration durationLong;      // Hybrid algorithm for long jobs
    std::vector<int64_t> capacity;   // Server capacity
    int64_t threshold;               // Threshold for job duration
    long long total_span;

    int64_t greedy_count = 0;
    int64_t duration_count = 0;
    int64_t total_count = 0;

    // Constructor to initialize both greedyShort and hybridLong with the capacity and threshold
    Greedy_Duration(const std::vector<int64_t>& _capacity, int64_t _threshold, long long _total_span): capacity(_capacity), threshold(_threshold),total_span(_total_span), greedyShort(_capacity), durationLong(_capacity, _total_span, 100 ) {}

    // Add job to the appropriate algorithm based on job duration
    void add_job(Job_dimensional& j) {
        int64_t job_duration = j.end - j.start;
        total_count +=1;

        // Compare job duration with the threshold
        if (job_duration <= threshold) {
            greedyShort.add_job(j);  // Assign to short-duration jobs
            greedy_count++;
        } else {
            durationLong.addJob(j);    // Assign to long-duration jobs using HybridAlgorithm
            duration_count++;
        }
    }

    void print_job_count() const{
        std::cout << "Number of jobs processed by Greedy: " << greedy_count << std::endl;
        std::cout << "Number of jobs processed by Duration: " << duration_count << std::endl;
        std::cout << "total: " << total_count << std::endl;

    }

    // Calculate the total cost (duration) for both short and long-duration jobs
    long long cost() {
        return greedyShort.cost() + durationLong.getCost();
    }
};