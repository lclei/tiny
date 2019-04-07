#include "mynet.h"

int open_clientfd(char *hostname, char *port);
int open_listenfd(char *port);

int main(int argc, char** argv)
{
	int clientfd;
	char *host, *port, buf[MAXLINE];
	

	if(argc!=3){
		fprintf(stderr,"usage: %s <host> <port>\n",argv[0]);
		exit(0);
	}
	host = argv[1];
	port = argv[2];

	clientfd = open_clientfd(host,port);
	//rio_readinitb(&rio,clientfd);

	while(fgets(buf,MAXLINE,stdin) != NULL){
		write(clientfd,buf,strlen(buf));
		read(clientfd, buf, MAXLINE);
		fputs(buf,stdout);
	}
	close(clientfd);
	exit(0);
}

