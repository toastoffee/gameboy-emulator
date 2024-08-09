cmake_minimum_required(VERSION 3.27)
project(gameboy_emulator)

set(CMAKE_CXX_STANDARD 11)

add_executable(gameboy_emulator main.cpp
        src/cartridge.cpp
        src/cartridge.h
)
