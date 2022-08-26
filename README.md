# NES-Emulator

[![CodeFactor](https://www.codefactor.io/repository/github/stardustgogeta/nes-emulator/badge)](https://www.codefactor.io/repository/github/stardustgogeta/nes-emulator)


An incomplete NES emulator made using SDL2 and C++.

#### Prerequisites
- cmake
- gcc / MinGW

In order to build this, create a folder named `build`, navigate to this folder, and run

    cmake .. -G "Unix Makefiles"
    make

This will create a `bin` folder in the repository and build the project from the source.

---

### References

- NES memory mapping: https://fceux.com/web/help/NESRAMMappingFindingValues.html
- NES emulator in JS: https://web.archive.org/web/20210624190954/http://blog.alexanderdickson.com/javascript-nes-emulator-part-1 (no longer active, has some typos)
- NES memory mapping: https://en.wikibooks.org/wiki/NES_Programming/Memory_Map
- 6502 instruction set: https://web.archive.org/web/20140619200254/http://www.obelisk.demon.co.uk/6502/reference.html (no longer active)
- 6502 addressing modes: https://web.archive.org/web/20150128022425/http://www.obelisk.demon.co.uk/6502/addressing.html (no longer active)
- iNES file format: https://www.nesdev.org/wiki/INES
- List of games by mapper: https://nesdir.github.io/mapper0.html
- NES addressing modes: https://www.nesdev.org/wiki/CPU_addressing_modes
- 6502 opcodes: http://www.6502.org/tutorials/6502opcodes.html
- More 6502 opcodes: http://www.oxyron.de/html/opcodes02.html
- Status flags: https://www.nesdev.org/wiki/Status_flags
- Detailed opcode descriptions: http://www.romdetectives.com/Wiki/index.php?title=AND
- Reference implementation: https://github.com/fogleman/nes
- NES overflow flag: http://www.righto.com/2012/12/the-6502-overflow-flag-explained.html
- NES opcode timing: http://atarihq.com/danb/files/64doc.txt
