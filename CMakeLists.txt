cmake_minimum_required(VERSION 3.24.0)

project("NES Emulator" CXX)

# Set default compile flags for GCC, per https://stackoverflow.com/a/2274040
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    message(STATUS "GCC detected, adding compile flags")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG -Wall -Wextra -pedantic -static-libstdc++")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wall -Wextra -pedantic -Werror -static-libstdc++")
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} -Wall -Wextra -pedantic -Werror -static-libstdc++")
elseif(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    message(STATUS "MSVC detected, adding compile flags")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /DDEBUG /W4 /WX")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /W4 /WX")
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} /W4 /WX")
endif()

set(BIN_DIR "${CMAKE_SOURCE_DIR}/bin")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BIN_DIR})

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

add_subdirectory(lib)

enable_testing()
add_test(NAME nestest_execute COMMAND main CPU_TEST nestest
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set_tests_properties(nestest_execute PROPERTIES TIMEOUT 1)
add_test(
    NAME nestest_match
    COMMAND diff    ${CMAKE_SOURCE_DIR}/test/nestestLog.txt
                    ${CMAKE_SOURCE_DIR}/test/nestest.log
)

# Could also compare the first lines of each file using technique from https://superuser.com/a/511406
add_test(NAME blargg_cpu_test5_official_execute COMMAND main CPU_TEST blargg5official
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set_tests_properties(blargg_cpu_test5_official_execute PROPERTIES TIMEOUT 5)
add_test(
    NAME blargg_cpu_test5_official_match
    COMMAND python ${CMAKE_SOURCE_DIR}/test/logCompare.py
        ${CMAKE_SOURCE_DIR}/test/blargg5Log.txt
        ${CMAKE_SOURCE_DIR}/test/blargg_cpu_test5_official.log
)
