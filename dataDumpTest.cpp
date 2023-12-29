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

int main() {
    graph g(0);

    g.addVertex(100, "bingus", "cat", "username:bingus~!password:loveyoubuddy~!email:mujtaba.asim.amin@gmail.com~");
    g.addVertex(101, "freegoblinpics", "cat", "username:freegoblinpics~!password:goblinsevilpass~!email:villain@gmail.com~");

    g.mergeVertex(102, "bingus", "cat", "!password:fuckyeah");

    g.filter(103, "cat", "");

    g.addEdge(104, "follows", 0, "bingus", "freegoblinpics", "cat", "cat");

}