#include "lib/cpp-httplib/httplib.h"
#include "lib/json.hpp"
#include <iostream>
#include <string>
#include <cstring>

using json = nlohmann::json;
#define BUFFER_SIZE 512

int main(void)
{
    using namespace httplib;

    Server svr;

    svr.Post("/content_receiver", [&](const httplib::Request &req, httplib::Response &res, const httplib::ContentReader &content_reader) {
        if (req.is_multipart_form_data()) {
            // Handle multipart form data (if needed)
        } else {
            std::string body;
            content_reader([&](const char *data, size_t data_length) {
                body.append(data, data_length);
                return true;
            });

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
            //connect client to server (DB) via sockets
            if (connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
                perror("Connection failed");
                exit(EXIT_FAILURE);
            }

            //generate query to send to server
            std::string query = body;
            std::cout << "QUERY WE ARE SENDING TO DB: " << query << "\n";

            // Send query to server
            int stringSize = query.size();
            if (send(clientSocket, &query[0], stringSize, 0) == -1) {
                perror("Sending query to server failed");
                close(clientSocket);
                exit(EXIT_FAILURE);
            }

            // json received_json = json::parse(body);
            // Do something with the JSON data (e.g., process it, validate it)

            // Recieve feedback/answer string from server in a char buffer,
            char* buffer = new char[BUFFER_SIZE];
            std::memset(buffer, 0, BUFFER_SIZE);
            int bytesRead = 10;
            // /*
            bytesRead = recv(clientSocket, buffer, BUFFER_SIZE, 0);
            if (bytesRead == -1) {
                perror("Failed to recieve message");
                exit(1);
            } else if (bytesRead == 0) {
                std::cout << "Client disconnected\n";
                exit(1);
            }
            
            // Trim buffer down to length
            std::string serverResponse = std::string(buffer, bytesRead);
            delete[] buffer;
            std::cout << "RESPONSE WE GOT FROM THE SERVER: " << serverResponse << std::endl;

            close(clientSocket);
            
            // Set CORS headers if needed
            res.set_header("Access-Control-Allow-Origin", "*");
            res.set_header("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
            res.set_header("Access-Control-Allow-Headers", "Content-Type");

            // Set response content type and body
            res.set_content(serverResponse, "application/json");
        }
    });

    svr.listen("192.168.100.118", 1234);
}