#include "shell.h"

#define ANSI_NULL_IS_ZERO if(parsedText=="")parsedText="0"

static std::string UserInput = "";

static std::vector<AnsiData> parsedContent;

// Function to parse ANSI escape codes
// Here are some cases
// 1. \033[n;n;~(m|H|J) <--- DONE!!!!!!!!!!!!!!!!!!!!!!!!
// 2. ^[[n;n;~(m|H|J)
// h l H J B m K
//  \33(B\33[m\33[39;49m\33[K\n\33(B\33[m <--- wtf is this????
void SHELL::parseANSICodes(const std::string &text)
{

	std::string parsedText;
	bool isInCode = false;
	bool zxc = false;
	AnsiData ad;

	for (char c : text)
	{
		if (c == ESC)
		{
			isInCode = true;

			if (parsedText != "")
			{
				ad.type = AnsiDataType::PLAIN_TEXT;
				ad.parameter = parsedText;
				parsedContent.push_back(ad);
				parsedText.clear();
			}
		}
		else if (isInCode == false && zxc == false && c == '^')
		{
			zxc = true;
		}
		else if (isInCode == false && zxc == true && '[')
		{

			zxc = false;
			isInCode = true;

			if (parsedText != "")
			{
				ad.type = AnsiDataType::PLAIN_TEXT;
				ad.parameter = parsedText;
				parsedContent.push_back(ad);
				parsedText.clear();
			}
		}
		else if (isInCode && c == 'm')
		{
			isInCode = false;
			ad.type = AnsiDataType::TEXT_FORMAT;
			ANSI_NULL_IS_ZERO;
			ad.parameter = parsedText;
			parsedContent.push_back(ad);
			parsedText.clear();
		}
		else if (isInCode && c == 'H')
		{
			isInCode = false;
			ad.type = AnsiDataType::CURSOR_CONTROL;
			ANSI_NULL_IS_ZERO;
			ad.parameter = parsedText;
			parsedContent.push_back(ad);
			parsedText.clear();
		}
		else if (isInCode && c == 'J')
		{
			isInCode = false;
			// ad.type = AnsiDataType::ERASE_DISPLAY;
			ad.parameter = "";
			parsedContent.clear();
			parsedText.clear();
		}
		else if (isInCode && c == 'h')
		{
				isInCode = false;
				ad.type = AnsiDataType::PLAIN_TEXT;
				ad.parameter = "";
				parsedContent.push_back(ad);
				parsedText.clear();
			
		}
		else if (isInCode)
		{ // Didn't meet (m|H|J) but ESC
			if (c != '[')
				parsedText += c;
		}
		else
		{
			parsedText += c;
		}
	}

	if (parsedText != "") // push left texts :D
	{
		ad.type = AnsiDataType::PLAIN_TEXT;
		ad.parameter = parsedText;
		parsedContent.push_back(ad);
	}
}

std::vector<std::string> split(std::string inp, char cha)
{
	std::vector<std::string> a;
	std::stringstream b(inp);
	std::string c;

	while (getline(b, c, cha))
	{
		a.push_back(c);
	}

	return a;
}

