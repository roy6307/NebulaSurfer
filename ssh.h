#pragma once

#define WIN32_LEAN_AND_MEAN

#include "imgui/imgui.h"
#include "imgui/imgui_stdlib.h"

#include "libssh2/include/libssh2_config.h"
#include "libssh2/include/libssh2.h"
#include "libssh2/include/libssh2_sftp.h"
#include "libssh2/include/libssh2_publickey.h"

#include <winsock2.h>
#include <windows.h>

#include <string>
#include <regex>



class SSH {
private:
	LIBSSH2_SESSION* session = libssh2_session_init();
	LIBSSH2_CHANNEL* channel;
	std::string fingerprint = "";
	std::string userauthlist = "";
	struct sockaddr_in sin;
	WSADATA wsadata;
	int ssh_socket;
	ImGuiTextBuffer mainContent;
	std::string userInput;

public:
	std::string pathToPubKey = "";
	std::string pathToPrivKey = "";
	std::string username = "";
	std::string password = "";
	std::string host = "";
	int port;
	bool init_session();
	void textAppend(const char*);
	void exec(const char*);
	void Render(const char*);
};

bool SSH::init_session() {

	if (WSAStartup(MAKEWORD(2, 0), &wsadata) != 0) {
		fprintf(stderr, "WSAStartup failed\n");
		return false;
	}

	if (libssh2_init(0) != 0) {
		fprintf(stderr, "libssh2 initialization failed\n");
		return false;
	}

	this->ssh_socket = socket(AF_INET, SOCK_STREAM, 0);
	this->sin.sin_family = AF_INET;
	this->sin.sin_port = htons(this->port);
	this->sin.sin_addr.s_addr = inet_addr(this->host.c_str());

	if (connect(this->ssh_socket, (sockaddr*)(&(this->sin)),
		sizeof(struct sockaddr_in)) != 0) {
		fprintf(stderr, "failed to connect!\n");
		return false;
	}

	this->session = libssh2_session_init();

	if (libssh2_session_handshake(this->session, this->ssh_socket) != 0) {
		fprintf(stderr, "Failure establishing SSH session\n");
		return false;
	}

	this->userauthlist = libssh2_userauth_list(this->session, this->username.c_str(), strlen(this->username.c_str()));
	if (0 != libssh2_userauth_publickey_fromfile(this->session, this->username.c_str(), this->pathToPubKey.c_str(), this->pathToPrivKey.c_str(), this->password.c_str())) {
		fprintf(stderr, "Authentication by public key failed!\n");
		libssh2_session_disconnect(this->session, "");
		libssh2_session_free(this->session);
		return false;
	}
	else {
		fprintf(stderr, "Authentication by public key succeeded.\n");
	}

	this->channel = libssh2_channel_open_session(this->session);

	if (!this->channel) {
		fprintf(stderr, "Unable to open a session.\n");
		libssh2_session_disconnect(this->session, "");
		libssh2_session_free(this->session);
		return false;
	}

	if (libssh2_channel_request_pty(this->channel, "vanilla")) {
		fprintf(stderr, "Failed requesting pty\n");
		libssh2_channel_free(this->channel);
		channel = NULL;
	}

	if (libssh2_channel_shell(this->channel)) {
		fprintf(stderr, "Unable to request shell on allocated pty\n");
		libssh2_session_disconnect(this->session, "");
		libssh2_session_free(this->session);
	}

	this->mainContent.append("-- Succesfully connected!\n");



	char temp[4096];
	memset(temp, '\0', 4096);
	libssh2_channel_read(this->channel, temp, 4096);

	this->mainContent.append(temp);
	memset(temp, '\0', 4096);
	libssh2_channel_read(this->channel, temp, 4096);

	libssh2_session_set_blocking(this->session, 0);

	this->textAppend(temp);



	fprintf(stderr, "-- All done!\n\n");

	return true;
}

void SSH::exec(const char* buf) {
	int rc;
	char response[4096];
	memset(response, '\0', 4096);

	char temp[2048];
	memset(temp, '\0', 2048);
	sprintf_s(temp, 2048, "%s\n", buf);// idk why, but without "\n" libssh2_channel_read doesn't work in way I expected.

	libssh2_session_set_blocking(this->session, 1);

	rc = libssh2_channel_write(channel, temp, strlen(temp)); // write buf + \n to channel

	rc = libssh2_channel_read(channel, response, 4096);	// user input

	libssh2_session_set_blocking(this->session, 0);
}

// Responsible for ANSI Escape Code interpretation and print
void SSH::textAppend(const char* data) {

	//\x1b[?2004h    WTF???? What the fuck is this ANSI escape code?
	// Okay, bracketed paste mode. I think it doesn't need for us.

	char vic[4096];
	const char* idx;

	memset(vic, '\0', 4096);
	memcpy(vic, data, strlen(data));

	do {
		idx = strstr(vic, "\x1b[?2004h");

		if (idx != NULL) {
			char tempCont[4096];
			DWORD_PTR gap = (DWORD_PTR)idx - (DWORD_PTR)vic;
			memset(tempCont, '\0', 4096);

			memcpy(tempCont, (char*)((DWORD_PTR)idx + 8), strlen(vic) - (gap + 9));   // \x1b[?2004h  == 9 characters.

			memset((char*)((DWORD_PTR)vic + gap), '\0', strlen(tempCont)+9);
			strcat_s(vic, tempCont);
		}
	} while (idx != NULL);
	
	this->mainContent.append(vic);
}

void SSH::Render(const char* title) {

	ImGui::Begin(title);

	char received[2048];
	memset(received, '\0', 2048);

	int r = libssh2_channel_read(this->channel, received, 2048);

	if (r > 0) this->textAppend(received);

	if (ImGui::BeginChild("scrolling", ImVec2(0, -(ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing())), false, ImGuiWindowFlags_HorizontalScrollbar))
	{
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
		ImGui::TextUnformatted(this->mainContent.begin());
		ImGui::PopStyleVar();
	}

	ImGui::EndChild();

	if (ImGui::InputText(":D  ", &this->userInput, ImGuiInputTextFlags_EnterReturnsTrue)) {
		this->mainContent.appendf("%s\n", this->userInput.c_str());
		ImGui::SetItemDefaultFocus();
		ImGui::SetKeyboardFocusHere(-1);
		this->exec(this->userInput.c_str());
		this->userInput = "";
	}
	ImGui::SameLine();
	if (ImGui::Button("Clear")) { this->mainContent.clear(); }

	ImGui::End();
}