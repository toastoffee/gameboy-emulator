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
#include <fstream>

Emulator::~Emulator() {
    Close();
}

void Emulator::Init(std::string cartridgePath, const void *cartridgeData, u64 cartridgeDataSize) {
    assert(cartridgeData && cartridgeDataSize && "cartridge data is empty!");

    isCartLoaded = true;

    // copy cartridge data
    this->cartridge_path = cartridgePath;
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

    num_rom_banks = ((u32)32) << header->rom_size / 16;

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
    memset(oam, 0,  160);
    memset(hRam, 0, 128);

    intFlags = 0;
    intEnableFlags = 0;
    timer.Init();
    serial.Init();
    ppu.init();
    joypad.init();

    // init the cartridge ram
    switch(header->ram_size)
    {
        case 2: cRam_size = 8 * kb; break;
        case 3: cRam_size = 32 * kb; break;
        case 4: cRam_size = 128 * kb; break;
        case 5: cRam_size = 64 * kb; break;
        default: break;
    }
    if(cRam_size)
    {
        cRam = (byte *)malloc(cRam_size);
        memset(cRam, 0, cRam_size);
        if(is_cart_battery(header->cartridge_type)) {
            load_cartridge_ram_data();
        }
    }
}

void Emulator::Update(f64 deltaTime) {
    joypad.update(this);
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
    if(addr >= 0xFE00 && addr <= 0xFE9F)
    {
        return oam[addr - 0xFE00];
    }
    if(addr == 0xFF00)
    {
        return joypad.bus_read();
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
    if(addr >= 0xFE00 && addr <= 0xFE9F)
    {
        oam[addr - 0xFE00] = data;
        return;
    }
    if(addr == 0xFF00)
    {
        joypad.bus_write(data);
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
    if(cRam)
    {
        CartridgeHeader* header = GetCartridgeHeader(romData);
        if(is_cart_battery(header->cartridge_type))
        {
            save_cartridge_ram_data();
        }
        free(cRam);
        cRam = nullptr;
        cRam_size = 0;
    }
    if(romData) {
        free(romData);
        romData = nullptr;
        romDataSize = 0;
        isCartLoaded = false;

        INFO("Cartridge Unloaded.");
    }
}

void Emulator::load_cartridge_ram_data() {
    auto save_path = cartridge_path.substr(0, cartridge_path.length() - 2) + "sav";

    FILE* f = fopen(save_path.c_str(), "rb");
    if (f == NULL) {
        WARN("save file not found!");
        return;
    }
    fseek(f, 0, SEEK_END);
    auto fileSize = (size_t)ftell(f);
    if (fileSize == -1) {
        ERROR("save file is empty!");
        return;
    }
    fseek(f, 0, SEEK_SET);

    fread(cRam, 1, cRam_size, f);

    INFO("cartridge RAM data loaded: %s", save_path.c_str());
}

void Emulator::save_cartridge_ram_data() {
    auto save_path = cartridge_path.substr(0, cartridge_path.length() - 2) + "sav";

    std::ofstream saveFile(save_path, std::ios::out | std::ios::binary);
    saveFile.write((const char *)cRam, cRam_size);

    saveFile.close();

    INFO("Save cartridge RAM data to %s.", save_path.c_str());
}

