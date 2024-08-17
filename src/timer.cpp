/**
  ******************************************************************************
  * @file           : timer.cpp
  * @author         : toastoffee
  * @brief          : None
  * @attention      : None
  * @date           : 2024/8/17
  ******************************************************************************
  */



#include "timer.h"
#include "emulator.h"

void Timer::Tick(Emulator *emu) {
    // increase DIV
    u16 prevDiv = div;
    ++div;
    if(IsTimaEnabled()) {
        // check whether we need to increase TIMA in this tick
        bool timaUpdate = false;
        switch (ClockSelect()) {
            case 0:
                timaUpdate = (prevDiv & (1 << 9)) && (!(div & (1 << 9)));   //! 4096Hz
                break;
            case 1:
                timaUpdate = (prevDiv & (1 << 3)) && (!(div & (1 << 3)));   //! 262144Hz
                break;
            case 2:
                timaUpdate = (prevDiv & (1 << 5)) && (!(div & (1 << 5)));   //! 65536Hz
                break;
            case 3:
                timaUpdate = (prevDiv & (1 << 7)) && (!(div & (1 << 7)));   //! 16384Hz
                break;
        }
        if(timaUpdate) {
            if(tima == 0xFF) {
                emu->intFlags |= INT_TIMER;
                tima = tma;
            }
            else {
                tima++;
            }
        }
    }
}

u8 Timer::BusRead(u16 addr) {
    assert(addr >= 0xFF04 && addr <= 0xFF07 && "timer register address illegal!");
    switch (addr) {
        case 0xFF04:
            return ReadDiv();
        case 0xFF05:
            return tima;
        case 0xFF06:
            return tma;
        case 0xFF07:
            return tac;
        default:
            return 0;
    }
}

void Timer::BusWrite(u16 addr, u8 data) {
    assert(addr >= 0xFF04 && addr <= 0xFF07 && "timer register address illegal!");

    switch (addr) {
        case 0xFF04:
            div = 0;
            return;
        case 0xFF05:
            tima = data;
            return;
        case 0xFF06:
            tma = data;
            return;
        case 0xFF07:
            tac = 0xF8 | (data & 0x07);
            return;
        default:
            return;
    }
}
