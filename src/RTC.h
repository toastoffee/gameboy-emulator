/**
  ******************************************************************************
  * @file           : RTC.h
  * @author         : toastoffee
  * @brief          : None
  * @attention      : None
  * @date           : 2024/8/24
  ******************************************************************************
  */



#ifndef GAMEBOY_EMULATOR_RTC_H
#define GAMEBOY_EMULATOR_RTC_H

#include "type.h"
#include "bit_oper.h"

class RTC {
public:
    u8 s; // Seconds.
    u8 m; // Minutes.
    u8 h; // Hours.
    u8 dl;// Lower 8 bits of Day Counter.
    u8 dh;// Upper 1 bit of Day Counter, Carry Bit, Halt Flag.

    // Internal state.
    f64 time;
    bool time_latched;
    // Set to `true` when writing 0x00 to 0x6000~0x7FFF.
    bool time_latching;

    void init();
    void update(f64 delta_time);
    void update_time_registers();
    void update_timestamp();
    void latch();
    u16 days() const { return (u16)dl + (((u16)(dh & 0x01)) << 8); }
    bool halted() const { return bitTest(&dh, 6); }
    bool day_overflow() const { return bitTest(&dh, 7); }
};


#endif //GAMEBOY_EMULATOR_RTC_H
