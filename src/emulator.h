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
#include "cpu.h"
#include "timer.h"
#include "serial.h"
#include "ppu.h"
#include "joypad.h"

#include <string>

class Emulator {
public:
    std::string cartridge_path;

    byte* romData = nullptr;
    u64 romDataSize = 0;

    //! The cartridge RAM.
    byte* cRam = nullptr;
    //! The cartridge RAM size.
    u64 cRam_size = 0;

    //! The number of ROM banks. 16KB per bank.
    u32 num_rom_banks = 0;
    //! MBC1: The cartridge RAM is enabled for reading / writing.
    bool cram_enable = false;
    //! MBC1: The ROM bank number controlling which rom bank is mapped to 0x4000~0x7FFF.
    u8 rom_bank_number = 1;
    //! MBC1: The RAM bank number register controlling which ram bank is mapped to 0xA000~0xBFFF.
    //! If the cartridge ROM size is larger than 512KB (32 banks), this is used to control the
    //! high 2 bits of rom bank number, enabling the game to use at most 2MB of ROM data.
    u8 ram_bank_number = 0;
    //! MBC1: The banking mode.
    //! 0: 0000–3FFF and A000–BFFF are locked to bank 0 of ROM and SRAM respectively.
    //! 1: 0000–3FFF and A000-BFFF can be bank-switched via the 4000–5FFF register.
    u8 banking_mode = 0;

    bool isPaused = false;         // is the emulation paused
    f32 clockSpeedScale = 1.0f;    // clock speed scale value
    u64 clockCycles = 0;           // the cycle counter
    constexpr static const f32 GB_CLOCK_FREQUENCY = 4194304.f;
    constexpr static const u32 GB_CLOCK_CYCLES_PER_MACHINE_CYCLE = 4;

    CPU cpu;

    byte vRam[8 * kb];  // visual ram
    byte wRam[8 * kb];  // working ram
    byte hRam[128];     // high ram
    byte oam[160];

    //! 0xFF0F - The interruption flags
    u8 intFlags;
    //! 0xFFFF - The interruption enabling flags
    u8 intEnableFlags;

    Timer timer;
    Serial serial;
    PPU ppu;
    Joypad joypad;

    bool isCartLoaded = false;

public:
    ~Emulator();

    void Init(std::string cartridgePath, const void* cartridgeData, u64 cartridgeDataSize);
    void Close();

    void Update(f64 deltaTime);

    // advances clock and updates all hardware states (except CPU)
    // This is called from the CPU instructions
    void Tick(u32 machineCycles);

    u8 BusRead(u16 addr);
    void BusWrite(u16 addr, u8 data);
    void load_cartridge_ram_data();
    void save_cartridge_ram_data();

};

constexpr u8 INT_VBLANK = 1;
constexpr u8 INT_LCD_STAT = 2;
constexpr u8 INT_TIMER = 4;
constexpr u8 INT_SERIAL = 8;
constexpr u8 INT_JOYPAD = 16;

#endif //GAMEBOY_EMULATOR_EMULATOR_H
