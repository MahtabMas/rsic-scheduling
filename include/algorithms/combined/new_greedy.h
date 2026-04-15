#pragma once
#include <vector>
#include <cassert>
#include <cstdint>
#include <limits>
#include "../../base_algorithm.h"

class NewGreedy : public BaseAlgorithm{
public:
    std::vector<int64_t> active_servers_idx;
    int64_t cur_time;
    int64_t threshold;  

    NewGreedy(const std::vector<int64_t>& _capacity, int64_t _threshold) : BaseAlgorithm(_capacity) , cur_time(0), threshold(_threshold) {}

    void add_job(Job_dimensional& j) {
        assert(j.start >= cur_time);
        cur_time = j.start;

        Server_dimensional* best_server = nullptr;
        int64_t best_extension = std::numeric_limits<int64_t>::max();

        // Remove inactive servers
        auto it = active_servers_idx.begin();
        while (it != active_servers_idx.end()) {
            if (!all_servers[*it].advance_time(cur_time)) {
                it = active_servers_idx.erase(it);
            } else {
                ++it;
            }
        }

        // Find the best server for the job
        for (const int64_t idx : active_servers_idx) {
            int64_t extension;
            if (all_servers[idx].get_server_finishing_time() < j.end) {
                // The job finishes after the server's current finishing time
                extension = j.end - all_servers[idx].get_server_finishing_time();
            } else {
                // The job finishes before the server's current finishing time
                extension = 0;
            }

            if (extension < best_extension) {
                bool enoughCapacity = true;
                for (size_t i = 0; i < j.size.size(); ++i) {
                    if (all_servers[idx].remaining_capacity()[i] < j.size[i]) {
                        enoughCapacity = false;
                        break;
                    }
                }
                if (enoughCapacity) {
                    best_extension = extension;
                    best_server = &all_servers[idx];
                }
            }
        }

        // Check if the best server's extension exceeds the threshold
        if (best_server && best_extension <= threshold) {
            best_server->add_job(j);
        } else {
            // If no suitable server or extension exceeds the threshold, create a new server
            Server_dimensional new_server(j, capacity);
            all_servers.push_back(new_server);
            active_servers_idx.push_back(static_cast<int64_t>(all_servers.size()) - 1);
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
