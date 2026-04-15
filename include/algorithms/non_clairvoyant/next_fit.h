#pragma once
#include <vector>
#include <cassert>
#include <cstdint>
#include "../../base_algorithm.h"



class NextFit_dimensional : public BaseAlgorithm {
public:
    int64_t cur_time;

    NextFit_dimensional(const std::vector<int64_t>& _capacity) : BaseAlgorithm(_capacity), cur_time(0) {}

    void add_job(Job_dimensional& j) {
        assert(j.start >= cur_time);
        cur_time = j.start;
        if (all_servers.empty()) {
            all_servers.push_back(Server_dimensional(j, capacity));
            for (size_t dimension = 0; dimension < j.size.size(); ++dimension) {
                int64_t job_volume = static_cast<int64_t>(j.end - j.start) * static_cast<int64_t>(j.size[dimension]);
                all_servers.back().wasted_space[dimension] = (1000 * (j.end - j.start)) - job_volume;
            }
        } else {
            if (!all_servers.rbegin()[0].add_job(j)) {
                all_servers.rbegin()[0].deactivate();
                all_servers.push_back(Server_dimensional(j, capacity));
                for (size_t dimension = 0; dimension < j.size.size(); ++dimension) {
                    int64_t job_volume = static_cast<int64_t>(j.end - j.start) * static_cast<int64_t>(j.size[dimension]);
                    all_servers.back().wasted_space[dimension] = (1000 * (j.end - j.start)) - job_volume;
                }
            }
        }
    }
 

    long long cost() {
        long long res = 0;
        for (decltype(all_servers.begin()) it = all_servers.begin(); it != all_servers.end(); it++) {
            res += (*it).duration();
        }
        return res;
    }

};