#include <iostream>
#include "lib/queue.h"
#include <vector>
#include <cstring>       //memset

#include <unistd.h>      //socket-handling + system-level operations
#include <sys/socket.h>  //for sockets
#include <netinet/in.h>  //network structures, e.g sockaddr_in, used in conjunction with;
#include <arpa/inet.h>   //handling IP addresses + converting b/w host & network addresses
#include <chrono>        //answer queue pops un-picked-up answer
#include <pthread.h>     //threads

struct answer {
    int transactionID;
    std::string retMessage;
};

// Global Queues
Queue<std::string> filesBeingProcessed; // holds names of files on disk that are being currently modified
Queue<answer> answerQueue;              // stores generated answers, and pops them when no client picks an answer up for 5 seconds

#include "graph.h"

graph g(0); //globally accessible graph object

#define SLEEP_TIME 690000 //0.69s
#define MAX_CLIENTS 5
#define BUFFER_SIZE 512

void* decider(void* clientSocketPtr) {
    int clientSocket = *((int*)(&clientSocketPtr));
    free(clientSocketPtr);
    std::ofstream logger("_data/logs.txt", std::ios::app);

    // Recieve string in a char buffer, and trim it down to length afterwards
    char* buffer = new char[BUFFER_SIZE];
    std::memset(buffer, 0, BUFFER_SIZE);
    int bytesReceived = recv(clientSocket, buffer, BUFFER_SIZE, 0);
    if (bytesReceived == -1) {
        perror("Failed to recieve message"); return NULL;
    } else if (bytesReceived == 0) {
        std::cout << "Client disconnected" << std::endl; return NULL;
    }

    //trimming query down to length and logging it
    std::string query = std::string(buffer, bytesReceived);
    delete[] buffer;
    logger << time(0) << " | REQUEST: " << query << "\n";

    //Now, to parse the query and call graph functions accordingly:
    //remove transaction ID and function name from query

    //first word of query is the transaction ID
    int transactionID = std::stoi(query.substr(0, query.find("~")));
    query = query.substr(query.find("~") + 1);

    //second word of query is the function name
    std::string functionToCall = query.substr(0, query.find("~"));
    query = query.substr(query.find("~") + 1);

    //the rest of the query is now the arguments to the function, convert them to a vector
    std::vector<std::string> arguments;
    while (query.find(" ") != std::string::npos) {
        arguments.push_back(query.substr(0, query.find("~")));
        query = query.substr(query.find("~") + 1);
    }

    /*
    * Call one of the graph functions accordingly.
    * They will push a success/failure message to the answer queue
    * along with the transaction ID, which will be used to identify the answer
    * Except for the filter and relationalQuery functions, which will push the answer to the answer queue
    */

    if (functionToCall == "addVertex") {

        //arguments: transactionID, vertexID, vertexType, vertexProperties
        g.addVertex(transactionID, arguments[0], arguments[1], arguments[2]);

    } else if (functionToCall == "addEdge") {

        //arguments: transactionID, edgeTypeLabel, bidirectional, vertex1ID, vertex2ID, vertex1Type, vertex2Type
        g.addEdge(transactionID, arguments[0], stoi(arguments[1]), arguments[2], arguments[3], arguments[4], arguments[5]);

    } else if (functionToCall == "mergeVertex") {

        //arguments: transactionID, vertexID, vertexType, vertexProperties
        g.mergeVertex(transactionID, arguments[0], arguments[1], arguments[2]);

    } else if (functionToCall == "removeVertex") {

        //arguments: transactionID, vertexID, vertexType
        g.removeVertex(transactionID, arguments[0], arguments[1]);

    } else if (functionToCall == "removeEdge") {

        //arguments: transactionID, edgeTypeLabel, bidirectional, vertex1ID, vertex2ID, vertex1Type, vertex2Type
        g.removeEdge(transactionID, arguments[0], stoi(arguments[1]), arguments[2], arguments[3], arguments[4], arguments[5]);

    } else if (functionToCall == "filter") {

        //arguments: transactionID, vertexType, vertexProperties
        g.filter(transactionID, arguments[0], arguments[1]);

    } else if (functionToCall == "relationalQuery") {

        //arguments: transactionID, vertex1ID, vertex1Type, vertex2Type, edgeTypeLabel, properties
        g.relationalQuery(transactionID, arguments[0], arguments[1], arguments[2], arguments[3], arguments[4]);

    } else {
        std::cout << "Invalid function name" << std::endl;
        //cleanup
        close(clientSocket);
        logger.close();
        return NULL;
    }

    //pop answer from answer queue now (functions above will push to it)
    std::string feedback;
    while(true) {
        if (!answerQueue.empty()) {
            answer temp = answerQueue.front();
            //confirming if element at top of Queue is the one generated for this thread
            if (temp.transactionID == transactionID) {
                //if it is, remove it from the answer queue, store it, and exit
                answerQueue.pop();
                feedback = temp.retMessage;
                break;
            }
        } else
            usleep(SLEEP_TIME); 
    }

    //send answer to client and log it
    if (send(clientSocket, &feedback, sizeof(feedback), 0) == -1) {
        perror("Failed to send answer"); return NULL;
        logger << time(0) << " | REPORT: " << "Failed to send answer, transaction #" << transactionID << "\n";
    } 
    else
        logger << time(0) << " | REPORT: " << feedback << "\n";

    //close file, client socket and thread
    logger.close();
    close(clientSocket);
    pthread_exit(NULL);
}

