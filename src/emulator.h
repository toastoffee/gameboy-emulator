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

class Emulator {
private:
    byte* _romData = nullptr;
    u64 _romDataSize = 0;

    bool _isPaused = false;         // is the emulation paused
    f32 _clockSpeedScale = 1.0f;    // clock speed scale value
    u64 _clockCycles = 0;           // the cycle counter
    constexpr static const f32 GB_CLOCK_FREQUENCY = 4194304.f;
    constexpr static const u32 GB_CLOCK_CYCLES_PER_MACHINE_CYCLE = 4;

    CPU _cpu;

    byte _vRam[8 * kb];
    byte _wRam[8 * kb];
    byte _hRam[128];

public:
    ~Emulator();

    void Init(const void* cartridgeData, u64 cartridgeDataSize);

    void Update(f64 deltaTime);

    // advances clock and updates all hardware states (except CPU)
    // This is called from the CPU instructions
    void Tick(u32 machineCycles);

    u8 BusRead(u16 addr);
    void BusWrite(u16 addr, u8 data);
};


#endif //GAMEBOY_EMULATOR_EMULATOR_H
