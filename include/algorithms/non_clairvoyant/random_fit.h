#pragma once
#include <vector>
#include <cassert>
#include <cstdint>
#include <random>
#include <algorithm>
#include "../../base_algorithm.h"


class RandomFit_dimensional : public BaseAlgorithm {
public:
    std::vector<int64_t> active_servers_idx;
    int64_t cur_time;
    std::random_device rd;
    std::mt19937 gen;

    RandomFit_dimensional(const std::vector<int64_t>& _capacity) : BaseAlgorithm(_capacity), cur_time(0), gen(rd()) {}

    void add_job(Job_dimensional& j) {
        assert(j.start >= cur_time); // Jobs should be added by non-decreasing start times
        // Advance time for RF and all its servers and update active_server_idx
        cur_time = j.start;
        // Update the time for active servers and remove inactive servers
        auto it = active_servers_idx.begin();
        while (it != active_servers_idx.end()) {
            if (!all_servers[*it].advance_time(cur_time)) {
                it = active_servers_idx.erase(it);
            } else {
                ++it;
            }
        }
        // Randomly select an active server that can accommodate the job (if any) or create a new server
        std::vector<int64_t> shuffled_servers = active_servers_idx;
        std::shuffle(shuffled_servers.begin(), shuffled_servers.end(), gen);
    
        bool job_assigned = false;
        for (int64_t server_index : shuffled_servers) {
            bool enoughCapacity = true;
            std::vector<int64_t> remaining_capacity = all_servers[server_index].remaining_capacity();

            for (size_t i = 0; i < j.size.size(); ++i) {
                if (remaining_capacity[i] < j.size[i]) {
                    enoughCapacity = false;
                    break;
                }
            }
            if (enoughCapacity) {
                Server_dimensional* random_server = &all_servers[server_index];
                int64_t finishing_time = random_server->start_time + random_server->duration();
                random_server->add_job(j);
                job_assigned = true;
                // Calculate and update wasted space for the assigned job
                int64_t delta_t = std::max<int64_t>(0, static_cast<int64_t>(j.end - finishing_time));
                // Update wasted space dimensionally
                for (size_t dimension = 0; dimension < j.size.size(); ++dimension) {
                    int64_t job_volume = static_cast<int64_t>(j.end - j.start) * static_cast<int64_t>(j.size[dimension]);
                    int64_t new_wasted_space = (static_cast<int64_t>(1000) * delta_t) - job_volume; 
                    random_server->wasted_space[dimension] += new_wasted_space;
                }
                break;
            }
        }

        if (!job_assigned) {
            // If no active server can accommodate the job, open a new server
            Server_dimensional new_server(j, capacity);
            all_servers.push_back(new_server);
            active_servers_idx.push_back(all_servers.size() - 1); // Add the new server to active_servers_idx
            // Calculate wasted space for the new server
            for (size_t dimension = 0; dimension < j.size.size(); ++dimension) {
                int64_t job_volume = static_cast<int64_t>(j.end - j.start) * static_cast<int64_t>(j.size[dimension]);
                all_servers.back().wasted_space[dimension] = (1000 * (j.end - j.start)) - job_volume;
            }
        }
    }

    long long cost() {
        long long res = 0;
        for (auto it = all_servers.begin(); it != all_servers.end(); it++) {
            res += (*it).duration();
        }
        return res;
    }
};