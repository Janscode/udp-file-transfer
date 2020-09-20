#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#define BUFSIZE 1024

void error(char *msg) {
    perror(msg);
    exit(0);
}

/* save file funciton 
argument(s):
    filename character string
returns:
    0 on success, otherwise error code
*/

int saveFile(int sockfd, char * buf, struct sockaddr_in * serveraddr, int serverlen){
    bzero(buf, BUFSIZE);
    int n;
    buf[0] = '2';
    printf("What file would you like to transfer?\n");
    fgets(buf + 1, BUFSIZE - 1, stdin);
    buf[strlen(buf) - 1] = '\0';
    FILE * fd = fopen(buf + 1, "r");
    if (fd){
        n = sendto(sockfd, buf, strlen(buf), 0, (struct sockaddr *) serveraddr, serverlen);
        if (n < 0){
            perror("Initial send failed");
        }
        while (fgets(buf, BUFSIZE, fd)){
            n = sendto(sockfd, buf, strlen(buf), 0, (struct sockaddr *) serveraddr, serverlen);
            if (n < 0){
                perror("Packet send failed");
            }
            bzero(buf, BUFSIZE);
        }
        /*send goodbye to server*/
        buf[0] = '\0';
        n = sendto(sockfd, buf, 1, 0, (struct sockaddr *) serveraddr, serverlen);
        if (n < 0){
            perror("Final send failed");
        }
    }
    else{
        error("Unable to open file.");
    }
    fclose(fd);
    return 0;
}

/* ls function
arguments: none
returns:
    0 on success, otherwise error code.
*/
int remotels(){
    return 0;
}

// int pullFile(int sockfd, char *buf, struct sockaddr_in * serveraddr, int * serverlen){
//     bzero(buf, BUFSIZE);
//     buf[0] = '3';
//     printf("What file would you like to transfer?\n");
//     fgets(buf + 1, BUFSIZE - 1, stdin);
//     buf[strlen(buf) - 1] = NULL;
//     sendto(sockfd, buf, strlen(buf), 0, serveraddr, serverlen);
//     FILE * fd = fopen(buf + 1, "w");
//     int n;
//     while (1) {
//         bzero(buf, BUFSIZE);
//         n = n = recvfrom(sockfd, buf, BUFSIZE, 0, serveraddr, serverlen);
//         if (n < 0){
//             error("There was some issue getting data from the socket");
//         }
//         if (!strlen(buf)){
//             //fclose(fd);
//             printf("saving");
//             break;
//         }
//         fwrite(buf, sizeof(char), strlen(buf), fd);
//         printf(buf);
//     }
//     fclose(fd);
//     return 0;
// }



int main(int argc, char **argv){
    /*used example code for basic socket usage*/
    int sockfd, portno, n;
    int serverlen;
    struct sockaddr_in serveraddr;
    struct hostent *server;
    char *hostname;
    char buf[BUFSIZE];

    /* check command line arguments */
    if (argc != 3) {
       fprintf(stderr,"usage: %s <hostname> <port>\n", argv[0]);
       exit(0);
    }
    hostname = argv[1];
    portno = atoi(argv[2]);

    /* socket: create the socket */
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket");

    /* gethostbyname: get the server's DNS entry */
    server = gethostbyname(hostname);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host as %s\n", hostname);
        exit(0);
    }

    /* build the server's Internet address */
    bzero((char *) &serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
	  (char *)&serveraddr.sin_addr.s_addr, server->h_length);
    serveraddr.sin_port = htons(portno);

    /* get a message from the user */
    bzero(buf, BUFSIZE);
    printf("Please enter msg: ");
    fgets(buf, BUFSIZE, stdin);

    /* send the message to the server */
    serverlen = sizeof(serveraddr);

    
    int selection;
    while (1)
    {
        /* main input loop*/
        bzero(buf, BUFSIZE);
        
        printf("What would you like to do:\n"
            "(1) List files from remote \n"
            "(2) Save  file to remote \n"
            "(3) Pull file from remote \n"
            "(4) Quit\n");

        fgets(buf, BUFSIZE, stdin);

        switch (atoi(buf))
        {
        case 1:
            /* code */
            remotels();
            break;
        case 2:
            /* code */
            saveFile(sockfd, buf, &serveraddr, serverlen);
            break;
        case 3:
            /* code */
            break;
        case 4:
            /* code */
            printf("Bye!\n");
            return(0);
            break;
        default:
            printf("Please select one of the listed options:");
            break;
        }
    }
}