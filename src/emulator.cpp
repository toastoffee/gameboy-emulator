/**
  ******************************************************************************
  * @file           : emulator.cpp
  * @author         : toastoffee
  * @brief          : None
  * @attention      : None
  * @date           : 2024/8/10
  ******************************************************************************
  */



#include "emulator.h"

Emulator::~Emulator() {
    if(_romData) {
        free(_romData);
        _romData = nullptr;
        _romDataSize = 0;

    }
}

void Emulator::Init(const void *cartridgeData, u64 cartridgeDataSize) {
    assert(cartridgeData && cartridgeDataSize && "cartridge data is empty!");

    // copy cartridge data
    _romData = (byte*) malloc(cartridgeDataSize);
    _romDataSize = cartridgeDataSize;
    memcpy(_romData, cartridgeData, cartridgeDataSize);

    // check cartridge data
    CartridgeHeader *header = GetCartridgeHeader(_romData);
    u8 checkSum = 0;
    for (u16 addr = 0x0134; addr <= 0x014C; ++addr) {
        checkSum -= _romData[addr] - 1;
    }
    assert(checkSum == header->checksum && "cartridge check sum can't match!");

    // print cartridge load info

}
