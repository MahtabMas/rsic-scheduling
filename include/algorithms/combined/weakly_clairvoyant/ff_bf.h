#pragma once
#include <vector>
#include <cstdint>
#include <iostream>
#include "../../non_clairvoyant/first_fit.h"
#include "../../non_clairvoyant/best_fit.h"

class FirstFit_BestFit {
public:
    FirstFit_dimensional FFShort;  // One FirstFit instances
    BestFit_dimensional FFLong;  // One BestFit instances
    std::vector<int64_t> capacity;               // Server capacity
    int64_t threshold;                           // Threshold for job duration
    int64_t short_count = 0;
    int64_t long_count = 0;
    int64_t total_count = 0;

    FirstFit_BestFit(const std::vector<int64_t>& _capacity, int64_t _threshold): capacity(_capacity), threshold(_threshold), FFShort(_capacity), FFLong(_capacity) {}

    void add_job(Job_dimensional& j) {
        int64_t job_duration = j.end - j.start;
        total_count++;
        
        // Compare job duration with the threshold
        if (job_duration <= threshold) {
            FFShort.add_job(j);  // Assign to short-duration jobs
            short_count++;
        } else {
            FFLong.add_job(j);   // Assign to long-duration jobs
            long_count++;
        }
    }

    void print_job_count() const{
        std::cout << "Number of jobs processed by FirstFit Short: " << short_count << std::endl;
        std::cout << "Number of jobs processed by BestFit Long: " << long_count << std::endl;
        std::cout << "total: " << total_count << std::endl;
    }

    // Print the servers for both short and long-duration jobs
    void print_servers() {
        std::cout << "************** SHORT DURATION JOB SERVERS ******************" << std::endl;
        FFShort.print_servers("FirstFit_Short");
        std::cout << "************** LONG DURATION JOB SERVERS ******************" << std::endl;
        FFLong.print_servers("BestFit_Long");
    }

    // Calculate the total cost (duration) for both short and long-duration jobs
    long long cost() {
        return FFShort.cost() + FFLong.cost();
    }
};