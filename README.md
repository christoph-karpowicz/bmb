# Basic message broker

A simple message broker application written in C. It works on top of a TCP/IP Linux socket server, which handles incoming connections on separate threads. 

## Prerequisites

The only third party library that bmb uses is cJSON. Before running the app, it has to be installed on your machine as a dynamically linked library.

## Installation

```
make
```
will create a new executable `bmb` in the current directory.

## Usage

Communication with the message broker is made possible by a RESTful API exposed on localhost port 8080.

Clients can freely create new queues with unique names and remove them. Each is a [FIFO](https://en.wikipedia.org/wiki/FIFO_(computing_and_electronics)) (First In First Out) queue and clients can add, read and remove messages without affecting the other queues. When it comes to reading messages from queues, they can be either peeked or consumed. Peeking means getting a specified node's message without removing it from the queue - whereas consuming removes the message from the the queue's tail and returns it to the client.

The application's RESTful API offers other useful operations on the queues and their nodes, like reading all nodes from a specified queue and returning them as an array to the client.

To start the server, simply go to the directory with the executable `bmb` file and run:
```
./bmb
```

**All available HTTP requests don't require URIs. They should be sent to `http://localhost:8080/` with the required data specified in the RESTful API section of this readme file.**

After the application receives an HTTP request:
1. the request gets parsed and evaluated,
2. if the request is correct and it contains the necessary data, the request action gets carried out by the broker,
3. the broker modifies both in-memory and persisted in files data if necessary,
4. an HTTP response is created and sent back to the client.
The response typically contains information whether the request operation was sucessful and data gathered by the broker if such was requested.

> Please note, that in the current state of the application, the API accepts only [query strings](https://en.wikipedia.org/wiki/Query_string) in the HTTP requests.

## RESTful API

The app responds with a JSON string:
```
{
    payload: {
        message: [confirmation of a successful operation and a reason why if unsucessful]
        data: [requested data]
    }
    success: [true/false]
}
```
In the payload, both message and data are optional so responses can include only a message, only data or both depending on the operation carried out by the broker.

### Available requests and their payload results:

**Add queue**  
description: Adds a new queue.  
method: POST  
request data: 
```
{
    type: "addQueue",
    data: [new queue's name]
}
```
as query string: `type=addQueue&data=[new queue's name]`  
response payload: message

**Consume**  
description: Consumes a message from a queue. A node storing the message is removed from the queue and it's message gets sent to the client.  
method: GET  
request data: 
```
{
    type: "consume",
    queue: [queue name]
}
```
as query string: `type=consume&queue=[queue name]`  
response payload: `{ data: [consumed message] }`

**Length**  
description: Gets the length of a queue (how many nodes it contains).  
method: GET  
request data: 
```
{
    type: "length",
    queue: [queue name]
}
```
as query string: `type=length&queue=[queue name]`  
response payload: `{ data: [queue length] }`

**Get**  
description: Peeks a node with the specified index, in the specified queue. Peeking does not remove a node from the queue.  
method: GET  
request data: 
```
{
    type: "get",
    index: [node's index],
    queue: [queue name]
}
```
as query string: `type=get&index=[node's index]&queue=[queue name]`  
response payload: `{ data: [node's message] }`

**Get all**  
description: Peeks all nodes from the specified queue. Peeking does not remove a node from the queue.  
method: GET  
request data: 
```
{
    type: "getAll",
    queue: [queue name]
}
```
as query string: `type=getAll&queue=[queue name]`  
response payload: `{ data: { nodes: [an object array of all nodes from the queue]} }`  
Note: the above array of nodes consists on individual JSON objects: `{ index: [node's index in the queue], message: [node's message] }`  

**Get all queue names**  
description: Gets all the existing queue names.  
method: GET  
request data: 
```
{
    type: "getAllQueueNames"
}
```
as query string: `type=getAllQueueNames`  
response payload: `{ data: { queueNames: [a string array of all queue names]} }`

**Produce**  
description: Produce na new message. This operation creates a new node holding a given message in the specified queue.  
method: POST  
request data: 
```
{
    type: "produce",
    queue: [queue name],
    data: [message]
}
```
as query string: `type=produce&queue=[queue name]&data=[message]`  
response payload: message

**Remove queue**  
description: Removes the queue and all its nodes.  
method: GET  
request data: 
```
{
    type: "removeQueue",
    queue: [queue name]
}
```
as query string: `type=removeQueue&queue=[queue name]`  
response payload: message

## Built With

gcc - the GNU Compiler Collection, v7.4.0  
[cJSON](https://github.com/DaveGamble/cJSON)