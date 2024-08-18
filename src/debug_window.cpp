/**
  ******************************************************************************
  * @file           : debug_window.cpp
  * @author         : toastoffee
  * @brief          : None
  * @attention      : None
  * @date           : 2024/8/18
  ******************************************************************************
  */



#include "debug_window.h"

void DebugWindow::DrawGui() {
    if(!show) {
        return;
    }

    ImGui::Begin("Debug Window");
    //! TODO: add debug window gui code

    ImGui::End();
}
