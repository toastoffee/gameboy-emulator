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
                                    ${PROJECT_SOURCE_DIR}/third_party)

add_executable(gameboy_emulator main.cpp
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

        glad/src/glad.c

        third_party/imgui/imgui.cpp
        third_party/imgui/imgui_draw.cpp
        third_party/imgui/imgui_tables.cpp
        third_party/imgui/imgui_widgets.cpp
        third_party/imgui/imgui_demo.cpp

        third_party/imgui/backends/imgui_impl_glfw.cpp
        third_party/imgui/backends/imgui_impl_opengl3.cpp

        src/debug_window.cpp
        src/debug_window.h
)

target_link_libraries(
        ${PROJECT_NAME}
        glfw
)