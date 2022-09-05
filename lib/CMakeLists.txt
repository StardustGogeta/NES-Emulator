# If on Windows, grab SDL2 from the local folder
if (CMAKE_HOST_WIN32)
    if (MSVC) # Get the Visual C++ version
        set(SDL2_DIR SDL2-VC/cmake)
    else() # Get the MinGW version
        set(SDL2_DIR SDL2/cmake)
    endif()
endif()

find_package(SDL2 CONFIG REQUIRED)

target_include_directories(main PRIVATE ${SDL2_INCLUDE_DIRS})

target_link_libraries(main PRIVATE ${SDL2_LIBRARIES})

# Add SDL2.dll to the executables folder if on Windows
if(CMAKE_HOST_WIN32)
    file(COPY "${SDL2_BINDIR}/SDL2.dll" DESTINATION ${BIN_DIR})
endif()
