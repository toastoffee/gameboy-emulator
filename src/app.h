/**
  ******************************************************************************
  * @file           : app.h
  * @author         : toastoffee
  * @brief          : None
  * @attention      : None
  * @date           : 2024/8/18
  ******************************************************************************
  */



#ifndef GAMEBOY_EMULATOR_APP_H
#define GAMEBOY_EMULATOR_APP_H

#include "debug_window.h"
#include "emulator.h"

#include <memory>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>


class App {
public:

    GLFWwindow *_mainWindow;

    DebugWindow _debugWindow;
    Emulator _emulator;

    bool _showOpenCartridgePanel = false;

    std::unique_ptr<Emulator> emulator;

public:
    ~App();

    void Init();
    void Update();
    void RenderLoop();

    void DrawGui();
    void DrawMainMenuBar();

    void DrawOpenCartridgePanel();
    void FileBrowser();
};

#endif //GAMEBOY_EMULATOR_APP_H
