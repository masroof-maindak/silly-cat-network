#include <iostream>
#include <fstream>
#include <vector>           //store image

#include <unistd.h>         //socket-handling + system-level operations
#include <sys/socket.h>     //for sockets
#include <netinet/in.h>     //network structures, e.g sockaddr_in, used in conjunction with;
#include <arpa/inet.h>      //handling IP addresses + converting b/w host & network addresses
#include <pthread.h>        //threads

#include "../lib/queue.h"   //Files being processed/Answer Queues
#include <chrono>           //answer queue pops un-picked-up answer

#define SLEEP_TIME 690000   // 0.69s
#define BUFFER_SIZE 1024
const char delimiter = '`';

struct answer {
    int transactionID;
    std::string retMessage;
};

Queue<answer> answerQueue;

#include "graph.h"

graph g(0);

// Client thread
void* receiveQuery (void* clientSocketPtr) {
    int clientSocket = *((int*)clientSocketPtr);
    free(clientSocketPtr);
    std::ofstream logger("_data/logs.txt", std::ios::app);

    // Receive query from client
    char* buffer = new char[BUFFER_SIZE];
    ssize_t bytesRead = recv(clientSocket, buffer, BUFFER_SIZE, 0);
    if (bytesRead < 1) {
        perror("Error receiving query");
        close(clientSocket);
        pthread_exit(NULL);
    }

    std::string query = std::string(buffer, bytesRead);
    logger << time(0) << " | REQUEST: " << query << "\n";
    std::cout << "QUERY RECEIVED FROM CLIENT: " << query << "\n";
    delete [] buffer;

    // Now, to parse the query and call graph functions accordingly:
    // remove transaction ID and function name from query

    // First word of query is the transaction ID
    int transaID = std::stoi(query.substr(0, query.find(delimiter)));
    query = query.substr(query.find(delimiter) + 1);

    // Second word of query is the function name
    std::string functionToCall = query.substr(0, query.find(delimiter));
    query = query.substr(query.find(delimiter) + 1);

    // The rest of the query is now the arguments to the function, convert them to a vector
    std::vector<std::string> arguments;
    while (query.find(delimiter) != std::string::npos) {
        arguments.push_back(query.substr(0, query.find(delimiter)));
        query = query.substr(query.find(delimiter) + 1);
    }

    /*
    * Call one of the graph functions accordingly.
    * They will push a success/failure message to the answer queue
    * along with the transaction ID, which will be used to identify the answer
    * Except for the filter and relationalQuery functions, which will push the answer to the answer queue
    */

    if (functionToCall == "addVertex") {
        //arguments: transactionID, vertexID, vertexType, vertexProperties
        g.addVertex(transaID, arguments[0], arguments[1], arguments[2]);
    } else if (functionToCall == "addEdge") {
        //arguments: transactionID, edgeTypeLabel, bidirectional, vertex1ID, vertex2ID, vertex1Type, vertex2Type
        g.addEdge(transaID, arguments[0], stoi(arguments[1]), arguments[2], arguments[3], arguments[4], arguments[5]);
    } else if (functionToCall == "mergeVertex") {
        //arguments: transactionID, vertexID, vertexType, vertexProperties
        g.mergeVertex(transaID, arguments[0], arguments[1], arguments[2]);
    } else if (functionToCall == "removeVertex") {
        //arguments: transactionID, vertexID, vertexType
        g.removeVertex(transaID, arguments[0], arguments[1]);
    } else if (functionToCall == "removeEdge") {
        //arguments: transactionID, edgeTypeLabel, bidirectional, vertex1ID, vertex2ID, vertex1Type, vertex2Type
        g.removeEdge(transaID, arguments[0], stoi(arguments[1]), arguments[2], arguments[3], arguments[4], arguments[5]);
    } else if (functionToCall == "filter") {
        //arguments: transactionID, vertexType, vertexProperties
        g.filter(transaID, arguments[0], arguments[1]);
    } else if (functionToCall == "relationalQuery") {
        //arguments: transactionID, vertex1ID, vertex1Type, vertex2Type, edgeTypeLabel, properties
        g.relationalQuery(transaID, arguments[0], arguments[1], arguments[2], arguments[3], arguments[4]);
    } else if (functionToCall == "fetchVertexProperties") {
        //arguments: transactionID, vertexID, vertexType
        g.fetchVertexProperties(transaID, arguments[0], arguments[1]);
    } else if (functionToCall == "addVE") {
        //arguments: transactionID, edgeTypeLabel, bidrectional, vertex1ID, vertex2ID, vertex1Type, vertex2Type, vertex2Properties
        g.addVE(transaID, arguments[0], stoi(arguments[1]), arguments[2], arguments[3], arguments[4], arguments[5], arguments[6]);
    } else if (functionToCall == "removeVE") {
        //arguments: transactionID, edgeTypeLabel, bidrectional, vertex1ID, vertex2ID, vertex1Type, vertex2Type
        g.removeVE(transaID, arguments[0], stoi(arguments[1]), arguments[2], arguments[3], arguments[4], arguments[5]);
    } else {
        std::cout << "Invalid function name: " << functionToCall << "\n";
        close(clientSocket);
        logger.close();
        return NULL;
    }

    std::string feedbackResponse;

    while(true) {
        if (!answerQueue.empty()) {
            answer temp = answerQueue.front();
            if (temp.transactionID == transaID) {
                answerQueue.pop();
                feedbackResponse = temp.retMessage;
                break;
            }
        } else
            usleep(SLEEP_TIME); 
    }

    int feedbackSize = feedbackResponse.size();
    if (send(clientSocket, &feedbackResponse[0], feedbackSize, 0) == -1) {
        perror("Sending char count failed");
        close(clientSocket); pthread_exit(NULL);
    }

    logger << time(0) << " | RESPONSE: " << feedbackResponse << "\n";
    std::cout << "SENDING RESPONSE TO CLIENT: " << feedbackResponse << "\n\n";
    close(clientSocket);
    pthread_exit(NULL);
}

