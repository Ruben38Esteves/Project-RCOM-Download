#include "../include/download.h"
#include <libgen.h>

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
    char buf[MAX_LENGTH];
    int command_length;

    if (get_status(socket) != READY4AUTH)
    {
        fprintf(stderr, "Server is not ready.\n");
        exit(EXIT_FAILURE);
    }
    command_length = snprintf(buf, MAX_LENGTH, "USER %s\n", username);
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

    command_length = snprintf(buf, MAX_LENGTH, "PASS %s\n", password);
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

void get_passive(int socket, char *ip, char *port){
    char *in_buf = "pasv\n", out_buf[MAX_LENGTH];

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

    sprintf(ip, "%hhu.%hhu.%hhu.%hhu", h1, h2, h3, h4);
    sprintf(port, "%hu", p1 * 256 + p2);
    printf("Entered passive mode\n");
}

void recieve_file(int socket, int psocket, const char *file, const char *resource){

    char buf[MAX_LENGTH];
    int command_length;
    command_length = snprintf(buf, MAX_LENGTH, "retr %s\n", resource);
    if (send(socket, buf, command_length, 0) < 0){
        fprintf(stderr, "Error sending file request (%s).\n", file);
        exit(EXIT_FAILURE);
    }

    char output_path[MAX_LENGTH];
    char *resource_copy = strdup(resource);
    char *filename = basename(resource_copy);
    strcpy(output_path, "output/");
    strcat(output_path, filename);


    FILE *fp = fopen(output_path, "w");
    if (fp == NULL){

        fprintf(stderr, "Error opening file (%s).\n", resource);
        exit(EXIT_FAILURE);
    }

    char buf2[MAX_LENGTH];
    ssize_t bytes;

    while ((bytes = recv(psocket, buf2, sizeof buf2 - 1, 0)) > 0){
        fwrite(buf2, 1, bytes, fp);
    }

    printf("File saved in %s\n",output_path);

    fclose(fp);
}