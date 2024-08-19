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

class Emulator;

class DebugWindow {
public:
    bool show = false;

    void DrawGui(Emulator* emu);

    void DrawCpuGui(Emulator* emu);
};


#endif //GAMEBOY_EMULATOR_DEBUG_WINDOW_H
