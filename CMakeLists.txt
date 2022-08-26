cmake_minimum_required(VERSION 3.24.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "../bin")

project("NES Emulator")

add_executable(
    main
    src/main.cpp
    src/rom.cpp
    src/memory.cpp
    src/cpu.cpp
    src/nes.cpp
)

target_include_directories(main PRIVATE src/include)

# enable_testing()
add_test(NAME nestest COMMAND main nestest)
