#include "../include/experiment.h"
#include <iostream>

void get_ratio_Azure(const std::vector<std::vector<int64_t>>& Es, int64_t mu,
                     const std::vector<std::string>& algs_array, std::vector<Job_dimensional>& jobs,
                     const std::string& output_dir,
                     int64_t threshold_clairvoyant,
                     int64_t threshold_greedy_greedy,
                     int64_t threshold_greedy_duration,
                     int64_t threshold_new_greedy,
                     int64_t threshold_weakly_clairvoyant,
                     int64_t interval_departure,
                     int64_t interval_duration) {

    int64_t nEs = Es.size();
    for (int k = 0; k < nEs; k++) {
        std::vector<int64_t> E = Es[k];
        for (const std::string& className : algs_array) {
            double rho = 0.0;

            // Calculate the cost of OPT (Ceiling of Volume)
            Opt_dimensional opt(jobs);
            std::pair<long long, long long> result = opt.ceilLBandSpan(E);
            long long cost_opt = result.first;
            long long total_span = result.second;
            
            if (className == "NextFit_dimensional") {
                NextFit_dimensional ff(E);
                for (auto& job : jobs) ff.add_job(job);
                rho = (double) ff.cost() / ((long long) cost_opt);
                ff.save_server_durations_and_wasted_space_to_file("NextFit_dimensional", output_dir);
            } else if (className == "ModifiedNextFit_dimensional") {
                ModifiedNextFit_dimensional ff(E, (1000 + mu) / (mu + 1));
                for (auto& job : jobs) ff.add_job(job);
                rho = (double) ff.cost() / ((long long) cost_opt);
                ff.save_server_durations_and_wasted_space_to_file("ModifiedNextFit_dimensional", output_dir);
            } else if (className == "ModifiedFirstFit_dimensional") {
                ModifiedFirstFit_dimensional ff(E, (1000 + mu) / (mu + 7));
                for (auto& job : jobs) ff.add_job(job);
                rho = (double) ff.cost() / ((long long) cost_opt);
                ff.save_server_durations_and_wasted_space_to_file("ModifiedFirstFit_dimensional", output_dir);
            } else if (className == "FirstFit_dimensional") {
                FirstFit_dimensional ff(E);
                for (auto& job : jobs) ff.add_job(job);
                rho = (double) ff.cost() / ((long long) cost_opt);
                ff.save_server_durations_and_wasted_space_to_file("FirstFitAlgorithm", output_dir);
            } else if (className == "BestFit_dimensional") {
                BestFit_dimensional ff(E);
                for (auto& job : jobs) ff.add_job(job);
                rho = (double) ff.cost() / ((long long) cost_opt);
                ff.save_server_durations_and_wasted_space_to_file("BestFit_dimensional", output_dir);
            } else if (className == "LastFit_dimensional") {
                LastFit_dimensional ff(E);
                for (auto& job : jobs) ff.add_job(job);
                rho = (double) ff.cost() / ((long long) cost_opt);
                ff.save_server_durations_and_wasted_space_to_file("LastFit_dimensional", output_dir);
            } else if (className == "RandomFit_dimensional") {
                RandomFit_dimensional ff(E);
                for (auto& job : jobs) ff.add_job(job);
                rho += (double) ff.cost() / ((long long) cost_opt);
                ff.save_server_durations_and_wasted_space_to_file("RandomFit_dimensional", output_dir);
            } else if (className == "WorstFit_dimensional") {
                WorstFit_dimensional ff(E);
                for (auto& job : jobs) ff.add_job(job);
                rho = (double) ff.cost() / ((long long) cost_opt);
                ff.save_server_durations_and_wasted_space_to_file("WorstFit_dimensional", output_dir);
            } else if (className == "Greedy_dimensional") {
                Greedy_dimensional ff(E);
                for (auto& job : jobs) ff.add_job(job);
                rho = (double) ff.cost() / ((long long) cost_opt);
                ff.save_server_durations_and_wasted_space_to_file("Greedy_dimensional", output_dir);
            } else if (className == "Greedy_Greedy") {
                Greedy_Greedy ff(E, threshold_greedy_greedy);
                for (auto& job : jobs) ff.add_job(job);
                rho = (double) ff.cost() / ((long long) cost_opt);
                ff.print_job_count();
            } else if (className == "Greedy_dimensional_with_Hybrid") {
                Greedy_dimensional_with_Hybrid ff(E, threshold_clairvoyant);
                for (auto& job : jobs) ff.add_job(job);
                rho = (double) ff.cost() / ((long long) cost_opt);
                ff.print_job_count();
            } else if (className == "MTF_dimensional") {
                MTF_dimensional ff(E);
                for (auto& job : jobs) ff.add_job(job);
                rho = (double) ff.cost() / ((long long) cost_opt);
                ff.save_server_durations_and_wasted_space_to_file("MTF_dimensional", output_dir);
            } else if (className == "DepartureStrategy") {
                DepartureStrategy ff(E, total_span, interval_departure);
                for (auto it = jobs.begin(); it != jobs.end(); it++) ff.addJob(*it);
                rho = (double) ff.getCost() / ((long long) cost_opt);
                ff.save_server_durations_and_wasted_space_to_file("DepartureStrategy", output_dir);
            } else if (className == "Duration") {
                Duration ff(E, total_span, interval_duration);
                for (auto it = jobs.begin(); it != jobs.end(); it++) ff.addJob(*it);
                rho = (double) ff.getCost() / ((long long) cost_opt);
                ff.save_server_durations_and_wasted_space_to_file("Duration", output_dir);
            } else if (className == "HybridAlgorithm") {
                HybridAlgorithm ff(E);
                for (auto it = jobs.begin(); it != jobs.end(); it++) ff.addJob(*it);
                rho = (double) ff.getCost() / ((long long) cost_opt);
                ff.save_server_durations_and_wasted_space_to_file("HybridAlgorithm", output_dir);
            } else if (className == "NewHybrid") {
                NewHybrid ff(E);
                for (auto it = jobs.begin(); it != jobs.end(); it++) ff.addJob(*it);
                rho = (double) ff.getCost() / ((long long) cost_opt);
                ff.save_server_durations_and_wasted_space_to_file("NewHybrid", output_dir);
            } else if (className == "NewGreedy") {
                NewGreedy ff(E, threshold_new_greedy);
                for (auto& job : jobs) ff.add_job(job);
                rho = (double) ff.cost() / ((long long) cost_opt);
            } else if (className == "Greedy_Duration") {
                Greedy_Duration ff(E, threshold_greedy_duration, total_span);
                for (auto& job : jobs) ff.add_job(job);
                rho = (double) ff.cost() / ((long long) cost_opt);
                ff.print_job_count();
            } else if (className == "FirstFit_FirstFit") {
                FirstFit_FirstFit ff(E, threshold_weakly_clairvoyant);
                for (auto& job : jobs) ff.add_job(job);
                rho = (double) ff.cost() / ((long long) cost_opt);
                ff.print_job_count();
            } else if (className == "FirstFit_BestFit") {
                FirstFit_BestFit ff(E, threshold_weakly_clairvoyant);
                for (auto& job : jobs) ff.add_job(job);
                rho = (double) ff.cost() / ((long long) cost_opt);
                ff.print_job_count();
            } else if (className == "BestFit_BestFit") {
                BestFit_BestFit ff(E, threshold_weakly_clairvoyant);
                for (auto& job : jobs) ff.add_job(job);
                rho = (double) ff.cost() / ((long long) cost_opt);
                ff.print_job_count();
            } else if (className == "BestFit_FirstFit") {
                BestFit_FirstFit ff(E, threshold_weakly_clairvoyant);
                for (auto& job : jobs) ff.add_job(job);
                rho = (double) ff.cost() / ((long long) cost_opt);
                ff.print_job_count();
            }
            
            std::cout << "Alg = " << className << " mu = " << mu << " rho = " << rho << std::endl;
        }
    }
}

