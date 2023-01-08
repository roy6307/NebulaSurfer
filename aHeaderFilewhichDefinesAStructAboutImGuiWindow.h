#pragma once

#include "imgui/imgui.h"
#include "imgui/imgui_stdlib.h"

#include "ssh.h"

#include <string>

class SSHW {
private:
	ImGuiTextBuffer mainContent;
	std::string userInput;

public:
	void appendContent(const char* content) {
		mainContent.append(content);
	}

	void Render(const char* title,SSH* ssh) {

		ImGui::Begin(title);

		if (ImGui::BeginChild("scrolling", ImVec2(0, -(ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing())), false, ImGuiWindowFlags_HorizontalScrollbar))
		{
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
			ImGui::Text(this->mainContent.begin());
			ImGui::PopStyleVar();
		}

		ImGui::EndChild();

		if (ImGui::InputText(":D  ", &this->userInput, ImGuiInputTextFlags_EnterReturnsTrue)) {
			appendContent(this->userInput.c_str());
			appendContent("\n");
			ImGui::SetItemDefaultFocus();
			ImGui::SetKeyboardFocusHere(-1);
			ssh->write(this->userInput.c_str());
			this->userInput = "";
		}
		ImGui::SameLine();
		if (ImGui::Button("Clear")) { this->mainContent.clear(); }

		ImGui::End();
	}

};