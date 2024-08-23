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
#include <fstream>


#include "app.h"


inline void saveRunningImg(const unsigned char* data, int width, int height) {

    std::ofstream binFile("img.ppm", std::ios::out | std::ios::binary);
    const char* header = "P6 160 144 255 ";
    binFile.write(header, strlen(header));

    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            int idx = i * width * 4 + j * 4;
            binFile.write((const char*)data + idx, 3);
        }
    }

    binFile.close();
}

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
    _mainWindow = glfwCreateWindow(960, 600, "Game Boy Emulator", nullptr, nullptr);

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

    ImGui::SetNextWindowPos(ImVec2(0, 20));
    ImGui::SetNextWindowSize(ImVec2(PPU_XRES * 3 + 20, PPU_YRES * 3 + 60));
    ImGui::Begin("GameView", NULL, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoCollapse
                                                    | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);

    if(emulator->romData) {
        renderer.GeneTex((const unsigned char*)emulator->ppu.pixels + ((emulator->ppu.current_back_buffer + 1) % 2) * PPU_XRES * PPU_YRES * 4,
                         PPU_XRES, PPU_YRES, ColorMode::RGBA);
        renderer.Render(PPU_XRES * 3);

        if(ImGui::Button("save screenshot as ppm")) {
            saveRunningImg((const unsigned char*)emulator->ppu.pixels + ((emulator->ppu.current_back_buffer + 1) % 2) * PPU_XRES * PPU_YRES * 4,
                        PPU_XRES, PPU_YRES);
        }
    }

    if(emulator->isCartLoaded) {
        update_emulator_input();
    }

    ImGui::End();

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

    int width = 500;
    int height = 80;
    ImGui::SetNextWindowPos(ImVec2(0, PPU_YRES * 3 + 60));
    ImGui::SetNextWindowSize(ImVec2(width, height), ImGuiCond_Appearing);

    ImGui::Begin("Open Cartridge");

    // all unit tests under (cpu_instrs/individual) passed
    static char cart_path[128] = "../gb/Dr. Mario (World).gb";
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

void App::update_emulator_input() {
    auto& joypad = emulator->joypad;

    if(ImGui::IsWindowFocused()) {
        joypad.up = ImGui::IsKeyDown(ImGui::GetKeyIndex(ImGuiKey_UpArrow))
                || ImGui::IsKeyDown(ImGui::GetKeyIndex(ImGuiKey_W));

        joypad.left = ImGui::IsKeyDown(ImGui::GetKeyIndex(ImGuiKey_LeftArrow))
                    || ImGui::IsKeyDown(ImGui::GetKeyIndex(ImGuiKey_A));

        joypad.down = ImGui::IsKeyDown(ImGui::GetKeyIndex(ImGuiKey_DownArrow))
                    || ImGui::IsKeyDown(ImGui::GetKeyIndex(ImGuiKey_S));

        joypad.right = ImGui::IsKeyDown(ImGui::GetKeyIndex(ImGuiKey_RightArrow))
                    || ImGui::IsKeyDown(ImGui::GetKeyIndex(ImGuiKey_D));

        joypad.a = ImGui::IsKeyDown(ImGui::GetKeyIndex(ImGuiKey_J));

        joypad.b = ImGui::IsKeyDown(ImGui::GetKeyIndex(ImGuiKey_K));

        joypad.select = ImGui::IsKeyDown(ImGui::GetKeyIndex(ImGuiKey_Space));

        joypad.start = ImGui::IsKeyDown(ImGui::GetKeyIndex(ImGuiKey_Enter));

    }
}

