#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/errno.h>
#include <sys/mman.h>
#include <netinet/in.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include "pool.h"

#define PORT 8000
#define BACKLOG 10
#define EPOLL_SIZE 50
#define BUFFER_SIZE 4096

void* session(int *argument) {
	int fd = *argument;
	char buffer[BUFFER_SIZE];
	int size;
	char *reply;
	size = recv(fd, buffer, sizeof(buffer), 0);
	if (size < 0) {
		close(fd);
		printf("socket %d closed\n", fd);
	} else if (size < BUFFER_SIZE) {
		memset(&buffer[size], '\0', 1);
		if (buffer[0] == 'g') {
			printf("%s\n",buffer+1);
			FILE *file = fopen(buffer+1, "rb");
			if (file) {
				reply = "ok";
				send(fd, reply, 2, 0);
				size = recv(fd, buffer, sizeof(buffer), 0);
				memset(&buffer[size], '\0', 1);
				printf("%s\n", buffer);
				printf("%d\n", size);
				if (size == 5 && strcmp(buffer, "ready") == 0) {
					printf("here\n");
					while (1) {
						int value = fread(buffer, sizeof(char), BUFFER_SIZE, file);
						if (value < BUFFER_SIZE) {
							send(fd, buffer, value, 0);
							break;
						} else {
							send(fd, buffer, value, 0);
							memset(buffer, 0, sizeof(char) * BUFFER_SIZE);
						}
					}
					fclose(file);
					close(fd);
				} else {
					fclose(file);
					close(fd);
				}
			} else {
				reply = "no";
				send(fd, reply, 2, 0);
				close(fd);
			}

		} else if (buffer[0] == 'p') {
			FILE *file = fopen(buffer+1, "a+b");
			if (file) {
				reply = "ok";
				send(fd, reply, 2, 0);
				while (1) {
					size = recv(fd, buffer, sizeof(buffer), 0);
					if (size > 0 && size < BUFFER_SIZE) {
						fwrite(buffer, sizeof(char), size,file);
                        break;
					} else {
						fwrite(buffer, sizeof(char), size, file);
                        memset(buffer, 0, sizeof(char) * BUFFER_SIZE);
					}
				}
				fclose(file);
				send(fd, reply, 2, 0);
				close(fd);
			} else {
				reply = "no";
				send(fd, reply, 2, 0);
				close(fd);
			}
		}
	}
	return NULL;
}

int main(int argc, char **argv) {
	int socket_fd;
    struct sockaddr_in server_address;    // server address information
    struct sockaddr_in clientaddress;  
    socklen_t address_size = sizeof(clientaddress);
    int yes = 1;  
  
    if ((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {  
        fprintf(stderr, "creating server socket error!\n");
        exit(-1);  
    }  
  
    if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {  
        fprintf(stderr, "set server socket error!\n");
        exit(-1);  
    }  
      
    server_address.sin_family = AF_INET;         // host byte order  
    server_address.sin_port = htons(PORT);     // short, network byte order  
    server_address.sin_addr.s_addr = inet_addr("127.0.0.1"); // automatically fill with my IP  
    memset(server_address.sin_zero, '\0', sizeof(server_address.sin_zero));  
  
    if (bind(socket_fd, (struct sockaddr *)&server_address, sizeof(server_address)) == -1) {  
        fprintf(stderr, "bind server socket error!\n");
        exit(-1);  
    }  
  
    if (listen(socket_fd, BACKLOG) == -1) {  
        fprintf(stderr, "listen socket error!\n"); 
        exit(-1);  
    }  
  
	printf("listening port %d\n", PORT); 

	pool_create(10);

	static struct epoll_event event, events[EPOLL_SIZE];
	int epollfd = epoll_create(EPOLL_SIZE);
	event.events = EPOLLIN;
	event.data.fd = socket_fd;
	epoll_ctl(epollfd, EPOLL_CTL_ADD, socket_fd, &event);

	while (1) {
		int event_count = epoll_wait(epollfd, events, EPOLL_SIZE, -1);
		int i;
		for (i = 0;i < event_count;i++) {
			if (events[i].data.fd == socket_fd) {
				int client_fd;
				client_fd = accept(socket_fd, (struct sockaddr *)&clientaddress, &address_size);
				if (client_fd <= 0) {
					fprintf(stderr, "accept failed\n");
					continue;
				}
				printf("new connection %s:%d\n", inet_ntoa(clientaddress.sin_addr), ntohs(clientaddress.sin_port));
				add_work(session, &client_fd);
			}
		}
	}
	return 0;
}