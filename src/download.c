#include "../include/download.h"

int get_status(int socket){
    char buffer[1024];
    ssize_t total_bytes_read = 0;

    while(1){
        usleep(100000);
        ssize_t bytes_read = recv(socket, buffer, sizeof buffer - total_bytes_read, MSG_DONTWAIT);
        if (bytes_read < 0){
            buffer[total_bytes_read] = '\0';
            break;
        }
        total_bytes_read += bytes_read;
    }

    int status;
    if (sscanf(buffer, "%d", &status) != 1){
        fprintf(stderr, "Invalid status line: %s\n", buffer);
        exit(EXIT_FAILURE);
    }
    return status;
}

void authenticate(int socket, const char *username, const char *password){
    char buf[266];
    int command_length;

    if (get_status(socket) != READY4AUTH)
    {
        fprintf(stderr, "Server is not ready.\n");
        exit(EXIT_FAILURE);
    }
    command_length = snprintf(buf, 266, "USER %s\n", username);
    if (send(socket, buf, command_length, 0) < 0)
    {
        fprintf(stderr, "Error sending username (%s).\n", username);
        exit(EXIT_FAILURE);
    }

    if (get_status(socket) != READY4PASS)
    {
        fprintf(stderr, "Error, server should be expecting password\n");
        exit(EXIT_FAILURE);
    }

    command_length = snprintf(buf, 266, "PASS %s\n", password);
    if (send(socket, buf, command_length, 0) < 0)
    {
        fprintf(stderr, "Error sending password (%s).\n", password);
        exit(EXIT_FAILURE);
    }

    if (get_status(socket) != LOGINSUCCESS)
    {
        fprintf(stderr, "Login failed.\n");
        exit(EXIT_FAILURE);
    }
}

void get_passive(int socket, char *host, char *port){
    char *in_buf = "pasv\n", out_buf[256];

    if (send(socket, in_buf, strlen(in_buf), 0) < 0)
    {
        fprintf(stderr, "Error sending passive command.\n");
        exit(EXIT_FAILURE);
    }

    ssize_t bytes = recv(socket, out_buf, sizeof out_buf - 1, 0);
    if (bytes < 0)
    {
        perror("recv");
        exit(EXIT_FAILURE);
    }
    out_buf[bytes] = '\0';

    int code;
    uint8_t h1, h2, h3, h4, p1, p2;
    sscanf(out_buf, "%d %*[^(](%hhu, %hhu, %hhu, %hhu, %hhu, %hhu)\n", &code, &h1, &h2, &h3, &h4, &p1, &p2);
    if (code != PASSIVE)
    {
        fprintf(stderr, "Error entering passive mode.\n");
        exit(EXIT_FAILURE);
    }

    sprintf(host, "%hhu.%hhu.%hhu.%hhu", h1, h2, h3, h4);
    sprintf(port, "%hu", p1 * 256 + p2);
    printf("%s\n",port);
}

void recieve_file(int socket, int psocket, const char *file, const char *resource){

    char buf[266];
    int command_length;

    command_length = snprintf(buf, 266, "retr %s\n", file);
    if (send(socket, buf, command_length, 0) < 0)
    {
        fprintf(stderr, "Error sending file request (%s).\n", file);
        exit(EXIT_FAILURE);
    }
    /*
    int status = get_status(socket);
    printf("%d\n", status);
    */
    char output_path[256];
    strcpy(output_path, "output/");
    strcat(output_path, resource);

    printf("%s\n",output_path);

    FILE *fp = fopen(output_path, "w");
    if (fp == NULL){
        fprintf(stderr, "Error opening file (%s).\n", resource);
        exit(EXIT_FAILURE);
    }

    char buf2[256];
    ssize_t bytes;
    while ((bytes = recv(psocket, buf2, sizeof buf2 - 1, 0)) > 0){
        fwrite(buf2, 1, bytes, fp);
    }

    fclose(fp);
}