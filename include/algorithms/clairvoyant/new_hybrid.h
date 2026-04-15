#pragma once
#include <vector>
#include <cstdint>
#include <algorithm>
#include <fstream>
#include "../../server.h"
#include "hybrid.h"

class NewHybrid{
public:
    std::vector<int64_t> capacity;
    std::vector<HybridAlgorithm> HA_list;

    NewHybrid(std::vector<int64_t> _capacity): capacity(_capacity){
        for (int64_t i = 0; i < capacity.size() ; i++){
            HA_list.emplace_back(capacity);
        }
    }

    void addJob(Job_dimensional& j){
        // maximum element in size of job j
        auto max_size = std::max_element(j.size.begin(), j.size.end());
        //find the index of the max element that appears first
        if (max_size != j.size.end()) {
            size_t max_index = std::distance(j.size.begin(), max_size);
            HA_list[max_index].addJob(j);
        }
    }

    long long getCost(){
        long long cost = 0;
        for (auto& hybridAlgorithm : HA_list){
            cost +=  hybridAlgorithm.getCost();
        }
        return cost;
    }

    void save_server_durations_and_wasted_space_to_file(const std::string& algorithm_name, const std::string& output_dir) {
        std::string file_path = output_dir + algorithm_name + "_server_data.csv";
        std::ofstream outfile(file_path);
        // Check if file opened successfully
        if (!outfile) {
            std::cerr << "Error: Could not open file " << file_path << " for writing!" << std::endl;
            return;
        }
        std::cout << "File opened successfully: " << file_path << std::endl;

        // Write CSV header once
        outfile << "Server Number, Duration, Wasted Space (Dimension 1";
        for (size_t dim = 1; dim < capacity.size(); ++dim) {
            outfile << ", Dimension " << dim + 1;
        }
        outfile << "\n";

        // Loop through each HybridAlgorithm in HA_list and write data
        for (auto& hybridAlgorithm : HA_list) {
            // Here, we are passing the file path to HybridAlgorithm
            hybridAlgorithm.save_server_durations_and_wasted_space_to_file(algorithm_name, output_dir);
        }
        // Close the file after all HybridAlgorithm instances have written to it
        outfile.close();
        std::cout << "All server data written to " << file_path << std::endl;
    }
};