#pragma once
#include <vector>
#include <cassert>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <string>
#include "../../base_algorithm.h"

class BestFit_dimensional : public BaseAlgorithm {
public:
    std::vector<int64_t> active_servers_idx;
    int64_t cur_time;
    BestFit_dimensional(const std::vector<int64_t>& _capacity) : BaseAlgorithm(_capacity), cur_time(0) {}

    void add_job(Job_dimensional& j) {
        assert(j.start >= cur_time); // Jobs should be added by non-decreasing start times
        // Advance time for BF and all its servers and update active_server_idx
        cur_time = j.start;
        auto it = active_servers_idx.begin();
        while (it != active_servers_idx.end()) {
            if (!all_servers[*it].advance_time(cur_time)){
                it = active_servers_idx.erase(it);
                continue;
            } else {
                ++it;
            }
        }
        // Find a server (if it exists) that can accommodate the job and has the best fit
        int64_t min_remain_capacity_idx = -1;
        // Find the server with the minimum remaining capacity
        it = active_servers_idx.begin();
        while (it != active_servers_idx.end()) {
            bool enoughCapacity = true;
            std::vector<int64_t> remaining_capacity = all_servers[*it].remaining_capacity();
            for (size_t i = 0; i < j.size.size(); ++i) {
                if (remaining_capacity[i] < j.size[i]) {
                    enoughCapacity = false;
                    break;
                }
            }
            if (enoughCapacity) {
                if (min_remain_capacity_idx == -1 || remaining_capacity < all_servers[min_remain_capacity_idx].remaining_capacity()) {
                    min_remain_capacity_idx = *it;
                }
            }
            ++it;
        }

        if (min_remain_capacity_idx != -1) {
            int64_t finishing_time = all_servers[min_remain_capacity_idx].start_time + all_servers[min_remain_capacity_idx].duration();
            all_servers[min_remain_capacity_idx].add_job(j);
            // Calculate wasted space for the placed job
            int64_t delta_t = std::max<int64_t>(0, static_cast<int64_t>(j.end - finishing_time));
            // Update wasted space dimensionally
            for (size_t dimension = 0; dimension < j.size.size(); ++dimension) {
                int64_t job_volume = static_cast<int64_t>(j.end - j.start) * static_cast<int64_t>(j.size[dimension]);
                int64_t new_wasted_space = (static_cast<int64_t>(1000) * delta_t)  - job_volume;
                all_servers[min_remain_capacity_idx].wasted_space[dimension] += new_wasted_space;
            }
        } else {
            // If no active server has enough capacity, create a new server
            Server_dimensional new_server(j, capacity);
            all_servers.push_back(new_server);
            int64_t newIndex = static_cast<int64_t>(all_servers.size()) - 1;
            active_servers_idx.push_back(newIndex);
            // Calculate wasted space for the new server
            for (size_t dimension = 0; dimension < j.size.size(); ++dimension) {
                int64_t job_volume = static_cast<int64_t>(j.end - j.start) * static_cast<int64_t>(j.size[dimension]);
                all_servers.back().wasted_space[dimension] = (1000 * (j.end - j.start)) - job_volume;
            }
        }
    }


    long long cost() {
        long long res = 0;
        for(decltype(all_servers.begin()) it = all_servers.begin(); it != all_servers.end(); it++) {
            res += (*it).duration();
        }
        return res;
    }
};