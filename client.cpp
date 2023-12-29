#include <iostream>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string>

#define BUFFER_SIZE 512
const char delimiter = '`';

int main() {

    //
    //SOCKET SETUP FOR SERVER
    //

    //init client socket
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    //bind it
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET; //ipv4
    serverAddr.sin_port = htons(9989); //port 9989

    //convert human-readable string to binary network address structure
    if (inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr) < 1) {
        perror("Invalid address or address not supported");
        exit(EXIT_FAILURE);
    }

    //connect client to server via sockets
    if (connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        perror("Connection failed");
        exit(EXIT_FAILURE);
    }

    //
    //  SETUP FOR APPLICATION FRONTEND ???
    //

    /*
        TODO: generate query to send to server here
    */

    std::string query = "put shit here later...";

    //query is in format "[transactionID]`[functionToCall]`[functionArg0]`[]...[functionArgN]`"
    query = "120941`addVertex`fatfatpankocat`cat`username:fatfatpankocat~!password:crosseyedcar~!email:pankster@gmail.com~age:2`";

    // Send query to server
    int stringSize = query.size();
    if (send(clientSocket, &query[0], stringSize, 0) == -1) {
        perror("Sending query to server failed");
        close(clientSocket); 
        exit(EXIT_FAILURE);
    }

    // Recieve feedback/answer string from server in a char buffer,
    char* buffer = new char[BUFFER_SIZE];
    std::memset(buffer, 0, BUFFER_SIZE);
    int bytesReceived = recv(clientSocket, buffer, BUFFER_SIZE, 0);
    if (bytesReceived == -1) {
        perror("Failed to recieve message");
        exit(1);
    } else if (bytesReceived == 0) {
        std::cout << "Client disconnected\n";
        exit(1);
    }

    // Trim buffer down to length
    std::string serverResponse = std::string(buffer, bytesReceived);
    delete[] buffer;

    std::cout << serverResponse << "\n";


    // Close the client socket
    close(clientSocket);

    //
    //  SEND SERVER'S RESPONSE TO REACT
    //

    //TODO: Parse response to JSON first...

    return 0;
}
