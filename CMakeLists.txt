cmake_minimum_required(VERSION 3.27)
project(gameboy_emulator)

set(CMAKE_CXX_STANDARD 11)

# 包含glad头文件
include_directories(glad/include)

# 包含glfw头文件
include_directories(/opt/homebrew/Cellar/glfw/3.4/include)
# 链接glfw库
link_directories(/opt/homebrew/Cellar/glfw/3.4/lib)


include_directories(${PROJECT_NAME} ${PROJECT_SOURCE_DIR}/src
                                    ${PROJECT_SOURCE_DIR}/third_party
                                    ${PROJECT_SOURCE_DIR}/third_party/imgui
                                    ${PROJECT_SOURCE_DIR}/third_party/stbi)

add_executable(gameboy_emulator main.cpp
        third_party/stbi/stb_image.cpp
        glad/src/glad.c

        third_party/imgui/imgui.cpp
        third_party/imgui/imgui_draw.cpp
        third_party/imgui/imgui_tables.cpp
        third_party/imgui/imgui_widgets.cpp
        third_party/imgui/imgui_demo.cpp

        third_party/imgui/backends/imgui_impl_glfw.cpp
        third_party/imgui/backends/imgui_impl_opengl3.cpp

        src/cartridge.cpp
        src/cartridge.h
        src/emulator.cpp
        src/emulator.h
        src/type.h
        src/log-min.cpp
        src/log-min.h
        src/cpu.cpp
        src/cpu.h
        src/instruction.cpp
        src/instruction.h
        src/bit_oper.h
        src/timer.cpp
        src/timer.h
        src/serial.cpp
        src/serial.h
        src/debug_window.cpp
        src/debug_window.h
        src/app.cpp
        src/app.h
        src/ppu.cpp
        src/ppu.h
        src/imgui_pixel_renderer.cpp
        src/imgui_pixel_renderer.h
        src/file_helper.cpp
        src/file_helper.h
        src/joypad.cpp
        src/joypad.h
        src/RTC.cpp
        src/RTC.h
)

target_link_libraries(
        ${PROJECT_NAME}
        glfw
)