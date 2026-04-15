#pragma once
#include "next_fit.h"

class ModifiedNextFit_dimensional {
public:
    NextFit_dimensional nfSmall, nfLarge;
    std::vector<int64_t> capacity;
    int64_t threshold; // small jobs are those that are <= threshold

    ModifiedNextFit_dimensional(const std::vector<int64_t>& _capacity, int64_t _threshold): capacity(_capacity), threshold(_threshold), nfSmall(_capacity), nfLarge(_capacity) {}

    void add_job(Job_dimensional &j) {
        bool isSmallJob = true;
        for (size_t i = 0; i < j.size.size(); ++i) {
            if (j.size[i] >= threshold) {
                isSmallJob = false;
                break;
            }
        }

        if (isSmallJob) {
            nfSmall.add_job(j);
        } else {
            nfLarge.add_job(j);
        }
    }

    void print_servers() {
        std::cout << "************** SMALL JOB SERVERS ******************"<<std::endl;
        nfSmall.print_servers("NextFit_Small");
        std::cout << "************** LARGE JOB SERVERS ******************"<<std::endl;
        nfLarge.print_servers("NextFit_Large");
    }

    long long cost() {
        return nfSmall.cost() + nfLarge.cost();
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
        for (size_t i = 0; i < nfSmall.all_servers.size(); ++i) {
            int64_t duration = nfSmall.all_servers[i].duration();
            outfile << i + 1 << ", " << duration;

            // Write wasted space for each dimension
            for (size_t dim = 0; dim < nfSmall.all_servers[i].wasted_space.size(); ++dim) {
                outfile << ", " << nfSmall.all_servers[i].wasted_space[dim];
            }
            outfile << "\n";
        }

        for (size_t i = 0; i < nfLarge.all_servers.size(); ++i) {
            int64_t duration = nfLarge.all_servers[i].duration();
            outfile << nfSmall.all_servers.size() + i + 1 << ", " << duration; // Continue numbering

            // Write wasted space for each dimension
            for (size_t dim = 0; dim < nfLarge.all_servers[i].wasted_space.size(); ++dim) {
                outfile << ", " << nfLarge.all_servers[i].wasted_space[dim];
            }
            outfile << "\n";
        }

        outfile.close();
        std::cout << "Server data written to " << file_path << std::endl;
    }
};
