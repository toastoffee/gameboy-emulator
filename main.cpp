#include <iostream>
#include <memory>
#include <chrono>

#include "src/emulator.h"
#include "src/cartridge.h"
#include "src/log-min.h"


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


//    while(!glfwWindowShouldClose(window)) {
//        auto frame = std::chrono::high_resolution_clock::now();
//        std::chrono::duration<double> fp_s = frame - lastFrame;
//        lastFrame = frame;
//
//        emulator->Update(fp_s.count());
//    }

    

    return 0;
}
