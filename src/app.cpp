/**
  ******************************************************************************
  * @file           : app.cpp
  * @author         : toastoffee
  * @brief          : None
  * @attention      : None
  * @date           : 2024/8/18
  ******************************************************************************
  */

#include <iostream>

#include "app.h"

void App::Init() {
    // GLFW初始化
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);           // Mac OS X需要额外执行这句
#endif

    // 创建窗口
    _mainWindow = glfwCreateWindow(800, 400, "hello", nullptr, nullptr);

    glfwMakeContextCurrent(_mainWindow);
    // 1:锁60帧, 0:没有帧率上限
    glfwSwapInterval(1);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))                   // GLAD加载系统相关的OpenGL函数指针地址
    {
        std::cout << "ERROR::GLAD::INITIALIZATION_FAILED" << std::endl;        // GLAD加载失败报错
    }

    IMGUI_CHECKVERSION();
    ImGui::CreateContext(nullptr);

    ImGui_ImplGlfw_InitForOpenGL(_mainWindow, true);
    ImGui_ImplOpenGL3_Init("#version 330");

}

void App::Update() {

    // Update window events
    glfwPollEvents();

    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // Draw GUI
    DrawGui();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(_mainWindow);
}


void App::RenderLoop() {
    while(!glfwWindowShouldClose(_mainWindow)) {
        Update();
    }
}


App::~App() {
    // TODO
}

void App::DrawGui() {
    // Begin GUI
    ImGui::NewFrame();

    DrawMainMenuBar();

    // End GUI
    ImGui::Render();
}

void App::DrawMainMenuBar() {
    if(ImGui::BeginMainMenuBar())
    {
        if(ImGui::BeginMenu("File"))
        {
            if(ImGui::MenuItem("Open"))
            {
                // TODO...
            }
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }
}