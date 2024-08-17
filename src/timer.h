/**
  ******************************************************************************
  * @file           : timer.h
  * @author         : toastoffee
  * @brief          : None
  * @attention      : None
  * @date           : 2024/8/17
  ******************************************************************************
  */



#ifndef GAMEBOY_EMULATOR_TIMER_H
#define GAMEBOY_EMULATOR_TIMER_H

#include "type.h"
#include "bit_oper.h"

class Emulator;

class Timer {

    //! 0xFF04
    //! Only the high 8-bit is accessible via bus, thus behaves like incrementing at 16384Hz (once per 256 clock cycles).
    //! Writing any value to this resets the value to 0.
    u16 div;

    //! 0xFF05 Timer counter
    //! Triggers a INT_TIMER when overflows (exceeds 0xFF)
    u8 tima;

    //! 0xFF06 Timer modulo
    //! The value to reset TIMA to if overflows.
    u8 tma;

    //! 0xFF07 Timer control
    u8 tac;

    u8 ReadDiv() const {
        return (u8)(div >> 8);
    }

    u8 ClockSelect() const { return tac & 0x03; }
    bool IsTimaEnabled() const { return bitTest(&tac, 2); }

    void Init() {
        div = 0xAC00;
        tima = 0;
        tma = 0;
        tac = 0xF8;
    }

    void Tick(Emulator* emu);
    u8 BusRead(u16 addr);
    void BusWrite(u16 addr, u8 data);
};


#endif //GAMEBOY_EMULATOR_TIMER_H
