#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netdb.h>

#define K	1024
#define PORT	47350

fd_set sfds;		// Saved FD set
int highfd;		// High descriptor number

struct client {
  int fd, dead;
  struct client *next;
} *clients;

void newclient(int sock)
{
  struct client *c;
  int fd;
  
  fd = accept(sock, NULL, NULL);
  if (fd < 0) {
    perror("accept");
    return;
  }
  printf("New connection on %d\n", fd);
  c = malloc(sizeof(struct client));
  c->fd = fd;
  c->dead = 0;
  c->next = clients;
  clients = c;
  
  if (fd+1 > highfd) highfd = fd+1;
  FD_SET(fd, &sfds);
}

struct client *disconnect(struct client *dead)
{
  struct client *c, *p;
  for(p = c = clients; c != NULL; c = c->next) {
    if (c == dead) break;
    p = c;
  }
  printf("Disconnecting %d\n", dead->fd);
  if (p == c) clients = dead->next;
  else p->next = c->next;
  p = c->next;

  close(dead->fd);
  FD_CLR(dead->fd, &sfds);
  free(dead);
  
  return p;
}

void broadcast(int from, char *msg, int len)
{
  struct client *c;
  int maxfd = clients->fd;

  printf("->%d) %.*s", from, len, msg);

  for(c = clients; c != NULL; c=c->next) {
    if (c->dead || from == c->fd) continue;
    if (write(c->fd, msg, len) < 0) c->dead = 1;
  }
}

int main(int argc, char *argv[])
{
  struct sockaddr_in server;
  struct client *c;
  char buf[K];
  int sock, r;
  fd_set rfds;
  
  clients = NULL;
  sock = socket(AF_INET, SOCK_STREAM, 0);
  if (sock < 0) {
    perror("socket");
    exit(1);
  }
  
  r = 1;
  if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &r, sizeof(r)) < 0) {
    perror("setsockopt");
  }
  
  server.sin_family = AF_INET;
  server.sin_addr.s_addr = INADDR_ANY;
  server.sin_port = htons(PORT);
  if (bind(sock, (struct sockaddr *)&server, sizeof(server)) < 0) {
    perror("bind");
    exit(1);
  }
  if (listen(sock, 128) < 0) {
    perror("listen");
    exit(1);
  }
  
  FD_ZERO(&sfds);
  FD_SET(sock, &sfds);
  
  highfd = sock+1;

  printf("Server starting...\n");
  for(;;) {
    // Re-initialize fd set:
    memcpy(&rfds, &sfds, sizeof(fd_set));
    
    r = select(highfd, &rfds, NULL, NULL, NULL);
    if (r < 0) perror("select");
    if (r <= 0) continue;
    
    if (FD_ISSET(sock, &rfds)) newclient(sock);
    for(c = clients; c != NULL; ) {
      if (c->dead) {
	c = disconnect(c);
	continue;
      }
      if (FD_ISSET(c->fd, &rfds)) {
	r = read(c->fd, buf, K);
	if (r <= 0) {
	  c = disconnect(c);
	  continue;
	}
	broadcast(c->fd, buf, r);
      }
      c = c->next;
    }
  }

  // NOT REACHED
  return 0;
}
