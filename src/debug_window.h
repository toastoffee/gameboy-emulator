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


class Emulator;

class DebugWindow {
public:
    bool show = false;
    std::string cpuLog;
    bool isCpuLogging = false;

    std::vector<u8> serialData;

    void DrawGui(Emulator* emu);

    void DrawCpuGui(Emulator* emu);

    void DrawSerialGui(Emulator* emu);
};


#endif //GAMEBOY_EMULATOR_DEBUG_WINDOW_H
