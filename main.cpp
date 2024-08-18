#include <iostream>
#include <memory>
#include <chrono>

#include "src/emulator.h"
#include "src/cartridge.h"
#include "src/log-min.h"

#include "src/debug_window.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>

int main() {

    // load the cartridge data
    std::string dir = "../gb/Tennis (World).gb";
    FILE* file = fopen(dir.c_str(), "rb");
    if(!file) {
        assert(false && "failed to open file.");
    }
    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);
    char* source = new char[fileSize];
    fread(source, sizeof(byte), fileSize, file);
    fclose(file);

    // instantiate the emulator
    std::unique_ptr<Emulator> emulator(new Emulator);
    emulator->Init(source, fileSize);

    auto lastFrame = std::chrono::high_resolution_clock::now();

    DebugWindow debugWindow;


    // GLFW初始化
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    #ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);                         // Mac OS X需要额外执行这句
    #endif

    // 创建窗口
    GLFWwindow* window = glfwCreateWindow(800, 400, "hello", nullptr, nullptr);

    glfwMakeContextCurrent(window);
    // 1:锁60帧, 0:没有帧率上限
    glfwSwapInterval(1);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))                                    // GLAD加载系统相关的OpenGL函数指针地址
    {
        std::cout << "ERROR::GLAD::INITIALIZATION_FAILED" << std::endl;                         // GLAD加载失败报错
    }

    IMGUI_CHECKVERSION();
    ImGui::CreateContext(nullptr);

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    while(!glfwWindowShouldClose(window)) {
        auto frame = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> fp_s = frame - lastFrame;
        lastFrame = frame;

        emulator->Update(fp_s.count());

        glClearColor(0.8f, 0.8f, 0.8f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

//        ImGui::ShowDemoWindow();
        debugWindow.gui();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    return 0;
}
