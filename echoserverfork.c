#include "mynet.h"
int open_listenfd(char *port);
void echo(int connfd);
void sigchld_handler(int sig)
{
	while(waitpid(-1,0,WNOHANG) >0)
	  ;
	return;
}

int main(int argc, char **argv)
{
	int listenfd, connfd, cno=1;
	socklen_t clientlen;
	struct sockaddr_storage clientaddr;
	char client_hostname[MAXLINE], client_port[MAXLINE];

	if(argc!=2){
	  fprintf(stderr,"usage: %s <port>\n",argv[0]);
	  exit(0);
	}
	signal(SIGCHLD, sigchld_handler);
	listenfd = open_listenfd(argv[1]);
	while(1){
		printf("Parent: waitting...\n");
		clientlen = sizeof(struct sockaddr_storage);
		connfd = accept(listenfd, (SA*)&clientaddr,&clientlen);
        getnameinfo((SA*)&clientaddr,clientlen,client_hostname,MAXLINE,client_port,MAXLINE,0);
		printf("name[%s] port[%s]connected! Child No.%d will handle it!\n",
					client_hostname,client_port,cno++);
		if(fork()==0) //child
		{
			close(listenfd);
			echo(connfd);
			close(connfd);
			exit(0);
		}
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
