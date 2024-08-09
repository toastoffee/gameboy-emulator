/**
  ******************************************************************************
  * @file           : cartridge.h
  * @author         : toastoffee
  * @brief          : None
  * @attention      : None
  * @date           : 2024/8/9
  ******************************************************************************
  */



#ifndef GAMEBOY_EMULATOR_CARTRIDGE_H
#define GAMEBOY_EMULATOR_CARTRIDGE_H

#include <cstdint>

typedef uint8_t u8;
typedef uint8_t byte;

typedef char c8;


struct CartridgeHeader {
    u8 entry[4];
    u8 logo[0x30];
    c8 title[16];
    u8 new_lic_code[2];
    u8 sgb_flag;
    u8 cartridge_type;
    u8 rom_size;
    u8 ram_size;
    u8 dest_code;
    u8 lic_code;
    u8 version;
    u8 checksum;
    u8 global_checksum[2];
};

CartridgeHeader* GetCartridgeHeader(byte* romData) {
    return (CartridgeHeader*)(romData + 0x0100);
}


#endif //GAMEBOY_EMULATOR_CARTRIDGE_H
