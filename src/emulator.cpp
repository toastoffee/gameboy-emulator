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
    Close();
}

void Emulator::Init(const void *cartridgeData, u64 cartridgeDataSize) {
    assert(cartridgeData && cartridgeDataSize && "cartridge data is empty!");

    isCartLoaded = true;

    // copy cartridge data
    romData = (byte*) malloc(cartridgeDataSize);
    romDataSize = cartridgeDataSize;
    memcpy(romData, cartridgeData, cartridgeDataSize);

    // check cartridge data
    CartridgeHeader *header = GetCartridgeHeader(romData);
    u8 checkSum = 0;
    for (u16 addr = 0x0134; addr <= 0x014C; ++addr) {
        checkSum = checkSum - romData[addr] - 1;
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

    // init cpu
    cpu.Init();

    // set the ram
    memset(wRam, 0, 8 * kb);
    memset(vRam, 0, 8 * kb);
    intFlags = 0;
    intEnableFlags = 0;
    timer.Init();
    serial.Init();
    ppu.init();

}

void Emulator::Update(f64 deltaTime) {
    u64 frameCycles = (u64)((f32)(GB_CLOCK_FREQUENCY * deltaTime) * clockSpeedScale );
    u64 endCycles = clockCycles + frameCycles;
    while(clockCycles < endCycles) {
        // step emulator and advance clockCycles
        if(isPaused) break;
        cpu.Step(this);
    }
}

void Emulator::Tick(u32 machineCycles) {
    u32 tickCycles = machineCycles * GB_CLOCK_CYCLES_PER_MACHINE_CYCLE;
    for (u32 i = 0; i < tickCycles; ++i) {
        ++clockCycles;
        timer.Tick(this);

        if((clockCycles % 512) == 0)
        {
            // Serial is ticked at 8192Hz
            serial.Tick(this);
        }
        ppu.tick(this);
    }
}

u8 Emulator::BusRead(u16 addr) {
    if(addr <= 0x7FFF)
    {
        // Cartridge ROM.
        return CartridgeRead(this, addr);
    }
    if(addr <= 0x9FFF)
    {
        // VRAM.
        return vRam[addr - 0x8000];
    }
    if(addr <= 0xBFFF)
    {
        // Cartridge RAM.
        return CartridgeRead(this, addr);
    }
    if(addr <= 0xDFFF)
    {
        // Working RAM.
        return wRam[addr - 0xC000];
    }
    if(addr >= 0xFF01 && addr <= 0xFF02)
    {
        return serial.BusRead(addr);
    }
    if(addr >= 0xFF04 && addr <= 0xFF07)
    {
        return timer.BusRead(addr);
    }
    if(addr == 0xFF0F)
    {
        // IF
        return intFlags | 0xE0;
    }
    if(addr >= 0xFF40 && addr <= 0xFF4B)
    {
        return ppu.bus_read(addr);
    }
    if(addr >= 0xFF80 && addr <= 0xFFFE)
    {
        return hRam[addr - 0xFF80];
    }
    if(addr == 0xFFFF)
    {
        // IE
        return intEnableFlags | 0xE0;
    }

    ERROR("Unsupported bus read address: 0x%04X", (u32)addr);
    return 0xFF;
}

void Emulator::BusWrite(u16 addr, u8 data) {
    if(addr <= 0x7FFF)
    {
        // Cartridge ROM.
        CartridgeWrite(this, addr, data);
        return;
    }
    if(addr <= 0x9FFF)
    {
        // VRAM.
        vRam[addr - 0x8000] = data;
        return;
    }
    if(addr <= 0xBFFF)
    {
        // Cartridge RAM.
        CartridgeWrite(this, addr, data);
        return;
    }
    if(addr <= 0xDFFF)
    {
        // Working RAM.
        wRam[addr - 0xC000] = data;
        return;
    }
    if(addr >= 0xFF01 && addr <= 0xFF02)
    {
        serial.BusWrite(addr, data);
        return;
    }
    if(addr >= 0xFF04 && addr <= 0xFF07)
    {
        timer.BusWrite(addr, data);
        return;
    }
    if(addr == 0xFF0F)
    {
        // IF
        intFlags = data & 0x1F;
        return;
    }
    if(addr >= 0xFF40 && addr <= 0xFF4B)
    {
        ppu.bus_write(addr, data);
        return;
    }
    if(addr >= 0xFF80 && addr <= 0xFFFE)
    {
        // High RAM.
        hRam[addr - 0xFF80] = data;
        return;
    }
    if(addr == 0xFFFF)
    {
        // IE
        intEnableFlags = data & 0x1F;
        return;
    }

    ERROR("Unsupported bus write address: 0x%04X", (u32)addr);
}

void Emulator::Close() {
    if(romData) {
        free(romData);
        romData = nullptr;
        romDataSize = 0;
        isCartLoaded = false;

        INFO("Cartridge Unloaded.");
    }
}
