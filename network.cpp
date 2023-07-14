#include "network.h"

static LIBSSH2_SESSION *session;
static LIBSSH2_CHANNEL *channel;
static LIBSSH2_SFTP *sftp_session;
static bool networkInitialized = false;

bool NebulaSurfer::Network::init(const char *hostaddr, int port, const char *username, const char *password, const char *pathToPem)
{

    WSADATA wsadata;
    sockaddr_in sin;
    SOCKET sock;

    session = libssh2_session_init();

    if (WSAStartup(MAKEWORD(2, 0), &wsadata) != 0)
    {
        fprintf(stderr, "WSAStartup failed\n");
        return false;
    }

    if (libssh2_init(0) != 0)
    {
        fprintf(stderr, "libssh2 initialization failed\n");
        return false;
    }

    sock = socket(AF_INET, SOCK_STREAM, 0);
    sin.sin_family = AF_INET;
    sin.sin_port = htons(port);
    sin.sin_addr.s_addr = inet_addr(hostaddr);

    if (connect(sock, (sockaddr *)(&(sin)), sizeof(struct sockaddr_in)) != 0)
    {
        fprintf(stderr, "failed to connect!\n");
        return false;
    }

    // https://stackoverflow.com/questions/17227092/how-to-make-send-non-blocking-in-winsock
    u_long mode = 1;
    ioctlsocket(sock, FIONBIO, &mode);

    //  really need?

    if (libssh2_session_handshake(session, sock) != 0)
    {
        fprintf(stderr, "Failure establishing SSH session\n");
        return false;
    }

    // only password
    if (strcmp(pathToPem, "") == 0)
    {

        int rc = 0;

        while ((rc = libssh2_userauth_password(session, username, password)) == LIBSSH2_ERROR_EAGAIN)
            ;

        if (rc)
        {
            fprintf(stderr, "Authentication by password failed!\n");
            libssh2_session_disconnect(session, "");
            libssh2_session_free(session);
            return false;
        }

        fprintf(stderr, "Authentication by password succeeded.\n");
    }
    else
    {

        int rc = 0;

        // need openssl for 3rd parameter.
        while ((rc = libssh2_userauth_publickey_fromfile(session, username, NULL, pathToPem, password)) == LIBSSH2_ERROR_EAGAIN)
            ;

        if (rc)
        {
            fprintf(stderr, "Authentication by public key failed!\n");
            libssh2_session_disconnect(session, "");
            libssh2_session_free(session);
            return false;
        }

        fprintf(stderr, "Authentication by public key succeeded.\n");
    }

    channel = libssh2_channel_open_session(session);

    if (!channel)
    {
        fprintf(stderr, "Unable to open a session.\n");
        libssh2_session_disconnect(session, "");
        libssh2_session_free(session);
        return false;
    }

    if (libssh2_channel_request_pty(channel, "vt100"))
    {
        fprintf(stderr, "Failed requesting pty\n");
        libssh2_channel_free(channel);
        channel = NULL;
    }

    if (libssh2_channel_shell(channel))
    {
        fprintf(stderr, "Unable to request shell on allocated pty\n");
        libssh2_session_disconnect(session, "");
        libssh2_session_free(session);
    }

    sftp_session = libssh2_sftp_init(session);

    if (!sftp_session)
    {
        fprintf(stderr, "Unable to init SFTP session\n");
        libssh2_session_disconnect(session, "");
        libssh2_session_free(session);
        return false;
    }
    else
    {
        fprintf(stderr, "Successfully init SFTP session\n");
    }

    libssh2_channel_set_blocking(channel, 0);

    networkInitialized = true;

    return true;
}

std::string NebulaSurfer::Network::SSH::Read()
{

    std::string r = "";
    char buffer[1024];
    ssize_t nbytes;

    do
    {
        nbytes = libssh2_channel_read(channel, buffer, sizeof(buffer) - 1);
        if (nbytes > 0)
        {
            buffer[nbytes] = '\0';
            r += buffer;
        }
    } while (LIBSSH2_ERROR_EAGAIN != nbytes && nbytes > 0);

    return (r != "") ? r : "";
}

