# If using Windows MSVC, grab SDL2 from the local folder
if (MSVC)
    set(SDL2_DIR SDL2-VC/cmake)
endif()

find_package(SDL2 CONFIG REQUIRED)

target_include_directories(main PRIVATE ${SDL2_INCLUDE_DIRS})

target_link_libraries(main PRIVATE ${SDL2_LIBRARIES})

# Add SDL2.dll to the executables folder if on Windows
if(CMAKE_HOST_WIN32)
    file(COPY "${SDL2_BINDIR}/SDL2.dll" DESTINATION ${CMAKE_BINARY_DIR})
endif()
