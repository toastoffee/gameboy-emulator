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
#include "app.h"


void DebugWindow::DrawGui(Emulator* emu) {
    if(!show) {
        return;
    }

    ImGui::Begin("Debug Window");
    //! TODO: add debug window gui code

    DrawCpuGui(emu);

    ImGui::End();
}

void DebugWindow::DrawCpuGui(Emulator* emu) {
    if(emu)
    {
        if(ImGui::CollapsingHeader("CPU Info"))
        {
            auto& cpu = emu->cpu;
            if(ImGui::BeginTable("Byte registers", 6, ImGuiTableFlags_Borders))
            {
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::Text("AF");
                ImGui::TableNextColumn();
                ImGui::Text("%4.4X", (u32)cpu.af());
                ImGui::TableNextColumn();
                ImGui::Text("A");
                ImGui::TableNextColumn();
                ImGui::Text("%2.2X", (u32)cpu.a);
                ImGui::TableNextColumn();
                ImGui::Text("F");
                ImGui::TableNextColumn();
                ImGui::Text("%2.2X", (u32)cpu.f);

                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::Text("BC");
                ImGui::TableNextColumn();
                ImGui::Text("%4.4X", (u32)cpu.bc());
                ImGui::TableNextColumn();
                ImGui::Text("B");
                ImGui::TableNextColumn();
                ImGui::Text("%2.2X", (u32)cpu.b);
                ImGui::TableNextColumn();
                ImGui::Text("C");
                ImGui::TableNextColumn();
                ImGui::Text("%2.2X", (u32)cpu.c);

                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::Text("DE");
                ImGui::TableNextColumn();
                ImGui::Text("%4.4X", (u32)cpu.de());
                ImGui::TableNextColumn();
                ImGui::Text("D");
                ImGui::TableNextColumn();
                ImGui::Text("%2.2X", (u32)cpu.d);
                ImGui::TableNextColumn();
                ImGui::Text("E");
                ImGui::TableNextColumn();
                ImGui::Text("%2.2X", (u32)cpu.e);

                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::Text("HL");
                ImGui::TableNextColumn();
                ImGui::Text("%4.4X", (u32)cpu.hl());
                ImGui::TableNextColumn();
                ImGui::Text("H");
                ImGui::TableNextColumn();
                ImGui::Text("%2.2X", (u32)cpu.h);
                ImGui::TableNextColumn();
                ImGui::Text("L");
                ImGui::TableNextColumn();
                ImGui::Text("%2.2X", (u32)cpu.l);

                ImGui::EndTable();
            }
            if(ImGui::BeginTable("Word registers", 4, ImGuiTableFlags_Borders))
            {
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::Text("PC");
                ImGui::TableNextColumn();
                ImGui::Text("%4.4X", (u32)cpu.pc);
                ImGui::TableNextColumn();
                ImGui::Text("SP");
                ImGui::TableNextColumn();
                ImGui::Text("%4.4X", (u32)cpu.sp);

                ImGui::EndTable();
            }
            ImGui::Text("Flags");
            if(ImGui::BeginTable("Flags", 4, ImGuiTableFlags_Borders))
            {
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::Text("Z");
                ImGui::TableNextColumn();
                ImGui::Text("N");
                ImGui::TableNextColumn();
                ImGui::Text("H");
                ImGui::TableNextColumn();
                ImGui::Text("C");

                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::Text(cpu.fz() ? "1" : "0");
                ImGui::TableNextColumn();
                ImGui::Text(cpu.fn() ? "1" : "0");
                ImGui::TableNextColumn();
                ImGui::Text(cpu.fh() ? "1" : "0");
                ImGui::TableNextColumn();
                ImGui::Text(cpu.fc() ? "1" : "0");

                ImGui::EndTable();
            }
            ImGui::Text("* All register values are represented in hexadecimal.");
            if(cpu.halted)
            {
                ImGui::Text("CPU Halted.");
            }
            ImGui::DragFloat("CPU Speed Scale", &emu->clockSpeedScale, 0.001f);
        }

//        if(ImGui::CollapsingHeader("CPU Stepping"))
//        {
//            bool cpu_stepping_enabled = emu->isPaused;
//            if(!cpu_stepping_enabled)
//            {
//                ImGui::Text("CPU stepping is enabled only when the game is paused.");
//            }
//            else
//            {
//                ImGui::Text("Next instruction: %s", (emu->BusRead(emu->cpu.pc)));
//                if(ImGui::Button("Step CPU"))
//                {
//                    emu->cpu.step(g_app->emulator.get());
//                }
//            }
//        }
    }
}
