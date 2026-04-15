#include "../include/base_algorithm.h"

void BaseAlgorithm::print_servers(const std::string& alg_name) {
    std::cout << "------------------" << alg_name << " STATUS BEGIN------------------" << std::endl;
    int64_t server_num = 1;
    for (auto it = all_servers.begin(); it != all_servers.end(); it++) {
        std::cout << "*** status of server " << server_num << " ***" << std::endl;
        server_num++;
        (*it).print_status();
    }
    std::cout << "------------------" << alg_name << " STATUS END------------------" << std::endl;
}

void BaseAlgorithm::save_server_durations_and_wasted_space_to_file(const std::string& alg_name, const std::string& output_dir) {
    std::string file_path = output_dir + alg_name + "_server_data.csv";
    std::ofstream outfile(file_path);
    if (!outfile) {
        std::cerr << "Error: Could not open file for writing!" << std::endl;
        return;
    }
    // Write CSV header
    outfile << "Server Number, Duration, Wasted Space (Dimension 1";
    for (size_t dim = 1; dim < capacity.size(); ++dim) {
        outfile << ", Dimension " << dim + 1;
    }
    outfile << ")\n";
    // Iterate over all servers and write their data
    for (size_t i = 0; i < all_servers.size(); ++i) {
        int64_t duration = all_servers[i].duration();
        outfile << i + 1 << ", " << duration;
        for (size_t dim = 0; dim < all_servers[i].wasted_space.size(); ++dim) {
            outfile << ", " << all_servers[i].wasted_space[dim];
        }
        outfile << "\n";
    }
    outfile.close();
    std::cout << "Server data written to " << file_path << std::endl;
}