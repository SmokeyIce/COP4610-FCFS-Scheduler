# Minimum CMake version required
cmake_minimum_required(VERSION 3.10)
project(os_scheduler VERSION 1.0)

# Set C++ standard to C++17 and make it required
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Enable warnings
if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Add source directory
add_subdirectory(src)

# Copy input data file to build directory
configure_file(${CMAKE_SOURCE_DIR}/process_data.txt
               ${CMAKE_BINARY_DIR}/src/process_data.txt COPYONLY)

# Configure for interactive terminal use
if(TARGET os_scheduler)
    set_target_properties(os_scheduler PROPERTIES
        VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        XCODE_GENERATE_SCHEME TRUE
    )
endif()
