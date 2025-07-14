#include "gui.hh"

#include <logging.hh>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <GL/glew.h>

#include <core.hh>

void GUI::newFrame()
{
    ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}

void GUI::renderImGui() 
{
    ImGui::ShowDemoWindow();

    ImGui::Begin("VRAM");
    
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1024, 512, 0, GL_RGBA, GL_UNSIGNED_SHORT_1_5_5_5_REV, core->getGPU()->VRAM);

    glBindTexture(GL_TEXTURE_2D, textureID);

    ImTextureID my_tex_id = (ImTextureID)textureID;
    float my_tex_w = (float)1024;
    float my_tex_h = (float)512;
    ImVec2 uv_min = ImVec2(0.0f, 0.0f);                 // Top-left
    ImVec2 uv_max = ImVec2(1.0f, 1.0f);                 // Lower-right
    ImVec4 tint_col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);   // No tint
    ImVec4 border_col = ImGui::GetStyleColorVec4(ImGuiCol_Border);
    ImGui::Image(my_tex_id, ImVec2(my_tex_w, my_tex_h), uv_min, uv_max, tint_col, border_col);

    ImGui::End();

    // if (ImGui::Begin("Logging")) 
    // {
    //     static bool SystemCheckBoxes[10] = { true, true, true, true, true, true, true, true, true, true };

    //     ImGui::Checkbox("DEFAULT", &SystemCheckBoxes[LOG::SystemLocation::DEFAULT]);
    //     ImGui::SameLine();
    //     ImGui::Checkbox("CPU", &SystemCheckBoxes[LOG::SystemLocation::CPU]);
    //     ImGui::SameLine();
    //     ImGui::Checkbox("GPU", &SystemCheckBoxes[LOG::SystemLocation::GPU]);
    //     ImGui::SameLine();
    //     ImGui::Checkbox("RENDERER", &SystemCheckBoxes[LOG::SystemLocation::RENDERER]);
    //     ImGui::SameLine();
    //     ImGui::Checkbox("APP", &SystemCheckBoxes[LOG::SystemLocation::APP]);
    //     ImGui::SameLine();
    //     ImGui::Checkbox("LOG", &SystemCheckBoxes[LOG::SystemLocation::LOG]);
    //     ImGui::SameLine();
    //     ImGui::Checkbox("MEM", &SystemCheckBoxes[LOG::SystemLocation::MEM]);
    //     ImGui::SameLine();
    //     ImGui::Checkbox("IO", &SystemCheckBoxes[LOG::SystemLocation::IO]);
    //     ImGui::SameLine();
    //     ImGui::Checkbox("DMA", &SystemCheckBoxes[LOG::SystemLocation::DMA]);
    //     ImGui::SameLine();
    //     ImGui::Checkbox("PSX", &SystemCheckBoxes[LOG::SystemLocation::PSX]);

    //     static bool LevelCheckBoxes[10] = { true, true, true, true, true, true, true, true, true, true };

    //     ImGui::Checkbox("DEBUG", &LevelCheckBoxes[LOG::LogLevel::DEBUG]);
    //     ImGui::SameLine();
    //     ImGui::Checkbox("INFO", &LevelCheckBoxes[LOG::LogLevel::INFO]);
    //     ImGui::SameLine();
    //     ImGui::Checkbox("INFO_2", &LevelCheckBoxes[LOG::LogLevel::INFO_2]);
    //     ImGui::SameLine();
    //     ImGui::Checkbox("INFO_3", &LevelCheckBoxes[LOG::LogLevel::INFO_3]);
    //     ImGui::SameLine();
    //     ImGui::Checkbox("INFO_4", &LevelCheckBoxes[LOG::LogLevel::INFO_4]);
    //     ImGui::SameLine();
    //     ImGui::Checkbox("WARN", &LevelCheckBoxes[LOG::LogLevel::WARN]);
    //     ImGui::SameLine();
    //     ImGui::Checkbox("WARN_2", &LevelCheckBoxes[LOG::LogLevel::WARN_2]);
    //     ImGui::SameLine();
    //     ImGui::Checkbox("WARN_3", &LevelCheckBoxes[LOG::LogLevel::WARN_3]);
    //     ImGui::SameLine();
    //     ImGui::Checkbox("ERROR", &LevelCheckBoxes[LOG::LogLevel::ERROR]);
    //     ImGui::SameLine();
    //     ImGui::Checkbox("FATAL", &LevelCheckBoxes[LOG::LogLevel::FATAL]);

    //     ImGui::BeginTable("LOGS", 4, ImGuiTableFlags_BordersH | ImGuiTableFlags_BordersOuterH | ImGuiTableFlags_BordersInnerH | ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersInner | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_Resizable | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV | ImGuiTableFlags_ContextMenuInBody);
    //     ImGui::TableSetupColumn("ID");
    //     ImGui::TableSetupColumn("Severity");
    //     ImGui::TableSetupColumn("Location");
    //     ImGui::TableSetupColumn("Message");
    //     ImGui::TableHeadersRow();

    //     std::vector<LOG::LOG_MESSAGE> logs = LOG::getLogRecords();

    //     // int i = (logs.size() - 100) < 0 ? 0 : logs.size() - 100;
    //     int i = 0;
    //     for (auto x : logs)
    //     {
    //         // auto x = logs[i];

    //         //Check if log passes filter
    //         if (false == SystemCheckBoxes[x.location])
    //             continue;
    //         if (false == LevelCheckBoxes[x.logLevel])
    //             continue;

    //         ImGui::TableNextRow();
    //         ImGui::TableNextColumn();
    //         ImGui::Text("%i", i);
    //         ImGui::TableNextColumn();
    //         ImGui::TextWrapped("%s", x.logLevel_text.c_str());
    //         ImGui::TableNextColumn();
    //         ImGui::TextWrapped("%s", x.location_text.c_str());
    //         ImGui::TableNextColumn();
    //         ImGui::TextWrapped("%s", x.message.c_str());

    //         i++;
    //     }

    //     ImGui::EndTable();
    //     ImGui::End();
    // }

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}