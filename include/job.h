#pragma once
#include <vector>
#include <cstdint>
#include <iostream>




// Raw job from Azure CSV: doubles, with priority (0=high, 1=low)
class RawJob_dimensional{
public:
    double start;
    double end;
    std::vector<double> size;
    int priority;

    RawJob_dimensional(double _start, double _end,
                      const std::vector<double>& _size, int _priority)
                      : start(_start), end(_end), size(_size), priority(_priority){}
};

// Integer-scaled job for algorithms: no priority
class Job_dimensional{
public:
    int64_t start;
    int64_t end;
    std::vector<int64_t> size;

    Job_dimensional(int64_t _start, int64_t _end, 
                    const std::vector<int64_t>& _size)
                    : start(_start), end(_end), size(_size){}

};

inline bool compareJobs(const Job_dimensional& a, const Job_dimensional& b){
    return a.start < b.start;
}

inline void print_jobs_dimensional(const std::vector<Job_dimensional>& jobs) {
    for (int i = 0; i < jobs.size(); i++) {
        std::cout << "Job #" << (i + 1) << " start: " << jobs[i].start << " end: " << jobs[i].end << " size: ";
        // Print each element of the size vector
        for (int j = 0; j < jobs[i].size.size(); j++) {
            std::cout << jobs[i].size[j];
            if (j < jobs[i].size.size() - 1) {
                 // Print comma between elements
                std::cout << ", "; 
            }
        }
        std::cout << std::endl;
    }
}