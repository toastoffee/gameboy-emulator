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


u8 mbc1_read(Emulator* emu, u16 addr)
{
    if(addr <= 0x3FFF)
    {
        if(emu->banking_mode && emu->num_rom_banks > 32)
        {
            u32 bank_index = emu->ram_bank_number;
            u32 bank_offset = bank_index * 32 * 16 * kb;
            return emu->romData[bank_offset + addr];
        }
        else
        {
            return emu->romData[addr];
        }
    }
    if(addr >= 0x4000 && addr <= 0x7FFF)
    {
        // Cartridge ROM bank 01-7F.
        if(emu->banking_mode && emu->num_rom_banks > 32)
        {
            u32 bank_index = emu->rom_bank_number + (emu->ram_bank_number << 5);
            u32 bank_offset = bank_index * 16 * kb;
            return emu->romData[bank_offset + (addr - 0x4000)];
        }
        else
        {
            u32 bank_index = emu->rom_bank_number;
            u32 bank_offset = bank_index * 16 * kb;
            return emu->romData[bank_offset + (addr - 0x4000)];
        }
    }
    if(addr >= 0xA000 && addr <= 0xBFFF)
    {
        if(emu->cRam)
        {
            if(!emu->cram_enable) return 0xFF;
            if(emu->num_rom_banks <= 32)
            {
                if(emu->banking_mode)
                {
                    // Advanced banking mode.
                    u32 bank_offset = emu->ram_bank_number * 8 * kb;
                    assert(bank_offset + (addr - 0xA000) <= emu->cRam_size);
                    return emu->cRam[bank_offset + (addr - 0xA000)];
                }
                else
                {
                    // Simple banking mode.
                    return emu->cRam[addr - 0xA000];
                }
            }
            else
            {
                // ram_bank_number is used for switching ROM banks, use 1 ram page.
                return emu->cRam[addr - 0xA000];
            }
        }
    }
    ERROR("Unsupported MBC1 cartridge read address: 0x%04X", (u32)addr);
    return 0xFF;
}

void mbc1_write(Emulator* emu, u16 addr, u8 data)
{
    if(addr <= 0x1FFF)
    {
        // Enable/disable cartridge RAM.
        if(emu->cRam)
        {
            if((data & 0x0F) == 0x0A)
            {
                emu->cram_enable = true;
            }
            else
            {
                emu->cram_enable = false;
            }
            return;
        }
    }
    if(addr >= 0x2000 && addr <= 0x3FFF)
    {
        // Set ROM bank number.
        emu->rom_bank_number = data & 0x1F;
        if(emu->rom_bank_number == 0)
        {
            emu->rom_bank_number = 1;
        }
        if(emu->num_rom_banks <= 2)
        {
            emu->rom_bank_number = emu->rom_bank_number & 0x01;
        }
        else if(emu->num_rom_banks <= 4)
        {
            emu->rom_bank_number = emu->rom_bank_number & 0x03;
        }
        else if(emu->num_rom_banks <= 8)
        {
            emu->rom_bank_number = emu->rom_bank_number & 0x07;
        }
        else if(emu->num_rom_banks <= 16)
        {
            emu->rom_bank_number = emu->rom_bank_number & 0x0F;
        }
        return;
    }
    if(addr >= 0x4000 && addr <= 0x5FFF)
    {
        // Set RAM bank number.
        emu->ram_bank_number = data & 0x03;
        // Discards unsupported banks.
        if(emu->num_rom_banks > 32)
        {
            if(emu->num_rom_banks <= 64)
            {
                emu->ram_bank_number &= 0x01;
            }
        }
        else
        {
            if(emu->cRam_size <= 8 * kb)
            {
                emu->ram_bank_number = 0;
            }
            else if(emu->cRam_size <= 16 * kb)
            {
                emu->ram_bank_number &= 0x01;
            }
        }
        return;
    }
    if(addr >= 0x6000 && addr <= 0x7FFF)
    {
        // Set banking mode.
        if(emu->num_rom_banks > 32 || emu->cRam_size > 8 * kb)
        {
            emu->banking_mode = data & 0x01;
        }
        return;
    }
    if(addr >= 0xA000 && addr <= 0xBFFF)
    {
        if(emu->cRam)
        {
            if(!emu->cram_enable) return;
            if(emu->num_rom_banks <= 32)
            {
                if(emu->banking_mode)
                {
                    // Advanced banking mode.
                    u32 bank_offset = emu->ram_bank_number * 8 * kb;
                    assert(bank_offset + (addr - 0xA000) <= emu->cRam_size);
                    emu->cRam[bank_offset + (addr - 0xA000)] = data;
                }
                else
                {
                    // Simple banking mode.
                    emu->cRam[addr - 0xA000] = data;
                }
            }
            else
            {
                // ram_bank_number is used for switching ROM banks, use 1 ram page.
                emu->cRam[addr - 0xA000] = data;
            }
            return;
        }
    }
    ERROR("Unsupported MBC1 cartridge write address: 0x%04X", (u32)addr);
}

