# SillyCatNetwork

## Todo:
### Core
- [ ] Text-based **graph database backend**
- [ ] Robust Filtering/**Searching** algorithms
- [ ] **Query language** to read JSON/write data efficiently
- [ ] _Account for images_
### Reading/Writing
- [ ] Serializing process - write all data to binary files and to a **central file**(s?) that holds information + 
- [ ] Separate program to **read information file and then relevant data**
- [ ] Main DB program recieves commands in the form of the query language and **initializes + writes new entries**
- [ ] E.g if a cat joined a chatroom, load both the cat and chatroom into memory and init an edge to the chatroom
- [ ] Reader program also received query commands, for instance CAT:MonkeyCatLuna.posts, it will **then scan the info file**
- [ ] Find all nodes for MonkeyCatLuna, her posts (via the edge between them), and return them
### Network Programming
- [ ] Basic socket-client architecture in the read/write programs
- [ ] Instead of sending output to a terminal emulator, it will be sent via **sockets to a unique port** which the clients can access
- [ ] Images will be **reconstructed** on the client's side, have to look into this, perhaps a Dart OpenCV rewrite?
- [ ] _Multithreading and creating a worker with a processing/return queue_
### Frontend
- [ ] Flutter: very-stripped down Instagram clone with just groups on the right, a feed in the middle, and profile accounts
- [ ] Darting: Receive JSON information, _images_ from the client port and display it
