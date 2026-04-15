cmake_minimum_required(VERSION 3.16)
project(rsic_scheduler)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(rsic_scheduler
    main.cpp
    src/server.cpp
    src/opt.cpp
    src/data_loader.cpp
    src/plotter.cpp
    src/experiment.cpp
    src/base_algorithm.cpp
)

target_include_directories(rsic_scheduler PRIVATE include)
