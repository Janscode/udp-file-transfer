#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define BUFSIZE 1024

void error(char *msg) {
    perror(msg);
    exit(0);
}

void ls(int sockfd, char * buf, struct sockaddr_in *clientaddr, int clientlen){
    FILE * fp = popen("ls", "r");
    int n;
    while (fgets(buf, BUFSIZE, fp)){
        n = sendto(sockfd, buf, strlen(buf), 0, (struct sockaddr *) clientaddr, clientlen);
        if (n < 0) error("Send Data Failed");
        bzero(buf, BUFSIZE);
    }
    buf[0] = '\0';
    n = sendto(sockfd, buf, 1, 0, (struct sockaddr *) clientaddr, clientlen);
        if (n < 0) error("Good Bye Failed");
}

void saveFile(int sockfd, char * buf, struct sockaddr_in * clientaddr, int * clientlen){
    int n;
    FILE* fd = fopen(buf + 1, "w");
    while (1) {
        bzero(buf, BUFSIZE);
        n = recvfrom(sockfd, buf, BUFSIZE, 0,
		 clientaddr, clientlen);
        if (n < 0){
            error("Issue receiving data during transfer.");
        }
        printf("Bytes: %d", n);
        if (n < 2){
            fclose(fd);
            break;
        }
        fwrite(buf + 1, sizeof(char), n - 1, fd);
    }
}


void pullFile(int sockfd, char * buf, struct sockaddr_in *clientaddr, int clientlen){
        /* 
    * gethostbyaddr: determine who sent the datagram
    */
    struct hostent *hostp; /* client host info */
    char *hostaddrp; /* dotted decimal host addr string */
    int n;
    

    FILE * fd = fopen(buf + 1, "r");

    if (fd){
        /*To do: add response to indicate succesful file open */
        /*data transfer loop*/
        while ((n = fread(buf + 1, sizeof(char), BUFSIZE - 1, fd)) > 0){
            /*send file line */
            buf[0] = 0xFF;
            n = sendto(sockfd, buf, n + 1, 0, (struct sockaddr *)
             clientaddr, clientlen);
            if (n < 0){
                perror("Packet send failed");
            }
            bzero(buf, BUFSIZE);
        }
        /*send goodbye to server*/
        buf[0] = 0xFF;
        n = sendto(sockfd, buf, 1, 0, (struct sockaddr *)
         clientaddr, clientlen);
        if (n < 0)
            perror("Final send failed");
    }
    else error("Unable to open file");
}








int main(int argc, char **argv){
    int sockfd; /* socket */
    int portno; /* port to listen on */
    int clientlen; /* byte size of client's address */
    struct sockaddr_in serveraddr; /* server's addr */
    struct sockaddr_in clientaddr; /* client addr */
    
    char buf[BUFSIZE]; /* message buf */
    
    int optval; /* flag value for setsockopt */
    int n; /* message byte size */
    
    /* 
   * check command line arguments 
   */
    if (argc != 2) {
        fprintf(stderr, "usage: %s <port>\n", argv[0]);
        exit(1);
    }
    portno = atoi(argv[1]);

    /* 
    * socket: create the parent socket 
    */
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket");

    /* setsockopt: Handy debugging trick that lets 
    * us rerun the server immediately after we kill it; 
    * otherwise we have to wait about 20 secs. 
    * Eliminates "ERROR on binding: Address already in use" error. 
    */
    optval = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, 
            (const void *)&optval , sizeof(int));

    /*
    * build the server's Internet address
    */
    bzero((char *) &serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr.sin_port = htons((unsigned short)portno);

    /* 
    * bind: associate the parent socket with a port 
    */
    if (bind(sockfd, (struct sockaddr *) &serveraddr, 
        sizeof(serveraddr)) < 0) 
        error("ERROR on binding");

    /* 
    * main loop: wait for a datagram, then echo it
    */
    clientlen = sizeof(clientaddr);
    while (1) {

    /*
     * recvfrom: receive a UDP datagram from a client
     */
    bzero(buf, BUFSIZE);
    n = recvfrom(sockfd, buf, BUFSIZE, 0,
		 (struct sockaddr *) &clientaddr, &clientlen);
    if (n < 0)
      error("ERROR in recvfrom");

    
    switch (atoi(buf))
        {
        case 1:
            /* ls */
            ls(sockfd, buf, &clientaddr, clientlen);
            break;
        
        case 2:
            /* save */
            printf("Saving file.");
            saveFile(sockfd, buf, &clientaddr, &clientlen);
            break;

        case 3:
            /* pull */
            printf("Pulling file");
            pullFile(sockfd, buf, &clientaddr, clientlen);
            break;

        default:
            break;
        }
    }
}