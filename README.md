# NES-Emulator

![GitHub Top Language](https://img.shields.io/github/languages/top/StardustGogeta/NES-Emulator)
![GitHub Issues](https://img.shields.io/github/issues/StardustGogeta/NES-Emulator)
[![CodeFactor](https://www.codefactor.io/repository/github/stardustgogeta/nes-emulator/badge)](https://www.codefactor.io/repository/github/stardustgogeta/nes-emulator)
[![CMake](https://github.com/StardustGogeta/NES-Emulator/actions/workflows/cmake.yml/badge.svg)](https://github.com/StardustGogeta/NES-Emulator/actions/workflows/cmake.yml)
[![Microsoft C++ Code Analysis](https://github.com/StardustGogeta/NES-Emulator/actions/workflows/msvc.yml/badge.svg)](https://github.com/StardustGogeta/NES-Emulator/actions/workflows/msvc.yml)
![GitHub Repo Size](https://img.shields.io/github/repo-size/StardustGogeta/NES-Emulator)
![CMake Version](https://img.shields.io/badge/CMake-3.24.0-red)
![C++ Version](https://img.shields.io/badge/C++-23-white)

An incomplete NES emulator made using SDL2 and C++23.

**Roadmap**

- ~~Basic source code layout~~
- ~~ROM file input and processing~~
- ~~Continuous integration / static analysis tool setup~~
- ~~Official CPU opcode emulation~~
- Full unofficial CPU opcode emulation
- ~~Finish nestest support~~
- Additional CPU tests
- PPU emulation
- Display support
- Mapper support
- Testing with game ROMs


## Quick-Start Instructions

#### Prerequisites
- cmake
- gcc / MinGW / MSVC (with C++23 standard)

**Note - This project uses `<print>`, which will not be available in the GCC standard library until GCC 14. Currently, only the latest version of MSVC is supported.**

In order to build this, create a folder named `build`, navigate to this folder, and run

    cmake .. -G "Unix Makefiles"
    make

This will create a `bin` folder in the repository and build the project from the source.

---

### References

- NES memory mapping: https://fceux.com/web/help/NESRAMMappingFindingValues.html
- NES emulator in JS: https://web.archive.org/web/20210624190954/http://blog.alexanderdickson.com/javascript-nes-emulator-part-1
- NES memory mapping: https://en.wikibooks.org/wiki/NES_Programming/Memory_Map
- 6502 instruction set: https://web.archive.org/web/20140619200254/http://www.obelisk.demon.co.uk/6502/reference.html (no longer active)
- 6502 addressing modes: https://web.archive.org/web/20150128022425/http://www.obelisk.demon.co.uk/6502/addressing.html (no longer active)
- iNES file format: https://www.nesdev.org/wiki/INES
- List of games by mapper: https://nesdir.github.io/mapper0.html
- NES addressing modes: https://www.nesdev.org/wiki/CPU_addressing_modes
- 6502 opcodes: http://www.6502.org/tutorials/6502opcodes.html
- More 6502 opcodes (with cycle counts): http://www.oxyron.de/html/opcodes02.html
- Status flags: https://www.nesdev.org/wiki/Status_flags
- Detailed opcode descriptions: http://www.romdetectives.com/Wiki/index.php?title=AND
- Reference implementation: https://github.com/fogleman/nes
- NES overflow flag: http://www.righto.com/2012/12/the-6502-overflow-flag-explained.html
- NES opcode timing: http://atarihq.com/danb/files/64doc.txt
- nestest CPU test: http://www.qmtpro.com/~nes/misc/nestest.txt
- Stack pointer and nestest: https://www.reddit.com/r/EmuDev/comments/g663hk/nestestlog_stack_pointer_starting_at_fd_and_sbc/
- BRK/IRQ/NMI/RESET and stack pointer behavior: https://www.pagetable.com/?p=410
- BRK command behavior: http://www.romdetectives.com/Wiki/index.php?title=BRK
- Detailed opcodes list: https://www.masswerk.at/6502/6502_instruction_set.html
- ADC and SBC commands: https://stackoverflow.com/questions/29193303/6502-emulation-proper-way-to-implement-adc-and-sbc
- NES PPU explanation: https://www.reddit.com/r/EmuDev/comments/evu3u2/what_does_the_nes_ppu_actually_do/
- PPU rendering guide: https://www.nesdev.org/wiki/PPU_rendering
- PPU frame timing: https://www.nesdev.org/wiki/PPU_frame_timing
- CPU power-up information: https://www.nesdev.org/wiki/CPU_power_up_state
- NES APU register guide: https://www.nesdev.org/wiki/APU_registers
- NES emulator guide for Rust: https://bugzmanov.github.io/nes_ebook/chapter_5_1.html
- NES mapper explanation: https://www.reddit.com/r/EmuDev/comments/cyrowl/mappers_and_nes/
- Interesting NES mapper exploration: https://somethingnerdy.com/unlocking-the-nes-for-former-dawn/
- Explanation of mappers and bank switching: https://somethingnerdy.com/mappers-matter/
- NES test ROM collection: https://github.com/christopherpow/nes-test-roms
- NES input data and registers: https://stackoverflow.com/questions/66406934/whats-an-elegant-way-to-read-controller-input-for-nes-6502-assembly-on-ca65
- PPU registers: https://www.nesdev.org/wiki/PPU_registers
- List of NES mappers: https://www.nesdev.org/wiki/Mapper
