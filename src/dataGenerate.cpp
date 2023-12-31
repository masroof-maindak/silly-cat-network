#include <iostream>
#include "../lib/queue.h"

struct answer {
    int transactionID;
    std::string retMessage;
    friend std::ostream& operator<<(std::ostream& os, const answer& ans) {
        os << ans.transactionID << " " << ans.retMessage;
        return os;
    }
};

Queue<answer> answerQueue;
#include "graph.h"

int main() {
    graph g(0);

    //Add cats
    // g.addVertex(100, "bingus", "cat", "username:bingus~!password:bestfeline~!email:mujtaba.asim.amin@gmail.com~age:2");
    // g.addVertex(101, "freegoblinpics", "cat", "username:freegoblinpics~!password:goblinsevilpass~!email:villain@gmail.com~");

    //Update properties
    // g.mergeVertex(102, "bingus", "cat", "!password:BASEDfeline");

    //Get all cats
    // g.filter(103, "cat", "");

    // Add follow edge and another cat
    // g.addEdge(104, "follows", 0, "bingus", "freegoblinpics", "cat", "cat");
    // g.addVertex(105, "meevinofficial", "cat", "username:meevinofficial~!password:meevinspass~!email:meevin@gmail.com~");

    // get bingus properties and remove bingus
    // g.fetchVertexProperties(106, "bingus", "cat");
    // g.removeVertex(107, "bingus", "cat");

    // Add follow edge and another cat and query for all cats bingus is following
    // g.addEdge(108, "follows", 0, "bingus", "meevinofficial", "cat", "cat");
    // g.relationalQuery(109, "bingus", "cat", "cat", "follows", "age:2~");

    // add bingus post
    // g.addVE(110, "posts",0,"bingus", "bingus_12345678","cat", "post","desc:making my database!~tags:null~timestamp:12345678~");

    // query for all posts bingus has made and then fetch properties of a bingus post
    // g.relationalQuery(111, "bingus", "cat", "post", "posts", "");
    // g.fetchVertexProperties(112, "bingus_12345678", "post");

    // g.fetchVertexProperties(113, "bingus", "cat");

    g.filter(114, "post", "");

    answerQueue.print();
}
