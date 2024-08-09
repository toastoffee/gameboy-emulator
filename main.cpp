#include <iostream>
#include "src/cartridge.h"


int main() {

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

    auto cart = GetCartridgeHeader((byte*)source);

    return 0;
}
