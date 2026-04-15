#pragma once
#include <vector>
#include <cassert>
#include <cstdint>
#include "../../base_algorithm.h"


class LastFit_dimensional : public BaseAlgorithm {
public:
    std::vector<int64_t> active_servers_idx;
    int64_t cur_time;

    LastFit_dimensional(const std::vector<int64_t>& _capacity): BaseAlgorithm(_capacity), cur_time(0) {}

    void add_job(Job_dimensional &j) {
        assert(j.start >= cur_time); // jobs should be added by non-decreasing start times
        cur_time = j.start;

        bool placed = false;
        auto it = active_servers_idx.rbegin();
        while (it != active_servers_idx.rend() && !placed) {
            // Try placing job in the current server
            int64_t finishing_time = all_servers[*it].start_time + all_servers[*it].duration();
            placed = all_servers[*it].add_job(j);

            if (placed) {
                // Calculate and update wasted space for the assigned server
                
                int64_t delta_t = std::max<int64_t>(0, static_cast<int64_t>(j.end - finishing_time));

                for (size_t dimension = 0; dimension < j.size.size(); ++dimension) {
                    int64_t job_volume = static_cast<int64_t>(j.end - j.start) * static_cast<int64_t>(j.size[dimension]);
                    int64_t new_wasted_space = (static_cast<int64_t>(1000) * delta_t) - job_volume;
                    all_servers[*it].wasted_space[dimension] += new_wasted_space;
                }
            } else {
                // Check if the server is still active; if not, erase it from active_servers_idx
                if (all_servers[*it].is_active) {
                    ++it;
                } else {
                    it = decltype(active_servers_idx.rbegin())(active_servers_idx.erase(std::next(it).base()));
                }
            }
        }

        // If not placed, create a new server
        if (!placed) {
            all_servers.push_back(Server_dimensional(j, capacity));
            active_servers_idx.push_back(static_cast<int64_t>(all_servers.size()) - 1);

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