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
    friend std::ostream& operator<<(std::ostream& os, const answer& ans) {
        os << ans.transactionID << " " << ans.retMessage;
        return os;
    }
};

// Global Queues
Queue<std::string> filesBeingProcessed; // holds names of files on disk that are being currently modified
Queue<answer> answerQueue;              // stores generated answers, and pops them when no client picks an answer up for 5 seconds

#include "graph.h"

int main() {
    graph g(0);

    // g.addVertex(100, "bingus", "cat", "username:bingus~!password:bestfeline~!email:mujtaba.asim.amin@gmail.com~age:2");
    // g.addVertex(101, "freegoblinpics", "cat", "username:freegoblinpics~!password:goblinsevilpass~!email:villain@gmail.com~");

    // g.mergeVertex(102, "bingus", "cat", "!password:BASEDfeline");

    // g.filter(103, "cat", "");
    // g.addEdge(104, "follows", 0, "bingus", "freegoblinpics", "cat", "cat");
    // g.addVertex(105, "meevinofficial", "cat", "username:meevinofficial~!password:meevinspass~!email:meevin@gmail.com~");

    // g.fetchVertexProperties(106, "bingus", "cat");
    // g.removeVertex(107, "bingus", "cat");

    // g.addEdge(108, "follows", 0, "bingus", "meevinofficial", "cat", "cat");
    // g.mergeVertex()
    g.relationalQuery(109, "bingus", "cat", "cat", "follows", "age:2~");

    // g.addVE(110, "posts",0,"bingus", "bingus_12345678","cat", "post","desc:making my database!~tags:null~timestamp:12345678~");

    // g.relationalQuery(111, "bingus", "cat", "post", "posts", "");
    // g.fetchVertexProperties(112, "bingus_12345678", "post");

    // g.removeEdge(113, "posts", 0, "bingus", "bingus_12345678", "cat", "post");
    // g.removeVertex(114, "bingus_12345678", "post");

    g.filter(115, "cat", "username:randomcataccount~!password:newcataccount~");

    g.filter(116, "post", "");
    answerQueue.print();
}
