#include "src/url.c"
#include "src/download.c"

#include <stdio.h>
#include <stdlib.h>
#define SV_GOODBYE 221

int main(int argc, char *argv[]){
    if(argc != 2){
        printf("Usage: %s <url>\n", argv[0]);
        exit(-1);
    }

    struct URL url = parse_url(argv[1]);

    printf("host: %s\n", url.host);
    printf("IP: %s\n", url.ip);

    struct sockaddr_in sv_sock;

    memset((void*)&sv_sock,0,sizeof(sv_sock));

    sv_sock.sin_family = AF_INET;
    sv_sock.sin_addr.s_addr = inet_addr(url.ip);
    sv_sock.sin_port = htons(21);

    int sock = socket(AF_INET, SOCK_STREAM, 0);

    if (sock < 0) {
        perror("socket()");
        exit(-1);
    }

    if (connect(sock, (struct sockaddr *)&sv_sock, sizeof(sv_sock)) < 0) 
    {
        perror("connect()");
        exit(-1);
    }

    printf("\n");
    printf("Username: %s\n", url.user);
    printf("Password: %s\n", url.password);
    authenticate(sock, url.user, url.password);
    printf("User authenticated.\n\n");

    get_passive(sock, url.ip, url.port);
    printf("Port: %s\n", url.port);

    struct sockaddr_in sv_sock2;

    memset((void*)&sv_sock2,0,sizeof(sv_sock2));

    sv_sock2.sin_family = AF_INET;
    sv_sock2.sin_addr.s_addr = inet_addr(url.ip);
    sv_sock2.sin_port = htons(atoi(url.port));

    int psock = socket(AF_INET, SOCK_STREAM, 0);

    if (psock < 0){
        perror("socket()");
        exit(-1);
    }

    if (connect(psock, (struct sockaddr *)&sv_sock2, sizeof(sv_sock2)) < 0){
        perror("connect()");
        exit(-1);
    }
    
    recieve_file(sock, psock, url.filepath, url.resource);

    close(sock);
    close(psock);

    return 0;
}