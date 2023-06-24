//https://www.libssh2.org/mail/libssh2-devel-archive-2012-01/att-0012/SmallSimpleSSH.c

#pragma once

#define WIN32_LEAN_AND_MEAN

#include "imgui/imgui.h"
#include "imgui/imgui_stdlib.h"

#include "libssh2/include/libssh2_config.h"
#include "libssh2/include/libssh2.h"
#include "libssh2/include/libssh2_sftp.h"
#include "libssh2/include/libssh2_publickey.h"

//https://github.com/Tencent/rapidjson/issues/1575#issuecomment-1374766359
#undef min;
#undef max;

#include "rapidjson/filereadstream.h"
#include "rapidjson/document.h"

#include <winsock2.h>
#include <windows.h>

#include <string>
#include <regex>
#include <vector>
#include <iostream>
#include <fstream>

class SSH {
private:
	LIBSSH2_SESSION* session = libssh2_session_init();

	std::string fingerprint = "";
	std::string userauthlist = "";
	struct sockaddr_in sin;
	WSADATA wsadata;
	int ssh_socket;
	rapidjson::Document d;

public:
	LIBSSH2_CHANNEL* channel;
	std::string pathToPubKey = "";
	std::string pathToPrivKey = "";
	std::string username = "";
	std::string password = "";
	std::string host = "";
	std::vector<std::string> cnfList;
	int port;
	bool init_session(const int);
	void exec(const char*);
	void setCnfList();
	void LoadCnf(const char*);
	std::string Read();
};

bool SSH::init_session(const int LOption) {

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

	//https://stackoverflow.com/questions/17227092/how-to-make-send-non-blocking-in-winsock
	u_long mode = 1;
	ioctlsocket(this->ssh_socket, FIONBIO, &mode);

	//this->session = libssh2_session_init(); really need?

	if (libssh2_session_handshake(this->session, this->ssh_socket) != 0) {
		fprintf(stderr, "Failure establishing SSH session\n");
		return false;
	}

	this->userauthlist = libssh2_userauth_list(this->session, this->username.c_str(), strlen(this->username.c_str()));

	if (LOption == 0) {

		int rc = 0;
		while ((rc = libssh2_userauth_password(this->session, this->username.c_str(), this->password.c_str())) == LIBSSH2_ERROR_EAGAIN);
		if (rc) {
			fprintf(stderr, "Authentication by password failed!\n");
			libssh2_session_disconnect(this->session, "");
			libssh2_session_free(this->session);
			return false;
		}

		fprintf(stderr, "Authentication by password succeeded.\n");

	}
	else {

		int rc = 0;
		while ((rc = libssh2_userauth_publickey_fromfile(this->session, this->username.c_str(), this->pathToPubKey.c_str(), this->pathToPrivKey.c_str(), this->password.c_str())) == LIBSSH2_ERROR_EAGAIN);
		if (rc) {
			fprintf(stderr, "Authentication by public key failed!\n");
			libssh2_session_disconnect(this->session, "");
			libssh2_session_free(this->session);
			return false;
		}

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

	libssh2_channel_set_blocking(this->channel, 0);
	libssh2_session_set_blocking(this->session, 0);

	fprintf(stderr, "-- All done!\n\n");

	return true;
}

std::string SSH::Read() {

	char received[2048];
	std::string r = "";
	memset(received, '\0', 2048);

	int rc = 0;

	do {
		rc = libssh2_channel_read(this->channel, received, 2048);
		r.append(received);
		memset(received, '\0', 2048);
	} while (LIBSSH2_ERROR_EAGAIN != rc && rc > 0);

	return r;
}

void SSH::setCnfList() {

	cnfList.clear();

	FILE* fp = NULL;
	fopen_s(&fp, "config.json", "rb");

	char readBuffer[30000];
	rapidjson::FileReadStream is(fp, readBuffer, sizeof(readBuffer));

	this->d.ParseStream(is);

	for (rapidjson::Value::ConstMemberIterator itr = this->d.MemberBegin();
		itr != this->d.MemberEnd(); ++itr)
	{
		this->cnfList.push_back(itr->name.GetString());
	}

	fclose(fp);
}

void SSH::LoadCnf(const char* n) {

	this->pathToPubKey = this->d[n]["pathToPubKey"].GetString();
	this->pathToPrivKey = this->d[n]["pathToPrivKey"].GetString();
	this->username = this->d[n]["username"].GetString();
	this->password = this->d[n]["password"].GetString();
	this->host = this->d[n]["host"].GetString();
	this->port = this->d[n]["port"].GetInt();

}