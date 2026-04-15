#pragma once
#include <vector>
#include <cassert>
#include <cstdint>
#include <set>
#include "../../base_algorithm.h"


class FirstFit_dimensional : public BaseAlgorithm {
public:
    std::vector<int64_t> active_servers_idx;
    int64_t cur_time;
    std::set<int64_t> events;

    FirstFit_dimensional(const std::vector<int64_t>& _capacity): BaseAlgorithm(_capacity), cur_time(0) {}

    void add_job(Job_dimensional &j) {
        assert(j.start >= cur_time); // jobs should be added by non-decreasing start times
        cur_time = j.start;
        events.insert(j.start);
        events.insert(j.end);

        bool placed = false;
        auto it = active_servers_idx.begin();
        while(it != active_servers_idx.end() && !placed) {
            // Try placing job in the current server
            int64_t finishing_time = all_servers[*it].start_time + all_servers[*it].duration(); // Server's finish time
            placed = all_servers[*it].add_job(j);
            if (placed) {
                int64_t delta_t = std::max<int64_t>(0, static_cast<int64_t>(j.end - finishing_time));
                // Update wasted space dimensionally
                for (size_t dimension = 0; dimension < j.size.size(); ++dimension) {
                    int64_t job_volume = static_cast<int64_t>(j.end - j.start) * static_cast<int64_t>(j.size[dimension]);
                    all_servers[*it].wasted_space[dimension] += (static_cast<int64_t>(1000) * delta_t) - job_volume;
                }
            } else {
                // Check if the server is still active
                if (all_servers[*it].is_active) {
                    it++;
                } else {
                    it = active_servers_idx.erase(it); // Remove inactive servers
                }
            }
        }
        // If not placed, create a new server
        if (!placed) {
            // Create a new server and place the job
            all_servers.push_back(Server_dimensional(j, capacity));
            active_servers_idx.push_back(static_cast<int64_t>(all_servers.size()) - 1);
            // Update wasted space for the new server
            for (size_t dimension = 0; dimension < j.size.size(); ++dimension) {
                int64_t job_volume = static_cast<int64_t>(j.end - j.start) * static_cast<int64_t>(j.size[dimension]);
                // Wasted space calculation for a new server: the remaining capacity after placing the job
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

    void add_job_active(Job_dimensional &j){
        assert(j.start >= cur_time); // jobs should be added by non-decreasing start times
        cur_time = j.start;
        bool hasActiveServer = false;
        for (int64_t idx : active_servers_idx){
            if (all_servers[idx].is_active){
                hasActiveServer = true;
                break; // No need to continue checking once we find an active server
            }
        }
        if (hasActiveServer){
            add_job(j);
        }
    }

};

