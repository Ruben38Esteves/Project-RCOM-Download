#include "../include/url.h"

struct URL parse_url(const char *arg){

    struct URL url = {"", "", "", "", "", "",""};
    regex_t regex;
    regcomp(&regex, BAR, 0);
    //if (regexec(&regex, arg, 0, NULL, 0)) return url;

    regcomp(&regex, AT, 0);
    if(regexec(&regex, arg, 0, NULL, 0) != 0){
        sscanf(arg, HOST_REGEX, url.host);
        strcpy(url.user, DEFAULT_USER);
        strcpy(url.password, DEFAULT_PASSWORD);

    }else{
        sscanf(arg, HOST_AT_REGEX, url.host);
        sscanf(arg, USER_REGEX, url.user);
        sscanf(arg, PASS_REGEX, url.password);
    }

    sscanf(arg, RESOURCE_REGEX, url.resource);
    strcpy(url.filepath, strrchr(arg, '/') + 1);

    struct hostent *h;

    if ((h = gethostbyname(url.host)) == NULL) {
        printf("Invalid hostname '%s'\n", url.host);
        exit(-1);
    }
    strcpy(url.ip, inet_ntoa(*((struct in_addr *) h->h_addr_list[0])));

    return url;
}