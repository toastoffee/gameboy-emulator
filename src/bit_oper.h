/**
  ******************************************************************************
  * @file           : bit_oper.h
  * @author         : toastoffee
  * @brief          : None
  * @attention      : None
  * @date           : 2024/8/17
  ******************************************************************************
  */



#ifndef GAMEBOY_EMULATOR_BIT_OPER_H
#define GAMEBOY_EMULATOR_BIT_OPER_H

#include "type.h"

inline bool bitTest(const u8* baseAddr, u8 bitOffset) {
    u8 v1 = *(baseAddr + bitOffset / 8);
    u8 v2 = 1 << (bitOffset % 8);
    return (v1 & v2) != 0;
}

inline void bitSet(u8* baseAddr, u8 bitOffset) {
    u8 v = 1 << (bitOffset % 8);
    *(baseAddr + bitOffset / 8) |= v;
}

inline void bitReset(u8* baseAddr, u8 bitOffset) {
    u8 v = 1 << (bitOffset % 8);
    v = ~v;
    *(baseAddr + bitOffset / 8) &= v;
}


#endif //GAMEBOY_EMULATOR_BIT_OPER_H
