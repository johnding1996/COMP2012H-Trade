# Trade
---------------
Project 1 for COMP2012H, 2016 Fall, HKUST

A prototypical stock trading system


## Author
---------------
Name: DING, MuCong
Student ID: 20323458
ITSC: mcding


## How-to-compile-and-run
---------------
>make
>server_main
>client_main hostname 5001


## Technical Details
---------------
This program is a single-server-multiple-clients system where messages are passed between the server and multiple client.

The server main program has an outer while-1 loop which continuously accepts new connection from multiple clients. Inside there are two inner while-1 loops running concurrently using fork(), one while-1 loop generates and writes updated stock price to all connected clients every second with the time the price was generated (all times in the sample executables are obtained via the system call time()), while the other while-1 loop simultaneously waits and processes buy requests when they are received.

The client main program has correspondingly two while-1 loops. One continuously accepts updated price from the server, while the other waits for the user to issue a buy request after pressing the enter key and submits the request to the server.

The proj1 hearder file contains declarations of two classes.

The Server class has five public functions: the constructor, the destructor, sock() to inialize the socket and return the file discriptor, gen_price(int newsockfd) to generate the price and broadcast to clients per second, process_buy_request (int newsockfd) to process the buy request by comparing the requested price and local price and reply to the client.

The Client class has four public functions: the constructor, the destructor, get_price () to get price from the server per second, gen_buy_request () to generate buy request and send to the server then process the response.