//answer thread
void* readAnswerQueue(void* arg) {
    std::chrono::seconds timeout(5);

    //forever
    while (true) {
        //if answerQ not emprty
        if (!answerQueue.empty()) {
            //current answer = front of queue
            std::string currAns = answerQueue.front().retMessage;
            std::string prevValue = currAns;

            if (answerQueue.size() == 1 || currAns != prevValue) {
                prevValue = currAns;
                auto startTime = std::chrono::steady_clock::now();
                while (true) {
                    //check elapsed time
                    auto elapsed_time = std::chrono::steady_clock::now() - startTime;
                    //if it exceeds our time out value
                    if (elapsed_time >= timeout) {
                        //leave inner while
                        break;
                    }

                    // sleep for some time
                    struct timespec sleepTime;
                    sleepTime.tv_sec = 0;
                    sleepTime.tv_nsec = 200000000; //200 ms
                    nanosleep(&sleepTime, NULL);
                }
            }

            //if value at queue front still matches the previously recorded one; pop it.
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
    
    //create the thread that handles the answer queue
    pthread_t answerThread; //pops if the top of the queue remains unchanged for a fixed time period
    //e.g when the client disconnects
    if (pthread_create(&answerThread, NULL, readAnswerQueue, NULL) != 0) {
        perror("Answer thread creation failed");
        return 1;
    }

    //PTHREAD_CREATE() arguments:
    //1. thread by reference
    //2. thread attributes, e.g stack size/scheduling policy
    //3. function it's working in (scope of reference?)
    //4. function parameters

    //big daddy server socket
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        perror("Socket creation failed"); 
        return 1;
    }

    //SOCKET() arguments:
    //af_inet = address family (ipv4) - others include AF_INET6 (ipv6), AF_UNIX (IPC comm. on the same device)
    //sock_stream = socket type (in this case, STREAM). Stream sockets allow for connection-oriented communication. Others include SOCK_RAW, SOCK_SEQPACKET, etc.
    //0 = protocol (default - OS selects based on address family and socket type)

    //make it so that it overwrites a previous iteration's binding to the same port
    int reuse = 1;
    if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) == -1) {
        perror("setsockopt"); 
        return 1;
    }

    //bind the socket to an IP address, port and network protocol
    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;    //sin = socket internet
                    					//yes, it's fucking stupid.
    serverAddr.sin_port = htons(9989);  //port 9989 basically - htons = host TO network short (like the data type)
    serverAddr.sin_addr.s_addr = INADDR_ANY; //// All available network interfaces - e.g Wifi/Ethernet/Bluetooth, etc.
    // sin_addr - the field used to specify the IP address associated with the socket. 
    // Only sort of a wrapper for the s_addr value in this case. 
    // Exists for compatibility, extensibility, readability, and consistency. 
    // Keeps API versatile and accomodates future changes.
	// s_addr is used to store the actual IP address in binary format.

    //bind - associates socket with specific network address (IP + port)
    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        perror("Binding failed"); 
        return 1;
    }

    //listen for incoming connections
    if (listen(serverSocket, 5) == -1) {
        perror("Listening failed"); 
        return 1;
    }

    //LISTEN() arguments:
    //1. make this socket ready to accept connections
    //2. accept, at most, this value, before refusing all further connections

    //public service announcement
    std::cout << "Server listening on port 9989..." << std::endl;

    //connecting with client(s) (infinitely)
    while (true) {
        //struct to store client info
        struct sockaddr_in clientAddr;
        socklen_t clientAddrLen = sizeof(clientAddr);
        //dynamically allocated memory to store int (i.e socket file descriptor) for incoming client
        int* clientSocket = (int*)malloc(sizeof(int));
        //'accept' function. Blocking until client connects.
        *clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientAddrLen);

        //ACCEPT() arguments:
        //1. int socket fd, this is the socket that is waiting for clients to connect to it
        //2. a pointer to the sockAddr that will be filled with the client's information when it connects
        //   usually cast to (struct sockaddr*), but can also be cast to specific socket address structure 
        //   like struct sockaddr_in for IPv4 or struct sockaddr_in6 for IPv6.
        //3. Pointer to socklen_t variable that stores size of addr.

        //in case of error, accept() returns -1
        if (*clientSocket == -1) {
            perror("Failed to accept connection!"); 
            return 1;
        }

        //create client thread
        //inits the client in a separate function, passing the socket as a parameter
        pthread_t clientThread;
        if (pthread_create(&clientThread, NULL, decider, (void*)clientSocket) != 0) {
            perror("Thread creation failed"); 
            return 1;
        }

        pthread_detach(clientThread); // Detach the thread to allow it to clean up automatically
    }

    // Close sockets
    close(serverSocket);
    return 0;
}
