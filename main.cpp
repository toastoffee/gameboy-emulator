#include <iostream>
#include <memory>
#include <chrono>

#include "src/emulator.h"
#include "src/cartridge.h"
#include "src/log-min.h"
#include "app.h"

int main() {

    App::GetInstance()->Init();

    App::GetInstance()->RenderLoop();

    return 0;
}
