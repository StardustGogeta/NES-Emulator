# A script for comparing the contents of known NES log files
# with those generated by this C++ emulator

import os, sys

path = os.path.dirname(os.path.realpath(__file__))

assert(len(sys.argv) == 3)

# Allow ending before the end of the line, for dealing with cycle count and PPU timing later
END_INDEX = None

with open(sys.argv[1]) as emuLog:
    with open(sys.argv[2]) as goodLog:
        i = 0
        while emu_line := emuLog.readline():
            ref_line = goodLog.readline()

            if emu_line != ref_line:
                ref_addr, ref_opc = ref_line.split()[:2]
                emu_addr, emu_opc = emu_line.split()[:2]

                assert ref_addr == emu_addr, f"Address mismatch on line {i + 1}: {ref_addr} expected, but saw {emu_addr}."
                assert ref_opc == emu_opc, f"Opcode mismatch on line {i + 1}: {ref_opc} expected, but saw {emu_opc}."

                if END_INDEX:
                    ref_fragment = ref_line[:END_INDEX]
                    emu_fragment = emu_line[:END_INDEX]
                else:
                    ref_fragment = ref_line
                    emu_fragment = emu_line
                assert ref_fragment == emu_fragment, f"Line mismatch on line {i + 1}:\nExpected: {ref_fragment}\nSaw:      {emu_fragment}"
            i += 1

print("All checks completed successfully.")
