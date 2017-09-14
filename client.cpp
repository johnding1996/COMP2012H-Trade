#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <time.h>
#include "proj1.h"

Client::Client (const char *hostname, int portno) {
	//initialize a socket
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("ERROR opening socket");
        exit(1);
    }
    server = gethostbyname(hostname);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(1);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(sockfd,(struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        perror("ERROR connecting");
        exit(1);
    }

    //initialize and locate the shared memory
    buffer = (char* )mmap(NULL, 256*sizeof(char), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    result = (int* )mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    response = (char* )mmap(NULL, 256*sizeof(char), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    iswaiting = 0;
}

void Client::get_price () {
	//try read from the socket to get the price infomation
	bzero(buffer, 256);
    n = read(sockfd, buffer, 255);
    if (n < 0) {
       perror("ERROR reading from socket");
       exit(1);
    }

    //check if read the response for confirming buy request
    if (buffer[0] == 'c') {
    	//if so, use the shared memory to acknowledge the result
    	*result = 1;
    	bzero(response, 256);
    	strcpy(response, buffer);
    	return;
    }

    //check if read the response for denying buy request
    if (buffer[0] == 'd') {
    	//if so, use the shared memory to acknowledge the result
    	*result = 2;
    	bzero(response, 256);
    	strcpy(response, buffer);
    	return;
    }

    //print out the price infomation on screen
    printf("%s\n",buffer);

    //sleep for 1 second 
    sleep(1);
}

void Client::gen_buy_request () {
	//two states in this function:
	//iswaiting == 0: not waiting, getchar from the keyboard to process the next buy request
	//iswaiting != 0: is waiting, check the shared memory to know whether response has received
	if (iswaiting == 0) {
		char c = getchar();
		if (c!='\n') return;
		printf("\n");

		//copy the price infomation in the buffer to another char array because buffer is being updated
		if (buffer[0] != 'c') strcpy(buffer_copy, buffer);

		//genetate the buy request
		//legal buy request starts from 'b' and 
		char buyinfo[6];
		snprintf(buyinfo, 6, "b%.*s", 5, buffer_copy + 1);
		n = write(sockfd, buyinfo, 6);
		if (n < 0) {
    	    perror("ERROR writing to socket");
    	    exit(1);
    	}

    	//print out updated processes of buying
    	printf("from client: client buy  at ");
    	printf("%.*s", 5, buffer_copy);
    	printf(" at ");
    	printf("%s", buffer_copy + 6);
    	printf("\n");
    	iswaiting = 1;
        
    } 
    else {
    	//if responce == 1, then the response was confirmed
    	if (*result == 1) {

    		//print out the result of successful buying
    		printf("client buy  at ");
        	printf("%.*s", 5, buffer_copy);
        	printf(" at ");
        	printf("%s", buffer_copy + 6);
       		printf("\n");

    		printf("server sell  at ");
        	printf("%.*s", 5, response + 1);
        	printf(" at ");
        	printf("%s", response + 7);
        	printf("\n");

    		printf("all times approx\n");
    		printf("buy SUCCESSFUL from server!\n");
    		printf("\n");

    		//reset the booleans
    		*result = 0;
    		iswaiting = 0;
    	} 
        //if responce == 2, then the response was denied
    	else if (*result == 2) {

    		//print out the result of failed buying
    		printf("client buy  at ");
        	printf("%.*s", 5, buffer_copy);
        	printf(" at ");
        	printf("%s", buffer_copy + 6);
       		printf("\n");

    		printf("server sell  at ");
        	printf("%.*s", 5, response + 1);
        	printf(" at ");
        	printf("%s", response + 7);
        	printf("\n");

    		printf("all times approx\n");
    		printf("buy FAILED from server!\n");
    		printf("\n");

    		//reset the booleans
    		*result = 0;
    		iswaiting = 0;
    	}
        
        //sleep for 1 second after processing the response
        sleep(1);
	}

}

Client::~Client () {
    //close the socket
	close(sockfd);
}
