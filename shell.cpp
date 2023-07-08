#include "shell.h"

#define ANSI_NULL_IS_ZERO if(parsedText=="")parsedText="0"

static std::string UserInput = "";

static std::vector<AnsiData> parsedContent;

// Function to parse ANSI escape codes
// Here are some cases
// 1. \033[n;n;~(m|H|J) <--- DONE!!!!!!!!!!!!!!!!!!!!!!!!
// 2. ^[[n;n;~(m|H|J)
// h l H J B m K   |||  m H J h  |||  l B K
//  \33(B\33[m\33[39;49m\33[K\n\33(B\33[m <--- wtf is this????
// '\xf' << WTF is this tooooo????????
// Need to handle CSI n H
void NebulaSurfer::SHELL::parseANSICodes(const std::string &ctext)
{

	std::string parsedText;
	bool isInCode = false;
	bool zxc = false;
	AnsiData ad;

	std::regex re("(\\r)|\xf");
	std::string text = std::regex_replace(ctext, re, "");

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
		else if (isInCode && ((c == 'l') || (c == 'B') || (c == 'K')))
		{
				isInCode = false;
				ad.type = AnsiDataType::PLAIN_TEXT;
				ad.parameter = "";
				//parsedContent.push_back(ad);
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

void NebulaSurfer::SHELL::Print()
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
		else if (cont.type == AnsiDataType::CURSOR_CONTROL)
		{
			if (cont.parameter == "0")
			{
				0+0;
			}
		}
		else if (cont.type == AnsiDataType::PLAIN_TEXT)
		{
			if(IsPushed == true) ImGui::SameLine(); // New method need. this is a shitty way.
			ImGui::Text(u8"%s", cont.parameter.c_str());
			//ImVec2 tempvec2 = ImGui::GetCursorPos();
			//std::cout << "X: " << tempvec2.x << "\tY: " << tempvec2.y << std::endl;
		}
	}
}

void NebulaSurfer::SHELL::Render(const char *title)
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
		NebulaSurfer::Network::SSH::Exec(UserInput.c_str());
		UserInput.clear();
	}
	ImGui::SameLine();
	if(ImGui::Button("clear")){
		parsedContent.clear();
	}

	ImGui::End();
}