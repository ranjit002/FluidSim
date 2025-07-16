cmake_minimum_required(VERSION 3.16)
project(FluidSimCmake)

# Use Homebrew LLVM (supports OpenMP)
set(CMAKE_C_COMPILER "/opt/homebrew/opt/llvm/bin/clang")
set(CMAKE_CXX_COMPILER "/opt/homebrew/opt/llvm/bin/clang++")

# Enable modern C++ + maximum optimization
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_CXX_FLAGS_RELEASE "-O3 -march=native -ffast-math -flto")
set(CMAKE_EXE_LINKER_FLAGS_RELEASE "-flto")

# OpenMP support
set(OpenMP_C_FLAGS "-Xpreprocessor -fopenmp -I/opt/homebrew/opt/llvm/include")
set(OpenMP_omp_LIBRARY "/opt/homebrew/opt/llvm/lib/libomp.dylib")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_C_FLAGS}")
link_directories("/opt/homebrew/lib" "/opt/homebrew/opt/llvm/lib")

# Include paths
include_directories(
    /opt/homebrew/include
    /opt/homebrew/opt/llvm/include
)

# Sources
add_executable(my_project
    main.cpp
    Ensemble.cpp
)

# Link libraries
target_link_libraries(my_project
    sfml-graphics
    sfml-window
    sfml-system
    ${OpenMP_omp_LIBRARY}
)