void SHELL::Print()
{

	std::vector<AnsiData>::iterator iter;
	bool IsPushed = false; // To avoid error imgui popstyle underflow.
	
	for (iter = parsedContent.begin(); iter != parsedContent.end(); iter++)
	{
		AnsiData cont = *iter;

		if (cont.type == AnsiDataType::TEXT_FORMAT)
		{
			std::vector<std::string> aaaaa = split(cont.parameter, ';');

			for (int i = 0; i < aaaaa.size(); i++) {
					 if (aaaaa[i] == "30") { if(IsPushed){ImGui::PopStyleColor();} ImGui::PushStyleColor(ImGuiCol_Text, ANSI_COLOR_BLACK); IsPushed = true; }
				else if (aaaaa[i] == "31") { if(IsPushed){ImGui::PopStyleColor();} ImGui::PushStyleColor(ImGuiCol_Text, ANSI_COLOR_RED); IsPushed = true; }
				else if (aaaaa[i] == "32") { if(IsPushed){ImGui::PopStyleColor();} ImGui::PushStyleColor(ImGuiCol_Text, ANSI_COLOR_GREEN); IsPushed = true; }
				else if (aaaaa[i] == "33") { if(IsPushed){ImGui::PopStyleColor();} ImGui::PushStyleColor(ImGuiCol_Text, ANSI_COLOR_YELLOW); IsPushed = true; }
				else if (aaaaa[i] == "34") { if(IsPushed){ImGui::PopStyleColor();} ImGui::PushStyleColor(ImGuiCol_Text, ANSI_COLOR_BLUE); IsPushed = true; }
				else if (aaaaa[i] == "35") { if(IsPushed){ImGui::PopStyleColor();} ImGui::PushStyleColor(ImGuiCol_Text, ANSI_COLOR_MAGENTA); IsPushed = true; }
				else if (aaaaa[i] == "36") { if(IsPushed){ImGui::PopStyleColor();} ImGui::PushStyleColor(ImGuiCol_Text, ANSI_COLOR_CYAN); IsPushed = true; }
				else if (aaaaa[i] == "37") { if(IsPushed){ImGui::PopStyleColor();} ImGui::PushStyleColor(ImGuiCol_Text, ANSI_COLOR_WHITE); IsPushed = true; }
				else if ((aaaaa[i] == "0") && (IsPushed == true))  { ImGui::PopStyleColor(); IsPushed = false; }
			}

		}
		else if (cont.type == AnsiDataType::PLAIN_TEXT)
		{
			if(IsPushed == true) ImGui::SameLine(); 
			ImGui::Text(u8"%s", cont.parameter.c_str());
		}

		//if(cont.type == AnsiDataType::PLAIN_TEXT) std::cout << "PLAIN_TEXT\t\t" << cont.parameter << std::endl;
		//if(cont.type == AnsiDataType::TEXT_FORMAT) std::cout << "TEXT_FORMAT\t\t" << cont.parameter << std::endl;
		//if(cont.type == AnsiDataType::CURSOR_CONTROL) std::cout << "CURSOR_CONTROL\t\t" << cont.parameter << std::endl;

        
	}
}

// https://gist.github.com/fnky/458719343aabd01cfb17a3a4f7296797#common-private-modes
void SHELL::AddText(std::string data)
{
	if (data != "")
	{
		//std::regex re("\x1b\\[\\?2004(h\?l\?)");
		//std::string a = std::regex_replace(data, re, "");
		parseANSICodes(data);
	}
}

void SHELL::Exec(const char *buf, LIBSSH2_CHANNEL *cn)
{
	int rc = 0;
	char response[4096];
	memset(response, '\0', 4096);

	char temp[2048];
	memset(temp, '\0', 2048);
	sprintf_s(temp, 2048, u8"%s\r\n\0", buf); // idk why, but without "\r\n\0" libssh2_channel_read doesn't work in way I expected.
	int written = 0;

	do
	{
		rc = libssh2_channel_write(cn, temp, strlen(temp));
		written += rc;
	} while (LIBSSH2_ERROR_EAGAIN != rc && rc > 0 && written != strlen(temp));
}

void SHELL::Render(const char *title, LIBSSH2_CHANNEL *cn)
{

	ImGui::Begin(title);

	if (ImGui::BeginChild("scrolling", ImVec2(0, -(ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing())), false, ImGuiWindowFlags_HorizontalScrollbar))
	{
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
		if (parsedContent.size() > 0)
			Print();
		ImGui::PopStyleVar();
	}

	ImGui::EndChild();

	// Need to be changed. Not what I want.
	if (ImGui::InputText(":D  ", &UserInput, ImGuiInputTextFlags_EnterReturnsTrue))
	{
		ImGui::SetItemDefaultFocus();
		ImGui::SetKeyboardFocusHere(-1);
		SHELL::Exec(UserInput.c_str(), cn);
		UserInput.clear();
	}
	ImGui::SameLine();
	if(ImGui::Button("clear")){
		parsedContent.clear();
	}

	ImGui::End();
}