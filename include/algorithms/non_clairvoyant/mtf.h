#pragma once
#include <vector>
#include <cassert>
#include <cstdint>
#include <algorithm>
#include "../../base_algorithm.h"

class MTF_dimensional : public BaseAlgorithm {
public:
    std::vector<int64_t> active_servers_idx;
    int64_t cur_time;

    MTF_dimensional(const std::vector<int64_t>& _capacity):  BaseAlgorithm(_capacity), cur_time(0) {}

    void add_job(Job_dimensional& j) {
        assert(j.start >= cur_time); // jobs should be added by non-decreasing start times
        cur_time = j.start;

        bool placed = false;
        auto it = active_servers_idx.begin();
        while (it != active_servers_idx.end() && !placed) {
            int64_t serverIdx = *it;
            if (all_servers[serverIdx].is_active) {
                int64_t finishing_time = all_servers[serverIdx].start_time + all_servers[serverIdx].duration(); // Server's finish time
                placed = all_servers[serverIdx].add_job(j);
                if (placed) {
                    // Move the assigned server to the front of the list
                    active_servers_idx.erase(it);
                    active_servers_idx.insert(active_servers_idx.begin(), serverIdx);

                    // Calculate and update wasted space for the placed job
                    int64_t delta_t = std::max<int64_t>(0, static_cast<int64_t>(j.end - finishing_time));

                    // Update wasted space dimensionally
                    for (size_t dimension = 0; dimension < j.size.size(); ++dimension) {
                        int64_t job_volume = static_cast<int64_t>(j.end - j.start) * static_cast<int64_t>(j.size[dimension]);
                        int64_t new_wasted_space = (static_cast<int64_t>(1000) * delta_t)  - job_volume; 
                        all_servers[serverIdx].wasted_space[dimension] += new_wasted_space;
                    }
                }
            }
            if (!placed) {
                if (!all_servers[serverIdx].is_active) {
                    it = active_servers_idx.erase(it);
                } else {
                    ++it;
                }
            }
        }

        if (!placed) {
            // Create a new server
            all_servers.push_back(Server_dimensional(j, capacity));
            int64_t newIndex = static_cast<int64_t>(all_servers.size()) - 1;
            active_servers_idx.insert(active_servers_idx.begin(), newIndex);

            // Calculate and update wasted space for the new server
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