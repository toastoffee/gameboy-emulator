cmake_minimum_required(VERSION 3.27)
project(gameboy_emulator)

set(CMAKE_CXX_STANDARD 11)

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

        third_party/imgui/imgui.cpp
        third_party/imgui/imgui_draw.cpp
        third_party/imgui/imgui_tables.cpp
        third_party/imgui/imgui_widgets.cpp
)
