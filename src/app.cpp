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
#include <filesystem>


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
    _mainWindow = glfwCreateWindow(800, 400, "Game Boy Emulator", nullptr, nullptr);

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

    emulator = std::unique_ptr<Emulator>(new Emulator);
}

void App::Update() {


    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();

    // Draw GUI
    DrawGui();


    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    // Update window events
    glfwPollEvents();
    glfwSwapBuffers(_mainWindow);

}


void App::RenderLoop() {

    auto lastFrame = std::chrono::high_resolution_clock::now();

    while(!glfwWindowShouldClose(_mainWindow)) {
        Update();

        if(emulator->isCartLoaded) {
            auto frame = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> fp_s = frame - lastFrame;
            lastFrame = frame;

            emulator->Update(fp_s.count());
        }
    }
}


App::~App() {
    // TODO
}

void App::DrawGui() {
    // Begin GUI
    ImGui::NewFrame();

    DrawMainMenuBar();
    _debugWindow.DrawGui(emulator.get());
    DrawOpenCartridgePanel();

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
                _showOpenCartridgePanel = true;
            }
            if(ImGui::MenuItem("Close"))
            {
                emulator->Close();
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Play"))
        {
            if(ImGui::MenuItem("Play"))
            {
                if(emulator)
                {
                    emulator->isPaused = false;
                }
            }
            if(ImGui::MenuItem("Pause"))
            {
                if(emulator)
                {
                    emulator->isPaused = true;
                }
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Debug"))
        {
            if(ImGui::MenuItem("Debug Window"))
            {
                _debugWindow.show = true;
            }
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }
}

void App::DrawOpenCartridgePanel() {
    if(!_showOpenCartridgePanel) {
        return;
    }

    int width = 600;
    int height = 80;
    ImGui::SetNextWindowSize(ImVec2(width, height), ImGuiCond_Appearing);

    ImGui::Begin("Open Cartridge");

    // all unit tests under (cpu_instrs/individual) passed
    static char cart_path[128] = "../gb-test-rom/cpu_instrs/individual/11-op a,(hl).gb";;
    ImGui::InputText("Cartridge Path", cart_path, IM_ARRAYSIZE(cart_path));

    if(ImGui::Button("Confirm")) {
        _showOpenCartridgePanel = false;

        // load the cartridge data
        FILE* file = fopen(cart_path, "rb");
        if(!file) {
            assert(false && "failed to open file.");
        }
        fseek(file, 0, SEEK_END);
        long fileSize = ftell(file);
        fseek(file, 0, SEEK_SET);
        char* source = new char[fileSize];
        fread(source, sizeof(byte), fileSize, file);
        fclose(file);

        emulator->Init(source, fileSize);
    }
    ImGui::SameLine();
    if(ImGui::Button("Confirm without playing")) {
        _showOpenCartridgePanel = false;

        // load the cartridge data
        FILE* file = fopen(cart_path, "rb");
        if(!file) {
            assert(false && "failed to open file.");
        }
        fseek(file, 0, SEEK_END);
        long fileSize = ftell(file);
        fseek(file, 0, SEEK_SET);
        char* source = new char[fileSize];
        fread(source, sizeof(byte), fileSize, file);
        fclose(file);

        emulator->Init(source, fileSize);
        if(emulator) {
            emulator->isPaused = true;
        }
    }
    ImGui::SameLine();
    if(ImGui::Button("Cancel")) {
        _showOpenCartridgePanel = false;
    }

    ImGui::End();
}


void App::FileBrowser() {
//    if(!_showFileBrowser) {
//        return;
//    }
//
//    ImGui::Begin("File Browser");
//
//    ImGui::End();
}

