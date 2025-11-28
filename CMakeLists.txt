cmake_minimum_required(VERSION 3.28.0)

project("NES Emulator" CXX)

add_executable(main)

target_sources(main
    PUBLIC
    # Core
    src/core/main.cpp
    src/core/rom.cpp
    src/core/nes.cpp
    src/core/ppu.cpp
    # CPU
    src/cpu/cpu.cpp
    src/cpu/cpu_test.cpp
    src/cpu/logger.cpp
    src/cpu/opcodes.cpp
    # Display
    src/display/display.cpp
    # Memory
    src/memory/core_memory.cpp
    src/memory/mapper000.cpp
    src/memory/mapper001.cpp
    src/memory/memory_factory.cpp
)

target_include_directories(main PRIVATE
    src/core/include
    src/cpu/include
    src/display/include
    src/memory/include
)

target_compile_features(main PRIVATE cxx_std_23)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    target_link_libraries(main PRIVATE stdc++exp) # For <print>
endif()

add_custom_command(
   OUTPUT ${CMAKE_SOURCE_DIR}/test/blargg_cpu_test5_official.log
   COMMAND xz -kdf ${CMAKE_SOURCE_DIR}/test/blargg_cpu_test5_official.log.xz
)
add_custom_target(blargg_cpu_test5_official DEPENDS ${CMAKE_SOURCE_DIR}/test/blargg_cpu_test5_official.log)

add_dependencies(main blargg_cpu_test5_official)

add_subdirectory(lib)

enable_testing()

find_package(Python REQUIRED)

add_test(NAME nestest_execute COMMAND main CPU_TEST nestest
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
set_tests_properties(nestest_execute PROPERTIES TIMEOUT 2)
add_test(
    NAME nestest_match
    COMMAND ${Python_EXECUTABLE} ${CMAKE_SOURCE_DIR}/test/logCompare.py
        ${CMAKE_SOURCE_DIR}/test/nestestLog.txt
        ${CMAKE_SOURCE_DIR}/test/nestest.log
)

# Could also compare the first lines of each file using technique from https://superuser.com/a/511406
add_test(NAME blargg_cpu_test5_official_execute COMMAND main CPU_TEST blargg5official
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
set_tests_properties(blargg_cpu_test5_official_execute PROPERTIES TIMEOUT 20)

add_test(
    NAME blargg_cpu_test5_official_match
    COMMAND ${Python_EXECUTABLE} ${CMAKE_SOURCE_DIR}/test/logCompare.py
        ${CMAKE_SOURCE_DIR}/test/blargg5Log.txt
        ${CMAKE_SOURCE_DIR}/test/blargg_cpu_test5_official.log
)
