#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include "proj1.h"

int safefork();

int main(int argc, char *argv[]) {
   int sockfd, pid;
   if (argc < 3) {
      fprintf(stderr,"usage %s hostname port\n", argv[0]);
      exit(0);
   }
   Client client(argv[1],atoi(argv[2]));
   pid = fork();
   if (pid == 0) {
     while (1) 
       client.get_price ();
   }
   while (1) 
     client.gen_buy_request ();
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

