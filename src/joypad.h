/**
  ******************************************************************************
  * @file           : joypad.h
  * @author         : toastoffee
  * @brief          : None
  * @attention      : None
  * @date           : 2024/8/23
  ******************************************************************************
  */



#ifndef GAMEBOY_EMULATOR_JOYPAD_H
#define GAMEBOY_EMULATOR_JOYPAD_H

#include "type.h"

class Emulator;

class Joypad {
public:
    bool a;
    bool b;
    bool select;
    bool start;
    bool right;
    bool left;
    bool up;
    bool down;
    //! 0xFF00
    u8 p1;

    void init();
    u8 get_key_state() const;
    void update(Emulator* emu);
    u8 bus_read();
    void bus_write(u8 v);
};


#endif //GAMEBOY_EMULATOR_JOYPAD_H
