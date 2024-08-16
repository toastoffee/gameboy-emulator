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
#include "log-min.h"

#include <iostream>

Emulator::~Emulator() {
    if(_romData) {
        free(_romData);
        _romData = nullptr;
        _romDataSize = 0;

        INFO("Cartridge Unloaded.");
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
        checkSum = checkSum - _romData[addr] - 1;
    }
    assert(checkSum == header->checksum && "cartridge check sum can't match!");

    // print cartridge load info

    INFO("Cartridge Loaded.");
    INFO("Title    : %s", header->title);
    INFO("Type     : %2.2X (%s)", (u32)header->cartridge_type, GetCartridgeTypename(header->cartridge_type));
    INFO("ROM Size : %u KB", (u32)(32 << header->rom_size));
    INFO("RAM Size : %2.2X (%s)", (u32)(header->ram_size), (GetCartridgeRamSizeName(header->ram_size)));
    INFO("LIC Code : %2.2X (%s)", (u32)(header->lic_code), (GetCartridgeLicCodeName(header->lic_code)));
    INFO("ROM Ver. : %2.2X", (u32)(header->version));
}

void Emulator::Update(f64 deltaTime) {

}
