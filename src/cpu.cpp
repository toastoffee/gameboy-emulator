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

    isInterruptMasterEnabled = false;
    interruptMasterEnablingCountdown = 0;
}

void CPU::Step(Emulator *emu) {
    if(!halted) {

        if(isInterruptMasterEnabled && (emu->intFlags & emu->intEnableFlags)) {
            ServiceInterrupt(emu);
        }
        else {
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
        }

    } else {
        emu->Tick(1);

        // wake up cpu if any interruption is pending
        // this happens even if IME is disabled (interruptionEnabled = false)
        if(emu->intFlags & emu->intEnableFlags) {
            halted = false;
        }
    }

    // enable interrupt master when countdown reaches 0.
    if(interruptMasterEnablingCountdown) {
        --interruptMasterEnablingCountdown;
        if(!interruptMasterEnablingCountdown) {
            isInterruptMasterEnabled = true;
        }
    }
}

void CPU::EnableInterruptMaster() {
    /*TODO*/
    interruptMasterEnablingCountdown = 2;
}

void CPU::DisableInterruptMaster() {
    isInterruptMasterEnabled = false;
    interruptMasterEnablingCountdown = 0;
}

inline void push_16(Emulator* emu, u16 v)
{
    emu->cpu.sp -= 2;
    emu->BusWrite(emu->cpu.sp + 1, (u8)((v >> 8) & 0xFF));
    emu->BusWrite(emu->cpu.sp, (u8)(v & 0xFF));
}

void CPU::ServiceInterrupt(Emulator *emu) {
    u8 int_flags = emu->intFlags & emu->intEnableFlags;
    u8 service_int = 0;
    if(int_flags & INT_VBLANK) service_int = INT_VBLANK;
    else if(int_flags & INT_LCD_STAT) service_int = INT_LCD_STAT;
    else if(int_flags & INT_TIMER) service_int = INT_TIMER;
    else if(int_flags & INT_SERIAL) service_int = INT_SERIAL;
    else if(int_flags & INT_JOYPAD) service_int = INT_JOYPAD;
    emu->intFlags &= ~service_int;
    emu->cpu.DisableInterruptMaster();
    emu->Tick(2);
    push_16(emu, emu->cpu.pc);
    emu->Tick(2);
    switch(service_int)
    {
        case INT_VBLANK: emu->cpu.pc = 0x40; break;
        case INT_LCD_STAT: emu->cpu.pc = 0x48; break;
        case INT_TIMER: emu->cpu.pc = 0x50; break;
        case INT_SERIAL: emu->cpu.pc = 0x58; break;
        case INT_JOYPAD: emu->cpu.pc = 0x60; break;
    }
    emu->Tick(1);
}
