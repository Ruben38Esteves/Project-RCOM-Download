#ifndef URL_H
#define URL_H

#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include <regex.h>
#include <termios.h>

#define MAX_LENGTH  500
#define FTP_PORT    21

#define AT              "@"
#define BAR             "/"
#define HOST_REGEX      "%*[^/]//%[^/]"
#define HOST_AT_REGEX   "%*[^/]//%*[^@]@%[^/]"
#define RESOURCE_REGEX  "%*[^/]//%*[^/]/%s"
#define USER_REGEX      "%*[^/]//%[^:/]"
#define PASS_REGEX      "%*[^/]//%*[^:]:%[^@\n$]"
//#define RESPCODE_REGEX  "%d"

#define DEFAULT_USER        "anonymous"
#define DEFAULT_PASSWORD    "password"

struct URL {      
    char user[MAX_LENGTH];      
    char password[MAX_LENGTH]; 
    char host[MAX_LENGTH];
    char port[MAX_LENGTH];            
    char resource[MAX_LENGTH];  
    char filepath[MAX_LENGTH]; 
    char ip[MAX_LENGTH];        
};

struct URL parse_url(const char *arg);

#endif