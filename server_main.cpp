#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>
#include "proj1.h"

int safefork(); 

int main( int argc, char *argv[] ) {
   struct sockaddr_in cli_addr;
   int sockfd, newsockfd, ppid;
   socklen_t clilen;
   Server server; 
  
   sockfd = server.sock();
   listen(sockfd,5);   // listening to at most 5 clients 
   clilen = sizeof(cli_addr);
   while (1) {
     newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
     if (newsockfd < 0) {
       perror("ERROR on accept");
       exit(1);
     }
     ppid = safefork();
     if (ppid == 0) {
       while (1) 
         server.gen_price (newsockfd);
     }
     ppid = safefork();
     if (ppid == 0) {
       while (1) 
         server.process_buy_request(newsockfd);
     }
   }
   return 0;
}

int safefork() {
  int pid = fork();
  if (pid < 0) {
    perror("ERROR on fork");
    exit(1);
  }
  return pid;
}

