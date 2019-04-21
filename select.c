#include "mynet.h"
int open_listenfd(char *port);
void echo(int connfd);
void command(void);

int main(int argc, char **argv)
{
	int listenfd, connfd;
	socklen_t clientlen;
	struct sockaddr_storage clientaddr;
	char client_hostname[MAXLINE], client_port[MAXLINE];
	fd_set read_set, ready_set;

	if(argc!=2){
	  fprintf(stderr,"usage: %s <port>\n",argv[0]);
	  exit(0);
	}
	listenfd = open_listenfd(argv[1]);
	FD_ZERO(&read_set);	//clear read_set
	FD_SET(STDIN_FILENO, &read_set);	//add stdin to read_set
	FD_SET(listenfd,&read_set);	//add listenfd to read_set

	while(1){
		ready_set = read_set;
		select(listenfd+1,&ready_set,NULL,NULL,NULL);
		if(FD_ISSET(STDIN_FILENO,&ready_set))
		{
			printf("stdin is ready.\n");
			command();
		}
		if(FD_ISSET(listenfd,&ready_set)){
			printf("client connetted.\n");
			clientlen = sizeof(struct sockaddr_storage);
			connfd = accept(listenfd, (SA*)&clientaddr,&clientlen);
			getnameinfo((SA*)&clientaddr,clientlen,client_hostname,MAXLINE,client_port,MAXLINE,0);
			echo(connfd);
			close(connfd);
		}
	}
	exit(0);
}
void command(void){
	char buf[MAXLINE];
	if(!fgets(buf,MAXLINE,stdin)){
		exit(0);	//EOF
	}
	printf("%s",buf);
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
