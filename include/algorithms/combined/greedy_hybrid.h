#pragma once
#include <vector>
#include <cstdint>
#include <iostream>
#include "../clairvoyant/greedy.h"
#include "../clairvoyant/hybrid.h"

class Greedy_dimensional_with_Hybrid {
public:
    Greedy_dimensional greedyShort;  // Regular greedy for short jobs
    HybridAlgorithm hybridLong;      // Hybrid algorithm for long jobs
    std::vector<int64_t> capacity;   // Server capacity
    int64_t threshold;               // Threshold for job duration

    int64_t greedy_count = 0;
    int64_t hybrid_count = 0;
    int64_t total_count = 0;
    // Constructor to initialize both greedyShort and hybridLong with the capacity and threshold
    Greedy_dimensional_with_Hybrid(const std::vector<int64_t>& _capacity, int64_t _threshold): capacity(_capacity), threshold(_threshold), greedyShort(_capacity), hybridLong(_capacity) {}

    // Add job to the appropriate algorithm based on job duration
    void add_job(Job_dimensional& j){
        int64_t job_duration = j.end - j.start;
        total_count++;
        // Compare job duration with the threshold
        if (job_duration <= threshold){
            greedyShort.add_job(j);  // Assign to short-duration jobs
            greedy_count++;
        } else{
            hybridLong.addJob(j);    // Assign to long-duration jobs using HybridAlgorithm
            hybrid_count++; 
        }
    }

    void print_job_count() const{
        std::cout << "Number of jobs processed by Greedy: " << greedy_count << std::endl;
        std::cout << "Number of jobs processed by Departure: " << hybrid_count << std::endl;
        std::cout << "total: " << total_count << std::endl;
    }

    // Calculate the total cost (duration) for both short and long-duration jobs
    long long cost() {
        return greedyShort.cost() + hybridLong.getCost();
    }
};