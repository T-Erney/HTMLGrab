#ifndef HTMLGRAB_H
#define HTMLGRAB_H

#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <netdb.h>

#define HTTP 80

static char*  htmlgrab(char*);
static char** get_host_url(char*, size_t);
static char*  htmlgrab_build_request_absuri(char*, char*);
static char*  htmlgrab_build_request_proxuri(char*, char*);

# ifndef HTMLGRAB_NOBUILD
static char* htmlgrab(char* url) {
    // create a stream for the socket data
    char* html;
    size_t html_size;
    FILE* stream = open_memstream(&html, &html_size);

    char** host_url = get_host_url(url, strlen(url));

    int tcp_sock;
    struct hostent* server;
    struct sockaddr_in serveraddr;

    tcp_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (tcp_sock < 0) {
        printf("htmlgrab: error: socket failed to open\n");
        exit(-1);
    }

    server = gethostbyname(host_url[0]);
    memcpy(&serveraddr.sin_addr, server->h_addr_list[0], server->h_length);
    serveraddr.sin_port = htons(HTTP);
    serveraddr.sin_family = AF_INET;
    if (connect(tcp_sock, (struct sockaddr*)&serveraddr, sizeof(serveraddr)) < 0) {
        printf("htmlgrab: error: socket failed to connect\n");
        exit(-1);
    }

    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 100000;
    if (setsockopt(tcp_sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof timeout) < 0) {
        printf("htmlgrab: error: socket options failed\n");
        exit(-1);
    }

    if (setsockopt(tcp_sock, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof timeout) < 0) {
        printf("htmlgrab: error: socket options failed\n");
        exit(-1);
    }

    char* request = (host_url[1][0] != 0) ? htmlgrab_build_request_proxuri(host_url[0], host_url[1]) : htmlgrab_build_request_absuri(host_url[0], host_url[1]);

    if (send(tcp_sock, request, strlen(request), 0) < 0) {
        printf("htmlgrab: error: send failed\n");
        exit(-1);
    }

    char buffer[1025] = {0};
    ssize_t bytes = 0;
    while ((bytes = read(tcp_sock, buffer, 1024)) > 0) {
        if (bytes == 0) break;
        fprintf(stream, "%s", buffer);
        memset(buffer, 0, 1024);
    }

    fclose(stream);  // close string stream
    close(tcp_sock); // close socket stream

    return html;
}

static char** get_host_url(char* url, size_t len) {
    char** pack = malloc(sizeof(char*) * 2);
    pack[0] = calloc(1000, 1);
    pack[1] = calloc(1000, 1);

    int i = 0, j = 0;
    while ((int)len > i) {
        if (*url == '/') break;
        pack[0][i++] = *url;
        url += 1;
    }

    j = i;
    i = 0;
    while ((int)len > (j + i)) {
        pack[1][i++] = *url;
        url += 1;
    }
    if (i == 0) pack[1][0] = '/';

    return pack;
}

static char* htmlgrab_build_request_absuri(char* host, char* url) {
#define format_str "GET %s HTTP/1.1\r\nUser-Agent: htmlgrab/0.0.1\r\nAccept: */*\r\nAccept-Encoding: identity\r\nHost: %s\r\n\r\n"
    char* request = malloc(strlen(format_str) + strlen(host) + strlen(url));
    sprintf(request, format_str, host, url);

#undef format_str
    return request;
}

static char* htmlgrab_build_request_proxuri(char* host, char* url) {
#define format_str "GET %s HTTP/1.1\r\nUser-Agent: htmlgrab/0.0.1\r\nAccept: */*\r\nAccept-Encoding: identity\r\nHost: %s\r\n\r\n"
    char* request = malloc(strlen(format_str) + strlen(host) + strlen(url));
    sprintf(request, format_str, url, host);

#undef format_str
    return request;
}

# endif // HTMLGRAB_NOBUILD
#endif // HTMLGRAB_H
