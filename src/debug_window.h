/**
  ******************************************************************************
  * @file           : debug_window.h
  * @author         : toastoffee
  * @brief          : None
  * @attention      : None
  * @date           : 2024/8/18
  ******************************************************************************
  */



#ifndef GAMEBOY_EMULATOR_DEBUG_WINDOW_H
#define GAMEBOY_EMULATOR_DEBUG_WINDOW_H

#include <imgui/imgui.h>

#include <string>
#include <vector>

#include "singleton_util.h"
#include "type.h"
#include "imgui_pixel_renderer.h"


class Emulator;

class DebugWindow {
public:
    bool show = false;
    std::string cpuLog;
    bool isCpuLogging = false;

    static constexpr int WIDTH = 16 * 8;
    static constexpr int HEIGHT = 24 * 8;

    std::vector<u8> serialData;

    // Tiles inspector
    unsigned char* tileTexData = new unsigned char[WIDTH * HEIGHT * 4];

    ImGuiPixelRenderer pixelRenderer;

    void DrawGui(Emulator* emu);

    void DrawCpuGui(Emulator* emu);

    void DrawSerialGui(Emulator* emu);

    void DrawTilesGui(Emulator* emu);

    void DrawJoypadGui(Emulator* emu);
};


#endif //GAMEBOY_EMULATOR_DEBUG_WINDOW_H
