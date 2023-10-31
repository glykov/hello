#include <iostream>
#include <fstream>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

using namespace std;

int main() {
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        cerr << "Error: Could not create socket." << endl;
        return 1;
    }

    struct sockaddr_in serverAddress;
    memset(&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(54000);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    if (bind(serverSocket, (struct sockaddr *) &serverAddress, sizeof(serverAddress)) == -1) {
        cerr << "Error: Could not bind to port." << endl;
        close(serverSocket);
        return 1;
    }

    if (listen(serverSocket, 5) == -1) {
        cerr << "Error: Could not listen on port." << endl;
        close(serverSocket);
        return 1;
    }

    cout << "Server started. Listening on port 54000." << endl;

    while (1) {
        int clientSocket = accept(serverSocket, nullptr, nullptr);
        if (clientSocket == -1) {
            cerr << "Error: Could not accept incoming connection." << endl;
            close(serverSocket);
            return 1;
        }

        char buffer[1024];
        int bytesSent = 0;

        cout << "Enter file name: ";
        fgets(buffer, 1024, stdin);
        buffer[strcspn(buffer, "\n")] = 0;

        ifstream file;
        file.open(buffer, ios::in | ios::binary);
        if (!file) {
            cerr << "Error: Could not open file." << endl;
            close(clientSocket);
            continue;
        }

        while (!file.eof()) {
            file.read(buffer, 1024);

            int bytesWritten = send(clientSocket, buffer, file.gcount(), 0);
            if (bytesWritten == -1) {
                cerr << "Error: Could not send data." << endl;
                break;
            }

            bytesSent += bytesWritten;
        }

        file.close();
        close(clientSocket);

        cout << "File sent successfully! " << bytesSent << " bytes sent." << endl;
    }

    close(serverSocket);

    return 0;
}