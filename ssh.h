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
	std::string pathToPubKey = "";
	std::string pathToPrivKey = "";
	std::string username = "";
	std::string password = "";
	std::string host = "";
	std::string fingerprint = "";
	std::string userauthlist = "";
	int ssh_socket;
	int port;
	int rc;
	struct sockaddr_in sin;
	WSADATA wsadata;

public:
	bool Set_ssh_info(std::string, std::string, std::string, std::string, std::string, int);
	bool Init_and_establish(); // Use after Set_ssh_info().
};

/*
* path: A path to *.pub key.
* name: Username for login.
* pwd: Password for login.
* ht: Host address.
* pt: Port
*/
bool SSH::Set_ssh_info(std::string PubPath, std::string PrivPath, std::string name, std::string pwd, std::string ht, int pt) {
	this->pathToPubKey = PubPath;
	this->pathToPrivKey = PrivPath;
	this->username = name;
	this->password = pwd;
	this->host = ht;
	this->port = pt;
	return true;
}


bool SSH::Init_and_establish() {

	if (WSAStartup(MAKEWORD(2, 0), &wsadata) != 0) {
		fprintf(stderr, "WSAStartup failed\n");
		return false;
	}

	rc = libssh2_init(0);
	if (rc != 0) {
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

	this->fingerprint = libssh2_hostkey_hash(this->session, LIBSSH2_HOSTKEY_HASH_SHA1);

	fprintf(stderr, "Fingerprint: ");
	for (int i = 0; i < 20; i++) {
		fprintf(stderr, "%02X ", (unsigned char)fingerprint[i]);
	}
	fprintf(stderr, "\n");

	this->userauthlist = libssh2_userauth_list(this->session, this->username.c_str(), strlen(this->username.c_str()));
	if (libssh2_userauth_publickey_fromfile(this->session, this->username.c_str(), this->pathToPubKey.c_str(), this->pathToPrivKey.c_str(), this->password.c_str())) {
		fprintf(stderr, "\tAuthentication by public key failed!\n");
		libssh2_session_disconnect(this->session,
			"Normal Shutdown, Thank you for playing");
		libssh2_session_free(this->session);
	}
	else {
		fprintf(stderr, "\tAuthentication by public key succeeded.\n");
		libssh2_session_disconnect(this->session,
			"Normal Shutdown, Thank you for playing");
		libssh2_session_free(this->session);
	}
}