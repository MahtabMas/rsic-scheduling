#pragma once

#include <vector>
#include <map>
#include <cassert>
#include <iostream>
#include <algorithm>
#include <cstdint>
#include "job.h"

class Server_dimensional{
public:
    std::vector<int64_t> capacity;
    int64_t start_time;
    int64_t cur_time;
    int64_t finish_time;
    bool is_active;
    std::vector<int64_t> cur_size;
    std::vector<int64_t> wasted_space;
    std::map<int64_t, std::vector<int64_t>> cur_jobs;
    std::map<int64_t, int64_t> job_start_finish_times;

    // opens a new server with the first job assigned
    Server_dimensional(Job_dimensional& firstJob, std::vector<int64_t>& _capacity);

    // advance clock to to_time, returns false if server becomes inactive
    bool advance_time(int64_t to_time);

    // assign job j, returns false if not enough capacity
    bool add_job(Job_dimensional& j);

    // cost = last job departure - server open time
    int64_t duration(); 

    // free capacity per dimension at current time
    std::vector<int64_t> remaining_capacity(); 

    // close server, no more jobs will be assigned
    void deactivate(); 

    // latest departure time of all active jobs
    int64_t get_server_finishing_time(); 

     // calculates total area under the load curve at a given time
    int64_t area_at_time(int64_t current_time);

    // debug: print server status to console
    void print_status();   

};