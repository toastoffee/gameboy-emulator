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
#include "emulator.h"

#include <cassert>

inline u8 apply_palette(u8 color, u8 palette)
{
    switch(color)
    {
        case 0: color = palette & 0x03; break;
        case 1: color = ((palette >> 2) & 0x03); break;
        case 2: color = ((palette >> 4) & 0x03); break;
        case 3: color = ((palette >> 6) & 0x03); break;
        default: break;
    }
    return color;
}

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
    memset(pixels, 0, sizeof(pixels));
    current_back_buffer = 0;
}
void PPU::tick(Emulator* emu)
{
    /*TODO*/
    if(!enabled()) return;
    ++line_cycles;
    switch(get_mode())
    {
        case PPUMode::OAM_SCAN:
            tick_oam_scan(emu); break;
        case PPUMode::DRAWING:
            tick_drawing(emu); break;
        case PPUMode::H_BLANK:
            tick_hblank(emu); break;
        case PPUMode::V_BLANK:
            tick_vblank(emu); break;
        default:
            break;
    }
}
u8 PPU::bus_read(u16 addr)
{
    assert(addr >= 0xFF40 && addr <= 0xFF4B);
    return ((u8*)(&lcdc))[addr - 0xFF40];
}
void PPU::bus_write(u16 addr, u8 data)
{
    assert(addr >= 0xFF40 && addr <= 0xFF4B);
    if(addr == 0xFF40 && enabled() && !bitTest(&data, 7))
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

void PPU::tick_oam_scan(Emulator* emu)
{
    // TODO
    if(line_cycles >= 80)
    {
        set_mode(PPUMode::DRAWING);
    }
}

void PPU::tick_drawing(Emulator* emu)
{
    // The fetcher is ticked once per 2 cycles.
    if((line_cycles % 2) == 0)
    {
        switch(fetch_state)
        {
            case PPUFetchState::TILE:
                fetcher_get_tile(emu); break;
            case PPUFetchState::DATA0:
                fetcher_get_data(emu, 0); break;
            case PPUFetchState::DATA1:
                fetcher_get_data(emu, 1); break;
            case PPUFetchState::IDLE:
                fetch_state = PPUFetchState::PUSH; break;
            case PPUFetchState::PUSH:
                fetcher_push_pixels(); break;
            default: break;
        }
        if(draw_x >= PPU_XRES)
        {
            assert(line_cycles >= 252 && line_cycles <= 369);
            set_mode(PPUMode::H_BLANK);
            if(hblank_int_enabled())
            {
                emu->intFlags |= INT_LCD_STAT;
            }
        }
    }
    // LCD driver is ticked once per cycle.
    lcd_draw_pixel();
}

void PPU::tick_hblank(Emulator* emu)
{
    if(line_cycles >= PPU_CYCLES_PER_LINE)
    {
        increase_ly(emu);
        if(ly >= PPU_YRES)
        {
            set_mode(PPUMode::V_BLANK);
            emu->intFlags |= INT_VBLANK;
            if(vblank_int_enabled())
            {
                emu->intFlags |= INT_LCD_STAT;
            }
            current_back_buffer = (current_back_buffer + 1) % 2;
        }
        else
        {
            set_mode(PPUMode::OAM_SCAN);
            if(oam_int_enabled())
            {
                emu->intFlags |= INT_LCD_STAT;
            }
        }
        line_cycles = 0;
    }
}

void PPU::tick_vblank(Emulator* emu)
{
    if(line_cycles >= PPU_CYCLES_PER_LINE)
    {
        increase_ly(emu);
        if(ly >= PPU_LINES_PER_FRAME)
        {
            // move to next frame.
            set_mode(PPUMode::OAM_SCAN);
            ly = 0;
            window_line = 0;
            if(oam_int_enabled())
            {
                emu->intFlags |= INT_LCD_STAT;
            }
        }
        line_cycles = 0;
    }
}

void PPU::increase_ly(Emulator* emu)
{
    if(window_visible() && ly >= wy &&
       (u16)ly < (u16)(wy + PPU_YRES))
    {
        ++window_line;
    }

    ++ly;
    if(ly == lyc)
    {
        set_lyc_flag();
        if(lyc_int_enabled())
        {
            emu->intFlags |= INT_LCD_STAT;
        }
    }
    else
    {
        reset_lyc_flag();
    }
}

void PPU::fetcher_get_tile(Emulator* emu)
{
    if(bg_window_enable())
    {
        if(fetch_window)
        {
            fetcher_get_window_tile(emu);
        }
        else
        {
            fetcher_get_background_tile(emu);
        }
    }
    fetch_state = PPUFetchState::DATA0;
    fetch_x += 8;
}
void PPU::fetcher_get_data(Emulator* emu, u8 data_index)
{
    if(bg_window_enable())
    {
        bgw_fetched_data[data_index] = emu->BusRead(bgw_data_area() + bgw_data_addr_offset + data_index);
    }
    if(data_index == 0) fetch_state = PPUFetchState::DATA1;
    else fetch_state = PPUFetchState::IDLE;
}
void PPU::fetcher_push_pixels()
{
    bool pushed = false;
    if(bgw_queue.size() < 8)
    {
        fetcher_push_bgw_pixels();
        pushed = true;
    }
    if(pushed)
    {
        fetch_state = PPUFetchState::TILE;
    }
}
void PPU::lcd_draw_pixel()
{
    // The LCD driver is drived by BGW queue only, it works when at least 8 pixels are in BGW queue.
    if(bgw_queue.size() >= 8)
    {
        if (draw_x >= PPU_XRES) return;
        BGWPixel bgw_pixel = bgw_queue.front();
        bgw_queue.pop();
        // Calculate background color.
        u8 bg_color = apply_palette(bgw_pixel.color, bgw_pixel.palette);
        // Output pixel.
        switch(bg_color)
        {
            case 0: set_pixel(draw_x, ly, 153, 161, 120, 255); break;
            case 1: set_pixel(draw_x, ly, 87, 93, 67, 255); break;
            case 2: set_pixel(draw_x, ly, 42, 46, 32, 255); break;
            case 3: set_pixel(draw_x, ly, 10, 10, 2, 255); break;
        }
        ++draw_x;
    }
}