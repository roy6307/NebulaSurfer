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
	void textPrint(const char*);
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

	this->mainContent.append(temp);

	fprintf(stderr, "-- All done!\n\n");

	return true;
}

void SSH::exec(const char* buf) {
	int rc;
	char response[4096];
	memset(response, '\0', 4096);

	char temp[2048];
	memset(temp, '\0', 2048);
	strcat_s(temp,2048, buf);
	strcat_s(temp,2048 ,"\n"); // idk why, but without "\n" libssh2_channel_read doesn't work in way I expected.

	rc = libssh2_channel_write(channel, temp, strlen(temp));
	//libssh2_channel_send_eof(this->channel);

	rc = libssh2_channel_read(channel, response, 4096);

	if (rc > 0) this->mainContent.appendf("%s\n", response);
	else fprintf(stderr, "Error %d\n", rc);

}

// Responsible for ANSI Escape Code interpretation and print
void SSH::textPrint(const char* buf) {

	//\x1b[?2004h    WTF???? What the fuck is this ANSI escape code?

}

void SSH::Render(const char* title) {

	ImGui::Begin(title);

	if (ImGui::BeginChild("scrolling", ImVec2(0, -(ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing())), false, ImGuiWindowFlags_HorizontalScrollbar))
	{
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
		ImGui::TextUnformatted(this->mainContent.begin()); // I'll use textPrint() func because of asni escape codes.
		ImGui::PopStyleVar();
	}

	ImGui::EndChild();

	if (ImGui::InputText(":D  ", &this->userInput, ImGuiInputTextFlags_EnterReturnsTrue)) {
		this->mainContent.appendf("%s\n",this->userInput.c_str());
		ImGui::SetItemDefaultFocus();
		ImGui::SetKeyboardFocusHere(-1);
		this->exec(this->userInput.c_str());
		this->userInput = "";
	}
	ImGui::SameLine();
	if (ImGui::Button("Clear")) { this->mainContent.clear(); }

	ImGui::End();
}