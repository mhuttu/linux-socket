#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <strings.h>
#include <errno.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/stat.h> // open
#include <fcntl.h> // open
#include <unistd.h> // read, write, close, lseek, getpid
#include <stdlib.h> // exit, system
#include <sys/sendfile.h>

void process_client(int clientfd)
{
    int HTMLfd;
    off_t offset=NULL;
    size_t retval, readcount, count=1024;
    char buffer[256];

    bzero(&buffer, 256);

    HTMLfd = open("./index.html", O_RDONLY);
    if (HTMLfd == -1) {
      // error opening file
      // use perror to display error string
        perror("ERROR While opening source file index.html");
      // terminate program with exit code 2 (another way of
      // terminating main, but this would work anywhere
      // inside our program, not just main).
        exit(2);
    }
    fprintf(stderr, "FD %d -> source file\n", HTMLfd);

    //send to socket
    while(1)
    {
        readcount = read(HTMLfd, buffer, 256);
        printf("Here's the message %s\n", buffer);
        if(readcount >0)
        {
            retval = sendfile(clientfd, HTMLfd, &offset, count);
            if (retval == -1)
            {
                perror("pieleen meni lähetys");
                if(errno == EBADF)
                        perror ("tiedoston luku ei onnistu");
            }
        }
        else
            break;
    }
    close(HTMLfd);
}

int main(void)
{
    struct sockaddr_in my_addr, client_addr;
    int s, sockfd;

    sockfd = socket (AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        perror("Socket()");
    memset(&my_addr, 0, sizeof(my_addr));
    my_addr.sin_family=AF_INET;
    my_addr.sin_port = htons(80);
    my_addr.sin_addr.s_addr = INADDR_ANY;

    s = bind(sockfd, (struct sockaddr*)&my_addr, sizeof(my_addr));
    if (s < 0) {
        if(errno == EACCES)
                perror("not superuser\n");
        perror("bind()\n");
        exit(1);
    }
    printf("bind ohitettu");
    s = listen(sockfd, 5);
    if (s < 0) {
        perror("listen()");
        exit(1);
    }

    socklen_t clilen = sizeof(client_addr);
    while(1)
    {
        int clientfd = accept(sockfd, (struct sockaddr*)&client_addr, &clilen);
        if(clientfd < 0 )
            perror("accept()");
        process_client(clientfd);
        close(clientfd);
    }


    printf("Hello World!\n");
    close(sockfd);
    return 0;
}

