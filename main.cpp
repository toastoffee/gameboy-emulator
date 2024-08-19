#include <iostream>
#include <memory>
#include <chrono>

#include "src/emulator.h"
#include "src/cartridge.h"
#include "src/log-min.h"
#include "app.h"

int main() {

    App app;

    app.Init();

    app.RenderLoop();

    return 0;
}
