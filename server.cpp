#include <stdio.h>
#include <math.h>
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


Server::Server () {
    //initialize and locate the shared memory
	srand(time(NULL)); 
	portno = 5001;
	time_interval = 10;
	priceinfo = (char* )mmap(NULL, 32*sizeof(char), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	last_update_time = (int* )mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	price = (double* )mmap(NULL, sizeof(double), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	buyno = (int* )mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	*last_update_time = time(NULL) - time_interval;
	*price = 0.00;
	*buyno = 0;
}

void Server::update_price () {
    //if the time interval past, then update the price
	if ((time(NULL) - *last_update_time) >= time_interval) {
		*price = (rand() % 100) / 100.0;
		*last_update_time = time(NULL);
	}
}

int Server::sock() {
    //initialize a socket and return it
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("ERROR opening socket");
        exit(1);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        perror("ERROR on binding");
        exit(1);
    }
    return sockfd;
}

void Server::gen_price (int newsockfd) {
    //first try updating the price
	update_price();

    char strprice[32]; //char array for the whole price information string
    char strtime[25]; //char array for only the local time information string

    //print time to char array strtime
    time (&rawtime);
    timeinfo = localtime (&rawtime);
    strftime (strtime, 25, "%a %b %d %X %Y", timeinfo);
    strtime[24] = '\0';
    
    //print price to char array strprice
	snprintf(strprice, 7, "$%.2f ", *price);
    strprice[6] = '\0';

    //connect the strtime to the end of strprice
	strcat(strprice, strtime);
    strprice[31] = '\0';

    //update the local priceinfo string
    strcpy(priceinfo, strprice);

    //try writing the price infomation to the socket
	n = write(newsockfd, strprice, 32);
    if (n < 0) {
    	perror("ERROR writing to socket");
    	exit(1);
    }
    printf("%s\n", strprice); //if wirte succssfully, print it on screen

    //sleep for 1 second
    sleep(1);
}

void Server::process_buy_request (int newsockfd) {
    //receive the buy request from the client
	bzero(request, 256);
    n = read(newsockfd, request, 255);
    if (n < 0) {
        perror("ERROR reading from socket");
        exit(1);
    }

    //sleep from 1 second before processing the request
    sleep(1);

    //the legal buy request starts from 'b'
    if (request[0]=='b') {

        //extract out the requested price
    	char decimalPrice[3];
    	snprintf(decimalPrice, 3, "%.*s", 2, request + 3);
    	double request_price = atoi(decimalPrice) / 100.0;
        double localprice = *price;
        
        //check whether it is the same as local price, the tolerance is 0.00001
    	if (fabs(request_price - localprice) < 0.005) {

            //update buyno and print out on screen
        	*buyno = *buyno + 1;
    		printf("num buy attempts = %d\n", *buyno);

            //the legal response for confirming buy starts from 'c'
            char confirminfo[33];
		    snprintf(confirminfo, 33, "c%s", priceinfo);
		    n = write(newsockfd, confirminfo, 33);
    		if (n < 0) {
    			perror("ERROR writing to socket");
    			exit(1);
    		}
    	} else {
    		//the legal response for denying buy starts from 'd'
    		char denyinfo[33];
		    snprintf(denyinfo, 33, "d%s", priceinfo);
		    n = write(newsockfd, denyinfo, 33);
    		if (n < 0) {
    			perror("ERROR writing to socket");
    			exit(1);
    		}
    	}
    }

}

Server::~Server () {
    //close the socket
    close(sockfd);
}
