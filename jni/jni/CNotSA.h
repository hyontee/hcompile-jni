// new 21.06.2025
#include "main.h"
#include "game/common.h"
#include "gui/gui.h"
#include "vendor/imgui/imgui_internal.h"
namespace notsa {
    namespace ui {
        inline void Render() {
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
            ImGui::Begin("###LR", nullptr,
                         ImGuiWindowFlags_NoTitleBar |
                         ImGuiWindowFlags_NoResize |
                         ImGuiWindowFlags_NoMove |
                         ImGuiWindowFlags_NoScrollbar |
                         ImGuiWindowFlags_NoSavedSettings |
                         ImGuiWindowFlags_NoBackground);
            ImGui::SetWindowPos(ImVec2(0, 0));
            ImGui::SetWindowSize(ImVec2(ImGui::GetIO().DisplaySize.x, 37));

            ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.2f, 0.2f, 0.2f, 1.0f));
            ImGui::BeginChild("BarContent", ImVec2(0, 0), false, ImGuiWindowFlags_NoScrollbar);

            auto textColor = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
            ImGui::PushStyleColor(ImGuiCol_Text, textColor);
            ImGui::SetCursorPos(ImVec2(10, 5));
            ImGui::Text("Visualization Extra");

            ImGui::PopStyleColor();
            ImGui::EndChild();
            ImGui::PopStyleColor();
            ImGui::End();
            ImGui::PopStyleVar();
        }

        template<typename R, typename T>
        inline void DoNestedMenu(R&& menuPath, T OnAllVisibleFn) {
            assert(menuPath.size() > 0);
            int32 nopen{};
            for (auto name : menuPath) {
                if (!ImGui::BeginMenu(name)) {
                    break;
                }
                nopen++;
            }
            if (nopen == menuPath.size()) {
                OnAllVisibleFn();
            }

            while (nopen--) {
                ImGui::EndMenu();
            }
        }

        //! Initializer list version of `DoNestedMenu` (So no ugly `std::to_array` has to be used)
        template<typename T>
        inline void DoNestedMenuIL(std::initializer_list<const char*> menuPath, T OnAllVisibleFn) {
            DoNestedMenu(menuPath, OnAllVisibleFn);
        }
    }; // namespace ui
}; // namespace notsa