#pragma once
#include <vector>
#include <cstdint>
#include <iostream>
#include "../clairvoyant/greedy.h"

class Greedy_Greedy {
public:
    Greedy_dimensional greedyShort, greedyLong;  // Two separate greedy instances
    std::vector<int64_t> capacity;               // Server capacity
    int64_t threshold;                           // Threshold for job duration
    int64_t short_count = 0;
    int64_t long_count = 0;
    int64_t total_count = 0;

    // Constructor to initialize both greedy instances and the duration threshold
    Greedy_Greedy(const std::vector<int64_t>& _capacity, int64_t _threshold): capacity(_capacity), threshold(_threshold), greedyShort(_capacity), greedyLong(_capacity) {}

    // Add job to the appropriate greedy instance based on job duration
    void add_job(Job_dimensional& j) {
        int64_t job_duration = j.end - j.start;
        total_count++;
        // Compare job duration with the threshold
        if (job_duration <= threshold) {
            greedyShort.add_job(j);  // Assign to short-duration jobs
            short_count++;
        } else {
            greedyLong.add_job(j);   // Assign to long-duration jobs
            long_count++;
        }
    }

    void print_job_count() const{
        std::cout << "Number of jobs processed by Greedy Short: " << short_count << std::endl;
        std::cout << "Number of jobs processed by Greedy Long: " << long_count << std::endl;
        std::cout << "total: " << total_count << std::endl;
    }

    // Print the servers for both short and long-duration jobs
    void print_servers() {
        std::cout << "************** SHORT DURATION JOB SERVERS ******************" << std::endl;
        greedyShort.print_servers("Greedy_Short");
        std::cout << "************** LONG DURATION JOB SERVERS ******************" << std::endl;
        greedyLong.print_servers("Greedy_Long");
    }
    // Calculate the total cost (duration) for both short and long-duration jobs
    long long cost() {
        return greedyShort.cost() + greedyLong.cost();
    }
};