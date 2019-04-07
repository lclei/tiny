#include "mynet.h"
int open_listenfd(char *port);
void echo(int connfd);

int main(int argc, char **argv)
{
	int listenfd, connfd;
	socklen_t clientlen;
	struct sockaddr_storage clientaddr;
	char client_hostname[MAXLINE], client_port[MAXLINE];

	if(argc!=2){
	  fprintf(stderr,"usage: %s <port>\n",argv[0]);
	  exit(0);
	}
	listenfd = open_listenfd(argv[1]);
	while(1){
		clientlen = sizeof(struct sockaddr_storage);
		connfd = accept(listenfd, (SA*)&clientaddr,&clientlen);
		getnameinfo((SA*)&clientaddr,clientlen,client_hostname,MAXLINE,client_port,MAXLINE,0);
		echo(connfd);
		close(connfd);
	}
	exit(0);
}
void echo(int connfd)
{
	size_t n;
	char buf[MAXLINE];

	while((n=read(connfd,buf,MAXLINE))!=0){
		printf("server recived %d bytes\n",(int)n);
		write(connfd,buf,n);
	}
}
