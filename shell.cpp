#include "shell.h"

static std::string CONTENT;
static std::string UserInput = "";

// Function to parse ANSI escape codes
std::vector<std::pair<std::string, bool>> SHELL::parseANSICodes(const std::string& text) {
	std::string parsedText;
	std::string currentCode;
	std::vector<std::pair<std::string, bool>> t;

	bool isInCode = false;
	for (char c : text) {
		if (c == ESC) {
			isInCode = true;
			currentCode.clear();
		}
		else if (isInCode && c == 'm') {
			isInCode = false;

			if (parsedText != "") t.push_back(std::make_pair(parsedText, false));
			if (currentCode != "") t.push_back(std::make_pair(currentCode, true));

			parsedText.clear();
			currentCode.clear();
		}
		else if (isInCode) {
			if (c != '[') currentCode += c;
		}
		else {
			parsedText += c;
		}
	}

	if (parsedText != "") t.push_back(std::make_pair(parsedText, false));

	return t;
}

std::vector<std::string> split(std::string inp, char cha) {
	std::vector<std::string> a;
	std::stringstream b(inp);
	std::string c;

	while (getline(b, c, cha)) {
		a.push_back(c);
	}

	return a;
}

void SHELL::Print(std::vector<std::pair<std::string, bool>> testingVal) {

	std::vector<std::pair<std::string, bool>>::iterator iter;
	bool colored = false;

	for (iter = testingVal.begin(); iter != testingVal.end(); iter++) {

		std::pair<std::string, bool> cont = *iter;

		if (cont.second == true) { // color

			std::vector<std::string> aaaaa = split(cont.first, ';');

			for (int i = 0; i < aaaaa.size(); i++) {
				if (aaaaa[i] == "30") {
					ImGui::PushStyleColor(ImGuiCol_Text, ANSI_COLOR_BLACK);
					//ImGui::SameLine();
					colored = true;
				}
				else if (aaaaa[i] == "31") {
					ImGui::PushStyleColor(ImGuiCol_Text, ANSI_COLOR_RED);
					//ImGui::SameLine();
					colored = true;
				}
				else if (aaaaa[i] == "32") {
					ImGui::PushStyleColor(ImGuiCol_Text, ANSI_COLOR_GREEN);
					//ImGui::SameLine();
					colored = true;
				}
				else if (aaaaa[i] == "33") {
					ImGui::PushStyleColor(ImGuiCol_Text, ANSI_COLOR_YELLOW);
					//ImGui::SameLine();
					colored = true;
				}
				else if (aaaaa[i] == "34") {
					ImGui::PushStyleColor(ImGuiCol_Text, ANSI_COLOR_BLUE);
					//ImGui::SameLine();
					colored = true;
				}
				else if (aaaaa[i] == "35") {
					ImGui::PushStyleColor(ImGuiCol_Text, ANSI_COLOR_MAGENTA);
					//ImGui::SameLine();
					colored = true;
				}
				else if (aaaaa[i] == "36") {
					ImGui::PushStyleColor(ImGuiCol_Text, ANSI_COLOR_CYAN);
					//ImGui::SameLine();
					colored = true;
				}
				else if (aaaaa[i] == "37") {
					ImGui::PushStyleColor(ImGuiCol_Text, ANSI_COLOR_WHITE);
					//ImGui::SameLine();
					colored = true;
				}
			}

		}
		else { // or

			if (colored == true) { ImGui::Text(u8"%s", cont.first.c_str()); ImGui::SameLine(); ImGui::PopStyleColor(); colored = false; }
			else { ImGui::Text(u8"%s", cont.first.c_str()); }
		}

	}

}

//https://gist.github.com/fnky/458719343aabd01cfb17a3a4f7296797#common-private-modes
void SHELL::AddText(std::string data) {

	if (data != "") {
		std::regex re("\x1b\\[\\?2004(h?l)");
		std::string a = std::regex_replace(data, re, "");
		CONTENT += a;
	}

}

void SHELL::Exec(const char* buf, LIBSSH2_CHANNEL* cn) {
	int rc = 0;
	char response[4096];
	memset(response, '\0', 4096);

	char temp[2048];
	memset(temp, '\0', 2048);
	sprintf_s(temp, 2048, u8"%s\r\n\0", buf);// idk why, but without "\r\n\0" libssh2_channel_read doesn't work in way I expected.
	//fprintf(stderr, "%s\n", temp);

	int written = 0;

	do {
		rc = libssh2_channel_write(cn, temp, strlen(temp));
		written += rc;
	} while (LIBSSH2_ERROR_EAGAIN != rc && rc > 0 && written != strlen(temp));
}

void SHELL::Render(const char* title, LIBSSH2_CHANNEL* cn) {

	ImGui::Begin(title);

	if (ImGui::BeginChild("scrolling", ImVec2(0, -(ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing())), false, ImGuiWindowFlags_HorizontalScrollbar))
	{
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
		if (CONTENT != "") Print(SHELL::parseANSICodes(CONTENT));
		ImGui::PopStyleVar();
	}

	ImGui::EndChild();

	if (ImGui::InputText(":D  ", &UserInput, ImGuiInputTextFlags_EnterReturnsTrue)) {
		ImGui::SetItemDefaultFocus();
		ImGui::SetKeyboardFocusHere(-1);
		SHELL::Exec(UserInput.c_str(), cn);
		UserInput.clear();
	}
	//ImGui::SameLine();
	//if (ImGui::Button("Clear")) { TextBuf.clear(); }

	ImGui::End();
}