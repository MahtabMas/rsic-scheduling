#pragma once
#include <vector>
#include <map>
#include <tuple>
#include <cstdint>
#include <iostream>
#include <cmath>
#include "../../server.h"
#include "../non_clairvoyant/first_fit.h"

class Duration {
public:  
    std::vector<int64_t> capacity;
    int64_t mu;
    int64_t intervals_duration;
    std::vector<std::tuple<int64_t, int64_t>> _intervals;
    std::map<std::tuple<int64_t, int64_t>, std::vector<Job_dimensional> > mapJobs;
    std::map<std::tuple<int64_t, int64_t>, FirstFit_dimensional*> mapFirstFits;

    Duration(const std::vector<int64_t>& _capacity, int64_t _mu, int64_t _intervals_duration): capacity(_capacity), mu(_mu), intervals_duration(_intervals_duration) {
    _intervals = divide_mu_into_intervals(mu, intervals_duration);
    }

    std::vector<std::tuple<int64_t, int64_t>> divide_mu_into_intervals(int64_t mu, int64_t interval_duration) {
        // Calculate the number of intervals
        long long num_intervals = ceil(log(mu) / log(interval_duration)) + 1;; 
        // Initialize a vector to store the intervals
        std::vector<std::tuple<int64_t, int64_t>> intervals;
        // Determine the boundaries of each interval
        for (int64_t i = 0; i <= num_intervals; ++i) {
            int64_t lower_boundary = pow(interval_duration, (i-1));
            int64_t upper_boundary = pow(interval_duration, i);
            intervals.push_back(std::make_tuple(lower_boundary, upper_boundary));
        }
        return intervals;
    }

    void addJob(Job_dimensional& j) {
        long long job_duration = j.end - j.start;
        // Calculate the interval for the job based on its finishing time
        for (const auto& interval : _intervals) {
            if (job_duration >= std::get<0>(interval) && job_duration < std::get<1>(interval)) {
                // Add the job to mapJobs with the interval as the key
                mapJobs[interval].push_back(j);
                // Check the map for the same interval in mapFirstFits
                if (mapFirstFits.count(interval) == 0) {
                    // If the interval does not exist in mapFirstFits, create a new FirstFit instance for it
                    mapFirstFits[interval] = new FirstFit_dimensional(capacity);
                }
                // Add the job to the corresponding FirstFit instance
                mapFirstFits[interval]->add_job(j);
                break; // Break the loop after adding to the correct interval.
            }
        }
    }

    long long getCost(){
        long long total_cost = 0;
        for (const auto& pair : mapFirstFits){
            if (pair.second) {
                long long cost = pair.second->cost();
                total_cost += cost;
            }
        }
        return total_cost;
    }

    void save_server_durations_and_wasted_space_to_file(const std::string& algorithm_name, const std::string& output_dir) {
        std::string file_path = output_dir + algorithm_name + "_server_data.csv";
        std::ofstream outfile(file_path);

        if (!outfile) {
            std::cerr << "Error: Could not open file for writing!" << std::endl;
            return;
        }
        // Write CSV header
        outfile << "Server Number, Duration, Wasted Space (Dimension 1";
        for (size_t dim = 1; dim < capacity.size(); ++dim) {
            outfile << ", Dimension " << dim + 1;
        }
        outfile << ")\n";

        int server_number = 1;
        // Write data for servers in each instance in `mapFirstFits`
        for (auto it = mapFirstFits.begin(); it != mapFirstFits.end(); ++it) {
            FirstFit_dimensional* firstFit_instance = it->second; // Access the instance directly
            for (const auto& server : firstFit_instance->all_servers) {
                int64_t duration = const_cast<Server_dimensional&>(server).duration();
                // Write server number and duration
                outfile << server_number++ << ", " << duration;
                // Write wasted space for each dimension
                for (size_t dim = 0; dim < server.wasted_space.size(); ++dim) {
                    outfile << ", " << server.wasted_space[dim];
                }
                outfile << "\n";
            }
        }
        outfile.close();
        std::cout << "All server data written to " << file_path << std::endl;
    }
};