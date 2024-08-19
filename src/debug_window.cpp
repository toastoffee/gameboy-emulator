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
#include "instruction.h"

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

        if(ImGui::CollapsingHeader("CPU Stepping"))
        {
            bool cpu_stepping_enabled = emu->isPaused;
            if(!cpu_stepping_enabled)
            {
                ImGui::Text("CPU stepping is enabled only when the game is paused.");
            }
            else
            {
                ImGui::Text("Next instruction: %s", GetOpcodeName(emu->BusRead(emu->cpu.pc)));
                if(ImGui::Button("Step CPU"))
                {
                    emu->cpu.Step(emu);
                }
            }
        }

        if(ImGui::CollapsingHeader("CPU Logging"))
        {
            if(ImGui::Button("Clear"))
            {
                cpuLog.clear();
            }
            if(isCpuLogging)
            {
                if(ImGui::Button("Stop logging"))
                {
                    isCpuLogging = false;
                }
            }
            else
            {
                if(ImGui::Button("Start logging"))
                {
                    isCpuLogging = true;
                }
            }
            if(ImGui::Button("Save"))
            {
//                Window::FileDialogFilter filter;
//                filter.name = "Text file";
//                const c8* extension = "txt";
//                filter.extensions = {&extension, 1};
//                auto rpath = Window::save_file_dialog("Save", {&filter, 1});
//                if (succeeded(rpath) && !rpath.get().empty())
//                {
//                    if(rpath.get().extension() == Name())
//                    {
//                        rpath.get().replace_extension("txt");
//                    }
//                    auto f = open_file(rpath.get().encode().c_str(), FileOpenFlag::write, FileCreationMode::create_always);
//                    if(succeeded(f))
//                    {
//                        auto _ = f.get()->write(cpu_log.c_str(), cpu_log.size(), nullptr);
//                    }
//                }
            }
            ImGui::Text("Log size: %llu bytes.", (u64)cpuLog.size());
        }
    }
}
