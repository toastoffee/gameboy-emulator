/**
  ******************************************************************************
  * @file           : cpu.h
  * @author         : toastoffee
  * @brief          : None
  * @attention      : None
  * @date           : 2024/8/16
  ******************************************************************************
  */



#ifndef GAMEBOY_EMULATOR_CPU_H
#define GAMEBOY_EMULATOR_CPU_H

#include "type.h"

class Emulator;

class CPU {

public:
    u8 a;   // A register
    u8 f;   // F register
    u8 b;   // B register
    u8 c;   // C register
    u8 d;   // D register
    u8 e;   // E register
    u8 h;   // h register
    u8 l;   // l register
    u16 sp; // SP register (stack pointer)
    u16 pc; // PC register (Program counter/pointer)

    bool halted;    // is CPU halted

    bool isInterruptMasterEnabled;       // interrupt master enable flag
    u8 interruptMasterEnablingCountdown; // interrupt master enabling countdown

    u16 af() const { return (((u16)a) << 8) + (u16)f ; }
    u16 bc() const { return (((u16)b) << 8) + (u16)c ; }
    u16 de() const { return (((u16)d) << 8) + (u16)e ; }
    u16 hl() const { return (((u16)h) << 8) + (u16)l ; }

    void af(u16 v) { a = (u8)(v >> 8); f = (u8)(v & 0xF0); } // The lower 4 bits of F should always be 0.
    void bc(u16 v) { b = (u8)(v >> 8); c = (u8)(v & 0xFF); }
    void de(u16 v) { d = (u8)(v >> 8); e = (u8)(v & 0xFF); }
    void hl(u16 v) { h = (u8)(v >> 8); l = (u8)(v & 0xFF); }

    bool fz() const { return (f & 0x80) != 0; }
    bool fn() const { return (f & 0x40) != 0; }
    bool fh() const { return (f & 0x20) != 0; }
    bool fc() const { return (f & 0x10) != 0; }

    void set_fz() { f |= 0x80; }
    void reset_fz() { f &= 0x7F; }
    void set_fn() { f |= 0x40; }
    void reset_fn() { f &= 0xBF; }
    void set_fh() { f |= 0x20; }
    void reset_fh() { f &= 0xDF; }
    void set_fc() { f |= 0x10; }
    void reset_fc() { f &= 0xEF; }

    void Init();
    void Step(Emulator* emu);

    // enable interrupt master
    void EnableInterruptMaster();

    // disable interrupt master
    void DisableInterruptMaster();

    void ServiceInterrupt(Emulator* emu);
};


#endif //GAMEBOY_EMULATOR_CPU_H
