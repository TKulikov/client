#include <arpa/inet.h>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <string>
#include <sys/socket.h>
#include <unistd.h>

#define INPUT_ERROR 1
#define FILE_OPEN_ERROR 2
#define PORT_ERROR 3
#define SOCKET_OPEN_ERROR 4
#define CONNECT_ERROR 5
#define RECV_ERROR 6

using namespace std;

int main(int argc, char *argv[])
{
    const char *ip
        = "127.0.0.1"; //Так как ip не вводится, его нужно задать. Не string, так как inet_pton требует const chat*
    if (argc < 3) {
        cerr << "ERROR: You must input <file> <port>" << endl;
        return INPUT_ERROR;
    }

    ifstream file(argv[1]);
    if (!file.is_open()) {
        cerr << "ERROR: Can't open file" << endl;
        return FILE_OPEN_ERROR;
    }
    uint16_t port;
    try {
        port = stoi(argv[2]);
    } catch (...) {
        cerr << "ERROR: Port must be integer";
        return PORT_ERROR;
    }
    int32_t sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        cerr << "ERROR: Con't open socket";
        return SOCKET_OPEN_ERROR;
    }
    sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    inet_pton(AF_INET, ip, &server.sin_addr);
    if (connect(sock, (const struct sockaddr *) &server, sizeof(server)) == 0) {
        string line;
        while (getline(file, line)) {
            send(sock, line.c_str(), line.length(), 0);
            cout << "String ->" << line << "<- was sended" << endl;
        }
        shutdown(sock, SHUT_WR);
        cout << "File was sended" << endl;
        cout << "Waiting server..." << endl;
        char buffer[1024];
        ssize_t recv_bytes = recv(sock, buffer, sizeof(buffer) - 1, 0);
        if (recv_bytes > 0) {
            buffer[recv_bytes] = '\0';
            cout << buffer;
        } else if (recv_bytes == 0) {
            cout << "Connecting closed";
        } else {
            cerr << "ERROR: Server answer";
            return RECV_ERROR;
        }
    } else {
        cerr << "ERROR: Can't connect to server";
        return CONNECT_ERROR;
    }
    return 0;
}
