/**
  ******************************************************************************
  * @file           : cpu.cpp
  * @author         : toastoffee
  * @brief          : None
  * @attention      : None
  * @date           : 2024/8/16
  ******************************************************************************
  */



#include "cpu.h"
#include "emulator.h"
#include "instruction.h"
#include "log-min.h"

void CPU::Init() {
    af(0x01B0);
    bc(0x0013);
    de(0x00D8);
    hl(0x014D);
    sp = 0xFFFE;
    pc = 0x0100;
    halted = false;
}

void CPU::Step(Emulator *emu) {
    if(!halted) {
        // fetch opcode
        u8 opcode = emu->BusRead(pc);

        // increase counter
        ++pc;

        // execute counter
        InstructionFunc* inst = instructionsMap[opcode];
        if(!inst) {
            ERROR("Instruction 0x%02X not present.", (u32)opcode);
            emu->isPaused = true;
        } else {
            inst(emu);
        }

    } else {
        emu->Tick(1);
    }
}

void CPU::EnableInterruptMaster() {
    /*TODO*/
}
