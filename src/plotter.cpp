#include "../include/plotter.h"
#include "../include/utils.h"
#include <fstream>
#include <iostream>
#include <algorithm>
#include <unordered_map>
#include <set>

void saveDataForPlotting_int(const std::vector<Job_dimensional>& jobs, const std::string& output_dir) {
    std::vector<int64_t> durations, coreSizes, memorySizes, ssdSizes, nicSizes;
    std::vector<std::pair<int64_t, int64_t>> totalCoreSizes, totalMemorySizes, totalssdSizes, totalnicSizes;

    // sort jobs by start time
    std::vector<Job_dimensional> sortedJobs = jobs;
    std::sort(sortedJobs.begin(), sortedJobs.end(), compareJobs);

    // build add/remove maps and event times
    std::unordered_map<int64_t, std::vector<Job_dimensional>> addMap, removeMap;
    std::set<int64_t> eventTimes;
    for (const auto& job : sortedJobs) {
        addMap[job.start].push_back(job);
        removeMap[job.end].push_back(job);
        eventTimes.insert(job.start);
        eventTimes.insert(job.end);
    }
    std::vector<int64_t> sortedEventTimes(eventTimes.begin(), eventTimes.end());

    // compute total resource sizes per event time
    int64_t totalCoreSize = 0, totalMemorySize = 0, totalssdSize = 0, totalnicSize = 0;
    for (int64_t time : sortedEventTimes) {
        if (addMap.count(time))
            for (const auto& job : addMap[time]) {
                totalCoreSize   += job.size[0];
                totalMemorySize += job.size[1];
                totalssdSize    += job.size[2];
                totalnicSize    += job.size[3];
            }
        if (removeMap.count(time))
            for (const auto& job : removeMap[time]) {
                totalCoreSize   -= job.size[0];
                totalMemorySize -= job.size[1];
                totalssdSize    -= job.size[2];
                totalnicSize    -= job.size[3];
            }
        totalCoreSizes.emplace_back(time, totalCoreSize);
        totalMemorySizes.emplace_back(time, totalMemorySize);
        totalssdSizes.emplace_back(time, totalssdSize);
        totalnicSizes.emplace_back(time, totalnicSize);
    }

    // collect per-job stats
    for (const auto& job : sortedJobs) {
        durations.push_back(job.end - job.start);
        coreSizes.push_back(job.size[0]);
        memorySizes.push_back(job.size[1]);
        ssdSizes.push_back(job.size[2]);
        nicSizes.push_back(job.size[3]);
    }

    // write files
    std::ofstream outFileDurations(output_dir + "durations.csv");
    std::ofstream outFileCore(output_dir + "core_sizes.csv");
    std::ofstream outFileMemory(output_dir + "memory_sizes.csv");
    std::ofstream outFilessd(output_dir + "ssd_sizes.csv");
    std::ofstream outFilenic(output_dir + "nic_sizes.csv");
    std::ofstream outFileTotalCore(output_dir + "total_core_sizes.csv");
    std::ofstream outFileTotalMemory(output_dir + "total_memory_sizes.csv");
    std::ofstream outFileTotalssd(output_dir + "total_ssd_sizes.csv");
    std::ofstream outFileTotalnic(output_dir + "total_nic_sizes.csv");

    if (!outFileDurations.is_open() || !outFileCore.is_open()) {
        std::cerr << "Unable to open one or more files for writing." << std::endl;
        return;
    }

    outFileDurations << "Duration\n";
    outFileCore << "CoreSize\n";
    outFileMemory << "MemorySize\n";
    outFilessd << "ssdSize\n";
    outFilenic << "nicSize\n";
    outFileTotalCore << "Time,TotalCoreSize\n";
    outFileTotalMemory << "Time,TotalMemorySize\n";
    outFileTotalssd << "Time,TotalSsdSize\n";
    outFileTotalnic << "Time,TotalNicSize\n";

    for (auto d : durations)    outFileDurations << d << "\n";
    for (auto c : coreSizes)    outFileCore      << c << "\n";
    for (auto m : memorySizes)  outFileMemory    << m << "\n";
    for (auto s : ssdSizes)     outFilessd       << s << "\n";
    for (auto n : nicSizes)     outFilenic       << n << "\n";
    for (const auto& e : totalCoreSizes)   outFileTotalCore   << e.first << "," << e.second << "\n";
    for (const auto& e : totalMemorySizes) outFileTotalMemory << e.first << "," << e.second << "\n";
    for (const auto& e : totalssdSizes)    outFileTotalssd    << e.first << "," << e.second << "\n";
    for (const auto& e : totalnicSizes)    outFileTotalnic    << e.first << "," << e.second << "\n";
}


void saveDataForPlotting_raw(const std::vector<RawJob_dimensional>& jobs, const std::string& output_dir) {
    std::vector<double> durations, coreSizes, memorySizes, ssdSizes, nicSizes;

    // sort jobs by start time
    std::vector<RawJob_dimensional> sortedJobs = jobs;
    std::sort(sortedJobs.begin(), sortedJobs.end(), [](const RawJob_dimensional& a, const RawJob_dimensional& b) {
        return a.start < b.start;
    });

    // collect per-job stats
    for (const auto& job : sortedJobs) {
        durations.push_back(job.end - job.start);
        coreSizes.push_back(job.size[0]);
        memorySizes.push_back(job.size[1]);
        ssdSizes.push_back(job.size[2]);
        nicSizes.push_back(job.size[3]);
    }

    // write files
    std::ofstream outFileDurations(output_dir + "raw_durations.csv");
    std::ofstream outFileCore(output_dir + "raw_core_sizes.csv");
    std::ofstream outFileMemory(output_dir + "raw_memory_sizes.csv");
    std::ofstream outFilessd(output_dir + "raw_ssd_sizes.csv");
    std::ofstream outFilenic(output_dir + "raw_nic_sizes.csv");

    if (!outFileDurations.is_open() || !outFileCore.is_open()) {
        std::cerr << "Unable to open one or more files for writing." << std::endl;
        return;
    }

    outFileDurations << "Duration\n";
    outFileCore      << "CoreSize\n";
    outFileMemory    << "MemorySize\n";
    outFilessd       << "ssdSize\n";
    outFilenic       << "nicSize\n";

    for (auto d : durations)   outFileDurations << d << "\n";
    for (auto c : coreSizes)   outFileCore      << c << "\n";
    for (auto m : memorySizes) outFileMemory    << m << "\n";
    for (auto s : ssdSizes)    outFilessd       << s << "\n";
    for (auto n : nicSizes)    outFilenic       << n << "\n";
}