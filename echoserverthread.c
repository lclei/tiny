#include "mynet.h"
int open_listenfd(char *port);
void *thread(void *vargp);
void echo(int connfd);

int main(int argc, char **argv)
{
	int listenfd, *connfdp, cno=1;
	socklen_t clientlen;
	pthread_t tid;
	struct sockaddr_storage clientaddr;
	char client_hostname[MAXLINE], client_port[MAXLINE];

	if(argc!=2){
	  fprintf(stderr,"usage: %s <port>\n",argv[0]);
	  exit(0);
	}
	listenfd = open_listenfd(argv[1]);
	while(1){
		clientlen = sizeof(struct sockaddr_storage);
		connfdp = malloc(sizeof(int));
		*connfdp = accept(listenfd, (SA*)&clientaddr,&clientlen);
        getnameinfo((SA*)&clientaddr,clientlen,client_hostname,MAXLINE,client_port,MAXLINE,0);
		pthread_create(&tid,NULL,thread,connfdp);
		printf("name[%s] port[%s]connected! Thread id:%ld will serve it!\n",
					client_hostname,client_port,(unsigned long long)tid);
	}
	exit(0);
}

void *thread(void* vargp){
	int connfd = *((int *)vargp);
	pthread_detach(pthread_self());
	free(vargp);
	echo(connfd);
	close(connfd);
	return NULL;
}

void echo(int connfd)
{
	size_t n;
	char buf[MAXLINE];

	while((n=read(connfd,buf,MAXLINE))!=0){
		printf("server recived %d bytes from fd %d\n",(int)n,connfd);
		write(connfd,buf,n);
	}
}
