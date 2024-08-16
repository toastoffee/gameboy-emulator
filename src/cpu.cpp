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


    } else {
        emu->Tick(1);
    }
}
