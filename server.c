/* A simple server in the internet domain using TCP
   The port number is passed as an argument
   This version runs forever, forking off a separate
   process for each connection
*/
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "backend.h"
#include "backend.c"
#include "server.h"
#include "database.h"
#include "database.c"



void error(const char *msg)
{
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[])
{
     int sockfd, newsockfd, portno, pid;
     socklen_t clilen;
     struct sockaddr_in serv_addr, cli_addr;

     if (argc < 2) {
         fprintf(stderr,"ERROR, no port provided\n");
         exit(1);
     }

     sockfd = socket(AF_INET, SOCK_STREAM, 0);
     if (sockfd < 0)
        error("ERROR opening socket");


     bzero((char *) &serv_addr, sizeof(serv_addr));
     portno = atoi(argv[1]);
     serv_addr.sin_family = AF_INET;
     serv_addr.sin_addr.s_addr = INADDR_ANY;
     serv_addr.sin_port = htons(portno);
     if (bind(sockfd, (struct sockaddr *) &serv_addr,
              sizeof(serv_addr)) < 0)
              error("ERROR on binding");

     listen(sockfd,5);
     clilen = sizeof(cli_addr);


     while (1) {
         newsockfd = accept(sockfd,
               (struct sockaddr *) &cli_addr, &clilen);
         if (newsockfd < 0)
             error("ERROR on accept");
         pid = fork(); // start new process
         if (pid < 0)
             error("ERROR on fork");
         if (pid == 0)  {
             close(sockfd);
             game(newsockfd);
             exit(0);
         }
         else close(newsockfd);
     }
     close(sockfd);
     return 0;
}

void game (int sockfd)
{
    char* top10;
    char user_name[32];
    char buffer[256];
    int n,score;
    printf("Player connected\n");

    n = read(sockfd,user_name,strlen(user_name));
    if(n < 0) error("ERROR on reading from socket");

    n = write(sockfd,"Enter 1 key to play, enter 2 to quit, enter 3 to view highscore\n", strlen("Enter 1 key to play, enter 2 to quit, enter 3 to view highscore\n")); // send instructions to the player
    if(n < 0) error("ERROR on writing to socket");

    while(1) {

        int decision;
        n = read(sockfd,&decision,sizeof(int)); // recieve player input

        switch(decision) {
            case 1:
                score = play(sockfd);
                send(sockfd,&score,sizeof(int),0);
                insert(user_name,score);
                break;
            case 2:
                printf("Player disconnected\n");
                exit(0);
                break;
            case 3:
                top10 = get_highscore();
                send(sockfd,top10,strlen(top10),0);
                break;
            default:
                continue;
        }
    }

}

int play(int sockfd)
{
    int xmax, ymax,n;

    n = read(sockfd,&xmax,sizeof(int));
    if(n < 0 ) error("ERROR on reading");
    n = read(sockfd,&ymax,sizeof(int));
    if(n < 0 ) error("ERROR on reading");

    Board* board = create_board(create_snake(),NULL,xmax,ymax);
    for(int i = 0; i < 6; i++) {
        add_new_food(board);
    }

    while(true) {
        serialize_pointList(board->snake,1,sockfd);
        serialize_pointList(board->foods,1,sockfd);
        int dir;
        n = recv(sockfd,&dir,sizeof(int),0);
        if(n < 0 ) error("ERROR on recieving");

        enum Status status = move_snake(board,dir);
        n = send(sockfd, &status, sizeof(int),0);
        if(n < 0 ) error("ERROR on sending");

        if(status == FAILURE) break;
    }
    return count_points(board->snake);
}

void serialize_pointList(PointList* pointList,int c, int sockfd) {
    int n;
    PointList* p = pointList->next;
    if(p != NULL) {
        serialize_pointList(p,c+1,sockfd); // recursively count length of a pointlist
    }
    else {
        n = send(sockfd,&c,sizeof(int),0); // send length of a pointlist
        if(n < 0 ) error("ERROR on sending");
    }
    int x = pointList->x;
    int y = pointList->y;

    n = send(sockfd,&x,sizeof(int),0);  // send points
    if(n < 0 ) error("ERROR on sending");
    n = send(sockfd,&y,sizeof(int),0);
    if(n < 0 ) error("ERROR on sending");

    return;
}
