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
#include <fcntl.h>  
#include <errno.h> 

pthread_mutex_t threadCountMutex = PTHREAD_MUTEX_INITIALIZER;

#define MAX_THREADS 4
#define MAX_PENDING_CONNECTIONS 100

typedef enum
{
    WAIT_FOR_MSG,
    IN_MSG
} ProcessingState;

void perror_die(const char *message)
{
    perror(message);
    exit(EXIT_FAILURE);
}

int threadCount = 0;
int overFlowingThread[MAX_PENDING_CONNECTIONS];
int overflowingCount = 0;

void add_connection(int sockfd)
{
    if (overflowingCount < MAX_PENDING_CONNECTIONS)
    {
        overFlowingThread[overflowingCount] = sockfd;
        overflowingCount++;
        printf("Added socket %d to overflow queue. Overflowing count: %d\n", sockfd, overflowingCount);
    }
    else
    {
        printf("Overflow queue is full. Rejecting connection from socket %d\n", sockfd);
        close(sockfd);
    }
}

int remove_connection()
{
    int sock = 0;
    if (overflowingCount > 0)
    {
        sock = overFlowingThread[0];
        for (int i = 1; i < overflowingCount; i++)
        {
            overFlowingThread[i - 1] = overFlowingThread[i];
        }
        overflowingCount--;
        printf("Removed socket %d from overflow queue. Overflowing count: %d\n", sock, overflowingCount);
    }
    else
    {
        printf("No connection in overflow queue to remove.\n");
    }
    return sock;
}
void server_connection(int sockfd)
{
    printf("Server connection started for socket %d\n", sockfd);
    if (send(sockfd, "Hello", 5, 0) < 1)
    {
        perror_die("send");
    }
    printf("Sent greeting 'Hello' to socket %d\n", sockfd);

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
            printf("Socket %d closed the connection\n", sockfd);
            break;
        }

        printf("Received %d bytes from socket %d\n", len, sockfd);

        // Handle message processing logic
        for (int i = 0; i < len; ++i)
        {
            switch (state)
            {
            case WAIT_FOR_MSG:
                if (buf[i] == '^')
                {
                    state = IN_MSG;
                    printf("Switching to IN_MSG state\n");
                }
                break;

            case IN_MSG:
                if (buf[i] == '$')
                {
                    state = WAIT_FOR_MSG;
                    printf("Switching back to WAIT_FOR_MSG state\n");
                }
                else
                {
                    buf[i] += 1;
                    printf("Incremented byte at index %d to %d, sending it back\n", i, buf[i]);
                    if (send(sockfd, &buf[i], 1, 0) < 1)
                    {
                        perror("send error");
                        close(sockfd);
                        return;
                    }
                }
                break;

            default:
                if (overflowingCount > 0)
                {
                    printf("Default case: Overflow detected, attempting to handle next connection\n");
                    int sock = remove_connection();
                    if (sock != 0)
                    {
                        server_connection(sock);
                    }
                }
                break;
            }
        }

        const char *message = "This is a message from the server.";
        if (send(sockfd, message, strlen(message), 0) < 1)
        {
            perror("send error");
            close(sockfd);
            return;
        }
        printf("Sent message to socket %d: %s\n", sockfd, message);
    }

    close(sockfd);
    printf("Server connection ended for socket %d\n", sockfd);
}

typedef struct
{
    int sockfd;
} thread_config_t;

void *server_thread(void *arg)
{
    thread_config_t *config = (thread_config_t *)arg;
    int sockfd = config->sockfd;
    free(config);

    unsigned long id = (unsigned long)pthread_self();
    printf("Thread %lu started for socket %d\n", id, sockfd);
    server_connection(sockfd);
    printf("Thread %lu finished for socket %d\n", id, sockfd);

    pthread_mutex_lock(&threadCountMutex);
    threadCount--;
    pthread_mutex_unlock(&threadCountMutex);
    printf("Thread count decreased. Current thread count: %d\n", threadCount);

    remove_connection();
    return 0;
}

int listen_inet_socket(int portnum)
{
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        perror_die("socket");
    }
    printf("Created socket with descriptor %d\n", sockfd);

    int opt = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
    {
        perror_die("setsockopt");
    }
    printf("Socket options set for reuse address\n");

    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(portnum);

    if (bind(sockfd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        perror_die("bind");
    }
    printf("Socket bound to port %d\n", portnum);

    if (listen(sockfd, 10) < 0)
    {
        perror_die("listen");
    }
    printf("Socket is listening for incoming connections\n");

    return sockfd;
}

void report_peer_connected(struct sockaddr_in *peer_addr, socklen_t peer_addr_len)
{
    char peer_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(peer_addr->sin_addr), peer_ip, INET_ADDRSTRLEN);
    printf("Connection from %s:%d\n", peer_ip, ntohs(peer_addr->sin_port));
}

int main(int argc, const char** argv) {
  setvbuf(stdout, NULL, _IONBF, 0);

  int portnum = 9988;
  if (argc >= 2) {
    portnum = atoi(argv[1]);
  }
  printf("Listening on port %d\n", portnum);

  int sockfd = listen_inet_socket(portnum);
  struct sockaddr_in peer_addr;
  socklen_t peer_addr_len = sizeof(peer_addr);

  int newsockfd = accept(sockfd, (struct sockaddr*)&peer_addr, &peer_addr_len);
  if (newsockfd < 0) {
    perror_die("ERROR on accept");
  }
  report_peer_connected(&peer_addr, peer_addr_len);

  // Set nonblocking mode on the socket.
  int flags = fcntl(newsockfd, F_GETFL, 0);
  print("%d", flags) ;
  
  if (flags == -1) {
    perror_die("fcntl F_GETFL");
  }

  if (fcntl(newsockfd, F_SETFL, flags | O_NONBLOCK) == -1) {
    perror_die("fcntl F_SETFL O_NONBLOCK");
  }

  while (1) {
    uint8_t buf[1024];
    printf("Calling recv...\n");
    int len = recv(newsockfd, buf, sizeof buf, 0);
    if (len < 0) {
      if (errno == EAGAIN || errno == EWOULDBLOCK) {
        usleep(200 * 1000);
        continue;
      }
      perror_die("recv");
    } else if (len == 0) {
      printf("Peer disconnected; I'm done.\n");
      break;
    }
    printf("recv returned %d bytes\n", len);
  }

  close(newsockfd);
  close(sockfd);

  return 0;
}