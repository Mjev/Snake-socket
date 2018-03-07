#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include "frontend.h"
#include "frontend.c"
#include "client.h"



void error(const char *msg)
{
    perror(msg);
    exit(0);
}

int main(int argc, char *argv[])
{
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;


    if (argc < 3) {
       fprintf(stderr,"usage %s hostname port\n", argv[0]);
       exit(0);
    }
    portno = atoi(argv[2]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");
    server = gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)
        error("ERROR connecting");

    game(sockfd);

    close(sockfd);
    return 0;
}

void game(int sockfd)
{
    char top10[1024];
    char buffer[512];
    char user_name[32];
    int decision,score,c,n;

    //n = read(sockfd,buffer,sizeof(buffer));
    //if(n < 0) error("error reading");
    printf("hello,enter your username\n");

    bzero(user_name,33);
    fgets(user_name,32,stdin);

    n = write(sockfd,user_name,strlen(user_name));
    if(n < 0) error("error writing");

    bzero(buffer,513);
    n = read(sockfd,buffer,sizeof(buffer));
    if(n < 0) error("error reading");

    while(1) {
        printf("%s",buffer);

        char term;
        if(scanf("%d%c", &decision, &term) != 2 || term != '\n') {
            printf("valid integer followed by enter key are allowed\n");
            continue;
        }

        int n = write(sockfd,&decision,sizeof(int));
        if(n < 0) error("ERROR writing");
        switch(decision) {
            case 1:
                score = play(sockfd);
                printf("Your Score was: %i \n",score);
                break;
            case 2:
                exit(0);
                break;
            case 3:
                bzero(top10,1025);
                n = recv(sockfd,top10,1024,0);
                if(n < 0) error("error recieving");
                top10[n] = '\0';
                printf("%s",top10);
                break;
            default:
                printf("Unknown command\n");
                break;
        }
    }
}
int play(int sockfd)
{
    // ncurses settings
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);
    timeout(100);

    int xmax, ymax, n;
    getmaxyx(stdscr, ymax, xmax);
    enum Direction dir = RIGHT;

    // send screen size
    n = write(sockfd,&xmax,sizeof(int));
    if(n < 0) error("error writing");
    n = write(sockfd,&ymax,sizeof(int));
    if(n < 0) error("error writing");

    int k = 0;

     while(true) {
        clear();

        // request snake pointlist length
        n = recv(sockfd,&k,sizeof(int),0);
        if(n < 0) error("error recieving");

        PointList* snake = NULL;
        snake = deserialize_pointList(snake,k,sockfd);

        // request food pointlist length
        n = recv(sockfd,&k,sizeof(int),0);
        if(n < 0) error("error recieving");
        PointList* food = NULL;
        food = deserialize_pointList(food,k,sockfd);

        display_points(snake, ACS_BLOCK);
        display_points(food, ACS_DIAMOND);
        free_pointList(snake);
        free_pointList(food);
        refresh();

        dir = get_next_move(dir); // get player input
        send(sockfd,&dir,sizeof(int),0); // send input to the server

        enum Status status;
        read(sockfd,&status,sizeof(int)); // recieve status of a game
        if (status == FAILURE) break;
    }
    endwin();
    int score;
    recv(sockfd,&score,sizeof(int),0); // recieve score of a player
    return score;
}

PointList* deserialize_pointList(PointList* pointList,int k,int sockfd)
{
    int x,y,n;

    PointList* p = pointList;
    for(int i = 0; i<k; i++) {
        n = recv(sockfd,&x,sizeof(int),0);
        if(n < 0) error("Error recieving");

        n = recv(sockfd,&y,sizeof(int),0);
        if(n < 0) error("Error recieving");

        if(p == NULL) {
            p = create_cell(x,y);
            pointList = p;
        }
        else {
            p->next = create_cell(x,y);
            p = p->next;
        }
    }
    return pointList;
}

void free_pointList(PointList* pointList)
{
    if(pointList == NULL) return;
    PointList* p = pointList->next;
    while(p != NULL) {
        free(pointList);
        pointList = p;
        p = p->next;
    }
}
