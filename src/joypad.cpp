/**
  ******************************************************************************
  * @file           : joypad.cpp
  * @author         : toastoffee
  * @brief          : None
  * @attention      : None
  * @date           : 2024/8/23
  ******************************************************************************
  */



#include "joypad.h"
#include "emulator.h"
#include "bit_oper.h"

void Joypad::init()
{
    memset(this, 0, sizeof(Joypad));
    p1 = 0xFF;
}
u8 Joypad::get_key_state() const
{
    u8 v = 0xFF;
    if(!bitTest(&p1, 4))
    {
        if(right) bitReset(&v, 0);
        if(left) bitReset(&v, 1);
        if(up) bitReset(&v, 2);
        if(down) bitReset(&v, 3);
        bitReset(&v, 4);
    }
    if(!bitTest(&p1, 5))
    {
        if(a) bitReset(&v, 0);
        if(b) bitReset(&v, 1);
        if(select) bitReset(&v, 2);
        if(start) bitReset(&v, 3);
        bitReset(&v, 5);
    }
    return v;
}
void Joypad::update(Emulator* emu)
{
    u8 v = get_key_state();
    // Any button is pressed in this update.
    if((bitTest(&p1, 0) && !bitTest(&v, 0)) ||
       (bitTest(&p1, 1) && !bitTest(&v, 1)) ||
       (bitTest(&p1, 2) && !bitTest(&v, 2)) ||
       (bitTest(&p1, 3) && !bitTest(&v, 3)))
    {
        emu->intFlags |= INT_JOYPAD;
    }
    p1 = v;
}
u8 Joypad::bus_read()
{
    return p1;
}
void Joypad::bus_write(u8 v)
{
    // Only update bit 4 and bit 5.
    p1 = (v & 0x30) | (p1 & 0xCF);
    // Refresh key states.
    p1 = get_key_state();
}