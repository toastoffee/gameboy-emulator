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

#include "type.h"

class Emulator;

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

CartridgeHeader* GetCartridgeHeader(byte* romData);

static const c8* ROM_TYPES[] = {
    "ROM ONLY",
    "MBC1",
    "MBC1+RAM",
    "MBC1+RAM+BATTERY",
    "0x04 ???",
    "MBC2",
    "MBC2+BATTERY",
    "0x07 ???",
    "ROM+RAM 1",
    "ROM+RAM+BATTERY 1",
    "0x0A ???",
    "MMM01",
    "MMM01+RAM",
    "MMM01+RAM+BATTERY",
    "0x0E ???",
    "MBC3+TIMER+BATTERY",
    "MBC3+TIMER+RAM+BATTERY 2",
    "MBC3",
    "MBC3+RAM 2",
    "MBC3+RAM+BATTERY 2",
    "0x14 ???",
    "0x15 ???",
    "0x16 ???",
    "0x17 ???",
    "0x18 ???",
    "MBC5",
    "MBC5+RAM",
    "MBC5+RAM+BATTERY",
    "MBC5+RUMBLE",
    "MBC5+RUMBLE+RAM",
    "MBC5+RUMBLE+RAM+BATTERY",
    "0x1F ???",
    "MBC6",
    "0x21 ???",
    "MBC7+SENSOR+RUMBLE+RAM+BATTERY",
};

static const c8* RAM_SIZE_TYPES[] = {
    "0",
    "-",
    "8 KB (1 bank)",
    "32 KB (4 banks of 8KB each)",
    "128 KB (16 banks of 8KB each)",
    "64 KB (8 banks of 8KB each)"
};

const c8* GetCartridgeTypename(u8 type);

const c8* GetCartridgeRamSizeName(u8 ramSizeCode);

const c8* GetCartridgeLicCodeName(u8 licCode);

u8 CartridgeRead(Emulator *emu, u16 addr);

void CartridgeWrite(Emulator *emu, u16 addr, u8 data);

#endif //GAMEBOY_EMULATOR_CARTRIDGE_H
