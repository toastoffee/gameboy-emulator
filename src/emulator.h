/**
  ******************************************************************************
  * @file           : emulator.h
  * @author         : toastoffee
  * @brief          : None
  * @attention      : None
  * @date           : 2024/8/10
  ******************************************************************************
  */



#ifndef GAMEBOY_EMULATOR_EMULATOR_H
#define GAMEBOY_EMULATOR_EMULATOR_H

#include "cartridge.h"
#include "memory"

class Emulator {
private:
    byte* _romData = nullptr;
    u64 _romDataSize = 0;

public:
    ~Emulator();

    void Init(const void* cartridgeData, u64 cartridgeDataSize);

    void Update()
};


#endif //GAMEBOY_EMULATOR_EMULATOR_H
