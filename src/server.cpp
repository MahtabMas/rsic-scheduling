#include "../include/server.h"


Server_dimensional::Server_dimensional(Job_dimensional& firstJob, std::vector<int64_t>& _capacity){
    assert(firstJob.size.size() == _capacity.size());
    for (size_t i = 0; i < firstJob.size.size(); ++i){
        assert(firstJob.size[i] <= _capacity[i]);
    }
    capacity = _capacity;
    start_time = cur_time = firstJob.start;
    finish_time = -1;
    is_active = true;
    cur_size = std::vector<int64_t>(capacity.size(),0);
    wasted_space = std::vector<int64_t>(capacity.size(), 0);
    cur_jobs[firstJob.end] = firstJob.size;
    job_start_finish_times[firstJob.start] = firstJob.end;

    for (size_t i = 0; i < firstJob.size.size(); ++i){
        cur_size[i] = firstJob.size[i];
    }
}

bool Server_dimensional:: advance_time(int64_t to_time) {
    if (to_time >= cur_time) {
        cur_time = to_time;
        if (!is_active) {
            return false;
        } else
            {
            assert(cur_jobs.size() > 0);
            auto it = cur_jobs.begin();
            while (it != cur_jobs.end() && it->first < to_time) {
                const std::vector<int64_t>& jobSizeVector = it->second;
                // Update cur_size element-wise
                for (size_t i = 0; i < cur_size.size(); ++i) {
                    cur_size[i] -= jobSizeVector[i];
                }
                it++;
            }

            if (it == cur_jobs.end()) {
                deactivate();
            }

            // Erase elements from cur_jobs
            cur_jobs.erase(cur_jobs.begin(), it);
            return is_active;
        }
    }
    return false;
} 

bool Server_dimensional::add_job(Job_dimensional &j) {
    if (advance_time(j.start)) {
        bool hasEnoughCapacity = true;
        for (size_t i = 0; i < j.size.size(); ++i) {
            if (cur_size[i] + j.size[i] > capacity[i]) {
                hasEnoughCapacity = false;
                break;
            }
        }
        if (hasEnoughCapacity) {
            if (cur_jobs.find(j.end) != cur_jobs.end()) {
                // if there is already a job with the given end-time, add this job size to it
                for (size_t i = 0; i < j.size.size(); ++i) {
                    cur_jobs[j.end][i] += j.size[i];
            }
            } else {
                cur_jobs[j.end] = j.size;
            }
            job_start_finish_times[j.start] = j.end;// add start time finsih time mapping

            // Update cur_size element-wise
            for (size_t i = 0; i < cur_size.size(); ++i) {
                cur_size[i] += j.size[i];
            }
            return true;
        }
    }
    return false;
}

// If the server is not active, return finish time minus start time; Otherwise, return latest finish time of current jobs minus start time
int64_t Server_dimensional::duration() {
    if(!is_active) {
        return finish_time-start_time;
    } else {
        assert(cur_jobs.size()>0);
        return cur_jobs.rbegin()->first - start_time;
    }
} 

std::vector<int64_t> Server_dimensional::remaining_capacity(){
    std::vector<int64_t> remainingCapacity(capacity.size());
    for (size_t i = 0; i< capacity.size(); i++){
        remainingCapacity[i] = capacity[i] - cur_size[i];
    }
    return remainingCapacity;
}

void Server_dimensional::deactivate() {
    if(is_active) {
        if(!cur_jobs.empty()) {
            finish_time = cur_jobs.rbegin()->first;
        }

        for (size_t i=0; i< cur_size.size(); i++ ){
            cur_size[i]= 0;
        }
        cur_jobs.clear();
        is_active = false;
    }
}

int64_t Server_dimensional::get_server_finishing_time() {
    int64_t latest_finish_time = -1;
    if (is_active) {
        for (const auto& job : cur_jobs) {
            if (job.first > latest_finish_time) {
                latest_finish_time = job.first;
            }
        }
    } else {
        latest_finish_time = finish_time;
    }
    return latest_finish_time;
}

int64_t Server_dimensional::area_at_time(int64_t current_time){
    std::vector<int64_t> event;
    int64_t total_area = 0;
    int64_t prev_time = current_time;
    std::vector<int64_t> active_size = cur_size;

    for (const auto& job:cur_jobs){
        if (job.first > current_time){
            event.push_back(job.first);
        }
    }
    // Remove duplicate times and sort the event vector
    std::sort(event.begin(), event.end());
    event.erase(std::unique(event.begin(), event.end()), event.end());

    for (size_t i = 0; i < event.size(); ++i) {
    int64_t current_event_time = event[i];

    if (current_event_time > prev_time){
        int64_t time_interval = current_time - prev_time;
        int64_t vol = 0;
        for(size_t j= 0; j< active_size.size(); ++j){
            vol += active_size[j];
        }
        // Calculate the difference between capacity and current volume
        // int64_t capacity_difference = capacity[0] - vol;
        // total_area += time_interval * capacity_difference;
        total_area += time_interval * vol;
        prev_time = current_time;
    }
    auto it = cur_jobs.begin();
    while(it != cur_jobs.end() && it->first == current_event_time){
        for (size_t j = 0; j< it->second.size(); ++j){
            active_size[j] -= it->second[j]; 
        }
        it = cur_jobs.erase(it);
        }
    }
    return total_area;   
}

void Server_dimensional::print_status() {
    std::cout << "Capacity: ";
    for (size_t i = 0; i < capacity.size(); ++i) {
        std::cout << capacity[i] << " ";
    }
    std::cout << "start time: " << start_time << " current time: " << cur_time
            << " finish time: " << finish_time;

    std::cout << " current size: ";
    for (size_t i = 0; i < cur_size.size(); ++i) {
        std::cout << cur_size[i] << " ";
    }
    std::cout << " duration: " << duration() << std::endl;
    std::cout << "Is active: " << is_active << std::endl;
    std::cout << "Current jobs:" << std::endl;
    for (const auto& entry : cur_jobs) {
        std::cout << "\tJob ending at " << entry.first << " of size: ";
        for (size_t i = 0; i < entry.second.size(); ++i) {
            std::cout << entry.second[i] << " ";
        }
        std::cout << std::endl;
    }
}


 