#ifndef PROJ1_H 
#define PROJ1_H

using namespace std;

//the Client class
class Client{
	struct hostent *server;
	int sockfd; //sockfd: file descriptor that refers to a socket
	int n; //n: the int type return value of read and wirte, indicating success or failure
	struct sockaddr_in serv_addr;
	int iswaiting; //boolean for whether the client is waiting for the response of buy request form the server
	char buffer_copy[256]; //copy of the price information in the buffer

	char* buffer; //pointer to the shared memeory of buffer
	int* result; //pointer to the shared memeory of result of buy request
	char* response; //pointer to the shared memeory of response string
public:
	Client (const char *hostname, int portno);
	~Client ();
	void get_price (); //get price from the server per second
	void gen_buy_request (); //generate buy request and send to the server then process the response
};

class Server{
	int portno; //portno to be used, default = 5001
	int time_interval; //time interval to update the price, defalut = 10
	time_t rawtime;
    struct tm * timeinfo;
    int sockfd; //sockfd: file descriptor that refers to a socket
    int n; //n: the int type return value of read and wirte, indicating success or failure
	struct sockaddr_in serv_addr;
	char request[256]; //buy request char array read from the clients

	char* priceinfo; //pointer to the shared memory of the latest price infomation string
	int* last_update_time; //pointer to the shared memory of last_update_time
	double* price; //pointer to the shared memory of price
	int* buyno; //pointer to the shared memory of buy number

    void update_price (); //private function for updating the price per 10 seconds
public:
	Server ();
	~Server ();
	int sock (); //initialize the socket and return the socket file discriptor
	void gen_price (int newsockfd); //generate the price and broadcast to clients per second
	void process_buy_request (int newsockfd); //process the buy request by comparing the requested price and local price and reply to the client
};

#endif
