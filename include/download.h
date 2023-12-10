#ifndef DOWNLOAD_H
#define DOWNLOAD_H

#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <string.h>
#include <regex.h>
#include <termios.h>

#define READY4AUTH           220
#define READY4PASS           331
#define LOGINSUCCESS         230
#define PASSIVE              227
#define READY4TRANSFER       150
#define TRANSFER_COMPLETE    226
#define GOODBYE              221

int get_status(int socket);

void authenticate(int socket, const char *username, const char *password);

void get_passive(int socket, char *host, char *port);

void recieve_file(int socket, int psocket, const char *file, const char *resource);





#endif