void run_Azure_experiment_dimensional(int64_t mu, std::vector<Job_dimensional>& jobs,
                                      const std::string& output_dir,
                                      int64_t threshold_clairvoyant,
                                      int64_t threshold_greedy_greedy,
                                      int64_t threshold_greedy_duration,
                                      int64_t threshold_new_greedy,
                                      int64_t threshold_weakly_clairvoyant,
                                      int64_t interval_departure,
                                      int64_t interval_duration) {
    std::vector<std::vector<int64_t>> Es;
    Es.push_back(std::vector<int64_t>(4, 1000)); // capacity of bin is 1000 for each dimension

    std::vector<std::string> algs_array;
    algs_array.push_back("NextFit_dimensional");
    algs_array.push_back("ModifiedNextFit_dimensional");
    algs_array.push_back("WorstFit_dimensional");
    algs_array.push_back("FirstFit_dimensional");
    algs_array.push_back("ModifiedFirstFit_dimensional");
    algs_array.push_back("BestFit_dimensional");
    algs_array.push_back("LastFit_dimensional");
    algs_array.push_back("RandomFit_dimensional");
    algs_array.push_back("MTF_dimensional");
    algs_array.push_back("Duration");
    algs_array.push_back("HybridAlgorithm");
    algs_array.push_back("NewHybrid");
    algs_array.push_back("Greedy_dimensional");
    algs_array.push_back("Greedy_Greedy");
    algs_array.push_back("NewGreedy");
    algs_array.push_back("Greedy_dimensional_with_Hybrid"); 
    algs_array.push_back("Greedy_Duration");
    algs_array.push_back("DepartureStrategy");
    algs_array.push_back("FirstFit_FirstFit");
    algs_array.push_back("FirstFit_BestFit");
    algs_array.push_back("BestFit_BestFit");
    algs_array.push_back("BestFit_FirstFit");

    get_ratio_Azure(Es, mu, algs_array, jobs, output_dir,
                    threshold_clairvoyant,
                    threshold_greedy_greedy,
                    threshold_greedy_duration,
                    threshold_new_greedy,
                    threshold_weakly_clairvoyant,
                    interval_departure,
                    interval_duration);
}

