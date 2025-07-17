cmake_minimum_required(VERSION 3.16)
project(FluidSimCmake)

# Compiler
set(CMAKE_C_COMPILER "/opt/homebrew/opt/llvm/bin/clang")
set(CMAKE_CXX_COMPILER "/opt/homebrew/opt/llvm/bin/clang++")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -march=native -ffast-math -flto")
set(CMAKE_EXE_LINKER_FLAGS_RELEASE "-flto")

# OpenMP
set(OpenMP_C_FLAGS "-Xpreprocessor -fopenmp -I/opt/homebrew/opt/libomp/include")
set(OpenMP_CXX_FLAGS "${OpenMP_C_FLAGS}")
set(OpenMP_omp_LIBRARY "/opt/homebrew/opt/libomp/lib/libomp.dylib")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")

set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -g")

# Executable
add_executable(my_project
    main.cpp
    core/Ensemble.cpp
    core/Initialiser.cpp
    core/CollisionGrid.cpp
)

# Includes
target_include_directories(my_project PRIVATE
    ${CMAKE_SOURCE_DIR}/core
    /opt/homebrew/Cellar/sfml/3.0.1/include
)

# Link SFML manually
target_link_directories(my_project PRIVATE
    /opt/homebrew/Cellar/sfml/3.0.1/lib
)

target_link_libraries(my_project
    sfml-graphics
    sfml-window
    sfml-system
    sfml-audio
    sfml-network
    ${OpenMP_omp_LIBRARY}
)

# Enable compile_commands.json for clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
