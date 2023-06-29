#include "explorer.h"

static ExplorerMain em;

void flistUpdate()
{
    em.flist.clear();

    for (const std::filesystem::directory_entry& entry : std::filesystem::directory_iterator(em.currentPath)) {
    ExplorerFileInfo tefi;
    tefi.path = entry.path();
    tefi.size = entry.file_size();
    tefi.ftype = entry.is_directory() ? ExplorerFileType::DIRECTORY : ExplorerFileType::UNKNOWN;
    em.flist.push_back(tefi);
  }
}

void Explorer::Local::Render(const char *title)
{
    ImGui::Begin(title);
    {
        ImGui::BeginChild("arrows", ImVec2(90, 40), false);
        {
            ImGui::Spacing();
            ImGui::Spacing();
            ImGui::ArrowButton("arLeft", ImGuiDir_Left);
            

            ImGui::SameLine(0, 1 * ImGui::GetStyle().ItemSpacing.x);
            ImGui::ArrowButton("arRight", ImGuiDir_Right);


            ImGui::SameLine(0, 1 * ImGui::GetStyle().ItemSpacing.x);
            ImGui::ArrowButton("arUp", ImGuiDir_Up);

            ImGui::EndChild();
        }

        ImGui::SameLine(0, 1 * ImGui::GetStyle().ItemSpacing.x);
        ImGui::BeginChild("drives", {-1, 40}, false);
        {

            ImGui::Spacing();
            ImGui::Spacing();
            if(ImGui::Button("C:/", {0, 0})){
                em.currentPath = "C:/";
                flistUpdate();
            }

            ImGui::EndChild();
        }

        // split(em.currentPath, '/')
        ImGui::BeginChild("splitedPaths", {70, -1}, false);
        {

            if(ImGui::Button("C:/", {0, 0})){
                em.currentPath = "C:/";
                flistUpdate();
            }

            ImGui::EndChild();
        }

        ImGui::SameLine(0, 1 * ImGui::GetStyle().ItemSpacing.x);
        ImGui::BeginChild("filelist", {-1, -1}, false);
        {

            for (int n = 0; n < em.flist.size(); n++)
            {
                ImGui::PushID(n);
                if ((n % 7) != 0)
                    ImGui::SameLine();
                
                ImGui::Button(em.flist.at(n).path.filename().generic_u8string().c_str(), ImVec2(60, 60)); // show detail when click

                // Our buttons are both drag sources and drag targets here!
                if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
                {
                    // Set payload to carry the index of our item (could be anything)
                    ImGui::SetDragDropPayload("flis", &n, sizeof(int));

                    // Display preview (could be anything, e.g. when dragging an image we could decide to display
                    // the filename and a small preview of the image, etc.)
                    { ImGui::Text("Moving %s", em.flist.at(n).path.filename().generic_u8string().c_str()); }
                    ImGui::EndDragDropSource();
                }
                if (ImGui::BeginDragDropTarget())
                {
                    if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("flis"))
                    {
                        IM_ASSERT(payload->DataSize == sizeof(int));
                        int payload_n = *(const int*)payload->Data;
                        {
                            em.flist.at(n) = em.flist.at(payload_n);
                            em.flist.erase(em.flist.begin()+payload_n);
                        }
                    }
                    ImGui::EndDragDropTarget();
                }
                ImGui::PopID();
            }

            ImGui::EndChild();
        }
        ImGui::End();
    }
}