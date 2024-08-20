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
#include "log-min.h"

void DebugWindow::DrawGui(Emulator* emu) {
    if(!show) {
        return;
    }

    ImGui::Begin("Debug Window");
    //! TODO: add debug window gui code

    DrawCpuGui(emu);
    DrawSerialGui(emu);

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
                WARN("TODO: log save");
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
            ImGui::TextUnformatted(cpuLog.c_str());
        }
    }
}

void DebugWindow::DrawSerialGui(Emulator *emu) {
    if(emu) {
        // Read serial data.
        while(!emu->serial.outputBuffer.empty()) {
            u8 data = emu->serial.outputBuffer.front();
            emu->serial.outputBuffer.pop();
            serialData.push_back(data);
        }
    }
    if(ImGui::CollapsingHeader("Serial data"))
    {
        ImGui::Text("Serial Data:");
        if(ImGui::BeginChild("Serial Data", ImVec2(500.0f, 100.0f), ImGuiChildFlags_Border | ImGuiChildFlags_AutoResizeX))
        {
            auto beginIter = serialData.begin();
            u8* p1 = beginIter.operator->();
            auto endIter = serialData.end();
            u8* p2 = endIter.operator->();

            ImGui::TextUnformatted((c8*)p1,(c8*)p2);
        }
        ImGui::EndChild();
        if(ImGui::Button("Clear"))
        {
            serialData.clear();
        }
    }
}

void DebugWindow::DrawTilesGui(Emulator *emu) {
    if(emu)
    {
        if(ImGui::CollapsingHeader("Tiles"))
        {
            u32 width = 16 * 8;
            u32 height = 24 * 8;
            // Create texture if not present.
            if(!tile_texture)
            {
                auto tex = g_app->rhi_device->new_texture(RHI::MemoryType::local,
                                                          RHI::TextureDesc::tex2d(RHI::Format::rgba8_unorm, RHI::TextureUsageFlag::copy_dest | RHI::TextureUsageFlag::read_texture,
                                                                                  width, height, 1, 1));
                if(failed(tex))
                {
                    log_error("LunaGB", "Failed to create texture for tile inspector : %s", explain(tex.errcode()));
                    return;
                }
                tile_texture = tex.get();
            }
            // Update texture data.
            usize num_pixel_bytes = width * height * 4;
            usize row_pitch = width * 4;
            Blob pixel_bytes(num_pixel_bytes);
            u8* pixels = pixel_bytes.data();
            for(u32 y = 0; y < height / 8; ++y)
            {
                for(u32 x = 0; x < width / 8; ++x)
                {
                    u32 tile_index = y * width / 8 + x;
                    usize tile_color_begin = y * row_pitch * 8 + x * 8 * 4;
                    for(u32 line = 0; line < 8; ++line)
                    {
                        decode_tile_line(g_app->emulator->vram + tile_index * 16 + line * 2, pixels + tile_color_begin + line * row_pitch);
                    }
                }
            }
            auto r = RHI::copy_resource_data(g_app->cmdbuf, {
                    RHI::CopyResourceData::write_texture(tile_texture, {0, 0}, 0, 0, 0, pixels, row_pitch, num_pixel_bytes, width, height, 1)
            });
            if(failed(r))
            {
                log_error("LunaGB", "Failed to upload texture data for tile inspector : %s", explain(r.errcode()));
                ImGui::End();
                return;
            }
            // Draw.
            ImGui::Image(tile_texture, {(f32)(width * 4), (f32)(height * 4)});
        }
    }
}
