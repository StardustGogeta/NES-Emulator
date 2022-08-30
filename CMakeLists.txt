cmake_minimum_required(VERSION 3.24.0)

project("NES Emulator" CXX)

# Set default compile flags for GCC, per https://stackoverflow.com/a/2274040
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    message(STATUS "GCC detected, adding compile flags")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG -Wall -Wextra -pedantic -Werror")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wall -Wextra -pedantic -Werror")
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} -Wall -Wextra -pedantic -Werror")
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")

add_executable(
    main
    # Core
    src/core/main.cpp
    src/core/rom.cpp
    src/core/core_memory.cpp
    src/core/nes.cpp
    # CPU
    src/cpu/cpu.cpp
    src/cpu/logger.cpp
    src/cpu/opcodes.cpp
)

target_include_directories(main PRIVATE
    src/core/include
    src/cpu/include
)

target_compile_features(main PRIVATE cxx_std_20)

enable_testing()
add_test(NAME nestest_execute COMMAND main nestest)
set_tests_properties(nestest_execute PROPERTIES TIMEOUT 0.5)
add_test(NAME nestest_match COMMAND python ${CMAKE_SOURCE_DIR}/test/nestestCompare.py)
