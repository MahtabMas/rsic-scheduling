#pragma once
#include "first_fit.h"

class ModifiedFirstFit_dimensional {
public:
    FirstFit_dimensional ffSmall, ffLarge;
    std::vector<int64_t> capacity;
    int64_t threshold; // small jobs are those that are <= threshold

    ModifiedFirstFit_dimensional(const std::vector<int64_t>& _capacity, int64_t _threshold): capacity(_capacity), threshold(_threshold), ffSmall(_capacity), ffLarge(_capacity) {}
    
    void add_job(Job_dimensional &j) {
        bool isSmallJob = true;
        for (size_t i = 0; i < j.size.size(); ++i) {
            if (j.size[i] >= threshold) {
                isSmallJob = false;
                break;
            }
        }
        if (isSmallJob) {
            ffSmall.add_job(j);
        } else {
            ffLarge.add_job(j);
        }
    }

    void print_servers() {
        std::cout << "************** SMALL JOB SERVERS ******************"<<std::endl;
        ffSmall.print_servers("FirstFit_Small");
        std::cout << "************** LARGE JOB SERVERS ******************"<<std::endl;
        ffLarge.print_servers("FirstFit_Large");
    }

    long long cost() {
        return ffSmall.cost() + ffLarge.cost();
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
        
        // Write data for all servers, combining small and large
        for (size_t i = 0; i < ffSmall.all_servers.size(); ++i) {
            int64_t duration = ffSmall.all_servers[i].duration();
            outfile << i + 1 << ", " << duration;

            // Write wasted space for each dimension
            for (size_t dim = 0; dim < ffSmall.all_servers[i].wasted_space.size(); ++dim) {
                outfile << ", " << ffSmall.all_servers[i].wasted_space[dim];
            }
            outfile << "\n";
        }

        for (size_t i = 0; i < ffLarge.all_servers.size(); ++i) {
            int64_t duration = ffLarge.all_servers[i].duration();
            outfile << ffSmall.all_servers.size() + i + 1 << ", " << duration; // Continue numbering
            // Write wasted space for each dimension
            for (size_t dim = 0; dim < ffLarge.all_servers[i].wasted_space.size(); ++dim) {
                outfile << ", " << ffLarge.all_servers[i].wasted_space[dim];
            }
            outfile << "\n";
        }

        outfile.close();
        std::cout << "Server data written to " << file_path << std::endl;
    }
};