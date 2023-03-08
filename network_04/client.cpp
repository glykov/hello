#include <iostream>
#include <fstream>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>

using namespace std;

int main() {
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == -1) {
        cerr << "Error: Could not create socket." << endl;
        return 1;
    }

    struct hostent *host = gethostbyname("localhost");
    if (host == nullptr) {
        cerr << "Error: Could not get host name." << endl;
        close(clientSocket);
        return 1;
    }

    struct sockaddr_in serverAddress;
    memset(&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(54000);
    serverAddress.sin_addr = *((struct in_addr *) host->h_addr);

    if (connect(clientSocket, (struct sockaddr *) &serverAddress, sizeof(serverAddress)) == -1) {
        cerr << "Error: Could not connect to server." << endl;
        close(clientSocket);
        return 1;
    }

    char buffer[1024];
    cout << "Enter file name: ";
    fgets(buffer, 1024, stdin);
    buffer[strcspn(buffer, "\n")] = 0;

    ofstream file;
    file.open(buffer, ios::out | ios::binary);
    if (!file) {
        cerr << "Error: Could not create file." << endl;
        close(clientSocket);
        return 1;
    }

    int bytesReceived = 0;
    while (1) {
        int bytesRead = recv(clientSocket, buffer, 1024, 0);
        if (bytesRead == -1) {
            cerr << "Error: Could not receive data." << endl;
            close(clientSocket);
            file.close();
            return 1;
        } else if (bytesRead == 0) {
            break;
        }

        file.write(buffer, bytesRead);
        bytesReceived += bytesRead;
    }

    file.close();
    close(clientSocket);

    cout << "File received successfully! " << bytesReceived << " bytes received." << endl;

    return 0;
}