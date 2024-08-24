/**
  ******************************************************************************
  * @file           : cartridge.cpp
  * @author         : toastoffee
  * @brief          : None
  * @attention      : None
  * @date           : 2024/8/9
  ******************************************************************************
  */



#include "cartridge.h"
#include "emulator.h"
#include "log-min.h"

CartridgeHeader *GetCartridgeHeader(byte *romData) {
    return (CartridgeHeader*)(romData + 0x0100);
}

const c8 *GetCartridgeTypename(u8 type) {
    if (type <= 0x22) {
        return ROM_TYPES[type];
    }
    return "UNKNOWN";
}

const c8 *GetCartridgeRamSizeName(u8 ramSizeCode) {
    if(ramSizeCode <= 0x05) {
        return RAM_SIZE_TYPES[ramSizeCode];
    }
    return "UNKNOWN";
}

const c8 *GetCartridgeLicCodeName(u8 licCode) {
    switch(licCode)
    {
        case 0x00 : return "None";
        case 0x01 : return "Nintendo R&D1";
        case 0x08 : return "Capcom";
        case 0x13 : return "Electronic Arts";
        case 0x18 : return "Hudson Soft";
        case 0x19 : return "b-ai";
        case 0x20 : return "kss";
        case 0x22 : return "pow";
        case 0x24 : return "PCM Complete";
        case 0x25 : return "san-x";
        case 0x28 : return "Kemco Japan";
        case 0x29 : return "seta";
        case 0x30 : return "Viacom";
        case 0x31 : return "Nintendo";
        case 0x32 : return "Bandai";
        case 0x33 : return "Ocean/Acclaim";
        case 0x34 : return "Konami";
        case 0x35 : return "Hector";
        case 0x37 : return "Taito";
        case 0x38 : return "Hudson";
        case 0x39 : return "Banpresto";
        case 0x41 : return "Ubi Soft";
        case 0x42 : return "Atlus";
        case 0x44 : return "Malibu";
        case 0x46 : return "angel";
        case 0x47 : return "Bullet-Proof";
        case 0x49 : return "irem";
        case 0x50 : return "Absolute";
        case 0x51 : return "Acclaim";
        case 0x52 : return "Activision";
        case 0x53 : return "American sammy";
        case 0x54 : return "Konami";
        case 0x55 : return "Hi tech entertainment";
        case 0x56 : return "LJN";
        case 0x57 : return "Matchbox";
        case 0x58 : return "Mattel";
        case 0x59 : return "Milton Bradley";
        case 0x60 : return "Titus";
        case 0x61 : return "Virgin";
        case 0x64 : return "LucasArts";
        case 0x67 : return "Ocean";
        case 0x69 : return "Electronic Arts";
        case 0x70 : return "Infogrames";
        case 0x71 : return "Interplay";
        case 0x72 : return "Broderbund";
        case 0x73 : return "sculptured";
        case 0x75 : return "sci";
        case 0x78 : return "THQ";
        case 0x79 : return "Accolade";
        case 0x80 : return "misawa";
        case 0x83 : return "lozc";
        case 0x86 : return "Tokuma Shoten Intermedia";
        case 0x87 : return "Tsukuda Original";
        case 0x91 : return "Chunsoft";
        case 0x92 : return "Video system";
        case 0x93 : return "Ocean/Acclaim";
        case 0x95 : return "Varie";
        case 0x96 : return "Yonezawa/s’pal";
        case 0x97 : return "Kaneko";
        case 0x99 : return "Pack in soft";
        case 0xA4 : return "Konami (Yu-Gi-Oh!)";
        default: break;
    }
    return "UNKNOWN";
}

u8 CartridgeRead(Emulator *emu, u16 addr) {
    if(addr <= 0x7FFF) {
        return emu->romData[addr];
    }
    if(addr >= 0xA000 && addr <= 0xBFFF && emu->cRam)
    {
        return emu->cRam[addr - 0xA000];
    }
    ERROR("Unsupported cartridge read address: 0x%04X", (u32)addr);
    return 0xFF;
}

void CartridgeWrite(Emulator *emu, u16 addr, u8 data) {

    if(addr >= 0xA000 && addr <= 0xBFFF && emu->cRam)
    {
        emu->cRam[addr - 0xA000] = data;
        return;
    }

    ERROR("Unsupported cartridge write address: 0x%04X", (u32)addr);
}
