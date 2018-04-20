#include "Editor.h"
#include "../imgui/imgui.h"

void ImGui::Editor::Proccess()
{
//    ImGui::SetNextWindowPos(ImVec2(0, 0));
//    ImGui::SetNextWindowSize(ImVec2(550,200), ImGuiCond_FirstUseEver);
//    ImGui::Begin("Editor", nullptr);

//    ImGui::End();
//    bool demo = true;
//    ImGui::ShowDemoWindow(&demo);

    ImGui::Begin("Debug Menu");
    if(ImGui::Button("AddSpeed"))
    {
        //DispatchSignal(AddSpeed, 10);
    }
    ImGui::End();

//    //ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.65f);  // 2/3 of the space for widget and 1/3 for labels
//    ImGui::PushItemWidth(-140);                               // Right align, keep 140 pixels for labels

//    ImGui::Text("dear imgui says hello. (%s)", IMGUI_VERSION);

//    ImGui::Begin("ImGui Demo");
//    if (ImGui::BeginMenuBar())
//    {
//        if (ImGui::BeginMenu("Help"))
//        {
//            bool a1 = true;
//            bool a2 = true;
//            bool a3 = true;

//            ImGui::MenuItem("Metrics", NULL, &a1);
//            ImGui::MenuItem("Style Editor", NULL, &a2);
//            ImGui::MenuItem("About Dear ImGui", NULL, &a3);
//            ImGui::EndMenu();
//        }
//        ImGui::EndMenuBar();
//    }
//    ImGui::End();

//    ImGui::Begin("Settings", &demo, ImGuiWindowFlags_NoResize);

//    ImGui::Button("File", {100, 30});
//    ImGui::SameLine(100, 0);
//    ImGui::Button("Edit", {100, 30});
//    ImGui::SameLine(200, 0);
//    ImGui::Button("Play", {100, 30});
//    ImGui::End();

//    ImGui::Begin("Choice tree2");

//    static bool open = false;

//    if (ImGui::Button("Stacked modals.."))
//    {
//        open = !open;
//    }

//    if(open)
//    {
//        ImGui::Begin("Hello", &open);
//        ImGui::Text("dsadas");
//        ImGui::End();
//    }

//    ImGui::End();
}
