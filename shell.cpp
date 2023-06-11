#include "shell.h"

#include <iostream>

static ImGuiTextBuffer TextBuf;
static std::string UserInput="";

// Function to parse ANSI escape codes
void SHELL::parseANSICodes(const std::string& text) {
	std::string parsedText;
	std::string currentCode;

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
			std::cout << "Found ANSI code: " << currentCode << std::endl;
			currentCode.clear();
		}
		else if (isInCode) {
			currentCode += c;
		}
		else {
			parsedText += c;
		}
	}

	// Output the parsed text
	std::cout << "Parsed text: " << parsedText << std::endl;
}

/*
*
*  Fucking Bug Is Here.
*  I Don't Know Why.
*  SSIBAL.
* 
*/
void SHELL::AddText(std::string data) {

#ifdef _DEBUG
	if(data != "") std::cout << "HIT" << std::endl;
#endif

	if (data != "") {
		std::regex re("\x1b\\[\\?2004h");
		std::string a = std::regex_replace(data, re, "");
		TextBuf.append(a.c_str());
	}

}

void SHELL::Print() {

	if (TextBuf.empty()) {

		std::string s(TextBuf.Buf.Data);

		std::regex re("(\x1b)(\\[(\\d+|\\d+;\\d+|\\d+;\\d+;\\d+)m)");
		std::string ms = std::regex_replace(s, re, "");
		ImGui::Text(u8"%s", ms.c_str());

	}

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
	ImGui::SameLine();
	if (ImGui::Button("Clear")) { TextBuf.clear(); }

	ImGui::End();
}