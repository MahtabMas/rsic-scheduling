#pragma once
#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include "server.h"

class BaseAlgorithm {
public:
    std::vector<Server_dimensional> all_servers;
    std::vector<int64_t> capacity;

    BaseAlgorithm(const std::vector<int64_t>& _capacity) : capacity(_capacity) {}

    void print_servers(const std::string& alg_name); // prints all server statuses
    void save_server_durations_and_wasted_space_to_file(const std::string& alg_name, const std::string& output_dir); // saves server data to CSV
};