void NebulaSurfer::Network::SSH::Exec(const char *buf)
{
    ssize_t rc = 0;
    char *temp = new char[strlen(buf) + 3];
    memset(temp, '\0', strlen(buf) + 3);
    sprintf_s(temp, strlen(buf) + 3, u8"%s\r\n\0", buf); // idk why, but without "\r\n\0" libssh2_channel_read doesn't work in way I expected.
    ssize_t written = 0;

    do
    {
        rc = libssh2_channel_write(channel, temp, strlen(temp));
        written += rc;
    } while (LIBSSH2_ERROR_EAGAIN != rc && rc > 0 && written != strlen(temp));

    delete[] temp;
}

void NebulaSurfer::Network::SFTP::Mkdir(const char *path)
{
    while (libssh2_sftp_mkdir(sftp_session,
                              "/home/ubuntu/testingsftp",
                              LIBSSH2_SFTP_S_IRWXU |
                                  LIBSSH2_SFTP_S_IRGRP | LIBSSH2_SFTP_S_IXGRP |
                                  LIBSSH2_SFTP_S_IROTH | LIBSSH2_SFTP_S_IXOTH) != LIBSSH2_ERROR_EAGAIN);
}

bool NebulaSurfer::Network::SFTP::List(const char *path, std::vector<std::string>* fnames, std::vector<std::string>* fpaths, std::vector<unsigned long long>* fsizes, std::vector<bool>* isDir)
{

    if(networkInitialized == false){
        return false;
    }

    int rc = 0;
    LIBSSH2_SFTP_HANDLE *sftp_handle;

    do {

        sftp_handle = libssh2_sftp_opendir(sftp_session, path);

        if((!sftp_handle) && (libssh2_session_last_errno(session) != LIBSSH2_ERROR_EAGAIN)) {
            fprintf(stderr, "Unable to open dir with SFTP\n");
            return false;
        }

    } while(!sftp_handle);

     do {

        char mem[512]; // file name
        LIBSSH2_SFTP_ATTRIBUTES attrs;
 
        /* loop until we fail */ 
        while((rc = libssh2_sftp_readdir(sftp_handle, mem, sizeof(mem), &attrs)) == LIBSSH2_ERROR_EAGAIN);

        if(rc > 0) {

            std::string temppath = path;
            temppath += mem;

            //fprintf(stderr, "%s %s\n", mem, LIBSSH2_SFTP_S_ISDIR(attrs.permissions)?"DIR":"FILE");
            isDir->push_back(LIBSSH2_SFTP_S_ISDIR(attrs.permissions));
            fpaths->push_back(temppath);
            fsizes->push_back(attrs.filesize);
            fnames->push_back(mem);

        }
        else if(rc == LIBSSH2_ERROR_EAGAIN) {
            /* blocking */ 
            fprintf(stderr, "");
        }
        else {
            break;
        }
 
    } while(1);

    libssh2_sftp_closedir(sftp_handle);

    return true;
}

// read src file from server and save to dst path
bool NebulaSurfer::Network::SFTP::Download(std::string src, std::string dst){

LIBSSH2_SFTP_HANDLE *sftp_handle;

std::cout << "src: " << src << std::endl << "dst: " << dst << std::endl;


do {

        sftp_handle = libssh2_sftp_open(sftp_session, src.c_str(), LIBSSH2_FXF_READ, 0);

        if((!sftp_handle) && (libssh2_session_last_errno(session) != LIBSSH2_ERROR_EAGAIN)) {
            fprintf(stderr, "Unable to open dir with SFTP\n");
            return false;
        }

    } while(!sftp_handle);

    std::ofstream out(dst, std::ios::app);
 
    fprintf(stderr, "libssh2_sftp_open() is done, now receive data!\n");

    do {
        char mem[1024 * 24];
        ssize_t nread;

        /* loop until we fail */
        while((nread = libssh2_sftp_read(sftp_handle, mem, sizeof(mem))) == LIBSSH2_ERROR_EAGAIN);

        if(nread > 0) {
            //fprintf(stderr, "Read: %lld\n", nread);
            out.write(mem, nread);
        }
        else {
            break;
        }
    } while(1);

    out.close();

    fprintf(stderr, "Done!\n");

    return true;
}

// read src file and upload to server
bool NebulaSurfer::Network::SFTP::Upload(std::string src, std::string dst){
return false;
}