u8 mbc2_read(Emulator* emu, u16 addr)
{
    if(addr <= 0x3FFF)
    {
        return emu->romData[addr];
    }
    if(addr >= 0x4000 && addr <= 0x7FFF)
    {
        // Cartridge ROM bank 01-0F.
        u32 bank_index = emu->rom_bank_number;
        u32 bank_offset = bank_index * 16 * kb;
        return emu->romData[bank_offset + (addr - 0x4000)];
    }
    if(addr >= 0xA000 && addr <= 0xBFFF)
    {
        if(!emu->cram_enable) return 0xFF;
        u16 data_offset = addr - 0xA000;
        data_offset %= 512;
        return (emu->cRam[data_offset] & 0x0F) | 0xF0;
    }
    ERROR("Unsupported MBC2 cartridge read address: 0x%04X", (u32)addr);
    return 0xFF;
}

void mbc2_write(Emulator* emu, u16 addr, u8 data)
{
    if(addr <= 0x3FFF)
    {
        if(addr & 0x100) // bit 8 is set.
        {
            // Set ROM bank number.
            emu->rom_bank_number = data & 0x0F;
            if(emu->rom_bank_number == 0)
            {
                emu->rom_bank_number = 1;
            }
            if(emu->num_rom_banks <= 2)
            {
                emu->rom_bank_number = emu->rom_bank_number & 0x01;
            }
            else if(emu->num_rom_banks <= 4)
            {
                emu->rom_bank_number = emu->rom_bank_number & 0x03;
            }
            else if(emu->num_rom_banks <= 8)
            {
                emu->rom_bank_number = emu->rom_bank_number & 0x07;
            }
            return;
        }
        else
        {
            // Enable/disable cartridge RAM.
            if(emu->cRam)
            {
                if(data == 0x0A)
                {
                    emu->cram_enable = true;
                }
                else
                {
                    emu->cram_enable = false;
                }
                return;
            }
        }
    }
    else if(addr >= 0xA000 && addr <= 0xBFFF)
    {
        if(!emu->cram_enable) return;
        u16 data_offset = addr - 0xA000;
        data_offset %= 512;
        emu->cRam[data_offset] = data & 0x0F;
        return;
    }
    ERROR("Unsupported MBC2 cartridge write address: 0x%04X", (u32)addr);
}

u8 mbc3_read(Emulator* emu, u16 addr)
{
    if(addr <= 0x3FFF)
    {
        return emu->romData[addr];
    }
    if(addr >= 0x4000 && addr <= 0x7FFF)
    {
        // Cartridge ROM bank 01-7F.
        u32 bank_index = emu->rom_bank_number;
        u32 bank_offset = bank_index * 16 * kb;
        return emu->romData[bank_offset + (addr - 0x4000)];
    }
    if(addr >= 0xA000 && addr <= 0xBFFF)
    {
        if(emu->ram_bank_number <= 0x03)
        {
            if(emu->cRam)
            {
                if(!emu->cram_enable) return 0xFF;
                u32 bank_offset = emu->ram_bank_number * 8 * kb;
                assert(bank_offset + (addr - 0xA000) <= emu->cRam_size);
                return emu->cRam[bank_offset + (addr - 0xA000)];
            }
        }
        if(is_cart_timer(GetCartridgeHeader(emu->romData)->cartridge_type) &&
           emu->ram_bank_number >= 0x08 && emu->ram_bank_number <= 0x0C)
        {
            return ((u8*)(&emu->rtc.s))[emu->ram_bank_number - 0x08];
        }
    }
    ERROR("Unsupported MBC3 cartridge read address: 0x%04X", (u32)addr);
    return 0xFF;
}

