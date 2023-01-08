#pragma once

#define WIN32_LEAN_AND_MEAN

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

public:
	std::string pathToPubKey = "";
	std::string pathToPrivKey = "";
	std::string username = "";
	std::string password = "";
	std::string host = "";
	int port;
	bool get_channel();
	long long read(char*);
	long long read_stderr(char*);
	long long write(const char*);
	long long write(char*, int);
	long long write_stderr(char*, int);
};



bool SSH::get_channel() {

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

	/* At this point the shell can be interacted with using
	* libssh2_channel_read()
	* libssh2_channel_read_stderr()
	* libssh2_channel_write()
	* libssh2_channel_write_stderr()
	*
	* Blocking mode may be (en|dis)abled with: libssh2_channel_set_blocking()
	* If the server send EOF, libssh2_channel_eof() will return non-0
	* To send EOF to the server use: libssh2_channel_send_eof()
	* A channel can be closed with: libssh2_channel_close()
	* A channel can be freed with: libssh2_channel_free()
	*/

	fprintf(stderr, "-- All done!\n\n");

	return true;
}

long long SSH::read(char* res) {
	libssh2_channel_set_blocking(this->channel, 0);

	int rc = libssh2_channel_read(this->channel, res, 32760);
	
	libssh2_channel_set_blocking(this->channel, 1);

	return rc;
}

long long SSH::read_stderr(char* res) {
	libssh2_channel_set_blocking(this->channel, 0);

	int rc = libssh2_channel_read_stderr(this->channel, res, 32760);

	libssh2_channel_set_blocking(this->channel, 1);

	return rc;
}

long long SSH::write(const char* buf) {
	bool ended = false;
	int idx = 0;
	do {
		if (buf[idx] == '\0') ended = true; // Let's find the end~
		else idx -= -1; // Beautiful code
	} while (ended); 

	int rc = libssh2_channel_write(this->channel, buf, idx);
	return rc;
}

long long SSH::write(char* buf, int bufSize) {
	int rc = libssh2_channel_write(this->channel, buf, bufSize);
	return rc;
}

long long SSH::write_stderr(char* buf, int bufSize) {
	int rc = libssh2_channel_write_stderr(this->channel, buf, bufSize);
	return rc;
}