/**
  ******************************************************************************
  * @file           : ppu.cpp
  * @author         : toastoffee
  * @brief          : None
  * @attention      : None
  * @date           : 2024/8/19
  ******************************************************************************
  */



#include "ppu.h"


void PPU::init()
{
    lcdc = 0x91;
    lcds = 0;
    scroll_y = 0;
    scroll_x = 0;
    ly = 0;
    lyc = 0;
    dma = 0;
    bgp = 0xFC;
    obp0 = 0xFF;
    obp1 = 0xFF;
    wy = 0;
    wx = 0;
    set_mode(PPUMode::OAM_SCAN);

    line_cycles = 0;
}
void PPU::tick(Emulator* emu)
{
    /*TODO*/
}
u8 PPU::bus_read(u16 addr)
{
    luassert(addr >= 0xFF40 && addr <= 0xFF4B);
    return ((u8*)(&lcdc))[addr - 0xFF40];
}
void PPU::bus_write(u16 addr, u8 data)
{
    luassert(addr >= 0xFF40 && addr <= 0xFF4B);
    if(addr == 0xFF40 && enabled() && !bit_test(&data, 7))
    {
        // Reset mode to HBLANK.
        lcds &= 0x7C;
        // Reset LY.
        ly = 0;
        line_cycles = 0;
    }
    if(addr == 0xFF41) // the lower 3 bits are read only.
    {
        lcds = (lcds & 0x07) | (data & 0xF8);
        return;
    }
    if(addr == 0xFF44) return; // read only.
    ((u8*)(&lcdc))[addr - 0xFF40] = data;
}