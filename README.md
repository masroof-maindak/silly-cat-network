
## Setup
* First go to the WebApp directory and run `npm i` to automatically install dependencies.
* Then, run the server and client binaries, in that order.
* Lastly, to start the webapp server, run `npm run dev` inside WebApp/.

## Introduction
This is an implementation of a multi-threaded C++ graph database that uses bTrees and linked lists to store vertices and edges respectively. The database itself is fairly generic but a sample application developed with React has been provided that communicates with the database via an intermediary client which is responsible for transmitting queries and information.

## Features
The bTree, LinkedList and Queue classes are my own implementations and the former in particular is a crucial aspect of the project. The bTree is used to hold unique identifiers of all the vertices in the graph as 'keys,' and can effortlessly be modified to hold 'values,' i.e locations to where the actual properties of that key are kept. In our case however, this was unecessary because a vertex' unique key can also serve as its file name. Regardless, bTrees have been kept to demonstrate knowledge of the concept and to serve as a proof of concept of the aforementioned idea.

Furthermore, while storing data to disk, the user can also choose to encrypt certain properties if they wish to do so, via an elementary but scalable hashing algorithm. These encrypted properties are also automatically decrypted while being updated or fetched.

## Performance
In addition, the program is also extremely memory-efficient. The only data structures that will be kept in memory in an idle state are two linked lists of the TYPES of vertices and edges in our graph respectively, and an array of bTrees that is same size as our vertexTypeList. The two linked lists will be serialized/deserialized at program starting/termination but everything else is updated in real time and logged to a file.

One index in the prior-mentioned array of B Trees holds only the unique keys of vertices of its corresponding type in the vertexTypeList. A bTree only holds the root node in memory in an idle state. Child (b)nodes are only accessed when operations need to be performed on them. Edge information is also only fetched/written to when needed.

## Complexity
Operations pertaining to vertices are particularly efficient, due to the decision to use bTrees, which can achieve logarithmic time complexity (O(log n)) for insertion, deletion, and search operations. This contrasts with linked lists that offer constant time complexity (O(1)) for certain operations like insertion and deletion at the beginning or end but have linear time complexity (O(n)) for searching through elements.

One unfortunate linear time complexity penalty present throughout the entire program is having to parse either a string of vertices/edges separated by a delimiter or parsing a string for properties. A proper JSON implementation would certainly ameliorate this problem though. One step taken that does slightly help with this problem is the use of a hashmap in the 'mergeVertex' function to check/update key-value format properties in constant time.

## Connections
The database/server advertises port #9989 which a client can connect to. The client (application backend), upon receiving a POST request from the React + Vite webapp (via the REST API credited below), initializes a new client socket, binds it to the aforementioned server socket, transmits the query as is, receives a response from the server, and forwards it unconditionally.

Complex application functionality has been implemented as a part of the webapp, e.g fetching a list of nodes, looping through them and grabbing all the information from them. The frontend itself is fairly limited in terms of its functionality (for instance there are no options to delete a post/account or follow another account) but the database is reasonably robust in terms of what it CAN do (and the implementation of the aforementioned features only involves sending a different query to the server).

## Acknowledgement
* [cpp-httplib](https://github.com/yhirose/cpp-httplib)
