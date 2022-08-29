cmake_minimum_required(VERSION 3.24.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG -Wall -Wextra -pedantic -Werror")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wall -Wextra -pedantic -Werror")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} -Wall -Wextra -pedantic -Werror")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")

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

enable_testing()
add_test(NAME nestest_execute COMMAND main nestest)
set_tests_properties(nestest_execute PROPERTIES TIMEOUT 0.2)
add_test(NAME nestest_match COMMAND python ${CMAKE_SOURCE_DIR}/test/nestestCompare.py)
