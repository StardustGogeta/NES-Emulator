#include "nes.h"
#include "display.h"
#include "cpu_test.h"
#include <iostream>
#include <string>

#include "SDL.h"

int main(int argc, char* argv[]) {
    std::string path;
    
    if (argc > 1) {
        path = argv[1];
        std::cout << "Argument: " << path << std::endl;
    } else {
        path = "mario.nes";
        std::cout << "Defaulting to " << path << std::endl;
    }

    if (path == "CPU_TEST") {
        std::string testName = argc > 2 ? argv[2] : "";
        runCpuTest(testName);
    } else if (path == "DISPLAY_TEST") {
        std::string testType = argc > 2 ? argv[2] : "rectangle";
        runDisplayTest(testType);
    } else {
        ROM* rom = new ROM(); // Prepare to load ROM file
        rom->setPath(path); // Decode header into ROM object, parse header
        
        NES* nes = new NES();
        nes->loadROM(rom);
    }

    return 0;
}