void run_grid_search(int64_t mu,
                     std::vector<Job_dimensional>& jobs,
                     const std::string& output_dir,
                     int64_t threshold_min,
                     int64_t threshold_max,
                     int64_t threshold_step) {

    std::vector<int64_t> E(4, 1000); // 4 dimensions, capacity 1000

    // Compute OPT lower bound once
    Opt_dimensional opt(jobs);
    std::pair<long long, long long> result = opt.ceilLBandSpan(E);
    long long cost_opt = result.first;
    long long total_span = result.second;

    // Open output files for each algorithm
    std::ofstream outFF_FF(output_dir + "GridSearch_FirstFit_FirstFit.csv");
    std::ofstream outFF_BF(output_dir + "GridSearch_FirstFit_BestFit.csv");
    std::ofstream outBF_BF(output_dir + "GridSearch_BestFit_BestFit.csv");
    std::ofstream outBF_FF(output_dir + "GridSearch_BestFit_FirstFit.csv");
    std::ofstream outGG(output_dir + "GridSearch_Greedy_Greedy.csv");
    std::ofstream outGH(output_dir + "GridSearch_Greedy_Hybrid.csv");
    std::ofstream outNG(output_dir + "GridSearch_NewGreedy.csv");
    std::ofstream outGD(output_dir + "GridSearch_Greedy_Duration.csv");

    // Write headers
    outFF_FF << "Threshold,Rho\n";
    outFF_BF << "Threshold,Rho\n";
    outBF_BF << "Threshold,Rho\n";
    outBF_FF << "Threshold,Rho\n";
    outGG    << "Threshold,Rho\n";
    outGH    << "Threshold,Rho\n";
    outNG    << "Threshold,Rho\n";
    outGD    << "Threshold,Rho\n";

    // Grid search loop
    for (int64_t Thre = threshold_min; Thre <= threshold_max; Thre += threshold_step) {
        std::cout << "Running threshold = " << Thre << std::endl;

        // FirstFit_FirstFit
        FirstFit_FirstFit ff1(E, Thre);
        for (auto& job : jobs) ff1.add_job(job);
        outFF_FF << Thre << "," << (double)ff1.cost() / cost_opt << "\n";

        // FirstFit_BestFit
        FirstFit_BestFit ff2(E, Thre);
        for (auto& job : jobs) ff2.add_job(job);
        outFF_BF << Thre << "," << (double)ff2.cost() / cost_opt << "\n";

        // BestFit_BestFit
        BestFit_BestFit ff3(E, Thre);
        for (auto& job : jobs) ff3.add_job(job);
        outBF_BF << Thre << "," << (double)ff3.cost() / cost_opt << "\n";

        // BestFit_FirstFit
        BestFit_FirstFit ff4(E, Thre);
        for (auto& job : jobs) ff4.add_job(job);
        outBF_FF << Thre << "," << (double)ff4.cost() / cost_opt << "\n";

        // Greedy_Greedy
        Greedy_Greedy ff5(E, Thre);
        for (auto& job : jobs) ff5.add_job(job);
        outGG << Thre << "," << (double)ff5.cost() / cost_opt << "\n";

        // Greedy_dimensional_with_Hybrid
        Greedy_dimensional_with_Hybrid ff6(E, Thre);
        for (auto& job : jobs) ff6.add_job(job);
        outGH << Thre << "," << (double)ff6.cost() / cost_opt << "\n";

        // NewGreedy
        NewGreedy ff7(E, Thre);
        for (auto& job : jobs) ff7.add_job(job);
        outNG << Thre << "," << (double)ff7.cost() / cost_opt << "\n";

        // Greedy_Duration
        Greedy_Duration ff8(E, Thre, total_span);
        for (auto& job : jobs) ff8.add_job(job);
        outGD << Thre << "," << (double)ff8.cost() / cost_opt << "\n";
    }

    outFF_FF.close(); outFF_BF.close();
    outBF_BF.close(); outBF_FF.close();
    outGG.close();    outGH.close();
    outNG.close();    outGD.close();

    std::cout << "Grid search complete! Results written to " << output_dir << std::endl;
}