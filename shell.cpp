#include "shell.h"

#include <iostream>
#include <sstream>

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

			// Process the parsed code
			// Here, we are just printing the code as an example
#if _DEBUG
			std::cout << "Parsed text: " << parsedText.length() << std::endl;
			std::cout << "Found ANSI code: " << currentCode << std::endl;
#endif

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

	// Output the parsed text
#ifdef _DEBUG
	//std::cout << "Parsed text: " << parsedText << std::endl;
#endif
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

void TestFunc(std::vector<std::pair<std::string, bool>> testingVal) {

	std::vector<std::pair<std::string, bool>>::iterator iter;
	bool colored = false;

	for (iter = testingVal.begin(); iter != testingVal.end(); iter++) {

		std::pair<std::string, bool> cont = *iter;

		if (cont.second == true) { // color
			//if (cont.first == "0") ImGui::PopStyleColor();
			if (false) 1 + 1;
			else {
				std::vector<std::string> aaaaa = split(cont.first, ';');

				for (int i = 0; i < aaaaa.size(); i++) {
					if (aaaaa[i] == "30") {
						ImGui::PushStyleColor(ImGuiCol_Text, ANSI_COLOR_BLACK);
						colored = true;
					}
					else if (aaaaa[i] == "31") {
						ImGui::PushStyleColor(ImGuiCol_Text, ANSI_COLOR_RED);
						colored = true;
					}
					else if (aaaaa[i] == "32") {
						ImGui::PushStyleColor(ImGuiCol_Text, ANSI_COLOR_GREEN);
						colored = true;
					}
					else if (aaaaa[i] == "33") {
						ImGui::PushStyleColor(ImGuiCol_Text, ANSI_COLOR_YELLOW);
						colored = true;
					}
					else if (aaaaa[i] == "34") {
						ImGui::PushStyleColor(ImGuiCol_Text, ANSI_COLOR_BLUE);
						colored = true;
					}
					else if (aaaaa[i] == "35") {
						ImGui::PushStyleColor(ImGuiCol_Text, ANSI_COLOR_MAGENTA);
						colored = true;
					}
					else if (aaaaa[i] == "36") {
						ImGui::PushStyleColor(ImGuiCol_Text, ANSI_COLOR_CYAN);
						colored = true;
					}
					else if (aaaaa[i] == "37") {
						ImGui::PushStyleColor(ImGuiCol_Text, ANSI_COLOR_WHITE);
						colored = true;
					}
				}
			}
		}
		else { // or
			ImGui::Text(u8"%s", cont.first.c_str());
			if (colored == true) { ImGui::PopStyleColor(); colored = false; }
		}

	}

}

void SHELL::AddText(std::string data) {

	if (data != "") {
		std::regex re("\x1b\\[\\?2004h");
		std::string a = std::regex_replace(data, re, "");
		CONTENT += a;
	}

}

void SHELL::Print() {

#ifdef _DEBUG
	if (CONTENT != "") TestFunc(SHELL::parseANSICodes(CONTENT));

#endif

	/*if (!TextBuf.empty()) {

		std::string s(TextBuf.Buf.Data);

		std::regex re("(\x1b)(\\[(\\d+|\\d+;\\d+|\\d+;\\d+;\\d+)m)");
		std::string ms = std::regex_replace(s, re, "");
		ImGui::Text(u8"%s", ms.c_str());

	}*/

}





void SHELL::Exec(const char* buf, LIBSSH2_CHANNEL* cn) {
	int rc = 0;
	char response[4096];
	memset(response, '\0', 4096);

	char temp[2048];
	memset(temp, '\0', 2048);
	sprintf_s(temp, 2048, u8"%s\r\n\0", buf);// idk why, but without "\r\n\0" libssh2_channel_read doesn't work in way I expected.
	fprintf(stderr, "%s\n", temp);

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
		SHELL::Print();
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