// Answer thread
void* readAnswerQueue(void* arg) {
    std::chrono::seconds timeout(1);

    while (true) {
        if (!answerQueue.empty()) {
            std::string currAns = answerQueue.front().retMessage;
            std::string prevValue = currAns;

            if (answerQueue.size() == 1 || currAns != prevValue) {
                prevValue = currAns;
                auto startTime = std::chrono::steady_clock::now();
                while (true) {
                    auto elapsed_time = std::chrono::steady_clock::now() - startTime;
                    if (elapsed_time >= timeout)
                        break;

                    struct timespec sleepTime;
                    sleepTime.tv_sec = 0;
                    sleepTime.tv_nsec = 200000000; //200 ms
                    nanosleep(&sleepTime, NULL);
                }
            }

            // 'Waste' the answer if it hasn't been picked up
            if (currAns == answerQueue.front().retMessage) {
                answerQueue.pop();
                std::cout << "Popped value: " << currAns << "; Client might have disconnected" << std::endl;
            }
        } else
            usleep(SLEEP_TIME);
    }
    return NULL;
}

int main() {
    pthread_t answerThread;
    if (pthread_create(&answerThread, NULL, readAnswerQueue, NULL) != 0) {
        perror("Answer thread creation failed"); 
        return 1;
    }

    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        perror("Socket creation failed"); 
        return 1;
    }

    int reuse = 1;
    if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) == -1) {
        perror("setsockopt");
        return 1;
    }

    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;         // IPv4
    serverAddr.sin_port = htons(9989);       // port 9989 basically - htons = host TO network short (like the data type)
    serverAddr.sin_addr.s_addr = INADDR_ANY; // All available network interfaces - e.g Wifi/Ethernet/Bluetooth, etc.

    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        perror("Binding failed"); 
        return 1;
    }

    if (listen(serverSocket, 5) == -1) {
        perror("Listening failed"); 
        return 1;
    }

    std::cout << "Server listening on port 9989..." << std::endl;

    while (true) {
        struct sockaddr_in clientAddr;
        socklen_t clientAddrLen = sizeof(clientAddr);
        int* clientSocket = (int*)malloc(sizeof(int));
        *clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientAddrLen);
        
        if (*clientSocket == -1) {
            perror("Failed to accept connection!"); 
            return 1;
        }

        std::cout << "New client connected...\n\n";

        pthread_t clientThread;
        if (pthread_create(&clientThread, NULL, receiveQuery, (void*)clientSocket) != 0) {
            perror("Thread creation failed"); 
            return 1;
        }

        pthread_detach(clientThread);
    }

    close(serverSocket);
    return 0;
}
