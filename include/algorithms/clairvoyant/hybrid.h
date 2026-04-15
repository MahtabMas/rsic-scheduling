#pragma once
#include <vector>
#include <map>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <string>
#include <fstream>
#include "../../server.h"
#include "../non_clairvoyant/first_fit.h"


class HybridAlgorithm {
    public:
        std::map<long long, std::vector<Job_dimensional> > mapJobs;
        std::map<long long, FirstFit_dimensional *> mapFirstFits;
        FirstFit_dimensional *firstFit;
        std::vector<int64_t> capacity;
        long totalSizeActiveJobs; // d

        HybridAlgorithm(std::vector<int64_t> _capacity){
            firstFit = new FirstFit_dimensional(_capacity);
            capacity = _capacity;
        }

        std::pair<long long, long long> calculateParameters(const Job_dimensional &job){
            long long i = static_cast<int64_t>(floor(log2(job.end - job.start))) + 1;
            long long c = static_cast<long long>(floor(job.start / pow(2, i - 1)));
            return std::make_pair(i, c);
        }

        long long cantor_encode(long long i, long long c){
            long long type_name;
            if (i >= 0 && c >= 0){
                type_name = (i + c) * (i + c + 1) / 2 + c;
            }
            else{
                std::cout << "ERROR";
            }
            return type_name;
        }

        std::pair<int64_t, int64_t> cantor_decode(long long type_name){
            int64_t w, t;
            long long i, c;
            w = floor(((sqrt((type_name * 8) + 1)) - 1) / 2);
            t = (w * (w + 1)) / 2;
            c = type_name - t;
            i = w - c;
            return std::make_pair(i, c);
        }

        void addJob(Job_dimensional &j){
            std::pair<long long, long long> parameters = calculateParameters(j);
            long long _i = parameters.first;
            long long _c = parameters.second;
            long long typeName = cantor_encode(_i, _c);
            
            // calculate d
            // iterate over the j.size
            for (size_t i = 0; i < j.size.size(); ++i){
                totalSizeActiveJobs += j.size[i];
                totalSizeActiveJobs = 0;
                if (mapJobs.size() == 0){
                    totalSizeActiveJobs = j.size[i];
                }
                else{
                    if (mapJobs.find(typeName) != mapJobs.end()){
                        for (auto &job_arrived : mapJobs.at(typeName)){
                            if (j.start >= job_arrived.start && j.start <= job_arrived.end){
                                totalSizeActiveJobs += job_arrived.size[i];
                            }
                        }
                    }
                    else{
                        totalSizeActiveJobs = j.size[i];
                    }
                }
            }

            // check the map for the same type_name
            if (mapFirstFits.size() != 0 && mapFirstFits.count(typeName) > 0){
                // existent
                mapFirstFits.at(typeName)->add_job_active(j);
            }
            else if (totalSizeActiveJobs <= 1 / (2 * sqrt(_i))){
                // general
                firstFit->add_job(j);
            }
            else{
                // new
                mapFirstFits.insert(std::make_pair(typeName, new FirstFit_dimensional(capacity)));
                mapFirstFits.at(typeName)->add_job(j);
            }
        }

        long long getCost(){
            long long total_cost = firstFit->cost();
            for (auto &firstFit : mapFirstFits){
                long long cost = firstFit.second->cost();
                total_cost += cost;
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
        for (size_t dim = 1; dim < firstFit->capacity.size(); ++dim) {
            outfile << ", Dimension " << dim + 1;
        }
        outfile << ")\n";

        int server_number = 1;
        // Write data for servers in `firstFit`
        for (const auto& server : firstFit->all_servers) {
            int64_t duration = const_cast<Server_dimensional&>(server).duration();

            // Write server number and duration
            outfile << server_number++ << ", " << duration;

            // Write wasted space for each dimension
            for (size_t dim = 0; dim < server.wasted_space.size(); ++dim) {
                outfile << ", " << server.wasted_space[dim];
            }
            outfile << "\n";
        }

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