void mbc3_write(Emulator* emu, u16 addr, u8 data)
{
    if(addr <= 0x1FFF)
    {
        // Enable/disable cartridge RAM.
        if(data == 0x0A)
        {
            emu->cram_enable = true;
        }
        else
        {
            emu->cram_enable = false;
        }
        return;
    }
    if(addr >= 0x2000 && addr <= 0x3FFF)
    {
        // Set ROM bank number.
        emu->rom_bank_number = data & 0x7F;
        if(emu->rom_bank_number == 0)
        {
            emu->rom_bank_number = 1;
        }
        return;
    }
    if(addr >= 0x4000 && addr <= 0x5FFF)
    {
        // Set RAM bank number, or map RTC registers.
        emu->ram_bank_number = data;
        return;
    }
    if(addr >= 0x6000 && addr <= 0x7FFF)
    {
        if(is_cart_timer(GetCartridgeHeader(emu->romData)->cartridge_type))
        {
            if(data == 0x01 && emu->rtc.time_latching)
            {
                emu->rtc.latch();
            }
            if(data == 0x00)
            {
                emu->rtc.time_latching = true;
            }
            else
            {
                emu->rtc.time_latching = false;
            }
            return;
        }
    }
    if(addr >= 0xA000 && addr <= 0xBFFF)
    {
        if(emu->ram_bank_number <= 0x03)
        {
            if(emu->cRam)
            {
                if(!emu->cram_enable) return;
                u32 bank_offset = emu->ram_bank_number * 8 * kb;
                assert(bank_offset + (addr - 0xA000) <= emu->cRam_size);
                emu->cRam[bank_offset + (addr - 0xA000)] = data;
                return;
            }
        }
        if(is_cart_timer(GetCartridgeHeader(emu->romData)->cartridge_type) &&
           emu->ram_bank_number >= 0x08 && emu->ram_bank_number <= 0x0C)
        {
            ((u8*)(&emu->rtc.s))[emu->ram_bank_number - 0x08] = data;
            emu->rtc.update_timestamp();
            return;
        }
    }
    ERROR("Unsupported MBC3 cartridge write address: 0x%04X", (u32)addr);
}

u8 CartridgeRead(Emulator *emu, u16 addr) {

    u8 cartridge_type = GetCartridgeHeader(emu->romData)->cartridge_type;
    if(is_cart_mbc1(cartridge_type)) {
        return mbc1_read(emu, addr);
    }
    else if(is_cart_mbc2(cartridge_type)) {
        return mbc2_read(emu, addr);
    }
    else if(is_cart_mbc3(cartridge_type))
    {
        return mbc3_read(emu, addr);
    }
    else {
        if(addr <= 0x7FFF) {
            return emu->romData[addr];
        }
        if(addr >= 0xA000 && addr <= 0xBFFF && emu->cRam) {
            return emu->cRam[addr - 0xA000];
        }
    }

    ERROR("Unsupported cartridge read address: 0x%04X", (u32)addr);
    return 0xFF;
}


void CartridgeWrite(Emulator *emu, u16 addr, u8 data) {
    u8 cartridge_type = GetCartridgeHeader(emu->romData)->cartridge_type;

    if(is_cart_mbc1(cartridge_type)) {
        mbc1_write(emu, addr, data);
        return;
    }
    else if(is_cart_mbc2(cartridge_type))
    {
        mbc2_write(emu, addr, data);
        return;
    }
    else if(is_cart_mbc3(cartridge_type))
    {
        mbc3_write(emu, addr, data);
        return;
    }
    else {
        if(addr >= 0xA000 && addr <= 0xBFFF && emu->cRam)
        {
            emu->cRam[addr - 0xA000] = data;
            return;
        }
    }

    ERROR("Unsupported cartridge write address: 0x%04X", (u32)addr);
}
