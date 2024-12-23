#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>

typedef enum
{
    WAIT_FOR_MSG,
    IN_MSG
} ProcessingState;

void perror_die(const char *message) {
    perror(message);
    exit(EXIT_FAILURE);
}

void server_connection(int sockfd)
{
    if (send(sockfd, "*", 1, 0) < 1)
    {
        perror_die("send");
    }

    ProcessingState state = WAIT_FOR_MSG;

    while (1)
    {
        uint8_t buf[1024];
        int len = recv(sockfd, buf, sizeof(buf), 0);
        if (len < 0)
        {
            perror_die("recv");
        }
        else if (len == 0)
        {
            break;
        }

        for (int i = 0; i < len; ++i)
        {
            switch (state)
            {
            case WAIT_FOR_MSG:
                if (buf[i] == '^')
                {
                    state = IN_MSG;
                }
                break;

            case IN_MSG:
                if (buf[i] == '$')
                {
                    state = WAIT_FOR_MSG;
                }
                else
                {
                    buf[i] += 1;
                    if (send(sockfd, &buf[i], 1, 0) < 1)
                    {
                        perror("send error");
                        close(sockfd);
                        return;
                    }
                }
                break;

            default:
                break;
            }
        }
    }
    close(sockfd);
}

typedef struct { int sockfd; } thread_config_t;

void* server_thread(void* arg) {
    thread_config_t* config = (thread_config_t*)arg;
    int sockfd = config->sockfd;
    free(config);

    unsigned long id = (unsigned long)pthread_self();
    printf("Thread %lu created to handle connection with socket %d\n", id,
           sockfd);
    server_connection(sockfd);
    printf("Thread %lu done\n", id);
    return 0;
}

int listen_inet_socket(int portnum) {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror_die("socket");
    }

    int opt = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror_die("setsockopt");
    }

    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(portnum);

    char ip_str[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(addr.sin_addr), ip_str, INET_ADDRSTRLEN);
    printf("Listening on %s:%d\n", ip_str, ntohs(addr.sin_port));

    if (bind(sockfd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror_die("bind");
    }

    if (listen(sockfd, 10) < 0) {
        perror_die("listen");
    }

    return sockfd;
}

void report_peer_connected(struct sockaddr_in *peer_addr, socklen_t peer_addr_len) {
    char peer_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(peer_addr->sin_addr), peer_ip, INET_ADDRSTRLEN);
    printf("Connection from %s:%d\n", peer_ip, ntohs(peer_addr->sin_port));
}

int main(int argc, char **argv)
{
    setvbuf(stdout, NULL, _IONBF, 0);
    int portnum = 9090;
    if (argc >= 2)
    {
        portnum = atoi(argv[1]);
    }

    printf("Serving on port %d\n", portnum);

    int sockfd = listen_inet_socket(portnum);

    while (1) {
        struct sockaddr_in peer_addr;
        socklen_t peer_addr_len = sizeof(peer_addr);

        int newsockfd = accept(sockfd, (struct sockaddr*)&peer_addr, &peer_addr_len);

        if (newsockfd < 0) {
            perror_die("ERROR on accept");
        }

        report_peer_connected(&peer_addr, peer_addr_len);
        pthread_t the_thread;

        thread_config_t* config = (thread_config_t*)malloc(sizeof(*config));
        if (!config) {
            perror_die("OOM");
        }
        config->sockfd = newsockfd;
        pthread_create(&the_thread, NULL, server_thread, config);

        pthread_detach(the_thread);
    }
    return 0;
}
